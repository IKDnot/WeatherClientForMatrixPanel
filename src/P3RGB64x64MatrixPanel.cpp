#include "P3RGB64x64MatrixPanel.h"

volatile SemaphoreHandle_t P3RGB64x64MatrixPanel::timerSemaphore;
P3RGB64x64MatrixPanel* P3RGB64x64MatrixPanel::singleton;

void IRAM_ATTR P3RGB64x64MatrixPanel::onTimer() {
  portMUX_TYPE timerMux = portMUX_INITIALIZER_UNLOCKED;
  portENTER_CRITICAL_ISR(&timerMux);
  singleton->draw();
  portEXIT_CRITICAL_ISR(&timerMux);
  xSemaphoreGiveFromISR(timerSemaphore, nullptr);
}

void P3RGB64x64MatrixPanel::begin() {
  singleton = this;

  pinMode(pinR1, OUTPUT);
  pinMode(pinG1, OUTPUT);
  pinMode(pinB1, OUTPUT);
  pinMode(pinR2, OUTPUT);
  pinMode(pinG2, OUTPUT);
  pinMode(pinB2, OUTPUT);
  pinMode(pinLAT, OUTPUT);
  pinMode(pinCLK, OUTPUT);
  pinMode(pinOE, OUTPUT);
  pinMode(pinA, OUTPUT);
  pinMode(pinB, OUTPUT);
  pinMode(pinC, OUTPUT);
  pinMode(pinD, OUTPUT);
  pinMode(pinE, OUTPUT);

  digitalWrite(pinLAT, LOW);
  digitalWrite(pinCLK, LOW);
  digitalWrite(pinOE, HIGH);

  setBrightness(brightnessPercent);
  timerSemaphore = xSemaphoreCreateBinary();
  timer = timerBegin(0, MatrixPanelConfiguration::kTimerPrescaler, true);
  timerAttachInterrupt(timer, &onTimer, true);
  timerAlarmWrite(timer, MatrixPanelConfiguration::kTimerAlarmTicks, true);
  timerAlarmEnable(timer);
}

void P3RGB64x64MatrixPanel::stop() {
  if (timer != nullptr) {
    timerDetachInterrupt(timer);
    timerEnd(timer);
    timer = nullptr;
  }
}

void P3RGB64x64MatrixPanel::setDisplayEnabled(bool enabled) {
  displayEnabled = enabled;
}

void P3RGB64x64MatrixPanel::setBrightness(uint8_t valuePercent) {
  if (valuePercent > MatrixPanelConfiguration::kMaximumDisplayBrightnessPercent) {
    valuePercent = MatrixPanelConfiguration::kMaximumDisplayBrightnessPercent;
  }
  if (brightnessLookupInitialized && brightnessPercent == valuePercent) {
    return;
  }
  brightnessPercent = valuePercent;
  for (uint8_t comparison = 0; comparison < MatrixPanelConfiguration::kScanlineCount;
       ++comparison) {
    const uint16_t threshold = valuePercent == 0
        ? MatrixPanelConfiguration::kMaximumPwmComparison + 1
        : (static_cast<uint16_t>(comparison) *
           MatrixPanelConfiguration::kBrightnessPercentageScale) / valuePercent;
    brightnessThresholds[comparison] = threshold > MatrixPanelConfiguration::kMaximumPwmComparison
                                           ? MatrixPanelConfiguration::kMaximumPwmComparison + 1
                                           : static_cast<uint8_t>(threshold);
  }
  brightnessLookupInitialized = true;
}

P3RGB64x64MatrixPanel::P3RGB64x64MatrixPanel(
    uint8_t redUpperPin, uint8_t greenUpperPin, uint8_t blueUpperPin,
    uint8_t redLowerPin, uint8_t greenLowerPin, uint8_t blueLowerPin,
    uint8_t clockPin, uint8_t latchPin, uint8_t outputEnablePin,
    uint8_t addressAPin, uint8_t addressBPin, uint8_t addressCPin,
    uint8_t addressDPin, bool useDoubleBuffer)
    : Adafruit_GFX(MatrixPanelConfiguration::kWidth, MatrixPanelConfiguration::kHeight),
      pinR1(redUpperPin), pinG1(greenUpperPin), pinB1(blueUpperPin),
      pinR2(redLowerPin), pinG2(greenLowerPin), pinB2(blueLowerPin),
      pinCLK(clockPin), pinLAT(latchPin), pinOE(outputEnablePin),
      pinA(addressAPin), pinB(addressBPin), pinC(addressCPin), pinD(addressDPin),
      doubleBuffer(useDoubleBuffer) {
  initMatrixBuff();
}

P3RGB64x64MatrixPanel::P3RGB64x64MatrixPanel(bool useDoubleBuffer)
    : Adafruit_GFX(MatrixPanelConfiguration::kWidth, MatrixPanelConfiguration::kHeight),
      doubleBuffer(useDoubleBuffer) {
  initMatrixBuff();
}

