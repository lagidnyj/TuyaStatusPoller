#include "JsonOutput.h"

#include "CloudUploader.h"
#include "DeviceConfig.h"
#include "DpsFormatter.h"

namespace {

void appendJsonString(String& output, const char* value) {
  while (*value != '\0') {
    const uint8_t c = static_cast<uint8_t>(*value++);
    switch (c) {
      case '"':
        output += "\\\"";
        break;
      case '\\':
        output += "\\\\";
        break;
      case '\b':
        output += "\\b";
        break;
      case '\f':
        output += "\\f";
        break;
      case '\n':
        output += "\\n";
        break;
      case '\r':
        output += "\\r";
        break;
      case '\t':
        output += "\\t";
        break;
      default:
        if (c < 0x20) {
          static const char hex[] = "0123456789abcdef";
          output += "\\u00";
          output += hex[c >> 4];
          output += hex[c & 0x0f];
        } else {
          output += static_cast<char>(c);
        }
        break;
    }
  }
}

String makeDevicePayload(const TuyaDeviceConfig& device,
                         const String& formattedStatus) {
  const String ip = device.ip.toString();
  String payload = "{\"device\":";
  appendJsonString(payload, device.name);
  payload += ",\"ip\":";
  appendJsonString(payload, ip.c_str());
  payload += ",\"status\":";
  payload += formattedStatus;
  payload += "}";
  return payload;
}

void emitDeviceError(const TuyaDeviceConfig& device, const char* error) {
  const String ip = device.ip.toString();
  String escapedName;
  String escapedIp;
  appendJsonString(escapedName, device.name);
  appendJsonString(escapedIp, ip.c_str());

  Serial.print("{\"device\":");
  Serial.print(escapedName);
  Serial.print(",\"ip\":");
  Serial.print(escapedIp);
  Serial.print(",\"error\":\"");
  Serial.print(error);
  Serial.println("\"}");
}

}  // namespace

void emitStatusJson(const TuyaDeviceConfig& device, const String& statusJson) {
  String formattedStatus;
  if (!formatTuyaStatus(statusJson, formattedStatus)) {
    emitDeviceError(device, "status_format_failed");
    return;
  }

  const String payload = makeDevicePayload(device, formattedStatus);
  Serial.println(payload);

  const int statusCode = uploadCloudPayload(payload);
  if (statusCode != 200) {
    Serial.print("{\"device\":");
    String escapedName;
    appendJsonString(escapedName, device.name);
    Serial.print(escapedName);
    Serial.print(",\"error\":\"cloud_upload_failed\",\"http_code\":");
    Serial.print(statusCode);
    Serial.println('}');
  }
}

void emitStatusError(const TuyaDeviceConfig& device) {
  emitDeviceError(device, "status_request_failed");
}
