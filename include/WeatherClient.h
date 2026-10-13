#ifndef WeatherClient_h
#define WeatherClient_h

#include <Arduino.h>
#include <WiFiUdp.h>
#include <NTPClient.h>

typedef struct {
    float maxTemp;
    float minTemp;
    uint weatherId;
    String weatherAbst;
    String weatherDesc;
    bool isForecast;
    uint8_t forecastHours;
} Weather;

typedef struct {
  String formattedTime;
  ulong epochTime;
} Time;

class WeatherClient {
  public:
    WeatherClient(const char* ssid, const char* password, const char* city, const char* apiKey, bool displayAsCelsius = true);
    ~WeatherClient();
    float getMaxTemperature();
    float getMinTemperature();
    uint getWeatherId();
    String getWeather();
    String getWeatherDescription();
    String getDisplayLabel();
    ulong getEpochTime();
    String getFormattedTime();
    bool update();
    bool update(bool forecastMode, uint8_t forecastHours);

  private:
    const char* _ssid;
    const char* _password;
    const char* _city;
    const char* _apiKey;
    bool _displayAsCelsius;
    Weather _weather;
    WiFiUDP _udp;
    NTPClient _ntp;
    Time _time;
    bool connectWiFi();
    bool fetchWeatherData(bool forecastMode, uint8_t forecastHours);
    bool fetchTimeData();
};

#endif