void P3RGB64x64MatrixPanel::initMatrixBuff() {
  _matrixbuff.resize(doubleBuffer ? MatrixPanelConfiguration::kDoubleFramebufferCount
                                  : MatrixPanelConfiguration::kFramebufferCount);
  matrixbuff = _matrixbuff[0].data();
}

void P3RGB64x64MatrixPanel::swapBuffer() {
  matrixbuff = drawBuffer();
}

void P3RGB64x64MatrixPanel::copyBuffer() {
  if (!doubleBuffer) return;
  if (matrixbuff == _matrixbuff[0].data()) {
    _matrixbuff[0] = _matrixbuff[1];
  } else {
    _matrixbuff[1] = _matrixbuff[0];
  }
}

uint16_t* P3RGB64x64MatrixPanel::drawBuffer() {
  if (!doubleBuffer) return _matrixbuff[0].data();
  return matrixbuff == _matrixbuff[0].data() ? _matrixbuff[1].data()
                                              : _matrixbuff[0].data();
}

uint16_t P3RGB64x64MatrixPanel::color444(uint8_t red, uint8_t green, uint8_t blue) {
  return ((red & MatrixPanelConfiguration::kRgb444ChannelMask)
          << MatrixPanelConfiguration::kRed444Shift) |
         (static_cast<uint16_t>(green & MatrixPanelConfiguration::kRgb444ChannelMask)
          << MatrixPanelConfiguration::kGreen444Shift) |
         (static_cast<uint16_t>(blue & MatrixPanelConfiguration::kRgb444ChannelMask)
          << MatrixPanelConfiguration::kBlue444Shift);
}

uint16_t P3RGB64x64MatrixPanel::color555(uint8_t red, uint8_t green, uint8_t blue) {
  return ((red & MatrixPanelConfiguration::kRgb555ChannelMask)
          << MatrixPanelConfiguration::kRed555Shift) |
         (static_cast<uint16_t>(green & MatrixPanelConfiguration::kRgb555ChannelMask)
          << MatrixPanelConfiguration::kGreen555Shift) |
         (static_cast<uint16_t>(blue & MatrixPanelConfiguration::kRgb555ChannelMask)
          << MatrixPanelConfiguration::kBlue555Shift);
}

void P3RGB64x64MatrixPanel::copyRgb888Pixels(size_t firstPixel, uint8_t* destination,
                                              size_t pixelCount) const {
  if (destination == nullptr || firstPixel >= MatrixPanelConfiguration::kPixelCount) {
    return;
  }

  const size_t availablePixels = MatrixPanelConfiguration::kPixelCount - firstPixel;
  if (pixelCount > availablePixels) {
    pixelCount = availablePixels;
  }

  for (size_t offset = 0; offset < pixelCount; ++offset) {
    const size_t outputIndex = offset * MatrixPanelConfiguration::kRgb888BytesPerPixel;
    if (!displayEnabled || brightnessPercent == 0) {
      destination[outputIndex] = 0;
      destination[outputIndex + 1] = 0;
      destination[outputIndex + 2] = 0;
      continue;
    }

    const uint16_t color = matrixbuff[firstPixel + offset];
    const uint8_t red = (color >> MatrixPanelConfiguration::kRed555Shift) &
                        MatrixPanelConfiguration::kRgb555ChannelMask;
    const uint8_t green = (color >> MatrixPanelConfiguration::kGreen555Shift) &
                          MatrixPanelConfiguration::kRgb555ChannelMask;
    const uint8_t blue = (color >> MatrixPanelConfiguration::kBlue555Shift) &
                         MatrixPanelConfiguration::kRgb555ChannelMask;
    const uint16_t brightness = brightnessPercent;
    destination[outputIndex] = (((red << MatrixPanelConfiguration::kRgb555To8BitShift) |
                   (red >> MatrixPanelConfiguration::kRgb555LowBitsShift)) *
                    brightness +
                  MatrixPanelConfiguration::kBrightnessRoundingOffset) /
                   MatrixPanelConfiguration::kBrightnessPercentageScale;
    destination[outputIndex + 1] =
      ((((green << MatrixPanelConfiguration::kRgb555To8BitShift) |
         (green >> MatrixPanelConfiguration::kRgb555LowBitsShift)) *
          brightness +
        MatrixPanelConfiguration::kBrightnessRoundingOffset) /
       MatrixPanelConfiguration::kBrightnessPercentageScale);
    destination[outputIndex + 2] =
      ((((blue << MatrixPanelConfiguration::kRgb555To8BitShift) |
         (blue >> MatrixPanelConfiguration::kRgb555LowBitsShift)) *
          brightness +
        MatrixPanelConfiguration::kBrightnessRoundingOffset) /
       MatrixPanelConfiguration::kBrightnessPercentageScale);
  }
}

void P3RGB64x64MatrixPanel::drawPixel(int16_t x, int16_t y, uint16_t color) {
  if (x < 0 || x >= MatrixPanelConfiguration::kWidth ||
      y < 0 || y >= MatrixPanelConfiguration::kHeight) {
    return;
  }
  const size_t pixelIndex = static_cast<size_t>(x) +
                            static_cast<size_t>(y) * MatrixPanelConfiguration::kWidth;
  drawBuffer()[pixelIndex] = color;
}

