#include "PracticeMod.hpp"
#include "prime/CFinalInput.hpp"
#include "prime/CGameState.hpp"
#include "prime/CGraphics.hpp"
#include "prime/CMain.hpp"
#include "prime/CMainFlow.hpp"
#include "prime/CStateManager.hpp"
#include "prime/CPlayer.hpp"
#include "utils/ReplaceFunction.hpp"
#include <os.h>
#include <prime/CRandom16.hpp>
#include <settings.hpp>

// clang format loves to just stomp everything here in the least useful possible way
// clang-format off


// CGraphics::EndScene
DECLARE_FUNCTION_REPLACEMENT(CGraphics_EndScene) {
  static void Callback() {
    // PracticeMod::GetInstance()->render();
    Orig();
  }
};

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
      for (int controller = 0; controller < 4; ++controller) {
        memcpy(PracticeMod::GetInstance()->inputs + controller,
               &g_CStateManager.GetFinalInput()[controller],
               sizeof(CFinalInput));
      }
      PracticeMod::GetInstance()->HandleInputs();
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

// CStateManager::Update
// TODO
// DECLARE_FUNCTION_REPLACEMENT(CStateManager_Update) {
//   static void Callback(CStateManager *self, float dt) {
//     Orig(self, dt);
//     PracticeMod::GetInstance()->update(dt);
//   }
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
  CGraphics_EndScene::InstallAtFuncPtr(&CGraphics::EndScene);
  CMain_DrawDebugMetrics::InstallAtFuncPtr(&CMain::DrawDebugMetrics);
  CMainFlow_OnMessage::InstallAtFuncPtr(&CMainFlow::OnMessage);
  CPlayer_ProcessInput::InstallAtFuncPtr(&CPlayer::ProcessInput);
  // TODO
  //CStateManager_Update::InstallAtFuncPtr(&CStateManager::Update);
  CRandom16_Next::InstallAtFuncPtr(&CRandom16::Next);
}
