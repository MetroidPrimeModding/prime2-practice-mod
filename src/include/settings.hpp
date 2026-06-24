#pragma once

#include <gctypes.h>

// whether to display the on-screen display by default
#define OSD_DEFAULT true

// whether to display the loads by default
#define LOADS_DEFAULT false

// whether to display the items in the monitor window by default
#define MONITOR_DEFAULT true

// whether to display the memory window by default
#define MEMORY_DEFAULT false

// whether to display the inputs by default
#define INPUT_DEFAULT true

#ifdef DEBUG
#undef LOADS_DEFAULT
#define LOADS_DEFAULT true
#endif

#ifdef RANDOMIZER
#undef OSD_DEFAULT
#define OSD_DEFAULT false
#endif

struct Settings {
  bool OSD_show                   : 1 {OSD_DEFAULT};
  bool OSD_showFrameTime          : 1 {MONITOR_DEFAULT && false};
  bool OSD_showMemoryInfo         : 1 {MEMORY_DEFAULT && false};
  bool OSD_showMemoryGraph        : 1 {MEMORY_DEFAULT && false};
  bool OSD_showInput              : 1 {INPUT_DEFAULT};
  bool OSD_showVelocity           : 1 {MONITOR_DEFAULT && true};
  bool OSD_showRotationalVelocity : 1 {MONITOR_DEFAULT && false};
  bool OSD_showPos                : 1 {MONITOR_DEFAULT && true};
  bool OSD_showIGT                : 1 {MONITOR_DEFAULT && true};
  bool OSD_showCurrentRoomTime    : 1 {MONITOR_DEFAULT && true};
  bool OSD_showPreviousRoomTime   : 1 {MONITOR_DEFAULT && true};
  bool OSD_showLoads              : 1 {LOADS_DEFAULT};
  bool OSD_showJumpState          : 1 {MONITOR_DEFAULT && false};

  bool RNG_lockSeed{false};
};

extern Settings SETTINGS;

#undef OSD_DEFAULT
#undef LOADS_DEFAULT
#undef MONITOR_DEFAULT
#undef MEMORY_DEFAULT
#undef INPUT_DEFAULT
