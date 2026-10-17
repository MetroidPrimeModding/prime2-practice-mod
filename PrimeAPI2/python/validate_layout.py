#!/usr/bin/env python3
"""Validate the ELF reload contract, optionally against a retail or patched G2ME01 DOL."""
import argparse
import hashlib
import json
import struct
from pathlib import Path


def elf_layout(path, absolute_symbols=None):
    data = Path(path).read_bytes()
    assert data[:7] == b'\x7fELF\x01\x02\x01', 'expected big-endian ELF32'
    h = struct.unpack_from('>16sHHIIIIIHHHHHH', data)
    assert h[2] == 20, 'expected PowerPC'
    loads = []
    for i in range(h[10]):
        p = struct.unpack_from('>8I', data, h[5] + i * h[9])
        if p[0] == 1 and p[4]:
            _, off, va, pa, size, mem, flags, align = p
            assert va == pa and va % 32 == 0 and size % 32 == 0, 'DMA start/length alignment'
            assert size == mem and off + size <= len(data), 'reload must include zero-initialized data'
            assert 0x80000000 <= va < va + size <= 0x81800000, 'MEM1 bounds'
            loads.append((va, size, data[off:off + size]))
    sections = [struct.unpack_from('>10I', data, h[6] + i * h[11]) for i in range(h[12])]
    symbols = {}
    for section in sections:
        if section[1] != 2:
            continue
        strings = sections[section[6]]
        names = data[strings[4]:strings[4] + strings[5]]
        for off in range(section[4], section[4] + section[5], section[9]):
            name, value, size, info, other, index = struct.unpack_from('>IIIBBH', data, off)
            label = names[name:].split(b'\0', 1)[0].decode()
            symbols[label] = value
            if absolute_symbols is not None and index == 0xfff1:
                absolute_symbols[label] = value
    return loads, symbols, sections


def validate(elf, config, dol=None, patched=False):
    from carveouts import load_config, selected_carveouts
    cfg = load_config(config)
    profile = cfg['profile']
    absolute = {}
    loads, symbols, sections = elf_layout(elf, absolute)
    regions = [(c.load_start, c.load_end) for c in selected_carveouts(cfg)]
    stomps = [(c.load_start, c.load_end) for c in cfg['stomps']]
    regions += stomps
    def is_stomp(va, size):
        return any(lo <= va < va + size <= hi for lo, hi in stomps)
    for label, address in absolute.items():
        assert not any(lo <= address < hi for lo, hi in stomps), f'game binding {label} inside reclaimed code'
    start, end = symbols['_LINK_START'], symbols['_LINK_END']
    assert start == 0x80437A00 and end % 32 == 0, 'Echoes arena boundaries'
    assert sum(not is_stomp(va, size) for va, size, _ in loads) <= cfg['max_carveouts'] + 1, 'DOL capacity'
    occupied = []
    for va, size, content in loads:
        assert (va == start and va + size == end) or any(lo <= va < va + size <= hi for lo, hi in regions), 'unexpected segment'
        assert all(va + size <= lo or va >= hi for lo, hi in occupied), 'overlapping segments'
        occupied.append((va, va + size))
    boot_symbols = ['_prolog', '_earlyboot_memset'] + (['InstallGalleryHooks'] if stomps else [])
    for name in boot_symbols:
        assert start <= symbols[name] < end, 'boot code outside maindata'
    for section in sections:
        # Writable allocated sections, including .bss, must be in reloadable maindata.
        if section[2] & 3 == 3 and section[5]:
            assert start <= section[3] < section[3] + section[5] <= end, 'mutable data in carveout'
        if section[1] == 8 and section[5]:
            va, size = section[3], section[5]
            segment = next((p for p in loads if p[0] <= va and va + size <= p[0] + p[1]), None)
            assert segment and not any(segment[2][va - segment[0]:va - segment[0] + size]), 'BSS not backed by reloadable zeros'
    if dol:
        data = Path(dol).read_bytes()
        w = struct.unpack_from('>64I', data)
        if not patched:
            assert hashlib.sha256(data).hexdigest() == profile['dol_sha256'], 'retail revision mismatch'
            assert (list(w[:18]) == profile['section_offsets'] and list(w[18:36]) == profile['section_addresses']
                    and list(w[36:54]) == profile['section_sizes']), 'DOL profile mismatch'
            for entry in cfg.get('reclamation', {}).get('ranges', []):
                va, size = int(entry['start'], 0), int(entry['size'], 0)
                off = next(o + va - a for o, a, n in zip(w[:18], w[18:36], w[36:54]) if a <= va < va + size <= a + n)
                assert hashlib.sha256(data[off:off + size]).hexdigest() == entry['sha256'], 'stomp retail fingerprint mismatch'
        else:
            for va, size, content in loads:
                if is_stomp(va, size):
                    matches = [(o + va - a, a, n) for o, a, n in zip(w[:18], w[18:36], w[36:54])
                               if a <= va < va + size <= a + n]
                    # In-place loads must keep the enclosing retail text section and consume no slot.
                    assert any(a <= va < va + size <= a + n for a, n in
                               zip(profile['section_addresses'][:7], profile['section_sizes'][:7]) if n)
                else:
                    matches = [(o, a, n) for o, a, n in zip(w[:18], w[18:36], w[36:54]) if a == va and n == size]
                assert len(matches) == 1, 'missing/mis-sized patched section'
                off = matches[0][0]
                assert off % 32 == 0 and data[off:off + size] == content, 'DOL reload data/alignment'
            for i, original_size in enumerate(profile['section_sizes']):
                if original_size:
                    assert (w[i], w[18+i], w[36+i]) == (profile['section_offsets'][i],
                            profile['section_addresses'][i], original_size), 'retail section changed'
            def word(address):
                for off, va, size in zip(w[:18], w[18:36], w[36:54]):
                    if va <= address < va + size:
                        return struct.unpack_from('>I', data, off + address - va)[0]
                raise AssertionError('patch outside DOL')
            for address, target, link in [(0x80003510, symbols['_earlyboot_memset'], 1), (0x8035712C, symbols['_prolog'], 0)]:
                assert word(address) == (0x48000000 | ((target - address) & 0x3fffffc) | link), 'boot branch mismatch'
            for address in (0x8036DBBC, 0x8036DBF4):
                hi, lo = word(address), word(address + 4)
                actual = ((hi & 0xffff) << 16) + ((lo & 0xffff) - (0x10000 if lo & 0x8000 else 0))
                assert actual == end, 'arena patch mismatch'
    print(f'{elf}: {len(loads)} aligned reload segment(s), {sum(is_stomp(va, size) for va, size, _ in loads)} in-place; arena 0x{start:08x}..0x{end:08x}; boot/data bounds OK')


def main():
    p = argparse.ArgumentParser(description=__doc__)
    p.add_argument('elf')
    p.add_argument('--config', default='carveouts.json')
    p.add_argument('--dol')
    p.add_argument('--patched', action='store_true')
    args = p.parse_args()
    validate(args.elf, args.config, args.dol, args.patched)


if __name__ == '__main__':
    main()
