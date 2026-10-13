#pragma once

#include <stdint.h>

class WeatherRenderer;

class NightIndicator {
 public:
  explicit NightIndicator(WeatherRenderer& renderer);
  void update(bool active, bool reset, uint32_t currentMillis, uint8_t brightness);

 private:
  WeatherRenderer& _renderer;
  uint32_t _lastUpdateMillis;
  bool _visible;
};
