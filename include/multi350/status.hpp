#pragma once
// SPDX-License-Identifier: BSD-3-Clause
#include <string>
#include <vector>

namespace multi350 {
/// @brief Status report
namespace status {
/// @brief Hardware status
namespace hardware {
/// @brief Initialization error
extern std::vector<std::string> initError;
/// @brief DMD reset control error
extern std::vector<std::string> DRCError;
/// @brief Forced Swap Error
extern std::vector<std::string> forcedSwap;
/// @brief Sequencer Abort Status
extern std::vector<std::string> sequenceAbort;
/// @brief Sequencer Error
extern std::vector<std::string> sequenceError;
};  // namespace hardware

/// @brief System status
namespace system {
/// @brief Internal memory test
extern std::vector<std::string> memoryTest;
};  // namespace system

/// @brief Main status
namespace main {
/// @brief DMD micromirrors park status
extern std::vector<std::string> DMDParked;
/// @brief Sequencer run flag
extern std::vector<std::string> sequenceRunning;
/// @brief Framebuffer swap flag
extern std::vector<std::string> bufferFrozen;
/// @brief Gamma correction function enable
extern std::vector<std::string> gammaCorrection;
};  // namespace main
};  // namespace status
};  // namespace multi350