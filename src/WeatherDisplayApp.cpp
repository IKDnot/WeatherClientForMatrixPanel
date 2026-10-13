#include "WeatherDisplayApp.h"

#include <Arduino.h>

#include "DeviceConfiguration.h"

WeatherDisplayApp::WeatherDisplayApp()
    : _panel(),
      _weather(DeviceConfiguration::kWifiSsid,
               DeviceConfiguration::kWifiPassword,
               DeviceConfiguration::kWeatherCity,
               DeviceConfiguration::kWeatherApiKey,
               DeviceConfiguration::kDisplayCelsius),
      _renderer(_panel),
      _clock(),
      _displaySettings(),
      _schedule(_clock, _panel, _displaySettings),
      _controlCenter(_displaySettings, _panel),
      _nightIndicator(_renderer),
      _weatherRefreshTimer(),
      _otaService(),
      _forceWeatherRefresh(false),
      _forecastSettingsInitialized(false),
      _lastForecastModeEnabled(DeviceConfiguration::kDefaultForecastMode),
      _lastForecastHours(DeviceConfiguration::kDefaultForecastHours),
      _weatherUpdateAttempted(false),
      _weatherAvailable(false),
      _lastWeatherUpdateSucceeded(false),
      _displayedForecast(false),
      _displayedForecastHours(0),
      _lastWeatherUpdateMillis(0) {}

void WeatherDisplayApp::begin() {
  Serial.begin(DeviceConfiguration::kSerialBaudRate);
  _panel.begin();
  _panel.setDisplayEnabled(false);
  _renderer.begin();
  _otaService.begin(DeviceConfiguration::kOtaNamePrefix,
                    DeviceConfiguration::kWifiSsid,
                    DeviceConfiguration::kWifiPassword);
  _displaySettings.begin();
  _controlCenter.begin();
  _clock.begin();
}

void WeatherDisplayApp::update() {
  _otaService.handle();
  _controlCenter.update();

  const uint32_t currentMillis = millis();
  const ScheduleUpdate scheduleUpdate = _schedule.update();
  const DisplaySettingsData displaySettings = _displaySettings.get();
  const bool nightMode = _schedule.isKnown() && !_schedule.isDaytime();
  _panel.setBrightness(nightMode ? MatrixPanelConfiguration::kMaximumDisplayBrightnessPercent
                                : displaySettings.dayBrightnessPercent);
  if (_forecastSettingsInitialized &&
      (displaySettings.forecastModeEnabled != _lastForecastModeEnabled ||
       displaySettings.forecastHours != _lastForecastHours)) {
    _forceWeatherRefresh = true;
  }
  _forecastSettingsInitialized = true;
  _lastForecastModeEnabled = displaySettings.forecastModeEnabled;
  _lastForecastHours = displaySettings.forecastHours;
  if (scheduleUpdate.becameDaytime) {
    _forceWeatherRefresh = true;
  }

  if (_weatherRefreshTimer.isDue(currentMillis, _forceWeatherRefresh)) {
    _forceWeatherRefresh = false;
    _weatherUpdateAttempted = true;
    _lastWeatherUpdateMillis = currentMillis;
    _lastWeatherUpdateSucceeded =
        _weather.update(displaySettings.forecastModeEnabled, displaySettings.forecastHours);
    if (_lastWeatherUpdateSucceeded) {
      _weatherAvailable = true;
      _displayedForecast = displaySettings.forecastModeEnabled;
      _displayedForecastHours = displaySettings.forecastModeEnabled
                                    ? displaySettings.forecastHours
                                    : 0;
      _renderer.renderWeather(_weather);
    } else {
      _weatherAvailable = false;
      _renderer.renderError();
    }
  }

  _nightIndicator.update(nightMode, scheduleUpdate.changed, currentMillis,
                         displaySettings.indicatorBrightness);
    WeatherPanelRuntimeStatus runtimeStatus = {};
    runtimeStatus.timeSynchronized = _schedule.isKnown();
    runtimeStatus.nightModeActive = nightMode;
    runtimeStatus.weatherAvailable = _weatherAvailable;
    runtimeStatus.lastUpdateSucceeded = _lastWeatherUpdateSucceeded;
    runtimeStatus.displayedForecast = _displayedForecast;
    runtimeStatus.displayedForecastHours = _displayedForecastHours;
    runtimeStatus.displayBrightnessPercent =
      _schedule.isKnown() && !nightMode ? displaySettings.dayBrightnessPercent : 0;
    runtimeStatus.indicatorBrightness = displaySettings.indicatorBrightness;
    runtimeStatus.lastUpdateMillis = _lastWeatherUpdateMillis;
    runtimeStatus.hasUpdated = _weatherUpdateAttempted;
    _controlCenter.setRuntimeStatus(runtimeStatus);
  delay(DeviceConfiguration::kMainLoopDelayMs);
}
