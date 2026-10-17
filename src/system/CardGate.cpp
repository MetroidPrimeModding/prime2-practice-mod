#include "CardGate.hpp"

#include "card.h"
#include "os.h"
#include "prime/CMemoryCardSys.hpp"
#include "settings.hpp"
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

namespace CardGate {
  namespace {
    constexpr s32 kChan = 0;
    constexpr CMemoryCardSys::EMemoryCardPort kPort = CMemoryCardSys::kCS_SlotA;
    constexpr char kFileName[] = "EchoesPractice";
    constexpr char kComment[] = "Echoes Practice Mod";
    constexpr u32 kBlockSize = 0x2000;
    constexpr int kTimeoutFrames = 60 * 20;
    constexpr s64 kDriverDrainTimeout = 5 * TICKS_PER_SECOND;

    // Echoes uses one 0x6000-byte file: header plus two in-file transactional slots.
    constexpr u32 kGameFirstSaveBytes = 0x6000;
    constexpr u32 kGameFirstSaveFiles = 1;
    constexpr u32 kGameResaveBytes = 0;
    constexpr u32 kGameResaveFiles = 0;

    // Comment/art layout followed by our checksummed settings payload; not a game-save header.
    constexpr u32 kCommentOffset = 4;
    constexpr u32 kCommentSize = 64;
    constexpr u32 kBannerOffset = kCommentOffset + kCommentSize;
    constexpr u32 kBannerBytes = 96 * 32 + 512;
    constexpr u32 kIconBytes = 32 * 32 + 512;
    constexpr u32 kPayloadOffsetWithArt = kBannerOffset + kBannerBytes + kIconBytes;

    constexpr u32 kMagic = 0x45505356; // 'EPSV'
    constexpr u32 kVersion = 1;
    constexpr u32 kMaxRecordsBytes = 1024;

    // Records are [id u8][length u8][big-endian value]. Unknown ids are skipped and missing ones keep their defaults,
    // so settings can be added or removed without bumping kVersion; it only changes if this encoding does.
    struct PayloadHeader {
      u32 magic;
      u32 version;
      u32 recordsSize;
      u32 recordsCrc;
    };

#define SETTINGS_COUNT_FIELD(id, field) +1
    constexpr u32 kFieldCount = 0 SETTINGS_FIELDS(SETTINGS_COUNT_FIELD);
#undef SETTINGS_COUNT_FIELD
    static_assert(sizeof(PayloadHeader) == 16);
    static_assert(kFieldCount * 3 <= kMaxRecordsBytes);
    static_assert(kPayloadOffsetWithArt + sizeof(PayloadHeader) + kMaxRecordsBytes <= kBlockSize);

    // Bitfields can't be bound to a reference, so each field goes through a local
    template <class Visitor>
    void visitSettings(Settings &s, Visitor &visitor) {
#define SETTINGS_VISIT_FIELD(id, field)                                                                                \
  {                                                                                                                    \
    auto value = s.field;                                                                                              \
    visitor(static_cast<u8>(id), value);                                                                               \
    if constexpr (Visitor::kLoads) s.field = value;                                                                    \
  }
      SETTINGS_FIELDS(SETTINGS_VISIT_FIELD)
#undef SETTINGS_VISIT_FIELD
    }

    struct RecordWriter {
      static constexpr bool kLoads = false;
      u8 *out;
      u32 size = 0;
      __attribute__((noinline)) void put(u8 id, u32 value, u8 length) {
        out[size++] = id;
        out[size++] = length;
        for (int shift = (length - 1) * 8; shift >= 0; shift -= 8)
          out[size++] = value >> shift;
      }
      __attribute__((noinline)) void operator()(u8 id, bool value) {
        put(id, value, 1);
      }
    };

    struct RecordReader {
      static constexpr bool kLoads = true;
      const u8 *data;
      u32 size;
      // Value bytes of the record with this id and length, or null
      const u8 *find(u8 id, u8 length) const {
        for (u32 at = 0; at + 2 <= size && at + 2 + data[at + 1] <= size; at += 2 + data[at + 1]) {
          if (data[at] == id && data[at + 1] == length) return data + at + 2;
        }
        return nullptr;
      }
      __attribute__((noinline)) void operator()(u8 id, bool &value) {
        if (const u8 *p = find(id, 1)) value = p[0] != 0;
      }
    };

