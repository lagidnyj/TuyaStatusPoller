#pragma once

#include <Arduino.h>
#include <LynkTuya.h>

#include "DpsFormatter.h"

struct TuyaDeviceConfig {
  bool enabled;
  const char* name;
  IPAddress ip;
  const char* localKey;
  TuyaProtocolVersion protocol;
  DpsFormatterId formatter;
};

extern const TuyaDeviceConfig* const kTuyaDevices;
extern const size_t kTuyaDeviceCount;
