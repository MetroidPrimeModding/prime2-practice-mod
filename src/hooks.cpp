#include "UI/SaveAnywhere.hpp"
#include "PracticeMod.hpp"
#include "system/CardGate.hpp"
#include "prime/CMemoryCardSys.hpp"
#include "prime/CAutoMapper.hpp"
#include "prime/CGameArea.hpp"
#include "prime/CFinalInput.hpp"
#include "prime/CMain.hpp"
#include "prime/CMainFlow.hpp"
#include "prime/CPauseScreen.hpp"
#include "prime/CPlayMovie.hpp"
#include "prime/CPlayer.hpp"
#include "prime/CStateManager.hpp"
#include "prime/CStringTable.hpp"
#include "version.h"
#include "utils/ReplaceFunction.hpp"
#include <prime/CRandom16.hpp>
#include <settings.hpp>

// clang format loves to just stomp everything here in the least useful possible way
// clang-format off
// DECLARE_FUNCTION_REPLACEMENT(CMain_DrawDebugMetrics) {
//   static void Callback() {
//     PracticeMod::GetInstance()->render();
//     Orig();
//   }
// };

DECLARE_FUNCTION_REPLACEMENT(CardDriver_Construct) {
  static CMemoryCardDriver *Callback(CMemoryCardDriver *self, CMemoryCardSys::EMemoryCardPort port,
                                    u32 banner, u32 icon0, u32 icon1, bool import) {
    CardGate::driverConstructing();
    return Orig(self, port, banner, icon0, icon1, import);
  }
};

DECLARE_FUNCTION_REPLACEMENT(CardDriver_Destroy) {
  static CMemoryCardDriver *Callback(CMemoryCardDriver *self, int deleting) {
    CMemoryCardDriver *result = Orig(self, deleting);
    if (self) CardGate::driverDestroyed();
    return result;
  }
};

DECLARE_FUNCTION_REPLACEMENT(CardSys_Destroy) {
  static CMemoryCardSys *Callback(CMemoryCardSys *self, int deleting) {
    if (self) CardGate::drain();
    return Orig(self, deleting);
  }
};

// CStringTable::GetString
DECLARE_FUNCTION_REPLACEMENT(CStringTable_GetString) {
  static const char16_t *Callback(const CStringTable *self, int idx) {
    // FrontEnd.pak STRG 98E7E268: identify the menu by its language-independent keys.
    // Do not cache the table pointer: resources can be unloaded between menu visits.
    if (idx == 2 && self->GetStringIndex("MainMenu") == 0 && self->GetStringIndex("SinglePlayer") == 1 &&
        self->GetStringIndex("Multiplayer") == 2) {
      return u"Practice Mod v" MOD_VERSION;
    }
    return Orig(self, idx);
  }
};

// CGraphics::EndScene
DECLARE_FUNCTION_REPLACEMENT(CGraphics_EndScene) {
  static void Callback() {
    SaveAnywhere::tick();
    CardGate::tick();
    PracticeMod::GetInstance()->render();
    Orig();
  }
};


// CMainFlow::OnMessage
DECLARE_FUNCTION_REPLACEMENT(CMainFlow_OnMessage) {
  static CIOWin::EMessageReturn Callback(CMainFlow *thiz, const CArchitectureMessage &msg, CArchitectureQueue &queue) {
    if (msg.x4_type == EArchMsgType_UserInput) {
      const CArchMsgParmUserInput *status = static_cast<const CArchMsgParmUserInput *>(msg.GetParm());
      const CFinalInput *input = status->GetInput();

      // The mod 4 is just for safety
      memcpy(&PracticeMod::GetInstance()->inputs[input->ControllerIdx() % 4],
             input,
             sizeof(CFinalInput));
      if (input->ControllerIdx() == 0) {
        PracticeMod::GetInstance()->HandleInputs();
      }
    }

    CIOWin::EMessageReturn res = Orig(thiz, msg, queue);
    return res;
  }
};

DECLARE_FUNCTION_REPLACEMENT(SaveAnywhere_Teardown) {
  static CStateManager *Callback(CStateManager *self, int deleting) {
    SaveAnywhere::cancel();
    return Orig(self, deleting);
  }
};

DECLARE_FUNCTION_REPLACEMENT(SaveAnywhere_Update) {
  static void Callback(CStateManager *self, float dt, CArchitectureQueue &queue) {
    Orig(self, dt, queue);
    SaveAnywhere::update();
  }
};

// CPlayer::ProcessInput
DECLARE_FUNCTION_REPLACEMENT(CPlayer_ProcessInput) {
  static void Callback(CPlayer *self, const CFinalInput &input, CStateManager &mgr) {
    Orig(self, input, mgr);
    PracticeMod::GetInstance()->handlePosLoadIfNeeded(self);
  }
};

