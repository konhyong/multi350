// SPDX-FileCopyrightText: 2015 Texas Instruments Incorporated
// SPDX-FileCopyrightText: 2026 Kon Hyong Kim <konhyong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
#pragma once
#include <string>
#include <vector>

namespace multi350 {
/// @brief Status report
namespace status {
/// @brief Hardware status
namespace hardware {
/// @brief Initialization error
extern std::vector<std::string> g_initError;
/// @brief DMD reset control error
extern std::vector<std::string> g_DRCError;
/// @brief Forced Swap Error
extern std::vector<std::string> g_forcedSwap;
/// @brief Sequencer Abort Status
extern std::vector<std::string> g_sequenceAbort;
/// @brief Sequencer Error
extern std::vector<std::string> g_sequenceError;
};  // namespace hardware

/// @brief System status
namespace system {
/// @brief Internal memory test
extern std::vector<std::string> g_memoryTest;
};  // namespace system

/// @brief Main status
namespace main {
/// @brief DMD micromirrors park status
extern std::vector<std::string> g_DMDParked;
/// @brief Sequencer run flag
extern std::vector<std::string> g_sequenceRunning;
/// @brief Framebuffer swap flag
extern std::vector<std::string> g_bufferFrozen;
/// @brief Gamma correction function enable
extern std::vector<std::string> g_gammaCorrection;
};  // namespace main
};  // namespace status
};  // namespace multi350