#!/usr/bin/env bash
set -e
ROOT="$(cd "$(dirname "$0")" && pwd)"
BUILD_DIR="$ROOT/build"

# If GameNetworkingSockets isn't installed system-wide, point GNS_ROOT at your
# local build/install, e.g.:
#   GNS_ROOT=/opt/GameNetworkingSockets ./build.sh
# or build and install GNS first:
#   git clone https://github.com/ValveSoftware/GameNetworkingSockets
#   cmake -B gns_build -DCMAKE_INSTALL_PREFIX=/opt/gns GameNetworkingSockets
#   cmake --build gns_build --target install
#   GNS_ROOT=/opt/gns ./build.sh

CMAKE_EXTRA_ARGS=()
if [ -n "$GNS_ROOT" ]; then
    CMAKE_EXTRA_ARGS+=("-DGNS_ROOT=$GNS_ROOT")
fi

cmake -B "$BUILD_DIR" -S "$ROOT" \
    -DCMAKE_BUILD_TYPE=Debug \
    "${CMAKE_EXTRA_ARGS[@]}"

cmake --build "$BUILD_DIR" -- -j"$(nproc)"

echo ""
echo "Build complete. Binaries:"
echo "  $BUILD_DIR/server/server"
echo "  $BUILD_DIR/client/client"

ln -sf build/compile_commands.json "$ROOT/compile_commands.json"
