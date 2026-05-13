#pragma once

#include "types.h"

#define kInvalidAssetId 0xFFFFFFFFu

typedef uint FourCC;

struct SObjectTag {
  FourCC type;
  CAssetId id;

  SObjectTag() {}
  SObjectTag(FourCC type, CAssetId id) : type(type), id(id) {}
  SObjectTag(const SObjectTag& other) : type(other.type), id(other.id) {}

  static const char* Type2Text(FourCC type);
};