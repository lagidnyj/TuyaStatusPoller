#include "DpsFormatter.h"

#include <string.h>

namespace {

struct DpsField {
  const char* id;
  const char* name;
  uint8_t scale;
};

struct DpsSchema {
  const DpsField* fields;
  size_t fieldCount;
};

const DpsField kAtorchCw24Fields[] = {
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

const DpsField kAtorchGr2pwFields[] = {
    {"1", "switch_enabled", 0},
    {"9", "countdown_seconds", 0},
    {"17", "added_energy_kilowatt_hours", 2},
    {"18", "current_amps", 3},
    {"19", "active_power_watts", 2},
    {"20", "voltage_volts", 2},
    {"101", "electricity_price", 3},
    {"102", "total_electricity_cost", 3},
    {"103", "added_electricity_cost", 2},
    {"104", "overvoltage_threshold_volts", 1},
    {"105", "overcurrent_threshold_amps", 1},
    {"106", "overpower_threshold_watts", 0},
    {"107", "language", 0},
    {"108", "working_screen_brightness", 0},
    {"109", "standby_screen_brightness", 0},
    {"110", "standby_timeout_seconds", 0},
    {"111", "beep_enabled", 0},
    {"112", "switch_mode", 0},
    {"113", "reset_accumulated_data", 0},
    {"114", "reset_wifi", 0},
    {"115", "restore_factory_settings", 0},
    {"116", "screen_rotation_enabled", 0},
    {"117", "standby_screen", 0},
    {"118", "display_style", 0},
    {"119", "undervoltage_threshold_volts", 1},
    {"120", "over_limit_control_enabled", 0},
    {"121", "leakage_current_threshold_amps", 3},
    {"122", "leakage_energy_kilowatt_hours", 3},
    {"123", "total_energy_kilowatt_hours", 3},
    {"124", "leakage_current_amps", 3},
    {"125", "measurement_refresh_interval_seconds", 0},
    {"126", "live_refresh_enabled", 0},
    {"132", "warning", 0},
    {"133", "frequency_hertz", 2},
    {"134", "power_factor", 2},
    {"135", "device_cpu_temperature_celsius", 0},
    {"136", "electricity_price_mode", 0},
    {"137", "overvoltage_recovery_delay_minutes", 0},
    {"138", "power_on_state", 0},
    {"139", "prepayment_enabled", 0},
    {"140", "remaining_energy_kilowatt_hours", 2},
    {"141", "clear_remaining_energy", 0},
    {"142", "energy_recharge_kilowatt_hours", 2},
    {"143", "low_energy_alarm_threshold_kilowatt_hours", 0},
};

const DpsField kGoE31_16AFields[] = {
    {"1", "forward_energy_total", 2},
    {"3", "work_state", 0},
    {"4", "charge_cur_set", 0},
    {"6", "phase_a", 0},
    {"7", "phase_b", 0},
    {"8", "phase_c", 0},
    {"9", "power_total", 3},
    {"10", "fault", 0},
    {"13", "connection_state", 0},
    {"17", "energy_charge", 2},
    {"18", "switch", 0},
    {"19", "local_timer", 0},
    {"24", "temp_current", 0},
    {"25", "charge_energy_once", 2},
    {"28", "timer_on", 0},
    {"33", "mode_set", 0},
    {"101", "customize_scene", 0},
    {"102", "cur_voltage", 1},
    {"103", "a_current", 1},
    {"104", "voltb", 0},
    {"105", "cvolt", 0},
    {"106", "b_current", 1},
    {"107", "c_current", 1},
    {"108", "abc_power", 3},
    {"109", "volta", 0},
    {"110", "runtime", 0},
    {"111", "cook_time", 0},
    {"112", "use_time", 0},
    {"113", "timer_sync", 0},
    {"114", "countdown_left", 0},
    {"115", "target_time", 0},
    {"116", "chg_handle", 0},
    {"117", "appoint_time", 0},
    {"118", "state", 0},
    {"119", "swipe_enable", 0},
};

const DpsField kPc473Fields[] = {
    {"1", "switch_1", 0},
    {"26", "fault", 0},
    {"38", "relay_status", 0},
    {"101", "voltage_a", 1},
    {"102", "current_a", 3},
    {"103", "power_a", 3},
    {"104", "pf_a", 2},
    {"105", "forward_energy_a", 2},
    {"106", "reverse_energy_a", 2},
    {"107", "voltage_b", 1},
    {"108", "current_b", 3},
    {"109", "power_b", 3},
    {"110", "pf_b", 2},
    {"111", "forward_energy_b", 2},
    {"112", "reverse_energy_b", 2},
    {"113", "voltage_c", 1},
    {"114", "current_c", 3},
    {"115", "power_c", 3},
    {"116", "pf_c", 2},
    {"117", "forward_energy_c", 2},
    {"118", "reverse_energy_c", 2},
    {"119", "forward_energy_total", 2},
    {"120", "current_total", 3},
    {"121", "power_total", 3},
    {"122", "freq", 0},
    {"123", "dev_status", 0},
    {"124", "reverse_energy_total", 2},
    {"125", "ov_pwr_val_a", 3},
    {"126", "ov_pwr_val_b", 3},
    {"127", "ov_pwr_val_c", 3},
    {"128", "ov_pwr_en_a", 0},
    {"129", "ov_pwr_en_b", 0},
    {"130", "ov_pwr_en_c", 0},
    {"131", "countdown", 0},
};

const DpsField kPj1103Fields[] = {
    {"101", "power_a", 1},
    {"102", "direction_a", 0},
    {"103", "tbd", 0},
    {"104", "direction_b", 0},
    {"105", "power_b", 1},
    {"106", "forward_energy_a", 2},
    {"107", "reverse_energy_a", 2},
    {"108", "forward_energy_b", 2},
    {"109", "reverse_energy_b", 2},
    {"110", "power_factor_a", 2},
    {"111", "frequency_hz", 2},
    {"112", "voltage_a", 1},
    {"113", "current_a_milliamps", 0},
    {"114", "current_b_milliamps", 0},
    {"115", "total_power_watts", 1},
    {"116", "voltage_coef", 3},
    {"117", "current_a_calibration", 3},
    {"118", "power_a_calibration", 3},
    {"119", "energy_a_calibration_fwd", 3},
    {"120", "coef_a_reset", 0},
    {"121", "power_factor_b", 2},
    {"122", "freq_calibration", 3},
    {"123", "current_b_calibration", 3},
    {"124", "power_b_calibration", 3},
    {"125", "energy_b_calibration_fwd", 3},
    {"126", "coef_b_reset", 0},
    {"127", "energy_a_calibration_rev", 3},
    {"128", "energy_b_calibration_rev", 3},
    {"129", "report_rate_control_seconds", 0},
    {"130", "forward_energy_total", 2},
    {"131", "reverse_energy_total", 2},
    {"132", "buzzer_enabled", 0},
    {"133", "overvoltage_protection_enabled", 0},
    {"134", "undervoltage_protection_enabled", 0},
    {"135", "overcurrent_protection_enabled", 0},
    {"136", "overpower_protection_enabled", 0},
    {"137", "overvoltage_threshold_volts", 1},
    {"138", "undervoltage_threshold_volts", 1},
    {"139", "overcurrent_threshold_amps", 3},
    {"140", "overpower_threshold_watts", 1},
    {"141", "overvoltage_status", 0},
    {"142", "undervoltage_status", 0},
    {"143", "overcurrent_status", 0},
    {"144", "overpower_status", 0},
    {"145", "overcurrent_threshold_b_amps", 3},
    {"146", "overpower_threshold_b_watts", 1},
    {"147", "overcurrent_status_b", 0},
    {"148", "overpower_status_b", 0},
    {"149", "overcurrent_protection_enabled_b", 0},
    {"150", "overpower_protection_enabled_b", 0},
};

const DpsField kKws303wfCbuFields[] = {
    {"10", "fault_bitmap", 0},
    {"16", "switch_enabled", 0},
    {"101", "voltage_volts", 1},
    {"102", "current_amps", 3},
    {"103", "power_watts", 1},
    {"104", "run_time_minutes", 0},
    {"105", "energy_kilowatt_hours", 3},
    {"106", "power_factor", 2},
    {"107", "frequency_hertz", 0},
    {"108", "temperature_celsius", 0},
    {"109", "undervoltage_threshold_volts", 0},
    {"110", "overvoltage_threshold_volts", 0},
    {"111", "overcurrent_threshold_amps", 3},
    {"112", "poweroff_timer_minutes", 0},
    {"113", "overenergy_threshold_kilowatt_hours", 3},
    {"114", "overpower_threshold_kilowatts", 3},
    {"115", "overtemperature_threshold_celsius", 0},
    {"116", "meter_state", 0},
    {"117", "power_restore_mode", 0},
    {"118", "clear_energy", 0},
    {"119", "screen_off_minutes", 0},
    {"120", "clear_run_time", 0},
    {"121", "countdown_seconds", 0},
    {"122", "scheduled_switch_enabled", 0},
    {"123", "added_energy_kilowatt_hours", 3},
    {"124", "screen_orientation_reversed", 0},
    {"125", "relay_power_on_state", 0},
};

bool getDpsSchema(DpsFormatterId formatter, DpsSchema& schema) {
  switch (formatter) {
    case DpsFormatterId::AtorchCw24:
      schema.fields = kAtorchCw24Fields;
      schema.fieldCount =
          sizeof(kAtorchCw24Fields) / sizeof(kAtorchCw24Fields[0]);
      return true;
    case DpsFormatterId::AtorchGr2pw:
      schema.fields = kAtorchGr2pwFields;
      schema.fieldCount =
          sizeof(kAtorchGr2pwFields) / sizeof(kAtorchGr2pwFields[0]);
      return true;
    case DpsFormatterId::GoE31_16A:
      schema.fields = kGoE31_16AFields;
      schema.fieldCount =
          sizeof(kGoE31_16AFields) / sizeof(kGoE31_16AFields[0]);
      return true;
    case DpsFormatterId::Pc473:
      schema.fields = kPc473Fields;
      schema.fieldCount = sizeof(kPc473Fields) / sizeof(kPc473Fields[0]);
      return true;
    case DpsFormatterId::Pj1103:
      schema.fields = kPj1103Fields;
      schema.fieldCount = sizeof(kPj1103Fields) / sizeof(kPj1103Fields[0]);
      return true;
    case DpsFormatterId::Kws303wfCbu:
      schema.fields = kKws303wfCbuFields;
      schema.fieldCount =
          sizeof(kKws303wfCbuFields) / sizeof(kKws303wfCbuFields[0]);
      return true;
  }
  return false;
}

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

const DpsField* findDpsField(const String& json, size_t start, size_t end,
                             const DpsSchema& schema) {
  const size_t keyLength = end - start - 2;
  for (size_t i = 0; i < schema.fieldCount; ++i) {
    const char* id = schema.fields[i].id;
    if (strlen(id) == keyLength &&
        strncmp(json.c_str() + start + 1, id, keyLength) == 0) {
      return &schema.fields[i];
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

bool formatDpsObject(const String& raw, JsonReader& reader,
                     const DpsSchema& schema, String& formatted) {
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

    const DpsField* field = findDpsField(raw, keyStart, keyEnd, schema);
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

bool formatTuyaStatus(const String& rawStatus, DpsFormatterId formatter,
                      String& formattedStatus) {
  DpsSchema schema;
  if (!getDpsSchema(formatter, schema)) {
    return false;
  }

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
        if (foundDps ||
            !formatDpsObject(rawStatus, reader, schema, formattedDps)) {
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
