#pragma once

#include "raylib.h"
#include "Graphics.h"

class Mission5Graphics {
public:
    static void GenerateAll(
        Texture2D& bgSummitSky,
        Texture2D& bgSummitMount
    );

    // Cinematic & Mission 5 Visual Effects
    static void DrawRain(float time, float intensity = 1.0f);
    static void DrawRainbowBridge(float alpha, float time);
    static void DrawGodDescend(Vector2 pos, float time);
    static void DrawMercyDialogue(bool selectAgree, float time);
    static void DrawCrucifixionScene(float time, float fadeAlpha);
    static void DrawHallucinationStars(Vector2 headPos, float time, float clearProgress);
    static void DrawAscendingAngels(Vector2 jesusPos, float timer);
    static void DrawHeavenSkeeBallScene(float time, int score, float ballRollProgress, int ballsRolled, bool ballInHole, int tickets);
};