DECLARE_FUNCTION_REPLACEMENT(CPauseScreen_ProcessControllerInput) {
  static void Callback(CPauseScreen *self, const CFinalInput &input) {
    if (self->GetState() != CPauseScreen::EPauseScreenState::EState_Active) {
      Orig(self, input);
      return;
    }

    if (SaveAnywhere::consumeCloseRequest()) {
      self->CloseForSave();
      PracticeMod::GetInstance()->pauseScreenClosed();
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

// Visibility queries only: leave visited/mapped save data untouched.
DECLARE_FUNCTION_REPLACEMENT(CMapWorldInfo_IsWorldVisible) {
  static bool Callback(const CMapWorldInfo *, TAreaId, bool) { return true; }
};
DECLARE_FUNCTION_REPLACEMENT(CMapWorldInfo_IsAreaVisible) {
  static bool Callback(const CMapWorldInfo *, TAreaId) { return true; }
};
DECLARE_FUNCTION_REPLACEMENT(CMapArea_GetIsVisibleToAutoMapper) {
  static bool Callback(const CMapArea *, bool, bool) { return true; }
};

DECLARE_FUNCTION_REPLACEMENT(CAutoMapper_Draw) {
  static void Callback(const CAutoMapper *self, const CStateManager &mgr, const CTransform4f &xf, float alpha) {
    Orig(self, mgr, xf, alpha);
    PracticeMod::GetInstance()->mapActive = self->IsWorldMap() && self->IsNormalMap();
  }
};

DECLARE_FUNCTION_REPLACEMENT(CAutoMapper_ProcessMapScreenInput) {
  static void Callback(CAutoMapper *self, const CFinalInput &input, CStateManager &mgr) {
    // The caller already excludes interpolation, hint navigation and pending world loads.
    // Consume D-pad Up for warping; X keeps retail light/dark map switching.
    if (input.ControllerIdx() == 0 && input.PDPUp() && self->IsWorldMap() && self->IsNormalMap()) {
      IWorld *world = self->World();
      TAreaId areaId = self->FocusArea();
      if (world && areaId.id >= 0 && areaId.id < world->IGetAreaCount()) {
        const IGameArea *area = world->IGetAreaAlways(areaId);
        if (area) {
          PracticeMod::GetInstance()->mapActive = false;
          warp(world->IGetWorldAssetId(), area->IGetAreaAssetId());
        }
      }
      return;
    }
    Orig(self, input, mgr);
  }
};

DECLARE_FUNCTION_REPLACEMENT(CRandom16_Next) {
  static s32 Callback(CRandom16* rng) {
    if (!SETTINGS.RNG_lockSeed) {
      rng->m_seed = static_cast<u32>(rng->m_seed) * 0x41c64e6du + 0x00003039u;
    }
    return (rng->m_seed >> 16) & 0xffff;
  }
};

DECLARE_FUNCTION_REPLACEMENT(CMoviePlayer_constructor) {
  static CMoviePlayer *Callback(CMoviePlayer* self, const char *path, float preloadSeconds, bool loop, bool deinterlace) {
    OSReport("Playing movie: %s (preloadSeconds=%f loop=%d deinterlace=%d)\n", path, preloadSeconds, loop, deinterlace);

    bool isAttract2 = strcmp("Video/Attract02_32.thp", path) == 0;
    if (isAttract2) {
      OSReport("Skipping attract 2, playing 9 instead\n");
      return Orig(self, "Video/Attract09_31.thp", preloadSeconds, loop, deinterlace);
    }

    return Orig(self, path, preloadSeconds, loop, deinterlace);
  }
};

// DECLARE_FUNCTION_REPLACEMENT(CStateManger_SpawnPlayerAtPoint) {
//   static void Callback(CStateManager* self, CScriptSpawnPoint* point, int playerIndex) {
//     CTransform4f *transform = point->GetTransform();
//     OSReport("Spawning player at point: %08x pos %0.2f %0.2f %0.2f\n", point->getEditorID(), transform->x, transform->y, transform->z);
//     Orig(self, point, playerIndex);
//   }
// };


// DECLARE_FUNCTION_REPLACEMENT(Repl_PadRead) {
//   static u32 Callback(PADStatus *status) {
//     u32 res = Orig(status);
//     static bool spam = false;
//     if (spam) {
//       status->button |= PAD_BUTTON_A;
//     }
//     spam = !spam;
//     return res;
//   }
// };

// clang-format on

void InstallHooks() {
  SaveAnywhere_Teardown::InstallAtFuncPtr(&CStateManager::Destroy);
  SaveAnywhere_Update::InstallAtFuncPtr(&CStateManager::Update);
  CardSys_Destroy::InstallAtFuncPtr(&CMemoryCardSys::Destroy);
  CardDriver_Destroy::InstallAtFuncPtr(&CMemoryCardDriver::Destroy);
  CardDriver_Construct::InstallAtFuncPtr(&CMemoryCardDriver::Construct);
  CMapWorldInfo_IsWorldVisible::InstallAtFuncPtr(&CMapWorldInfo::IsWorldVisible);
  CMapWorldInfo_IsAreaVisible::InstallAtFuncPtr(&CMapWorldInfo::IsAreaVisible);
  CMapArea_GetIsVisibleToAutoMapper::InstallAtFuncPtr(&CMapArea::GetIsVisibleToAutoMapper);
  CAutoMapper_Draw::InstallAtFuncPtr(&CAutoMapper::Draw);
  CAutoMapper_ProcessMapScreenInput::InstallAtFuncPtr(&CAutoMapper::ProcessMapScreenInput);
  CStringTable_GetString::InstallAtFuncPtr(&CStringTable::GetString);
  // CMain_DrawDebugMetrics::InstallAtFuncPtr(&CMain::DrawDebugMetrics);
  CGraphics_EndScene::InstallAtFuncPtr(&CGraphics::EndScene);
  CMainFlow_OnMessage::InstallAtFuncPtr(&CMainFlow::OnMessage);
  CPlayer_ProcessInput::InstallAtFuncPtr(&CPlayer::ProcessInput);
  CPauseScreen_ProcessControllerInput::InstallAtFuncPtr(&CPauseScreen::ProcessControllerInput);
  CRandom16_Next::InstallAtFuncPtr(&CRandom16::Next);
  CMoviePlayer_constructor::InstallAtFuncPtr(&CMoviePlayer::CMoviePlayerConstructor);
  // CStateManger_SpawnPlayerAtPoint::InstallAtFuncPtr(&CStateManager::SpawnPlayerAtPoint);
  // Repl_PadRead::InstallAtFuncPtr(&PAD_Read);
}
