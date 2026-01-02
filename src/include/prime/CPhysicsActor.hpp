#pragma once

#include "CActor.hpp"

class CPhysicsActor : public CActor {
public:
  // CPhysicsState GetPhysicsState() const;
  //
  inline CVector3f *GetVelocity() { return GetField<CVector3f>(this, 0x1A8); };
  inline CVector3f *GetAngularVelocity() { return GetField<CVector3f>(this, 0x1B4); };
};
