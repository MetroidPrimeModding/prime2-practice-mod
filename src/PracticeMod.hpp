#pragma once

#include "include/prime/CStateManager.hpp"
#include <include/prime/CFinalInput.hpp>

void warp(uint32_t world, uint32_t area);

class PracticeMod {
public:
  static PracticeMod *GetInstance();
  static void ClearInstance();
  bool menuActive{true};

  bool pauseScreenActive{true};
  bool mapActive{false};
  CFinalInput inputs[4];

  PracticeMod();

  void render();
  void HandleInputs();
  void update(float d) const;

  void pauseScreenClosed();
  void pauseScreenOpened();
private:
  void renderMenu();
  float hotkeyInputTimeout = 0;
  int upPresses = 0;
  int downPresses = 0;
};
