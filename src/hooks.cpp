#include "PracticeMod.hpp"
#include "prime/CFinalInput.hpp"
#include "prime/CMain.hpp"
#include "prime/CMainFlow.hpp"
#include "prime/CPauseScreen.hpp"
#include "prime/CPlayer.hpp"
#include "prime/CStateManager.hpp"
#include "utils/ReplaceFunction.hpp"
#include <prime/CRandom16.hpp>
#include <settings.hpp>

// clang format loves to just stomp everything here in the least useful possible way
// clang-format off
DECLARE_FUNCTION_REPLACEMENT(CMain_DrawDebugMetrics) {
  static void Callback() {
    PracticeMod::GetInstance()->render();
    Orig();
  }
};

// CMainFlow::OnMessage
DECLARE_FUNCTION_REPLACEMENT(CMainFlow_OnMessage) {
  static CIOWin::EMessageReturn Callback(CMainFlow *thiz, const CArchitectureMessage &msg, CArchitectureQueue &queue) {
    CIOWin::EMessageReturn res = Orig(thiz, msg, queue);
    if (msg.x4_type == EArchMsgType_UserInput) {
      CArchMsgParmUserInput *status = (CArchMsgParmUserInput *)(msg.GetParm());
      CFinalInput *input = status->GetInput();
      // The mod 4 is just for safety
      memcpy(&PracticeMod::GetInstance()->inputs[input->ControllerIdx() % 4],
             input,
             sizeof(CFinalInput));
      if (input->ControllerIdx() == 0) {
        PracticeMod::GetInstance()->HandleInputs();
      }
    }
    return res;
  }
};

// CPlayer::ProcessInput
DECLARE_FUNCTION_REPLACEMENT(CPlayer_ProcessInput) {
  static void Callback(CPlayer *self, double dt, CFinalInput *input, CStateManager *mgr) {
    Orig(self, dt, input, mgr);
  }
};

DECLARE_FUNCTION_REPLACEMENT(CPauseScreen_ProcessInput) {
  static void Callback(CPauseScreen *self, CFinalInput &input) {
    if (self->GetState() != CPauseScreen::EPauseScreenState::EState_Active) {
      Orig(self, input);
      return;
    }

    PracticeMod::GetInstance()->pauseScreenOpened();
    if (input.PStart()) {
      PracticeMod::GetInstance()->pauseScreenClosed();
      Orig(self, input);
      return;
      // Play some noises too
      // CSfxManager::SfxStart(0x59A, 0x7F, 0x40, false, 0x7F, false, kInvalidAreaId.id);
      // self->StartTransition(0.5f, mgr, CPauseScreen::ESubScreen_ToGame, 2);
    } else if (input.PZ()) {
      PracticeMod::GetInstance()->menuActive = !PracticeMod::GetInstance()->menuActive;
    }
    if (!PracticeMod::GetInstance()->menuActive) {
      Orig(self, input);
    }
  }
};
// };

DECLARE_FUNCTION_REPLACEMENT(CRandom16_Next) {
  static s32 Callback(CRandom16* rng) {
    if (!SETTINGS.RNG_lockSeed) {
      rng->m_seed = (rng->m_seed * 0x41c64e6d) + 0x00003039;
    }
    return (rng->m_seed >> 16) & 0xffff;
  }
};

// clang-format on

void InstallHooks() {
  CMain_DrawDebugMetrics::InstallAtFuncPtr(&CMain::DrawDebugMetrics);
  CMainFlow_OnMessage::InstallAtFuncPtr(&CMainFlow::OnMessage);
  CPlayer_ProcessInput::InstallAtFuncPtr(&CPlayer::ProcessInput);
  CPauseScreen_ProcessInput::InstallAtFuncPtr(&CPauseScreen::ProcessInput);
  CRandom16_Next::InstallAtFuncPtr(&CRandom16::Next);
}
