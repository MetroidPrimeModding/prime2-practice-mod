#pragma once

#include "GetField.hpp"

class CFinalInput {
public:
  static constexpr float kInput_AnalogOnThreshhold = 0.7f;
  static constexpr float kInput_AnalogTriggerOnThreshhold = 0.05f;

  float DeltaTime() const { return x0_dt; }

  int ControllerIdx() const { return x4_controllerIdx; }

  float ALAUp() const { return xc_anaLeftY > 0.f ? xc_anaLeftY : 0.f; }

  float ALADown() const { return xc_anaLeftY < 0.f ? -xc_anaLeftY : 0.f; }

  float ALALeft() const { return x8_anaLeftX < 0.f ? -x8_anaLeftX : 0.f; }

  float ALARight() const { return x8_anaLeftX > 0.f ? x8_anaLeftX : 0.f; }

  float ARAUp() const { return x14_anaRightY > 0.f ? x14_anaRightY : 0.f; }

  float ARADown() const { return x14_anaRightY < 0.f ? -x14_anaRightY : 0.f; }

  float ARALeft() const { return x10_anaRightX < 0.f ? -x10_anaRightX : 0.f; }

  float ARARight() const { return x10_anaRightX > 0.f ? x10_anaRightX : 0.f; }

  float ALTrigger() const { return x18_anaLeftTrigger; }

  float ARTrigger() const { return x1c_anaRightTrigger; }

  float ADPUp() const { return x2c_b31_DPUp ? 1.f : 0.f; }

  float ADPDown() const { return x2d_b25_DPDown ? 1.f : 0.f; }

  float ADPLeft() const { return x2d_b26_DPLeft ? 1.f : 0.f; }

  float ADPRight() const { return x2d_b24_DPRight ? 1.f : 0.f; }

  float AA() const { return x2c_b24_A ? 1.f : 0.f; }

  float AB() const { return x2c_b25_B ? 1.f : 0.f; }

  float AX() const { return x2c_b26_X ? 1.f : 0.f; }

  float AY() const { return x2c_b27_Y ? 1.f : 0.f; }

  float AZ() const { return x2c_b28_Z ? 1.f : 0.f; }

  float AL() const { return x2c_b29_L ? 1.f : 0.f; }

  float AR() const { return x2c_b30_R ? 1.f : 0.f; }

  float AStart() const { return x2d_b27_Start ? 1.f : 0.f; }

  bool DLAUp() const { return kInput_AnalogOnThreshhold < xc_anaLeftY ? true : false; }

  bool DLADown() const { return -kInput_AnalogOnThreshhold > xc_anaLeftY ? true : false; }

  bool DLALeft() const { return -kInput_AnalogOnThreshhold > x8_anaLeftX ? true : false; }

  bool DLARight() const { return kInput_AnalogOnThreshhold < x8_anaLeftX ? true : false; }

  bool DRAUp() const { return kInput_AnalogOnThreshhold < x14_anaRightY ? true : false; }

  bool DRADown() const { return -kInput_AnalogOnThreshhold > x14_anaRightY ? true : false; }

  bool DRALeft() const { return -kInput_AnalogOnThreshhold > x10_anaRightX ? true : false; }

  bool DRARight() const { return kInput_AnalogOnThreshhold < x10_anaRightX ? true : false; }

  bool DLTrigger() const {
    return kInput_AnalogTriggerOnThreshhold < x18_anaLeftTrigger ? true : false;
  }

  bool DRTrigger() const {
    return kInput_AnalogTriggerOnThreshhold < x1c_anaRightTrigger ? true : false;
  }

  bool DDPUp() const { return x2c_b31_DPUp; }

  bool DDPDown() const { return x2d_b25_DPDown; }

  bool DDPLeft() const { return x2d_b26_DPLeft; }

  bool DDPRight() const { return x2d_b24_DPRight; }

  bool DA() const { return x2c_b24_A; }

  bool DB() const { return x2c_b25_B; }

  bool DX() const { return x2c_b26_X; }

  bool DY() const { return x2c_b27_Y; }

  bool DZ() const { return x2c_b28_Z; }

  bool DL() const { return x2c_b29_L; }

  bool DR() const { return x2c_b30_R; }

  bool DStart() const { return x2d_b27_Start; }

  bool PLAUp() const { return kInput_AnalogOnThreshhold < xc_anaLeftY && x21_enableAnaLeftYP ? true : false; }

