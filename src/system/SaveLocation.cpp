#include "SaveLocation.hpp"
#include "CardGate.hpp"
#include "prime/CCameraManager.hpp"
#include "prime/CGameArea.hpp"
#include "prime/CMemoryCardSys.hpp"
#include "prime/CStateManager.hpp"
#include "prime/CWorld.hpp"

namespace SaveLocation {
  namespace {
    Record records[3];
    bool dirty[3]{};
    bool requested = false;
    CMemoryCardDriver *candidateDriver = nullptr;
    int candidateSlot = -1;
    Record candidate;
    bool restorePending = false;
    uint64_t loadedSerial = 0;
    double loadedTime = 0.;
    int restoreFrames = 0;
    CGameState *loadedGame = nullptr;
    int loadedSlot = -1;
    const char *status = nullptr;

    bool identify(Record &record) {
      auto &mgr = g_CStateManager;
      if (!gpGameState || !mgr.FullyInitialized() || !mgr.GetPlayer() || !mgr.GetWorld() || mgr.IsMultiplayer())
        return false;
      int areaIndex = *GetField<int>(&mgr, 0x16a0);
      auto *world = mgr.GetWorld();
      if (areaIndex < 0 || areaIndex >= world->IGetAreaCount()) return false;
      auto *area = world->IGetAreaAlways(TAreaId{areaIndex});
      if (!area) return false;
      record.setSerial(gpGameState->CardSerial());
      record.words[Record::World] = world->IGetWorldAssetId();
      record.words[Record::Area] = area->IGetAreaAssetId();
      record.setTime(gpGameState->PlayTime());
      record.words[Record::Dark] = gpGameState->IsDarkWorld();
      record.words[Record::Morph] = static_cast<uint32_t>(mgr.GetPlayer()->MorphState());
      memcpy(record.words + Record::Transform, mgr.GetPlayer()->getTransform(), 48);
      return record.valid();
    }
    void changed(int slot, const Record &record) {
      records[slot] = record;
      dirty[slot] = true;
      CardGate::requestLocationSave(record.serial());
    }
  } // namespace
  const Record &record(int slot) {
    return records[slot];
  }
  void loadRecord(int slot, const uint8_t *bytes, uint64_t cardSerial) {
    if (dirty[slot] && records[slot].serial() == cardSerial)
      return; // Never overwrite a newly committed game save with an older companion file.
    Record value = Record::decode(bytes);
    records[slot] = value.valid() && value.serial() == cardSerial ? value : Record{};
    dirty[slot] = false;
  }
  void persisted(int slot, const Record &written) {
    if (records[slot] == written) dirty[slot] = false;
    if (status && !dirty[0] && !dirty[1] && !dirty[2]) status = "Exact save location stored.";
  }
  const char *message() {
    if ((dirty[0] || dirty[1] || dirty[2]) && !CardGate::busy() && !CardGate::saveRequested())
      return "Location change is not on card. Save settings retries it.";
    return status;
  }
  void beginSave() {
    requested = true;
    candidateDriver = nullptr;
    candidateSlot = -1;
    status = "Exact location will be retained after a successful save.";
  }
  void slotBuilt(CMemoryCardDriver *driver, int slot) {
    candidateDriver = nullptr;
    candidateSlot = -1;
    if (slot < 0 || slot >= 3 || !driver->SlotData(slot)) return;
    candidateDriver = driver;
    candidateSlot = slot;
    if (!requested) return; // Track a normal save too, to distinguish it from frontend option writes.
    if (!identify(candidate) || g_CStateManager.DeferredTransition() != CStateManager::Transition::Save ||
        g_CStateManager.UnsafeSaveTransition() || g_CStateManager.GetPlayer()->IsMorphBallTransitioning()) {
      requested = false;
      return;
    }
    candidate.setSerial(driver->CardSerial()); // Native UI may have accepted a replacement card.
    if (!candidate.valid()) requested = false;
  }

