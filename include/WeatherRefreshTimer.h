#pragma once

#include <stdint.h>

class WeatherRefreshTimer {
 public:
  bool isDue(uint32_t currentMillis, bool forceRefresh);

 private:
  uint32_t _lastRefreshMillis = 0;
  bool _firstRefreshPending = true;
};