  bool PLADown() const { return -kInput_AnalogOnThreshhold > xc_anaLeftY && x21_enableAnaLeftYP ? true : false; }

  bool PLALeft() const { return -kInput_AnalogOnThreshhold > x8_anaLeftX && x20_enableAnaLeftXP ? true : false; }

  bool PLARight() const { return kInput_AnalogOnThreshhold < x8_anaLeftX && x20_enableAnaLeftXP ? true : false; }

  bool PRAUp() const { return kInput_AnalogOnThreshhold < x14_anaRightY && x23_enableAnaRightYP ? true : false; }

  bool PRADown() const { return -kInput_AnalogOnThreshhold > x14_anaRightY && x23_enableAnaRightYP ? true : false; }

  bool PRALeft() const { return -kInput_AnalogOnThreshhold > x10_anaRightX && x22_enableAnaRightXP ? true : false; }

  bool PRARight() const { return kInput_AnalogOnThreshhold < x10_anaRightX && x22_enableAnaRightXP ? true : false; }

  bool PLTrigger() const { return kInput_AnalogTriggerOnThreshhold < x24_anaLeftTriggerP ? true : false; }

  bool PRTrigger() const { return kInput_AnalogTriggerOnThreshhold < x28_anaRightTriggerP ? true : false; }

  bool PDPUp() const { return x2e_b27_PDPUp; }

  bool PDPDown() const { return x2e_b29_PDPDown; }

  bool PDPLeft() const { return x2e_b30_PDPLeft; }

  bool PDPRight() const { return x2e_b28_PDPRight; }

  bool PA() const { return x2d_b28_PA; }

  bool PB() const { return x2d_b29_PB; }

  bool PX() const { return x2d_b30_PX; }

  bool PY() const { return x2d_b31_PY; }

  bool PZ() const { return x2e_b24_PZ; }

  bool PL() const { return x2e_b25_PL; }

  bool PR() const { return x2e_b26_PR; }

  bool PStart() const { return x2e_b31_PStart; }

  float ALeftX() const { return ALARight() - ALALeft(); };
  float ALeftY() const { return ALADown() - ALAUp(); };
  float ARightX() const { return ARARight() - ARALeft(); };
  float ARightY() const { return ARADown() - ARAUp(); };
  float ALeftTrigger() const { return ALTrigger(); };
  float ARightTrigger() const { return ARTrigger(); };
private:

  float x0_dt;
  uint x4_controllerIdx;
  float x8_anaLeftX;
  float xc_anaLeftY;
  float x10_anaRightX;
  float x14_anaRightY;
  float x18_anaLeftTrigger;
  float x1c_anaRightTrigger;

  bool x20_enableAnaLeftXP;
  bool x21_enableAnaLeftYP;
  bool x22_enableAnaRightXP;
  bool x23_enableAnaRightYP;

  float x24_anaLeftTriggerP;
  float x28_anaRightTriggerP;

  union {
    struct {
      u8 x2c_b24_A : 1;
      u8 x2c_b25_B : 1;
      u8 x2c_b26_X : 1;
      u8 x2c_b27_Y : 1;
      u8 x2c_b28_Z : 1;
      u8 x2c_b29_L : 1;
      u8 x2c_b30_R : 1;
      u8 x2c_b31_DPUp : 1;
    };
    u8 btns1;
  };
  union {
    struct {
      u8 x2d_b24_DPRight : 1;
      u8 x2d_b25_DPDown : 1;
      u8 x2d_b26_DPLeft : 1;
      u8 x2d_b27_Start : 1;
      u8 x2d_b28_PA : 1;
      u8 x2d_b29_PB : 1;
      u8 x2d_b30_PX : 1;
      u8 x2d_b31_PY : 1;
    };
    u8 btns2;
  };
  union {
    struct {
      u8 x2e_b24_PZ : 1;
      u8 x2e_b25_PL : 1;
      u8 x2e_b26_PR : 1;
      u8 x2e_b27_PDPUp : 1;
      u8 x2e_b28_PDPRight : 1;
      u8 x2e_b29_PDPDown : 1;
      u8 x2e_b30_PDPLeft : 1;
      u8 x2e_b31_PStart : 1;
    };
    u8 btns3;
  };
};

static_assert(sizeof(CFinalInput) == 48, "Incorrect size for CFinalInput");
