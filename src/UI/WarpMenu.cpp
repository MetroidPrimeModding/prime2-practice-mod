#include <imgui.h>
#include "WarpMenu.h"

const WarpWorld WARP_WORLDS[] = {
};

namespace GUI {
  void drawWarpMenu() {
    if (ImGui::TreeNode("Warps")) {
      ImGui::Text("TODO");

      ImGui::TreePop();
    }
  }
}

const char* getNameForWorldAsset(u32 world) {
  for (auto &warpWorld : WARP_WORLDS) {
    if (warpWorld.world == world) {
      return warpWorld.name;
    }
  }
  return "Unknown";
}

const char* getNameForAreaAsset(u32 world, u32 area) {
  for (auto &warpWorld : WARP_WORLDS) {
    if (warpWorld.world == world) {
      for (int i = 0; i < warpWorld.areaCount; i++) {
        auto &warpArea = warpWorld.areas[i];
        if (warpArea.area == area) {
          return warpArea.name;
        }
      }
    }
  }
  return "Unknown";
}
