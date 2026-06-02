#pragma once

#include "kyoto/CObjectReference.hpp"
#include "kyoto/IObj.hpp"

class CToken {
public:
  CToken() : x0_objRef(nullptr), x4_lockHeld(false) {}
  CToken(CObjectReference* ref) : x0_objRef(ref), x4_lockHeld(false) {
    x0_objRef->AddReference();
  }
  CToken(IObj* obj); // : x0_objRef(new CObjectReference(obj)), x4_lockHeld(false) {}
  CToken(const CToken& other);
  // ~CToken();

  CToken* CToken_dtor(u16 param);

  CObjOwnerDerivedFromIObjUntyped* GetObj();
  void Lock();
  void Unlock();
  bool IsLoaded() const { return x0_objRef->IsLoaded(); }
  void RemoveRef();
  CToken& operator=(const CToken&);
  bool HasReference() const { return x0_objRef != nullptr; }
  const SObjectTag& GetTag() const { return x0_objRef->GetTag(); }
  FourCC GetReferenceType() { return GetTag().type; }

  bool HasLock() { return x4_lockHeld; }

  const CObjectReference* GetRef() const { return x0_objRef; }
  inline CObjectReference* GetRefNonConst() { return x0_objRef; }

  // for crimes
  inline void InitializeFromOther(const CToken& other) {
    x0_objRef = other.x0_objRef;
    x0_objRef->AddReference();
    if (other.x4_lockHeld) {
      Lock();
    }
  }

private:
  CObjectReference* x0_objRef;
  bool x4_lockHeld;
};