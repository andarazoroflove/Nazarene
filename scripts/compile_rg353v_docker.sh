#!/bin/bash
set -e

echo "========================================================================"
echo " Building Nazarene for Anbernic RG-353V (Ubuntu 20.04 / GLIBC 2.31 / ARM64)"
echo "========================================================================"

WS_DIR="/workspace/Nazarene"
EO_DIR="/workspace/ExecutiveOrder"
RAYLIB_SRC="$EO_DIR/build/_deps/raylib-src/src"
RAYLIB_LIB="$EO_DIR/build_rg353v_arm64/libraylib.a"
DIST_DIR="$WS_DIR/dist_rg353v/ports/Nazarene"
TMP_OBJ_DIR="/tmp/nazarene_rg353v_objs"

mkdir -p "$TMP_OBJ_DIR"
mkdir -p "$DIST_DIR/libs"

CC="aarch64-linux-gnu-gcc"
CXX="aarch64-linux-gnu-g++"
CFLAGS="-O3 -DPLATFORM_DESKTOP_SDL -DGRAPHICS_API_OPENGL_ES2 -D_GNU_SOURCE -I$RAYLIB_SRC -I/usr/include/SDL2"
CXXFLAGS="-O3 -std=c++17 -DPLATFORM_DESKTOP_SDL -DGRAPHICS_API_OPENGL_ES2 -D_GNU_SOURCE -I$RAYLIB_SRC -I$WS_DIR/src -I/usr/include/SDL2"

SRCS=(
    "src/main.cpp"
    "src/Input.cpp"
    "src/Audio.cpp"
    "src/Graphics.cpp"
    "src/Level.cpp"
    "src/Player.cpp"
    "src/Enemies.cpp"
    "src/BonusStage.cpp"
    "src/Mission2Graphics.cpp"
    "src/Mission3Graphics.cpp"
    "src/Mission4Graphics.cpp"
    "src/Mission5Graphics.cpp"
)

echo "[1/2] Compiling C++17 sources in parallel for aarch64..."
OBJS=()
for src in "${SRCS[@]}"; do
    obj="$TMP_OBJ_DIR/$(basename "${src%.cpp}.o")"
    if [ ! -f "$obj" ] || [ "$WS_DIR/$src" -nt "$obj" ]; then
        echo "  -> Compiling $src..."
        $CXX -c $CXXFLAGS "$WS_DIR/$src" -o "$obj" &
    else
        echo "  -> Up-to-date: $src"
    fi
    OBJS+=("$obj")
done
wait

echo "[2/2] Linking Nazarene.aarch64 with Raylib GLES2 + SDL2..."
$CXX -O3 -std=c++17 "${OBJS[@]}" "$RAYLIB_LIB" \
    "$DIST_DIR/libs/libSDL2-2.0.so.0" \
    -L"$DIST_DIR/libs" -L/usr/lib/aarch64-linux-gnu \
    -lGLESv2 -lEGL -lpthread -lm -ldl \
    -static-libgcc -static-libstdc++ \
    -Wl,-rpath,'$ORIGIN:$ORIGIN/libs' \
    -o "/tmp/Nazarene.aarch64"

echo "Stripping debug symbols..."
aarch64-linux-gnu-strip --strip-all "/tmp/Nazarene.aarch64"
cp -f "/tmp/Nazarene.aarch64" "$DIST_DIR/Nazarene.aarch64"
chmod +x "$DIST_DIR/Nazarene.aarch64"

echo "========================================================================"
echo " SUCCESS: Nazarene.aarch64 built successfully!"
ls -lh "$DIST_DIR/Nazarene.aarch64"
echo "========================================================================"

