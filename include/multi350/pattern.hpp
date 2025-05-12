#pragma once
// SPDX-License-Identifier: BSD-3-Clause
#include <cassert>
#include <cstdint>
#include <vector>

#include "usb.hpp"

namespace multi350 {

/// @brief Maximum pattern entries stored in LUT
inline constexpr size_t g_maxPatterns = 128;
/// @brief Maximum variable exposure pattern entries stored in LUT
inline constexpr size_t g_maxVarExpPats = 1824;

/// @brief Pattern data
struct Pattern {
  enum class LEDSelect : uint8_t {
    PASS = 0,  // No LED, Pass through
    RED = 1,
    GREEN = 2,
    YELLOW = 3,  // Green + Red
    BLUE = 4,
    MAGENTA = 5,  // Blue + Red
    CYAN = 6,     // Blue + Green
    WHITE = 7     // Red + Blue + Green
  };

  enum class TriggerType : uint8_t {
    INTERNAL = 0,
    EXTERNAL_POSITIVE = 1,
    EXTERNAL_NEGATIVE = 2,
    NO_TRIGGER = 3  // Continue from previous, full exposure
  };

  enum class Pattern1bit : uint8_t {
    G0 = 0,
    G1 = 1,
    G2 = 2,
    G3 = 3,
    G4 = 4,
    G5 = 5,
    G6 = 6,
    G7 = 7,
    R0 = 8,
    R1 = 9,
    R2 = 10,
    R3 = 11,
    R4 = 12,
    R5 = 13,
    R6 = 14,
    R7 = 15,
    B0 = 16,
    B1 = 17,
    B2 = 18,
    B3 = 19,
    B4 = 20,
    B5 = 21,
    B6 = 22,
    B7 = 23,
    BLACK = 24
  };

  enum class Pattern2bit : uint8_t {
    G1G0 = 0,
    G3G2 = 1,
    G5G4 = 2,
    G7G6 = 3,
    R1R0 = 4,
    R3R2 = 5,
    R5R4 = 6,
    R7R6 = 7,
    B1B0 = 8,
    B3B2 = 9,
    B5B4 = 10,
    B7B6 = 11
  };

  enum class Pattern3bit : uint8_t {
    G2G1G0 = 0,
    G5G4G3 = 1,
    R0G7G6 = 2,
    R3R2R1 = 3,
    R6R5R4 = 4,
    B1B0R7 = 5,
    B4B3B2 = 6,
    B7B6B5 = 7
  };

  enum class Pattern4bit : uint8_t {
    G3G2G1G0 = 0,
    G7G6G5G4 = 1,
    R3R2R1R0 = 2,
    R7R6R5R4 = 3,
    B3B2B1B0 = 4,
    B7B6B5B4 = 5
  };

  enum class Pattern5bit : uint8_t {
    G5G4G3G2G1 = 0,
    R3R2R1R0G7 = 1,
    B1B0R7R6R5 = 2,
    B7B6B5B4B3 = 3
  };

  enum class Pattern6bit : uint8_t {
    G5G4G3G2G1G0 = 0,
    R3R2R1R0G7G6 = 1,
    B1B0R7R6R5R4 = 2,
    B7B6B5B4B3B2 = 3
  };

  enum class Pattern7bit : uint8_t {
    G7G6G5G4G3G2G1 = 0,
    R7R6R5R4R3R2R1 = 1,
    B7B6B5B4B3B2B1 = 2
  };

  enum class Pattern8bit : uint8_t {
    G7G6G5G4G3G2G1G0 = 0,
    R7R6R5R4R3R2R1R0 = 1,
    B7B6B5B4B3B2B1B0 = 2
  };

  union {
    uint32_t value;
    struct {
      TriggerType triggerType : 2;
      uint8_t patternIndex    : 6;
      uint8_t bitDepth        : 4;
      LEDSelect ledSelect     : 4;
      bool invertPattern      : 1;
      bool insertBlack        : 1;
      bool bufferSwap         : 1;  // requires 230us before next pattern
      bool triggerOutPrevious : 1;
      uint8_t                 : 4;
      uint8_t                 : 8;
    };
  };

  Pattern() : value{0} {}
  Pattern(TriggerType _triggerType, uint8_t _patternIndex, uint8_t _bitDepth,
          LEDSelect _ledSelect, bool _invertPattern, bool _insertBlack,
          bool _bufferSwap, bool _triggerOutPrevious)
      : triggerType{_triggerType},
        patternIndex{_patternIndex},
        bitDepth{_bitDepth},
        ledSelect{_ledSelect},
        invertPattern{_invertPattern},
        insertBlack{_insertBlack},
        bufferSwap{_bufferSwap},
        triggerOutPrevious{_triggerOutPrevious}
  {
  }
};

/// @brief Pattern sequence data
class PatternSequence {
 public:
  PatternSequence() : m_patternNum(0), m_exposure{0x4010}, m_period{0x411A} {}

  /// @brief Clear the pattern sequence data
  inline void clear() { m_patternNum = 0; }

