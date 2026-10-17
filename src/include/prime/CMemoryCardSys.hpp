#pragma once
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
  CMemoryCardDriver *Destroy(int deleting);
};
extern "C" volatile bool EchoesCardDriverExists;
extern "C" volatile bool EchoesCardSysExists;
extern "C" volatile bool EchoesCardBusy;