void IRAM_ATTR P3RGB64x64MatrixPanel::draw() {
  static uint8_t counter = MatrixPanelConfiguration::kInitialCounter;
  static uint8_t row = MatrixPanelConfiguration::kInitialScanline;
  static uint32_t output = 0;

  row = (row + 1) % MatrixPanelConfiguration::kScanlineCount;
  if (row == 0) {
    counter = (counter + 1) % MatrixPanelConfiguration::kScanlineCount;
  }

  const uint8_t comparison = (counter >> MatrixPanelConfiguration::kCounterMaskHighShift) |
                            ((counter >> MatrixPanelConfiguration::kCounterMaskShift) &
                             MatrixPanelConfiguration::kCounterMaskTwoBit) |
                            (counter & MatrixPanelConfiguration::kCounterMaskFourBit) |
                            ((counter << MatrixPanelConfiguration::kCounterMaskShift) &
                             MatrixPanelConfiguration::kCounterMaskEightBit) |
                            ((counter << MatrixPanelConfiguration::kCounterMaskHighShift) &
                             MatrixPanelConfiguration::kCounterMaskSixteenBit);
  const uint8_t adjustedComparison = brightnessThresholds[comparison];

  for (int16_t column = 0; column < MatrixPanelConfiguration::kWidth; ++column) {
    bool redUpper;
    bool greenUpper;
    bool blueUpper;
    bool redLower;
    bool greenLower;
    bool blueLower;

    uint16_t color = matrixbuff[column + row * MatrixPanelConfiguration::kWidth];
    redUpper = ((color >> MatrixPanelConfiguration::kRed555Shift) &
          MatrixPanelConfiguration::kRgb555ChannelMask) > adjustedComparison;
    greenUpper = ((color >> MatrixPanelConfiguration::kGreen555Shift) &
                  MatrixPanelConfiguration::kRgb555ChannelMask) > adjustedComparison;
    blueUpper = ((color >> MatrixPanelConfiguration::kBlue555Shift) &
                 MatrixPanelConfiguration::kRgb555ChannelMask) > adjustedComparison;

    color = matrixbuff[column + (row + MatrixPanelConfiguration::kSecondPanelRowOffset) *
                                    MatrixPanelConfiguration::kWidth];
    redLower = ((color >> MatrixPanelConfiguration::kRed555Shift) &
          MatrixPanelConfiguration::kRgb555ChannelMask) > adjustedComparison;
    greenLower = ((color >> MatrixPanelConfiguration::kGreen555Shift) &
                  MatrixPanelConfiguration::kRgb555ChannelMask) > adjustedComparison;
    blueLower = ((color >> MatrixPanelConfiguration::kBlue555Shift) &
                 MatrixPanelConfiguration::kRgb555ChannelMask) > adjustedComparison;

    REG_WRITE(GPIO_OUT_REG, output |
        (static_cast<uint32_t>(redUpper) << pinR1) |
        (static_cast<uint32_t>(greenUpper) << pinG1) |
        (static_cast<uint32_t>(blueUpper) << pinB1) |
        (static_cast<uint32_t>(redLower) << pinR2) |
        (static_cast<uint32_t>(greenLower) << pinG2) |
        (static_cast<uint32_t>(blueLower) << pinB2));
    REG_WRITE(GPIO_OUT_W1TS_REG, (1UL << pinCLK));
  }

  REG_WRITE(GPIO_OUT1_W1TS_REG,
            (1UL << (pinOE - MatrixPanelConfiguration::kGpioHighRegisterOffset)) |
            (1UL << (pinLAT - MatrixPanelConfiguration::kGpioHighRegisterOffset)));

  output = ((row & MatrixPanelConfiguration::kAddressABitMask) << pinA) |
           ((row & MatrixPanelConfiguration::kAddressBBitMask) <<
            (pinB - MatrixPanelConfiguration::kAddressBShift)) |
           ((row & MatrixPanelConfiguration::kAddressCBitMask) <<
            (pinC - MatrixPanelConfiguration::kAddressCShift)) |
           ((row & MatrixPanelConfiguration::kAddressDBitMask) <<
            (pinD - MatrixPanelConfiguration::kAddressDShift)) |
           ((row & MatrixPanelConfiguration::kAddressEBitMask) <<
            (pinE - MatrixPanelConfiguration::kAddressEShift));

  REG_WRITE(GPIO_OUT_REG, output);
  for (uint8_t waitCycle = 0;
       waitCycle < MatrixPanelConfiguration::kLatchWaitIterations;
       ++waitCycle) {
    NOP();
  }

  REG_WRITE(GPIO_OUT1_W1TC_REG,
            (1UL << (pinLAT - MatrixPanelConfiguration::kGpioHighRegisterOffset)));
  if (displayEnabled) {
    REG_WRITE(GPIO_OUT1_W1TC_REG,
              (1UL << (pinOE - MatrixPanelConfiguration::kGpioHighRegisterOffset)));
  }
}
