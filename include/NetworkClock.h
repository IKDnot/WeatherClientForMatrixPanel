#pragma once

#include <time.h>

class NetworkClock {
 public:
  void begin();
  bool getLocalTime(struct tm& localTime) const;
};
