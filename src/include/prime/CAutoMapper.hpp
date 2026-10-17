#pragma once

#include "GetField.hpp"
#include "prime/CWorld.hpp"

class CFinalInput;
class CStateManager;
class CTransform4f;

class CAutoMapper {
public:
  void Draw(const CStateManager &, const CTransform4f &, float) const;
  void ProcessMapScreenInput(const CFinalInput &, CStateManager &);

  // G2ME01 retail SetCurAreaId, IsInMapperState and ProcessMapScreenInput.
  bool IsWorldMap() const {
    return *GetField<int>(this, 0x200) == 1 && *GetField<int>(this, 0x204) == 1;
  }
  bool IsNormalMap() const { return *GetField<int>(this, 0x210) == 0; }
  IWorld *World() const { return *GetField<IWorld *>(this, 0x38); }
  TAreaId FocusArea() const { return *GetField<TAreaId>(this, 0xe4); }
};

class CMapWorldInfo {
public:
  bool IsWorldVisible(TAreaId, bool inDarkWorld) const;
  bool IsAreaVisible(TAreaId) const;
};

class CMapArea {
public:
  bool GetIsVisibleToAutoMapper(bool worldVis, bool areaVis) const;
};
