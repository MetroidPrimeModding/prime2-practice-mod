#pragma once

#include "kyoto/CToken.hpp"

class IFactory;
class IObjectStore {
public:
  IObjectStore() {}
  // virtual CToken GetObj(const SObjectTag& tag, CVParamTransfer xfer) = 0;
  // virtual CToken GetObj(const SObjectTag& tag) = 0;
  // virtual CToken GetObj(const char* name) = 0;
  // virtual CToken GetObj(const char* name, CVParamTransfer xfer) = 0;
  // virtual bool HasObject(const SObjectTag& tag) = 0;
  // virtual bool ObjectIsLive(const SObjectTag& tag) = 0;
  // virtual IFactory& GetFactory() = 0;
  // virtual void Flush() = 0;
  // virtual void ObjectUnreferenced(const SObjectTag& tag) = 0;
};