#include "DeviceConfig.h"

#include "secrets.h"

const TuyaDeviceConfig* const kTuyaDevices = secrets::devices;
const size_t kTuyaDeviceCount =
    sizeof(secrets::devices) / sizeof(secrets::devices[0]);
