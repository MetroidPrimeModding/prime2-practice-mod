#pragma once

#include "GetField.hpp"

class CFinalInput {
public:
  static constexpr float kInput_AnalogOnThreshhold = 0.7f;
  static constexpr float kInput_AnalogTriggerOnThreshhold = 0.05f;

  float DeltaTime() const { return dt; }

  int ControllerIdx() const { return controllerIdx; }

  float ALAUp() const { return anaLeftY > 0.f ? anaLeftY : 0.f; }

  float ALADown() const { return anaLeftY < 0.f ? -anaLeftY : 0.f; }

  float ALALeft() const { return anaLeftX < 0.f ? -anaLeftX : 0.f; }

  float ALARight() const { return anaLeftX > 0.f ? anaLeftX : 0.f; }

  float ARAUp() const { return anaRightY > 0.f ? anaRightY : 0.f; }

  float ARADown() const { return anaRightY < 0.f ? -anaRightY : 0.f; }

  float ARALeft() const { return anaRightX < 0.f ? -anaRightX : 0.f; }

  float ARARight() const { return anaRightX > 0.f ? anaRightX : 0.f; }

  float ALTrigger() const { return anaLeftTrigger; }

  float ARTrigger() const { return anaRightTrigger; }

  float ADPUp() const { return dp_up ? 1.f : 0.f; }

  float ADPDown() const { return dp_down ? 1.f : 0.f; }

  float ADPLeft() const { return dp_left ? 1.f : 0.f; }

  float ADPRight() const { return dp_right ? 1.f : 0.f; }

  float AA() const { return a ? 1.f : 0.f; }

  float AB() const { return b ? 1.f : 0.f; }

  float AX() const { return x ? 1.f : 0.f; }

  float AY() const { return y ? 1.f : 0.f; }

  float AZ() const { return z ? 1.f : 0.f; }

  float AL() const { return lt_digital ? 1.f : 0.f; }

  float AR() const { return rt_digital ? 1.f : 0.f; }

  float AStart() const { return start ? 1.f : 0.f; }

  bool DLAUp() const { return kInput_AnalogOnThreshhold < anaLeftY ? true : false; }

  bool DLADown() const { return -kInput_AnalogOnThreshhold > anaLeftY ? true : false; }

  bool DLALeft() const { return -kInput_AnalogOnThreshhold > anaLeftX ? true : false; }

  bool DLARight() const { return kInput_AnalogOnThreshhold < anaLeftX ? true : false; }

  bool DRAUp() const { return kInput_AnalogOnThreshhold < anaRightY ? true : false; }

  bool DRADown() const { return -kInput_AnalogOnThreshhold > anaRightY ? true : false; }

  bool DRALeft() const { return -kInput_AnalogOnThreshhold > anaRightX ? true : false; }

  bool DRARight() const { return kInput_AnalogOnThreshhold < anaRightX ? true : false; }

  bool DLTrigger() const {
    return kInput_AnalogTriggerOnThreshhold < anaLeftTrigger ? true : false;
  }

  bool DRTrigger() const {
    return kInput_AnalogTriggerOnThreshhold < anaRightTrigger ? true : false;
  }

  bool DDPUp() const { return dp_up; }

  bool DDPDown() const { return dp_down; }

  bool DDPLeft() const { return dp_left; }

  bool DDPRight() const { return dp_right; }

  bool DA() const { return a; }

  bool DB() const { return b; }

  bool DX() const { return x; }

  bool DY() const { return y; }

  bool DZ() const { return z; }

  bool DL() const { return lt_digital; }

  bool DR() const { return rt_digital; }

  bool DStart() const { return start; }

  bool PLAUp() const { return kInput_AnalogOnThreshhold < anaLeftY; }

  bool PLADown() const { return -kInput_AnalogOnThreshhold > anaLeftY; }

  bool PLALeft() const { return -kInput_AnalogOnThreshhold > anaLeftX; }

  bool PLARight() const { return kInput_AnalogOnThreshhold < anaLeftX; }

  bool PRAUp() const { return kInput_AnalogOnThreshhold < anaRightY; }

  bool PRADown() const { return -kInput_AnalogOnThreshhold > anaRightY; }

  bool PRALeft() const { return -kInput_AnalogOnThreshhold > anaRightX; }

  bool PRARight() const { return kInput_AnalogOnThreshhold < anaRightX; }

  bool PLTrigger() const { return kInput_AnalogTriggerOnThreshhold < anaLeftTriggerP; }

  bool PRTrigger() const { return kInput_AnalogTriggerOnThreshhold < anaRightTriggerP; }

  bool PDPUp() const { return pdp_up; }

  bool PDPDown() const { return pdp_down; }

  bool PDPLeft() const { return pdp_left; }

  bool PDPRight() const { return pdp_right; }

  bool PA() const { return pa; }

  bool PB() const { return pb; }

  bool PX() const { return px; }

  bool PY() const { return py; }

  bool PZ() const { return pz; }

  bool PL() const { return plt_digital; }

  bool PR() const { return prt_digital; }

  bool PStart() const { return pstart; }

  float ALeftX() const { return ALARight() - ALALeft(); };
  float ALeftY() const { return ALADown() - ALAUp(); };
  float ARightX() const { return ARARight() - ARALeft(); };
  float ARightY() const { return ARADown() - ARAUp(); };
  float ALeftTrigger() const { return ALTrigger(); };
  float ARightTrigger() const { return ARTrigger(); };

  float dt;
  uint controllerIdx;
  float anaLeftX;
  float anaLeftY;
  float anaRightX;
  float anaRightY;
  float anaLeftTrigger;
  float anaRightTrigger;

  float anaLeftTriggerP;
  float anaRightTriggerP;

  union {
    struct {
      u8 stick_leftright: 1;
      u8 stick_updown: 1;
      u8 c_leftright: 1;
      u8 c_updown: 1;
      u8 a : 1;
      u8 b : 1;
      u8 x : 1;
      u8 y : 1;
    };
    u8 btns1;
  };
  union {
    struct {
      u8 z: 1;
      u8 lt_digital: 1;
      u8 rt_digital: 1;
      u8 dp_up: 1;
      u8 dp_right: 1;
      u8 dp_down: 1;
      u8 dp_left: 1;
      u8 start: 1;
    };
    u8 btns2;
  };
  union {
    struct {
      u8 pb : 1;
      u8 pa : 1;
      u8 py: 1;
      u8 px: 1;
      u8 pz: 1;
      u8 plt_digital: 1;
      u8 prt_digital: 1;
      u8 pdp_up: 1;
    };
    u8 btns3;
  };
  union {
    struct {
      u8 pdp_right : 1;
      u8 pdp_down : 1;
      u8 pdp_left: 1;
      u8 pstart: 1;
      u8 unk5: 1;
      u8 unk6: 1;
      u8 unk7: 1;
      u8 always_true: 1;
    };
    u8 btns4;
  };
};

static_assert(sizeof(CFinalInput) == 0x2C, "Incorrect size for CFinalInput");
