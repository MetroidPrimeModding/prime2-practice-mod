#include "InventoryMenu.hpp"
#include <imgui.h>

#include "prime/CGameState.hpp"
#include "prime/CPlayerState.hpp"
#include "prime/CStateManager.hpp"

static constexpr CPlayerState::EItemType GeneralItems[] = {
    CPlayerState::EItemType::EnergyTank,   CPlayerState::EItemType::CombatVisor,
    CPlayerState::EItemType::ScanVisor,    CPlayerState::EItemType::DarkVisor,
    CPlayerState::EItemType::EchoVisor,    CPlayerState::EItemType::GrappleBeam,
    CPlayerState::EItemType::GravityBoost, CPlayerState::EItemType::SpaceJumpBoots,
    CPlayerState::EItemType::ScrewAttack,  CPlayerState::EItemType::VariaSuit,
    CPlayerState::EItemType::DarkSuit,     CPlayerState::EItemType::LightSuit,
};

static constexpr CPlayerState::EItemType WeaponItems[] = {
    CPlayerState::EItemType::PowerBeam,       CPlayerState::EItemType::Missile,
    CPlayerState::EItemType::DarkBeam,        CPlayerState::EItemType::DarkAmmo,
    CPlayerState::EItemType::LightBeam,       CPlayerState::EItemType::LightAmmo,
    CPlayerState::EItemType::AnnihilatorBeam, CPlayerState::EItemType::SuperMissile,
    CPlayerState::EItemType::SeekerLauncher,  CPlayerState::EItemType::Darkburst,
    CPlayerState::EItemType::Sunburst,        CPlayerState::EItemType::SonicBoom,
    CPlayerState::EItemType::ChargeBeam,
};

static constexpr CPlayerState::EItemType MorphBallItems[] = {
    CPlayerState::EItemType::PowerBomb, CPlayerState::EItemType::MorphBall,  CPlayerState::EItemType::MorphBallBomb,
    CPlayerState::EItemType::BoostBall, CPlayerState::EItemType::SpiderBall, CPlayerState::EItemType::CannonBall,
};

static constexpr CPlayerState::EItemType KeyItems[] = {
    CPlayerState::EItemType::DarkAgonKey1,   CPlayerState::EItemType::DarkAgonKey2,
    CPlayerState::EItemType::DarkAgonKey3,   CPlayerState::EItemType::DarkTorvusKey1,
    CPlayerState::EItemType::DarkTorvusKey2, CPlayerState::EItemType::DarkTorvusKey3,
    CPlayerState::EItemType::IngHiveKey1,    CPlayerState::EItemType::IngHiveKey2,
    CPlayerState::EItemType::IngHiveKey3,    CPlayerState::EItemType::SkyTempleKey1,
    CPlayerState::EItemType::SkyTempleKey2,  CPlayerState::EItemType::SkyTempleKey3,
    CPlayerState::EItemType::SkyTempleKey4,  CPlayerState::EItemType::SkyTempleKey5,
    CPlayerState::EItemType::SkyTempleKey6,  CPlayerState::EItemType::SkyTempleKey7,
    CPlayerState::EItemType::SkyTempleKey8,  CPlayerState::EItemType::SkyTempleKey9,
};

static constexpr CPlayerState::EItemType MiscItems[] = {
    CPlayerState::EItemType::AmberTranslator,
    CPlayerState::EItemType::CobaltTranslator,
    CPlayerState::EItemType::EmeraldTranslator,
    CPlayerState::EItemType::VioletTranslator,
    CPlayerState::EItemType::EnergyTransferModulePickup,
    CPlayerState::EItemType::EnergyTransferModuleInventory,
};

struct SItemAmt {
  CPlayerState::EItemType item;
  int count{1};
};

static constexpr SItemAmt StartingItems[] = {
    {CPlayerState::EItemType::VariaSuit},   {CPlayerState::EItemType::PowerBeam}, {CPlayerState::EItemType::ChargeBeam},
    {CPlayerState::EItemType::CombatVisor}, {CPlayerState::EItemType::ScanVisor}, {CPlayerState::EItemType::MorphBall},
};

