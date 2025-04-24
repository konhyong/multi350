#pragma once
// SPDX-License-Identifier: BSD-3-Clause
#include <cstdint>
#include <memory>
#include <vector>

#include "hidapi.h"

/// @brief Multi350 library namespace
namespace multi350 {
/// @brief USB communication related definitions
namespace USB {
/// @brief uint8_t array managed by unique pointers
using Buffer = std::unique_ptr<uint8_t, std::default_delete<uint8_t[]>>;

/// @brief Vendor ID for DLPC350
inline constexpr uint16_t vendorId = 0x0451;

/// @brief Product ID for DLPC350
inline constexpr uint16_t productId = 0x6401;

/// @brief Current HID device used for transactions
extern hid_device* device;

/// @brief All DLPC350 devices connected via HID
extern std::vector<hid_device*> devices;

/// @brief Timeout duration for hid read in milliseconds
inline constexpr int32_t readTimeout = 2000;

/// @brief Maximum packet size in bytes for a single command
inline constexpr size_t packetSize = 64;

/// @brief In/Out buffers equal to HID endpoint size + 1. First byte is for
/// Windows internal use and it is always 0
inline constexpr size_t bufferSize = packetSize + 1;

/// @brief Initialize the HID API for USB transactions
/// @return True on success
extern bool init();

/// @brief Finalize the HID API library and free resources
/// @return True on succcess
extern bool exit();

/// @brief Open all connected DLPC350 devices
/// @return True on success
extern bool open();

/// @brief Close all connections to DLPC350 devices
extern void close();

/// @brief Check if any devices are connected
/// @return True if any devices are connected
extern bool isConnected();

/// @brief Number of connected devices
/// @return Number of connected devices
extern unsigned int deviceNum();

/// @brief Select which device to control
/// @param index Index of device
/// @return True on success
extern bool select(const unsigned int index);

/// @brief Prints information on all connected devices
extern void printDevices();

/// @brief Read from the USB connection
/// @return Buffer containing the  data
extern Buffer read();

/// @brief Write to the USB connection
/// @param data Data buffer to write
/// @return Number of bytes written
extern int32_t write(const Buffer& data);
};  // namespace USB
};  // namespace multi350