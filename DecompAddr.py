#!/usr/bin/env python3
"""Look up Echoes main-DOL ownership in the decomp symbols and splits (RELs are separate)."""
import argparse
import re
from pathlib import Path


def lookup(decomp, version, address):
    config = decomp / 'config' / version
    owners = []
    tu = None
    for line in (config / 'splits.txt').read_text().splitlines():
        if line and not line[0].isspace() and line.endswith(':'):
            tu = line[:-1]
        m = re.search(r'(\S+)\s+start:(0x[\da-fA-F]+) end:(0x[\da-fA-F]+)', line)
        if m and int(m[2], 16) <= address < int(m[3], 16):
            owners.append(f'{m[1]} {tu} [{m[2]}, {m[3]})')
    symbols = []
    for line in (config / 'symbols.txt').read_text().splitlines():
        m = re.match(r'(.*?) = (\S+):(0x[\da-fA-F]+);.*?size:(0x[\da-fA-F]+)', line)
        if m and int(m[3], 16) <= address < int(m[3], 16) + int(m[4], 16):
            symbols.append(f'{m[1]} +0x{address - int(m[3], 16):x} ({m[2]})')
    return '\n'.join([f'0x{address:08x}:'] + owners + symbols + ([] if owners or symbols else ['  outside known main-DOL symbols/splits; do not infer free memory']))


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('addresses', nargs='+', type=lambda s: int(s, 16))
    parser.add_argument('--decomp', type=Path, default=Path(__file__).resolve().parent.parent / 'echoes-decomp')
    parser.add_argument('--version', default='G2ME01')
    args = parser.parse_args()
    for address in args.addresses:
        print(lookup(args.decomp, args.version, address))


if __name__ == '__main__':
    main()
