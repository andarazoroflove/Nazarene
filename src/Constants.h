#pragma once

#include "raylib.h"

// Screen and Display Constants
constexpr int VIRTUAL_WIDTH = 320;
constexpr int VIRTUAL_HEIGHT = 224;
constexpr int WINDOW_WIDTH = 1280;
constexpr int WINDOW_HEIGHT = 896;
constexpr int TARGET_FPS = 60;

// Tile and Level Metrics
constexpr int TILE_SIZE = 16;
constexpr int LEVEL_TILES_X = 160;  // 2560 pixels wide (~8 screens)
constexpr int LEVEL_TILES_Y = 14;   // 224 pixels high (exact match to screen)
constexpr int LEVEL_PIXEL_WIDTH = LEVEL_TILES_X * TILE_SIZE;
constexpr int LEVEL_PIXEL_HEIGHT = LEVEL_TILES_Y * TILE_SIZE;

// Platform Tiers (Y coordinates)
constexpr float GROUND_Y = 192.0f;          // Lower street level (paved Roman road / Temple floor)
constexpr float UPPER_TIER_Y = 112.0f;      // Elevated tier (balconies, aqueduct, roofs)
constexpr float INTERMEDIATE_TIER_Y = 152.0f; // Mid-level crates and awnings

// Board #2 Multi-Tier Temple Platform Heights (Two levels to jump up from!)
constexpr float TEMPLE_TIER_1_Y = 144.0f;   // Level 1: Temple portico / mezzanine (Y = 9 tiles)
constexpr float TEMPLE_TIER_2_Y = 96.0f;    // Level 2: High temple gallery / parapet (Y = 6 tiles)

// Arena Bounds (Boss area at far right)
constexpr float ARENA_GATE_X = 2120.0f;
constexpr float ARENA_MIN_X = 2140.0f;
constexpr float ARENA_MAX_X = 2540.0f;

// Physics Constants
constexpr float GRAVITY = 950.0f;
constexpr float MAX_FALL_SPEED = 380.0f;
constexpr float PLAYER_WALK_SPEED = 105.0f;
constexpr float PLAYER_NORMAL_JUMP_SPEED = -280.0f;
constexpr float PLAYER_HIGH_JUMP_SPEED = -430.0f; // Somersault high jump to reach upper tier

// Combat Constants
constexpr int PLAYER_MAX_HP = 3;
constexpr int PLAYER_INITIAL_LIVES = 3;
constexpr float MELEE_TRIGGER_DISTANCE = 24.0f; // Within 24 pixels, melee triggers instead of ranged
constexpr float NAIL_SPEED = 340.0f;
constexpr float ARROW_SPEED = 190.0f;
constexpr int TOTAL_BASKETS = 4;
constexpr int BOSS_MAX_HP = 10;

// Refined 16-Bit Sega Arcade Palette
inline const Color COLOR_ROMAN_STONE      = Color{ 198, 185, 160, 255 };
inline const Color COLOR_ROMAN_DARK       = Color{  75,  65,  55, 255 };
inline const Color COLOR_ROMAN_CRIMSON    = Color{ 195,  28,  38, 255 };
inline const Color COLOR_CRIMSON_BRIGHT   = Color{ 245,  65,  75, 255 };
inline const Color COLOR_CRIMSON_DARK     = Color{ 115,  15,  22, 255 };
inline const Color COLOR_ROMAN_GOLD       = Color{ 238, 195,  45, 255 };
inline const Color COLOR_GOLD_BRIGHT      = Color{ 255, 235, 110, 255 };
inline const Color COLOR_GOLD_DARK        = Color{ 175, 125,  20, 255 };
inline const Color COLOR_ROBE_WHITE       = Color{ 248, 248, 244, 255 };
inline const Color COLOR_ROBE_SHADOW      = Color{ 185, 192, 202, 255 };
inline const Color COLOR_ROBE_DEEP        = Color{ 135, 142, 155, 255 };
inline const Color COLOR_SKIN             = Color{ 230, 178, 130, 255 };
inline const Color COLOR_SKIN_HIGHLIGHT   = Color{ 250, 210, 175, 255 };
inline const Color COLOR_SKIN_DARK        = Color{ 180, 125,  85, 255 };
inline const Color COLOR_BRONZE           = Color{ 195, 140,  60, 255 };
inline const Color COLOR_BRONZE_DARK      = Color{ 120,  80,  30, 255 };
inline const Color COLOR_IRON             = Color{ 145, 155, 168, 255 };
inline const Color COLOR_STEEL_BRIGHT     = Color{ 215, 225, 238, 255 };
inline const Color COLOR_HUD_BG           = Color{  18,  16,  24, 230 };

// Temple Sanctuary Colors
inline const Color COLOR_TEMPLE_MARBLE    = Color{ 246, 244, 238, 255 };
inline const Color COLOR_TEMPLE_SHADOW    = Color{ 175, 170, 162, 255 };
inline const Color COLOR_TEMPLE_GOLD      = Color{ 255, 215,  55, 255 };
inline const Color COLOR_TEMPLE_PURPLE    = Color{  95,  25, 115, 255 };
inline const Color COLOR_TEMPLE_CEDAR     = Color{ 120,  75,  40, 255 };
inline const Color COLOR_SICARII_DARK     = Color{  28,  32,  48, 255 };
inline const Color COLOR_SICARII_BLUE     = Color{  48,  65,  98, 255 };
inline const Color COLOR_SICARII_BLADE    = Color{ 230, 240, 255, 255 };

