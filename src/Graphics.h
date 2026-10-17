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

    // Stage 3 Boss: Tribunal Praetor Longinus (Golgotha / Citadel Commander)
    SPR_TRIBUNAL_IDLE,
    SPR_TRIBUNAL_WALK_0,
    SPR_TRIBUNAL_WALK_1,
    SPR_TRIBUNAL_SLASH,
    SPR_TRIBUNAL_SLAM,
    SPR_TRIBUNAL_HURT,
    SPR_TRIBUNAL_KNEEL,

    // Bonus Stage: Mount of Beatitudes / Feeding the 5,000
    SPR_VILLAGER_ELDER_RUN_0,
    SPR_VILLAGER_ELDER_RUN_1,
    SPR_VILLAGER_WOMAN_RUN_0,
    SPR_VILLAGER_WOMAN_RUN_1,
    SPR_VILLAGER_YOUTH_RUN_0,
    SPR_VILLAGER_YOUTH_RUN_1,
    SPR_VILLAGER_BEGGAR_RUN_0,
    SPR_VILLAGER_BEGGAR_RUN_1,
    SPR_VILLAGER_SATISFIED,
    SPR_BREAD_LOAF,
    SPR_FISH_PROJECTILE,
    SPR_JESUS_BASKET_AIM,
    SPR_JESUS_BASKET_THROW,

    // Mission 2: Tormentor Guard (Torch & Spiked Cudgel)
    SPR_TORMENTOR_WALK_0,
    SPR_TORMENTOR_WALK_1,
    SPR_TORMENTOR_ATTACK,
    SPR_TORMENTOR_HURT,

    // Mission 2: Thorn Sicarii (Hedge Camouflaged Assassin)
    SPR_THORN_SICARII_CROUCH,
    SPR_THORN_SICARII_LEAP,
    SPR_THORN_SICARII_SLASH,
    SPR_THORN_SICARII_HURT,

    // Mission 2: Corrupted Priest (Dark Magic Caster)
    SPR_CORRUPTED_PRIEST_STAND,
    SPR_CORRUPTED_PRIEST_CAST,
    SPR_CORRUPTED_PRIEST_HURT,
    SPR_DARK_ORB,
    SPR_DARK_SPIKE,

    // Mission 2 Boss: The Evil Genius Clone ("Christus Tenebris")
    SPR_CLONE_IDLE,
    SPR_CLONE_WALK_0,
    SPR_CLONE_WALK_1,
    SPR_CLONE_JUMP,
    SPR_CLONE_THROW_STAND,
    SPR_CLONE_THROW_CROUCH,
    SPR_CLONE_MELEE_STRIKE,
    SPR_CLONE_CHANNEL,
    SPR_CLONE_HURT,
    SPR_CLONE_KNEEL,

    // Mission 2 Pickups
    SPR_SILVER_PITCHER,
    SPR_OLIVE_WREATH,
    SPR_SACRED_CHALICE,

    // Mission 3: The Old City Bazaar Enemies
    SPR_MONEY_CHANGER_WALK_0,
    SPR_MONEY_CHANGER_WALK_1,
    SPR_MONEY_CHANGER_ATTACK,
    SPR_MONEY_CHANGER_HURT,

    // Mission 3: Mount Zion / Palace of Caiaphas Enemies
    SPR_LEVITE_ENFORCER_WALK_0,
    SPR_LEVITE_ENFORCER_WALK_1,
    SPR_LEVITE_ENFORCER_ATTACK,
    SPR_LEVITE_ENFORCER_HURT,

    // Stage 7 Boss: Barabbas the Zealot
    SPR_BARABBAS_IDLE,
    SPR_BARABBAS_WALK_0,
    SPR_BARABBAS_WALK_1,
    SPR_BARABBAS_SLASH,
    SPR_BARABBAS_SLAM,
    SPR_BARABBAS_HURT,
    SPR_BARABBAS_KNEEL,

    // Stage 8 Boss: Caiaphas the High Priest
    SPR_CAIAPHAS_STAND,
    SPR_CAIAPHAS_CAST,
    SPR_CAIAPHAS_HURT,
    SPR_CAIAPHAS_KNEEL,
    SPR_CAIAPHAS_LIGHTNING,

    // Stage 9 Final Boss: Judas Iscariot (The Traitor)
    SPR_JUDAS_IDLE,
    SPR_JUDAS_WALK_0,
    SPR_JUDAS_WALK_1,
    SPR_JUDAS_TOSS_SILVER,
    SPR_JUDAS_DASH,
    SPR_JUDAS_STAB,
    SPR_JUDAS_HURT,
    SPR_JUDAS_KNEEL,

    // Mission 3 Projectiles & Pickups
    SPR_SILVER_COIN_PROJECTILE,
    SPR_BLOOD_SHOCKWAVE,
    SPR_BITTER_HERBS,
    SPR_PASSOVER_MATZOT,
    SPR_HOLY_GRAIL,

    // Mission 4: The Judgment of Pontius Pilate
    // Standard Bearer (Signifer)
    SPR_SIGNIFER_WALK_0,
    SPR_SIGNIFER_WALK_1,
    SPR_SIGNIFER_ATTACK,
    SPR_SIGNIFER_HURT,

    // Praetorian Guard (Scutum Shield & Spatha)
    SPR_PRAETORIAN_WALK_0,
    SPR_PRAETORIAN_WALK_1,
    SPR_PRAETORIAN_SHIELD_BASH,
    SPR_PRAETORIAN_HURT,

    // Imperial Lictor (Fasces Rods & Axe)
    SPR_LICTOR_WALK_0,
    SPR_LICTOR_WALK_1,
    SPR_LICTOR_FASCES_SWING,
    SPR_LICTOR_HURT,

    // Stage 10 Boss: Praetor Prefect Quintus
    SPR_PREFECT_IDLE,
    SPR_PREFECT_WALK_0,
    SPR_PREFECT_WALK_1,
    SPR_PREFECT_SLASH,
    SPR_PREFECT_SHIELD_RUSH,
    SPR_PREFECT_HURT,
    SPR_PREFECT_KNEEL,

    // Stage 11 Boss: Imperial Advocate Tertullus
    SPR_ADVOCATE_IDLE,
    SPR_ADVOCATE_CAST,
    SPR_ADVOCATE_SHOUT,
    SPR_ADVOCATE_HURT,
    SPR_ADVOCATE_KNEEL,
    SPR_SHOUT_WAVE,

    // Stage 12 Final Boss: Pontius Pilate (Prefect of Judaea)
    SPR_PILATE_IDLE,
    SPR_PILATE_WALK_0,
    SPR_PILATE_WALK_1,
    SPR_PILATE_SLASH,
    SPR_PILATE_BASIN_SPLASH,
    SPR_PILATE_STYLUS_BEAM,
    SPR_PILATE_TELEPORT,
    SPR_PILATE_HURT,
    SPR_PILATE_KNEEL_WASH,

    // Mission 4 Projectiles & Pickups
    SPR_WATER_DROPLET,
    SPR_GOLDEN_STYLUS_BLADE,
    SPR_ROMAN_SCROLL,
    SPR_CAESAR_SEAL,
    SPR_PILATE_EWER,

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

    // Stage 3: Golgotha Citadel & Crags Tiles
    TILE_GOLGOTHA_ROCK_SURFACE,
    TILE_GOLGOTHA_ROCK_SUB,
    TILE_GOLGOTHA_LEDGE_1,
    TILE_GOLGOTHA_LEDGE_2,
    TILE_GOLGOTHA_COLUMN_HEAD,
    TILE_GOLGOTHA_COLUMN_MID,
    TILE_GOLGOTHA_COLUMN_BASE,
    TILE_GOLGOTHA_BRAZIER,
    TILE_GOLGOTHA_STANDARD,
    TILE_GOLGOTHA_WALL_STONE,

    // Stage 4: Kidron Valley & Ravine Tiles
    TILE_KIDRON_ROCK_SURFACE,
    TILE_KIDRON_ROCK_SUB,
    TILE_KIDRON_BRIDGE_WOOD,
    TILE_KIDRON_LEDGE,
    TILE_KIDRON_COLUMN_HEAD,
    TILE_KIDRON_COLUMN_MID,
    TILE_KIDRON_COLUMN_BASE,
    TILE_KIDRON_TORCH,

    // Stage 5: Royal Cypress Hedge Maze Tiles
    TILE_HEDGE_TOP,
    TILE_HEDGE_MID,
    TILE_HEDGE_LEDGE_1,
    TILE_HEDGE_LEDGE_2,
    TILE_HEDGE_ARCH,
    TILE_HEDGE_URN,
    TILE_HEDGE_FOUNTAIN,

    // Stage 6: Garden of Gethsemane & Ancient Olive Tree Tiles
    TILE_GETHSEMANE_EARTH_SURFACE,
    TILE_GETHSEMANE_EARTH_SUB,
    TILE_GETHSEMANE_OLIVE_TRUNK,
    TILE_GETHSEMANE_OLIVE_BRANCH,
    TILE_GETHSEMANE_OLIVE_LEAVES,
    TILE_GETHSEMANE_LEDGE,
    TILE_GETHSEMANE_ALTAR_STONE,
    TILE_GETHSEMANE_LANTERN,

    // Stage 7: Siloam Bazaar Tiles
    TILE_BAZAAR_STONE_SURFACE,
    TILE_BAZAAR_STONE_SUB,
    TILE_BAZAAR_AWNING_RED,
    TILE_BAZAAR_AWNING_GOLD,
    TILE_BAZAAR_SPICE_SACK,
    TILE_BAZAAR_POTTERY,
    TILE_BAZAAR_LEDGE,
    TILE_BAZAAR_LANTERN,

    // Stage 8: Mount Zion & Palace of Caiaphas Tiles
    TILE_CAIAPHAS_MARBLE_SURFACE,
    TILE_CAIAPHAS_MARBLE_SUB,
    TILE_CAIAPHAS_BRAZIER,
    TILE_CAIAPHAS_BALUSTRADE,
    TILE_CAIAPHAS_COLUMN_HEAD,
    TILE_CAIAPHAS_COLUMN_MID,
    TILE_CAIAPHAS_COLUMN_BASE,
    TILE_CAIAPHAS_GATE,

    // Stage 9: The Upper Room (Cenacle / Last Supper) Tiles
    TILE_CENACLE_WOOD_FLOOR,
    TILE_CENACLE_WOOD_SUB,
    TILE_CENACLE_TABLE_CLOTH,
    TILE_CENACLE_CHALICE_ROW,
    TILE_CENACLE_BEAM_ARCH,
    TILE_CENACLE_CANDELABRA,
    TILE_CENACLE_WINDOW_MOON,
    TILE_CENACLE_STONE_WALL,

    // Stage 10: Via Dolorosa / Streets of Judgment Tiles
    TILE_DOLOROSA_STONE_SURFACE,
    TILE_DOLOROSA_STONE_SUB,
    TILE_DOLOROSA_BARRICADE,
    TILE_DOLOROSA_ARCH_ECCE,
    TILE_DOLOROSA_EAGLE_BANNER,
    TILE_DOLOROSA_TORCH_BRACKET,
    TILE_DOLOROSA_BALCONY,
    TILE_DOLOROSA_GATE,

    // Stage 11: The Gabbatha Courtroom (Lithostrotos) Tiles
    TILE_COURT_MOSAIC_SURFACE,
    TILE_COURT_MOSAIC_SUB,
    TILE_COURT_BENCH_WOOD,
    TILE_COURT_BALUSTRADE,
    TILE_COURT_COLUMN_HEAD,
    TILE_COURT_COLUMN_MID,
    TILE_COURT_COLUMN_BASE,
    TILE_COURT_EAGLE_PEDESTAL,

    // Stage 12: Praetorium Throne Room Tiles
    TILE_THRONE_MARBLE_SURFACE,
    TILE_THRONE_MARBLE_SUB,
    TILE_THRONE_VELVET_CANOPY,
    TILE_THRONE_BASIN_PEDESTAL,
    TILE_THRONE_GOLD_EAGLE,
    TILE_THRONE_PILLAR_ARCH,
    TILE_THRONE_CANDELABRA,
    TILE_THRONE_DAIS_STEP,

    TILE_TOTAL_COUNT
};