static constexpr SItemAmt ILSItems[] = {
    {CPlayerState::EItemType::EnergyTank, 0}, {CPlayerState::EItemType::Missile, 5},
    {CPlayerState::EItemType::ChargeBeam},

    {CPlayerState::EItemType::MorphBallBomb}, {CPlayerState::EItemType::BoostBall},
    {CPlayerState::EItemType::SpiderBall},    {CPlayerState::EItemType::SpaceJumpBoots},
};

static constexpr SItemAmt AnyPercentItems[] = {
    {CPlayerState::EItemType::EnergyTank, 2}, {CPlayerState::EItemType::Missile, 25},

    {CPlayerState::EItemType::MorphBall},     {CPlayerState::EItemType::MorphBallBomb},
    {CPlayerState::EItemType::BoostBall},     {CPlayerState::EItemType::PowerBomb, 3},
    {CPlayerState::EItemType::ScrewAttack},

    {CPlayerState::EItemType::VariaSuit},     {CPlayerState::EItemType::LightSuit},

    {CPlayerState::EItemType::DarkBeam},      {CPlayerState::EItemType::LightBeam},

    {CPlayerState::EItemType::DarkVisor},     {CPlayerState::EItemType::GravityBoost},
};

static constexpr SItemAmt HundredPercentItems[] = {
    {CPlayerState::EItemType::PowerBeam},        {CPlayerState::EItemType::DarkBeam},
    {CPlayerState::EItemType::LightBeam},        {CPlayerState::EItemType::AnnihilatorBeam},
    {CPlayerState::EItemType::SuperMissile},     {CPlayerState::EItemType::Darkburst},
    {CPlayerState::EItemType::Sunburst},         {CPlayerState::EItemType::SonicBoom},
    {CPlayerState::EItemType::CombatVisor},      {CPlayerState::EItemType::ScanVisor},
    {CPlayerState::EItemType::DarkVisor},        {CPlayerState::EItemType::EchoVisor},
    {CPlayerState::EItemType::VariaSuit},        {CPlayerState::EItemType::DarkSuit},
    {CPlayerState::EItemType::LightSuit},        {CPlayerState::EItemType::MorphBall},
    {CPlayerState::EItemType::BoostBall},        {CPlayerState::EItemType::SpiderBall},
    {CPlayerState::EItemType::MorphBallBomb},    {CPlayerState::EItemType::ChargeBeam},
    {CPlayerState::EItemType::GrappleBeam},      {CPlayerState::EItemType::SpaceJumpBoots},
    {CPlayerState::EItemType::GravityBoost},     {CPlayerState::EItemType::SeekerLauncher},
    {CPlayerState::EItemType::ScrewAttack},      {CPlayerState::EItemType::EnergyTransferModulePickup},
    {CPlayerState::EItemType::SkyTempleKey1},    {CPlayerState::EItemType::SkyTempleKey2},
    {CPlayerState::EItemType::SkyTempleKey3},    {CPlayerState::EItemType::DarkAgonKey1},
    {CPlayerState::EItemType::DarkAgonKey2},     {CPlayerState::EItemType::DarkAgonKey3},
    {CPlayerState::EItemType::DarkTorvusKey1},   {CPlayerState::EItemType::DarkTorvusKey2},
    {CPlayerState::EItemType::DarkTorvusKey3},   {CPlayerState::EItemType::IngHiveKey1},
    {CPlayerState::EItemType::IngHiveKey2},      {CPlayerState::EItemType::IngHiveKey3},
    {CPlayerState::EItemType::EnergyTank, 14},   {CPlayerState::EItemType::PowerBomb, 8},
    {CPlayerState::EItemType::Missile, 255},     {CPlayerState::EItemType::DarkAmmo, 250},
    {CPlayerState::EItemType::LightAmmo, 250},   {CPlayerState::EItemType::VioletTranslator},
    {CPlayerState::EItemType::AmberTranslator},  {CPlayerState::EItemType::EmeraldTranslator},
    {CPlayerState::EItemType::CobaltTranslator}, {CPlayerState::EItemType::SkyTempleKey4},
    {CPlayerState::EItemType::SkyTempleKey5},    {CPlayerState::EItemType::SkyTempleKey6},
    {CPlayerState::EItemType::SkyTempleKey7},    {CPlayerState::EItemType::SkyTempleKey8},
    {CPlayerState::EItemType::SkyTempleKey9},    {CPlayerState::EItemType::EnergyTransferModuleInventory},
};

