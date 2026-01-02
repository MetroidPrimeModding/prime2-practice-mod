#include "PlayerMenu.hpp"
#include "WarpMenu.h"
#include "imgui.h"

namespace GUI {
  CTransform4f savedPos{CTransform4f::Identity()};
  CVector3f savedVelocity{};
  CVector3f savedAngularVelocity{};
  u32 savedWorldAssetID{0};
  u32 savedAreaAssetID{0};

  void drawPlayerMenu() {
    if (ImGui::TreeNode("Player")) {
      ImGui::Text("TODO");

      ImGui::TreePop();
    }
  }

  void loadPos() {
    // TODO
  }

  void savePos() {
    // TODO
  }
}

