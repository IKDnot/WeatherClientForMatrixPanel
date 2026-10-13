#include "NightIndicator.h"

#include "DeviceConfiguration.h"
#include "WeatherRenderer.h"

NightIndicator::NightIndicator(WeatherRenderer& renderer)
    : _renderer(renderer), _lastUpdateMillis(0), _visible(true) {}

void NightIndicator::update(bool active, bool reset, uint32_t currentMillis,
                            uint8_t brightness) {
  if (!active) {
    return;
  }

  const bool intervalElapsed =
      currentMillis - _lastUpdateMillis >= DeviceConfiguration::kNightIndicatorIntervalMs;
  if (!reset && !intervalElapsed) {
    return;
  }

  if (reset) {
    _visible = true;
  } else {
    _visible = !_visible;
  }
  _lastUpdateMillis = currentMillis;
  _renderer.drawNightIndicator(_visible, brightness);
}