  void gameSaved(CMemoryCardDriver *driver) {
    if (!gpGameState || driver->State() != 3 || driver->Port() != 0) return;
    int slot = gpGameState->SaveSlot();
    if (slot < 0 || slot >= 3 || !driver->SlotData(slot)) return;
    if (requested && candidateDriver == driver && candidateSlot == slot && candidate.serial() == driver->CardSerial()) {
      changed(slot, candidate);
      status = "Game saved; writing exact location to EchoesPractice.";
    } else if (candidateDriver == driver && candidateSlot == slot) {
      // Normal saves must retire an earlier Save Anywhere record for this slot/card.
      Record invalid;
      invalid.setSerial(driver->CardSerial());
      if (records[slot].valid() && records[slot].serial() == invalid.serial()) changed(slot, invalid);
      status = nullptr;
    }
    requested = false;
    candidateDriver = nullptr;
    candidateSlot = -1;
  }
  void driverClosed(CMemoryCardDriver *driver) {
    // Null cleanup and native Retry/replace-card driver lifetimes do not cancel the save screen.
    if (driver && candidateDriver == driver) {
      candidateDriver = nullptr;
      candidateSlot = -1;
    }
  }
  void beforeLoad(bool skipCheckpoint) {
    restorePending = false;
    loadedGame = nullptr;
    loadedSlot = -1;
    loadedSerial = 0;
    restoreFrames = 0;
    if (!gpGameState) return;
    // The retail stream chooses checkpoint data when present and the bool is false. Never override it.
    if (!skipCheckpoint && gpGameState->HasCheckpoint()) return;
    int slot = gpGameState->SaveSlot();
    if (slot < 0 || slot >= 3 || !gpGameState->CardSerial()) return;
    loadedSlot = slot;
    loadedSerial = gpGameState->CardSerial();
    candidateSlot = -1;
    // Mark only a native streamed load. World/portal manager lifetimes alone never arm restoration.
    restorePending = true;
  }
  void afterLoad() {
    if (!restorePending || !gpGameState) {
      restorePending = false;
      return;
    }
    loadedGame = gpGameState;
    loadedTime = gpGameState->PlayTime();
  }
  void worldEnded() {
    if (requested) status = "Save canceled or not completed; previous exact location retained.";
    requested = false;
    candidateDriver = nullptr;
    candidateSlot = -1;
    // A streamed load can precede old-world cleanup. Its GameState identity remains the authority.
  }
  void update() {
    auto &mgr = g_CStateManager;
    if (requested && mgr.DeferredTransition() != CStateManager::Transition::Save) worldEnded();
    if (!restorePending) return;
    if (!gpGameState || loadedGame != gpGameState || gpGameState->CardSerial() != loadedSerial) {
      restorePending = false;
      status = "Exact load: game/card/slot changed.";
      return;
    }
    // Card loading and spawn cameras can outlast the first gameplay tick. Wait only during startup.
    if (!CardGate::loaded() || !mgr.FullyInitialized() || !mgr.GetPlayer() || !mgr.SaveCamera() ||
        mgr.SaveCamera()->IsInCinematicCamera() || mgr.GetPlayer()->IsMorphBallTransitioning()) {
      if (++restoreFrames >= 300) {
        restorePending = false;
        status = "Exact load: startup timed out.";
      }
      return;
    }
    restorePending = false; // One attempt after startup; never reapply on later frames or portal travel.
    if (mgr.UnsafeSaveTransition() || mgr.GameState() != 0 ||
        mgr.DeferredTransition() != CStateManager::Transition::InGame) {
      status = "Exact load: unsafe transition.";
      return;
    }
    int slot = gpGameState->SaveSlot();
    if (slot < 0 || slot >= 3 || slot != loadedSlot) {
      status = "Exact load: game/card/slot changed.";
      return;
    }
    Record identity;
    if (!identify(identity)) {
      status = "Exact load: room not ready.";
      return;
    }
    identity.setTime(loadedTime); // Exclude time spent loading/spawning.
    if (!records[slot].sameSave(identity)) {
      status = "Exact load: no matching record.";
      return;
    }
    const auto &location = records[slot];
    CTransform4f transform;
    memcpy(&transform, location.words + Record::Transform, sizeof(transform));
    auto *player = mgr.GetPlayer();
    CTransform4f fallback = *player->getTransform();
    auto oldMorph = player->MorphState();
    auto morph = static_cast<CPlayer::Morph>(location.words[Record::Morph]);
    player->Teleport(transform, mgr, true);
    player->SetSpawnedMorphBallState(morph, mgr);
    if (player->MorphState() != morph) {
      player->Teleport(fallback, mgr, true);
      player->SetSpawnedMorphBallState(oldMorph, mgr);
      player->SetCameraState(oldMorph == CPlayer::Morph::Ball ? CPlayer::Camera::Ball : CPlayer::Camera::FirstPerson,
                             mgr);
      status = "Exact load: morph failed.";
      return;
    }
    // Morph changes can shift Z. Teleport again synchronizes final position, collision cache, physics and cameras.
    player->Teleport(transform, mgr, true);
    player->SetCameraState(morph == CPlayer::Morph::Ball ? CPlayer::Camera::Ball : CPlayer::Camera::FirstPerson, mgr);
    status = "Restored exact save location.";
  }
} // namespace SaveLocation
