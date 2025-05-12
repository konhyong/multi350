// SPDX-License-Identifier: BSD-3-Clause
#include "multi350/usb.hpp"

#include <iostream>

namespace multi350 {
namespace USB {

hid_device* g_device = nullptr;
std::vector<hid_device*> g_devices;

inline bool init() { return (hid_init() == 0); }

inline bool exit() { return (hid_exit() == 0); }

inline bool open()
{
  if (!g_devices.empty()) close();

  hid_device_info* hid_info;
  hid_info = hid_enumerate(g_vendorId, g_productId);
  if (!hid_info) {
    return false;
  }

  while (hid_info) {
    if (hid_info->interface_number == 0) {
      g_device = hid_open_path(hid_info->path);

      if (!g_device) {
        std::wcerr << "[HID] Failed to open device: " << hid_info->serial_number
                   << std::endl;
        close();
        return false;
      }

      g_devices.push_back(g_device);
    }
    hid_info = hid_info->next;
    g_device = nullptr;  // reset to default
  }

  return true;
}

inline void close()
{
  for (auto* handle : g_devices) {
    hid_close(handle);
  }
  g_devices.clear();
  g_device = nullptr;
}

inline bool isConnected() { return !g_devices.empty(); }

inline unsigned int deviceNum() { return g_devices.size(); }

inline bool select(const unsigned int index)
{
  if (index >= g_devices.size()) {
    std::cerr << "Unable to select device " << index << std::endl;
    return false;
  }

  g_device = g_devices[index];
  return true;
}

inline void printDevices()
{
  struct hid_device_info* hid_info;
  hid_info = hid_enumerate(g_vendorId, g_productId);
  std::cout << "[Device List]" << std::endl;
  while (hid_info) {
    if (hid_info->interface_number == 0) {
      std::cout << " path: " << hid_info->path << std::endl;
      // std::wcout << "  manufacturer: " << hid_info->manufacturer_string
      // << std::endl;
      // std::wcout << "  product: " << hid_info->product_string << std::endl;
      // std::wcout << "  S/N: " << hid_info->serial_number << std::endl;
    }
    hid_info = hid_info->next;
  }
}

inline Buffer read()
{
  if (!isConnected()) return nullptr;

  if (!g_device) {
    std::cerr << "Device not selected" << std::endl;
    return nullptr;
  }

  Buffer ret(new uint8_t[g_bufferSize]);
  int32_t readBytes =
      hid_read_timeout(g_device, ret.get(), g_bufferSize, g_readTimeout);

  if (readBytes == -1) {
    std::cerr << "USB Read failed" << std::endl;
    close();
    return nullptr;
  }

  return ret;
}

inline int32_t write(const Buffer& data)
{
  if (!isConnected()) return -1;

  if (!g_device) {
    std::cerr << "Device not selected" << std::endl;
    return -1;
  }

  int32_t writtenBytes = hid_write(g_device, data.get(), g_bufferSize);

  if (writtenBytes == -1) {
    std::cerr << "USB Write failed" << std::endl;
    close();
    return -1;
  }

  return writtenBytes;
}
};  // namespace USB
};  // namespace multi350
