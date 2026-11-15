#pragma once

#include <Arduino.h>

struct TuyaDeviceConfig;

void emitStatusJson(const TuyaDeviceConfig& device, const String& statusJson);
void emitStatusError(const TuyaDeviceConfig& device);
