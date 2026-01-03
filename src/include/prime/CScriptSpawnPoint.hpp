#pragma once


class CScriptSpawnPoint: public CEntity {
public:
  inline CTransform4f *GetTransform() { return GetField<CTransform4f>(this, 0x30); }
};
