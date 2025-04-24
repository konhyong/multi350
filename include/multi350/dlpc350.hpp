#pragma once
// SPDX-License-Identifier: BSD-3-Clause
#include <cstdint>
#include <memory>
#include <string>

#include "pattern.hpp"

namespace multi350 {
/// @brief Bitfield representing hardware status
union HardwareStatus {
  uint8_t value;
  struct {
    bool initError     : 1;  // 0 : error
    uint8_t            : 1;  // reserved
    bool DRCError      : 1;  // 1 : error
    bool forcedSwap    : 1;  // 1 : error
    uint8_t            : 2;  // reserved
    bool sequenceAbort : 1;  // 1 : error
    bool sequenceError : 1;  // 1 :: error
  };
  HardwareStatus() : value{0} {}
};

/// @brief Bitfield representing system status
union SystemStatus {
  uint8_t value;
  struct {
    bool memoryTest : 1;  // 0 : error
    uint8_t         : 7;  // reserved
  };
  SystemStatus() : value{0} {}
};

/// @brief Bitfield representing main status
union MainStatus {
  uint8_t value;
  struct {
    bool DMDParked       : 1;  // 1 : parked
    bool sequenceRunning : 1;  // 1 : running
    bool bufferFrozen    : 1;  // 1 : frozen
    bool gammaCorrection : 1;  // 1 : enabled
    uint8_t              : 4;
  };
  MainStatus() : value{0} {}
};

/// @brief Version data
struct Version {
  union {
    uint32_t value;
    struct {
      uint16_t patch : 16;
      uint8_t minor  : 8;
      uint8_t major  : 8;
    };
  } app, api, softwareConfig, sequenceConfig;
  Version() {}
  Version(const uint32_t _app, const uint32_t _api, const uint32_t _sw,
          const uint32_t _seq)
      : app{_app}, api{_api}, softwareConfig{_sw}, sequenceConfig{_seq}
  {
  }
};

/// @brief Power control mode
enum class PowerMode : bool {
  STANDBY = true,  // Standby Mode
  NORMAL = false   // Normal Operation
};

/// @brief Curtain color control
struct CurtainColor {
  uint16_t red   : 10;
  uint16_t green : 10;
  uint16_t blue  : 10;

  CurtainColor() : red{0}, green{0}, blue{0} {}
  CurtainColor(const uint16_t _red, const uint16_t _green, const uint16_t _blue)
      : red{_red}, green{_green}, blue{_blue}
  {
  }
};

/// @brief Input source type
enum class InputType : uint8_t {
  PARALLEL = 0,
  TEST_PATTERN = 1,
  FLASH = 2,
  FPDLINK = 3
};

/// @brief Parallel interface bit depth
enum class InputBitDepth : uint8_t {
  INTERNAL = 0,
  BITS30 = 0,
  BITS24 = 1,
  BITS20 = 2,
  BITS16 = 3,
  BITS10 = 4,
  BITS8 = 5
};

/// @brief Input source configuration
union InputSource {
  uint8_t value;
  struct {
    InputType type         : 3;
    InputBitDepth bitDepth : 3;
    uint8_t                : 2;
  };
  InputSource() : value{0} {}
  InputSource(const uint8_t _value) : value{_value} {}
  InputSource(const InputType _type, const InputBitDepth _bitDepth)
      : type{_type}, bitDepth{_bitDepth}
  {
  }
};

/// @brief Type of test pattern
enum class TestPattern : uint8_t {
  SOLID_FIELD = 0,
  HORIZONTAL_RAMP = 1,
  VERTICAL_RAMP = 2,
  HORIZONTAL_LINES = 3,
  DIAGONAL_LINES = 4,
  VERTICAL_LINES = 5,
  GRID = 6,
  CHECKERBOARD = 7,
  RGB_RAMP = 8,
  COLOR_BARS = 9,
  STEP_BARS = 10
};

/// @brief LED enable control
enum class LEDEnableMode : bool {
  AUTO = true,    // LED enables are controlled by sequencer
  MANUAL = false  // controlled by LED enable bits
};

/// @brief LED enable configuration
union LEDEnable {
  uint8_t value;
  struct {
    bool red           : 1;
    bool green         : 1;
    bool blue          : 1;
    LEDEnableMode mode : 1;
    uint8_t            : 4;
  };
  LEDEnable() : value{0} {}
  LEDEnable(const uint8_t _value) : value{_value} {}
  LEDEnable(const LEDEnableMode _mode, const bool _red, const bool _green,
            const bool _blue)
      : mode{_mode}, red{_red}, green{_green}, blue{_blue}
  {
  }
};

/// @brief LED driver current control
union LEDCurrent {
  uint32_t value;
  struct {
    uint8_t red;
    uint8_t green;
    uint8_t blue;
  };
  LEDCurrent() : value{0} {}
  LEDCurrent(const uint32_t _value) : value{_value} {}
  LEDCurrent(const uint8_t _red, const uint8_t _green, const uint8_t _blue)
      : red{_red}, green{_green}, blue{_blue}
  {
  }
};

/// @brief Display mode selection
enum class DisplayMode : bool {
  VIDEO = false,  // Video Display
  PATTERN = true  // Pattern Display
};

/// @brief Gamma correction setting. Only applies to video mode.
union GammaCorrection {
  uint8_t value;
  struct {
    bool degammaTable : 1;  // 0 = TI Video (Enhanced)
    uint8_t           : 6;
    bool enable       : 1;
  };
  GammaCorrection() : value{0} {}
  GammaCorrection(const uint8_t _value) : value{_value} {}
  GammaCorrection(const bool _degammaTable, const bool _enable)
      : degammaTable(_degammaTable), enable(_enable)
  {
  }
};

/// @brief Pattern sequence data validation status
union PatternSequenceValidation {
  uint8_t value;
  struct {
    bool invalidPeriod           : 1;
    bool invalidPattern          : 1;
    bool invalidTriggerOut1      : 1;
    bool invalidPostVector       : 1;
    bool invalidPeriodDifference : 1;
    uint8_t                      : 2;
    bool busy                    : 1;
  };
  PatternSequenceValidation() : value{0} {}
  PatternSequenceValidation(const uint8_t _value) : value(_value) {}
  inline bool isValid() { return (value & ((1 << 5) - 1)) == 0; }
  inline bool isReady() { return !busy; }
};

/// @brief Pattern trigger mode selection
enum class PatternTriggerMode : uint8_t {
  MODE0 = 0,  // Pattern Trigger Mode 0: VSYNC serves to trigger the pattern
              // display sequence.
  MODE1 = 1,  // Pattern Trigger Mode 1: Internally or Externally (through
              // TRIG_IN1 and TRIG_IN2) generated trigger.
  MODE2 = 2,  // Pattern Trigger Mode 2: TRIG_IN_1 alternates between two
              // patterns,while TRIG_IN_2 advances to the next pair of patterns.
  MODE3 = 3,  // Pattern Trigger Mode 3: Internally or externally generated
              // trigger for Variable Exposure display sequence.
  MODE4 = 4   // Pattern Trigger Mode 4: VSYNC triggered for Variable Exposure
              // display sequence.
};

/// @brief Pattern display data input source
enum class PatternDataSource : uint8_t {
  EXTERNAL = 0,  // Video port(24-bit RGB / FPD-Link)
  RESERVED1 = 1,
  RESERVED2 = 2,
  INTERNAL = 3  // Internal flash
};

/// @brief Pattern display start/stop status
enum class PatternStatus : uint8_t {
  STOP = 0,   // Next start will restart sequence from beginning
  PAUSE = 1,  // Next start will restart sequence from current pattern
  START = 2   // Start pattern display sequence
};

/// @brief Pattern exposure time and frame period
struct PatternPeriod {
  uint32_t exposure;  // Pattern exposure time (us)
  uint32_t period;    // period period (us)

