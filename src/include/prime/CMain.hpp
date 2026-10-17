#pragma once

#include "CGameGlobalObjects.hpp"
#include "GetField.hpp"

typedef enum EFlowState {
  EFlowState_None,
  EFlowState_WinBad,
  EFlowState_WinGood,
  EFlowState_WinBest,
  EFlowState_LoseGame,
  EFlowState_Default,
  EFlowState_StateSetter,
} EFlowState;

class CMain {
public:
  void StreamNewGameState(bool skipCheckpoint);
  void SetFlowState(EFlowState s) { *(GetField<EFlowState>(this, 0x58)) = s; };
  void DrawDebugMetrics(double dt, class CStopwatch &stopwatch);
  CGameGlobalObjects* GetGlobalObjects() { return *GetField<CGameGlobalObjects *>(this, 0x54); }
};

// extern CMain g_Main_Body;
extern CMain* gpMain;
