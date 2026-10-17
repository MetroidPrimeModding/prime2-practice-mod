#!/usr/bin/env python3
"""Explore hypothetical main-DOL reachability changes in Echoes G2ME01.

No output is proof that memory is safe to overwrite. Seeds assume entry paths have
already been disabled. Computed calls, assembly omissions, resource callbacks and
randomizer uses beyond the mod symbol list require independent retail investigation.
REL code is never reported as reclaimable main-DOL memory. All textual references
from every configured REL to DOL symbols are conservatively retained as roots,
including references with an ambiguous same-name module definition.

Run without seeds to inventory coverage; supply exact decomp mangled names to
explore a hypothesis. This tool does not generate stomp configuration or patches.
"""
import argparse
import re
import sys
from collections import defaultdict
from pathlib import Path

# Names with template args are quoted and may contain commas: .fn "vector<a,b>::f", weak
DEF = re.compile(r'^\.(fn|obj)\s+("[^"]+"|[^,\s]+)(?:,\s*(\w+))?')
END = re.compile(r"^\.end(fn|obj)\s")
SECTION = re.compile(r"^(\.text|\.data|\.rodata|\.init|\.bss|\.sdata2?|\.sbss2?)\b|^\.section\s+([^,\s]+)")
SIZE_COMMENT = re.compile(r"^# \.\S+:0x[0-9A-Fa-f]+ \| 0x([0-9A-Fa-f]{8}) \| size: 0x([0-9A-Fa-f]+)")
TOKEN = re.compile(r'"[^"]+"|[A-Za-z_$@.][\w$@.<>,:\-]*')
RELOC_SUFFIX = re.compile(r"@(ha|h|l|sda21|sda2)$")
# Exception tables reference every function they cover; that isn't a use.
IGNORED_REFERRER_SECTIONS = {"extab", "extabindex"}
MOD = ("", "<mod>")
REL = ("", "<REL references>")


class Program:
  def __init__(self, asm_dir):
    self.defs = {}  # key -> dict(name, file, section, addr, size); key is (file, name) for locals, ("", name) else
    self.locals = defaultdict(dict)
    self.globals = {}
    self.refs = defaultdict(set)  # target key -> referrer keys
    body = defaultdict(list)
    files = sorted(asm_dir.rglob("*.s"))
    if not files:
      raise ValueError(f"no disassembly under {asm_dir}")
    self.file_count = len(files)
    for f in files:
      self._parse(f, body)
    for key, lines in body.items():
      if self.defs[key]["section"] not in IGNORED_REFERRER_SECTIONS:
        self._add_refs(key, lines)

  def _parse(self, f, body):
    section = cur = pending = None
    for line in f.read_text(errors="replace").splitlines():
      s = line.strip()
      if m := SECTION.match(s):
        section = m.group(2) or m.group(1)
      elif m := SIZE_COMMENT.match(s):
        pending = (int(m.group(1), 16), int(m.group(2), 16))
      elif m := DEF.match(s):
        name = m.group(2).strip('"')
        local = m.group(3) == "local"
        cur = (str(f), name) if local else ("", name)
        addr, size = pending or (None, None)
        if cur in self.defs:
          raise ValueError(f"duplicate definition {name}: {f}")
        self.defs[cur] = dict(name=name, file=str(f), section=section, addr=addr, size=size)
        (self.locals[str(f)] if local else self.globals)[name] = cur
        pending = None
      elif END.match(s):
        cur = None
      elif cur is not None:
        body[cur].append(s)

  def resolve(self, file, name):
    return self.locals[file].get(name) or self.globals.get(name)

  def _add_refs(self, key, lines):
    file = self.defs[key]["file"]
    for s in lines:
      code = s.split("*/", 1)[-1].split("#", 1)[0]
      for tok in TOKEN.findall(code):
        target = self.resolve(file, RELOC_SUFFIX.sub("", tok.strip('"')))
        if target and target != key:
          self.refs[target].add(key)


