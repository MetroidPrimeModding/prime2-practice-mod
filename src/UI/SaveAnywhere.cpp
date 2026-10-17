#include "system/SaveLocation.hpp"
#include "SaveAnywhere.hpp"
#include "imgui.h"
#include "prime/CCameraManager.hpp"
#include "prime/CStateManager.hpp"
#include "system/CardGate.hpp"

namespace SaveAnywhere {
  namespace {
    enum class State { Idle, Closing, Waiting };
    State state = State::Idle;
    int frames = 0;
    u64 serial = 0;
    CPlayer *requestPlayer = nullptr;
    const char *status = "";
  } // namespace

  const char *whyNot() {
    auto &mgr = g_CStateManager;
    if (!gpGameState || !mgr.FullyInitialized() || !mgr.GetWorld() || !mgr.GetPlayer()) return "No game in progress";
    if (mgr.IsMultiplayer()) return "Multiplayer is not supported";
    if (!gpGameState->CardSerial()) return "No memory card associated with this game";
    if (CardGate::busy() || CardGate::saveRequested()) return "Settings card operation pending";
    auto *playerState = *GetField<CPlayerState *>(&mgr, 0x150c);
    if (!playerState || !(*GetField<u8>(playerState, 4) & 0x80) || !(playerState->GetHealthInfo()->GetHealth() > 0.f) ||
        *GetField<float>(mgr.GetPlayer(), 0x12a4) > 0.f)
      return "Samus is dead";
    if (mgr.GameState() != 0) return "Gameplay is paused by a scan or script";
    if (mgr.UnsafeSaveTransition()) return "World or portal transition in progress";
    if (!mgr.SaveCamera() || mgr.SaveCamera()->IsInCinematicCamera()) return "A cutscene is playing";
    if (mgr.GetEscapeSequenceTimer() > 0.f) return "Escape sequence in progress";
    if (mgr.GetPlayer()->IsMorphBallTransitioning()) return "Samus is morphing";
    auto transition = mgr.DeferredTransition();
    if (transition != CStateManager::Transition::InGame && transition != CStateManager::Transition::Pause &&
        transition != CStateManager::Transition::PauseMenu && transition != CStateManager::Transition::LogBook)
      return "Another screen transition is pending";
    return nullptr;
  }

  void cancel() {
    state = State::Idle;
    status = "";
  }

  void draw() {
    const char *reason = whyNot();
    if (ImGui::Button("Save game") && !reason && state == State::Idle) {
      state = State::Closing;
      frames = 0;
      serial = gpGameState->CardSerial();
      requestPlayer = g_CStateManager.GetPlayer();
      status = "Waiting for pause to close";
    }
    if (state != State::Idle && ImGui::Button("Cancel save request")) cancel();
    ImGui::TextWrapped("%s", reason ? reason : status);
    if (SaveLocation::message()) ImGui::TextWrapped("%s", SaveLocation::message());
  }

  bool consumeCloseRequest() {
    if (state != State::Closing) return false;
    state = State::Waiting;
    return true;
  }

  void tick() {
    if (state != State::Idle && ++frames >= 300) {
      cancel();
      status = "Save request timed out";
    }
  }

  void update() {
    if (state != State::Waiting) return;
    auto &mgr = g_CStateManager;
    if (mgr.DeferredTransition() != CStateManager::Transition::InGame) return;
    state = State::Idle;
    const char *reason = whyNot();
    if (reason) {
      status = reason;
      return;
    }
    if (gpGameState->CardSerial() != serial || mgr.GetPlayer() != requestPlayer) {
      status = "Game changed; save request canceled";
      return;
    }
    SaveLocation::beginSave();
    mgr.DeferStateTransition(CStateManager::Transition::Save);
    status = "Save screen requested";
  }
} // namespace SaveAnywhere
