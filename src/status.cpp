// SPDX-FileCopyrightText: 2015 Texas Instruments Incorporated
// SPDX-FileCopyrightText: 2026 Kon Hyong Kim <konhyong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
#include "multi350/status.hpp"

namespace multi350 {
namespace status {
namespace hardware {
std::vector<std::string> g_initError = {"Error", "Successful"};
std::vector<std::string> g_DRCError = {
    "No error has occurred",
    "Multiple overlapping bias or reset operations "
    "are accessing the same DMD block"};
std::vector<std::string> g_forcedSwap = {"No error has occurred",
                                         "Forced Swap Error occurred"};
std::vector<std::string> g_sequenceAbort = {
    "No error has occurred",
    "Sequencer has detected an error condition that caused an abort"};
std::vector<std::string> g_sequenceError = {"No error has occurred",
                                            "Sequencer detected an error"};
};  // namespace hardware

namespace system {
std::vector<std::string> g_memoryTest = {"Internal Memory Test failed",
                                         "Internal Memory Test passed"};
};  // namespace system

namespace main {
std::vector<std::string> g_DMDParked = {"DMD micromirrors are not parked",
                                        "DMD micromirrors are parked"};
std::vector<std::string> g_sequenceRunning = {"Sequencer is stopped",
                                              "Sequencer is running normally"};
std::vector<std::string> g_bufferFrozen = {"Frame buffer is not frozen",
                                           "Frame buffer is frozen"};
std::vector<std::string> g_gammaCorrection = {"Gamma correction is disabled",
                                              "Gamma correction is enabled"};
};  // namespace main
};  // namespace status
};  // namespace multi350