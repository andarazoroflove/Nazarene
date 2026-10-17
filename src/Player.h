#pragma once

#include "raylib.h"
#include "Constants.h"
#include "Input.h"
#include "Graphics.h"
#include "Audio.h"
#include "Level.h"
#include <vector>

enum class PlayerState {
    IDLE,
    WALK,
    CROUCH,
    JUMP,
    HIGH_JUMP,
    DROP_THROUGH,
    ATTACK_STAND,
    ATTACK_CROUCH,
    ATTACK_AIR,
    HURT,
    DIVINE_WRATH,
    DEAD,
    ASCENDING
};

struct PlayerNail {
    Vector2 position;
    float velocityX;
    bool active;
    bool crouchShot;
    float spinTimer;
};

struct MeleeHitbox {
    Rectangle rect;
    bool active;
    float timer;
};

class Player {
public:
    Player();
    void Reset(bool resetLives = true);
    void Update(float dt, const InputState& input, Level& level, float closestEnemyDistX, bool enemyInFront);
    void Draw();

    // Damage & Life
    void TakeDamage(int damage = 1, bool inBossFight = false);
    void RestoreHealth(int pips = 1);
    void AddScore(int points) { m_score += points; }
    void AddLives(int count = 1) { m_lives += count; }
    void Resurrect(bool inBossArena, float bossArenaX = ARENA_MIN_X + 24.0f);

    // State queries
    Vector2 GetPosition() const { return m_position; }
    Rectangle GetHurtbox() const;
    PlayerState GetState() const { return m_state; }
    bool IsFacingLeft() const { return m_facingLeft; }
    bool IsInvulnerable() const { return m_invulnerableTimer > 0.0f; }
    bool IsWrathActive() const { return m_state == PlayerState::DIVINE_WRATH; }
    bool HasWrathAvailable() const { return m_wrathAvailable; }
    int GetHp() const { return m_hp; }
    int GetEffectiveMaxHp() const { return PLAYER_MAX_HP - m_shatteredHearts; }
    int GetShatteredHearts() const { return m_shatteredHearts; }
    int GetLives() const { return m_lives; }
    int GetScore() const { return m_score; }
    bool IsDead() const { return m_state == PlayerState::DEAD; }
    bool IsAscending() const { return m_state == PlayerState::ASCENDING; }

    void StartAscension(bool diagonal = false, bool toRight = true);
    void UpdateAscension(float dt);
    void SetPosition(Vector2 pos) { m_position = pos; m_velocity = { 0, 0 }; }
    void SetFacingLeft(bool facingLeft) { m_facingLeft = facingLeft; }

    // Wrath sequence queries
    bool IsWrathScreenFrozen() const;
    bool ShouldTriggerWrathBoardClear() const { return m_wrathTriggeredWipe; }
    void ClearWrathBoardClearTrigger() { m_wrathTriggeredWipe = false; }
    float GetScreenFlashAlpha() const { return m_screenFlashAlpha; }

    // Projectiles & Melee
    std::vector<PlayerNail>& GetNails() { return m_nails; }
    const MeleeHitbox& GetMeleeHitbox() const { return m_meleeHitbox; }
    void ClearMeleeHitbox() { m_meleeHitbox.active = false; }

private:
    Vector2 m_position;
    Vector2 m_velocity;
    bool m_facingLeft;
    bool m_onGround;
    bool m_onOneWayPlatform;
    float m_dropThroughIgnoreTimer;
    bool m_isAscendingDiagonal;
    bool m_ascendToRight;

    PlayerState m_state;
    float m_stateTimer;
    float m_animTimer;
    int m_walkFrame;

    int m_hp;
    int m_lives;
    int m_score;
    int m_bossHitsTaken;
    int m_shatteredHearts;
    float m_invulnerableTimer;
    bool m_wrathAvailable;

    // Divine Wrath timing
    float m_wrathTimer;
    bool m_wrathTriggeredWipe;
    float m_screenFlashAlpha;

    // Attacks
    std::vector<PlayerNail> m_nails;
    MeleeHitbox m_meleeHitbox;
    bool m_isPerformingMelee;

    void FireNail(bool crouch);
    void TriggerMelee();
    void StartDivineWrath();
    void UpdateWrath(float dt);
};

