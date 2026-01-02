#pragma once

#include "GetField.hpp"
#include "rstl/vector.h"
#include "types.h"
#include <PrimeAPI.h>
#include <prime/CHealthInfo.hpp>

class CInputStream;

class COutputStream;

class CStateManager;

constexpr int PowerUpMaxValues[] = {
    1, 1, 1, 1,  250, 1, 1, 8, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    1, 1, 1, 14, 1,   0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
};

struct ScanState {
  CAssetId asset;
  float time;
};

class CPlayerState {
public:
  enum class EItemType : s32 {
    PowerBeam = 0x0,
    DarkBeam = 0x1,
    LightBeam = 0x2,
    AnnihilatorBeam = 0x3,
    SuperMissile = 0x4,
    Darkburst = 0x5,
    Sunburst = 0x6,
    SonicBoom = 0x7,
    CombatVisor = 0x8,
    ScanVisor = 0x9,
    DarkVisor = 0xA,
    EchoVisor = 0xB,
    VariaSuit = 0xC,
    DarkSuit = 0xD,
    LightSuit = 0xE,
    MorphBall = 0xF,
    BoostBall = 0x10,
    SpiderBall = 0x11,
    MorphBallBomb = 0x12,
    LightBomb = 0x13,
    DarkBomb = 0x14,
    AnnihilatorBomb = 0x15,
    ChargeBeam = 0x16,
    GrappleBeam = 0x17,
    SpaceJumpBoots = 0x18,
    GravityBoost = 0x19,
    SeekerLauncher = 0x1A,
    ScrewAttack = 0x1B,
    EnergyTransferModulePickup = 0x1C,
    SkyTempleKey1 = 0x1D,
    SkyTempleKey2 = 0x1E,
    SkyTempleKey3 = 0x1F,
    DarkAgonKey1 = 0x20,
    DarkAgonKey2 = 0x21,
    DarkAgonKey3 = 0x22,
    DarkTorvusKey1 = 0x23,
    DarkTorvusKey2 = 0x24,
    DarkTorvusKey3 = 0x25,
    IngHiveKey1 = 0x26,
    IngHiveKey2 = 0x27,
    IngHiveKey3 = 0x28,
    HealthRefill = 0x29,
    EnergyTank = 0x2A,
    PowerBomb = 0x2B,
    Missile = 0x2C,
    DarkAmmo = 0x2D,
    LightAmmo = 0x2E,
    ItemPercentage = 0x2F,
    NumPlayersJoined = 0x30,
    NumPlayersInOptionsMenu = 0x31,
    MiscCounter3 = 0x32,
    MiscCounter4 = 0x33,
    SwitchWeaponPower = 0x34,
    SwitchWeaponDark = 0x35,
    SwitchWeaponLight = 0x36,
    SwitchWeaponAnnihilator = 0x37,
    MultiChargeUpgrade = 0x38,
    Invisibility = 0x39,
    AmpDamage = 0x3A,
    Invincibility = 0x3B,
    UnknownItem60 = 0x3C,
    UnknownItem61 = 0x3D,
    UnknownItem62 = 0x3E,
    UnknownItem63 = 0x3F,
    FragCount = 0x40,
    DiedCount = 0x41,
    ArchenemyCount = 0x42,
    PersistentCounter1 = 0x43,
    PersistentCounter2 = 0x44,
    PersistentCounter3 = 0x45,
    PersistentCounter4 = 0x46,
    PersistentCounter5 = 0x47,
    PersistentCounter6 = 0x48,
    PersistentCounter7 = 0x49,
    PersistentCounter8 = 0x4A,
    SwitchVisorCombat = 0x4B,
    SwitchVisorScan = 0x4C,
    SwitchVisorDark = 0x4D,
    SwitchVisorEcho = 0x4E,
    CoinAmplifier = 0x4F,
    CoinCounter = 0x50,
    UnlimitedMissiles = 0x51,
    UnlimitedBeamAmmo = 0x52,
    DarkShield = 0x53,
    LightShield = 0x54,
    AbsorbAttack = 0x55,
    DeathBall = 0x56,
    ScanVirus = 0x57,
    VisorStatic = 0x58,
    DisableBeamAmmo = 0x59,
    DisableMissiles = 0x5A,
    DisableMorphBall = 0x5B,
    DisableBall = 0x5C,
    DisableSpaceJump = 0x5D,
    UnknownItem94 = 0x5E,
    HackedEffect = 0x5F,
    CannonBall = 0x60,
    VioletTranslator = 0x61,
    AmberTranslator = 0x62,
    EmeraldTranslator = 0x63,
    CobaltTranslator = 0x64,
    SkyTempleKey4 = 0x65,
    SkyTempleKey5 = 0x66,
    SkyTempleKey6 = 0x67,
    SkyTempleKey7 = 0x68,
    SkyTempleKey8 = 0x69,
    SkyTempleKey9 = 0x6A,
    EnergyTransferModuleInventory = 0x6B,
    ChargeCombo = 0x6C,

    /* This must remain at the end of the list */
    Max = 0x6D
  };

  enum class EPlayerVisor : u32 {
    Combat,
    XRay,
    Scan,
    Thermal,

    /* This must remain at the end of the list */
    Max
  };

  enum class EPlayerSuit : s32 {
    Invalid = -1,
    Power,
    Gravity,
    Varia,
    Phazon,
    FusionPower,
    FusionGravity,
    FusionVaria,
    FusionPhazon
  };

  enum class EBeamId : s32 { Invalid = -1, Power, Ice, Wave, Plasma, Phazon, Phazon2 = 27 };

  class CPowerUp {
  public:
    int amount;
    int capacity;
  };

  CPlayerState();
  CPlayerState(CInputStream &);

  inline CHealthInfo *GetHealthInfo() {
    return GetField<CHealthInfo>(this, 0x10);
  }
  inline CPowerUp *GetPowerups() {
    return GetField<CPowerUp>(this, 0x5C);
  }

  bool CanVisorSeeFog(const CStateManager &) const;
  CPlayerState::EPlayerVisor GetActiveVisor(const CStateManager &) const;
  void UpdateStaticInterference(CStateManager &, const float &);
  void NewScanTime(uint);
  void IncreaseScanTime(uint, float);
  float GetScanTime(uint) const;
  static float GetEnergyTankCapacity();
  static float GetBaseHealthCapacity();
  bool GetIsVisorTransitioning() const;
  float GetVisorTransitionFactor() const;
  void UpdateVisorTransition(float);
  void StartTransitionToVisor(CPlayerState::EPlayerVisor);
  void ResetVisor();
  bool ItemEnabled(CPlayerState::EItemType) const;
  void DisableItem(CPlayerState::EItemType);
  void EnableItem(CPlayerState::EItemType);

  inline CPowerUp *GetPowerUp(EItemType Item) {
    return &GetField<CPowerUp>(this, 0x24)[(int)Item];
  }

  bool HasPowerUp(CPlayerState::EItemType) const;
  uint GetItemCapacity(CPlayerState::EItemType) const;
  uint GetItemAmount(CPlayerState::EItemType) const;
  void DecrPickUp(CPlayerState::EItemType, int);
  void IncrPickUp(CPlayerState::EItemType, int);
  void AddPowerUp(CPlayerState::EItemType, int);
  void ReInitializePowerUp(CPlayerState::EItemType, int);
  void PutTo(COutputStream &);

  static u32 GetPowerUpMaxValue(EItemType type);

  void ResetAndIncrPickUp(EItemType type, u32 amount);
};
