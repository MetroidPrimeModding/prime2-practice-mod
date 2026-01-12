#include "PracticeMod.hpp"
#include "UI/DumpMemoryUI.hpp"
#include "UI/InventoryMenu.hpp"
#include "UI/MonitorWindow.hpp"
#include "UI/PlayerMenu.hpp"
#include "UI/QR.hpp"
#include "UI/SettingsMenu.hpp"
#include "UI/WarpMenu.h"
#include "imgui.h"
#include "imgui_internal.h"
#include "os.h"
#include "prime/CGameState.hpp"
#include "prime/CMain.hpp"
#include "prime/CWorld.hpp"
#include "prime/CWorldState.hpp"
#include "settings.hpp"
#include "stb_sprintf.h"
#include "system/ImGuiEngine.hpp"
#include "version.h"

#define PAD_MAX_CONTROLLERS 4

PracticeMod::PracticeMod() {
  // This will cause the crash screen to appear every time
  *((u32 *) 0x8028c604) = 0x60000000;

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
void PracticeMod::handlePosLoadIfNeeded(CPlayer *player) {
  GUI::handlePosIfNeeded(player);
}

void PracticeMod::HandleInputs() {
  if (this->pauseScreenActive && this->menuActive) {
    ImGuiIO *io = &ImGui::GetIO();
    ImGui::GetCurrentContext()->NavInputSource = ImGuiInputSource_Gamepad;
    io->BackendFlags |= ImGuiBackendFlags_HasGamepad;
    io->AddKeyEvent(ImGuiKey_GamepadFaceDown, inputs[0].DA());
    io->AddKeyEvent(ImGuiKey_GamepadFaceRight, inputs[0].DB());
    io->AddKeyEvent(ImGuiKey_GamepadFaceLeft, inputs[0].DX());
    io->AddKeyEvent(ImGuiKey_GamepadFaceUp, inputs[0].DY());
    io->AddKeyEvent(ImGuiKey_GamepadR1, inputs[0].DRTrigger());
    io->AddKeyEvent(ImGuiKey_GamepadL1, inputs[0].DLTrigger());
    io->AddKeyAnalogEvent(ImGuiKey_GamepadL2, inputs[0].ALTrigger() > 0.0, inputs[0].ALTrigger());
    io->AddKeyAnalogEvent(ImGuiKey_GamepadR2, inputs[0].ARTrigger() > 0.0, inputs[0].ARTrigger());

    // dpad
    io->AddKeyEvent(ImGuiKey_GamepadDpadLeft, inputs[0].DDPLeft() || inputs[0].DLALeft());
    io->AddKeyEvent(ImGuiKey_GamepadDpadRight, inputs[0].DDPRight() || inputs[0].DLARight());
    io->AddKeyEvent(ImGuiKey_GamepadDpadUp, inputs[0].DDPUp() || inputs[0].DLAUp());
    io->AddKeyEvent(ImGuiKey_GamepadDpadDown, inputs[0].DDPDown() || inputs[0].DLADown());
    // analog move window
    io->AddKeyAnalogEvent(ImGuiKey_GamepadLStickLeft, inputs[0].ALALeft() > 0.0, inputs[0].ALALeft());
    io->AddKeyAnalogEvent(ImGuiKey_GamepadLStickRight, inputs[0].ALARight() > 0.0, inputs[0].ALARight());
    io->AddKeyAnalogEvent(ImGuiKey_GamepadLStickUp, inputs[0].ALAUp() > 0.0, inputs[0].ALAUp());
    io->AddKeyAnalogEvent(ImGuiKey_GamepadLStickDown, inputs[0].ALADown() > 0.0, inputs[0].ALADown());

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
    if (downPresses >= 3) {
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
  bool renderMenu = this->pauseScreenActive && this->menuActive;
  if (renderMenu) {
    ImGui::SetNextWindowPos(ImVec2(20, 20), ImGuiCond_Always, ImVec2(0, 0));
  } else {
    // Hack: if we're not rendering, just move off-screen so that it doesn't show up on-screen
    // This allows it to persist selected item state
    ImGui::SetNextWindowPos(ImVec2(1000, 20), ImGuiCond_Always, ImVec2(0, 0));
  }
  if (renderMenu && !wasRenderingLastFrame) {
    ImGui::SetNextWindowFocus();
  }
  wasRenderingLastFrame = renderMenu;
  ImGui::SetNextWindowCollapsed(!renderMenu, ImGuiCond_Always);
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
        GUI::drawQRCode("https://github.com/MetroidPrimeModding/prime2-practice-mod/releases", 3.0f);
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

void warp(uint32_t world, uint32_t area) {
  OSReport("Warping to %x, %x\n", world, area);
  CAssetId worldID = world;
  CAssetId areaID = area;

  CStateManager *mgr = &g_CStateManager;
  mgr->GetWorld()->SetLoadPauseState(true);

  CGameState *gameState = gpGameState;
  // CSfxManager::SfxStart(0x59A, 0x7F, 0x40, false, 0x7F, false, kInvalidAreaId.id);
  gameState->SetCurrentWorldId(worldID);
  gameState->CurrentWorldState().SetDesiredAreaAssetId(areaID);

  gpMain->SetFlowState(EFlowState_None);

  mgr->SetShouldQuitGame(true);

  PracticeMod::GetInstance()->pauseScreenClosed();
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
