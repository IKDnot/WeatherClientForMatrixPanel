#pragma once

#include <Arduino.h>

class P3RGB64x64MatrixPanel;
class WeatherClient;

class WeatherRenderer {
 public:
  explicit WeatherRenderer(P3RGB64x64MatrixPanel& panel);
  void begin();
  void renderWeather(WeatherClient& weather);
  void renderError();
  void drawNightIndicator(bool visible, uint8_t brightness);

 private:
  P3RGB64x64MatrixPanel& _panel;
  uint16_t textColorForWeather(uint weatherId);
  uint16_t iconColorForWeather(uint weatherId);
  const uint8_t* iconForWeather(uint weatherId) const;
};
