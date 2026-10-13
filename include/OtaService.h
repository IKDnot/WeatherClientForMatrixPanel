#pragma once

class OtaService {
 public:
  void begin(const char* namePrefix, const char* wifiSsid, const char* wifiPassword);
  void handle();

 private:
  static void handleTask(void* parameter);
};