namespace GUI {
  void RenderItemType(CPlayerState *playerState, CPlayerState::EItemType itemType);
  const char *ItemTypeToName(CPlayerState::EItemType type);
  static inline void RenderItemsDualColumn(CPlayerState *playerState, const CPlayerState::EItemType *items, int start,
                                           int end);

  void clearItems(CPlayerState *playerState);

  void refillItems(CPlayerState *playerState);

  void drawInventoryMenu() {
    CGameState *gameState = gpGameState;
    if (!gameState) return;
    auto playerState = *gameState->GetPlayerState(0);
    if (!playerState) return;

    if (ImGui::TreeNode("Inventory")) {
      if (ImGui::Button("Refill")) {
        refillItems(playerState);
      }
      ImGui::SameLine();
      if (ImGui::Button("All")) {
        for (auto v : HundredPercentItems)
          playerState->ReInitializePowerUp(v.item, v.count);
        refillItems(playerState);
      }
      ImGui::SameLine();
      if (ImGui::Button("None")) {
        clearItems(playerState);
        refillItems(playerState);
      }
      ImGui::SameLine();
      if (ImGui::Button("Start")) {
        clearItems(playerState);
        for (auto v : StartingItems)
          playerState->ReInitializePowerUp(v.item, v.count);
        refillItems(playerState);
      }
      ImGui::SameLine();
      if (ImGui::Button("Any%")) {
        clearItems(playerState);
        for (auto v : ILSItems)
          playerState->ReInitializePowerUp(v.item, v.count);
        for (auto v : AnyPercentItems)
          playerState->ReInitializePowerUp(v.item, v.count);
        for (auto v : KeyItems)
          playerState->ReInitializePowerUp(v, 1);
        refillItems(playerState);
      }
      ImGui::SameLine();
      if (ImGui::Button("ILS")) {
        clearItems(playerState);
        for (auto v : StartingItems)
          playerState->ReInitializePowerUp(v.item, v.count);
        for (auto v : ILSItems)
          playerState->ReInitializePowerUp(v.item, v.count);
        refillItems(playerState);
      }

      if (ImGui::BeginTabBar("Items")) {
        if (ImGui::BeginTabItem("General")) {
          RenderItemType(playerState, GeneralItems[0]); // full width
          RenderItemsDualColumn(playerState, GeneralItems, 1, sizeof(GeneralItems) / sizeof(GeneralItems[0]));
          ImGui::EndTabItem();
        }
        if (ImGui::BeginTabItem("Weapons")) {
          RenderItemType(playerState, WeaponItems[0]); // full width
          RenderItemsDualColumn(playerState, WeaponItems, 1, sizeof(WeaponItems) / sizeof(WeaponItems[0]));
          ImGui::EndTabItem();
        }
        if (ImGui::BeginTabItem("Morph Ball")) {
          RenderItemType(playerState, MorphBallItems[0]); // full width
          RenderItemsDualColumn(playerState, MorphBallItems, 1, sizeof(MorphBallItems) / sizeof(MorphBallItems[0]));
          ImGui::EndTabItem();
        }
        if (ImGui::BeginTabItem("Keys")) {
          ImGui::Text("NOTE: This doesn't affect layers");
          RenderItemsDualColumn(playerState, KeyItems, 0, sizeof(KeyItems) / sizeof(KeyItems[0]));
          ImGui::EndTabItem();
        }
        if (ImGui::BeginTabItem("Misc")) {
          ImGui::Text("NOTE: This doesn't affect layers");
          RenderItemsDualColumn(playerState, MiscItems, 0, sizeof(MiscItems) / sizeof(MiscItems[0]));
          ImGui::EndTabItem();
        }
        ImGui::EndTabBar();
      }

      ImGui::TreePop();
    }
  }

  void refillItems(CPlayerState *playerState) {
    for (int i = 0; i < int(CPlayerState::EItemType::Max); i++) {
      auto itemType = static_cast<CPlayerState::EItemType>(i);
      u32 maxValue = CPlayerState::GetPowerUpMaxValue(itemType);
      playerState->ResetAndIncrPickUp(itemType, maxValue);
    }
  }