  PatternPeriod() : exposure{0x4010}, period{0x411A} {}
  PatternPeriod(const uint32_t _exposure, const uint32_t _period)
      : exposure{_exposure}, period{_period}
  {
  }
};

/// @brief Pattern display LUT access control
enum class MailboxMode : uint8_t {
  DISABLE = 0,              // Disable(Close) mailbox
  IMAGE_INDEX = 1,          // Open mailbox for image index configuration
  PATTERN = 2,              // Open mailbox for pattern definition
  VAR_EXPOSURE_PATTERN = 3  // Open mailbox for var exposure pattern definition
};

//// Status Commands

/// @brief getHardwareStatus - CMD2 : 0x1A, CMD3 : 0x0A
std::unique_ptr<HardwareStatus> getHardwareStatus();
/// @brief getSystemStatus - CMD2 : 0x1A, CMD3 : 0x0B
std::unique_ptr<SystemStatus> getSystemStatus();
/// @brief getMainStatus - CMD2 : 0x1A, CMD3 : 0x0C
std::unique_ptr<MainStatus> getMainStatus();
/// @brief getVersion - CMD2 : 0x02, CMD3 : 0x05
std::unique_ptr<Version> getVersion();
/// @brief getFirmwareTag - CMD2 : 0x1A, CMD3 : 0xFF
std::unique_ptr<std::string> getFirmwareTag();

//// Chipset Control Commands

/// @brief softwareReset - CMD2 : 0x08, CMD3 : 0x02
bool softwareReset();

/// @brief getPowerMode - CMD2 : 0x02, CMD3 : 0x00
std::unique_ptr<PowerMode> getPowerMode();
/// @brief setPowerMode - CMD2 : 0x02, CMD3 : 0x00, Param : 1
bool setPowerMode(const PowerMode mode);

/// @brief getColorCurtain - CMD2 : 0x11, CMD3 : 0x00
std::unique_ptr<CurtainColor> getColorCurtain();
/// @brief setColorCurtain - CMD2 : 0x11, CMD3 : 0x00, Param : 6
bool setColorCurtain(const uint16_t red, const uint16_t green,
                     const uint16_t blue);

/// @brief getInputSource - CMD2 : 0x1A, CMD3 : 0x00
std::unique_ptr<InputSource> getInputSource();
/// @brief setInputSource - CMD2 : 0x1A, CMD3 : 0x00, Param : 1
bool setInputSource(const InputType type,
                    const InputBitDepth bitDepth = InputBitDepth::INTERNAL);

/// @brief getTestPattern - CMD2 : 0x12, CMD3 : 0x03
std::unique_ptr<TestPattern> getTestPattern();
/// @brief setTestPattern - CMD2 : 0x12, CMD3 : 0x03, Param : 1
bool setTestPattern(const TestPattern pattern);

/// @brief getLEDEnable - CMD2 : 0x1A, CMD3 : 0x07
std::unique_ptr<LEDEnable> getLEDEnable();
/// @brief setLEDEnable - CMD2 : 0x1A, CMD3 : 0x07, Param : 1
bool setLEDEnable(const LEDEnableMode mode, const bool redEnabled = true,
                  const bool greenEnabled = true,
                  const bool blueEnabled = true);

/// @brief getLEDCurrent - CMD2 : 0x0B, CMD3 : 0x01
std::unique_ptr<LEDCurrent> getLEDCurrent();
/// @brief setLEDCurrent - CMD2 : 0x0B, CMD3 : 0x01, Param : 3
bool setLEDCurrent(const uint8_t red, const uint8_t green, const uint8_t blue);

//// Display Sequences

/// @brief getDisplayMode - CMD2 : 0x1A, CMD3 : 0x1B
std::unique_ptr<DisplayMode> getDisplayMode();
/// @brief setDisplayMode - CMD2 : 0x1A, CMD3 : 0x1B, Param : 1
bool setDisplayMode(const DisplayMode mode);

/// @brief getGammaCorrection - CMD2 : 0x1A, CMD3 : 0x0E
std::unique_ptr<GammaCorrection> getGammaCorrection();
/// @brief setGammaCorrection - CMD2 : 0x1A, CMD3 : 0x0E, Param : 1
bool setGammaCorrection(const bool enable, const bool degammaTable = false);

/// @brief startPatternValidation - CMD2 : 0x1A, CMD3 : 0x1A, Param : 1 // dummy
/// byte
std::unique_ptr<PatternSequenceValidation> startPatternValidation();
/// @brief checkPatternValidation - CMD2 : 0x1A, CMD3 : 0x1A
std::unique_ptr<PatternSequenceValidation> checkPatternValidation();

/// @brief getPatternTriggerMode - CMD2 : 0x1A, CMD3 : 0x23
std::unique_ptr<PatternTriggerMode> getPatternTriggerMode();
/// @brief setPatternTriggerMode - CMD2 : 0x1A, CMD3 : 0x23, Param : 1
bool setPatternTriggerMode(const PatternTriggerMode mode);

/// @brief getPatternDataSource - CMD2 : 0x1A, CMD3 : 0x22
std::unique_ptr<PatternDataSource> getPatternDataSource();
/// @brief setPatternDataSource - CMD2 : 0x1A, CMD3 : 0x22, Param : 1
bool setPatternDataSource(const PatternDataSource input);

/// @brief getPatternStatus - CMD2 : 0x1A, CMD3 : 0x24
std::unique_ptr<PatternStatus> getPatternStatus();
/// @brief setPatternStatus - CMD2 : 0x1A, CMD3 : 0x24, Param : 1
bool setPatternStatus(const PatternStatus mode);

/// @brief getPatternPeriod - CMD2 : 0x1A, CMD3 : 0x29
std::unique_ptr<PatternPeriod> getPatternPeriod();
/// @brief setPatternPeriod - CMD2 : 0x1A, CMD3 : 0x29, Param : 8
bool setPatternPeriod(const uint32_t exposure, const uint32_t frame);

/// @brief setMailboxMode - CMD2 : 0x1A, CMD3 : 0x33, Param : 1
bool setMailboxMode(const MailboxMode mode);
/// @brief setMailboxOffset - CMD2 : 0x1A, CMD3 : 0x32, Param : 1
bool setMailboxOffset(const uint8_t offset);
/// @brief setMailboxVarExpOffset - CMD2 : 0x1A, CMD3 : 0x3F, Param : 2
bool setMailboxVarExpOffset(const uint16_t offset);

/// @brief configurePatternSequence - CMD2 : 0x1A, CMD3 : 0x31, Param : 4
bool configurePatternSequence(PatternSequence& patternSequence,
                              const bool repeat = true,
                              uint8_t patternNumPerTrigOut2 = 1);
/// @brief configureVarExpPatSequence - CMD2 : 0x1A, CMD3 : 0x40, Param : 6
bool configureVarExpPatSequence(VarExpPatSequence& varExpPatSequence,
                                const bool repeat = true,
                                uint16_t varExpPatNumPerTrigOut2 = 1);

/// @brief sendPatternDisplayLUT - CMD2 : 0x1A, CMD3 : 0x34, Param : 3
bool sendPatternDisplayLUT(PatternSequence& patternSequence);
/// @brief sendVarExpPatDisplayLUT - CMD2 : 0x1A, CMD3 : 0x3E, Param : 12
bool sendVarExpPatDisplayLUT(VarExpPatSequence& varExpPatSequence);
};  // namespace multi350