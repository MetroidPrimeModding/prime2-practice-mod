#pragma once

#include "CGameState.hpp"
#include "CGraphics.hpp"
#include "CObjectList.hpp"
#include "CPlayer.hpp"
#include "CScriptSpawnPoint.hpp"
#include "GetField.hpp"
#include "PrimeAPI.h"
#include "rstl/rc_ptr.h"
#include "rstl/vector.h"
#include "types.h"

class CFinalInput;
class CPlayerState;
class CWorldLayerState;
class CGameState;
class CWorldState;
class CWorld;
class CPlayer;
class CCameraManager;
class CWorldTransManager;
class CRandom16;
class CScriptMailbox;
class CMapWorldInfo;

struct StateManagerFlags {
  bool xf94_24_ : 1;
  bool xf94_25_quitGame : 1;
  bool xf94_26_generatingObject : 1;
  bool xf94_27_inMapScreen : 1;
  bool xf94_28_inSaveUI : 1;
  bool xf94_29_cinematicPause : 1;
  bool xf94_30_fullThreat : 1;
};

class CStateManager {

  // PADDING(0x8b8);
  // rstl::rc_ptr<CPlayerState> mpPlayerState;
  // PADDING(12);
  // rstl::rc_ptr<CWorldLayerState> worldLayerState;

public:
  // Constructor bridge: MWCC returns this in r3; opaque references keep all five arguments in r4-r8.
  CStateManager *CStateManager_ctor(const rstl::ncrc_ptr<CScriptMailbox> &mailbox,
                                    const rstl::ncrc_ptr<CMapWorldInfo> &mapInfo,
                                    const rstl::reserved_vector<rstl::ncrc_ptr<CPlayerState>, 4> &playerStates,
                                    const rstl::ncrc_ptr<CWorldTransManager> &worldTrans,
                                    const rstl::ncrc_ptr<CWorldLayerState> &worldLayers);

  enum EInitPhase { kInit_LoadWorld = 0, kInit_LoadFirstArea = 1, kInit_Done = 2 };

  void InitializeState(uint WorldAssetId, TAreaId AreaId, uint AreaAssetId);
  CStateManager *Destroy(int deleting); // MWCC deleting destructor; returns this.
  void Update(float dt, class CArchitectureQueue &queue);
  bool IsMultiplayer() const;
  float GetEscapeSequenceTimer() const;
  enum class Transition { InGame = 0, Pause = 2, PauseMenu = 3, LogBook = 4, Save = 5 };
  void DeferStateTransition(Transition transition);
  Transition DeferredTransition() const { return *GetField<Transition>(this, 0x2470); }
  // G2ME01 offsets and masks audited in SAVE_ANYWHERE.md.
  bool FullyInitialized() const { return *GetField<int>(this, 0x16f0) == 3; }
  int GameState() const { return *GetField<int>(this, 0x16ec); }
  bool UnsafeSaveTransition() const {
    // Quit or cinematic pause, pending dock, or owned portal transition.
    return (*GetField<u8>(this, 0x294c) & 0x44) ||
           *GetField<int>(this, 0x28f8) != -1 || *GetField<void *>(this, 0x2900);
  }
  CCameraManager *SaveCamera() const { return *GetField<CCameraManager *>(this, 0x151c); }

  inline CFinalInput *GetFinalInput() const {
    return GetField<CFinalInput>(this, 0x153C);
  }
  inline CPlayer *GetPlayer(int index) {
    CPlayer **players = *GetField<CPlayer *[4]>(this, 0x14FC);
    if (!players) return nullptr;
    CPlayer *res = players[index % 4];
    if (!res) return nullptr;
    if (*res->getMovementState() > CPlayer::EPlayerMovementState::Bouncing) {
      // it's not initialized yet!
      return nullptr;
    }
    return res;
  }
  inline CPlayer *GetPlayer() {
    return GetPlayer(0);
  }
  // inline EInitPhase GetInitPhase() const { return *GetField<EInitPhase>(this, 0xB3C); }
  // inline CRandom16 *GetRandom() const { return GetField<CRandom16>(this, 0x8FC); }
  // inline CRandom16 *GetActiveRandom() const { return *GetField<CRandom16 *>(this, 0x900); }
  // inline CPlayerState *GetPlayerState() const { return mpPlayerState.RawPointer(); }
  // inline CWorldLayerState *GetWorldLayerState() const { return worldLayerState.RawPointer(); }
  CWorld *GetWorld() const {
    return *GetField<CWorld *>(this, 0x1604);
  };
  // CWorldTransManager *WorldTransManager() const { return GetField<CWorldTransManager>(this, 0x8c4); }
  // CPlayer *Player() const { return *GetField<CPlayer *>(this, 0x84C); };
  // CObjectList *GetAllObjs() { return *GetField<CObjectList *>(this, 0x810); };
  // CEntity *ObjectById(TUniqueId uid) { return GetAllObjs()->GetObjectById(uid); }
  // inline CCameraManager *x870_cameraManager() { return *GetField<CCameraManager *>(this, 0x870); };

  void SetShouldQuitGame(bool should) {
    u8 *flag = GetField<u8>(this, 0x294c);
    if (should) {
      *flag = (*flag & 0xBF) | 0x40;
    } else {
      *flag = (*flag & 0xBF);
    }
  }

  void DrawWorld() const;
  void DrawDebugStuff() const;
  CFrustum SetupViewForDraw(const SViewport &vp) const;
  void ResetViewAfterDraw(const SViewport &backupViewport, const CTransform4f &backupViewMatrix) const;
  void SpawnPlayerAtPoint(CScriptSpawnPoint *point, int playerIndex);
};

extern CStateManager g_CStateManager;
