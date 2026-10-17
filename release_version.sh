#!/usr/bin/env bash
# Print the configured version; optionally require an exact matching v<version> tag.
set -euo pipefail
DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
if (( $# > 1 )); then echo 'Usage: release_version.sh [tag]' >&2; exit 1; fi
python3 - "${DIR}/src/patcher_config.toml" "${1:-}" <<'PY'
import re
import sys
import tomllib

with open(sys.argv[1], 'rb') as config:
    version = tomllib.load(config).get('version', '')
if not isinstance(version, str) or not re.fullmatch(r'[0-9]+\.[0-9]+\.[0-9]+(?:-[0-9A-Za-z]+(?:[.-][0-9A-Za-z]+)*)?', version):
    sys.exit('Expected a release version such as 1.2.5 in src/patcher_config.toml')
if sys.argv[2] and sys.argv[2] != 'v' + version:
    sys.exit(f'Tag {sys.argv[2]!r} does not match configured version v{version}')
print(version)
PY
