#pragma once

#include "NetworkClock.h"
#include "DisplaySettings.h"
#include "P3RGB64x64MatrixPanel.h"

struct ScheduleUpdate {
  bool changed;
  bool becameDaytime;
};

class DisplaySchedule {
 public:
  DisplaySchedule(NetworkClock& clock, P3RGB64x64MatrixPanel& panel,
                  const DisplaySettings& settings);
  ScheduleUpdate update();
  bool isKnown() const;
  bool isDaytime() const;

 private:
  NetworkClock& _clock;
  P3RGB64x64MatrixPanel& _panel;
  const DisplaySettings& _settings;
  bool _known;
  bool _daytime;
};
