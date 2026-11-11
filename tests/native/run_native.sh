
#!/usr/bin/env bash
set -euo pipefail

ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/../.." && pwd)"

SRC="$ROOT/firmware/main/sensor_service.c"
TEST="$ROOT/tests/native/test_sensor_service.c"
STUBS="$ROOT/tests/native/stubs"
INCLUDE="$ROOT/firmware/main"

BUILD_DIR="$(mktemp -d)"
trap 'rm -rf "$BUILD_DIR"' EXIT

echo "Building native sensor-service tests..."

gcc \
    -std=c11 \
    -Wall -Wextra -Werror \
    -I "$STUBS" \
    -I "$INCLUDE" \
    "$SRC" \
    "$TEST" \
    -o "$BUILD_DIR/test_sensor_service"

echo "Running native tests..."

"$BUILD_DIR/test_sensor_service"

echo "Native test execution completed."
