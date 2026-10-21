#include "Input.h"
#include <cmath>

void InputHandler::Update(InputState& outState) {
    // Reset edge triggers
    outState.upPressed = false;
    outState.downPressed = false;
    outState.leftPressed = false;
    outState.rightPressed = false;
    outState.jumpPressed = false;
    outState.attackPressed = false;
    outState.wrathPressed = false;
    outState.startPressed = false;
    outState.cancelPressed = false;
    outState.toggleFullscreen = false;
    outState.restartPressed = false;
    outState.secretSelectPressed = false;

    // 1. Keyboard Inputs
    bool kbLeft  = IsKeyDown(KEY_LEFT)  || IsKeyDown(KEY_A);
    bool kbRight = IsKeyDown(KEY_RIGHT) || IsKeyDown(KEY_D);
    bool kbUp    = IsKeyDown(KEY_UP)    || IsKeyDown(KEY_W);
    bool kbDown  = IsKeyDown(KEY_DOWN)  || IsKeyDown(KEY_S);
    bool kbUpPressed    = IsKeyPressed(KEY_UP)    || IsKeyPressed(KEY_W);
    bool kbDownPressed  = IsKeyPressed(KEY_DOWN)  || IsKeyPressed(KEY_S);
    bool kbLeftPressed  = IsKeyPressed(KEY_LEFT)  || IsKeyPressed(KEY_A);
    bool kbRightPressed = IsKeyPressed(KEY_RIGHT) || IsKeyPressed(KEY_D);

    bool kbJumpPressed = IsKeyPressed(KEY_SPACE);
    bool kbJumpDown    = IsKeyDown(KEY_SPACE);

    bool kbAttackPressed = IsKeyPressed(KEY_J) || IsKeyPressed(KEY_Z);
    bool kbAttackDown    = IsKeyDown(KEY_J)    || IsKeyDown(KEY_Z);

    bool kbWrathPressed = IsKeyPressed(KEY_K) || IsKeyPressed(KEY_X);

    bool kbCoinPressed      = IsKeyPressed(KEY_C) || IsKeyPressed(KEY_FIVE) || IsKeyPressed(KEY_LEFT_SHIFT) || IsKeyPressed(KEY_RIGHT_SHIFT);
    bool kbStartPressed     = IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_ONE);
    bool kbCancelPressed    = IsKeyPressed(KEY_ESCAPE) || IsKeyPressed(KEY_BACKSPACE);
    bool kbToggleFullscreen = IsKeyPressed(KEY_F11)   || ((IsKeyDown(KEY_LEFT_ALT) || IsKeyDown(KEY_RIGHT_ALT)) && IsKeyPressed(KEY_ENTER));
    bool kbRestart          = IsKeyPressed(KEY_R);
    bool kbBothTriggers     = (IsKeyDown(KEY_Q) && IsKeyDown(KEY_E)) ||
                              (IsKeyDown(KEY_LEFT_BRACKET) && IsKeyDown(KEY_RIGHT_BRACKET)) ||
                              (IsKeyDown(KEY_LEFT_CONTROL) && IsKeyDown(KEY_M)) ||
                              IsKeyPressed(KEY_F1);

    // 2. Universal Gamepad Inputs (Scan all 4 slots: 0 to 3)
    bool gpLeft = false;
    bool gpRight = false;
    bool gpUp = false;
    bool gpDown = false;
    bool gpUpPressed = false;
    bool gpDownPressed = false;
    bool gpLeftPressed = false;
    bool gpRightPressed = false;
    bool gpJumpPressed = false;
    bool gpJumpDown = false;
    bool gpAttackPressed = false;
    bool gpAttackDown = false;
    bool gpWrathPressed = false;
    bool gpCoinPressed = false;
    bool gpStartPressed = false;
    bool gpCancelPressed = false;
    bool gpBothTriggers = false;

    constexpr float DEADZONE = 0.28f; // Budget stick tolerance

    for (int pad = 0; pad < 4; ++pad) {
        if (!IsGamepadAvailable(pad)) continue;

        // D-Pad Standard Buttons
        if (IsGamepadButtonDown(pad, GAMEPAD_BUTTON_LEFT_FACE_LEFT))  gpLeft = true;
        if (IsGamepadButtonDown(pad, GAMEPAD_BUTTON_LEFT_FACE_RIGHT)) gpRight = true;
        if (IsGamepadButtonDown(pad, GAMEPAD_BUTTON_LEFT_FACE_UP))    gpUp = true;
        if (IsGamepadButtonDown(pad, GAMEPAD_BUTTON_LEFT_FACE_DOWN))  gpDown = true;
        if (IsGamepadButtonPressed(pad, GAMEPAD_BUTTON_LEFT_FACE_UP))   gpUpPressed = true;
        if (IsGamepadButtonPressed(pad, GAMEPAD_BUTTON_LEFT_FACE_DOWN)) gpDownPressed = true;

        // Left Analog Stick (Axes 0 and 1 - Standard for All Gamepads)
        float stickX = GetGamepadAxisMovement(pad, GAMEPAD_AXIS_LEFT_X);
        float stickY = GetGamepadAxisMovement(pad, GAMEPAD_AXIS_LEFT_Y);

        if (stickX < -DEADZONE) gpLeft = true;
        if (stickX > DEADZONE)  gpRight = true;
        if (stickY < -DEADZONE) gpUp = true;
        if (stickY > DEADZONE)  gpDown = true;

        // --- Action Buttons ---
        // Anbernic RG-353V / Handheld Layout:
        // A Button (Face Right) = Shoot / Attack
        // B Button (Face Down)  = Jump
        // X Button (Face Up)    = Divine Wrath
        // Y Button (Face Left)  = DOES NOTHING

        // Jump: Button B (Right Face Down on Nintendo/Anbernic)
        if (IsGamepadButtonPressed(pad, GAMEPAD_BUTTON_RIGHT_FACE_DOWN)) {
            gpJumpPressed = true;
        }
        if (IsGamepadButtonDown(pad, GAMEPAD_BUTTON_RIGHT_FACE_DOWN)) {
            gpJumpDown = true;
        }

        // Shoot / Attack: Button A (Right Face Right on Nintendo/Anbernic) OR Right Bumper / Right Trigger
        // On desktop platforms, also allow Face Left (Xbox X button)
        if (IsGamepadButtonPressed(pad, GAMEPAD_BUTTON_RIGHT_FACE_RIGHT) ||
#ifndef __linux__
            IsGamepadButtonPressed(pad, GAMEPAD_BUTTON_RIGHT_FACE_LEFT) ||
#endif
            IsGamepadButtonPressed(pad, GAMEPAD_BUTTON_RIGHT_TRIGGER_1) ||
            IsGamepadButtonPressed(pad, GAMEPAD_BUTTON_RIGHT_TRIGGER_2)) {
            gpAttackPressed = true;
        }
        if (IsGamepadButtonDown(pad, GAMEPAD_BUTTON_RIGHT_FACE_RIGHT) ||
#ifndef __linux__
            IsGamepadButtonDown(pad, GAMEPAD_BUTTON_RIGHT_FACE_LEFT) ||
#endif
            IsGamepadButtonDown(pad, GAMEPAD_BUTTON_RIGHT_TRIGGER_1) ||
            IsGamepadButtonDown(pad, GAMEPAD_BUTTON_RIGHT_TRIGGER_2)) {
            gpAttackDown = true;
        }
        if (GetGamepadAxisMovement(pad, GAMEPAD_AXIS_RIGHT_TRIGGER) > 0.4f) {
            gpAttackDown = true;
        }

        // Divine Wrath: Button X (Right Face Up on Nintendo/Anbernic) OR Left Bumper / Left Trigger
        if (IsGamepadButtonPressed(pad, GAMEPAD_BUTTON_RIGHT_FACE_UP) ||
            IsGamepadButtonPressed(pad, GAMEPAD_BUTTON_LEFT_TRIGGER_1) ||
            IsGamepadButtonPressed(pad, GAMEPAD_BUTTON_LEFT_TRIGGER_2)) {
            gpWrathPressed = true;
        }
        if (GetGamepadAxisMovement(pad, GAMEPAD_AXIS_LEFT_TRIGGER) > 0.4f) {
            gpWrathPressed = true;
        }

        // Button Y (Right Face Left on Nintendo/Anbernic): DOES NOTHING!

        if (IsGamepadButtonPressed(pad, GAMEPAD_BUTTON_LEFT_FACE_LEFT))  gpLeftPressed = true;
        if (IsGamepadButtonPressed(pad, GAMEPAD_BUTTON_LEFT_FACE_RIGHT)) gpRightPressed = true;

        // Cancel / Back: SELECT button or Menu Back
        if (IsGamepadButtonPressed(pad, GAMEPAD_BUTTON_MIDDLE_LEFT)) {
            gpCancelPressed = true;
        }

        // Add Coin: SELECT button on controller (Button 9 / Back / Share / Middle Left)
        if (IsGamepadButtonPressed(pad, GAMEPAD_BUTTON_MIDDLE_LEFT) ||
            IsGamepadButtonPressed(pad, GAMEPAD_BUTTON_MIDDLE)) {
            gpCoinPressed = true;
        }

        // Start Game: START button on controller (Button 10 / Menu / Options / Middle Right)
        if (IsGamepadButtonPressed(pad, GAMEPAD_BUTTON_MIDDLE_RIGHT)) {
            gpStartPressed = true;
        }

        // Both Triggers on controller (LT + RT or L1/L2 + R1/R2)
        bool padLeftTrigger = IsGamepadButtonDown(pad, GAMEPAD_BUTTON_LEFT_TRIGGER_1) ||
                              IsGamepadButtonDown(pad, GAMEPAD_BUTTON_LEFT_TRIGGER_2) ||
                              (GetGamepadAxisMovement(pad, GAMEPAD_AXIS_LEFT_TRIGGER) > 0.35f);

        bool padRightTrigger = IsGamepadButtonDown(pad, GAMEPAD_BUTTON_RIGHT_TRIGGER_1) ||
                               IsGamepadButtonDown(pad, GAMEPAD_BUTTON_RIGHT_TRIGGER_2) ||
                               (GetGamepadAxisMovement(pad, GAMEPAD_AXIS_RIGHT_TRIGGER) > 0.35f);

        if (padLeftTrigger && padRightTrigger) {
            gpBothTriggers = true;
        }
    }

    // 3. Combine Inputs
    outState.left  = kbLeft  || gpLeft;
    outState.right = kbRight || gpRight;
    outState.up    = kbUp    || gpUp;
    outState.down  = kbDown  || gpDown;
    outState.upPressed    = kbUpPressed    || gpUpPressed;
    outState.downPressed  = kbDownPressed  || gpDownPressed;
    outState.leftPressed  = kbLeftPressed  || gpLeftPressed;
    outState.rightPressed = kbRightPressed || gpRightPressed;

    outState.jumpPressed = kbJumpPressed || gpJumpPressed;
    outState.jumpDown    = kbJumpDown    || gpJumpDown;

    outState.attackPressed = kbAttackPressed || gpAttackPressed;
    outState.attackDown    = kbAttackDown    || gpAttackDown;

    outState.wrathPressed   = kbWrathPressed || gpWrathPressed;
    outState.coinPressed    = kbCoinPressed  || gpCoinPressed;
    outState.startPressed   = kbStartPressed || gpStartPressed;
    outState.cancelPressed  = kbCancelPressed || gpCancelPressed;
    outState.toggleFullscreen = kbToggleFullscreen;
    outState.restartPressed = kbRestart;

    // Secret Mission Select Trigger Edge Detection
    bool bothTriggersDown = gpBothTriggers || kbBothTriggers;
    static bool s_prevBothTriggers = false;
    outState.secretSelectPressed = bothTriggersDown && !s_prevBothTriggers;
    s_prevBothTriggers = bothTriggersDown;
}

bool InputHandler::IsGamepadActive() {
    for (int pad = 0; pad < 4; ++pad) {
        if (IsGamepadAvailable(pad)) return true;
    }
    return false;
}

const char* InputHandler::GetActiveControllerName() {
    for (int pad = 0; pad < 4; ++pad) {
        if (IsGamepadAvailable(pad)) {
            return GetGamepadName(pad);
        }
    }
    return "Keyboard";
}
