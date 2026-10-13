#include "DisplaySettings.h"

#include <Preferences.h>

#include "DeviceConfiguration.h"

DisplaySettings::DisplaySettings()
    : _settings{DeviceConfiguration::kNightModeDefaultEnabled,
                DeviceConfiguration::kNightModeDefaultStartHour,
                DeviceConfiguration::kNightModeDefaultEndHour,
                DeviceConfiguration::kDefaultDayBrightnessPercent,
                DeviceConfiguration::kDefaultIndicatorBrightness,
                DeviceConfiguration::kDefaultForecastMode,
                DeviceConfiguration::kDefaultForecastHours} {}

void DisplaySettings::begin() {
  Preferences preferences;
  if (!preferences.begin(DeviceConfiguration::kDisplayPreferencesNamespace, true)) {
    return;
  }

  const DisplaySettingsData stored{
      preferences.getBool(DeviceConfiguration::kNightModeEnabledPreferenceKey,
                          DeviceConfiguration::kNightModeDefaultEnabled),
      preferences.getUChar(DeviceConfiguration::kNightModeStartHourPreferenceKey,
                           DeviceConfiguration::kNightModeDefaultStartHour),
      preferences.getUChar(DeviceConfiguration::kNightModeEndHourPreferenceKey,
                           DeviceConfiguration::kNightModeDefaultEndHour),
      preferences.getUChar(DeviceConfiguration::kDayBrightnessPreferenceKey,
                           DeviceConfiguration::kDefaultDayBrightnessPercent),
      preferences.getUChar(DeviceConfiguration::kIndicatorBrightnessPreferenceKey,
                           DeviceConfiguration::kDefaultIndicatorBrightness),
      preferences.getBool(DeviceConfiguration::kForecastModePreferenceKey,
                          DeviceConfiguration::kDefaultForecastMode),
      preferences.getUChar(DeviceConfiguration::kForecastHoursPreferenceKey,
                           DeviceConfiguration::kDefaultForecastHours)};
  preferences.end();
  if (stored.nightStartHour < DeviceConfiguration::kHoursPerDay &&
      stored.nightEndHour < DeviceConfiguration::kHoursPerDay &&
      stored.nightStartHour != stored.nightEndHour &&
      stored.dayBrightnessPercent >= DeviceConfiguration::kMinimumDayBrightnessPercent &&
      stored.dayBrightnessPercent <= DeviceConfiguration::kMaximumDayBrightnessPercent &&
      stored.indicatorBrightness <= DeviceConfiguration::kMaximumIndicatorBrightness &&
      stored.forecastHours >= DeviceConfiguration::kForecastIntervalHours &&
      stored.forecastHours <= DeviceConfiguration::kMaximumForecastHours &&
      stored.forecastHours % DeviceConfiguration::kForecastIntervalHours == 0) {
    _settings = stored;
  }
}

DisplaySettingsData DisplaySettings::get() const {
  return _settings;
}

bool DisplaySettings::set(const DisplaySettingsData& settings) {
  if (settings.nightStartHour >= DeviceConfiguration::kHoursPerDay ||
      settings.nightEndHour >= DeviceConfiguration::kHoursPerDay ||
      settings.nightStartHour == settings.nightEndHour ||
      settings.dayBrightnessPercent < DeviceConfiguration::kMinimumDayBrightnessPercent ||
      settings.dayBrightnessPercent > DeviceConfiguration::kMaximumDayBrightnessPercent ||
      settings.indicatorBrightness > DeviceConfiguration::kMaximumIndicatorBrightness ||
      settings.forecastHours < DeviceConfiguration::kForecastIntervalHours ||
      settings.forecastHours > DeviceConfiguration::kMaximumForecastHours ||
      settings.forecastHours % DeviceConfiguration::kForecastIntervalHours != 0) {
    return false;
  }

  Preferences preferences;
  if (!preferences.begin(DeviceConfiguration::kDisplayPreferencesNamespace, false)) {
    return false;
  }

  const bool saved =
      preferences.putBool(DeviceConfiguration::kNightModeEnabledPreferenceKey,
                          settings.nightModeEnabled) > 0 &&
      preferences.putUChar(DeviceConfiguration::kNightModeStartHourPreferenceKey,
                           settings.nightStartHour) > 0 &&
      preferences.putUChar(DeviceConfiguration::kNightModeEndHourPreferenceKey,
                           settings.nightEndHour) > 0 &&
      preferences.putUChar(DeviceConfiguration::kDayBrightnessPreferenceKey,
                           settings.dayBrightnessPercent) > 0 &&
      preferences.putUChar(DeviceConfiguration::kIndicatorBrightnessPreferenceKey,
                           settings.indicatorBrightness) > 0 &&
      preferences.putBool(DeviceConfiguration::kForecastModePreferenceKey,
                          settings.forecastModeEnabled) > 0 &&
      preferences.putUChar(DeviceConfiguration::kForecastHoursPreferenceKey,
                           settings.forecastHours) > 0;
  preferences.end();
  if (saved) {
    _settings = settings;
  }
  return saved;
}

bool DisplaySettings::isNightHour(uint8_t hour) const {
  if (!_settings.nightModeEnabled || hour >= DeviceConfiguration::kHoursPerDay) {
    return false;
  }

  if (_settings.nightStartHour > _settings.nightEndHour) {
    return hour >= _settings.nightStartHour || hour < _settings.nightEndHour;
  }
  return hour >= _settings.nightStartHour && hour < _settings.nightEndHour;
}