  /// @brief Add pattern to the sequence
  /// @tparam PatternType Pattern definition based on bit depth
  /// @param triggerType Type of trigger for the pattern
  /// @param patternType Which pattern type to use
  /// @param bitDepth Bit depth of pattern
  /// @param ledSelect Which LEDs to activate
  /// @param invertPattern Invert pattern on 1
  /// @param insertBlack Insert black-fill after current pattern. Requires 230us
  /// before the start of next pattern.
  /// @param triggerOutPrevious Behavior of TriggerOut1. This setting cannot be
  /// combined with the black-fill pattern
  /// @return True on success
  template <typename PatternType>
  bool addPattern(const Pattern::TriggerType triggerType,
                  const PatternType patternType, const uint8_t bitDepth,
                  const Pattern::LEDSelect ledSelect,
                  const bool invertPattern = false,
                  const bool insertBlack = false,
                  const bool triggerOutPrevious = false)
  {
    uint8_t patternIndex = static_cast<uint8_t>(patternType);

    assert(patternIndex <= 24);
    assert(bitDepth <= 8);

    bool bufferSwap = (getPatternNum() == 0) ? true : false;

    Pattern pattern(triggerType, patternIndex, bitDepth, ledSelect,
                    invertPattern, insertBlack, bufferSwap, triggerOutPrevious);

    addPattern(pattern);

    return true;
  }

  /// @brief Add pattern to sequence
  /// @param pat Pattern data to add
  inline void addPattern(const Pattern& pat)
  {
    m_patterns[m_patternNum++] = pat;
  }

  /// @brief Get number of patterns in sequence
  /// @return Number of patterns
  inline size_t getPatternNum() { return m_patternNum; }

  /// @brief Get pattern stored at index
  /// @param index Index of pattern
  /// @return Stored pattern
  inline Pattern& getPattern(const size_t index) { return m_patterns[index]; }

  /// @brief Set the exposure time of a pattern. Must be same as exposure time
  /// or 230us less than the frame period. In external video input pattern
  /// sequence modes they must be equal.
  /// @param exposure Desired exposure time in microseconds
  inline void setExposure(const uint32_t exposure) { m_exposure = exposure; }

  /// @brief Get the current exposure time for a pattern
  /// @return Exposure time in microseconds
  inline uint32_t getExposure() { return m_exposure; }

  /// @brief Set the frame period. In external video input pattern this must
  /// match the pattern exposure time.
  /// @param period Frame period in microseconds
  inline void setPeriod(const uint32_t period) { m_period = period; }

  /// @brief Get the current frame period
  /// @return Frame period in microseconds
  inline uint32_t getPeriod() { return m_period; }

 private:
  /// @brief Number of stored pattern data
  size_t m_patternNum;

  /// @brief Array containing the pattern data
  Pattern m_patterns[g_maxPatterns];

  /// @brief Pattern exposure time(us)
  uint32_t m_exposure;

  /// @brief Frame period(us)
  uint32_t m_period;
};

/// @brief Variable exposure pattern data
struct VarExpPat {
  /// @brief Pattern data
  Pattern pattern;
  /// @brief Exposure time(us)
  uint32_t exposure;
  /// @brief Frame period(us)
  uint32_t period;

  VarExpPat() : pattern(), exposure{0}, period{0} {}
  VarExpPat(Pattern _pattern, uint32_t _exposure, uint32_t _period)
      : pattern{_pattern}, exposure{_exposure}, period{_period}
  {
  }
};

/// @brief Sequence containing variable exposure patterns
class VarExpPatSequence {
 public:
  VarExpPatSequence() : m_varExpPatNum(0) {}

  /// @brief Clear the pattern sequence data
  inline void clear() { m_varExpPatNum = 0; }

  /// @brief Add variable exposure pattern to the sequence
  /// @tparam PatternType Pattern definition based on bit depth
  /// @param exposure Exposure time(us)
  /// @param period Pattern period(us)
  /// @param triggerType Type of trigger for the pattern
  /// @param patternType Which pattern type to use
  /// @param bitDepth Bit depth of pattern
  /// @param ledSelect Which LEDs to activate
  /// @param invertPattern Invert pattern on 1
  /// @param insertBlack Insert black-fill after current pattern. Requires 230us
  /// before the start of next pattern.
  /// @param triggerOutPrevious Behavior of TriggerOut1. This setting cannot be
  /// combined with the black-fill pattern
  /// @return True on success
  template <typename PatternType>
  bool addVarExpPat(uint32_t exposure, uint32_t period,
                    Pattern::TriggerType triggerType, PatternType patternType,
                    uint8_t bitDepth, Pattern::LEDSelect ledSelect,
                    bool invertPattern = false, bool insertBlack = false,
                    bool triggerOutPrevious = false)
  {
    uint8_t patternIndex = static_cast<uint8_t>(patternType);

    assert(patternIndex <= 25);  // 25 with bit depth 1 == white fill
    assert(bitDepth <= 8);

    bool bufferSwap = (getVarExpPatNum() == 0) ? true : false;

    Pattern pattern(triggerType, patternIndex, bitDepth, ledSelect,
                    invertPattern, insertBlack, bufferSwap, triggerOutPrevious);

    VarExpPat varExpPat(pattern, exposure, period);

    addVarExpPat(varExpPat);

    return true;
  }

  /// @brief Add variable exposure pattern to sequence
  /// @param varExpPat Variable exposure pattern to add
  inline void addVarExpPat(VarExpPat& varExpPat)
  {
    m_varExpPats[m_varExpPatNum++] = varExpPat;
  }

  /// @brief Number of stored variable exposure pattern data
  /// @return Number of patterns
  inline size_t getVarExpPatNum() { return m_varExpPatNum; }

  /// @brief Get the variable exposure pattern at the index
  /// @param index Index of pattern
  /// @return Variable exposure pattern data
  inline VarExpPat& getVarExpPat(size_t index) { return m_varExpPats[index]; }

 private:
  /// @brief Number of stored variable exposure pattern data
  size_t m_varExpPatNum;
  /// @brief Array storing variable exposure pattern data
  VarExpPat m_varExpPats[g_maxVarExpPats];
};
};  // namespace multi350