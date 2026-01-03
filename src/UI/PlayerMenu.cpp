#include "PlayerMenu.hpp"
#include "WarpMenu.h"
#include "imgui.h"
#include "prime/CGameArea.hpp"
#include "prime/CGameState.hpp"
#include "prime/CPlayer.hpp"
#include "prime/CWorld.hpp"
#include "prime/CWorldState.hpp"

#include <optional>

namespace GUI {
  CTransform4f savedPos{CTransform4f::Identity()};
  CVector3f savedVelocity{};
  CVector3f savedAngularVelocity{};
  // TODO: save world/area if not set
  // u32 savedWorldAssetID{0};
  // u32 savedAreaAssetID{0};

  std::optional<CTransform4f> requestedPos;
  std::optional<CVector3f> requestedVelocity{};
  std::optional<CVector3f> requestedAngularVelocity{};

  void drawPlayerMenu() {
    CPlayer *player = g_CStateManager.GetPlayer();
    CPlayerState *playerState = *gpGameState->GetPlayerState(0);

    // TODO: save world/area if not set
    // CGameState *gameState = gpGameState;
    // u32 currentWorldAssetID = gameState->CurrentWorldAssetId();
    // u32 currentAreaAssetID = 0;
    // CWorldState &worldState = gpGameState->CurrentWorldState();
    // CWorld *world = g_CStateManager.GetWorld();
    // if (world) {
    //   currentAreaAssetID = world->areas()->ptr[gameState->CurrentWorldState().x4_areaId.id].ptr->AssetId();
    // }
    // if (savedWorldAssetID == 0 || savedWorldAssetID == 0xFFFFFFFF) {
    //   savedWorldAssetID = currentWorldAssetID;
    // }
    // if (savedAreaAssetID == 0 || savedAreaAssetID == 0xFFFFFFFF) {
    //   savedAreaAssetID = currentAreaAssetID;
    // }

    ImGuiSliderFlags flags = ImGuiSliderFlags_None
                             | ImGuiSliderFlags_NoRoundToFormat;
    if (ImGui::TreeNode("Player")) {
      ImGui::Text("Saved position:");
      if (ImGui::Button("Save")) {
        savePos();
      }
      ImGui::SameLine();
      if (ImGui::Button("Load")) {
        loadPos();
      }
      ImGui::SameLine();
      // if (ImGui::Button("Warp")) {
      //   warp(savedWorldAssetID, savedAreaAssetID);
      // }
      // const char *worldName = getNameForWorldAsset(savedWorldAssetID);
      // const char *areaName = getNameForAreaAsset(savedWorldAssetID, savedAreaAssetID);
      // ImGui::Text("%s", worldName);
      // ImGui::Text("%s", areaName);
      ImGui::Text("%.2fx, %.2fy, %.2fz", savedPos.x, savedPos.y, savedPos.z);

      float xyz[3] = {
          player->getTransform()->x,
          player->getTransform()->y,
          player->getTransform()->z
      };
      if (ImGui::DragFloat3("##pos", xyz, 1.f, -FLT_MAX, FLT_MAX, "%.3f", flags)) {
        CTransform4f requestedTransform = *player->getTransform();
        requestedTransform.x = xyz[0];
        requestedTransform.y = xyz[1];
        requestedTransform.z = xyz[2];
        *player->getTransform() = requestedTransform;
        player->getPos()->x = requestedTransform.x;
        player->getPos()->y = requestedTransform.y;
        player->getPos()->z = requestedTransform.z;
        requestPosition(requestedTransform);
      }

      if (ImGui::Button("IS on")) {
        player->GetAngularVelocity()->x = NAN;
        player->GetAngularVelocity()->y = NAN;
        player->GetAngularVelocity()->z = NAN;
      }
      ImGui::SameLine();
      ImGui::Text("Use while morphed");

      // TODO: lightshow fix
      // auto *bobber = player->getCameraBobber();
      // if (ImGui::Button("Lightshow fix")) {
      //   player->GetAngularVelocity()->x = 0;
      //   player->GetAngularVelocity()->y = 0;
      //   player->GetAngularVelocity()->z = 0;
      //
      //
      //   *(bobber->getTargetBobMagnitude()) = 0;
      //   *(bobber->getBobMagnitude()) = 0;
      //   *(bobber->getBobTimescale()) = 0;
      //   *(bobber->getBobTime()) = 0;
      //   bobber->getCameraBobTransform()->x = 0;
      //   bobber->getCameraBobTransform()->y = 0;
      //   bobber->getCameraBobTransform()->z = 0;
      // }
      // ImGui::SameLine();
      // ImGui::Text("Use while unmorphed");

      // TODO: Floaty
      // bool floaty = player->getFluidCounter() >= 1 && *player->getDepthUnderWater() >= 2;
      // if (ImGui::Checkbox("Floaty", &floaty)) {
      //   if (floaty) {
      //     player->setFluidCounter(1);
      //     *player->getDepthUnderWater() = 20;
      //   } else {
      //     player->setFluidCounter(0);
      //     *player->getDepthUnderWater() = 0;
      //   };
      // }

      // int fluidCounter = (int) player->getFluidCounter();
      // if (ImGui::DragInt("Water box count", &fluidCounter, 1, 0, 0b11)) {
      //   player->setFluidCounter((u32) fluidCounter);
      // }
      // ImGui::DragFloat("Water depth", player->getDepthUnderWater(), 1.f, -FLT_MAX, FLT_MAX, "%.3f", flags);

      ImGui::TreePop();
    }
  }

  void requestPosition(const CTransform4f &pos) {
    requestedPos = pos;
  }

  void loadPos() {
    requestedPos = savedPos;
    requestedVelocity = savedVelocity;
    requestedAngularVelocity = savedAngularVelocity;
  }

  void savePos() {
    CPlayer *player = g_CStateManager.GetPlayer();
    // CGameState *gameState = gpGameState;
    // TODO: save world
    // u32 currentWorldAssetID = gameState->CurrentWorldAssetId();
    // u32 currentAreaAssetID = 0;
    // CWorldState &worldState = gpGameState->CurrentWorldState();
    // CWorld *world = g_CStateManager.GetWorld();
    // if (!world) return;
    // currentAreaAssetID = world->areas()->ptr[gameState->CurrentWorldState().x4_areaId.id].ptr->AssetId();

    savedPos = *player->getTransform();
    savedVelocity = *player->GetVelocity();
    savedAngularVelocity = *player->GetAngularVelocity();
    // savedWorldAssetID = currentWorldAssetID;
    // savedAreaAssetID = currentAreaAssetID;
  }

  void handlePosIfNeeded(CPlayer *player) {
    if (requestedPos.has_value()) {
      *player->getTransform() = requestedPos.value();
      player->getPos()->x = requestedPos.value().x;
      player->getPos()->y = requestedPos.value().y;
      player->getPos()->z = requestedPos.value().z;
      requestedPos.reset();
    }
    if (requestedVelocity.has_value()) {
      *player->GetVelocity() = requestedVelocity.value();
      requestedVelocity.reset();
    }
    if (requestedAngularVelocity.has_value()) {
      *player->GetAngularVelocity() = requestedAngularVelocity.value();
      requestedAngularVelocity.reset();
    }
  }
}

