#include "DisplaySchedule.h"

#include <Arduino.h>

#include "DeviceConfiguration.h"

DisplaySchedule::DisplaySchedule(NetworkClock& clock, P3RGB64x64MatrixPanel& panel,
                                 const DisplaySettings& settings)
  : _clock(clock), _panel(panel), _settings(settings), _known(false), _daytime(false) {}

ScheduleUpdate DisplaySchedule::update() {
  struct tm localTime;
  if (!_clock.getLocalTime(localTime)) {
    return {false, false};
  }

  const bool daytime = !_settings.isNightHour(static_cast<uint8_t>(localTime.tm_hour));
  const bool changed = !_known || daytime != _daytime;
  const bool becameDaytime = changed && daytime;
  if (changed) {
    _known = true;
    _daytime = daytime;
    _panel.setDisplayEnabled(true);
    Serial.printf("LED display %s at %02d:%02d JST\n",
                  _daytime ? "enabled" : "night indicator",
                  localTime.tm_hour, localTime.tm_min);
  }
  return {changed, becameDaytime};
}

bool DisplaySchedule::isKnown() const {
  return _known;
}

bool DisplaySchedule::isDaytime() const {
  return _daytime;
}
