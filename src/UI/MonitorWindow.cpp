#include "MonitorWindow.hpp"
#include "os.h"
#include "settings.hpp"
#include <imgui.h>
#include <prime/CGameGlobalObjects.hpp>
#include <prime/CGameState.hpp>
#include <prime/CRandom16.hpp>
#include <prime/CStateManager.hpp>

#define IMGUI_DEFINE_MATH_OPERATORS

#include "imgui_internal.h"
#include "stb_sprintf.h"

namespace GUI {
  void drawFrameTime();

  void drawMemoryUsage();

  void drawInput(CFinalInput *inputs);

  // void drawPos();
  //
  // void drawVelocity();
  //
  // void drawRotationalVelocity();

  void drawIGT();

  // void drawRoomTime();

  // void handleLoadBasedRoomTiming(double current_time);

  // void drawLoads();

  void drawRng();

  void drawMonitorWindow(CFinalInput *inputs) {
    if (!SETTINGS.OSD_show) {
      return;
    }
    {
      ImGui::SetNextWindowPos(ImVec2(630, 10), ImGuiCond_None, ImVec2(1, 0));
      ImGui::Begin("Monitor", nullptr,
                   ImGuiWindowFlags_NoResize | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoTitleBar |
                       ImGuiWindowFlags_NoInputs | ImGuiWindowFlags_NoNavInputs | ImGuiWindowFlags_NoNavFocus |
                       ImGuiWindowFlags_NoNav | ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoMove |
                       ImGuiWindowFlags_NoDecoration |
                       //        ImGuiWindowFlags_NoBackground |
                       ImGuiFocusedFlags_None // just for conveneint commenting in/out
      );
      if (SETTINGS.OSD_showIGT) {
        drawIGT();
      }
      //TODO: drawRoomTime();
      if (SETTINGS.OSD_showPos) {
        //TODO: drawPos();
      }
      if (SETTINGS.OSD_showVelocity) {
        //TODO: drawVelocity();
      }
      if (SETTINGS.OSD_showRotationalVelocity) {
        //TODO: drawRotationalVelocity();
      }
      if (SETTINGS.OSD_showFrameTime) {
        drawFrameTime();
      }
      if (SETTINGS.OSD_showRng) {
        drawRng();
      }
      if (SETTINGS.OSD_showMemoryGraph || SETTINGS.OSD_showMemoryInfo) {
        // TODO: drawMemoryUsage();
      }

      ImGui::End();
    }

    if (SETTINGS.OSD_showLoads) {
      //TODO: drawLoads();
    }

    if (SETTINGS.OSD_showInput) {
      drawInput(inputs);
    }
  }

  void drawIGT() {
    CGameGlobalObjects *globals = ((CGameGlobalObjects *)0x80457798);
    CGameState *gameState = globals->getGameState();
    if (gameState) {
      double time = gameState->PlayTime();
      int ms = (int)(time * 1000.0) % 1000;
      int seconds = (int)time % 60;
      int minutes = ((int)time / 60) % 60;
      int hours = ((int)time / 60 / 60) % 60;
      ImGui::Text("%02d:%02d:%02d.%03d", hours, minutes, seconds, ms);
    }
  }

  s64 lastFrame;
  constexpr u32 GRAPH_LENGTH = 32;
  float frames[GRAPH_LENGTH];

  void drawFrameTime() {
    s64 thisFrame = OSGetTime();
    s64 diff = thisFrame - lastFrame;
    lastFrame = thisFrame;

    float ms = static_cast<float>(diff) / static_cast<float>(TICKS_PER_MS);
    for (int i = 1; i < GRAPH_LENGTH; i++) {
      frames[i - 1] = frames[i];
    }
    frames[GRAPH_LENGTH - 1] = ms;

    char title[32];
    stbsp_snprintf(title, sizeof(title), "Frame time: %02.2f", ms);
    ImGui::PlotLines("", frames, GRAPH_LENGTH, 0, title, 0.f, 32.f, ImVec2(0, 40.0f));
  }

