// SPDX-FileCopyrightText: 2025 Kon Hyong Kim <konhyong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
#pragma once
#include <array>
#include <map>
#include <string>
#include <vector>

#include "al/ui/al_ParameterGUI.hpp"
#include "al/ui/al_PresetHandler.hpp"
#include "multi350/controller.hpp"

using namespace al;

/// @brief multi350 with GUI for use with allolib library.
/// https://github.com/AlloSphere-Research-Group/allolib
class Multi350GUI {
 public:
  /// @brief Initialize the gui and setup parameters and presets
  /// @return True on success
  bool init()
  {
    m_presetCurrents << m_proj0Red << m_proj0Green << m_proj0Blue << m_proj1Red
                     << m_proj1Green << m_proj1Blue << m_proj2Red
                     << m_proj2Green << m_proj2Blue << m_proj3Red
                     << m_proj3Green << m_proj3Blue;
    m_LEDpresets = m_presetCurrents.availablePresets();
    m_presetCurrents.recallPresetSynchronous("currents");
    m_LEDpresetIdx = m_presetCurrents.getCurrentPresetIndex();

    m_presetProjectors << m_proj0Index << m_proj1Index << m_proj2Index
                       << m_proj3Index;
    m_presetProjectors.recallPresetSynchronous("projectors");

    if (!controller.init()) {
      std::cerr << "Failed to initialize multi350 controller" << std::endl;
      return false;
    }

    setupProjectorIndices();
    setupPatternSequences();
    setupCallbacks();

    return true;
  }

  /// @brief Shutdown the controller
  void shutdown()
  {
    controller.close();
    controller.exit();
  }

  /// @brief Configure the imgui using parameters
  void configureGUI()
  {
    ImGui::Begin("MULTI350 Control");

    ParameterGUI::draw(&m_deviceList);
    ImGui::SameLine();
    ParameterGUI::draw(&m_deviceOpen);
    ImGui::SameLine();
    ParameterGUI::draw(&m_deviceClose);

    if (!m_usbNames.empty() &&
        ImGui::CollapsingHeader("Adjust Projector Order",
                                ImGuiTreeNodeFlags_CollapsingHeader)) {
      ImGui::Indent();

      for (int idx = 0; idx < m_usbNames.size(); ++idx) {
        ImGui::Text("Proj %d: ", idx);
        ImGui::SameLine();
        ImGui::Selectable(m_usbNames[idx].c_str(), true, 0, ImVec2(30, 15));

        if (ImGui::IsItemActive() && !ImGui::IsItemHovered()) {
          int next_idx = idx + (ImGui::GetMouseDragDelta(0).y < 0.f ? -1 : 1);
          if (next_idx >= 0 && next_idx < m_usbNames.size()) {
            std::string temp_name(m_usbNames[idx]);
            m_usbNames[idx] = m_usbNames[next_idx];
            m_usbNames[next_idx] = temp_name;
            int temp_idx(m_projsIndex[idx]);
            m_projsIndex[idx] = m_projsIndex[next_idx];
            m_projsIndex[next_idx] = temp_idx;
            ImGui::ResetMouseDragDelta();
          }
        }
      }

      ParameterGUI::draw(&m_applyIndices);
      ImGui::SameLine();
      ParameterGUI::draw(&m_saveIndices);

      ImGui::Unindent();

      m_proj0Index.setNoCalls(m_projsIndex[0]);
      m_proj1Index.setNoCalls(m_projsIndex[1]);
      m_proj2Index.setNoCalls(m_projsIndex[2]);
      m_proj3Index.setNoCalls(m_projsIndex[3]);
    }

    ImGui::NewLine();

    if (controller.isConnected()) {
      ImGui::TextColored(ImVec4(0.0f, 1.0f, 0.0f, 1.0f), "Connected: %u",
                         controller.deviceNum());
    }
    else {
      ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "Not Connected");
    }

    int device_num = controller.deviceNum();
    if (device_num < 4) {
      m_proj3Control.setHint("hide", true);
      if (device_num < 3) {
        m_proj2Control.setHint("hide", true);
        if (device_num < 2) {
          m_proj1Control.setHint("hide", true);
          if (device_num < 1) {
            m_proj0Control.setHint("hide", true);
          }
          else {
            m_proj0Control.setHint("hide", false);
          }
        }
        else {
          m_proj1Control.setHint("hide", false);
        }
      }
      else {
        m_proj2Control.setHint("hide", false);
      }
    }
    else {
      m_proj3Control.setHint("hide", false);
      m_proj2Control.setHint("hide", false);
      m_proj1Control.setHint("hide", false);
      m_proj0Control.setHint("hide", false);
    }

