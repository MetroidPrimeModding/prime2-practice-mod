#pragma once

#include "GetField.hpp"
#include "CMapWorldInfo.hpp"

typedef enum EMappableObjectType {
  // door types
  EMappableObjectType_NormalDoor = 0x0,
  EMappableObjectType_MissileDoor,
  EMappableObjectType_DarkDoor,
  EMappableObjectType_AnnihilatorDoor,
  EMappableObjectType_LightDoor,
  EMappableObjectType_SuperMissileDoor,
  EMappableObjectType_SeekerMissileDoor,
  EMappableObjectType_PowerBombDoor,
  
  // regular icon types
  EMappableObjectType_Elevator = 0x10,
  EMappableObjectType_SaveStation,
  EMappableObjectType_Unused_0x12,
  EMappableObjectType_Unused_0x13,
  EMappableObjectType_AmmoStation,
  EMappableObjectType_Portal,
  EMappableObjectType_LightTeleporter,
  EMappableObjectType_TranslatorGate,
  EMappableObjectType_UpArrow,
  EMappableObjectType_DownArrow,
} EMappableObjectType;

class CMappableObject {
public:
  void Draw(u32 curArea, const CMapWorldInfo& mwInfo, float alpha, bool needsVtxLoad);

  inline EMappableObjectType *getMappableObjectType() { return GetField<EMappableObjectType>(this, 0x0); }
  inline u32 *getEditorId() { return GetField<uint>(this, 0x8); }
};