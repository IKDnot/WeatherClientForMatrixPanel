#ifndef _ESP32_P3_RGB_64_32_MATRIX_PANEL
#define _ESP32_P3_RGB_64_32_MATRIX_PANEL

#include <array>
#include <vector>
#include "Adafruit_GFX.h"
#include "MatrixPanelConfiguration.h"

class P3RGB64x64MatrixPanel : public Adafruit_GFX {
 public:
  P3RGB64x64MatrixPanel(
      uint8_t redUpperPin, uint8_t greenUpperPin, uint8_t blueUpperPin,
      uint8_t redLowerPin, uint8_t greenLowerPin, uint8_t blueLowerPin,
      uint8_t clockPin, uint8_t latchPin, uint8_t outputEnablePin,
      uint8_t addressAPin, uint8_t addressBPin, uint8_t addressCPin,
      uint8_t addressDPin, bool useDoubleBuffer = false);
  P3RGB64x64MatrixPanel(bool useDoubleBuffer = false);

  void begin();
  void stop();
  void setDisplayEnabled(bool enabled);
  void setBrightness(uint8_t brightnessPercent);
  void drawPixel(int16_t x, int16_t y, uint16_t color) override;
  uint16_t color444(uint8_t red, uint8_t green, uint8_t blue);
  uint16_t color555(uint8_t red, uint8_t green, uint8_t blue);
  uint16_t colorHSV(long hue, uint8_t saturation, uint8_t value);
  void copyRgb888Pixels(size_t firstPixel, uint8_t* destination, size_t pixelCount) const;
  void swapBuffer();
  void copyBuffer();

 private:
  uint16_t* matrixbuff;
  std::vector<std::array<uint16_t, MatrixPanelConfiguration::kPixelCount>> _matrixbuff;
  void initMatrixBuff();
  static void IRAM_ATTR onTimer(void);
  void IRAM_ATTR draw();
  uint16_t* drawBuffer();

  hw_timer_t* timer = nullptr;
  uint8_t pinR1 = MatrixPanelConfiguration::kR1Pin;
  uint8_t pinG1 = MatrixPanelConfiguration::kG1Pin;
  uint8_t pinB1 = MatrixPanelConfiguration::kB1Pin;
  uint8_t pinR2 = MatrixPanelConfiguration::kR2Pin;
  uint8_t pinG2 = MatrixPanelConfiguration::kG2Pin;
  uint8_t pinB2 = MatrixPanelConfiguration::kB2Pin;
  uint8_t pinCLK = MatrixPanelConfiguration::kClockPin;
  uint8_t pinLAT = MatrixPanelConfiguration::kLatchPin;
  uint8_t pinOE = MatrixPanelConfiguration::kOutputEnablePin;
  uint8_t pinA = MatrixPanelConfiguration::kAddressAPin;
  uint8_t pinB = MatrixPanelConfiguration::kAddressBPin;
  uint8_t pinC = MatrixPanelConfiguration::kAddressCPin;
  uint8_t pinD = MatrixPanelConfiguration::kAddressDPin;
  uint8_t pinE = MatrixPanelConfiguration::kAddressEPin;
  bool doubleBuffer;
  volatile bool displayEnabled = true;
  volatile uint8_t brightnessPercent =
      MatrixPanelConfiguration::kDefaultDisplayBrightnessPercent;
    volatile uint8_t brightnessThresholds[MatrixPanelConfiguration::kScanlineCount] = {};
    bool brightnessLookupInitialized = false;

  static volatile SemaphoreHandle_t timerSemaphore;
  static P3RGB64x64MatrixPanel* singleton;
};

#endif