  void drawInput(CFinalInput *inputs) {
    CFinalInput *p1 = &inputs[0];

    ImGui::SetNextWindowPos(ImVec2(630, 450), ImGuiCond_None, ImVec2(1, 1));
    ImGui::Begin("Input", nullptr,
                 ImGuiWindowFlags_NoResize | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoTitleBar |
                     ImGuiWindowFlags_NoInputs | ImGuiWindowFlags_NoNavInputs | ImGuiWindowFlags_NoNavFocus |
                     ImGuiWindowFlags_NoNav | ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoMove |
                     ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoBackground |
                     ImGuiFocusedFlags_None // just for conveneint commenting in/out
    );

    ImDrawList *dl = ImGui::GetWindowDrawList();
    ImVec2 p = ImGui::GetCursorScreenPos();

    constexpr float leftStickRadius = 15;
    ImVec2 leftStickCenter = p + ImVec2(15, 35);
    constexpr float dpadRadius = 10;
    constexpr float dpadWidth = 4;
    ImVec2 dpadCenter = p + ImVec2(40, 60);
    constexpr float rightStickRadius = 10;
    ImVec2 rightStickCenter = p + ImVec2(80, 60);
    constexpr float startButtonRadius = 4;
    ImVec2 startButtonCenter = p + ImVec2(60, 40);
    constexpr float aButtonRadius = 8;
    ImVec2 aButtonCenter = p + ImVec2(110, 38);
    constexpr float bButtonRadius = 4;
    ImVec2 bButtonCenter = aButtonCenter + ImVec2(-12, 8);
    constexpr float xButtonRadius = 4;
    ImVec2 xButtonCenter = aButtonCenter + ImVec2(12, -8);
    constexpr float yButtonRadius = 4;
    ImVec2 yButtonCenter = aButtonCenter + ImVec2(-6, -12);
    constexpr float triggerWidth = leftStickRadius * 2;
    constexpr float triggerHeight = 4;
    ImVec2 lCenter = leftStickCenter + ImVec2(0, -25);
    ImVec2 rCenter = ImVec2(aButtonCenter.x, lCenter.y);

    constexpr ImU32 stickGray = IM_COL32(150, 150, 150, 255);
    constexpr ImU32 darkGray = IM_COL32(60, 60, 60, 255);
    constexpr ImU32 red = IM_COL32(255, 0, 0, 255);
    constexpr ImU32 green = IM_COL32(0, 255, 0, 255);

    // left stick
    {
      dl->AddCircleFilled(leftStickCenter, leftStickRadius, stickGray, 8);
      float x = p1->ALeftX();
      float y = p1->ALeftY();
      dl->AddLine(leftStickCenter, leftStickCenter + ImVec2(x * leftStickRadius, y * leftStickRadius), red);
    }

    // right stick
    {
      dl->AddCircleFilled(rightStickCenter, rightStickRadius, stickGray, 8);
      float x = p1->ARightX();
      float y = p1->ARightY();
      dl->AddLine(rightStickCenter, rightStickCenter + ImVec2(x * rightStickRadius, y * rightStickRadius), red);
    }

    // dpad
    {
      constexpr float halfWidth = dpadWidth / 2;
      dl->AddRectFilled(dpadCenter + ImVec2(-halfWidth, -dpadRadius), dpadCenter + ImVec2(halfWidth, dpadRadius),
                        stickGray);

      dl->AddRectFilled(dpadCenter + ImVec2(-dpadRadius, -halfWidth), dpadCenter + ImVec2(dpadRadius, halfWidth),
                        stickGray);

      if (p1->DDPUp()) {
        dl->AddRectFilled(dpadCenter + ImVec2(-halfWidth, -dpadRadius), dpadCenter + ImVec2(halfWidth, -dpadRadius / 2),
                          red);
      }

      if (p1->DDPDown()) {
        dl->AddRectFilled(dpadCenter + ImVec2(-halfWidth, dpadRadius), dpadCenter + ImVec2(halfWidth, dpadRadius / 2),
                          red);
      }

      if (p1->DDPLeft()) {
        dl->AddRectFilled(dpadCenter + ImVec2(-dpadRadius, -halfWidth), dpadCenter + ImVec2(-dpadRadius / 2, halfWidth),
                          red);
      }

      if (p1->DDPRight()) {
        dl->AddRectFilled(dpadCenter + ImVec2(dpadRadius, -halfWidth), dpadCenter + ImVec2(dpadRadius / 2, halfWidth),
                          red);
      }
    }

    // buttons
    {
      // start
      dl->AddCircleFilled(startButtonCenter, startButtonRadius, p1->DStart() ? red : stickGray, 16);

      // a
      dl->AddCircleFilled(aButtonCenter, aButtonRadius, p1->DA() ? green : stickGray, 16);

      // b
      dl->AddCircleFilled(bButtonCenter, bButtonRadius, p1->DB() ? red : stickGray, 16);

      // x
      dl->AddCircleFilled(xButtonCenter, xButtonRadius, p1->DX() ? red : stickGray, 16);

      // y
      dl->AddCircleFilled(yButtonCenter, yButtonRadius, p1->DY() ? red : stickGray, 16);
    }

    // triggers
    {
      float halfTriggerWidth = triggerWidth / 2;
      ImVec2 lStart = lCenter - ImVec2(halfTriggerWidth, 0);
      ImVec2 lEnd = lCenter + ImVec2(halfTriggerWidth, triggerHeight);
      float lValue = triggerWidth * p1->ALTrigger();

      dl->AddRectFilled(lStart, lStart + ImVec2(lValue, triggerHeight), p1->DL() ? red : stickGray);
      dl->AddRectFilled(lStart + ImVec2(lValue, 0), lEnd, darkGray);

      ImVec2 rStart = rCenter - ImVec2(halfTriggerWidth, 0);
      ImVec2 rEnd = rCenter + ImVec2(halfTriggerWidth, triggerHeight);
      float rValue = triggerWidth * p1->ARTrigger();

      dl->AddRectFilled(rEnd - ImVec2(rValue, triggerHeight), rEnd, p1->DR() ? red : stickGray);
      dl->AddRectFilled(rStart, rEnd - ImVec2(rValue, 0), darkGray);
    }

    ImGui::Dummy(ImVec2(130, 80));
    ImGui::End();
  }

  void drawRng() {
    CStateManager *stateManager = CStateManager::instance();
    CRandom16 *rng = stateManager->GetRandom();
    ImGui::Text("RNG: %08x", rng->GetSeed());
  }
} // namespace GUI
