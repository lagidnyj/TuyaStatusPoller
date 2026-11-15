#pragma once

#include <stdint.h>

namespace secrets {

constexpr char wifiSsid[] = "YOUR_WIFI_SSID";
constexpr char wifiPassword[] = "YOUR_WIFI_PASSWORD";

constexpr uint8_t device1Ip[4] = {192, 168, 1, 101};
constexpr char device1LocalKey[] = "0123456789abcdef";

constexpr uint8_t device2Ip[4] = {192, 168, 1, 102};
constexpr char device2LocalKey[] = "0123456789abcdef";

}  // namespace secrets
