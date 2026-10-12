#include "gba_types.h"
#include "gba_graphics.h"
#include "gba_audio.h"
#include "gba_game.h"

int main(void) {
    // 1. Initialize GBA Graphics Engine (Mode 4 240x160 double-buffered)
    gba_graphics_init();

    // 2. Initialize GBA Hardware PSG Sound Engine
    gba_audio_init();

    // 3. Initialize The Nazarene Game Engine
    gba_game_init();

    uint16_t prevKeys = 0;

    // 4. Main 60 FPS Game Loop
    while (1) {
        // Read GBA keypad (Active LOW -> invert bits)
        uint16_t currentKeys = ~REG_KEYINPUT & 0x03FF;
        uint16_t keysPressed = currentKeys & ~prevKeys;
        prevKeys = currentKeys;

        // Update Game Logic
        gba_game_update(currentKeys, keysPressed);

        // Render Frame
        gba_game_draw();

        // Flip Double Buffer during VBlank
        gba_graphics_flip();
    }

    return 0;
}
