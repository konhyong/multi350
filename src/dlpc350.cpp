// SPDX-FileCopyrightText: 2025 Kon Hyong Kim <konhyong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
#include "multi350/dlpc350.hpp"

#include "multi350/message.hpp"

namespace multi350 {

std::unique_ptr<HardwareStatus> getHardwareStatus()
{
  auto result = sendGetMessage<HardwareStatus>(0x1A0A);
  return std::make_unique<HardwareStatus>(*result.get());
}

std::unique_ptr<SystemStatus> getSystemStatus()
{
  auto result = sendGetMessage<SystemStatus>(0x1A0B);
  return std::make_unique<SystemStatus>(*result.get());
}

std::unique_ptr<MainStatus> getMainStatus()
{
  auto result = sendGetMessage<MainStatus>(0x1A0C);
  return std::make_unique<MainStatus>(*result.get());
}

std::unique_ptr<Version> getVersion()
{
  auto result = sendGetMessage<uint32_t>(0x0205);
  return std::make_unique<Version>(*(result.get()), *(result.get() + 1),
                                   *(result.get() + 2), *(result.get() + 3));
}

std::unique_ptr<std::string> getFirmwareTag()
{
  auto result = sendGetMessage<char>(0x1AFF);
  return std::make_unique<std::string>(result.get());
}

bool softwareReset()
{
  auto result = sendNoAckMessage(0x0802);
  return (result > 0);
}

std::unique_ptr<PowerMode> getPowerMode()
{
  auto result = sendGetMessage<PowerMode>(0x0200);
  return std::make_unique<PowerMode>(*result.get());
}

bool setPowerMode(const PowerMode mode)
{
  auto result = sendSetMessage<uint8_t>(0x0200, static_cast<uint8_t>(mode));
  return (result != nullptr);
}

std::unique_ptr<CurtainColor> getColorCurtain()
{
  auto result = sendGetMessage<uint16_t>(0x1100);
  return std::make_unique<CurtainColor>(*result.get(), *(result.get() + 1),
                                        *(result.get() + 2));
}

bool setColorCurtain(const uint16_t red, const uint16_t green,
                     const uint16_t blue)
{
  auto result = sendSetMessage<uint16_t, uint16_t, uint16_t>(
      0x1100, std::forward<const uint16_t>(red),
      std::forward<const uint16_t>(green), std::forward<const uint16_t>(blue));
  return (result != nullptr);
}

std::unique_ptr<InputSource> getInputSource()
{
  auto result = sendGetMessage<InputSource>(0x1A00);
  return std::make_unique<InputSource>(*result.get());
}

bool setInputSource(const InputType type, const InputBitDepth bitDepth)
{
  auto result =
      sendSetMessage<uint8_t>(0x1A00, InputSource(type, bitDepth).value);
  return (result != nullptr);
}

std::unique_ptr<TestPattern> getTestPattern()
{
  assert(getInputSource()->type == InputType::TEST_PATTERN);
  auto result = sendGetMessage<TestPattern>(0x1203);
  return std::make_unique<TestPattern>(*result.get());
}

bool setTestPattern(const TestPattern pattern)
{
  assert(getInputSource()->type == InputType::TEST_PATTERN);
  auto result = sendSetMessage<uint8_t>(0x1203, static_cast<uint8_t>(pattern));
  return (result != nullptr);
}

std::unique_ptr<LEDEnable> getLEDEnable()
{
  auto result = sendGetMessage<LEDEnable>(0x1A07);
  return std::make_unique<LEDEnable>(*result.get());
}

bool setLEDEnable(const LEDEnableMode mode, const bool redEnabled,
                  const bool greenEnabled, const bool blueEnabled)
{
  auto result = sendSetMessage<uint8_t>(
      0x1A07, LEDEnable(mode, redEnabled, greenEnabled, blueEnabled).value);
  return (result != nullptr);
}

std::unique_ptr<LEDCurrent> getLEDCurrent()
{
  auto result = sendGetMessage<uint32_t>(0x0B01);
  return std::make_unique<LEDCurrent>(*result.get());
}

bool setLEDCurrent(const uint8_t red, const uint8_t green, const uint8_t blue)
{
  auto result = sendSetMessage<uint8_t, uint8_t, uint8_t>(
      0x0B01, 255 - red, 255 - green, 255 - blue);
  return (result != nullptr);
}

std::unique_ptr<DisplayMode> getDisplayMode()
{
  auto result = sendGetMessage<DisplayMode>(0x1A1B);
  return std::make_unique<DisplayMode>(*result.get());
}

bool setDisplayMode(const DisplayMode mode)
{
  auto result = sendSetMessage<uint8_t>(0x1A1B, static_cast<uint8_t>(mode));
  return (result != nullptr);
}

std::unique_ptr<GammaCorrection> getGammaCorrection()
{
  auto result = sendGetMessage<GammaCorrection>(0x1A0E);
  return std::make_unique<GammaCorrection>(*result.get());
}

bool setGammaCorrection(const bool enable, const bool degammaTable)
{
  auto result = sendSetMessage<uint8_t>(
      0x1A0E, GammaCorrection(degammaTable, enable).value);
  return (result != nullptr);
}

std::unique_ptr<PatternSequenceValidation> startPatternValidation()
{
  auto result = sendSetMessage<PatternSequenceValidation>(0x1A1A, 0x00);
  return std::make_unique<PatternSequenceValidation>(*result.get());
}

std::unique_ptr<PatternSequenceValidation> checkPatternValidation()
{
  auto result = sendGetMessage<PatternSequenceValidation>(0x1A1A);
  return std::make_unique<PatternSequenceValidation>(*result.get());
}

std::unique_ptr<PatternTriggerMode> getPatternTriggerMode()
{
  auto result = sendGetMessage<PatternTriggerMode>(0x1A23);
  return std::make_unique<PatternTriggerMode>(*result.get());
}

bool setPatternTriggerMode(const PatternTriggerMode mode)
{
  auto result = sendSetMessage<uint8_t>(0x1A23, static_cast<uint8_t>(mode));
  return (result != nullptr);
}

std::unique_ptr<PatternDataSource> getPatternDataSource()
{
  auto result = sendGetMessage<PatternDataSource>(0x1A22);
  return std::make_unique<PatternDataSource>(*result.get());
}

bool setPatternDataSource(const PatternDataSource input)
{
  auto result = sendSetMessage<uint8_t>(0x1A22, static_cast<uint8_t>(input));
  return (result != nullptr);
}

std::unique_ptr<PatternStatus> getPatternStatus()
{
  auto result = sendGetMessage<PatternStatus>(0x1A24);
  return std::make_unique<PatternStatus>(*result.get());
}

bool setPatternStatus(const PatternStatus mode)
{
  auto result = sendSetMessage<uint8_t>(0x1A24, static_cast<uint8_t>(mode));
  return (result != nullptr);
}

std::unique_ptr<PatternPeriod> getPatternPeriod()
{
  auto result = sendGetMessage<uint32_t>(0x1A29);
  return std::make_unique<PatternPeriod>(*result.get(), *(result.get() + 1));
}

bool setPatternPeriod(const uint32_t exposure, const uint32_t frame)
{
  assert(exposure <= frame);
  assert(frame - exposure > 230);

  auto result = sendSetMessage<uint32_t, uint32_t>(
      0x1A29, std::forward<const uint32_t>(exposure),
      std::forward<const uint32_t>(frame));
  return (result != nullptr);
}

bool setMailboxMode(const MailboxMode mode)
{
  auto result = sendSetMessage<uint8_t>(0x1A33, static_cast<uint8_t>(mode));
  return (result != nullptr);
}

bool setMailboxOffset(const uint8_t offset)
{
  assert(offset <= 127);

  auto result =
      sendSetMessage<uint8_t>(0x1A32, std::forward<const uint8_t>(offset));
  return (result != nullptr);
}

bool setMailboxVarExpOffset(const uint16_t offset)
{
  assert(offset <= 1823);

  auto result =
      sendSetMessage<uint16_t>(0x1A3F, std::forward<const uint16_t>(offset));
  return (result != nullptr);
}

bool configurePatternSequence(PatternSequence& patternSequence,
                              const bool repeat, uint8_t patternNumPerTrigOut2)
{
  if (repeat) {
    patternNumPerTrigOut2 =
        static_cast<uint8_t>(patternSequence.getPatternNum());
  }
  auto result = sendSetMessage<uint8_t, uint8_t, uint8_t, uint8_t>(
      0x1A31, static_cast<uint8_t>(patternSequence.getPatternNum() - 1),
      static_cast<uint8_t>(repeat),
      static_cast<uint8_t>(patternNumPerTrigOut2 - 1),
      static_cast<uint8_t>(
          0));  // Irrelevant unless PatternDataSource::INTERNAL
  return (result != nullptr);
}

bool configureVarExpPatSequence(VarExpPatSequence& varExpPatSequence,
                                const bool repeat,
                                uint16_t varExpPatNumPerTrigOut2)
{
  if (repeat) {
    varExpPatNumPerTrigOut2 =
        static_cast<uint16_t>(varExpPatSequence.getVarExpPatNum());
  }
  auto result = sendSetMessage<uint16_t, uint16_t, uint8_t, uint8_t>(
      0x1A40, static_cast<uint16_t>(varExpPatSequence.getVarExpPatNum() - 1),
      static_cast<uint16_t>(varExpPatNumPerTrigOut2 - 1),
      static_cast<uint8_t>(0),  // Irrelevant unless PatternDataSource::INTERNAL
      static_cast<uint8_t>(repeat));
  return (result != nullptr);
}

bool sendPatternDisplayLUT(PatternSequence& patternSequence)
{
  if (!setMailboxMode(MailboxMode::PATTERN)) return false;

  setMailboxOffset(0);

  auto send = Message(Message::Type::WRITE, 0x1A34);

  // TODO: possible to use sendSetMessage & addData?
  for (size_t i = 0; i < patternSequence.getPatternNum(); i++) {
    Pattern& pattern = patternSequence.getPattern(i);
    uint8_t* value = reinterpret_cast<uint8_t*>(&pattern.value);
    for (size_t j = 0; j < 3; j++) {
      send.data[send.length++] = *(value++);
    }
  }

  auto result = transact(send);

  setMailboxMode(MailboxMode::DISABLE);

  return (result != nullptr);
}

bool sendVarExpPatDisplayLUT(VarExpPatSequence& varExpPatSequence)
{
  if (!setMailboxMode(MailboxMode::VAR_EXPOSURE_PATTERN)) return false;

  for (size_t i = 0; i < varExpPatSequence.getVarExpPatNum(); i++) {
    setMailboxVarExpOffset(i);

    auto send = Message(Message::Type::WRITE, 0x1A3E);

    VarExpPat& varExpPat = varExpPatSequence.getVarExpPat(i);
    uint8_t* value = reinterpret_cast<uint8_t*>(&varExpPat.pattern);
    for (size_t j = 0; j < 12; j++) {
      send.data[send.length++] = *(value++);
    }
    auto result = transact(send);
    if (result == nullptr) {
      return false;
    }
  }

  setMailboxMode(MailboxMode::DISABLE);

  return true;
}

};  // namespace multi350