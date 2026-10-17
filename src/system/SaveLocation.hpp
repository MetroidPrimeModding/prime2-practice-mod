#pragma once
#include "SaveLocationRecord.hpp"
class CMemoryCardDriver;
namespace SaveLocation {
  void beginSave();
  void slotBuilt(CMemoryCardDriver *driver, int slot);
  void gameSaved(CMemoryCardDriver *driver); // Native transactional write succeeded, after WriteBackupBuf.
  void driverClosed(CMemoryCardDriver *driver);
  void beforeLoad(bool skipCheckpoint);
  void afterLoad();
  void
  update(); // Before the native gameplay update; waits briefly for startup, restores at most once per streamed load.
  void worldEnded();
  const char *message();
  const Record &record(int slot);
  void loadRecord(int slot, const uint8_t *bytes, uint64_t cardSerial);
  void persisted(int slot, const Record &written);
} // namespace SaveLocation
