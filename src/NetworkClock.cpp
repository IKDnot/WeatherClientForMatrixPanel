#include "NetworkClock.h"

#include <time.h>
#include <Arduino.h>
#include <esp_sntp.h>

#include "DeviceConfiguration.h"

void NetworkClock::begin() {
  configTzTime(DeviceConfiguration::kTimezone,
               DeviceConfiguration::kNtpServerPrimary,
               DeviceConfiguration::kNtpServerSecondary,
               DeviceConfiguration::kNtpServerTertiary);
}

bool NetworkClock::getLocalTime(struct tm& localTime) const {
  const time_t now = time(nullptr);
  if (now < DeviceConfiguration::kMinimumValidEpoch) {
    return false;
  }
  return localtime_r(&now, &localTime) != nullptr;
}
