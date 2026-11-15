#include <WiFi.h>

#include "TuyaPoller.h"
#include "secrets.h"

namespace {

constexpr uint32_t kPollIntervalMs = 30000;
constexpr uint32_t kWifiRetryIntervalMs = 5000;

uint32_t lastPollMs = 0;
uint32_t lastWifiRetryMs = 0;
bool pollImmediately = true;

}  // namespace

void setup() {
  Serial.begin(115200);

  WiFi.mode(WIFI_STA);
  WiFi.setSleep(false);
  WiFi.begin(secrets::wifiSsid, secrets::wifiPassword);
}

void loop() {
  const uint32_t now = millis();

  if (WiFi.status() != WL_CONNECTED) {
    if (now - lastWifiRetryMs >= kWifiRetryIntervalMs) {
      lastWifiRetryMs = now;
      WiFi.reconnect();
    }
    delay(10);
    return;
  }

  if (pollImmediately || now - lastPollMs >= kPollIntervalMs) {
    pollImmediately = false;
    lastPollMs = now;
    pollAllDevices();
  }

  delay(10);
}
