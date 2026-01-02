#include "PracticeMod.hpp"
#include "prime/CFinalInput.hpp"
#include "prime/CGameState.hpp"
#include "prime/CGraphics.hpp"
#include "prime/CMainFlow.hpp"
#include "prime/CStateManager.hpp"
#include "utils/ReplaceFunction.hpp"
#include <os.h>
#include <prime/CRandom16.hpp>
#include <settings.hpp>

// clang format loves to just stomp everything here in the least useful possible way
// clang-format off


// CGraphics::EndScene
DECLARE_FUNCTION_REPLACEMENT(CGraphics_EndScene) {
  static void Callback() {
    PracticeMod::GetInstance()->render();
    Orig();
  }
};

// CMainFlow::OnMessage
DECLARE_FUNCTION_REPLACEMENT(CMainFlow_OnMessage) {
  static CIOWin::EMessageReturn Callback(CMainFlow *thiz, const CArchitectureMessage &msg, CArchitectureQueue &queue) {
    if (msg.x4_type == EArchMsgType_UserInput) {
      CArchMsgParmUserInput *status = (CArchMsgParmUserInput *)msg.x8_parm.RawPointer();
      // The mod 4 is just for safety
      PracticeMod::GetInstance()->inputs[status->x4_parm.ControllerIdx() % 4] = status->x4_parm;
      if (status->x4_parm.ControllerIdx() == 0) {
        PracticeMod::GetInstance()->HandleInputs();
        // TODO: move this to hook in Game::Update or something
        PracticeMod::GetInstance()->update(0);
      }
    }

    return Orig(thiz, msg, queue);
  }
};

// CStateManager::Update
DECLARE_FUNCTION_REPLACEMENT(CStateManager_Update) {
  static void Callback(CStateManager *self, float dt) {
    Orig(self, dt);
    PracticeMod::GetInstance()->update(dt);
  }
};

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
  CMainFlow_OnMessage::InstallAtFuncPtr(&CMainFlow::OnMessage);
  CStateManager_Update::InstallAtFuncPtr(&CStateManager::Update);
  CRandom16_Next::InstallAtFuncPtr(&CRandom16::Next);
}