    enum class Step { Idle, Probing, Mounting, Checking, Creating, Writing, SettingStatus, Reading };
    enum class Op { None, Load, Save };

    Step step = Step::Idle;
    Op op = Op::None;
    int stepFrames = 0;
    int driverCount = 0;

    bool saveWanted = false;
    bool loadDone = false;
    bool mounted = false;
    bool fileOpen = false;
    bool timedOut = false;
    CARDFileInfo file;
    const char *lastMessage = nullptr;

    // What is on the card; written is the snapshot that went into the buffer, which may differ from SETTINGS by the
    // time the write finishes
    Settings saved;
    Settings written;

    // Only allocated while a card operation runs. The SDK's DMA needs 32-byte alignment, which we ensure ourselves
    // rather than relying on the allocator
    u8 *bufferRaw = nullptr;
    u8 *buffer = nullptr;

    u32 crc32(const void *data, u32 len) {
      const u8 *p = static_cast<const u8 *>(data);
      u32 crc = 0xFFFFFFFF;
      for (u32 i = 0; i < len; i++) {
        crc ^= p[i];
        for (int bit = 0; bit < 8; bit++) {
          crc = (crc >> 1) ^ (0xEDB88320 & -(crc & 1));
        }
      }
      return ~crc;
    }

    bool sameSettings(const Settings &a, const Settings &b) {
#define COMPARE_FIELD(id, field)                                                                                       \
  if (a.field != b.field) return false;
      SETTINGS_FIELDS(COMPARE_FIELD)
#undef COMPARE_FIELD
      return true;
    }

    // The card is always left unmounted with the work area freed, success or failure
    void finish(const char *message) {
      if (fileOpen) {
        CARDClose(&file);
        fileOpen = false;
      }
      if (mounted) {
        CMemoryCardSys::UnmountCard(kPort);
        mounted = false;
      }
      free(bufferRaw);
      bufferRaw = buffer = nullptr;
      if (message) lastMessage = message;
      if (op == Op::Load) {
        loadDone = true;
        // Whatever is in use now (loaded or default) is the baseline for the unsaved-changes indicator
        memcpy(&saved, &SETTINGS, sizeof(Settings));
      }
      step = Step::Idle;
      op = Op::None;
      EchoesCardBusy = false;
    }

    void fail(const char *message, s32 code) {
      DebugLog("CardGate: %s (%d)\n", message, code);
      // A failed save stays visible as unsaved changes; the user can retry
      finish(message);
    }

    void enter(Step next) {
      step = next;
      stepFrames = 0;
    }

    // Layout a file has according to its card status, so a file written elsewhere still reads right
    u32 payloadOffset(const CARDStat &stat) {
      u32 offset = kBannerOffset;
      if ((stat.bannerFormat & 3) == 1)
        offset += 96 * 32 + 512;
      else if ((stat.bannerFormat & 3) == 2)
        offset += 96 * 32 * 2;
      bool palette = false;
      for (int i = 0; i < 8; i++) {
        u32 format = (stat.iconFormat >> (i * 2)) & 3;
        if (format == 0) break;
        if (format == 1) {
          offset += 32 * 32;
          palette = true;
        } else {
          offset += 32 * 32 * 2;
        }
      }
      return palette ? offset + 512 : offset;
    }

    // C8 pixels use GX 8x4 tiles; palettes contain big-endian RGB5A3 entries.
    void artPixel(u8 *image, int width, int x, int y, u8 color) {
      image[((y / 4) * (width / 8) + x / 8) * 32 + (y % 4) * 8 + x % 8] = color;
    }

