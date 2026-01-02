#pragma once

#include "GetField.hpp"

class CPauseScreen {
public:
  // other unknown states
  enum class EPauseScreenState {
    EState_Entering = 1,
    EState_Active = 2,
    EState_Exiting = 3,
  };
  void ProcessInput(const class CFinalInput &input);

  EPauseScreenState GetState() { return *GetField<EPauseScreenState>(this, 0x24C); }
};

// extern CMain g_Main_Body;
extern CMain* gpMain;
