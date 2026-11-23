#!/usr/bin/env python3
import json
import re
import socket
from concurrent.futures import ThreadPoolExecutor

import tinytuya

DEVICES_FILE = "tuya_devices.json"   # ваш JSON з хмари
IPS_FILE = "ips.txt"            # вставте таблицю з роутера як є
VERSIONS = [3.3, 3.4, 3.5, 3.1]
TIMEOUT = 3


def collect(node, out):
    if isinstance(node, dict):
        if "id" in node and ("localKey" in node or "key" in node):
            out.append(node)
        for v in node.values():
            collect(v, out)
    elif isinstance(node, list):
        for v in node:
            collect(v, out)


def load_devices(path=DEVICES_FILE):
    raw = []
    collect(json.load(open(path, encoding="utf-8")), raw)
    devs = []
    for d in raw:
        devs.append({
            "name": d.get("customName") or d.get("name") or d["id"],
            "id": d["id"],
            "key": d.get("localKey") or d.get("key"),
            "sub": bool(d.get("sub")),
            "cloud_online": d.get("isOnline"),
        })
    return devs


def load_ips(path=IPS_FILE):
    text = open(path, encoding="utf-8").read()
    return sorted(set(re.findall(r"\b(?:\d{1,3}\.){3}\d{1,3}\b", text)))


def port_open(ip, port=6668, t=1.0):
    try:
        with socket.create_connection((ip, port), timeout=t):
            return True
    except OSError:
        return False


def query(dev, ip, versions):
    for ver in versions:
        try:
            d = tinytuya.Device(dev["id"], ip, dev["key"], version=ver)
            d.set_socketTimeout(TIMEOUT)
            d.set_socketRetryLimit(1)
            d.set_socketPersistent(False)
            r = d.status()
        except Exception:
            continue
        if r and "dps" in r:
            return {"name": dev["name"], "id": dev["id"], "ip": ip,
                    "version": ver, "status": r}
    return None


def main():
    devices = [d for d in load_devices() if not d["sub"]]
    subs = [d for d in load_devices() if d["sub"]]
    ips = load_ips() if __import__("os").path.exists(IPS_FILE) else []
    print(f"Пристроїв: {len(devices)}, підпристроїв (пропущено): {len(subs)}, IP: {len(ips)}")

    # 1) UDP-скан
    print("Скан мережі (~20 с)...")
    found = {}
    try:
        scan = tinytuya.deviceScan(verbose=False, color=False)
        for ip, info in scan.items():
            found[info.get("gwId")] = (info.get("ip", ip), float(info.get("version") or 0))
    except Exception as e:
        print("Скан не вдався:", e)

    results = {}
    for dev in devices:
        if dev["id"] in found:
            ip, ver = found[dev["id"]]
            r = query(dev, ip, [ver] if ver else VERSIONS)
            if r:
                results[dev["id"]] = r
                print(f"[OK scan]  {dev['name']} -> {ip} v{r['version']}")

    # 2) Перебір для решти
    rest = [d for d in devices if d["id"] not in results]
    if rest and ips:
        print(f"Перебір для {len(rest)} пристроїв...")
        with ThreadPoolExecutor(32) as p:
            candidates = [ip for ip, ok in zip(ips, p.map(port_open, ips)) if ok]
        taken = {r["ip"] for r in results.values()}
        candidates = [ip for ip in candidates if ip not in taken]
        print("IP з відкритим 6668:", candidates)

        def job(a):
            return a[0]["id"], query(a[0], a[1], VERSIONS)

        pairs = [(d, ip) for d in rest for ip in candidates]
        with ThreadPoolExecutor(16) as p:
            for did, r in p.map(job, pairs):
                if r and did not in results:
                    results[did] = r
                    print(f"[OK brute] {r['name']} -> {r['ip']} v{r['version']}")

    for dev in devices:
        if dev["id"] not in results:
            print(f"[FAIL] {dev['name']} (хмара онлайн: {dev['cloud_online']})")
            results[dev["id"]] = {"name": dev["name"], "id": dev["id"],
                                  "cloud_online": dev["cloud_online"],
                                  "error": "no response"}
    for dev in subs:
        results[dev["id"]] = {"name": dev["name"], "id": dev["id"],
                              "error": "sub-device, needs gateway"}

    with open("results.json", "w", encoding="utf-8") as f:
        json.dump(list(results.values()), f, ensure_ascii=False, indent=2)
    print("Збережено: results.json")


if __name__ == "__main__":
    main()
