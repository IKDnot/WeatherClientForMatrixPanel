#pragma once

#include <WebServer.h>
#include <stdint.h>

struct WeatherPanelRuntimeStatus {
  bool timeSynchronized;
  bool nightModeActive;
  bool weatherAvailable;
  bool lastUpdateSucceeded;
  bool displayedForecast;
  uint8_t displayedForecastHours;
  uint8_t displayBrightnessPercent;
  uint8_t indicatorBrightness;
  int16_t wifiRssi;
  uint32_t lastUpdateMillis;
  bool hasUpdated;
};

class DisplaySettings;
class P3RGB64x64MatrixPanel;

class WeatherPanelControlCenter {
 public:
  WeatherPanelControlCenter(DisplaySettings& settings, P3RGB64x64MatrixPanel& panel);
  void begin();
  void update();
  void setRuntimeStatus(const WeatherPanelRuntimeStatus& status);

 private:
  WebServer _server;
  DisplaySettings& _settings;
  P3RGB64x64MatrixPanel& _panel;
  WeatherPanelRuntimeStatus _runtimeStatus;

  void handleHome();
  void handleGetDeviceInfo();
  void handleGetRuntimeStatus();
  void handleGetFrameBuffer();
  void handleGetSettings();
  void handleSaveSettings();
  bool isLocalClient();
  void replyForbidden();
};
