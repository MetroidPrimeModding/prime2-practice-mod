#!/usr/bin/env python3
"""Carveout handling for the two-pass link.

  header   - C header with the selected carveouts, for _earlyboot_memset to skip
  ldscript - linker script with nothing in carveouts (pass 1)
  pack     - pack input sections from a pass-1 map into carveouts, emit the pass-2 linker script
"""
import argparse
import fnmatch
import json
import re
import sys
from dataclasses import dataclass, field
from pathlib import Path

# Output sections in the template that hold packable input sections during pass 1.
MAINDATA_OUTPUT_SECTIONS = {".text", ".data"}

# lld map line: VMA LMA Size Align <indent><text>. Output sections have no extra indent, input
# sections 8 extra spaces, symbols 16.
MAP_LINE = re.compile(r"^\s*([0-9a-f]+)\s+([0-9a-f]+)\s+([0-9a-f]+)\s+(\d+) ( *)(.*)$")
INPUT_SECTION = re.compile(r"^(.*):\((.*)\)$")


@dataclass
class Carveout:
  name: str
  start: int
  size: int
  priority: int
  # Segments reloaded through DI on reset (OSReboot) are DMA'd with the address and length rounded down to 32
  # bytes, so each segment must start and end on a 32-byte boundary to load where it was linked.
  seg_align: int = 32

  @property
  def load_start(self):
    return align_up(self.start, self.seg_align)

  @property
  def load_end(self):
    return (self.start + self.size) // self.seg_align * self.seg_align

  @property
  def load_size(self):
    return max(0, self.load_end - self.load_start)


@dataclass
class Item:
  section: str
  size: int
  align: int


@dataclass
class Bin:
  carveout: Carveout
  limit: int
  items: list = field(default_factory=list)
  cursor: int = 0
  max_align: int = 32

  def __post_init__(self):
    self.cursor = self.carveout.load_start

  @property
  def load_start(self):
    return align_up(self.carveout.load_start, self.max_align)

  def try_add(self, item):
    alignment = max(self.max_align, item.align)
    if alignment > self.max_align:
      # Increasing the output alignment moves every input, so recompute the bin.
      end = align_up(self.carveout.load_start, alignment)
      for entry in self.items + [item]:
        end = align_up(end, entry.align) + entry.size
    else:
      end = align_up(self.cursor, item.align) + item.size
    if align_up(end, self.carveout.seg_align) > self.limit:
      return False
    self.max_align = alignment
    self.cursor = end
    self.items.append(item)
    return True

  def used(self):
    return align_up(self.cursor, self.carveout.seg_align) - self.load_start if self.items else 0


def align_up(v, a):
  return (v + a - 1) // a * a


def load_config(path):
  cfg = json.loads(Path(path).read_text())
  carveouts = [
    Carveout(c["name"], int(c["start"], 0), int(c["size"], 0), int(c["priority"]))
    for c in cfg["carveouts"]
  ]
  if cfg.get("stomps"):
    sys.exit("Code reclamation is outside step 6; stomps are disabled")
  names = [c.name for c in carveouts]
  if len(names) != len(set(names)):
    sys.exit(f"{path}: duplicate carveout names")
  profile_path = Path(path).resolve().parent / cfg["dol_profile"]
  profile = json.loads(profile_path.read_text())
  # One free section is reserved for maindata. The patcher can use text or data slots.
  cfg["max_carveouts"] = sum(n == 0 for n in profile["section_sizes"]) - 1
  cfg["profile"] = profile
  previous_end = 0
  for c in sorted(carveouts, key=lambda c: c.start):
    if c.name in {"maindata", "embedded_data"} or not re.fullmatch(r"[A-Za-z_][A-Za-z0-9_]*", c.name):
      sys.exit("Invalid carveout name")
    if c.size <= 0 or c.load_size <= 0 or c.start < previous_end:
      sys.exit("Empty or overlapping carveout")
    if c.start < profile["bss_start"] or c.start + c.size > profile["bss_start"] + profile["bss_size"]:
      sys.exit("Carveout outside verified Echoes BSS")
    for addr, size in zip(profile["section_addresses"], profile["section_sizes"]):
      if size and c.start < addr + size and addr < c.start + c.size:
        sys.exit("Carveout overlaps retail DOL section")
    previous_end = c.start + c.size
  if cfg["reserve_bytes"] < 0 or cfg["pack_input_sections"] != [".text.*", ".rodata.*"]:
    sys.exit("Only immutable code/rodata may be packed")
  cfg["carveouts"] = carveouts
  return cfg


def selected_carveouts(cfg):
  ranked = sorted(cfg["carveouts"], key=lambda c: (c.priority, -c.size, c.start))
  return ranked[: cfg["max_carveouts"]]


def matches_any(name, patterns):
  return any(fnmatch.fnmatchcase(name, p) for p in patterns)


def parse_map(map_path, cfg):
  items = {}
  current_out = None
  for line in Path(map_path).read_text().splitlines():
    m = MAP_LINE.match(line)
    if not m:
      continue
    size, align, indent, text = int(m.group(3), 16), int(m.group(4)), len(m.group(5)), m.group(6)
    if indent == 0:
      current_out = text
      continue
    if indent != 8 or current_out not in MAINDATA_OUTPUT_SECTIONS:
      continue
    im = INPUT_SECTION.match(text)
    if not im:
      continue
    obj, section = im.group(1), im.group(2)
    # <internal> sections are lld-synthesized (merged strings/constants) and can't be selected by name
    if obj == "<internal>" or size == 0:
      continue
    if not matches_any(section, cfg["pack_input_sections"]) or matches_any(section, cfg["never_pack"] + [".text._prolog*", ".text._earlyboot_memset*", ".boot*", ".init*"]):
      continue
    # Same-named sections from different objects all match one rule, so they must be placed together.
    if section in items:
      it = items[section]
      it.align = max(it.align, align)
      it.size = align_up(it.size, it.align) + size
    else:
      items[section] = Item(section, size, align)
  return list(items.values())