class GraphicsManager {
public:
    static void Init();
    static void Close();

    // Sprite drawing helpers
    static void DrawSprite(SpriteId id, float x, float y, bool flipH = false, Color tint = WHITE);
    static void DrawTile(TileId id, float x, float y, Color tint = WHITE);
    
    // Parallax background drawing (Supports all stages 1 through 6)
    static void DrawParallaxBackground(float cameraX, float time = 0.0f, int stageId = 1);
    static void DrawBonusStageBackground(float time);
    static void DrawDropShadow(float x, float y, float width = 20.0f, float height = 5.0f);

    // Title, Mission Map & Secret Selection screen drawing
    static void DrawTitleSplash(int destWidth, int destHeight);
    static void DrawMissionMap(int missionNumber, int currentArea, bool a1Cleared, bool a2Cleared, bool a3Cleared, float time);
    static void DrawSecretSelectScreen(int step, int missionIndex, int stageIndex, int credits, float time);

    // Authentic Sega System 16 / MAME Arcade Cabinet Bezel & In-Game HUD
    static void DrawArcadeCabinetBezel(int screenW, int screenH, int offsetX, int offsetY, int scaledW, int scaledH, int crtMode, int credits, float time);
    static void DrawArcadeHud(int score, int lives, int hp, int maxHp, int shatteredHearts, bool wrathReady, int collectedBaskets, int totalBaskets, int stageId, float stageTimeRemaining, float time);

