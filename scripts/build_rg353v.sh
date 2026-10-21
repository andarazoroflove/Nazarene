#!/bin/bash
set -e

# ==============================================================================
# The Nazarene: Way of the Cross - Anbernic RG-353V Automated Build & Package
# Runs Ubuntu 20.04 Docker container (eo-rg353v-builder) to target GLIBC 2.31
# ==============================================================================

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
WS_DIR="$(cd "$SCRIPT_DIR/.." && pwd)"
EO_DIR="$(cd "$WS_DIR/../ExecutiveOrder" && pwd)"

echo "========================================================================"
echo " Building Nazarene for Anbernic RG-353V (ARM64 / RK3566 / GLIBC 2.31)"
echo " Workspace: $WS_DIR"
echo "========================================================================"

# 1. Sync assets into dist directory
echo "[1/4] Syncing assets into dist_rg353v/ports/Nazarene/assets..."
mkdir -p "$WS_DIR/dist_rg353v/ports/Nazarene/assets"
cp -ru "$WS_DIR/assets/"* "$WS_DIR/dist_rg353v/ports/Nazarene/assets/"
if [ -f "$WS_DIR/title.jpg" ]; then
    cp -u "$WS_DIR/title.jpg" "$WS_DIR/dist_rg353v/ports/Nazarene/"
fi

# 2. Run compilation inside eo-rg353v-builder docker container
echo "[2/4] Running ARM64 cross-compilation in eo-rg353v-builder container..."
docker run --rm \
    -v "$WS_DIR:/workspace/Nazarene" \
    -v "$EO_DIR:/workspace/ExecutiveOrder" \
    eo-rg353v-builder:latest \
    bash /workspace/Nazarene/scripts/compile_rg353v_docker.sh

# 3. Verify ELF binary
echo "[3/4] Verifying Nazarene.aarch64 GLIBC symbols..."
BIN="$WS_DIR/dist_rg353v/ports/Nazarene/Nazarene.aarch64"
if [ ! -f "$BIN" ]; then
    echo "ERROR: $BIN was not built!"
    exit 1
fi

# 4. Create release archive
echo "[4/4] Packaging Nazarene-v5.0-anbernic-rg353v.zip..."
cd "$WS_DIR/dist_rg353v"
zip -r -q "$WS_DIR/Nazarene-v5.0-anbernic-rg353v.zip" ports

echo "========================================================================"
echo " BUILD SUCCESSFUL!"
echo " Output archive: $WS_DIR/Nazarene-v5.0-anbernic-rg353v.zip"
ls -lh "$WS_DIR/Nazarene-v5.0-anbernic-rg353v.zip"
echo "========================================================================"