def main():
  parser = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
  parser.add_argument("--decomp", type=Path, default=Path(__file__).resolve().parents[3] / "echoes-decomp",
                      help="Echoes decomp checkout, built for G2ME01")
  parser.add_argument("--mod-symbols", type=Path,
                      default=Path(__file__).resolve().parents[2] / "src" / "prime-practice.lst")
  parser.add_argument("seeds", nargs="*", help="hypothetically disabled main-DOL functions (not safety proof)")
  args = parser.parse_args()

  build = args.decomp / "build" / "G2ME01"
  prog = Program(build / "asm")
  # Read the authoritative module list, rather than accepting a partial build.
  config = (args.decomp / "config" / "G2ME01" / "config.yml").read_text()
  modules = re.findall(r"^  - object: files/RelProd/([^/]+)\.rel\s*$", config, re.M)
  if not modules or len(modules) != len(set(modules)):
    raise ValueError("missing or duplicate configured REL modules")
  rel_files = 0
  for module in modules:
    files = sorted((build / module / "asm").rglob("*.s"))
    if not files:
      raise ValueError(f"missing REL disassembly: {module}")
    rel_files += len(files)
    for file in files:
      for line in file.read_text().splitlines():
        code = line.split("*/", 1)[-1].split("#", 1)[0]
        if code.lstrip().startswith("."):
          # Data directives can contain function pointers; declarations cannot.
          if not code.lstrip().startswith((".4byte", ".long")):
            continue
        for token in TOKEN.findall(code):
          name = RELOC_SUFFIX.sub("", token.strip('"'))
          if key := prog.globals.get(name):
            prog.refs[key].add(REL)
  # ABI bridges can use different linker names; retain address matches too.
  by_addr = defaultdict(set)
  for key, definition in prog.defs.items():
    by_addr[definition["addr"]].add(key)
  for line in args.mod_symbols.read_text().splitlines():
    parts = line.split()
    if len(parts) >= 2 and not line.lstrip().startswith("#"):
      try:
        address = int(parts[0], 16)
      except ValueError:
        continue
      for key in by_addr.get(address, set()):
        prog.refs[key].add(MOD)
      if key := prog.globals.get(parts[1]):
        prog.refs[key].add(MOD)
  print(f"G2ME01 main DOL: {prog.file_count} files, {len(prog.defs)} definitions; "
        f"REL: {len(modules)} modules, {rel_files} files (excluded from ranges)", file=sys.stderr)
  print(f"Protected definitions: mod={sum(MOD in r for r in prog.refs.values())}, "
        f"REL={sum(REL in r for r in prog.refs.values())}", file=sys.stderr)
  if not args.seeds:
    return

  seeds = set()
  for s in args.seeds:
    if s not in prog.globals:
      sys.exit(f"unknown seed {s}")
    key = prog.globals[s]
    if prog.defs[key]["section"] not in {".text", ".init"}:
      sys.exit(f"seed is not main-DOL code: {s}")
    if {MOD, REL} & prog.refs[key]:
      sys.exit(f"seed has protected mod/REL references: {s}")
    seeds.add(key)

  uses = defaultdict(set)  # referrer -> targets
  for target, referrers in prog.refs.items():
    for r in referrers:
      uses[r].add(target)
  roots = [MOD, REL] + [k for k in prog.defs if not prog.refs.get(k)]

  def reachable(blocked):
    seen = set()
    stack = [r for r in roots if r not in blocked]
    while stack:
      k = stack.pop()
      if k in seen:
        continue
      seen.add(k)
      stack.extend(t for t in uses[k] if t not in blocked)
    return seen

  dead = (reachable(set()) - reachable(seeds)) | seeds
  dead.discard(MOD)
  dead.discard(REL)
  print("HYPOTHETICAL main-DOL candidates only; no ranges approved", file=sys.stderr)

  for k in sorted(dead, key=lambda k: prog.defs[k]["addr"] or 0):
    d = prog.defs[k]
    if d["addr"] is None or d["size"] is None:
      sys.exit(f"missing range metadata: {d['name']}")
    print(f"{d['section']:<8} 0x{d['addr']:08X} 0x{d['size']:05X} {d['name']}  [{Path(d['file']).name}]")


if __name__ == "__main__":
  try:
    main()
  except (OSError, ValueError) as error:
    sys.exit(str(error))