    // Particle / Effect drawing helpers
    static void DrawDivineBeam(float targetX, float alpha);
    static void DrawDivineLionAura(float x, float y, float timer);
    static void DrawMeleeSlashEffect(float x, float y, bool flipH, float progress);
    static void DrawSpark(float x, float y, float radius, Color color);

    // Cinematic & Finale effects
    static void DrawStormClouds(float cloudCoverage, float time, bool lightningFlash);
    static void DrawDivineAscensionBeam(float jesusX, float beamIntensity, float cloudsPartAmount, float time);
    static void DrawFinishHimBanner(float time, float alpha);
    static void DrawRainbow(float alpha, float time);

    // External asset hook
    static bool TryLoadExternalSpritesheet(const char* filepath);

    static Texture2D GetSpriteTexture(SpriteId id) { return s_sprites[id]; }
    static void SetSpriteTexture(SpriteId id, Texture2D tex) { s_sprites[id] = tex; }
    static void SetTileTexture(TileId id, Texture2D tex) { s_tiles[id] = tex; }
    static Texture2D CreateTextureFromPixels(int width, int height, const Color* pixels);

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

    // Stage 3 Golgotha & Antonia Citadel Parallax Layers
    static Texture2D s_bgGolgothaSky;
    static Texture2D s_bgGolgothaCrags;

