# TuyaStatusPoller

ESP32 poller for the CW24 power meter (Atorch).

Періодично читає статус Tuya-пристроїв у локальній мережі через
LynkTuyaLocal і виводить по одному JSON-об'єкту на рядок у Serial Monitor.
Команди керування пристроями не викликаються.

## Підготовка Arduino IDE

1. Встановіть підтримку плат ESP32 від Espressif Systems.
2. Встановіть бібліотеку LynkTuyaLocal з
   [репозиторію](https://github.com/F1chter/LynkTuyaLocal) через ZIP-архів
   (`Sketch > Include Library > Add .ZIP Library`).
3. Відкрийте `TuyaStatusPoller.ino` у папці `TuyaStatusPoller`.
4. Виберіть плату ESP32-C3, наприклад `ESP32C3 Dev Module`.
5. Заповніть локальний `secrets.h`: Wi-Fi, IP-адреси та 16-символьні Local
   Key. Цей файл виключений з Git; не додавайте реальні ключі в репозиторій.
6. Заповніть локальний `cloud_secrets.h` за шаблоном
   `cloud_secrets.example.h`: URL `backend/api.php`, ID пристрою та його
   індивідуальний `device_token`, створений у Micro-IoT-Cloud.
7. Відкрийте Serial Monitor зі швидкістю 115200 baud.

`secrets.example.h` містить шаблон. Якщо `secrets.h` відсутній, скопіюйте
його з шаблону та замініть тестові значення.
`cloud_secrets.h` також виключений з Git; не додавайте реальний токен до
репозиторію.

LynkTuya.h - треба змінити буфер на 512

## Модулі

- `TuyaStatusPoller.ino` — Wi-Fi та запуск опитування кожні 30 секунд.
- `DeviceConfig.h/.cpp` — список пристроїв і параметри протоколу.
- `TuyaPoller.h/.cpp` — запит статусу v3.4/v3.5 через LynkTuyaLocal.
- `JsonOutput.h/.cpp` — JSON Lines-вивід зі статусом або помилкою.
- `CloudUploader.h/.cpp` — POST стану до Micro-IoT-Cloud.
- `secrets.h` — локальні Wi-Fi та Tuya-облікові дані.
- `cloud_secrets.h` — URL, ID і токен пристрою для хмари.

Приклад успішного рядка:

```json
{"device":"tuya_1","ip":"192.168.1.101","status":{"dps":{"relay_enabled":true}}}
```

Known DPS IDs are emitted with descriptive English names, and scaled numeric
values are converted to their displayed units (for example, raw `42000` for
the overvoltage threshold becomes `420`). Unknown DPS IDs are preserved
unchanged. A malformed or unsupported status payload produces
`"error":"status_format_failed"`.

The same complete JSON object printed to Serial is posted to
`backend/api.php?device=<cloud-device-id>` with the device-specific token in
the `Authorization: Bearer` header. The current test configuration disables
TLS certificate verification; use it only temporarily on a trusted network.
Configure certificate validation before using a real token in production.
An unsuccessful cloud POST produces a separate
`"error":"cloud_upload_failed"` JSON line with the HTTP status code and API
response body (or a local diagnostic if the cloud settings are missing or
the request could not be started).

При помилці запиту друкується JSON-рядок з `error`. Опитування пристроїв
виконується послідовно.

Щоб додати пристрій, додайте запис до масиву `devices` у `secrets.h`.
Для кожного пристрою вкажіть `true` або `false` (опитувати чи пропускати),
ім'я, IP, Local Key і версію протоколу (`TUYA_V34` або `TUYA_V35`).
Також вкажіть форматер пристрою: для ATORCH CW24 це
`DpsFormatterId::AtorchCw24`. Пристрої опитуються послідовно в порядку
записів у масиві.

`AtorchCw24` у `DpsFormatter.cpp` є прикладом профілю: його таблиця
зіставляє DPS ID з назвами полів і масштабами значень. Для нової моделі
додайте окремий `DpsFormatterId` і таблицю DPS, потім підключіть її в
`getDpsSchema`. Спільний JSON-парсер використовуватиметься для всіх профілів.

Профіль `DpsFormatterId::AtorchGr2pw` відповідає ATORCH GR2PW (Things Data
Model `e1kyw9as`). Він зіставляє DPS ID з властивостями моделі та масштабує
числа за `typeSpec.scale`; наприклад, `20: 22889` стає `"voltage_volts":
228.89`, а `133: 5000` — `"frequency_hertz": 50`.

Профіль `DpsFormatterId::GoE31_16A` відповідає GO-E31-16A (Things Data
Model `e1k5nvko`) і масштабує числові значення за `typeSpec.scale`. DPS
`phase_a`, `phase_b` і `phase_c` містять упаковані дані у форматі raw; вони
поки лишаються без розпакування, доки не буде зразка їхнього значення в
локальному статусі. Опис фази A суперечливий: байти `00 03 E8` означають
1 A за вказаного масштабу 0.001 A, хоча текст прикладу називає це 10 A.

Профіль `DpsFormatterId::Pc473` зіставляє DPS ID з `code` із Things Data
Model PC473 та масштабує числові значення за `typeSpec.scale`. Наприклад,
`101: 2118` перетворюється на `"voltage_a": 211.8`. Bitmap-поля `fault` і
`dev_status` залишаються без масштабування; невідомі DPS ID зберігаються
без змін. Для `power_total` використовується масштаб 3 замість указаного в
моделі масштабу 2: зразок значень узгоджується з фазними потужностями
(`0.051 + 0.003 = 0.054 kW`).

Профіль `DpsFormatterId::Pj1103` відповідає PJ-1103 (Things Data Model
`e1kcg9c4`). Він масштабує значення за схемою моделі; струм каналів
залишається в міліамперах, як указано в моделі (`current_a_milliamps`).
Приклад конфігурації пристрою наведено в `secrets.example.h`.

Профіль `DpsFormatterId::Kws303wfCbu` відповідає KWS-303WF CBU (Things Data
Model `e1ktgaz8`). Числові DPS масштабуються за `typeSpec.scale`; bitmap,
перемикачі й enum-значення зберігаються без перетворення. Знімок властивостей
із Tuya підтверджує всі 27 DPS ID і коди; наприклад, `voltage: 2270`
перетворюється на `voltage_volts: 227.0`, а `energy: 3` — на
`energy_kilowatt_hours: 0.003`. Це хмарний знімок, не локальний `getStatus`
пакет, тож локальне форматування ще не перевірене.
