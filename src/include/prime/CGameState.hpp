#pragma once

#include <GetField.hpp>
#include <prime/CPlayerState.hpp>
#include "rstl/rc_ptr.h"

class CWorldState;
class CCameraManager;

class CGameState {
public:
  u64 CardSerial() const { return *GetField<u64>(this, 0x108); }
  void SetCurrentWorldId(CAssetId id);
  CAssetId CurrentWorldAssetId();
  CWorldState &StateForWorld(unsigned int world);
  CWorldState &CurrentWorldState();
  // Bridge for PlayerState(int): returns the address of an ncrc_ptr, whose first word is the player pointer.
  // Do not use this view to copy, assign or release the game's ownership wrapper.
  CPlayerState **GetPlayerState(int idx);
  // inline CAssetId MLVL() { return *GetField<u32>(this, 0x84); };
  inline double PlayTime() { return *GetField<double>(this, 0x48); }
};

extern CGameState *gpGameState;
