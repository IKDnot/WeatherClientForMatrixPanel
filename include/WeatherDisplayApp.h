#pragma once

#include "DisplaySchedule.h"
#include "NightIndicator.h"
#include "DisplaySettings.h"
#include "WeatherPanelControlCenter.h"
#include "NetworkClock.h"
#include "OtaService.h"
#include "P3RGB64x64MatrixPanel.h"
#include "WeatherClient.h"
#include "WeatherRefreshTimer.h"
#include "WeatherRenderer.h"

class WeatherDisplayApp {
 public:
  WeatherDisplayApp();
  void begin();
  void update();

 private:
  P3RGB64x64MatrixPanel _panel;
  WeatherClient _weather;
  WeatherRenderer _renderer;
  NetworkClock _clock;
  DisplaySettings _displaySettings;
  DisplaySchedule _schedule;
  WeatherPanelControlCenter _controlCenter;
  NightIndicator _nightIndicator;
  WeatherRefreshTimer _weatherRefreshTimer;
  OtaService _otaService;
  bool _forceWeatherRefresh;
  bool _forecastSettingsInitialized;
  bool _lastForecastModeEnabled;
  uint8_t _lastForecastHours;
  bool _weatherUpdateAttempted;
  bool _weatherAvailable;
  bool _lastWeatherUpdateSucceeded;
  bool _displayedForecast;
  uint8_t _displayedForecastHours;
  uint32_t _lastWeatherUpdateMillis;
};