    void artText(u8 *image, int width, int x, int y, const char *text, int scale) {
      // Five columns per glyph, with bit zero at the top. Only the card labels need these glyphs.
      constexpr char letters[] = "ACEHIOPRST2";
      constexpr u8 glyphs[][5] = {
          {0x7E, 0x09, 0x09, 0x09, 0x7E}, // A
          {0x3E, 0x41, 0x41, 0x41, 0x22}, // C
          {0x7F, 0x49, 0x49, 0x49, 0x41}, // E
          {0x7F, 0x08, 0x08, 0x08, 0x7F}, // H
          {0x41, 0x41, 0x7F, 0x41, 0x41}, // I
          {0x3E, 0x41, 0x41, 0x41, 0x3E}, // O
          {0x7F, 0x09, 0x09, 0x09, 0x06}, // P
          {0x7F, 0x09, 0x19, 0x29, 0x46}, // R
          {0x46, 0x49, 0x49, 0x49, 0x31}, // S
          {0x01, 0x01, 0x7F, 0x01, 0x01}, // T
          {0x62, 0x51, 0x49, 0x49, 0x46}, // 2
      };
      for (; *text; ++text, x += 6 * scale) {
        const char *glyph = letters;
        while (*glyph && *glyph != *text)
          ++glyph;
        if (!*glyph) continue;
        for (int col = 0; col < 5; ++col) {
          for (int row = 0; row < 7; ++row) {
            if (!(glyphs[glyph - letters][col] & (1 << row))) continue;
            for (int dy = 0; dy < scale; ++dy)
              for (int dx = 0; dx < scale; ++dx)
                artPixel(image, width, x + col * scale + dx, y + row * scale + dy, 3);
          }
        }
      }
    }

    void artBackground(u8 *image, int width) {
      for (int y = 0; y < 32; ++y) {
        for (int x = 0; x < width; ++x) {
          bool border = x == 0 || x == width - 1 || y == 0 || y == 31;
          artPixel(image, width, x, y, border ? 2 : 1);
        }
      }
      u8 *palette = image + width * 32;
      memset(palette, 0, 512);
      constexpr u16 colors[] = {0x8000, 0x8423, 0x82F7, 0xFFFF}; // black, navy, teal, white
      for (u32 i = 0; i < sizeof(colors) / sizeof(colors[0]); ++i) {
        palette[i * 2] = colors[i] >> 8;
        palette[i * 2 + 1] = colors[i];
      }
    }

    // Independent artwork avoids borrowing game texture/token storage.
    void writeArt(u8 *out) {
      artBackground(out, 96);
      artText(out, 96, 30, 6, "ECHOES", 1);
      artText(out, 96, 24, 19, "PRACTICE", 1);
      out += kBannerBytes;
      artBackground(out, 32);
      artText(out, 32, 5, 9, "P2", 2);
    }

    void buildFile() {
      memset(buffer, 0, kBlockSize);
      memcpy(buffer + kCommentOffset, kComment, sizeof(kComment) - 1);
      writeArt(buffer + kBannerOffset);
      written = SETTINGS;
      u8 *payload = buffer + kPayloadOffsetWithArt;
      RecordWriter writer{payload + sizeof(PayloadHeader)};
      visitSettings(written, writer);
      PayloadHeader header{kMagic, kVersion, writer.size, crc32(writer.out, writer.size)};
      memcpy(payload, &header, sizeof(header));
    }

    bool fileExists(const char *name) {
      CARDFileInfo info;
      if (CARDOpen(kChan, name, &info) != CARD_RESULT_READY) return false;
      CARDClose(&info);
      return true;
    }

    bool hasEnoughSpace() {
      s32 freeBytes, freeFiles;
      if (CARDFreeBlocks(kChan, &freeBytes, &freeFiles) != CARD_RESULT_READY) return false;
      bool hasGameSave = fileExists("MetroidPrime2");
      u32 leaveBytes = hasGameSave ? kGameResaveBytes : kGameFirstSaveBytes;
      u32 leaveFiles = hasGameSave ? kGameResaveFiles : kGameFirstSaveFiles;
      return freeBytes >= static_cast<s32>(kBlockSize + leaveBytes) && freeFiles >= static_cast<s32>(1 + leaveFiles);
    }

    void startCreate() {
      buildFile();
      s32 result = CARDCreateAsync(kChan, kFileName, kBlockSize, &file, nullptr);
      if (result != CARD_RESULT_READY) return fail("Could not create the settings file", result);
      enter(Step::Creating);
    }

    void startWrite() {
      buildFile();
      DCFlushRange(buffer, kBlockSize);
      s32 result = CARDWriteAsync(&file, buffer, kBlockSize, 0, nullptr);
      if (result != CARD_RESULT_READY) return fail("Could not write settings", result);
      enter(Step::Writing);
    }

