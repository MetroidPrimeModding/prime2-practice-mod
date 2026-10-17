#include "GalleryRemoval.hpp"
#include <os.h>

extern "C" u32 _PATCH_GALLERY_FLAGS[2];

#pragma clang attribute push(__attribute__((section(".boot.gallery"))), apply_to = function)

static void galleryPatchFailure() {
  OSReport("Unsupported gallery handler; stopping before reclaimed code can execute\n");
  __builtin_trap();
}

extern "C" void DisableImageGallery() {
  // G2ME01 CSlideShow::GetGalleriesUnlocked(): return no unlocked galleries.
  // ScriptGui.rel's UpdateUnlocks then deactivates the gallery's frontend network.
  // Leave persistent unlocks and the shared scan-completion helpers untouched.
  _PATCH_GALLERY_FLAGS[0] = 0x38600000; // li r3, 0
  _PATCH_GALLERY_FLAGS[1] = 0x4E800020; // blr
  DCFlushRange(_PATCH_GALLERY_FLAGS, sizeof(_PATCH_GALLERY_FLAGS));
  ICInvalidateRange(_PATCH_GALLERY_FLAGS, sizeof(_PATCH_GALLERY_FLAGS));
}

extern "C" void PatchGalleryModule(u32 base) {
  auto *entry = reinterpret_cast<volatile u32 *>(base + 0xCEC);
  if ((entry[0] != 0x9421FFD0 && entry[0] != 0x4E800020) || entry[1] != 0x7C0802A6 || entry[0x84 / 4] != 0x4E800020)
    galleryPatchFailure();

  // CScriptGuiFrontEndScreen::ShowSlideShow: return before allocation or CSlideShow construction.
  entry[0] = 0x4E800020;
  DCFlushRange(const_cast<u32 *>(entry), sizeof(u32));
  ICInvalidateRange(const_cast<u32 *>(entry), sizeof(u32));
}

#pragma clang attribute pop
