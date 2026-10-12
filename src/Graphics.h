#pragma once

#include "raylib.h"
#include "Constants.h"
#include <string>
#include <unordered_map>

enum SpriteId {
    // Player
    SPR_PLAYER_IDLE = 0,
    SPR_PLAYER_WALK_0,
    SPR_PLAYER_WALK_1,
    SPR_PLAYER_WALK_2,
    SPR_PLAYER_WALK_3,
    SPR_PLAYER_CROUCH,
    SPR_PLAYER_JUMP,
    SPR_PLAYER_SOMERSAULT_0,
    SPR_PLAYER_SOMERSAULT_1,
    SPR_PLAYER_SOMERSAULT_2,
    SPR_PLAYER_SOMERSAULT_3,
    SPR_PLAYER_ATTACK_STAND,
    SPR_PLAYER_ATTACK_CROUCH,
    SPR_PLAYER_MELEE_STRIKE,
    SPR_PLAYER_HURT,
    SPR_PLAYER_ASCEND,
    
    // Divine Wrath
    SPR_LION_OF_JUDAH,

    // Enemies
    SPR_GUARD_WALK_0,
    SPR_GUARD_WALK_1,
    SPR_GUARD_ATTACK,
    SPR_GUARD_HURT,

    SPR_ARCHER_DUCK,
    SPR_ARCHER_STAND,
    SPR_ARCHER_FIRE,
    SPR_ARCHER_HURT,

    SPR_HEAVY_WALK_0,
    SPR_HEAVY_WALK_1,
    SPR_HEAVY_ATTACK,
    SPR_HEAVY_HURT,

    // Boss "Brutus" (Stage 1)
    SPR_BOSS_IDLE,
    SPR_BOSS_WALK_0,
    SPR_BOSS_WALK_1,
    SPR_BOSS_WHIP_PREPARE,
    SPR_BOSS_WHIP_CRACK,
    SPR_BOSS_WHIP_SLAM,
    SPR_BOSS_HURT,
    SPR_BOSS_KNEEL,
    SPR_BOSS_WITHER_0,
    SPR_BOSS_WITHER_1,

    // Stage 2: Sicarii Shadow Assassin (Shinobi-style wall-crawler & hopper)
    SPR_SICARII_WALL_CLING,
    SPR_SICARII_WALL_CRAWL_0,
    SPR_SICARII_WALL_CRAWL_1,
    SPR_SICARII_HOP,
    SPR_SICARII_STAND,
    SPR_SICARII_SLASH,
    SPR_SICARII_THROW,
    SPR_SICARII_HURT,
    SPR_SICARII_DAGGER,

    // Stage 2 Boss: Commander Malchus (Temple Guard Master)
    SPR_TEMPLE_BOSS_IDLE,
    SPR_TEMPLE_BOSS_WALK_0,
    SPR_TEMPLE_BOSS_WALK_1,
    SPR_TEMPLE_BOSS_SLASH,
    SPR_TEMPLE_BOSS_SLAM,
    SPR_TEMPLE_BOSS_HURT,
    SPR_TEMPLE_BOSS_KNEEL,

    // Projectiles & Items
    SPR_NAIL,
    SPR_ARROW,
    SPR_WHIP_HEAD,
    SPR_BASKET_LOAVES_FISHES,
    SPR_CENSER_GOLD,

    // UI / Icons
    SPR_ICON_HEART,
    SPR_ICON_WRATH,
    SPR_ICON_BASKET,

    SPRITE_TOTAL_COUNT
};

enum TileId {
    TILE_AIR = 0,
    TILE_GROUND_SURFACE,
    TILE_GROUND_SUBSURFACE,
    TILE_BALCONY_PLATFORM,
    TILE_AQUEDUCT_ARCH_L,
    TILE_AQUEDUCT_ARCH_R,
    TILE_AQUEDUCT_TOP,
    TILE_COLUMN_HEAD,
    TILE_COLUMN_MID,
    TILE_COLUMN_BASE,
    TILE_WOOD_CRATE,
    TILE_CLAY_JAR,
    TILE_GATE_FRAME,
    TILE_GATE_BARS,
    TILE_SPQR_BANNER,
    TILE_STONE_WINDOW,
    TILE_OIL_LAMP,

    // Stage 2: Temple Interior Tiles
    TILE_TEMPLE_FLOOR_CHECKER,
    TILE_TEMPLE_FLOOR_SUB,
    TILE_TEMPLE_BALCONY_1,       // Mezzanine ledge (Tier 1)
    TILE_TEMPLE_BALCONY_2,       // High balcony ledge (Tier 2)
    TILE_TEMPLE_COLUMN_HEAD,
    TILE_TEMPLE_COLUMN_MID,
    TILE_TEMPLE_COLUMN_BASE,
    TILE_TEMPLE_MENORAH,
    TILE_TEMPLE_DRAPERY_PURPLE,
    TILE_TEMPLE_ALTAR,
    TILE_TEMPLE_INCENSE_BURNER,

    TILE_TOTAL_COUNT
};

class GraphicsManager {
public:
    static void Init();
    static void Close();

    // Sprite drawing helpers
    static void DrawSprite(SpriteId id, float x, float y, bool flipH = false, Color tint = WHITE);
    static void DrawTile(TileId id, float x, float y, Color tint = WHITE);
    
    // Parallax background drawing (Supports Stage 1 Streets & Stage 2 Temple)
    static void DrawParallaxBackground(float cameraX, float time = 0.0f, int stageId = 1);
    static void DrawDropShadow(float x, float y, float width = 20.0f, float height = 5.0f);

    // Title & Mission Map screen drawing
    static void DrawTitleSplash(int destWidth, int destHeight);
    static void DrawMissionMap(int currentMission, bool m1Cleared, bool m2Cleared, bool m3Cleared, float time);

    // Particle / Effect drawing helpers
    static void DrawDivineBeam(float targetX, float alpha);
    static void DrawDivineLionAura(float x, float y, float timer);
    static void DrawMeleeSlashEffect(float x, float y, bool flipH, float progress);
    static void DrawSpark(float x, float y, float radius, Color color);

    // Cinematic & Finale effects
    static void DrawStormClouds(float cloudCoverage, float time, bool lightningFlash);
    static void DrawDivineAscensionBeam(float jesusX, float beamIntensity, float cloudsPartAmount, float time);
    static void DrawFinishHimBanner(float time, float alpha);

    // External asset hook
    static bool TryLoadExternalSpritesheet(const char* filepath);

    static Texture2D GetSpriteTexture(SpriteId id) { return s_sprites[id]; }

private:
    static Texture2D s_sprites[SPRITE_TOTAL_COUNT];
    static Texture2D s_tiles[TILE_TOTAL_COUNT];
    static Texture2D s_bgSkyDistant;
    static Texture2D s_bgTempleOfDavid;
    static Texture2D s_bgMidJerusalemVillas;
    static Texture2D s_bgNearStreetscape;

    // Stage 2 Temple Parallax Layers
    static Texture2D s_bgTempleSanctuary;
    static Texture2D s_bgTempleColonnade;

    // Map screen texture
    static Texture2D s_texMissionMap;

    static Texture2D s_texTitleSplash;
    static bool s_initialized;
    static bool s_hasExternalTextures;

    static void GenerateAllProceduralSprites();
    static void GenerateAllProceduralTiles();
    static void GenerateParallaxBackgrounds();
    static void GenerateMissionMap();

    // Helper to generate textures from pixel arrays
    static Texture2D CreateTextureFromPixels(int width, int height, const Color* pixels);
};

