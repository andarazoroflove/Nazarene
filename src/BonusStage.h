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
    SUMMIT_FED
};

struct BonusVillager {
    Vector2 position;
    float speed;
    int lane; // 0 = Far Upper Ridge (Y=82), 1 = Middle Terrace (Y=128), 2 = Lower Foreground (Y=174)
    VillagerType type;
    VillagerState state;
    float animTimer;
    int walkFrame;
    float fedTimer;
    float cheerHop;
};

struct BreadFishItem {
    Vector2 position;
    Vector2 velocity;
    int targetLane;
    bool isFish;
    bool active;
    float spin;
    float life;
};

struct StarburstParticle {
    Vector2 position;
    Vector2 velocity;
    Color color;
    float alpha;
    float life;
};

struct BonusScorePopup {
    Vector2 position;
    int score;
    char text[32];
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
    bool IsPerfect() const { return m_allFed && m_summitBasket == 3; }
    int GetFedCount() const { return m_fedCount; }
    int GetTotalCount() const { return m_totalVillagers; }
    float GetRemainingTime() const { return m_timer; }
    int GetSummitBaskets() const { return m_summitBasket; }

private:
    float m_timer;               // Countdown from 30.0f
    int m_aimLane;               // 0 = Top (Y=82), 1 = Mid (Y=128), 2 = Low (Y=174)
    float m_tossCooldown;
    bool m_nextItemFish;         // Alternates bread and fish
    
    int m_fedCount;
    int m_totalVillagers;
    int m_summitBasket;          // Summit reserve loaves (3 chances before basket is depleted)
    int m_combo;                 // Consecutive successful hits
    float m_comboTimer;
    bool m_allFed;
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
    std::vector<BonusScorePopup> m_popups;

    void SpawnWaveVillager(int lane, VillagerType type, float speed);
    void UpdateSpawns(float dt);
    void TossItem();
    void AddScorePopup(float x, float y, int pts, const char* label, Color col);
};
