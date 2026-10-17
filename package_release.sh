#!/usr/bin/env bash
# Package an existing Echoes ELF. Optional asset directory supports verified offline downloads.
set -euo pipefail
if (( $# < 3 || $# > 4 )); then
  echo 'Usage: package_release.sh <version> <ELF> <output-directory> [patcher-asset-directory]' >&2
  exit 1
fi
DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
VERSION="$1"
ELF="$(realpath "$2")"
OUT="$3"
ASSET_DIR="${4:-}"
# Update this tag and packaging/patchers-v1.1.0.sha256 together; never use latest.
PATCHER_TAG=v1.1.0
PATCHER_URL="https://github.com/MetroidPrimeModding/gcn-static-patcher/releases/download/${PATCHER_TAG}"
[[ "${VERSION}" == "$("${DIR}/release_version.sh")" ]] || {
  echo 'Package version must match src/patcher_config.toml' >&2; exit 1;
}
"${DIR}/release_notes.sh" "${VERSION}" >/dev/null
[[ ! -e "${OUT}" && ! -L "${OUT}" ]] || { echo "${OUT} already exists" >&2; exit 1; }
# Reject game files, wrong architectures, and stale/mismatched mod metadata before downloading assets.
python3 - "${ELF}" "${DIR}/src/patcher_config.toml" <<'PY'
import struct
import sys
import tomllib
from pathlib import Path

try:
    elf = Path(sys.argv[1]).read_bytes()
    if elf[:6] != b'\x7fELF\x01\x02' or struct.unpack_from('>H', elf, 18)[0] != 20:
        raise ValueError('Expected a 32-bit big-endian PowerPC ELF')
    offset = struct.unpack_from('>I', elf, 32)[0]
    stride, count, strings_index = struct.unpack_from('>HHH', elf, 46)
    sections = [struct.unpack_from('>10I', elf, offset + i * stride) for i in range(count)]
    strings = sections[strings_index]
    names = elf[strings[4]:strings[4] + strings[5]]
    configs = [s for s in sections if names[s[0]:].split(b'\0', 1)[0] == b'.patcher_config']
    if len(configs) != 1:
        raise ValueError('Expected exactly one embedded patcher configuration')
    section = configs[0]
    embedded = tomllib.loads(elf[section[4]:section[4] + section[5]].decode())
    configured = tomllib.loads(Path(sys.argv[2]).read_text())
    if embedded != configured:
        raise ValueError('ELF patcher configuration differs from src/patcher_config.toml; rebuild Release')
except (ValueError, IndexError, struct.error, OSError) as error:
    sys.exit(str(error))
PY

PATCHERS="$(mktemp -d)"
trap 'rm -rf "${PATCHERS}"' EXIT
# Names and hashes come from the committed manifest, not the network response.
while read -r hash asset; do
  if [[ -n "${ASSET_DIR}" ]]; then
    cp "${ASSET_DIR}/${asset}" "${PATCHERS}/${asset}"
  else
    curl --fail --location --silent --show-error --retry 3 \
      -o "${PATCHERS}/${asset}" "${PATCHER_URL}/${asset}"
  fi
done < "${DIR}/packaging/patchers-v1.1.0.sha256"
(cd "${PATCHERS}" && sha256sum --check "${DIR}/packaging/patchers-v1.1.0.sha256")

fetch() {
  mkdir "${PATCHERS}/$2"
  case "$1" in
    *.zip) unzip -q "${PATCHERS}/$1" -d "${PATCHERS}/$2" ;;
    *.tar.gz) tar -xzf "${PATCHERS}/$1" -C "${PATCHERS}/$2" ;;
  esac
}
find_one() {
  local found
  found=$(find "${PATCHERS}/$1" -name "$2")
  if [[ -z "${found}" || "${found}" == *$'\n'* ]]; then
    echo "Expected exactly one $2 in $1, found: ${found:-none}" >&2; exit 1
  fi
  printf '%s\n' "${found}"
}
fetch gcn-static-patcher-windows.zip windows
fetch gcn-static-patcher-linux-x86_64.tar.gz linux
fetch gcn-static-patcher-linux-arm64.tar.gz linux-arm
# This signed/notarized macOS asset is uploaded separately by upstream.
fetch gcn-static-patcher-gui-macos-universal.zip macos
find "${PATCHERS}/macos" \( -name '__MACOSX' -o -name '._*' \) -prune -exec rm -rf {} +
find "${PATCHERS}" -type f \( -name gcn-static-patcher-gui -o -path '*/Contents/MacOS/*' \) -exec chmod +x {} +
WINDOWS_GUI=$(find_one windows gcn-static-patcher-gui.exe)
LINUX_GUI=$(find_one linux gcn-static-patcher-gui)
LINUX_ARM_GUI=$(find_one linux-arm gcn-static-patcher-gui)
MACOS_GUI=$(find_one macos gcn-static-patcher-gui.app)

mkdir -p "${OUT}"
OUT="$(cd "${OUT}" && pwd)"
package() {
  local name="prime2-practice-${VERSION}-$1"
  mkdir "${OUT}/${name}"
  cp "${ELF}" "${OUT}/${name}/mod.elf"
  cp "${DIR}/README.md" "${OUT}/${name}/"
  cp -a "$2" "${OUT}/${name}/$3"
  (
    cd "${OUT}"
    case "$4" in
      zip) zip -qry "${name}.zip" "${name}" ;;
      tar.gz) tar -czf "${name}.tar.gz" "${name}" ;;
    esac
  )
}
cp "${ELF}" "${OUT}/prime2-practice-${VERSION}.elf"
package windows "${WINDOWS_GUI}" patcher.exe zip
package linux "${LINUX_GUI}" patcher tar.gz
package linux-arm "${LINUX_ARM_GUI}" patcher tar.gz
package macos "${MACOS_GUI}" patcher.app zip
