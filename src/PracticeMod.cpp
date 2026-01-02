#include "PracticeMod.hpp"
#include "UI/DumpMemoryUI.hpp"
#include "UI/InventoryMenu.hpp"
#include "UI/MonitorWindow.hpp"
#include "UI/PlayerMenu.hpp"
#include "UI/QR.hpp"
#include "UI/SettingsMenu.hpp"
#include "UI/WarpMenu.h"
#include "imgui.h"
#include "os.h"
#include "prime/CGameState.hpp"
#include "prime/CMain.hpp"
#include "settings.hpp"
#include "system/ImGuiEngine.hpp"
#include "version.h"
#include "stb_sprintf.h"

#define PAD_MAX_CONTROLLERS 4

PracticeMod::PracticeMod() {
  ImGuiEngine::ImGui_Init();
  ImGuiEngine::ImGui_Init_Style();
  GUI::initQR();
  this->pauseScreenClosed();
}

void PracticeMod::render() {
  this->renderMenu();
}

void PracticeMod::pauseScreenClosed() {
  pauseScreenActive = false;
}

void PracticeMod::pauseScreenOpened() {
  this->pauseScreenActive = true;
}

void PracticeMod::HandleInputs() {
  if (this->pauseScreenActive && this->menuActive) {
    ImGuiIO *io = &ImGui::GetIO();
    io->NavInputs[ImGuiNavInput_Activate] = inputs[0].DA();
    io->NavInputs[ImGuiNavInput_Cancel] = inputs[0].DB();
//    io->NavInputs[ImGuiNavInput_Menu] = inputs[0].DX();
//    io->NavInputs[ImGuiNavInput_Input] = inputs[0].DY();

    // dpad
    io->NavInputs[ImGuiNavInput_DpadLeft] = inputs[0].DDPLeft() || inputs[0].DLALeft();
    io->NavInputs[ImGuiNavInput_DpadRight] = inputs[0].DDPRight() || inputs[0].DLARight();
    io->NavInputs[ImGuiNavInput_DpadUp] = inputs[0].DDPUp() || inputs[0].DLAUp();
    io->NavInputs[ImGuiNavInput_DpadDown] = inputs[0].DDPDown() || inputs[0].DLADown();
    // analog
    io->NavInputs[ImGuiNavInput_LStickLeft] = inputs[0].ARALeft();
    io->NavInputs[ImGuiNavInput_LStickRight] = inputs[0].ARARight();
    io->NavInputs[ImGuiNavInput_LStickUp] = inputs[0].ARAUp();
    io->NavInputs[ImGuiNavInput_LStickDown] = inputs[0].ARADown();


    /*
    MAP_BUTTON(ImGuiNavInput_FocusPrev,     SDL_CONTROLLER_BUTTON_LEFTSHOULDER);    // L1 / LB
    MAP_BUTTON(ImGuiNavInput_FocusNext,     SDL_CONTROLLER_BUTTON_RIGHTSHOULDER);   // R1 / RB
    MAP_BUTTON(ImGuiNavInput_TweakSlow,     SDL_CONTROLLER_BUTTON_LEFTSHOULDER);    // L1 / LB
    MAP_BUTTON(ImGuiNavInput_TweakFast,     SDL_CONTROLLER_BUTTON_RIGHTSHOULDER);   // R1 / RB
     */
  } else if (!this->pauseScreenActive) {
    //  warp hotkeys
    if (hotkeyInputTimeout <= 0) {
      upPresses = 0;
      downPresses = 0;
    } else {
      hotkeyInputTimeout -= 1.f / 60.f;
    }

    if (inputs[0].PDPUp()) {
      upPresses++;
      if (hotkeyInputTimeout <= 0) hotkeyInputTimeout = 2.0f;
    }
    if (inputs[0].PDPDown()) {
      downPresses++;
      if (hotkeyInputTimeout <= 0) hotkeyInputTimeout = 2.0f;
    }

    if (upPresses >= 3) {
      GUI::loadPos();
      hotkeyInputTimeout = 0;
    }
    if (downPresses >= 2) {
      GUI::savePos();
      hotkeyInputTimeout = 0;
    }
  }
}

