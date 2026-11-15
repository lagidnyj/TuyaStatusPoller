#pragma once

#include <Arduino.h>
#include <LynkTuya.h>

struct TuyaDeviceConfig {
  const char* name;
  IPAddress ip;
  const char* localKey;
  TuyaProtocolVersion protocol;
};

extern const TuyaDeviceConfig kTuyaDevices[];
extern const size_t kTuyaDeviceCount;
