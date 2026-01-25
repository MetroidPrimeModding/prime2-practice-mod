#include "PracticeMod.hpp"
#include "utils/ReplaceFunction.hpp"
#include <os.h>

// Forward decls
class CPlayer;

extern "C" {
#pragma clang attribute push(__attribute__((section(".boot"))), apply_to = function)
__attribute__((visibility("default"))) extern void _prolog();
[[maybe_unused]] __attribute__((visibility("default"))) extern void _earlyboot_memset(void *dst, char val, u32 size);
#pragma clang attribute pop

struct ForceStaticInit {
  ForceStaticInit() {
    OSReport("Static initializer forced to run\n");
  }
} g_force_static_init;

void InstallHooks();
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
  // this is the only thing in the function we replace
  asm volatile("mtfsb0 5");
  runStaticInitializers();
  PracticeMod::ClearInstance();
  Hook_OSResetSystem::InstallAtFuncPtr(&OSResetSystem);
  InstallHooks();
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
