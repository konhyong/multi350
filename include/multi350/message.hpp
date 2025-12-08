// SPDX-FileCopyrightText: 2025 Kon Hyong Kim <konhyong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
#pragma once
#include <algorithm>
#include <iomanip>
#include <iostream>
#include <utility>

#include "usb.hpp"

namespace multi350 {
/// @brief Internal settings for USB messages
namespace internal {
/// @brief Maximum data size for each command in bytes
inline constexpr size_t g_maxMessageDataSize = 512;
/// @brief Enable verbose logging for debug purposes
inline constexpr bool g_verbose = false;
};  // namespace internal

/// @brief Message data handler
struct Message {
  enum class Type : bool { WRITE = 0, READ = 1 };

  Message() : flags{0, 0, 0, 1, Type::WRITE}, sequence{0}, length{0}, data{0} {}

  /// @brief Constructs message with/without data bytes
  /// @tparam ...ParamList Type of data in data bytes if present
  /// @param _type Write/Read
  /// @param cmd Two bytes CMD2 + CMD3 indicating command.
  /// @param ...params Contents of data bytes if present
  template <typename... ParamList>
  Message(const Type _type, const uint16_t cmd, const ParamList&&... params)
      : flags{0, 0, 0, 1, _type}, sequence{0}, length{2}, data{0}
  {
    command = cmd;
    if (sizeof...(params) > 0) {
      addData(std::forward<const ParamList>(params)...);
    }
  }

  /// @brief Recursively adds data to message
  inline void addData() {}

  /// @brief Adding data to the message
  /// @tparam T Type of data
  /// @param first Content of data
  template <typename T>
  inline void addData(const T first)
  {
    T* ptr = reinterpret_cast<T*>(&data[length]);
    *ptr = first;
    length += sizeof(first);
  }

  /// @brief Recursively adds data to message
  /// @tparam T Type of first data
  /// @tparam ...ParamList Types of following data
  /// @param first Content of first data
  /// @param ...params Contents of following data
  template <typename T, typename... ParamList>
  inline void addData(const T first, const ParamList&&... params)
  {
    addData(first);
    if (sizeof...(params) > 0) {
      addData(std::forward<const ParamList>(params)...);
    }
  }

  /// @brief Flags byte
  struct {
    uint8_t destination : 3;
    uint8_t reserved    : 2;
    bool error          : 1;
    bool reply          : 1;
    Type rw             : 1;
  } flags;

  /// @brief Sequence byte if message is larger than 64 bytes and gets sent as
  /// multiple USB packets
  uint8_t sequence;
  /// @brief Length of data packet. Denotes total number of command bytes + data
  /// bytes
  uint16_t length;

