#pragma once

#include "rstl/rstl.h"

RSTL_BEGIN

class rmemory_allocator {};
template <typename t>
class char_traits;

// G2ME01 constructors and release (see prime-practice.lst) own allocation and reference counts.
// Character data follows the eight-byte control block inline. Only char and char16_t are bound.
template <typename _CharTp, typename traits, typename allocator>
class basic_string {
  struct COWData {
    u32 x0_capacity;
    u32 x4_refCount;
    _CharTp x8_data[];
  };

  static_assert(sizeof(COWData) == 8, "Game string data follows its eight-byte control block");

  const _CharTp *x0_ptr;
  COWData *x4_cow;
  u32 x8_size;
  u32 xc_allocator; // Echoes MWCC allocator slot and padding; game objects reserve 0x10 bytes per string

  void internal_dereference();

public:
  struct literal_t {};

  // No copy: x0_ptr points straight at data, which must outlive every copy (including ones the game keeps)
  basic_string(literal_t, const _CharTp *data) : x0_ptr(data), x4_cow(nullptr), x8_size(0), xc_allocator(0) {
    while (data[x8_size]) {
      ++x8_size;
    }
  }

  // size == -1 copies up to the terminator
  basic_string(const _CharTp *data, int size = -1, const allocator &alloc = allocator());
  basic_string(const basic_string &str);
  ~basic_string() {
    internal_dereference();
  }

  basic_string &operator=(const basic_string &) = delete;

  const _CharTp *data() const {
    return x0_ptr;
  }
  u32 size() const {
    return x8_size;
  }
};

typedef basic_string<char16_t, char_traits<char16_t>, rmemory_allocator> wstring;
typedef basic_string<char, char_traits<char>, rmemory_allocator> string;

inline wstring wstring_l(const char16_t *data) {
  return wstring(wstring::literal_t(), data);
}

inline string string_l(const char *data) {
  return string(string::literal_t(), data);
}

static_assert(sizeof(char16_t) == 2, "Game wide characters must be two bytes");
static_assert(sizeof(string) == 0x10, "Game strings reserve the allocator slot");
static_assert(sizeof(wstring) == 0x10, "Game wide strings reserve the allocator slot");

RSTL_END
