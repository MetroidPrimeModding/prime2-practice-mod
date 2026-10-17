#include <stdarg.h>
#include <stdio.h>
#include <string.h>

// G2ME01: 0x8034c284, verified against retail Runtime/printf.s.
// MSL's formatter behind the game's printf family. It calls write_proc for each chunk of output and returns the
// total length (or -1 if write_proc returns NULL); it never writes a terminator itself.
int __pformatter(void *(*write_proc)(void *ctx, const char *data, size_t len), void *ctx, const char *format,
                 va_list args);

typedef struct {
  char *buf;
  size_t cap; // excluding the terminator
  size_t pos;
} BoundedWriter;

// Unlike the game's __StringWrite, keeps counting past the end so vsnprintf can report the untruncated length.
static void *bounded_write(void *ctx, const char *data, size_t len) {
  BoundedWriter *w = (BoundedWriter *)ctx;
  if (w->pos < w->cap) {
    size_t n = w->cap - w->pos < len ? w->cap - w->pos : len;
    memcpy(w->buf + w->pos, data, n);
  }
  w->pos += len;
  return w;
}

int vsnprintf(char *buf, size_t size, const char *format, va_list args) {
  BoundedWriter w = {buf, size ? size - 1 : 0, 0};
  int len = __pformatter(bounded_write, &w, format, args);
  if (size) {
    buf[w.pos < w.cap ? w.pos : w.cap] = '\0';
  }
  return len;
}

int snprintf(char *buf, size_t size, const char *format, ...) {
  va_list args;
  va_start(args, format);
  int len = vsnprintf(buf, size, format, args);
  va_end(args);
  return len;
}
