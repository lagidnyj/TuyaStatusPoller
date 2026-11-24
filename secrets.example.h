#pragma once

#include "DeviceConfig.h"

namespace secrets {

constexpr char wifiSsid[] = "YOUR_WIFI_SSID";
constexpr char wifiPassword[] = "YOUR_WIFI_PASSWORD";

const TuyaDeviceConfig devices[] = {
    {true, "atorch_cw24", IPAddress(192, 168, 1, 101), "0123456789abcdef",
     TUYA_V35, DpsFormatterId::AtorchCw24},
    {false, "ATORCH GR2PW", IPAddress(192, 168, 1, 104),
     "0123456789abcdef", TUYA_V35, DpsFormatterId::AtorchGr2pw},
    {false, "PC473", IPAddress(192, 168, 1, 102), "0123456789abcdef",
     TUYA_V34, DpsFormatterId::Pc473},
    {false, "PJ-1103", IPAddress(192, 168, 1, 103), "0123456789abcdef",
     TUYA_V35, DpsFormatterId::Pj1103},
};

}  // namespace secrets