    ParameterGUI::draw(&m_proj0Control);
    ImGui::SameLine();
    ParameterGUI::draw(&m_proj1Control);
    ImGui::SameLine();
    ParameterGUI::draw(&m_proj2Control);
    ImGui::SameLine();
    ParameterGUI::draw(&m_proj3Control);

    ImGui::NewLine();

    ParameterGUI::draw(&m_testStart);
    ImGui::SameLine();
    ParameterGUI::draw(&m_testStop);

    ImGui::NewLine();

    ParameterGUI::draw(&m_powerNormal);
    ImGui::SameLine();
    ParameterGUI::draw(&m_powerStandby);
    ImGui::SameLine();
    ParameterGUI::draw(&m_reset);
    ImGui::SameLine();
    ParameterGUI::draw(&m_printStatus);

    ImGui::NewLine();

    ParameterGUI::draw(&m_videoMode);
    ParameterGUI::draw(&m_degammaTable);
    ImGui::SameLine();
    ParameterGUI::draw(&m_gammaCorrection);

    ImGui::RadioButton("8b", &m_patternSequenceIndex, 0);
    ImGui::SameLine();
    ImGui::RadioButton("7b", &m_patternSequenceIndex, 1);
    ImGui::SameLine();
    ImGui::RadioButton("4b", &m_patternSequenceIndex, 2);
    ImGui::SameLine();
    ImGui::RadioButton("2b", &m_patternSequenceIndex, 3);

    ParameterGUI::draw(&m_patternStart);
    ImGui::SameLine();
    ParameterGUI::draw(&m_varExpPatStart);
    ImGui::SameLine();
    ParameterGUI::draw(&m_patternStop);

    ImGui::NewLine();

    if (ImGui::BeginCombo("LED Presets", m_LEDpresets[m_LEDpresetIdx].c_str(),
                          ImGuiComboFlags_None)) {
      for (auto& preset : m_LEDpresets) {
        const bool isSelected = (m_LEDpresetIdx == preset.first);
        if (ImGui::Selectable(preset.second.c_str(), isSelected)) {
          m_LEDpresetIdx = preset.first;
        }

        if (isSelected) ImGui::SetItemDefaultFocus();
      }
      ImGui::EndCombo();
    }

    ParameterGUI::draw(&m_LEDload);
    ImGui::SameLine();
    ParameterGUI::draw(&m_LEDsave);
    ImGui::SameLine();
    ParameterGUI::draw(&m_LEDapply);

