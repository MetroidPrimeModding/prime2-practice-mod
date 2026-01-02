#pragma once

#include <GetField.hpp>
#include <prime/CPlayerState.hpp>
#include "rstl/rc_ptr.h"

class CWorldState;
class CCameraManager;

class CGameState {
public:
  void SetCurrentWorldId(CAssetId id);
  CWorldState &StateForWorld(unsigned int world);
  CWorldState &CurrentWorldState();
  CPlayerState **GetPlayerState(int idx);
  // inline CAssetId MLVL() { return *GetField<u32>(this, 0x84); };
  inline double PlayTime() { return *GetField<double>(this, 0x48); }
};

extern CGameState *gpGameState;
