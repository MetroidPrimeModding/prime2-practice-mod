#include "MultiplayerRemoval.hpp"
#include <os.h>
#include <types.h>

extern "C" {
extern u32 _PATCH_MP_DEATHMATCH;
extern u32 _PATCH_MP_COIN;
extern u32 _PATCH_MP_GUI_CONSTRUCTOR;
extern u32 _PATCH_MP_GUI_UPDATE;
extern u32 _PATCH_MP_GUI_DRAW;
extern u32 _PATCH_MP_QUIT_CREATE_SKIP;
extern u32 _PATCH_MP_QUIT_CONSTRUCTOR;
extern u32 _PATCH_MP_QUIT_DESTRUCTOR[3];
extern u32 _PATCH_MP_QUIT_DRAW;
extern u32 _PATCH_MP_QUIT_UPDATE;
extern u32 _PATCH_MP_QUIT_INPUT;
extern u32 _PATCH_MP_RESULTS_CONSTRUCTOR;
extern u32 _PATCH_MP_CAMERA_CTOR_1, _PATCH_MP_CAMERA_CTOR_2, _PATCH_MP_CAMERA_CTOR_3;
extern u32 _PATCH_MP_CAMERA_DTOR_1, _PATCH_MP_CAMERA_DTOR_2, _PATCH_MP_CAMERA_DTOR_3;
extern u32 _PATCH_MP_CAMERA_PTR_1, _PATCH_MP_CAMERA_PTR_2, _PATCH_MP_CAMERA_PTR_3;
}

// These gates must execute from maindata before any reclaimed match code could be called.
#pragma clang attribute push(__attribute__((section(".boot.multiplayer"))), apply_to = function)

static void multiplayerPatchFailure() {
  OSReport("Unsupported multiplayer handler; stopping before reclaimed code can execute\n");
  __builtin_trap();
}

extern "C" void PatchMultiplayerModule(u32 base) {
  auto *matchStart = reinterpret_cast<volatile u32 *>(base + 0x334C);
  auto *controllerCount = reinterpret_cast<volatile u32 *>(base + 0x4190);
  // StartMultiplayerGame is also the frontend's single-player start handler (the SetToZero message with neither
  // match switch open selects kSGM_SinglePlayer), so it is only verified here, never patched.
  if (matchStart[0] != 0x9421FCC0 || matchStart[1] != 0x7C0802A6 || matchStart[0x39C / 4] != 0x4E800020 ||
      (*controllerCount != 0x3B9C0001 && *controllerCount != 0x3B800001))
    multiplayerPatchFailure();

  // UpdateControllerCount: replace ++count with count=1. Its frontend-only count never exceeds one,
  // so the retail controller-count switch keeps multiplayer unavailable even with four pads attached.
  // Actual controller discovery/input and other callers remain unchanged.
  *controllerCount = 0x3B800001; // li r28,1
  DCFlushRange(const_cast<u32 *>(controllerCount), sizeof(u32));
  ICInvalidateRange(const_cast<u32 *>(controllerCount), sizeof(u32));
}

static void patchMultiplayerInstruction(volatile u32 *entry, u32 original, u32 replacement = 0x7FE00008) {
  // Default: tw 31,r0,r0, unconditional program exception before stack mutation.
  if (*entry != original && *entry != replacement) {
    OSReport("Unsupported multiplayer entry at %p; stopping before reclaimed code can execute\n", entry);
    __builtin_trap();
  }
  *entry = replacement;
  DCFlushRange(const_cast<u32 *>(entry), sizeof(u32));
  ICInvalidateRange(const_cast<u32 *>(entry), sizeof(u32));
}

static void disableQuitScreenDestructor() {
  // single_ptr destroys its pointee unconditionally, including nullptr during ordinary single-player cleanup.
  // Preserve the MWCC destructor's this return for nullptr; stop before the reused body for a live object.
  constexpr u32 original[3] = {0x9421FFF0, 0x7C0802A6, 0x90010014};
  constexpr u32 replacement[3] = {0x2C030000, 0x4D820020, 0x7FE00008}; // cmpwi r3,0; beqlr; trap
  auto *entry = reinterpret_cast<volatile u32 *>(_PATCH_MP_QUIT_DESTRUCTOR);
  bool originalMatch = true;
  bool replacementMatch = true;
  for (u32 i = 0; i < 3; ++i) {
    originalMatch &= entry[i] == original[i];
    replacementMatch &= entry[i] == replacement[i];
  }
  if (!originalMatch && !replacementMatch) multiplayerPatchFailure();
  for (u32 i = 0; i < 3; ++i)
    entry[i] = replacement[i];
  DCFlushRange(_PATCH_MP_QUIT_DESTRUCTOR, sizeof(_PATCH_MP_QUIT_DESTRUCTOR));
  ICInvalidateRange(_PATCH_MP_QUIT_DESTRUCTOR, sizeof(_PATCH_MP_QUIT_DESTRUCTOR));
}