  void clearItems(CPlayerState *playerState) {
    for (int i = 0; i < int(CPlayerState::EItemType::Max); i++) {
      auto itemType = static_cast<CPlayerState::EItemType>(i);
      playerState->ReInitializePowerUp(itemType, 0);
    }
  }

  void RenderItemType(CPlayerState *playerState, CPlayerState::EItemType itemType) {
    u32 maxValue = CPlayerState::GetPowerUpMaxValue(itemType);
    const char *name = ItemTypeToName(itemType);
    if (maxValue == 1) {
      bool enabled = playerState->GetItemCapacity(itemType) == 1;
      if (ImGui::Checkbox(name, &enabled)) {
        if (enabled) {
          playerState->ReInitializePowerUp(itemType, 1);
          playerState->ResetAndIncrPickUp(itemType, 1);
        } else {
          playerState->ReInitializePowerUp(itemType, 0);
        }
        // if (itemType == CPlayerState::EItemType::VariaSuit || itemType == CPlayerState::EItemType::PowerSuit ||
        //     itemType == CPlayerState::EItemType::GravitySuit || itemType == CPlayerState::EItemType::PhazonSuit) {
        //   // TODO
        //   //          g_StateManager->Player()->AsyncLoadSuit(*g_StateManager);
        //     }
      }
    } else if (maxValue > 1) {
      int capacity = int(playerState->GetItemCapacity(itemType));
      float speed = 1;
      if (maxValue > 100) {
        speed = 5;
      }
      if (ImGui::DragInt(name, &capacity, speed, 0, int(maxValue), "%d", ImGuiSliderFlags_AlwaysClamp)) {
        playerState->ReInitializePowerUp(itemType, u32(capacity));
        playerState->ResetAndIncrPickUp(itemType, u32(capacity));
      }
    }
  }

