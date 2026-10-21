#!/bin/sh
# ==============================================================================
# The Nazarene: Way of the Cross - Anbernic RG-353V Native Port Launcher
# Native Rockchip RK3566 ARM64 (aarch64) - 60 FPS Capcom Arcade Action Game
# Pure POSIX /bin/sh - 100% compatible with Stock Linux OS (Buildroot/ash) & CFWs
# ==============================================================================

# 1. Dynamically locate ports directory
SCRIPT_PATH="$0"
SCRIPT_DIR="$(cd "$(dirname "$SCRIPT_PATH")" && pwd)"

if [ -d "$SCRIPT_DIR/Nazarene" ]; then
    PORT_DIR="$SCRIPT_DIR/Nazarene"
elif [ -d "/roms/ports/Nazarene" ]; then
    PORT_DIR="/roms/ports/Nazarene"
elif [ -d "/roms2/ports/Nazarene" ]; then
    PORT_DIR="/roms2/ports/Nazarene"
elif [ -d "/storage/roms/ports/Nazarene" ]; then
    PORT_DIR="/storage/roms/ports/Nazarene"
elif [ -d "/media/roms/ports/Nazarene" ]; then
    PORT_DIR="/media/roms/ports/Nazarene"
else
    PORT_DIR="$SCRIPT_DIR"
fi

cd "$PORT_DIR" || exit 1

# 2. Permissions
chmod +x ./Nazarene.aarch64 2>/dev/null

# 3. Environment & Library Paths
export LD_LIBRARY_PATH="/usr/lib:/usr/lib64:/usr/local/lib:/usr/lib/aarch64-linux-gnu:$PORT_DIR/libs:$PORT_DIR:$LD_LIBRARY_PATH"
export SDL_GAMECONTROLLERCONFIG_FILE="$PORT_DIR/gamecontrollerdb.txt"

# 4. Display driver configuration (KMSDRM / Direct Framebuffer)
if [ -z "$SDL_VIDEODRIVER" ]; then
    export SDL_VIDEODRIVER="kmsdrm"
fi

# 5. PortMaster control folder & device specifics (if present on CFWs like ArkOS/ROCKNIX/JELOS)
XDG_DATA_HOME="${XDG_DATA_HOME:-$HOME/.local/share}"
controlfolder=""
if [ -d "/opt/system/Tools/PortMaster" ]; then
    controlfolder="/opt/system/Tools/PortMaster"
elif [ -d "/opt/tools/PortMaster" ]; then
    controlfolder="/opt/tools/PortMaster"
elif [ -d "$XDG_DATA_HOME/PortMaster" ]; then
    controlfolder="$XDG_DATA_HOME/PortMaster"
elif [ -d "/roms/ports/PortMaster" ]; then
    controlfolder="/roms/ports/PortMaster"
elif [ -d "/roms2/ports/PortMaster" ]; then
    controlfolder="/roms2/ports/PortMaster"
fi

if [ -n "$controlfolder" ] && [ -f "$controlfolder/control.txt" ]; then
    . "$controlfolder/control.txt"
    [ -f "${controlfolder}/mod_${CFW_NAME}.txt" ] && . "${controlfolder}/mod_${CFW_NAME}.txt"
    if type get_controls >/dev/null 2>&1; then
        get_controls
    fi
fi

# 6. gptokeyb integration (translates handheld buttons to keyboard if gptokeyb is present)
GPTOKEYB_BIN=""
if [ -n "$GPTOKEYB" ] && [ -x "$GPTOKEYB" ]; then
    GPTOKEYB_BIN="$GPTOKEYB"
elif [ -x "$PORT_DIR/gptokeyb" ]; then
    GPTOKEYB_BIN="$PORT_DIR/gptokeyb"
elif [ -x "/usr/bin/gptokeyb" ]; then
    GPTOKEYB_BIN="/usr/bin/gptokeyb"
elif [ -x "/usr/local/bin/gptokeyb" ]; then
    GPTOKEYB_BIN="/usr/local/bin/gptokeyb"
fi

GPTOKEYB_PID=""
if [ -n "$GPTOKEYB_BIN" ] && [ -f "$PORT_DIR/Nazarene.gptk" ]; then
    "$GPTOKEYB_BIN" "Nazarene.aarch64" -c "$PORT_DIR/Nazarene.gptk" -k "Nazarene.aarch64" &
    GPTOKEYB_PID=$!
fi

# 7. Safe POSIX redirection for logging
exec > "$PORT_DIR/log.txt" 2>&1

echo "========================================================"
echo " Starting The Nazarene: Way of the Cross on RG-353V"
echo " Architecture: aarch64 (Rockchip RK3566)"
echo " Working Dir:  $PORT_DIR"
echo " Date:         $(date)"
echo " Video Driver: $SDL_VIDEODRIVER"
echo "========================================================"

# 8. Execute Native ARM64 Game Engine
./Nazarene.aarch64

# 9. Clean Shutdown & Return to EmulationStation
if [ -n "$GPTOKEYB_PID" ]; then
    kill -9 "$GPTOKEYB_PID" 2>/dev/null
fi
killall -9 gptokeyb 2>/dev/null

if type pm_finish >/dev/null 2>&1; then
    pm_finish
fi

unset LD_LIBRARY_PATH
exit 0
