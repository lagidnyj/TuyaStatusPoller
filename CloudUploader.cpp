#include "CloudUploader.h"

#include <HTTPClient.h>
#include <WiFiClientSecure.h>

#include "cloud_secrets.h"

int uploadCloudPayload(const String& payload) {
  if (cloud_secrets::apiUrl[0] == '\0' ||
      cloud_secrets::deviceId[0] == '\0' ||
      cloud_secrets::deviceToken[0] == '\0') {
    return 0;
  }

  WiFiClientSecure client;
  // Test-only TLS mode; verify the server certificate before using this in production.
  client.setInsecure();

  HTTPClient http;
  http.setTimeout(10000);

  String url = cloud_secrets::apiUrl;
  url += "?device=";
  url += cloud_secrets::deviceId;
  if (!http.begin(client, url)) {
    return 0;
  }

  http.addHeader("Content-Type", "application/json");
  String authorization = "Bearer ";
  authorization += cloud_secrets::deviceToken;
  http.addHeader("Authorization", authorization);
  const int statusCode = http.POST(payload);
  http.end();
  return statusCode;
}