def pack(items, cfg):
  bins = [Bin(c, c.load_end - cfg["reserve_bytes"]) for c in selected_carveouts(cfg)]
  leftover = []
  for item in sorted(items, key=lambda i: (-i.size, i.section)):
    if not any(b.try_add(item) for b in bins):
      leftover.append(item)
  return bins, leftover


def render_ldscript(template_path, bins):
  phdrs = []
  sections = []
  for b in sorted((b for b in bins if b.items), key=lambda b: b.carveout.start):
    c = b.carveout
    phdrs.append(f"  {c.name} PT_LOAD;")
    rules = "\n".join(f"    *({it.section})" for it in b.items)
    sections.append(
      f"  . = 0x{b.load_start:08X};\n"
      f"  .carveout_{c.name} 0x{b.load_start:08X} :\n"
      f"  {{\n"
      f"{rules}\n"
      f"    . = ALIGN({c.seg_align});\n"
      f'    ASSERT(. <= 0x{c.load_end:08X}, "carveout {c.name} overflow");\n'
      f"  }} :{c.name}"
    )
  text = Path(template_path).read_text()
  text = text.replace("@CARVEOUT_PHDRS@", "\n".join(phdrs))
  text = text.replace("@CARVEOUT_SECTIONS@", "\n\n".join(sections))
  return text


def write(path, text):
  p = Path(path)
  p.parent.mkdir(parents=True, exist_ok=True)
  p.write_text(text)


def cmd_header(args):
  cfg = load_config(args.config)
  blocks = sorted(selected_carveouts(cfg), key=lambda c: c.start)
  lines = ["#pragma once", "", "#include <types.h>", "",
           "// Generated from carveouts.json: {start, size} pairs sorted by address.",
           'static const u32 CARVEOUT_SAFE_BLOCKS[] __attribute__((section(".boot.rodata"))) = {']
  lines += [f"    0x{c.load_start:08X}, 0x{c.load_size:X}, // {c.name}" for c in blocks]
  lines += ["};", ""]
  write(args.out, "\n".join(lines))


def cmd_ldscript(args):
  write(args.out, render_ldscript(args.template, []))


def report_row(name, addr, row, note):
  size, content, pad, lost, free = row
  filled = 100 * content / size if size else 0
  return (f"{name:<24} {addr:<10} {size:>7X} {content:>7X} {pad:>5X} {lost:>5X} {free:>6X}  {filled:5.1f}% "
          f"{note}").rstrip()


def cmd_pack(args):
  cfg = load_config(args.config)
  items = parse_map(args.map, cfg)
  if not items:
    sys.exit(f"{args.map}: found no packable input sections; is this a pass-1 map?")
  bins, leftover = pack(items, cfg)
  write(args.out, render_ldscript(args.template, bins))

  report = [f"DOL {cfg['profile']['revision']}: {cfg['max_carveouts']} carveout slots after maindata reservation", f"{'region':<24} {'address':<10} {'size':>7} {'code':>7} {'pad':>5} {'lost':>5} {'free':>6}  filled"]
  totals = [0, 0, 0, 0, 0]
  for b in bins:
    c = b.carveout
    content = sum(it.size for it in b.items)
    used = b.used()
    trimmed = c.size - c.load_size + b.load_start - c.load_start
    row = [c.size, content, used - content, trimmed, c.size - trimmed - used]
    totals = [t + v for t, v in zip(totals, row)]
    report.append(report_row(c.name, f"0x{b.load_start:08X}", row, f"({len(b.items)} section{'s' if len(b.items) != 1 else ''})"))
  report.append(report_row("total", "", totals, ""))
  report.append("code: packed sections, pad: alignment padding, lost: trimmed to align the segment, "
                "filled: code / size")
  left_size = sum(i.size for i in leftover)
  report.append(f"left in maindata: 0x{left_size:X} bytes of packable sections ({len(leftover)} sections)")
  report.append("largest leftovers:")
  report += [f"  0x{i.size:05X} {i.section}" for i in leftover[:10]]
  text = "\n".join(report) + "\n"
  print(text, end="")
  if args.report:
    Path(args.report).write_text(text)


def main():
  parser = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
  sub = parser.add_subparsers(dest="cmd", required=True)

  p = sub.add_parser("header")
  p.add_argument("--config", required=True)
  p.add_argument("--out", required=True)
  p.set_defaults(func=cmd_header)

  p = sub.add_parser("ldscript")
  p.add_argument("--template", required=True)
  p.add_argument("--out", required=True)
  p.set_defaults(func=cmd_ldscript)

  p = sub.add_parser("pack")
  p.add_argument("--config", required=True)
  p.add_argument("--template", required=True)
  p.add_argument("--map", required=True)
  p.add_argument("--out", required=True)
  p.add_argument("--report")
  p.set_defaults(func=cmd_pack)

  args = parser.parse_args()
  args.func(args)


if __name__ == "__main__":
  main()
