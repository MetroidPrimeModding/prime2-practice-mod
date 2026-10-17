#pragma once

// G2ME01 returns two-byte game characters, independently of the compiler's wchar_t.
class CStringTable {
public:
  const char16_t *GetString(int idx) const;
  int GetStringIndex(const char *name) const;
};
