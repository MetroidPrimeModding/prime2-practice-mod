#pragma once

#include <gctypes.h>

#ifdef DEBUG
#define DEBUG_TRUE true
#else
#define DEBUG_TRUE false
#endif

struct Settings {
  bool OSD_show : 1 {true};
  bool OSD_showFrameTime : 1 {false};
  bool OSD_showMemoryInfo : 1 {false};
  bool OSD_showMemoryGraph : 1 {false};
  bool OSD_showInput : 1 {true};
  bool OSD_showVelocity : 1 {true};
  bool OSD_showRotationalVelocity : 1 {false};
  bool OSD_showPos : 1 {true};
  bool OSD_showIGT : 1 {true};
  bool OSD_showCurrentRoomTime : 1 {true};
  bool OSD_showPreviousRoomTime : 1 {true};
  bool OSD_showLoads : 1 {DEBUG_TRUE};

  bool RNG_lockSeed{false};
};

extern Settings SETTINGS;
