#include "PracticeMod.hpp"
#include "carveouts.h"
#include "utils/ReplaceFunction.hpp"
#include <os.h>

// Forward decls
class CPlayer;

extern "C" {
#pragma clang attribute push(__attribute__((section(".boot"))), apply_to = function)
__attribute__((visibility("default"))) extern void _prolog();
__attribute__((used, retain, visibility("default"))) extern void *_earlyboot_memset(void *dst, int val, u32 size);
#pragma clang attribute pop
void *memcpy(void *dest, const void *src, size_t count);
}

// Called from __init_data before OSInit, constructors, allocation, or hooks are available.
// Use volatile stores so LLVM cannot turn this loop into a call to a later runtime helper.
void *_earlyboot_memset(void *dst, int val, u32 size) {
  u32 cursor = reinterpret_cast<u32>(dst);
  const u32 end = cursor + size;
  for (u32 i = 0; i < sizeof(CARVEOUT_SAFE_BLOCKS) / sizeof(u32); i += 2) {
    const u32 start = CARVEOUT_SAFE_BLOCKS[i];
    const u32 stop = start + CARVEOUT_SAFE_BLOCKS[i + 1];
    if (stop <= cursor) continue;
    if (start >= end) break;
    while (cursor < start && cursor < end) {
      *reinterpret_cast<volatile unsigned char *>(cursor++) = static_cast<unsigned char>(val);
    }
    cursor = stop < end ? stop : end;
  }
  while (cursor < end) {
    *reinterpret_cast<volatile unsigned char *>(cursor++) = static_cast<unsigned char>(val);
  }
  return dst;
}

struct ForceStaticInit {
  ForceStaticInit() {
    OSReport("Static initializer forced to run\n");
  }
} g_force_static_init;

void InstallHooks();
void InstallRandomizerHooks();
bool initialized{false};

// clang-format off
DECLARE_FUNCTION_REPLACEMENT(Hook_OSResetSystem) {
  static void Callback(int reset,u32 resetCode,int forceMenu) {
    OSReport("Hooked OSResetSystem: reset=%d code=%u forceMenu=%d\n", reset, resetCode, forceMenu);
    // initialized = false;
    // ReplaceFunctionHookPool::Reset();
    Orig(reset, resetCode, forceMenu);
  }
};
// clang-format on

void runStaticInitializers() {
  // call static initializers
  // asm volatile("bl _GLOBAL__sub_I_prime_practice.cpp\n\t");
  extern void (*__init_array_start[])(void);
  extern void (*__init_array_end[])(void);

  size_t count = __init_array_end - __init_array_start;
  OSReport("Calling %u static initializer(s)\n", count);
  for (size_t i = 0; i < count; i++) {
    __init_array_start[i]();
  }
}

void _prolog() {
  // Preserve G2ME01 PPCSetFpIEEEMode, including repeated calls.
  asm volatile("mtfsb0 29");
  // check to see if we have hooked OSSystemReset
  u32 orig = *(u32 *)&OSResetSystem;
  if (initialized && orig == 0x7c0802a6) { // original instruction: mflr r0
    OSReport("Hooks have reset, reinitializing\n");
    initialized = false;
  }
  if (initialized) {
    DebugLog("Already called prolog once\n");
    return;
  }
  initialized = true;
  runStaticInitializers();
  PracticeMod::ClearInstance();
  Hook_OSResetSystem::InstallAtFuncPtr(&OSResetSystem);
  InstallHooks();

#ifdef RANDOMIZER
  InstallRandomizerHooks();
#endif
}

// void resetLayerStates(const CStateManager &manager) {
//  CMemoryCardSys *memorySystem = *(CMemoryCardSys **) 0x805A8C44;
//  CGameState *gameState = *(CGameState **) (0x80457798 + 0x134);
//  uint32 currentMlvl = gameState->MLVL();
//
//  CSaveWorldIntermediate *worldIntermediates = memorySystem->worldIntermediate;
//  CSaveWorldIntermediate *intermediate = 0;
//  for (int i = 0; i < 8; i++) {
//    if (worldIntermediates[i].mlvlID == currentMlvl) {
//      intermediate = &(worldIntermediates[i]);
//      break;
//    }
//  }
//  if (intermediate != 0) {
////    intermediate = *((CSaveWorldIntermediate**)((int)memorySystem & 0x7FFFFFFF));
////    crashVar = *((int*)((int)(intermediate->mlvlID) & 0x7FFFFFFF));
////    crashVar = *((int*)((int)&(intermediate->defaultLayerStates) & 0x7FFFFFFF));
//  } else {
//    crashVar = *((int *) 0xDEAD0001);
//  }
//
//  CWorldState &worldState = gameState->StateForWorld(currentMlvl);
//  CWorldLayerState *layerState = *worldState.layerState;
//
//  rstl::vector<CWorldLayers::Area> &srcLayers = intermediate->defaultLayerStates;
//  rstl::vector<CWorldLayers::Area> &destLayers = layerState->areaLayers;
//
//  if (srcLayers.len == destLayers.len) {
//    for (int i = 0; i < srcLayers.len; i++) {
//      destLayers.ptr[i].m_layerBits = srcLayers.ptr[i].m_layerBits;
//    }
//  } else {
//    crashVar = *((int *) 0xDEAD0002);
//  }
//}
