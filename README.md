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
`"error":"cloud_upload_failed"` JSON line with the HTTP status code (or `0`
if the cloud settings are missing or the request could not be started).

При помилці запиту друкується JSON-рядок з `error`. Опитування пристроїв
виконується послідовно.

Щоб додати пристрій, додайте його IP і Local Key у `secrets.h`, а запис
пристрою — у масив `kTuyaDevices` у `DeviceConfig.cpp`. Вкажіть `TUYA_V34`
або `TUYA_V35` для відомої версії протоколу.
