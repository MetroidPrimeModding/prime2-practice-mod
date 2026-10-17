#pragma once
namespace CardGate {
  void tick(); // Per render frame; issues only asynchronous card transfers.
  void requestSave();
  void requestLoad();
  bool saveRequested();
  bool busy();
  bool dirty();
  const char *message();
  void driverConstructing();
  void driverDestroyed();
  void drain(); // Driver construction/reset: stop scheduling, wait for current DMA, release ownership.
} // namespace CardGate
