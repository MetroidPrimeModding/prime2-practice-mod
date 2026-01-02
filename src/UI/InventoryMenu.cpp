#include <imgui.h>
#include "InventoryMenu.hpp"

namespace GUI {
  /*void RenderItemType(CPlayerState *playerState, CPlayerState::EItemType itemType);
  const char *ItemTypeToName(CPlayerState::EItemType type);
  static inline void RenderItemsDualColumn(CPlayerState *playerState, const CPlayerState::EItemType *items, int start, int end);

  void clearItems(CPlayerState *playerState);

  void refillItems(CPlayerState *playerState);*/

  void drawInventoryMenu() {
    if (ImGui::TreeNode("Inventory")) {
      ImGui::Text("TODO");
    }
  }

}
