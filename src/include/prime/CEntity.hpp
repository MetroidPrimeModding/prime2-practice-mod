#pragma once

#include "GetField.hpp"
#include "PrimeAPI.h"
#include "rstl/pair.h"
#include "rstl/vector.h"

enum class EScriptObjectState {
  Any = -1,
  Active = 0,
  Arrived = 1,
  Closed = 2,
  Entered = 3,
  Exited = 4,
  Inactive = 5,
  Inside = 6,
  MaxReached = 7,
  Open = 8,
  Zero = 9,
  Attack = 10,
  UNKS1 = 11,
  Retreat = 12,
  Patrol = 13,
  Dead = 14,
  CameraPath = 15,
  CameraTarget = 16,
  UNKS2 = 17,
  Play = 18,
  UNKS3 = 19,
  DeathRattle = 20,
  UNKS4 = 21,
  Damage = 22,
  InvulnDamage = 23,
  UNKS5 = 24,
  Modify = 25,
  ScanStart = 26,
  ScanProcessing = 27,
  ScanDone = 28,
  UnFrozen = 29,
  Default = 30,
  ReflectedDamage = 31,
  InheritBounds = 32
};

enum class EScriptObjectMessage {
  None = -1,
  UNKM0 = 0,
  Activate = 1,
  Arrived = 2,
  Close = 3,
  Deactivate = 4,
  Decrement = 5,
  Follow = 6,
  Increment = 7,
  Next = 8,
  Open = 9,
  Reset = 10,
  ResetAndStart = 11,
  SetToMax = 12,
  SetToZero = 13,
  Start = 14,
  Stop = 15,
  StopAndReset = 16,
  ToggleActive = 17,
  UNKM18 = 18,
  Action = 19,
  Play = 20,
  Alert = 21,
  InternalMessage00 = 22,
  OnFloor = 23,
  InternalMessage02 = 24,
  InternalMessage03 = 25,
  Falling = 26,
  OnIceSurface = 27,
  OnMudSlowSurface = 28,
  OnNormalSurface = 29,
  Touched = 30,
  AddPlatformRider = 31,
  LandOnNotFloor = 32,
  Registered = 33,
  Deleted = 34,
  InitializedInArea = 35,
  WorldInitialized = 36,
  AddSplashInhabitant = 37,
  UpdateSplashInhabitant = 38,
  RemoveSplashInhabitant = 39,
  Jumped = 40,
  Damage = 41,
  InvulnDamage = 42,
  ProjectileCollide = 43,
  InSnakeWeed = 44,
  AddPhazonPoolInhabitant = 45,
  UpdatePhazonPoolInhabitant = 46,
  RemovePhazonPoolInhabitant = 47,
  SuspendedMove = 48
};

struct PACKED SConnection {
  EScriptObjectState x0_state;
  EScriptObjectMessage x4_msg;
  u32 x8_objId;
};

class CEntity {
public:
  static constexpr uint8_t ACTIVE_MASK = 0x80;
  static constexpr uint8_t GRAVEYARD_MASK = 0x40;
  static constexpr uint8_t BLOCKED_MASK = 0x20;
  static constexpr uint8_t USE_MASK = 0x10;

  inline u32 getVtablePtr() { return *GetField<u32>(this, 0x0); }

  inline u32 getAreaID() { return *GetField<u32>(this, 0x4); }

  inline u16 getUniqueID() { return *GetField<u16>(this, 0x8); }

  inline u32 getEditorID() { return *GetField<u32>(this, 0xC); }

  rstl::vector_type_2<SConnection> *getConnections() { return GetField<rstl::vector_type_2<SConnection>>(this, 0x20); }
};
