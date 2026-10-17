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
  bool OSD_showJumpState : 1 {false};

  bool RNG_lockSeed{false};
};

extern Settings SETTINGS;

// Stable on-card IDs. Never renumber or reuse a retired ID.
#define SETTINGS_FIELDS(X)                                                                                             \
  X(1, OSD_show)                                                                                                       \
  X(2, OSD_showFrameTime)                                                                                              \
  X(3, OSD_showMemoryInfo)                                                                                             \
  X(4, OSD_showMemoryGraph)                                                                                            \
  X(5, OSD_showInput)                                                                                                  \
  X(6, OSD_showVelocity)                                                                                               \
  X(7, OSD_showRotationalVelocity)                                                                                     \
  X(8, OSD_showPos)                                                                                                    \
  X(9, OSD_showIGT)                                                                                                    \
  X(10, OSD_showCurrentRoomTime)                                                                                       \
  X(11, OSD_showPreviousRoomTime)                                                                                      \
  X(12, OSD_showLoads)                                                                                                 \
  X(13, OSD_showJumpState)                                                                                             \
  X(14, RNG_lockSeed)