extern "C" void DisableMultiplayer() {
  // PauseGame(state=7): branch from the multiplayer-only allocation block to its existing epilogue.
  // The state!=7 branch still reaches the unmodified single-player/map/logbook/save/message path.
  patchMultiplayerInstruction(&_PATCH_MP_QUIT_CREATE_SKIP, 0x3C80803B, 0x48000088);
  // Retain traps at all externally referenced quit-screen entries while reusing the unreachable bodies.
  patchMultiplayerInstruction(&_PATCH_MP_QUIT_CONSTRUCTOR, 0x9421FFE0);
  disableQuitScreenDestructor();
  patchMultiplayerInstruction(&_PATCH_MP_QUIT_DRAW, 0x9421FFD0);
  patchMultiplayerInstruction(&_PATCH_MP_QUIT_UPDATE, 0x9421FFE0);
  patchMultiplayerInstruction(&_PATCH_MP_QUIT_INPUT, 0x9421FFD0);

  // The results screen is built only in CMFGame's kFS_MultiplayerResults state, which needs a live match.
  patchMultiplayerInstruction(&_PATCH_MP_RESULTS_CONSTRUCTOR, 0x9421FFA0);

  // CStateManagerContainer's camera managers 1-3 serve players 2-4 only; CreateStandardGameObjects, Update and
  // ProcessInput touch mCameraManagers[i] only for i < mNumPlayers. Skipping their constructors also skips the
  // CCameraHintManager/CCameraShakerManager heap allocations. The container storage (0x803C7848..0x803CA740) is a
  // BSS carveout, so the pointers are aliased to manager 0 rather than into reused memory.
  constexpr u32 kNop = 0x60000000;
  patchMultiplayerInstruction(&_PATCH_MP_CAMERA_CTOR_1, 0x481681C5, kNop);
  patchMultiplayerInstruction(&_PATCH_MP_CAMERA_CTOR_2, 0x481681AD, kNop);
  patchMultiplayerInstruction(&_PATCH_MP_CAMERA_CTOR_3, 0x48168195, kNop);
  patchMultiplayerInstruction(&_PATCH_MP_CAMERA_DTOR_3, 0x48000055, kNop);
  patchMultiplayerInstruction(&_PATCH_MP_CAMERA_DTOR_2, 0x48000049, kNop);
  patchMultiplayerInstruction(&_PATCH_MP_CAMERA_DTOR_1, 0x4800003D, kNop);
  constexpr u32 kAliasManager0 = 0x7C601B78; // mr r0,r3 (r3 is the container base)
  patchMultiplayerInstruction(&_PATCH_MP_CAMERA_PTR_1, 0x38030FA8, kAliasManager0);
  patchMultiplayerInstruction(&_PATCH_MP_CAMERA_PTR_2, 0x38031F50, kAliasManager0);
  patchMultiplayerInstruction(&_PATCH_MP_CAMERA_PTR_3, 0x38032EF8, kAliasManager0);

  // Match constructors have one retail caller each, in StartGameFromFrontEnd. Keeping a trap at
  // each entry also closes unexpected factory/script paths without creating a partially initialized mode.
  patchMultiplayerInstruction(&_PATCH_MP_DEATHMATCH, 0x9421FFC0);
  patchMultiplayerInstruction(&_PATCH_MP_COIN, 0x9421FFD0);
  // CStateManager::IsMultiplayer excludes both SNGL and FRND. These GUI paths are match-only;
  // retain their directly called entries as fail-closed gates while reusing their bodies.
  patchMultiplayerInstruction(&_PATCH_MP_GUI_CONSTRUCTOR, 0x9421FFE0);
  patchMultiplayerInstruction(&_PATCH_MP_GUI_UPDATE, 0x9421FF00);
  patchMultiplayerInstruction(&_PATCH_MP_GUI_DRAW, 0x9421FFF0);
}

#pragma clang attribute pop
