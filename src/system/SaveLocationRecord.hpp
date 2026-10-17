#pragma once
#include <stdint.h>
#include <string.h>

namespace SaveLocation {
  // Explicit words, encoded big endian in tagged record IDs E0..E2. Never write compiler object layouts.
  struct Record {
    enum Field { SerialHi, SerialLo, World, Area, TimeHi, TimeLo, Dark, Morph, Transform };
    static constexpr unsigned kWords = Transform + 12;
    static constexpr unsigned kBytes = kWords * 4;
    uint32_t words[kWords]{};
    uint64_t serial() const {
      return (uint64_t(words[SerialHi]) << 32) | words[SerialLo];
    }
    void setSerial(uint64_t serial) {
      words[SerialHi] = serial >> 32;
      words[SerialLo] = serial;
    }
    void setTime(double time) {
      uint64_t bits;
      memcpy(&bits, &time, sizeof(bits));
      words[TimeHi] = bits >> 32;
      words[TimeLo] = bits;
    }
    double time() const {
      uint64_t bits = (uint64_t(words[TimeHi]) << 32) | words[TimeLo];
      double result;
      memcpy(&result, &bits, sizeof(result));
      return result;
    }
    bool valid() const {
      if (!serial() || words[Dark] > 1 || words[Morph] > 1 || words[World] == 0xffffffff || words[Area] == 0xffffffff)
        return false;
      // Nonnegative finite play time, finite transform, and a usable horizontal forward direction.
      if ((words[TimeHi] & 0x80000000) || (words[TimeHi] & 0x7ff00000) == 0x7ff00000) return false;
      float transform[12];
      for (unsigned i = 0; i < 12; ++i)
        if ((words[Transform + i] & 0x7f800000) == 0x7f800000) return false;
      memcpy(transform, words + Transform, sizeof(transform));
      float forward = transform[1] * transform[1] + transform[5] * transform[5];
      return forward > 0.001f && forward < 2.f;
    }
    __attribute__((noinline)) void encode(uint8_t *out) const {
      for (unsigned i = 0; i < kWords; ++i)
        for (unsigned byte = 0; byte < 4; ++byte)
          out[4 * i + byte] = words[i] >> (24 - 8 * byte);
    }
    __attribute__((noinline)) static Record decode(const uint8_t *in) {
      Record result;
      if (in)
        for (unsigned i = 0; i < kWords; ++i)
          for (unsigned byte = 0; byte < 4; ++byte)
            result.words[i] = (result.words[i] << 8) | in[4 * i + byte];
      return result;
    }
    bool operator==(const Record &other) const {
      return !memcmp(words, other.words, sizeof(words));
    }
    bool sameSave(const Record &other) const {
      // Slot is implicit in the record ID. Match gameplay identity with Prime's one-second time tolerance.
      double delta = time() - other.time();
      return valid() && serial() == other.serial() && words[World] == other.words[World] &&
             words[Area] == other.words[Area] && words[Dark] == other.words[Dark] && delta >= -1. && delta <= 1.;
    }
  };
  static_assert(Record::kBytes == 80); // First deployed format; versioned by the enclosing file header.
} // namespace SaveLocation
