#pragma once

#include <stddef.h>
#include <stdint.h>

namespace MatrixPanelConfiguration {
constexpr int16_t kWidth = 64;
constexpr int16_t kHeight = 64;
constexpr size_t kPixelCount = static_cast<size_t>(kWidth) * kHeight;
constexpr size_t kRgb888BytesPerPixel = 3;
constexpr size_t kPreviewChunkPixelCount = 128;
constexpr size_t kFramebufferCount = 1;
constexpr size_t kDoubleFramebufferCount = 2;
constexpr uint8_t kR1Pin = 25;
constexpr uint8_t kG1Pin = 26;
constexpr uint8_t kB1Pin = 27;
constexpr uint8_t kR2Pin = 21;
constexpr uint8_t kG2Pin = 22;
constexpr uint8_t kB2Pin = 23;
constexpr uint8_t kClockPin = 15;
constexpr uint8_t kLatchPin = 32;
constexpr uint8_t kOutputEnablePin = 33;
constexpr uint8_t kAddressAPin = 12;
constexpr uint8_t kAddressBPin = 16;
constexpr uint8_t kAddressCPin = 17;
constexpr uint8_t kAddressDPin = 18;
constexpr uint8_t kAddressEPin = 4;
constexpr uint8_t kGpioHighRegisterOffset = 32;
constexpr uint8_t kScanlineCount = 31;
constexpr uint8_t kInitialScanline = 30;
constexpr uint8_t kSecondPanelRowOffset = 32;
constexpr uint8_t kMaximumPwmComparison = kScanlineCount - 1;
constexpr uint8_t kDefaultDisplayBrightnessPercent = 100;
constexpr uint8_t kMaximumDisplayBrightnessPercent = 100;
constexpr uint16_t kBrightnessPercentageScale = 100;
constexpr uint16_t kBrightnessRoundingOffset = 50;
constexpr uint8_t kTimerPrescaler = 80;
constexpr uint16_t kTimerAlarmTicks = 50;
constexpr uint8_t kLatchWaitIterations = 8;
constexpr uint16_t kHsvWheelSize = 1536;
constexpr uint16_t kHsvSegmentSize = 256;
constexpr uint8_t kHsvComponentShift = 8;
constexpr uint8_t kHsvValueShift = 11;
constexpr uint8_t kRgb555ChannelMask = 0x1f;
constexpr uint8_t kRgb444ChannelMask = 0x0f;
constexpr uint8_t kRgb555To8BitShift = 3;
constexpr uint8_t kRgb555LowBitsShift = 2;
constexpr uint8_t kRed555Shift = 0;
constexpr uint8_t kGreen555Shift = 5;
constexpr uint8_t kBlue555Shift = 10;
constexpr uint8_t kRed444Shift = 1;
constexpr uint8_t kGreen444Shift = 6;
constexpr uint8_t kBlue444Shift = 11;
constexpr uint8_t kGreenHsvSector = 2;
constexpr uint8_t kRedYellowHsvSector = 0;
constexpr uint8_t kYellowGreenHsvSector = 1;
constexpr uint8_t kGreenCyanHsvSector = 3;
constexpr uint8_t kCyanBlueHsvSector = 4;
constexpr uint8_t kMaximum8BitColor = 255;
constexpr uint8_t kSaturationAdjustment = 1;
constexpr uint8_t kValueAdjustment = 1;
constexpr uint8_t kInitialCounter = 30;
constexpr uint8_t kCounterMaskTwoBit = 0x02;
constexpr uint8_t kCounterMaskFourBit = 0x04;
constexpr uint8_t kCounterMaskEightBit = 0x08;
constexpr uint8_t kCounterMaskSixteenBit = 0x10;
constexpr uint8_t kCounterMaskShift = 2;
constexpr uint8_t kCounterMaskHighShift = 4;
constexpr uint8_t kAddressABitMask = 1;
constexpr uint8_t kAddressBBitMask = 2;
constexpr uint8_t kAddressCBitMask = 4;
constexpr uint8_t kAddressDBitMask = 8;
constexpr uint8_t kAddressEBitMask = 16;
constexpr uint8_t kAddressBShift = 1;
constexpr uint8_t kAddressCShift = 2;
constexpr uint8_t kAddressDShift = 3;
constexpr uint8_t kAddressEShift = 4;
}
