#pragma once

#include "math.h"

class CVector3f;

// TODO: find the float versions instead of calling the double version
class CMath {
public:
  // static int FloorPowerOfTwo(int);
  // static float FastArcCosR(float);
  // static float FastCosR(float);
  // static float FastSinR(float);
  static CVector3f BaryToWorld(const CVector3f &, const CVector3f &, const CVector3f &, const CVector3f &);
  static CVector3f GetBezierPoint(const CVector3f &, const CVector3f &, const CVector3f &, const CVector3f &, float);
  static float GetCatmullRomSplinePoint(float, float, float, float, float);
  static CVector3f GetCatmullRomSplinePoint(const CVector3f &, const CVector3f &, const CVector3f &, const CVector3f &,
                                            float);
  static float CeilingF(double param_1) {
    double dVar1 = floor(param_1);
    if (dVar1 != param_1) {
      return 1.0 + dVar1;
    }
    return dVar1;
  }
  static float FloorF(float);
  static float SlowTangentR(float);
  static float SlowCosineR(float);
  static float SlowSineR(float);
  static float PowF(float a, float b) { return pow(a, b); }
  static float ArcTangentR(float);
  static float ArcCosineR(float);
  static float ArcSineR(float);
  static float InvSqrtF(float);
  static double SqrtD(double);
  static float SqrtF(float);
};
