#include "DeviceConfiguration.h"
#include "DeviceSecrets.h"

namespace DeviceConfiguration {
const char* const kWifiSsid = DeviceSecrets::kWifiSsid;
const char* const kWifiPassword = DeviceSecrets::kWifiPassword;
const char* const kWeatherCity = DeviceSecrets::kWeatherCity;
const char* const kWeatherApiKey = DeviceSecrets::kWeatherApiKey;
const char* const kOtaNamePrefix = "LED64_OTA";
}
