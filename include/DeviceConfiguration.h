#pragma once

#include <stdint.h>
#include <time.h>
#include "MatrixPanelConfiguration.h"

namespace DeviceConfiguration {
extern const char* const kWifiSsid;
extern const char* const kWifiPassword;
extern const char* const kWeatherCity;
extern const char* const kWeatherApiKey;
extern const char* const kOtaNamePrefix;

constexpr uint32_t kSerialBaudRate = 9600;
constexpr uint32_t kWeatherRefreshIntervalMs = 30UL * 60UL * 1000UL;
constexpr uint32_t kNightIndicatorIntervalMs = 1000UL;
constexpr int kDayStartHour = 6;
constexpr int kNightStartHour = 18;
constexpr bool kNightModeDefaultEnabled = true;
constexpr uint8_t kNightModeDefaultStartHour = kNightStartHour;
constexpr uint8_t kNightModeDefaultEndHour = kDayStartHour;
constexpr uint16_t kControlCenterWebPort = 80;
constexpr uint32_t kPanelPreviewRefreshIntervalMs = 500UL;
constexpr const char* kControlCenterHostname = "esp32-ota";
constexpr const char* kDisplayPreferencesNamespace = "weather-client";
constexpr const char* kNightModeEnabledPreferenceKey = "nightEnabled";
constexpr const char* kNightModeStartHourPreferenceKey = "nightStart";
constexpr const char* kNightModeEndHourPreferenceKey = "nightEnd";
constexpr const char* kDayBrightnessPreferenceKey = "dayBright";
constexpr const char* kIndicatorBrightnessPreferenceKey = "indicatorBright";
constexpr const char* kForecastModePreferenceKey = "forecastMode";
constexpr const char* kForecastHoursPreferenceKey = "forecastHours";
constexpr uint8_t kHoursPerDay = 24;
constexpr uint8_t kDefaultDayBrightnessPercent =
	MatrixPanelConfiguration::kDefaultDisplayBrightnessPercent;
constexpr uint8_t kMinimumDayBrightnessPercent = 20;
constexpr uint8_t kMaximumDayBrightnessPercent =
	MatrixPanelConfiguration::kMaximumDisplayBrightnessPercent;
constexpr uint8_t kDefaultIndicatorBrightness = 3;
constexpr uint8_t kMaximumIndicatorBrightness = MatrixPanelConfiguration::kRgb444ChannelMask;
constexpr bool kDefaultForecastMode = false;
constexpr uint8_t kForecastIntervalHours = 3;
constexpr uint8_t kDefaultForecastHours = kForecastIntervalHours;
constexpr uint8_t kMaximumForecastHours = 120;
constexpr uint8_t kForecastResponseMaximumCount = 40;
constexpr uint8_t kForecastLookAheadPeriods = 3;
constexpr time_t kMinimumValidEpoch = 1700000000;
constexpr int16_t kNightIndicatorSize = 3;
constexpr int16_t kPanelWidth = MatrixPanelConfiguration::kWidth;
constexpr int16_t kPanelHeight = MatrixPanelConfiguration::kHeight;
constexpr int16_t kNightIndicatorX = kPanelWidth - kNightIndicatorSize;
constexpr int16_t kNightIndicatorY = kPanelHeight - kNightIndicatorSize;
constexpr int16_t kWeatherIconX = 16;
constexpr int16_t kWeatherIconY = 32;
constexpr int16_t kWeatherIconSize = 32;
constexpr uint8_t kTextSize = 1;
constexpr uint8_t kTextLineHeightPixels = 8;
constexpr uint8_t kWeatherHourLineIndex = 1;
constexpr uint8_t kWeatherNameLineIndex = 2;
constexpr uint8_t kMaximumTemperatureLineIndex = 3;
constexpr uint8_t kMinimumTemperatureLineIndex = 4;
constexpr bool kDisplayCelsius = true;
constexpr unsigned long kMainLoopDelayMs = 100UL;
constexpr const char* kNtpServerPrimary = "ntp.nict.jp";
constexpr const char* kNtpServerSecondary = "pool.ntp.org";
constexpr const char* kNtpServerTertiary = "time.google.com";
constexpr const char* kTimezone = "JST-9";
}
