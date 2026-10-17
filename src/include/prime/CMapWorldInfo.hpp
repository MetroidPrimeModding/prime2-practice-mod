#pragma once

#include "GetField.hpp"

class CMapWorldInfo {
public:
  // MWCC passes the four-byte TEditorId class indirectly. Keep an explicit pointer bridge:
  // Clang's by-value aggregate convention would pass the value instead of its address in r4.
  bool IsObjectUnmapped(const u32 *editorId) const;
};