    void startSave() {
      s32 result = CARDOpen(kChan, kFileName, &file);
      if (result == CARD_RESULT_NOFILE) {
        if (!hasEnoughSpace()) return fail("Not enough free memory card space", 0);
        return startCreate();
      }
      if (result != CARD_RESULT_READY) return fail("Could not open settings", result);
      fileOpen = true;
      CARDStat stat;
      if (CARDGetStatus(kChan, file.fileNo, &stat) != CARD_RESULT_READY || stat.length != kBlockSize)
        return fail("Unexpected settings file size; file preserved", 0);
      // Replace in place, never delete the existing file before creating a new one.
      startWrite();
    }

    void startLoad() {
      s32 result = CARDOpen(kChan, kFileName, &file);
      if (result == CARD_RESULT_NOFILE) return finish("No saved settings");
      if (result != CARD_RESULT_READY) return fail("Could not open the settings file", result);
      fileOpen = true;

      CARDStat stat;
      result = CARDGetStatus(kChan, file.fileNo, &stat);
      if (result != CARD_RESULT_READY || stat.length != kBlockSize) {
        return fail("Settings file has an unexpected size, using defaults", result);
      }
      DCInvalidateRange(buffer, kBlockSize);
      result = CARDReadAsync(&file, buffer, kBlockSize, 0, nullptr);
      if (result != CARD_RESULT_READY) return fail("Could not read the settings file", result);
      enter(Step::Reading);
    }

    void finishLoad() {
      CARDStat stat;
      if (CARDGetStatus(kChan, file.fileNo, &stat) != CARD_RESULT_READY) {
        return fail("Could not read the settings file status, using defaults", 0);
      }
      u32 offset = payloadOffset(stat);
      if (offset + sizeof(PayloadHeader) > kBlockSize) {
        return fail("Settings file is not recognized, using defaults", 0);
      }
      PayloadHeader header;
      memcpy(&header, buffer + offset, sizeof(header));
      const u8 *data = buffer + offset + sizeof(header);
      if (header.magic != kMagic || header.version != kVersion || header.recordsSize > kMaxRecordsBytes ||
          offset + sizeof(header) + header.recordsSize > kBlockSize ||
          header.recordsCrc != crc32(data, header.recordsSize)) {
        return fail("Saved settings are from a different version, using defaults", 0);
      }
      // Validate the entire stream before applying any field. Unknown records are skipped.
      for (u32 at = 0; at < header.recordsSize;) {
        if (header.recordsSize - at < 2 || data[at + 1] > header.recordsSize - at - 2)
          return fail("Truncated settings records; using defaults", 0);
        u8 id = data[at], length = data[at + 1];
        bool known = false;
        switch (id) {
#define KNOWN_FIELD(fieldId, field)                                                                                    \
  case fieldId:                                                                                                        \
    known = true;                                                                                                      \
    break;
          SETTINGS_FIELDS(KNOWN_FIELD)
#undef KNOWN_FIELD
        default:
          break;
        }
        if (known && (length != 1 || data[at + 2] > 1)) return fail("Invalid settings record; using defaults", 0);
        for (u32 prev = 0; prev < at; prev += 2 + data[prev + 1])
          if (data[prev] == id) return fail("Duplicate settings record; using defaults", 0);
        at += 2 + length;
      }
      Settings loaded{};
      RecordReader reader{data, header.recordsSize};
      visitSettings(loaded, reader);
      SETTINGS = loaded;
      finish("Loaded saved settings");
    }

    void startStatus() {
      CARDStat stat;
      s32 result = CARDGetStatus(kChan, file.fileNo, &stat);
      if (result != CARD_RESULT_READY) return fail("Could not read the settings file status", result);
      stat.commentAddr = kCommentOffset;
      stat.iconAddr = kBannerOffset;
      stat.bannerFormat = (stat.bannerFormat & ~3) | 1;
      stat.iconFormat = 1; // slot 0 = C8, other slots none
      stat.iconSpeed = 2;
      result = CARDSetStatusAsync(kChan, file.fileNo, &stat, nullptr);
      if (result != CARD_RESULT_READY) return fail("Could not finish the settings file", result);
      enter(Step::SettingStatus);
    }