  const char *ItemTypeToName(CPlayerState::EItemType type) {
    switch (type) {
    case CPlayerState::EItemType::PowerBeam:
      return "Power Beam";
    case CPlayerState::EItemType::DarkBeam:
      return "Dark Beam";
    case CPlayerState::EItemType::LightBeam:
      return "Light Beam";
    case CPlayerState::EItemType::AnnihilatorBeam:
      return "Annihilator Beam";
    case CPlayerState::EItemType::SuperMissile:
      return "Super Missile";
    case CPlayerState::EItemType::Darkburst:
      return "Darkburst";
    case CPlayerState::EItemType::Sunburst:
      return "Sunburst";
    case CPlayerState::EItemType::SonicBoom:
      return "Sonic Boom";
    case CPlayerState::EItemType::CombatVisor:
      return "Combat Visor";
    case CPlayerState::EItemType::ScanVisor:
      return "Scan Visor";
    case CPlayerState::EItemType::EchoVisor:
      return "Echo Visor";
    case CPlayerState::EItemType::DarkVisor:
      return "Dark Visor";
    case CPlayerState::EItemType::VariaSuit:
      return "Varia Suit";
    case CPlayerState::EItemType::DarkSuit:
      return "Dark Suit";
    case CPlayerState::EItemType::LightSuit:
      return "Light Suit";
    case CPlayerState::EItemType::MorphBall:
      return "Morph Ball";
    case CPlayerState::EItemType::BoostBall:
      return "Boost Ball";
    case CPlayerState::EItemType::SpiderBall:
      return "Spider Ball";
    case CPlayerState::EItemType::MorphBallBomb:
      return "Morph Ball Bomb";
    case CPlayerState::EItemType::LightBomb:
      return "Light Bomb";
    case CPlayerState::EItemType::DarkBomb:
      return "Dark Bomb";
    case CPlayerState::EItemType::AnnihilatorBomb:
      return "Annihilator Bomb";
    case CPlayerState::EItemType::ChargeBeam:
      return "Charge Beam";
    case CPlayerState::EItemType::GrappleBeam:
      return "Grapple Beam";
    case CPlayerState::EItemType::SpaceJumpBoots:
      return "Space Jump Boots";
    case CPlayerState::EItemType::GravityBoost:
      return "Gravity Boost";
    case CPlayerState::EItemType::SeekerLauncher:
      return "Seeker Launcher";
    case CPlayerState::EItemType::ScrewAttack:
      return "Screw Attack";
    case CPlayerState::EItemType::EnergyTransferModulePickup:
      return "Energy Transfer Module Pickup";
    case CPlayerState::EItemType::SkyTempleKey1:
      return "Sky Temple Key 1";
    case CPlayerState::EItemType::SkyTempleKey2:
      return "Sky Temple Key 2";
    case CPlayerState::EItemType::SkyTempleKey3:
      return "Sky Temple Key 3";
    case CPlayerState::EItemType::DarkAgonKey1:
      return "Dark Agon Key 1";
    case CPlayerState::EItemType::DarkAgonKey2:
      return "Dark Agon Key 2";
    case CPlayerState::EItemType::DarkAgonKey3:
      return "Dark Agon Key 3";
    case CPlayerState::EItemType::DarkTorvusKey1:
      return "Dark Torvus Key 1";
    case CPlayerState::EItemType::DarkTorvusKey2:
      return "Dark Torvus Key 2";
    case CPlayerState::EItemType::DarkTorvusKey3:
      return "Dark Torvus Key 3";
    case CPlayerState::EItemType::IngHiveKey1:
      return "Ing Hive Key 1";
    case CPlayerState::EItemType::IngHiveKey2:
      return "Ing Hive Key 2";
    case CPlayerState::EItemType::IngHiveKey3:
      return "Ing Hive Key 3";
    case CPlayerState::EItemType::HealthRefill:
      return "Health Refill";
    case CPlayerState::EItemType::EnergyTank:
      return "Energy Tank";
    case CPlayerState::EItemType::PowerBomb:
      return "Power Bomb";
    case CPlayerState::EItemType::Missile:
      return "Missile";
    case CPlayerState::EItemType::DarkAmmo:
      return "Dark Ammo";
    case CPlayerState::EItemType::LightAmmo:
      return "Light Ammo";
    case CPlayerState::EItemType::ItemPercentage:
      return "Item Percentage";
    case CPlayerState::EItemType::NumPlayersJoined:
      return "Num Players Joined";
    case CPlayerState::EItemType::NumPlayersInOptionsMenu:
      return "Num Players In Options Menu";
    case CPlayerState::EItemType::MiscCounter3:
      return "Misc Counter 3";
    case CPlayerState::EItemType::MiscCounter4:
      return "Misc Counter 4";
    case CPlayerState::EItemType::SwitchWeaponPower:
      return "Switch Weapon Power";
    case CPlayerState::EItemType::SwitchWeaponDark:
      return "Switch Weapon Dark";
    case CPlayerState::EItemType::SwitchWeaponLight:
      return "Switch Weapon Light";
    case CPlayerState::EItemType::SwitchWeaponAnnihilator:
      return "Switch Weapon Annihilator";
    case CPlayerState::EItemType::MultiChargeUpgrade:
      return "Multi Charge Upgrade";
    case CPlayerState::EItemType::Invisibility:
      return "Invisibility";
    case CPlayerState::EItemType::AmpDamage:
      return "Amp Damage";
    case CPlayerState::EItemType::Invincibility:
      return "Invincibility";
    case CPlayerState::EItemType::UnknownItem60:
      return "Unknown 60";
    case CPlayerState::EItemType::UnknownItem61:
      return "Unknown 61";
    case CPlayerState::EItemType::UnknownItem62:
      return "Unknown 62";
    case CPlayerState::EItemType::UnknownItem63:
      return "Unknown 63";
    case CPlayerState::EItemType::FragCount:
      return "Frag Count";
    case CPlayerState::EItemType::DiedCount:
      return "Died Count";
    case CPlayerState::EItemType::ArchenemyCount:
      return "Archenemy Count";
    case CPlayerState::EItemType::PersistentCounter1:
      return "Persistent Counter 1";
    case CPlayerState::EItemType::PersistentCounter2:
      return "Persistent Counter 2";
    case CPlayerState::EItemType::PersistentCounter3:
      return "Persistent Counter 3";
    case CPlayerState::EItemType::PersistentCounter4:
      return "Persistent Counter 4";
    case CPlayerState::EItemType::PersistentCounter5:
      return "Persistent Counter 5";
    case CPlayerState::EItemType::PersistentCounter6:
      return "Persistent Counter 6";
    case CPlayerState::EItemType::PersistentCounter7:
      return "Persistent Counter 7";
    case CPlayerState::EItemType::PersistentCounter8:
      return "Persistent Counter 8";
    case CPlayerState::EItemType::SwitchVisorCombat:
      return "Switch Visor Combat";
    case CPlayerState::EItemType::SwitchVisorScan:
      return "Switch Visor Scan";
    case CPlayerState::EItemType::SwitchVisorDark:
      return "Switch Visor Dark";
    case CPlayerState::EItemType::SwitchVisorEcho:
      return "Switch Visor Echo";
    case CPlayerState::EItemType::CoinAmplifier:
      return "Coin Amplifier";
    case CPlayerState::EItemType::CoinCounter:
      return "Coin Counter";
    case CPlayerState::EItemType::UnlimitedMissiles:
      return "Unlimited Missiles";
    case CPlayerState::EItemType::UnlimitedBeamAmmo:
      return "Unlimited Beam Ammo";
    case CPlayerState::EItemType::DarkShield:
      return "Dark Shield";
    case CPlayerState::EItemType::LightShield:
      return "Light Shield";
    case CPlayerState::EItemType::AbsorbAttack:
      return "Absorb Attack";
    case CPlayerState::EItemType::DeathBall:
      return "Death Ball";
    case CPlayerState::EItemType::ScanVirus:
      return "Scan Virus";
    case CPlayerState::EItemType::VisorStatic:
      return "Visor Static";
    case CPlayerState::EItemType::DisableBeamAmmo:
      return "Disable Beam Ammo";
    case CPlayerState::EItemType::DisableMissiles:
      return "Disable Missiles";
    case CPlayerState::EItemType::DisableMorphBall:
      return "Disable Morph Ball";
    case CPlayerState::EItemType::DisableBall:
      return "Disable Ball";
    case CPlayerState::EItemType::DisableSpaceJump:
      return "Disable Space Jump";
    case CPlayerState::EItemType::UnknownItem94:
      return "Unknown 94";
    case CPlayerState::EItemType::HackedEffect:
      return "Hacked Effect";
    case CPlayerState::EItemType::CannonBall:
      return "Cannon Ball";
    case CPlayerState::EItemType::VioletTranslator:
      return "Violet Translator";
    case CPlayerState::EItemType::AmberTranslator:
      return "Amber Translator";
    case CPlayerState::EItemType::EmeraldTranslator:
      return "Emerald Translator";
    case CPlayerState::EItemType::CobaltTranslator:
      return "Cobalt Translator";
    case CPlayerState::EItemType::SkyTempleKey4:
      return "Sky Temple Key 4";
    case CPlayerState::EItemType::SkyTempleKey5:
      return "Sky Temple Key 5";
    case CPlayerState::EItemType::SkyTempleKey6:
      return "Sky Temple Key 6";
    case CPlayerState::EItemType::SkyTempleKey7:
      return "Sky Temple Key 7";
    case CPlayerState::EItemType::SkyTempleKey8:
      return "Sky Temple Key 8";
    case CPlayerState::EItemType::SkyTempleKey9:
      return "Sky Temple Key 9";
    case CPlayerState::EItemType::EnergyTransferModuleInventory:
      return "Energy Transfer Module Inventory";
    case CPlayerState::EItemType::ChargeCombo:
      return "Charge Combo";
    case CPlayerState::EItemType::Max:
    default:
      return "Unknown";
    }
    return "Unknown";
  }

  void RenderItemsDualColumn(CPlayerState *playerState, const CPlayerState::EItemType *items, int start, int end) {
    ImGui::BeginGroup();
    // Render left group
    for (int i = start; i < end; i += 2) {
      RenderItemType(playerState, items[i]);
    }
    ImGui::EndGroup();
    ImGui::SameLine();
    ImGui::BeginGroup();
    // Render right group
    for (int i = start + 1; i < end; i += 2) {
      RenderItemType(playerState, items[i]);
    }
    ImGui::EndGroup();
  }
} // namespace GUI
