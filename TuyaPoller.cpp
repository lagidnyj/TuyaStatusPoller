#define LYNKTUYA_DEBUG 0

#include "TuyaPoller.h"

#include <LynkTuya.h>

#include "DeviceConfig.h"
#include "JsonOutput.h"

namespace {

constexpr uint32_t kDeviceResponseTimeoutMs = 5000;

template <TuyaProtocolVersion Version>
bool requestStatus(const TuyaDeviceConfig& config, String& json) {
  LynkTuyaDevice<Version> device(config.ip, config.localKey);
  device.setTimeout(kDeviceResponseTimeoutMs);
  return device.getStatus(json);
}

}  // namespace

void pollAllDevices() {
  for (size_t i = 0; i < kTuyaDeviceCount; ++i) {
    const TuyaDeviceConfig& config = kTuyaDevices[i];
    if (!config.enabled) {
      continue;
    }

    String json;
    bool succeeded = false;

    switch (config.protocol) {
      case TUYA_V34:
        succeeded = requestStatus<TUYA_V34>(config, json);
        break;
      case TUYA_V35:
        succeeded = requestStatus<TUYA_V35>(config, json);
        break;
      default:
        emitStatusError(config);
        continue;
    }

    if (succeeded && json.length() > 0) {
      emitStatusJson(config, json);
    } else {
      emitStatusError(config);
    }
  }
}
