#pragma once

#include "types.h"

#include "rstl/hash_map.h"
#include "rstl/rc_ptr.h"

#include "kyoto/CToken.hpp"
#include "kyoto/IObjectStore.hpp"

class IFactory;

class CSimplePool : public IObjectStore {
public:
  CSimplePool(IFactory& factory) : x18_factory(factory) {}
  ~CSimplePool();

  // virtual CToken GetObj(const SObjectTag& tag, CVParamTransfer xfer);
  // virtual CToken GetObj(const SObjectTag& tag);
  // virtual CToken GetObj(const char* name);
  // virtual CToken GetObj(const char* name, CVParamTransfer xfer);
  // virtual bool HasObject(const SObjectTag& tag);
  // virtual bool ObjectIsLive(const SObjectTag& tag);
  // virtual IFactory& GetFactory() { return x18_factory; }
  // virtual void Flush();
  // virtual void ObjectUnreferenced(const SObjectTag& tag);

  void fn_8029c7e8(const SObjectTag& tag);

  inline CToken GetObj(const SObjectTag& tag) {
    auto fn = VtableLookup<CToken (*)(CSimplePool *, const SObjectTag&)>(this, 1);
    return fn(this, tag);
  }

private:
  u8 x4_;
  u8 x5_;
  rstl::hash_map< void*, void*, void, void > x8_resources;
  IFactory& x18_factory;
  rstl::rc_ptr< CVParamTransfer > x1c_paramXfr;
};

extern CSimplePool* gpSimplePool;