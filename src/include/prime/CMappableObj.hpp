#pragma once

#include "CMapWorldInfo.hpp"
#include "GetField.hpp"

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
  void Draw(int curArea, const CMapWorldInfo &mwInfo, float alpha, bool needsVtxLoad) const;

  inline const EMappableObjectType *getMappableObjectType() const {
    return GetField<EMappableObjectType>(this, 0x0);
  }
  inline const u32 *getEditorId() const {
    return GetField<uint>(this, 0x8);
  }
};