  union {
    /// @brief Command bytes (CMD2 + CMD3)
    uint16_t command;
    /// @brief Array containing data packet
    uint8_t data[internal::g_maxMessageDataSize];
  };
};

/// @brief Read the usb packet for the message
/// @return Message data
extern inline std::unique_ptr<Message> read()
{
  auto received = USB::read();

  if (received == nullptr) {
    std::cerr << "Message Read failed" << std::endl;
    return nullptr;
  }

  auto ret =
      std::unique_ptr<Message>(new Message(), std::default_delete<Message>());
  memcpy(ret.get(), received.get(), USB::g_bufferSize);

  return ret;
}

/// @brief Write USB packet containing message
/// @param msg Message data
/// @return Number of bytes written
extern inline int32_t write(const Message& msg)
{
  uint16_t headerBytes =
      sizeof(msg.flags) + sizeof(msg.sequence) + sizeof(msg.length);
  uint16_t maxDataSize = USB::g_packetSize - headerBytes;
  uint16_t totalWrittenBytes = 0;
  uint16_t writtenBytes = std::min(msg.length, maxDataSize);

  USB::Buffer buffer(new uint8_t[USB::g_bufferSize]);
  memset(buffer.get(), 0, sizeof(uint8_t) * USB::g_bufferSize);
  memcpy(buffer.get() + 1, &msg,
         sizeof(uint8_t) * (headerBytes + writtenBytes));
  if (USB::write(buffer) == -1) {
    std::cerr << "Message write failed" << std::endl;
    return -1;
  }

  totalWrittenBytes += writtenBytes;

  while (totalWrittenBytes < msg.length) {
    writtenBytes =
        std::min(static_cast<uint16_t>(USB::g_packetSize),
                 static_cast<uint16_t>(msg.length - totalWrittenBytes));
    memcpy(buffer.get() + 1, &msg.data[totalWrittenBytes],
           sizeof(uint8_t) * writtenBytes);
    if (USB::write(buffer) == -1) {
      std::cerr << "Message write failed" << std::endl;
      return -1;
    }
    totalWrittenBytes += writtenBytes;
  }

  return totalWrittenBytes + headerBytes;
}

template <typename T>
using MessageData = std::unique_ptr<T, std::default_delete<T[]>>;

/// @brief Sends message to DLPC350 and gets the reply
/// @tparam T Type of data expected to return
/// @param msg Message data to send
/// @return Data packet of the reply
template <typename T = uint8_t>
extern MessageData<T> transact(const Message& msg)
{
  int32_t result = write(msg);

  if (internal::g_verbose) {
    std::cout << "W(" << msg.length << "): ";
    for (int i = 0; i < msg.length; ++i) {
      std::cout << std::hex << std::setw(4)
                << static_cast<unsigned int>(msg.data[i]) << " ";
    }
    std::cout << std::endl;
  }

  if (msg.flags.reply) {
    if (result <= 0) {
      std::cerr << "Failed to send message" << std::endl;
      return nullptr;
    }

    auto received = read();

    if (received == nullptr) {
      std::cerr << "Failed to receive proper reply" << std::endl;
      return nullptr;
    }
    if (received->flags.error ||
        (received->flags.rw == Message::Type::READ && received->length == 0)) {
      std::cerr << "Reply is empty/erroneous" << std::endl;
      return nullptr;
    }

    MessageData<T> ret(new T[USB::g_packetSize]);
    memcpy(ret.get(), received->data, USB::g_packetSize);

    if (internal::g_verbose) {
      std::cout << "R(" << received->length << "): ";
      for (int i = 0; i < received->length; ++i) {
        std::cout << std::hex << std::setw(4)
                  << static_cast<unsigned int>(received->data[i]) << " ";
      }
      std::cout << std::endl;
    }

    return ret;
  }

  std::cerr << "Message set to no ack. Use transactNoAck(Msg)." << std::endl;

  return nullptr;
}

/// @brief Send command to get data from DLPC350
/// @tparam T Type of expected data
/// @param cmd Command to send
/// @return Data from DLPC350
template <typename T = uint8_t>
extern inline MessageData<T> sendGetMessage(const uint16_t cmd)
{
  auto send = Message(Message::Type::READ, cmd);
  return transact<T>(send);
}

/// @brief Send command to set data on DLPC350
/// @tparam ...ParamList Type of data to send
/// @param cmd Command to send
/// @param ...params Data to send
/// @return Response from DLPC350
template <typename... ParamList>
extern inline MessageData<uint8_t> sendSetMessage(const uint16_t cmd,
                                                  const ParamList&&... params)
{
  auto send = Message(Message::Type::WRITE, cmd,
                      std::forward<const ParamList>(params)...);
  return transact<uint8_t>(send);
}

/// @brief Send message to DLPC350 with no acknowledge expected
/// @tparam ...ParamList Type of data to send
/// @param cmd Command to send
/// @param ...params Contents of data packet to send
/// @return Number of bytes written
template <typename... ParamList>
extern inline int32_t sendNoAckMessage(const uint16_t cmd,
                                       const ParamList&&... params)
{
  auto send = Message(Message::Type::WRITE, cmd,
                      std::forward<const ParamList>(params)...);
  send.flags.reply = false;

  return write(send);
}
};  // namespace multi350