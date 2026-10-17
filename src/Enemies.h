#pragma once

#include "raylib.h"
#include "Constants.h"
#include "Graphics.h"
#include "Audio.h"
#include "Level.h"
#include <vector>

enum class EnemyType {
    PATROL_GUARD,
    ARCHER,
    HEAVY_LEGIONARY,
    BOSS_GLADIATOR,
    SICARII_ASSASSIN,
    BOSS_TEMPLE_COMMANDER,
    BOSS_TRIBUNAL_PRAETOR,
    // Mission 2:
    TORMENTOR_GUARD,
    GARDEN_THORN_SICARII,
    CORRUPTED_PRIEST,
    BOSS_EVIL_CLONE
};

enum class ThornSicariiState {
    CROUCH,
    LEAP,
    SLASH,
    HURT
};

enum class CloneState {
    IDLE,
    APPROACH,
    THROW_STAND,
    THROW_CROUCH,
    JUMP_LEAP,
    MELEE_STRIKE,
    CHANNEL_WRATH,
    RECOVERY,
    KNEEL,
    WITHERING,
    DEAD_WITHERED
};

enum class SicariiState {
    WALL_CLING,
    WALL_CRAWL,
    HOP,
    STAND,
    SLASH,
    THROW,
    HURT
};

enum class BossPhase {
    PHASE_1_GATE,
    PHASE_2_ARENA
};

enum class BossWhipState {
    IDLE,
    WINDUP,
    HORIZONTAL_CRACK,
    OVERHEAD_SLAM,
    RECOVERY,
    DEFEATED,
    WALK_OFF,
    EXITED,
    // Phase 1 specific
    PHASE1_STANDBY,
    PHASE1_WHIP_1_WINDUP,
    PHASE1_WHIP_1_CRACK,
    PHASE1_PAUSE,
    PHASE1_WHIP_2_WINDUP,
    PHASE1_WHIP_2_CRACK,
    PHASE1_WALK_OFF,
    // Withering finale
    KNEEL,
    WITHERING,
    DEAD_WITHERED
};

struct DeathParticle {
    Vector2 position;
    Vector2 velocity;
    Color color;
    float life;
    float maxLife;
};

struct EnemyArrow {
    Vector2 position;
    float velocityX;
    bool active;
};

struct EnemyDagger {
    Vector2 position;
    float velocityX;
    bool active;
    float spinTimer;
};

struct DarkProjectile {
    Vector2 position;
    float velocityX;
    float velocityY;
    bool isSpike; // false = dark orb, true = dark floor spike
    bool active;
    float timer;
};

class Enemy {
public:
    Enemy(EnemyType type, Vector2 pos, float patrolMinX, float patrolMaxX);
    virtual ~Enemy() = default;

    virtual void Update(float dt, const Vector2& playerPos, Level& level);
    virtual void Draw();

    // Damage interface
    // Returns true if hit was absorbed/blocked by shield/armor
    // Returns false if hit succeeded in damaging or defeating
    virtual bool OnHit(int damage, bool hitFromBehind, bool isHighHit);

    bool IsActive() const { return m_active; }
    bool IsBoss() const {
        return m_type == EnemyType::BOSS_GLADIATOR ||
               m_type == EnemyType::BOSS_TEMPLE_COMMANDER ||
               m_type == EnemyType::BOSS_TRIBUNAL_PRAETOR ||
               m_type == EnemyType::BOSS_EVIL_CLONE;
    }
    bool IsDefeated() const {
        if (IsBoss()) {
            if (m_type == EnemyType::BOSS_EVIL_CLONE) {
                return m_hp <= 0 && (m_cloneState == CloneState::DEAD_WITHERED || m_cloneState == CloneState::KNEEL);
            }
            return m_hp <= 0 && (m_whipState == BossWhipState::DEAD_WITHERED || m_whipState == BossWhipState::KNEEL || m_whipState == BossWhipState::WALK_OFF || m_whipState == BossWhipState::EXITED);
        }
        return m_hp <= 0;
    }
    bool IsInDefeatSequence() const {
        if (!IsBoss()) return false;
        if (m_type == EnemyType::BOSS_EVIL_CLONE) {
            return m_hp <= 0 || m_cloneState == CloneState::KNEEL || m_cloneState == CloneState::WITHERING || m_cloneState == CloneState::DEAD_WITHERED;
        }
        return m_hp <= 0 || m_whipState == BossWhipState::KNEEL || m_whipState == BossWhipState::WITHERING || m_whipState == BossWhipState::DEAD_WITHERED;
    }
    bool IsWithering() const {
        if (!IsBoss()) return false;
        if (m_type == EnemyType::BOSS_EVIL_CLONE) {
            return m_cloneState == CloneState::KNEEL || m_cloneState == CloneState::WITHERING || m_cloneState == CloneState::DEAD_WITHERED;
        }
        return m_whipState == BossWhipState::KNEEL || m_whipState == BossWhipState::WITHERING || m_whipState == BossWhipState::DEAD_WITHERED;
    }
    bool IsWitheredDead() const {
        if (!IsBoss()) return false;
        if (m_type == EnemyType::BOSS_EVIL_CLONE) {
            return m_cloneState == CloneState::DEAD_WITHERED;
        }
        return m_whipState == BossWhipState::DEAD_WITHERED;
    }
    EnemyType GetType() const { return m_type; }
    Vector2 GetPosition() const { return m_position; }
    void SetPosition(Vector2 pos) { m_position = pos; m_velocity = { 0, 0 }; }
    void SetFacingLeft(bool facingLeft) { m_facingLeft = facingLeft; }
    Rectangle GetHurtbox() const;
    Rectangle GetAttackHitbox() const;
    bool HasActiveAttackHitbox() const { return m_attackHitboxActive; }

