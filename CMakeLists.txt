# SPDX-FileCopyrightText: 2026 Kon Hyong Kim <konhyong@gmail.com>
# SPDX-License-Identifier: BSD-3-Clause
cmake_minimum_required(VERSION 3.30)

project(multi350)

list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)
include(NoInSourceBuilds)
include(SetIPO)

option(
  MULTI350_BUILD_TESTS
  "Enable building tests for multi350"
  ${PROJECT_IS_TOP_LEVEL}
)

add_library(multi350 STATIC)
add_library(multi350::multi350 ALIAS multi350)

target_sources(
  multi350
  PUBLIC
    FILE_SET HEADERS
      BASE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/include
      BASE_DIRS ${CMAKE_CURRENT_BINARY_DIR}/include
      FILES
        ${CMAKE_CURRENT_SOURCE_DIR}/include/multi350/controller.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/include/multi350/dlpc350.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/include/multi350/message.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/include/multi350/pattern.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/include/multi350/status.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/include/multi350/usb.hpp
)

target_sources(
  multi350
  PRIVATE src/controller.cpp src/dlpc350.cpp src/status.cpp src/usb.cpp
)

target_include_directories(
  multi350
  PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/include>
    $<INSTALL_INTERFACE:include>
)

include(GetHidapi)
target_link_libraries(multi350 PUBLIC hidapi::hidapi)

set_target_properties(
  multi350
  PROPERTIES
    CXX_STANDARD 20
    CXX_STANDARD_REQUIRED ON
    CXX_EXTENSIONS OFF
    VERIFY_INTERFACE_HEADER_SETS ON
)

# install(
#   TARGETS multi350
#   EXPORT multi350_targets
#   LIBRARY DESTINATION lib
#   ARCHIVE DESTINATION lib
#   RUNTIME DESTINATION bin
#   INCLUDES DESTINATION include
# )

# install(
#   EXPORT multi350_targets
#   FILE multi350_targets.cmake
#   NAMESPACE multi350::
#   DESTINATION lib/cmake/multi350
# )

# if(MULTI350_BUILD_TESTS)
#   enable_testing()
#   add_subdirectory(tests/multi350)
# endif()
