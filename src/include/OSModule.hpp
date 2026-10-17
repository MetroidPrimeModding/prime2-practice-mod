#pragma once

#include "types.h"

struct GameOSSectionInfo {
  u32 addressAndFlags;
  u32 size;
};

struct GameOSModuleInfo {
  u32 id;
  u32 next;
  u32 previous;
  u32 numSections;
  GameOSSectionInfo *sections; // OSLinkFixed converts the on-disc offset to a pointer.
  u32 nameOffset;
  u32 nameSize;
  u32 version;
};
static_assert(sizeof(GameOSSectionInfo) == 8);
static_assert(sizeof(GameOSModuleInfo) == 0x20);
static_assert(__builtin_offsetof(GameOSModuleInfo, numSections) == 0x0C);
static_assert(__builtin_offsetof(GameOSModuleInfo, sections) == 0x10);
static_assert(__builtin_offsetof(GameOSModuleInfo, version) == 0x1C);

extern "C" int OSLinkFixed(GameOSModuleInfo *module, void *bss);
