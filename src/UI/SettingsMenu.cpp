#include <imgui.h>
#include "ImHelpers.hpp"
#include "SettingsMenu.hpp"

#include <prime/CRandom16.hpp>

#include "settings.hpp"
#include "prime/CStateManager.hpp"
#include "DumpMemoryUI.hpp"


namespace GUI {
  void drawHex32Editor(const char *title, s32 *value);

  void drawSettingsMenu() {
    if (ImGui::TreeNode("Settings")) {
      if (ImGui::TreeNode("On-screen display")) {
        BITFIELD_CHECKBOX("Show", SETTINGS.OSD_show);
        BITFIELD_CHECKBOX("Pos", SETTINGS.OSD_showPos);
        ImGui::SameLine();
        BITFIELD_CHECKBOX("Velocity", SETTINGS.OSD_showVelocity);
        ImGui::SameLine();
        BITFIELD_CHECKBOX("Angular Velocity", SETTINGS.OSD_showRotationalVelocity);
        BITFIELD_CHECKBOX("Input", SETTINGS.OSD_showInput);
        ImGui::SameLine();
        BITFIELD_CHECKBOX("Time", SETTINGS.OSD_showIGT);
        // BITFIELD_CHECKBOX("Previous Room time", SETTINGS.OSD_showPreviousRoomTime);
        // BITFIELD_CHECKBOX("Current Room time", SETTINGS.OSD_showCurrentRoomTime);
        // BITFIELD_CHECKBOX("Room time: Use Loads", SETTINGS.OSD_roomTimeIsBasedOnLoadStart);
        BITFIELD_CHECKBOX("Frame time", SETTINGS.OSD_showFrameTime);
        // BITFIELD_CHECKBOX("Memory info", SETTINGS.OSD_showMemoryInfo);
        // ImGui::SameLine();
        // BITFIELD_CHECKBOX("Memory graph", SETTINGS.OSD_showMemoryGraph);
        // BITFIELD_CHECKBOX("Loads", SETTINGS.OSD_showLoads);

        // end in-game display
        ImGui::TreePop();
      }

      if (ImGui::TreeNode("Dump Ram")) {
        ImGui::TextColored(ImVec4(0.9, 0.1, 0.1, 1.0), "Warning! Full screen flashing lights!");
        ImGui::Text("This dumps the entirety of RAM to the\nscreen for decoding from a video recording.\nThis will take several minutes.");
        if (ImGui::Button("Dump Ram")) {
          startMemoryDump();
        }
        ImGui::TreePop();
      }

      // End settings menu
      ImGui::TreePop();
    }
    void drawHex32Editor(const char *title, s32 *value);
  }

  void drawHex32Editor(const char *title, s32 *value) {
    ImGui::Text("%s", title);
    // draw the 8 characters; current character inverted
    static int current_char = 0;
    for (int i = 0; i < 8; i++) {
      int value_at_i = (*value >> ((7 - i) * 4)) & 0xF;
      if (i > 0) ImGui::SameLine();
      ImGui::TextColored(
        i == current_char ? ImVec4(1, 0, 0, 1) : ImVec4(1, 1, 1, 1),
        "%x", value_at_i
      );
    }

    // a grid of 16 buttons, for 0 - F in hex
    for (int i = 0; i < 16; i++) {
      if (i % 4 != 0) ImGui::SameLine();
      char label[2];
      if (i < 10) {
        label[0] = '0' + i;
      } else {
        label[0] = 'A' + (i - 10);
      }
      label[1] = '\0';
      if (ImGui::Button(label)) {
        // clear the value at current_char
        *value &= ~(0xF << ((7 - current_char) * 4));
        // set the value at current_char to i
        *value |= i << ((7 - current_char) * 4);
        current_char = (current_char + 1) % 8;
      }
    }
    if (ImGui::Button("|##start")) {
      current_char = 0;
    }
    ImGui::SameLine();
    if (ImGui::Button("<")) {
      current_char--;
      if (current_char < 0) current_char = 7;
    }
    ImGui::SameLine();
    if (ImGui::Button(">")) {
      current_char++;
      if (current_char > 7) current_char = 0;
    }
    ImGui::SameLine();

    if (ImGui::Button("|##end")) {
      current_char = 7;
    }
  }
}
