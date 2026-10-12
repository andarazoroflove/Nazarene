#pragma once

#include "raylib.h"
#include "Constants.h"
#include "Graphics.h"
#include <vector>

struct BasketPickup {
    Vector2 position;
    bool collected;
    float floatTimer;
    int tier; // 0 = ground, 1 = upper
};

struct OneWayPlatform {
    Rectangle rect;
};

class Level {
public:
    Level();
    void Reset(int stageId = 1);
    void SetStage(int stageId) { Reset(stageId); }
    int GetStage() const { return m_stageId; }
    void Update(float dt, const Vector2& playerPos);
    void Draw(float cameraX);

    // Collision queries
    bool IsOnGround(const Rectangle& bbox, float& outGroundY) const;
    bool CheckOneWayLanding(const Rectangle& bbox, float prevY, float& outLedgeY) const;
    bool CheckCrateLanding(const Rectangle& bbox, float prevY, float& outLedgeY) const;
    bool IsDropThroughPossible(const Rectangle& bbox) const;
    bool CheckWallCollision(const Rectangle& bbox, float& adjustedX, int facingDir) const;

    // Pickups and Objective
    int GetBasketsCollectedCount() const { return m_basketsCollected; }
    int GetTotalBasketsCount() const { return TOTAL_BASKETS; }
    bool AreAllBasketsCollected() const { return m_basketsCollected >= TOTAL_BASKETS; }
    bool IsArenaUnlocked() const { return m_arenaUnlocked; }
    bool IsArenaLockedDown() const { return m_arenaLockedDown; }
    bool IsExitOpen() const { return m_exitOpen; }

    bool CheckBasketPickup(const Rectangle& playerBox);
    void UnlockArena() { m_arenaUnlocked = true; }
    void TriggerArenaLockdown();
    void OpenExit();

    // Map bounds
    float GetLevelWidth() const { return static_cast<float>(LEVEL_PIXEL_WIDTH); }
    float GetArenaGateX() const { return ARENA_GATE_X; }
    const std::vector<Rectangle>& GetCrates() const { return m_crates; }

private:
    TileId m_tilemap[LEVEL_TILES_Y][LEVEL_TILES_X];
    std::vector<OneWayPlatform> m_oneWayPlatforms;
    std::vector<Rectangle> m_crates;
    std::vector<BasketPickup> m_baskets;
    int m_basketsCollected;
    bool m_arenaUnlocked;
    bool m_arenaLockedDown;
    bool m_exitOpen;
    int m_stageId;
    float m_gateOpenOffsetY; // Visual offset for portcullis lifting

    void BuildStreetsOfJerusalem();
    void BuildInsideTheTemple();
};

