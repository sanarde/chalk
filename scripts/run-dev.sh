#!/bin/sh
# SPDX-License-Identifier: GPL-3.0-only

set -eu

root=$(cd "$(dirname "$0")/.." && pwd)
style=${CHALK_STYLE_PREFIX:-$HOME/.local}

cmake -S "$root" -B "$root/build" -G Ninja -DCMAKE_PREFIX_PATH="$style" >/dev/null
cmake --build "$root/build"

QML_IMPORT_PATH="$style/lib/qt6/qml${QML_IMPORT_PATH:+:$QML_IMPORT_PATH}" \
LD_LIBRARY_PATH="$style/lib${LD_LIBRARY_PATH:+:$LD_LIBRARY_PATH}" \
exec "$root/build/src/chalk" "$@"