    int GetHp() const { return m_hp; }
    int GetMaxHp() const { return m_maxHp; }
    bool IsFacingLeft() const { return m_facingLeft; }
    bool HasBossWalkedOff() const { return m_bossWalkedOff; }
    bool HasPhase1WalkedOff() const { return m_phase1WalkedOff; }
    bool IsBossWalkingOff() const { return IsBoss() && (m_whipState == BossWhipState::WALK_OFF || m_whipState == BossWhipState::EXITED); }

    BossPhase GetBossPhase() const { return m_bossPhase; }
    void SetBossPhase(BossPhase phase) { m_bossPhase = phase; }
    BossWhipState GetBossWhipState() const { return m_whipState; }
    CloneState GetCloneState() const { return m_cloneState; }
    void StartArenaCombat() {
        m_bossPhase = BossPhase::PHASE_2_ARENA;
        m_whipState = BossWhipState::IDLE;
        m_cloneState = CloneState::IDLE;
        m_whipTimer = 0.0f;
        m_stateTimer = 0.0f;
        m_attackHitboxActive = false;
    }

    bool CheckAndClearJustFired(Vector2& outPos, float& outVelX) {
        if (m_justFired) {
            m_justFired = false;
            outPos = m_firedPos;
            outVelX = m_firedVelX;
            return true;
        }
        return false;
    }

    bool CheckAndClearJustThrewDagger(Vector2& outPos, float& outVelX) {
        if (m_justThrewDagger) {
            m_justThrewDagger = false;
            outPos = m_daggerPos;
            outVelX = m_daggerVelX;
            return true;
        }
        return false;
    }

    bool CheckAndClearJustFiredDark(Vector2& outPos, float& outVelX, float& outVelY, bool& outIsSpike) {
        if (m_justFiredDark) {
            m_justFiredDark = false;
            outPos = m_darkProjPos;
            outVelX = m_darkProjVelX;
            outVelY = m_darkProjVelY;
            outIsSpike = m_darkProjIsSpike;
            return true;
        }
        return false;
    }

protected:
    EnemyType m_type;
    Vector2 m_position;
    Vector2 m_velocity;
    float m_patrolMinX;
    float m_patrolMaxX;
    bool m_facingLeft;
    bool m_active;
    int m_hp;
    int m_maxHp;
    float m_stateTimer;
    float m_animTimer;
    int m_animFrame;
    float m_hurtTimer;
    bool m_attackHitboxActive;
    Rectangle m_attackHitbox;

    // Archer specific
    bool m_isDucking;
    bool m_isFiring;
    float m_arrowCooldown;
    bool m_justFired = false;
    Vector2 m_firedPos = { 0, 0 };
    float m_firedVelX = 0.0f;

    // Sicarii specific
    SicariiState m_sicariiState;
    float m_sicariiTimer;
    float m_sicariiThrowCooldown;
    float m_sicariiHopCooldown;
    bool m_justThrewDagger = false;
    Vector2 m_daggerPos = { 0, 0 };
    float m_daggerVelX = 0.0f;

    // Thorn Sicarii specific
    ThornSicariiState m_thornState;
    float m_thornTimer;
    float m_thornCooldown;

    // Tormentor Guard specific
    float m_torchTimer;
    bool m_torchSwinging;

    // Corrupted Priest specific
    float m_castTimer;
    float m_castCooldown;

    // Evil Clone specific
    CloneState m_cloneState;
    float m_cloneActionTimer;
    int m_cloneAttackPattern;
    bool m_cloneBarrierActive;
    float m_cloneLeapStartY;

    // Dark projectile emitter
    bool m_justFiredDark = false;
    Vector2 m_darkProjPos = { 0, 0 };
    float m_darkProjVelX = 0.0f;
    float m_darkProjVelY = 0.0f;
    bool m_darkProjIsSpike = false;

    // Heavy specific
    bool m_isShieldUp;

    // Boss specific
    BossPhase m_bossPhase;
    BossWhipState m_whipState;
    float m_whipTimer;
    float m_witherTimer;
    Rectangle m_whipHitbox;
    int m_whipSwingsCount;
    bool m_bossWalkedOff;
    bool m_phase1WalkedOff;
};

class EnemyManager {
public:
    EnemyManager();
    void Reset(int stageId = 1);
    void SpawnStageEnemies(int stageId = 1);
    void Update(float dt, const Vector2& playerPos, Level& level, bool screenFrozen, bool triggerWrathWipe);
    void Draw();

    // Collision & Combat with Player
    void CheckPlayerAttacks(class Player& player);
    void CheckPlayerDamage(class Player& player, bool inBossFight = false);

    // Queries
    float GetClosestEnemyDistance(const Vector2& playerPos, bool playerFacingLeft, bool& outInFront) const;
    bool IsBossActive() const;
    Enemy* GetBoss() const;
    const std::vector<EnemyArrow>& GetArrows() const { return m_arrows; }
    const std::vector<EnemyDagger>& GetDaggers() const { return m_daggers; }
    const std::vector<DarkProjectile>& GetDarkProjectiles() const { return m_darkProjectiles; }

private:
    std::vector<Enemy> m_enemies;
    std::vector<EnemyArrow> m_arrows;
    std::vector<EnemyDagger> m_daggers;
    std::vector<DarkProjectile> m_darkProjectiles;
    std::vector<DeathParticle> m_particles;

    void SpawnExplosion(Vector2 pos, Color mainColor);
    void SpawnSpark(Vector2 pos, Color sparkColor);
};
