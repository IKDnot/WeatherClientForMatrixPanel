#include "WeatherDisplayApp.h"

namespace {
WeatherDisplayApp application;
}

void setup() {
  application.begin();
}

void loop() {
  application.update();
}
