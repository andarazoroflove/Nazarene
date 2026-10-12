#pragma once

#include "raylib.h"

struct InputState {
    bool left = false;
    bool right = false;
    bool up = false;
    bool down = false;
    
    bool jumpPressed = false;
    bool jumpDown = false;
    bool attackPressed = false;
    bool attackDown = false;
    bool wrathPressed = false;
    
    bool coinPressed = false;
    bool startPressed = false;
    bool toggleFullscreen = false;
    bool restartPressed = false;
};

class InputHandler {
public:
    static void Update(InputState& outState);
    static bool IsGamepadActive();
    static const char* GetActiveControllerName();
};

