#include "DpsFormatter.h"

#include <string.h>

namespace {

struct DpsField {
  const char* id;
  const char* name;
  uint8_t scale;
};

const DpsField kDpsFields[] = {
    {"18", "current_amps", 3},
    {"19", "active_power_watts", 2},
    {"20", "voltage_volts", 2},
    {"101", "live_refresh_enabled", 0},
    {"102", "relay_enabled", 0},
    {"103", "battery_percentage", 1},
    {"104", "overvoltage_warning_threshold_volts", 2},
    {"105", "probe_overtemperature_threshold_celsius", 1},
    {"106", "overpower_warning_threshold_watts", 0},
    {"107", "language", 0},
    {"108", "working_screen_brightness", 0},
    {"109", "standby_screen_brightness", 0},
    {"110", "standby_timeout_seconds", 0},
    {"111", "discharge_current_amps", 3},
    {"112", "discharge_power_watts", 3},
    {"113", "reset_accumulated_data", 0},
    {"114", "reset_wifi", 0},
    {"115", "restore_factory_settings", 0},
    {"116", "zero_no_load_current", 0},
    {"117", "battery_full_voltage_volts", 2},
    {"118", "display_style", 0},
    {"119", "low_voltage_warning_threshold_volts", 2},
    {"120", "battery_empty_voltage_volts", 2},
    {"121", "minimum_measurement_current_milliamps", 0},
    {"122", "external_probe_temperature_celsius", 1},
    {"123", "total_energy_kilowatt_hours", 3},
    {"124", "capacity_measurement_mode", 0},
    {"125", "measurement_refresh_interval_seconds", 0},
    {"126", "charging_capacity_amp_hours", 3},
    {"127", "discharging_capacity_amp_hours", 3},
    {"128", "accumulated_capacity_amp_hours", 3},
    {"129", "accumulated_energy_kilowatt_hours", 3},
    {"132", "warning", 0},
    {"133", "measured_capacity_amp_hours", 3},
    {"134", "resistance_ohms", 2},
    {"135", "device_cpu_temperature_celsius", 0},
    {"136", "shunt_rating", 0},
};

class JsonReader {
 public:
  explicit JsonReader(const String& json)
      : data_(json.c_str()), length_(json.length()), position_(0) {}

  void skipWhitespace() {
    while (position_ < length_ &&
           (data_[position_] == ' ' || data_[position_] == '\t' ||
            data_[position_] == '\n' || data_[position_] == '\r')) {
      ++position_;
    }
  }

  bool consume(char expected) {
    skipWhitespace();
    if (position_ >= length_ || data_[position_] != expected) {
      return false;
    }
    ++position_;
    return true;
  }

  bool readString(size_t& start, size_t& end) {
    skipWhitespace();
    if (position_ >= length_ || data_[position_] != '"') {
      return false;
    }

    start = position_++;
    while (position_ < length_) {
      const unsigned char c = data_[position_++];
      if (c == '"') {
        end = position_;
        return true;
      }
      if (c < 0x20) {
        return false;
      }
      if (c == '\\') {
        if (position_ >= length_) {
          return false;
        }
        const char escaped = data_[position_++];
        if (escaped == 'u') {
          for (uint8_t i = 0; i < 4; ++i) {
            if (position_ >= length_ || !isHexDigit(data_[position_++])) {
              return false;
            }
          }
        } else if (escaped != '"' && escaped != '\\' && escaped != '/' &&
                   escaped != 'b' && escaped != 'f' && escaped != 'n' &&
                   escaped != 'r' && escaped != 't') {
          return false;
        }
      }
    }
    return false;
  }

  bool readScalar(size_t& start, size_t& end) {
    skipWhitespace();
    start = position_;
    while (position_ < length_ && data_[position_] != ',' &&
           data_[position_] != '}' && data_[position_] != ']' &&
           data_[position_] != ' ' && data_[position_] != '\t' &&
           data_[position_] != '\n' && data_[position_] != '\r') {
      if (data_[position_] == '{' || data_[position_] == '[' ||
          data_[position_] == '"') {
        return false;
      }
      ++position_;
    }
    end = position_;
    return end > start;
  }

  bool skipValue(uint8_t depth = 0) {
    if (depth > 8) {
      return false;
    }
    skipWhitespace();
    if (position_ >= length_) {
      return false;
    }

    if (data_[position_] == '"') {
      size_t start;
      size_t end;
      return readString(start, end);
    }
    if (data_[position_] == '{') {
      ++position_;
      skipWhitespace();
      if (consume('}')) {
        return true;
      }
      do {
        size_t keyStart;
        size_t keyEnd;
        if (!readString(keyStart, keyEnd) || !consume(':') ||
            !skipValue(depth + 1)) {
          return false;
        }
        skipWhitespace();
        if (consume('}')) {
          return true;
        }
      } while (consume(','));
      return false;
    }
    if (data_[position_] == '[') {
      ++position_;
      skipWhitespace();
      if (consume(']')) {
        return true;
      }
      do {
        if (!skipValue(depth + 1)) {
          return false;
        }
        skipWhitespace();
        if (consume(']')) {
          return true;
        }
      } while (consume(','));
      return false;
    }

    size_t start;
    size_t end;
    return readScalar(start, end);
  }

