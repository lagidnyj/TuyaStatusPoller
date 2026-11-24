#pragma once

#include <Arduino.h>

enum class DpsFormatterId : uint8_t {
  AtorchCw24,
  AtorchGr2pw,
  GoE31_16A,
  Pc473,
  Pj1103,
  Kws303wfCbu,
};

bool formatTuyaStatus(const String& rawStatus, DpsFormatterId formatter,
                      String& formattedStatus);
