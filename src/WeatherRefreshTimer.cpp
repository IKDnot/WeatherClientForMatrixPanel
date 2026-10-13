#include "WeatherRefreshTimer.h"

#include "DeviceConfiguration.h"

bool WeatherRefreshTimer::isDue(uint32_t currentMillis, bool forceRefresh) {
  if (!_firstRefreshPending && !forceRefresh &&
      currentMillis - _lastRefreshMillis < DeviceConfiguration::kWeatherRefreshIntervalMs) {
    return false;
  }

  _firstRefreshPending = false;
  _lastRefreshMillis = currentMillis;
  return true;
}
