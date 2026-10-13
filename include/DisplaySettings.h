#pragma once

#include <stdint.h>

struct DisplaySettingsData {
  bool nightModeEnabled;
  uint8_t nightStartHour;
  uint8_t nightEndHour;
  uint8_t dayBrightnessPercent;
  uint8_t indicatorBrightness;
  bool forecastModeEnabled;
  uint8_t forecastHours;
};

class DisplaySettings {
 public:
  DisplaySettings();
  void begin();
  DisplaySettingsData get() const;
  bool set(const DisplaySettingsData& settings);
  bool isNightHour(uint8_t hour) const;

 private:
  DisplaySettingsData _settings;
};
