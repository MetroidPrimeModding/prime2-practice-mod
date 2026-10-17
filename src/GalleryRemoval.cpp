#include "OSModule.hpp"
#include "utils/ReplaceFunction.hpp"

// Keep the module gate and its installer in maindata, independently of reclaimed gallery code.
#pragma clang attribute push(__attribute__((section(".boot.gallery"))), apply_to = function)

static void galleryPatchFailure() {
  OSReport("Unsupported ScriptGui module or gallery hook failure; stopping before gallery code can execute\n");
  __builtin_trap();
}

// clang-format off
DECLARE_FUNCTION_REPLACEMENT(Gallery_OSLinkFixed) {
  static int Callback(GameOSModuleInfo *module, void *bss) {
    const int result = Orig(module, bss);
    if (module->id != 60) return result;

    // G2ME01 ScriptGui.rel, after relocation and before CRelFile::Link calls its prolog.
    // Check the retail layout and returning entry stub on every load; never cache a REL address.
    if (!result || module->version != 3 || module->numSections != 13) galleryPatchFailure();
    const GameOSSectionInfo &text = module->sections[1];
    const u32 base = text.addressAndFlags & ~1u;
    if (!(text.addressAndFlags & 1) || text.size != 0x9E6C ||
        base < 0x80000000 || base > 0x81800000 - text.size) galleryPatchFailure();
    auto *entry = reinterpret_cast<volatile u32 *>(base + 0xCEC);
    if ((entry[0] != 0x9421FFD0 && entry[0] != 0x4E800020) || entry[1] != 0x7C0802A6 ||
        entry[0x84 / 4] != 0x4E800020) galleryPatchFailure();

    // CScriptGuiFrontEndScreen::ShowSlideShow: return before allocation or CSlideShow construction.
    entry[0] = 0x4E800020;
    DCFlushRange(const_cast<u32 *>(entry), sizeof(u32));
    ICInvalidateRange(const_cast<u32 *>(entry), sizeof(u32));
    return result;
  }
};
// clang-format on

extern "C" void InstallGalleryHooks() {
  Gallery_OSLinkFixed::InstallAtFuncPtr(&OSLinkFixed);
  // A missing hook would allow a later module load to enter reclaimed code.
  const u32 from = reinterpret_cast<u32>(&OSLinkFixed);
  const u32 to = reinterpret_cast<u32>(&Gallery_OSLinkFixed::Callback);
  const u32 expected = 0x48000000 | ((to - from) & 0x03FFFFFC);
  if (*reinterpret_cast<const volatile u32 *>(from) != expected) galleryPatchFailure();
}

#pragma clang attribute pop
