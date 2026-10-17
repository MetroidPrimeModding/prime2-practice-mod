#pragma once
namespace SaveAnywhere {
  const char *whyNot();
  void draw();
  bool consumeCloseRequest();
  void tick();   // Render-frame timeout, including frames where gameplay is paused.
  void update(); // Only from the native gameplay update.
  void cancel();
} // namespace SaveAnywhere