    void begin(Op newOp) {
      op = newOp;
      EchoesCardBusy = true;

      bufferRaw = static_cast<u8 *>(malloc(kBlockSize + 32));
      if (!bufferRaw) return fail("Out of memory", 0);
      buffer = reinterpret_cast<u8 *>((reinterpret_cast<uintptr_t>(bufferRaw) + 31) & ~uintptr_t(31));
      timedOut = false;
      enter(Step::Probing);
    }

    void startMount() {
      s32 memSize, sectorSize;
      s32 result = CARDProbeEx(kChan, &memSize, &sectorSize);
      if (result == CARD_RESULT_BUSY) return;
      if (result == CARD_RESULT_NOCARD) return fail("No memory card in slot A", result);
      if (result != CARD_RESULT_READY) return fail("Memory card in slot A is not usable", result);
      if (sectorSize != static_cast<s32>(kBlockSize)) return fail("Memory card has an unsupported sector size", 0);

      mounted = true;
      result = CMemoryCardSys::MountCard(kPort);
      if (result != CARD_RESULT_READY) return fail("Could not mount", result);
      enter(Step::Mounting);
    }

    // Anything but busy on a step's command ends that step
    void advance(bool draining) {
      if (!draining && ++stepFrames > kTimeoutFrames) timedOut = true;
      if (step == Step::Probing) {
        if (draining || timedOut) {
          bool retryLoad = op == Op::Load && draining;
          finish("Memory card operation interrupted");
          if (retryLoad) loadDone = false;
          return;
        }
        return startMount();
      }

      s32 result = CARDGetResultCode(kChan);
      if (result == CARD_RESULT_BUSY) return;
      // DMA/work buffers remain alive until the SDK reports terminal completion.
      if (draining || timedOut) {
        if (step == Step::Creating && result == CARD_RESULT_READY) fileOpen = true;
        bool retryLoad = op == Op::Load && draining;
        finish("Memory card operation interrupted; retry settings save");
        if (retryLoad) loadDone = false;
        return;
      }
      if (result != CARD_RESULT_READY) return fail("Memory card op failed", result);

      switch (step) {
      case Step::Mounting:
        result = CARDCheckAsync(kChan, nullptr);
        if (result != CARD_RESULT_READY) return fail("Memory card check failed", result);
        return enter(Step::Checking);
      case Step::Checking:
        return op == Op::Load ? startLoad() : startSave();
      case Step::Creating:
        fileOpen = true;
        return startWrite();
      case Step::Writing:
        return startStatus();
      case Step::SettingStatus:
        memcpy(&saved, &written, sizeof(Settings));
        return finish("Settings saved");
      case Step::Reading:
        return finishLoad();
      default:
        return;
      }
    }
  } // namespace

  void tick() {
    if (step != Step::Idle) return advance(false);
    if (!EchoesCardSysExists || driverCount || EchoesCardDriverExists) return;
    // Load before honoring queued saves so a save request cannot clobber unread settings.
    if (!loadDone) return begin(Op::Load);
    if (saveWanted) {
      saveWanted = false;
      lastMessage = "Saving settings...";
      begin(Op::Save);
    }
  }

  void requestSave() {
    saveWanted = true;
  }
  void requestLoad() {
    if (!busy()) loadDone = false;
  }
  bool saveRequested() {
    return saveWanted;
  }
  bool busy() {
    return step != Step::Idle;
  }
  bool dirty() {
    return loadDone && !sameSettings(saved, SETTINGS);
  }
  const char *message() {
    return lastMessage;
  }

  void driverConstructing() {
    drain();
    ++driverCount;
  }
  void driverDestroyed() {
    if (driverCount) --driverCount;
  }

  void drain() {
    s64 start = OSGetTime();
    while (step != Step::Idle) {
      advance(true);
      if (step == Step::Idle) break;
      // Fail closed: never release DMA buffers or let the driver take a busy channel.
      if (OSGetTime() - start > kDriverDrainTimeout) {
        OSReport("CardGate: SDK command failed to drain\n");
        __builtin_trap();
      }
      OSYieldThread();
    }
  }
} // namespace CardGate