void PracticeMod::renderMenu() {
  ImGuiIO &io = ImGui::GetIO();
  io.DisplaySize = ImVec2(SVIEWPORT_GLOBAL->x8_width, SVIEWPORT_GLOBAL->xc_height);
  io.DeltaTime = 1.f / 60.f;

  ImGui::NewFrame();
  GUI::qrNewFrame();
  bool render = this->pauseScreenActive && this->menuActive;
  render = true; // TODO
  if (render) {
    ImGui::SetNextWindowPos(ImVec2(20, 20), ImGuiCond_Always, ImVec2(0, 0));
  } else {
    // Hack: if we're not rendering, just move off-screen so that it doesn't show up on-screen
    // This allows it to persist selected item state
    ImGui::SetNextWindowPos(ImVec2(1000, 20), ImGuiCond_Always, ImVec2(0, 0));
  }
  ImGui::SetNextWindowFocus();
  ImGui::SetNextWindowCollapsed(!render, ImGuiCond_Always);
  ImGui::SetNextWindowSizeConstraints(
      ImVec2(0, 0),
      ImVec2(400, SVIEWPORT_GLOBAL->xc_height - 40)
  );
  if (ImGui::Begin(
      "Practice Mod", nullptr,
      ImGuiWindowFlags_AlwaysAutoResize
      | ImGuiWindowFlags_NoResize
      | ImGuiWindowFlags_NoMove
  )) {
    GUI::drawPlayerMenu();
    GUI::drawInventoryMenu();
    GUI::drawSettingsMenu();
    GUI::drawWarpMenu();
    if (ImGui::TreeNode("v%s", PRAC_MOD_VERSION)) {
      ImGui::Text("Links (QR codes):");
      if (ImGui::TreeNode("Releases")) {
        GUI::drawQRCode("https://github.com/MetroidPrimeModding/prime-practice-native/releases", 3.0f);
        ImGui::TreePop();
      }
      if (ImGui::TreeNode("Discord")) {
        GUI::drawQRCode("https://discord.gg/m4UreBdq9V", 3.0f);
        ImGui::TreePop();
      }
      ImGui::TreePop();
    }
  }
  ImGui::End();

  GUI::drawMonitorWindow(inputs);

  if (mapActive) {
    ImGui::SetNextWindowPos(ImVec2(320, 360), ImGuiCond_Always, ImVec2(0.5, 1));
    ImGui::Begin(
        "Hint", nullptr,
        ImGuiWindowFlags_NoResize |
        ImGuiWindowFlags_AlwaysAutoResize |
        ImGuiWindowFlags_NoTitleBar |
        ImGuiWindowFlags_NoInputs |
        ImGuiWindowFlags_NoNavInputs |
        ImGuiWindowFlags_NoNavFocus |
        ImGuiWindowFlags_NoNav |
        ImGuiWindowFlags_NoFocusOnAppearing |
        ImGuiWindowFlags_NoMove |
        ImGuiFocusedFlags_None // just for conveneint commenting in/out
    );
    ImGui::Text("Press X to warp");
    ImGui::End();
  }

  GUI::drawMemoryDump();

  ImGui::Render();
  ImDrawData *drawData = ImGui::GetDrawData();
  ImGuiEngine::ImGui_Render(drawData);
}

float bombTime = 0;

void PracticeMod::update(float dt) const {
  // Lagger (loops)
  if (!this->pauseScreenActive) {
    if (SETTINGS.LAG_loop_iterations > 0) {
      int c = 1;
      for (int i = 1; i < SETTINGS.LAG_loop_iterations; i++) {
        for (int j = 1; j < 100; j++) {
          c = (c + 1 + i + j) * c - j;
        }
      }
    }
  }
}

void warp(uint32_t world, uint32_t area) {
  // TODO
  // OSReport("Warping to %x, %x\n", world, area);
  // CAssetId worldID = (CAssetId) (world);
  // CAssetId areaID = (CAssetId) (area);
  //
  // CStateManager *mgr = CStateManager::instance();
  // mgr->GetWorld()->SetPauseState(true);
  //
  // CGameState *gameState = *((CGameState **) (0x80457798 + 0x134));
  // CSfxManager::SfxStart(0x59A, 0x7F, 0x40, false, 0x7F, false, kInvalidAreaId.id);
  // gameState->SetCurrentWorldId(worldID);
  // gameState->CurrentWorldState().SetDesiredAreaAssetId(areaID);
  //
  // CMain *cmain = *((CMain **) 0x805A8C38);
  // cmain->SetFlowState(EFlowState_None);
  //
  // mgr->SetShouldQuitGame(true);
  //
  // PracticeMod::GetInstance()->pauseScreenClosed();
}

PracticeMod *pracModInstance = nullptr;
PracticeMod *PracticeMod::GetInstance() {
  if (!pracModInstance) {
    OSReport("Initializing practice mod\n");
    pracModInstance = new PracticeMod();
  }
  return pracModInstance;
}
void PracticeMod::ClearInstance() {
  if (pracModInstance) {
    // delete pracModInstance;
    pracModInstance = nullptr;
  }
}