    if (ImGui::CollapsingHeader("LED Currents",
                                ImGuiTreeNodeFlags_CollapsingHeader)) {
      ImGui::Indent();
      ParameterGUI::draw(&m_proj0Red);
      ParameterGUI::draw(&m_proj0Green);
      ParameterGUI::draw(&m_proj0Blue);
      ImGui::NewLine();
      ParameterGUI::draw(&m_proj1Red);
      ParameterGUI::draw(&m_proj1Green);
      ParameterGUI::draw(&m_proj1Blue);
      ImGui::NewLine();
      ParameterGUI::draw(&m_proj2Red);
      ParameterGUI::draw(&m_proj2Green);
      ParameterGUI::draw(&m_proj2Blue);
      ImGui::NewLine();
      ParameterGUI::draw(&m_proj3Red);
      ParameterGUI::draw(&m_proj3Green);
      ParameterGUI::draw(&m_proj3Blue);

      ImGui::Unindent();
    }
    ImGui::End();
  }

  /// @brief Set up the parameter callbacks
  void setupCallbacks()
  {
    m_deviceList.registerChangeCallback(
        [&](float value) { controller.printDevices(); });

    m_deviceOpen.registerChangeCallback(
        [&](float value) { controller.open(); });

    m_reset.registerChangeCallback(
        [&](float value) { controller.softwareReset(); });

    m_printStatus.registerChangeCallback([&](float value) {
      controller.updateStatus();
      controller.printStatus();
    });

    m_testStart.registerChangeCallback([&](float value) {
      controller.startTestPattern(multi350::TestPattern::COLOR_BARS);
    });

    m_testStop.registerChangeCallback(
        [&](float value) { controller.stopTestPattern(); });

    m_LEDapply.registerChangeCallback([&](float value) {
      std::vector<multi350::LEDCurrent> currents;
      currents.emplace_back(static_cast<uint8_t>(m_proj0Red.get()),
                            static_cast<uint8_t>(m_proj0Green.get()),
                            static_cast<uint8_t>(m_proj0Blue.get()));
      currents.emplace_back(static_cast<uint8_t>(m_proj1Red.get()),
                            static_cast<uint8_t>(m_proj1Green.get()),
                            static_cast<uint8_t>(m_proj1Blue.get()));
      currents.emplace_back(static_cast<uint8_t>(m_proj2Red.get()),
                            static_cast<uint8_t>(m_proj2Green.get()),
                            static_cast<uint8_t>(m_proj2Blue.get()));
      currents.emplace_back(static_cast<uint8_t>(m_proj3Red.get()),
                            static_cast<uint8_t>(m_proj3Green.get()),
                            static_cast<uint8_t>(m_proj3Blue.get()));

      controller.setLEDCurrent(currents);
    });

    m_LEDsave.registerChangeCallback([&](float value) {
      m_presetCurrents.storePreset(m_LEDpresets[m_LEDpresetIdx]);
    });

    m_LEDload.registerChangeCallback([&](float value) {
      m_presetCurrents.recallPresetSynchronous(m_LEDpresets[m_LEDpresetIdx]);
    });

    m_varExpPatStart.registerChangeCallback([&](float value) {
      controller.startVarExpPatSequence(m_varExpPatSequences);
    });

    m_patternStart.registerChangeCallback([&](float value) {
      controller.startPatternSequence(
          m_patternSequences[m_patternSequenceIndex]);
    });

    m_patternStop.registerChangeCallback(
        [&](float value) { controller.stopPatternSequence(); });

    m_videoMode.registerChangeCallback(
        [&](float value) { controller.startVideoMode(); });

    m_degammaTable.registerChangeCallback([&](float value) {
      controller.setGammaCorrection(value, m_gammaCorrection.get());
    });

    m_gammaCorrection.registerChangeCallback([&](float value) {
      controller.setGammaCorrection(m_degammaTable.get(), value);
    });

    m_deviceClose.registerChangeCallback(
        [&](float value) { controller.close(); });

    m_proj0Control.registerChangeCallback([&](float value) {
      auto& projector = controller.getProjector(0);
      projector.controlled = value;
    });

    m_proj1Control.registerChangeCallback([&](float value) {
      auto& projector = controller.getProjector(1);
      projector.controlled = value;
    });

    m_proj2Control.registerChangeCallback([&](float value) {
      auto& projector = controller.getProjector(2);
      projector.controlled = value;
    });

    m_proj3Control.registerChangeCallback([&](float value) {
      auto& projector = controller.getProjector(3);
      projector.controlled = value;
    });

    m_powerNormal.registerChangeCallback([&](float value) {
      controller.setPowerMode(multi350::PowerMode::NORMAL);
    });

    m_powerStandby.registerChangeCallback([&](float value) {
      controller.setPowerMode(multi350::PowerMode::STANDBY);
    });

    m_applyIndices.registerChangeCallback(
        [&](float value) { controller.updateIndices(m_projsIndex); });

    m_saveIndices.registerChangeCallback(
        [&](float value) { m_presetProjectors.storePreset("projectors"); });
  }

  /// @brief Set up the preset pattern sequences
  void setupPatternSequences()
  {
    using namespace multi350;
    // TODO: check if insert black is needed on final pattern
    m_patternSequences[0].addPattern<Pattern::Pattern8bit>(
        Pattern::TriggerType::EXTERNAL_POSITIVE,
        Pattern::Pattern8bit::G7G6G5G4G3G2G1G0, 8, Pattern::LEDSelect::GREEN);
    m_patternSequences[0].addPattern<Pattern::Pattern8bit>(
        Pattern::TriggerType::NO_TRIGGER,
        Pattern::Pattern8bit::R7R6R5R4R3R2R1R0, 8, Pattern::LEDSelect::RED);
    m_patternSequences[0].addPattern<Pattern::Pattern8bit>(
        Pattern::TriggerType::NO_TRIGGER,
        Pattern::Pattern8bit::B7B6B5B4B3B2B1B0, 8, Pattern::LEDSelect::BLUE,
        false, true, false);
    m_patternSequences[0].setExposure(8333);
    m_patternSequences[0].setPeriod(8333);

    m_patternSequences[1].addPattern<Pattern::Pattern7bit>(
        Pattern::TriggerType::EXTERNAL_POSITIVE,
        Pattern::Pattern7bit::G7G6G5G4G3G2G1, 7, Pattern::LEDSelect::GREEN);
    m_patternSequences[1].addPattern<Pattern::Pattern7bit>(
        Pattern::TriggerType::NO_TRIGGER, Pattern::Pattern7bit::R7R6R5R4R3R2R1,
        7, Pattern::LEDSelect::RED);
    m_patternSequences[1].addPattern<Pattern::Pattern7bit>(
        Pattern::TriggerType::NO_TRIGGER, Pattern::Pattern7bit::B7B6B5B4B3B2B1,
        7, Pattern::LEDSelect::BLUE, false, true, false);
    m_patternSequences[1].setExposure(4500);
    m_patternSequences[1].setPeriod(4500);

    m_patternSequences[2].addPattern<Pattern::Pattern4bit>(
        Pattern::TriggerType::EXTERNAL_POSITIVE, Pattern::Pattern4bit::G7G6G5G4,
        4, Pattern::LEDSelect::GREEN);
    m_patternSequences[2].addPattern<Pattern::Pattern4bit>(
        Pattern::TriggerType::NO_TRIGGER, Pattern::Pattern4bit::R7R6R5R4, 4,
        Pattern::LEDSelect::RED);
    m_patternSequences[2].addPattern<Pattern::Pattern4bit>(
        Pattern::TriggerType::NO_TRIGGER, Pattern::Pattern4bit::B7B6B5B4, 4,
        Pattern::LEDSelect::BLUE, false, true, false);
    m_patternSequences[2].setExposure(1700);
    m_patternSequences[2].setPeriod(1700);

    m_patternSequences[3].addPattern<Pattern::Pattern2bit>(
        Pattern::TriggerType::EXTERNAL_POSITIVE, Pattern::Pattern2bit::G7G6, 2,
        Pattern::LEDSelect::GREEN);
    m_patternSequences[3].addPattern<Pattern::Pattern2bit>(
        Pattern::TriggerType::NO_TRIGGER, Pattern::Pattern2bit::R7R6, 2,
        Pattern::LEDSelect::RED);
    m_patternSequences[3].addPattern<Pattern::Pattern2bit>(
        Pattern::TriggerType::NO_TRIGGER, Pattern::Pattern2bit::B7B6, 2,
        Pattern::LEDSelect::BLUE, false, true, false);
    m_patternSequences[3].setExposure(700);
    m_patternSequences[3].setPeriod(700);

    m_varExpPatSequences.addVarExpPat<Pattern::Pattern4bit>(
        1700, 1700, Pattern::TriggerType::EXTERNAL_POSITIVE,
        Pattern::Pattern4bit::G7G6G5G4, 4, Pattern::LEDSelect::GREEN);
    m_varExpPatSequences.addVarExpPat<Pattern::Pattern4bit>(
        4250, 4250, Pattern::TriggerType::NO_TRIGGER,
        Pattern::Pattern4bit::R7R6R5R4, 4, Pattern::LEDSelect::RED);
    m_varExpPatSequences.addVarExpPat<Pattern::Pattern4bit>(
        1700, 1700, Pattern::TriggerType::NO_TRIGGER,
        Pattern::Pattern4bit::B7B6B5B4, 4, Pattern::LEDSelect::BLUE, false,
        true, false);
  }

  /// @brief Modify projector indices if order needs to be changed
  void setupProjectorIndices()
  {
    // // TODO: adjust for less than 4 projectors
    // if (multi350.deviceNum() != 4) {
    //   std::cout << "[Controller] Only " << multi350.deviceNum()
    //             << " projectors are connected. Using "
    //                "default values"
    //             << std::endl;
    //   return;
    // }

    m_projsIndex.push_back(m_proj0Index.get());
    m_projsIndex.push_back(m_proj1Index.get());
    m_projsIndex.push_back(m_proj2Index.get());
    m_projsIndex.push_back(m_proj3Index.get());

    for (int i = 0; i < 4; ++i) {
      m_usbNames.push_back("USB" + std::to_string(m_projsIndex[i]));
    }

    // multi350.updateIndices(m_projsIndex);
  }

 public:
  /// @brief Controller for multiple DLPC350 devices
  multi350::Controller controller;

 private:
  /// @brief Preset pattern sequences for 8, 7, 4, 2 bit depths
  std::array<multi350::PatternSequence, 4> m_patternSequences;
  /// @brief Index for the active pattern sequence
  int m_patternSequenceIndex{0};

  /// @brief Preset variable exposure sequence
  multi350::VarExpPatSequence m_varExpPatSequences;

  Trigger m_deviceList{"deviceList", "multi350"};
  Trigger m_deviceOpen{"deviceOpen", "multi350"};
  Trigger m_deviceClose{"deviceClose", "multi350"};

  ParameterInt m_proj0Index{"proj0_index", "multi350", 0, 0, 3};
  ParameterInt m_proj1Index{"proj1_index", "multi350", 1, 0, 3};
  ParameterInt m_proj2Index{"proj2_index", "multi350", 2, 0, 3};
  ParameterInt m_proj3Index{"proj3_index", "multi350", 3, 0, 3};
  std::vector<unsigned int> m_projsIndex;
  std::vector<std::string> m_usbNames;

  Trigger m_applyIndices{"applyIndex", "multi350"};
  Trigger m_saveIndices{"saveIndex", "multi350"};

  ParameterBool m_proj0Control{"proj0", "multi350", true};
  ParameterBool m_proj1Control{"proj1", "multi350", true};
  ParameterBool m_proj2Control{"proj2", "multi350", true};
  ParameterBool m_proj3Control{"proj3", "multi350", true};

  Trigger m_reset{"reset", "multi350"};
  Trigger m_printStatus{"status", "multi350"};

  Trigger m_testStart{"startTest", "multi350"};
  Trigger m_testStop{"stopTest", "multi350"};

  Trigger m_powerNormal{"normal", "multi350"};
  Trigger m_powerStandby{"standby", "multi350"};

  Trigger m_videoMode{"videoMode", "multi350"};
  Trigger m_varExpPatStart{"varExpPat", "multi350"};
  Trigger m_patternStart{"pattern", "multi350"};
  Trigger m_patternStop{"stop", "multi350"};

  ParameterBool m_degammaTable{"degammaTable", "multi350", false};
  ParameterBool m_gammaCorrection{"gammaCorrection", "multi350", true};

  Trigger m_LEDapply{"applyLED", "multi350"};
  Trigger m_LEDsave{"saveLED", "multi350"};
  Trigger m_LEDload{"loadLED", "multi350"};
  std::map<int, std::string> m_LEDpresets;
  int m_LEDpresetIdx{0};

  ParameterInt m_proj0Red{"proj0_red", "multi350", 40, 0, 255};
  ParameterInt m_proj0Green{"proj0_green", "multi350", 90, 0, 255};
  ParameterInt m_proj0Blue{"proj0_blue", "multi350", 255, 0, 255};
  ParameterInt m_proj1Red{"proj1_red", "multi350", 40, 0, 255};
  ParameterInt m_proj1Green{"proj1_green", "multi350", 90, 0, 255};
  ParameterInt m_proj1Blue{"proj1_blue", "multi350", 255, 0, 255};
  ParameterInt m_proj2Red{"proj2_red", "multi350", 40, 0, 255};
  ParameterInt m_proj2Green{"proj2_green", "multi350", 90, 0, 255};
  ParameterInt m_proj2Blue{"proj2_blue", "multi350", 255, 0, 255};
  ParameterInt m_proj3Red{"proj3_red", "multi350", 40, 0, 255};
  ParameterInt m_proj3Green{"proj3_green", "multi350", 90, 0, 255};
  ParameterInt m_proj3Blue{"proj3_blue", "multi350", 255, 0, 255};

  // TODO: change file to std::file to handle paths
  PresetHandler m_presetCurrents{"presets/currents"};
  PresetHandler m_presetProjectors{"presets/projectors"};
};