#pragma once

#include "../PracticeMod.hpp"

struct WarpArea {
  const char* name;
  u32 area;
};

struct WarpWorld {
  const char* name;
  u32 world;
  const WarpArea* areas;
  int areaCount;
};

const char* getNameForWorldAsset(u32 world);
const char* getNameForAreaAsset(u32 world, u32 area);

namespace GUI {
  void drawWarpMenu();
}
