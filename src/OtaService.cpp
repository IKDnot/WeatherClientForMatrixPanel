#define ESP32_RTOS
#include "OtaService.h"

#ifdef ESP32
#include <WiFi.h>
#include <ESPmDNS.h>
#else
#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#endif

#include <Arduino.h>
#include <ArduinoOTA.h>
#include <WiFiUdp.h>
#include <stdio.h>
#include <string.h>

namespace {
constexpr uint32_t kOtaTaskDelayMs = 3500UL;
constexpr uint32_t kOtaTaskStackSizeBytes = 10000UL;
constexpr UBaseType_t kOtaTaskPriority = 1;
constexpr uint8_t kMacSuffixStartIndex = 3;
constexpr uint8_t kLegacyHostNameBufferAdjustment = 7;
constexpr uint32_t kReconnectDelayMs = 5000UL;
constexpr const char* kMdnsHostName = "esp32-ota";
}

void OtaService::handleTask(void* parameter) {
  (void)parameter;
  for (;;) {
    ArduinoOTA.handle();
    delay(kOtaTaskDelayMs);
  }
}

void OtaService::begin(const char* namePrefix, const char* wifiSsid,
                       const char* wifiPassword) {
  // Preserve the existing host-name formatting used by the configured OTA target.
  const uint16_t hostNameBufferSize = strlen(namePrefix) + kLegacyHostNameBufferAdjustment;
  char* fullHostName = new char[hostNameBufferSize];
  uint8_t macAddress[6];
  WiFi.macAddress(macAddress);
  snprintf(fullHostName, hostNameBufferSize, "%s-%02x%02x%02x", namePrefix,
           macAddress[kMacSuffixStartIndex], macAddress[kMacSuffixStartIndex + 1],
           macAddress[kMacSuffixStartIndex + 2]);
  ArduinoOTA.setHostname(fullHostName);
  delete[] fullHostName;

  WiFi.mode(WIFI_STA);
  WiFi.begin(wifiSsid, wifiPassword);
  while (WiFi.waitForConnectResult() != WL_CONNECTED) {
    Serial.println("Connection Failed! Rebooting...");
    delay(kReconnectDelayMs);
    ESP.restart();
  }

  ArduinoOTA.onStart([]() {
    String type;
    if (ArduinoOTA.getCommand() == U_FLASH) {
      type = "sketch";
    } else {
      type = "filesystem";
    }
    Serial.println("Start updating " + type);
  });

  ArduinoOTA.onEnd([]() {
    Serial.println("\nEnd");
  });

  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
    Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
  });

  ArduinoOTA.onError([](ota_error_t error) {
    Serial.printf("Error[%u]: ", error);
    if (error == OTA_AUTH_ERROR) Serial.println("\nAuth Failed");
    else if (error == OTA_BEGIN_ERROR) Serial.println("\nBegin Failed");
    else if (error == OTA_CONNECT_ERROR) Serial.println("\nConnect Failed");
    else if (error == OTA_RECEIVE_ERROR) Serial.println("\nReceive Failed");
    else if (error == OTA_END_ERROR) Serial.println("\nEnd Failed");
  });

  ArduinoOTA.begin();
  if (!MDNS.begin(kMdnsHostName)) {
    Serial.println("Error setting up MDNS responder!");
  }

  Serial.println("OTA Initialized");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

#if defined(ESP32_RTOS) && defined(ESP32)
  xTaskCreate(handleTask, "OTA_HANDLE", kOtaTaskStackSizeBytes, nullptr,
              kOtaTaskPriority, nullptr);
#endif
}

void OtaService::handle() {
#if !defined(ESP32_RTOS) || !defined(ESP32)
  ArduinoOTA.handle();
#endif
}
