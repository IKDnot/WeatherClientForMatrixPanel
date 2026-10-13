#include "WeatherClient.h"

#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <stdio.h>
#include <time.h>

#include "DeviceConfiguration.h"

namespace {
constexpr char kCurrentWeatherApiUrlFormat[] =
  "http://api.openweathermap.org/data/2.5/weather?q=%s&appid=%s";
constexpr char kForecastApiUrlFormat[] =
  "http://api.openweathermap.org/data/2.5/forecast?q=%s&appid=%s&cnt=%u";
constexpr long kNtpUtcOffsetSeconds = 0;
constexpr int kHttpSuccessStatus = 200;
constexpr size_t kRequestUrlBufferSize = 1024;
constexpr size_t kWeatherJsonCapacity = 16384;
constexpr uint8_t kForecastItemsPerThreeHours = 1;
constexpr uint32_t kWifiRetryDelayMs = 1000UL;
constexpr uint8_t kMaximumWifiRetries = 10;
constexpr float kKelvinToCelsiusOffset = 273.15F;
}

WeatherClient::WeatherClient(const char* ssid, const char* password, const char* city,
                             const char* apiKey, bool displayAsCelsius)
    : _ssid(ssid),
      _password(password),
      _city(city),
      _apiKey(apiKey),
      _displayAsCelsius(displayAsCelsius),
      _weather{},
      _udp(),
      _ntp(_udp, DeviceConfiguration::kNtpServerPrimary, kNtpUtcOffsetSeconds),
      _time{} {}

WeatherClient::~WeatherClient() = default;

float WeatherClient::getMaxTemperature() {
  return _weather.maxTemp;
}

float WeatherClient::getMinTemperature() {
  return _weather.minTemp;
}

uint WeatherClient::getWeatherId() {
  return _weather.weatherId;
}

String WeatherClient::getWeather() {
  return _weather.weatherAbst;
}

String WeatherClient::getWeatherDescription() {
  return _weather.weatherDesc;
}

String WeatherClient::getDisplayLabel() {
  return _weather.isForecast ? String(_weather.forecastHours) + "h" : "Now";
}

ulong WeatherClient::getEpochTime() {
  return _time.epochTime;
}

String WeatherClient::getFormattedTime() {
  return _time.formattedTime;
}

bool WeatherClient::update() {
  return update(false, DeviceConfiguration::kDefaultForecastHours);
}

bool WeatherClient::update(bool forecastMode, uint8_t forecastHours) {
  if (WiFi.status() != WL_CONNECTED && !connectWiFi()) {
    return false;
  }
  return fetchWeatherData(forecastMode, forecastHours) && fetchTimeData();
}

bool WeatherClient::connectWiFi() {
  Serial.println("Connecting to WiFi...");
  Serial.println(_ssid);
  WiFi.begin(_ssid, _password);

  uint8_t retries = 0;
  while (WiFi.status() != WL_CONNECTED) {
    delay(kWifiRetryDelayMs);
    Serial.print(".");
    ++retries;
    if (retries > kMaximumWifiRetries) {
      Serial.println("Could not connect to WiFi");
      return false;
    }
  }

  Serial.println("Connected to WiFi");
  _ntp.begin();
  return true;
}

bool WeatherClient::fetchWeatherData(bool forecastMode, uint8_t forecastHours) {
  HTTPClient http;
  char url[kRequestUrlBufferSize];
  if (forecastMode) {
    uint16_t requestedPeriods =
        (forecastHours / DeviceConfiguration::kForecastIntervalHours) +
        DeviceConfiguration::kForecastLookAheadPeriods;
    if (requestedPeriods > DeviceConfiguration::kForecastResponseMaximumCount) {
      requestedPeriods = DeviceConfiguration::kForecastResponseMaximumCount;
    }
    snprintf(url, sizeof(url), kForecastApiUrlFormat, _city, _apiKey,
             static_cast<unsigned int>(requestedPeriods * kForecastItemsPerThreeHours));
  } else {
    snprintf(url, sizeof(url), kCurrentWeatherApiUrlFormat, _city, _apiKey);
  }
  http.begin(url);
  const int httpResponseCode = http.GET();
  if (httpResponseCode != kHttpSuccessStatus) {
    Serial.print("Error getting weather data. HTTP error code: ");
    Serial.println(httpResponseCode);
    http.end();
    return false;
  }

  const String payload = http.getString();
  StaticJsonDocument<512> filter;
  filter["main"]["temp_max"] = true;
  filter["main"]["temp_min"] = true;
  filter["weather"][0]["id"] = true;
  filter["weather"][0]["main"] = true;
  filter["weather"][0]["description"] = true;
  filter["list"][0]["dt"] = true;
  filter["list"][0]["main"]["temp_max"] = true;
  filter["list"][0]["main"]["temp_min"] = true;
  filter["list"][0]["weather"][0]["id"] = true;
  filter["list"][0]["weather"][0]["main"] = true;
  filter["list"][0]["weather"][0]["description"] = true;
  DynamicJsonDocument document(kWeatherJsonCapacity);
  const DeserializationError jsonError = deserializeJson(
      document, payload, DeserializationOption::Filter(filter));
  if (jsonError) {
    Serial.print("Error parsing weather data: ");
    Serial.println(jsonError.c_str());
    http.end();
    return false;
  }

  JsonObject selectedForecast;
  JsonObject selectedData;
  if (forecastMode) {
    JsonArray forecastList = document["list"].as<JsonArray>();
    if (forecastList.isNull() || forecastList.size() == 0) {
      http.end();
      return false;
    }

    const time_t now = time(nullptr);
    const time_t targetTime = now >= DeviceConfiguration::kMinimumValidEpoch
                                  ? now + static_cast<time_t>(forecastHours) * 3600
                                  : 0;
    uint64_t closestDifference = UINT64_MAX;
    for (JsonObject forecastEntry : forecastList) {
      if (targetTime == 0) {
        selectedForecast = forecastEntry;
        break;
      }
      const time_t entryTime = forecastEntry["dt"].as<time_t>();
      const uint64_t difference = entryTime > targetTime
                                      ? static_cast<uint64_t>(entryTime - targetTime)
                                      : static_cast<uint64_t>(targetTime - entryTime);
      if (difference < closestDifference) {
        closestDifference = difference;
        selectedForecast = forecastEntry;
      }
    }
    if (selectedForecast.isNull()) {
      http.end();
      return false;
    }
    selectedData = selectedForecast;
  } else {
    selectedData = document.as<JsonObject>();
  }

  _weather.maxTemp = selectedData["main"]["temp_max"];
  _weather.minTemp = selectedData["main"]["temp_min"];
  if (_displayAsCelsius) {
    _weather.maxTemp -= kKelvinToCelsiusOffset;
    _weather.minTemp -= kKelvinToCelsiusOffset;
  }
  _weather.weatherId = selectedData["weather"][0]["id"];
  _weather.weatherAbst = selectedData["weather"][0]["main"].as<String>();
  _weather.weatherDesc = selectedData["weather"][0]["description"].as<String>();
  _weather.isForecast = forecastMode;
  _weather.forecastHours = forecastMode ? forecastHours : 0;
  http.end();
  return true;
}

bool WeatherClient::fetchTimeData() {
  _time.epochTime = _ntp.getEpochTime();
  _time.formattedTime = _ntp.getFormattedTime();
  return true;
}