  bool atEnd() {
    skipWhitespace();
    return position_ == length_;
  }

  bool isStringNext() {
    skipWhitespace();
    return position_ < length_ && data_[position_] == '"';
  }

  bool isContainerNext() {
    skipWhitespace();
    return position_ < length_ &&
           (data_[position_] == '{' || data_[position_] == '[');
  }

  bool isHexDigit(char c) const {
    return (c >= '0' && c <= '9') || (c >= 'a' && c <= 'f') ||
           (c >= 'A' && c <= 'F');
  }

 private:
  const char* data_;
  size_t length_;
  size_t position_;
};

const DpsField* findDpsField(const String& json, size_t start, size_t end) {
  const size_t keyLength = end - start - 2;
  for (size_t i = 0; i < sizeof(kDpsFields) / sizeof(kDpsFields[0]); ++i) {
    const char* id = kDpsFields[i].id;
    if (strlen(id) == keyLength &&
        strncmp(json.c_str() + start + 1, id, keyLength) == 0) {
      return &kDpsFields[i];
    }
  }
  return nullptr;
}

bool scaleInteger(const String& raw, uint8_t scale, String& result) {
  bool negative = false;
  size_t firstDigit = 0;
  if (raw.length() > 0 && raw[0] == '-') {
    negative = true;
    firstDigit = 1;
  }
  if (firstDigit == raw.length()) {
    return false;
  }
  for (size_t i = firstDigit; i < raw.length(); ++i) {
    if (raw[i] < '0' || raw[i] > '9') {
      return false;
    }
  }
  if (scale == 0) {
    result = raw;
    return true;
  }

  String digits = raw.substring(firstDigit);
  while (digits.length() <= scale) {
    digits = "0" + digits;
  }
  const size_t decimalPosition = digits.length() - scale;
  String fraction = digits.substring(decimalPosition);
  while (fraction.length() > 0 && fraction[fraction.length() - 1] == '0') {
    fraction.remove(fraction.length() - 1);
  }

  result = negative ? "-" : "";
  result += digits.substring(0, decimalPosition);
  if (fraction.length() > 0) {
    result += ".";
    result += fraction;
  }
  return true;
}

bool formatDpsObject(const String& raw, JsonReader& reader, String& formatted) {
  if (!reader.consume('{')) {
    return false;
  }
  formatted = "{";
  reader.skipWhitespace();
  if (reader.consume('}')) {
    formatted += "}";
    return true;
  }

  bool first = true;
  while (true) {
    size_t keyStart;
    size_t keyEnd;
    if (!reader.readString(keyStart, keyEnd) || !reader.consume(':')) {
      return false;
    }

    const DpsField* field = findDpsField(raw, keyStart, keyEnd);
    if (!first) {
      formatted += ",";
    }
    first = false;
    if (field) {
      formatted += "\"";
      formatted += field->name;
      formatted += "\"";
    } else {
      formatted += raw.substring(keyStart, keyEnd);
    }
    formatted += ":";

    if (reader.isStringNext()) {
      size_t valueStart;
      size_t valueEnd;
      if (!reader.readString(valueStart, valueEnd)) {
        return false;
      }
      formatted += raw.substring(valueStart, valueEnd);
    } else {
      size_t valueStart;
      size_t valueEnd;
      if (reader.isContainerNext() ||
          !reader.readScalar(valueStart, valueEnd)) {
        return false;
      }
      const String value = raw.substring(valueStart, valueEnd);
      if (field && field->scale > 0) {
        String scaledValue;
        if (!scaleInteger(value, field->scale, scaledValue)) {
          return false;
        }
        formatted += scaledValue;
      } else {
        formatted += value;
      }
    }

    reader.skipWhitespace();
    if (reader.consume('}')) {
      formatted += "}";
      return true;
    }
    if (!reader.consume(',')) {
      return false;
    }
  }
}

bool keyIsDps(const String& raw, size_t start, size_t end) {
  return end - start == 5 && strncmp(raw.c_str() + start, "\"dps\"", 5) == 0;
}

}  // namespace

bool formatTuyaStatus(const String& rawStatus, String& formattedStatus) {
  JsonReader reader(rawStatus);
  if (!reader.consume('{')) {
    return false;
  }

  String formattedDps;
  bool foundDps = false;
  reader.skipWhitespace();
  if (!reader.consume('}')) {
    while (true) {
      size_t keyStart;
      size_t keyEnd;
      if (!reader.readString(keyStart, keyEnd) || !reader.consume(':')) {
        return false;
      }
      if (keyIsDps(rawStatus, keyStart, keyEnd)) {
        if (foundDps || !formatDpsObject(rawStatus, reader, formattedDps)) {
          return false;
        }
        foundDps = true;
      } else if (!reader.skipValue()) {
        return false;
      }

      reader.skipWhitespace();
      if (reader.consume('}')) {
        break;
      }
      if (!reader.consume(',')) {
        return false;
      }
    }
  }
  if (!foundDps || !reader.atEnd()) {
    return false;
  }

  formattedStatus = "{\"dps\":";
  formattedStatus += formattedDps;
  formattedStatus += "}";
  return true;
}
