#include "DeviceConfig.h"

#include "secrets.h"

namespace {

IPAddress makeIpAddress(const uint8_t (&octets)[4]) {
  return IPAddress(octets[0], octets[1], octets[2], octets[3]);
}

}  // namespace

const TuyaDeviceConfig kTuyaDevices[] = {
    {"tuya_1", makeIpAddress(secrets::device1Ip), secrets::device1LocalKey,
     TUYA_V35},
    {"tuya_2", makeIpAddress(secrets::device2Ip), secrets::device2LocalKey,
     TUYA_V35},
};

const size_t kTuyaDeviceCount =
    sizeof(kTuyaDevices) / sizeof(kTuyaDevices[0]);
