#pragma once
#include "MathyTypes.hpp"

class CPlayer;

namespace GUI {
  void drawPlayerMenu();

  void savePos();
  void loadPos();

  void requestPosition(const CTransform4f &pos);
  void handlePosIfNeeded(CPlayer *player);
}