    // Mission 2 Parallax Layers
    static Texture2D s_bgKidronSky;
    static Texture2D s_bgKidronRavine;
    static Texture2D s_bgHedgeSky;
    static Texture2D s_bgHedgeLabyrinth;
    static Texture2D s_bgGethsemaneSky;
    static Texture2D s_bgGethsemaneGrove;

    // Bonus Stage Background Layers
    static Texture2D s_bgBonusSunset;
    static Texture2D s_bgBonusTerraces;

    // Mission 3 Parallax Layers
    static Texture2D s_bgBazaarSky;
    static Texture2D s_bgBazaarRoofs;
    static Texture2D s_bgCaiaphasSky;
    static Texture2D s_bgCaiaphasPalace;
    static Texture2D s_bgCenacleSky;
    static Texture2D s_bgCenacleChamber;

    // Mission 4 Parallax Layers
    static Texture2D s_bgDolorosaSky;
    static Texture2D s_bgDolorosaStreet;
    static Texture2D s_bgCourtSky;
    static Texture2D s_bgCourtBasilica;
    static Texture2D s_bgThroneSky;
    static Texture2D s_bgThronePalace;

    // Map screen texture
    static Texture2D s_texMissionMap;

    static Texture2D s_texTitleSplash;
    static bool s_initialized;
    static bool s_hasExternalTextures;

    static void GenerateAllProceduralSprites();
    static void GenerateAllProceduralTiles();
    static void GenerateParallaxBackgrounds();
    static void GenerateMissionMap();
};

