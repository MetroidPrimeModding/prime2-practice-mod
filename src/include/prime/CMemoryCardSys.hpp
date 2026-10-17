#pragma once
#include "GetField.hpp"
#include "types.h"
class CMemoryCardSys {
public:
  enum EMemoryCardPort { kCS_SlotA = 0, kCS_SlotB = 1 };
  CMemoryCardSys *Destroy(int deleting);
  static s32 MountCard(EMemoryCardPort port);
  static s32 UnmountCard(EMemoryCardPort port);
};
// MWCC bridges preserve the this return and hidden deleting-destructor argument.
class CMemoryCardDriver {
public:
  CMemoryCardDriver *Construct(CMemoryCardSys::EMemoryCardPort port, u32 banner, u32 icon0, u32 icon1, bool import);
  void BuildExistingFileSlot(int slot);
  void WriteBackupBuf();
  int Port() const {
    return *GetField<int>(this, 0);
  }
  int State() const {
    return *GetField<int>(this, 0x10);
  }
  u64 CardSerial() const {
    return *GetField<u64>(this, 0x28);
  }
  const u8 *SlotData(int slot) const {
    if (slot < 0 || slot >= 3) return nullptr;
    auto *data = *GetField<const u8 *>(this, 0xfc + slot * 8);
    return data ? data + 4 : nullptr;
  }
  CMemoryCardDriver *Destroy(int deleting);
};
extern "C" volatile bool EchoesCardDriverExists;
extern "C" volatile bool EchoesCardSysExists;
extern "C" volatile bool EchoesCardBusy;
