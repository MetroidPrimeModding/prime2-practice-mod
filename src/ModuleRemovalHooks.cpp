#include "ModuleRemovalHooks.hpp"
#include "GalleryRemoval.hpp"
#include "MultiplayerRemoval.hpp"
#include "OSModule.hpp"
#include "utils/ReplaceFunction.hpp"

// Keep the module gate and its installer in maindata, independently of reclaimed game code.
#pragma clang attribute push(__attribute__((section(".boot.removal"))), apply_to = function)

static void modulePatchFailure() {
  OSReport("Unsupported ScriptGui module or hook failure; stopping before reclaimed game code can execute\n");
  __builtin_trap();
}

// clang-format off
DECLARE_FUNCTION_REPLACEMENT(Removal_OSLinkFixed) {
  static int Callback(GameOSModuleInfo *module, void *bss) {
    const int result = Orig(module, bss);
    if (module->id != 60) return result;

    // G2ME01 ScriptGui.rel, after relocation and before CRelFile::Link calls its prolog.
    // Check the retail layout and returning entry stub on every load; never cache a REL address.
    if (!result || module->version != 3 || module->numSections != 13) modulePatchFailure();
    const GameOSSectionInfo &text = module->sections[1];
    const u32 base = text.addressAndFlags & ~1u;
    if (!(text.addressAndFlags & 1) || text.size != 0x9E6C ||
        base < 0x80000000 || base > 0x81800000 - text.size) modulePatchFailure();
    PatchGalleryModule(base);
    PatchMultiplayerModule(base);
    return result;
  }
};
// clang-format on
extern "C" void InstallRemovalHooks() {
  Removal_OSLinkFixed::InstallAtFuncPtr(&OSLinkFixed);
  // A missing hook would allow a later module load to enter reclaimed code.
  const u32 from = reinterpret_cast<u32>(&OSLinkFixed);
  const u32 to = reinterpret_cast<u32>(&Removal_OSLinkFixed::Callback);
  const u32 expected = 0x48000000 | ((to - from) & 0x03FFFFFC);
  if (*reinterpret_cast<const volatile u32 *>(from) != expected) modulePatchFailure();
}

#pragma clang attribute pop
