#pragma once

#include "raylib.h"
#include "Constants.h"
#include "Input.h"
#include "Graphics.h"
#include "Audio.h"
#include <vector>

enum class VillagerType {
    ELDER = 0,
    WOMAN,
    YOUTH,
    BEGGAR
};

enum class VillagerState {
    APPROACHING,
    FED,
    SURGED
};

struct BonusVillager {
    Vector2 position;
    float speed;
    int lane; // 0 = Far Upper Ridge (Y=88), 1 = Middle Terrace (Y=132), 2 = Lower Foreground (Y=176)
    VillagerType type;
    VillagerState state;
    float animTimer;
    int walkFrame;
    float fedTimer;
};

struct BreadFishItem {
    Vector2 position;
    Vector2 startPos;
    Vector2 targetPos;
    float progress; // 0.0f -> 1.0f
    float flightDuration;
    int targetLane;
    bool isFish;
    bool active;
    float spinTimer;
};

struct StarburstParticle {
    Vector2 position;
    Vector2 velocity;
    Color color;
    float alpha;
    float life;
};

class BonusStage {
public:
    BonusStage();
    void Reset();
    void Update(float dt, const InputState& input, int& playerLives, int& playerScore);
    void Draw();

    bool IsFinished() const { return m_stageOver && m_transitionDelay <= 0.0f; }
    bool IsPerfect() const { return m_allFed; }
    int GetFedCount() const { return m_fedCount; }
    int GetTotalCount() const { return m_totalVillagers; }
    float GetRemainingTime() const { return m_timer; }

private:
    float m_timer;               // Countdown from 30.0f
    int m_aimLane;               // 0 = Top, 1 = Mid, 2 = Low
    float m_aimLeadX;            // Horizontal targeting lead
    float m_tossCooldown;
    bool m_nextItemFish;         // Alternates bread and fish
    
    int m_fedCount;
    int m_totalVillagers;
    bool m_allFed;
    bool m_crowdSurged;
    bool m_stageOver;
    bool m_rewardGiven;
    float m_transitionDelay;
    float m_bannerAlpha;

    // Spawn script pacing
    float m_elapsedTime;
    int m_spawnIndex;

    std::vector<BonusVillager> m_villagers;
    std::vector<BreadFishItem> m_projectiles;
    std::vector<StarburstParticle> m_particles;

    void SpawnWaveVillager(int lane, VillagerType type, float speed);
    void UpdateSpawns(float dt);
    void TossItem();
};

