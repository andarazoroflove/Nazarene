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
    BOSS_EVIL_CLONE,
    // Mission 3:
    MONEY_CHANGER,
    LEVITE_ENFORCER,
    BOSS_BARABBAS,
    BOSS_CAIAPHAS,
    BOSS_JUDAS,
    // Mission 4:
    SIGNIFER_GUARD,
    PRAETORIAN_GUARD,
    IMPERIAL_LICTOR,
    BOSS_PREFECT_QUINTUS,
    BOSS_ADVOCATE_TERTULLUS,
    BOSS_PONTIUS_PILATE
};

enum class PilateState {
    IDLE,
    APPROACH,
    BASIN_SPLASH,
    STYLUS_BEAM,
    TELEPORT_MIRAGE,
    SWORD_SLASH,
    RECOVERY,
    KNEEL_REMORSE,
    WASH_HANDS_CELESTIAL,
    DEFEATED_REDEEMED
};

enum class JudasState {
    IDLE,
    APPROACH,
    TOSS_SILVER,
    SHADOW_DASH,
    DAGGER_STAB,
    MONEY_SLAM,
    RECOVERY,
    KNEEL,
    WITHERING,
    DEAD_WITHERED
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

struct SilverProjectile {
    Vector2 position;
    Vector2 velocity;
    bool active;
    float timer;
    int bouncesLeft;
};

struct BloodShockwave {
    Vector2 position;
    float velocityX;
    bool active;
    float timer;
};

struct CaiaphasLightning {
    Vector2 position;
    float timer;
    bool active;
};

struct WaterDroplet {
    Vector2 position;
    Vector2 velocity;
    bool active;
    float timer;
};

struct StylusBlade {
    Vector2 position;
    float velocityX;
    bool active;
    float timer;
};

struct ShoutWave {
    Vector2 position;
    float velocityX;
    bool active;
    float timer;
    float scale;
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
               m_type == EnemyType::BOSS_EVIL_CLONE ||
               m_type == EnemyType::BOSS_BARABBAS ||
               m_type == EnemyType::BOSS_CAIAPHAS ||
               m_type == EnemyType::BOSS_JUDAS ||
               m_type == EnemyType::BOSS_PREFECT_QUINTUS ||
               m_type == EnemyType::BOSS_ADVOCATE_TERTULLUS ||
               m_type == EnemyType::BOSS_PONTIUS_PILATE;
    }
    bool IsDefeated() const {
        if (IsBoss()) {
            if (m_type == EnemyType::BOSS_PONTIUS_PILATE) {
                return m_hp <= 0 && (m_pilateState == PilateState::DEFEATED_REDEEMED || m_pilateState == PilateState::KNEEL_REMORSE || m_pilateState == PilateState::WASH_HANDS_CELESTIAL);
            }
            if (m_type == EnemyType::BOSS_EVIL_CLONE) {
                return m_hp <= 0 && (m_cloneState == CloneState::DEAD_WITHERED || m_cloneState == CloneState::KNEEL);
            }
            if (m_type == EnemyType::BOSS_JUDAS) {
                return m_hp <= 0 && (m_judasState == JudasState::DEAD_WITHERED || m_judasState == JudasState::KNEEL);
            }
            return m_hp <= 0 && (m_whipState == BossWhipState::DEAD_WITHERED || m_whipState == BossWhipState::KNEEL || m_whipState == BossWhipState::WALK_OFF || m_whipState == BossWhipState::EXITED);
        }
        return m_hp <= 0;
    }
    bool IsInDefeatSequence() const {
        if (!IsBoss()) return false;
        if (m_type == EnemyType::BOSS_PONTIUS_PILATE) {
            return m_hp <= 0 || m_pilateState == PilateState::KNEEL_REMORSE || m_pilateState == PilateState::WASH_HANDS_CELESTIAL || m_pilateState == PilateState::DEFEATED_REDEEMED;
        }
        if (m_type == EnemyType::BOSS_EVIL_CLONE) {
            return m_hp <= 0 || m_cloneState == CloneState::KNEEL || m_cloneState == CloneState::WITHERING || m_cloneState == CloneState::DEAD_WITHERED;
        }
        if (m_type == EnemyType::BOSS_JUDAS) {
            return m_hp <= 0 || m_judasState == JudasState::KNEEL || m_judasState == JudasState::WITHERING || m_judasState == JudasState::DEAD_WITHERED;
        }
        return m_hp <= 0 || m_whipState == BossWhipState::KNEEL || m_whipState == BossWhipState::WITHERING || m_whipState == BossWhipState::DEAD_WITHERED;
    }
    bool IsWithering() const {
        if (!IsBoss()) return false;
        if (m_type == EnemyType::BOSS_PONTIUS_PILATE) {
            return m_pilateState == PilateState::KNEEL_REMORSE || m_pilateState == PilateState::WASH_HANDS_CELESTIAL || m_pilateState == PilateState::DEFEATED_REDEEMED;
        }
        if (m_type == EnemyType::BOSS_EVIL_CLONE) {
            return m_cloneState == CloneState::KNEEL || m_cloneState == CloneState::WITHERING || m_cloneState == CloneState::DEAD_WITHERED;
        }
        if (m_type == EnemyType::BOSS_JUDAS) {
            return m_judasState == JudasState::KNEEL || m_judasState == JudasState::WITHERING || m_judasState == JudasState::DEAD_WITHERED;
        }
        return m_whipState == BossWhipState::KNEEL || m_whipState == BossWhipState::WITHERING || m_whipState == BossWhipState::DEAD_WITHERED;
    }
    bool IsWitheredDead() const {
        if (!IsBoss()) return false;
        if (m_type == EnemyType::BOSS_PONTIUS_PILATE) {
            return m_pilateState == PilateState::DEFEATED_REDEEMED;
        }
        if (m_type == EnemyType::BOSS_EVIL_CLONE) {
            return m_cloneState == CloneState::DEAD_WITHERED;
        }
        if (m_type == EnemyType::BOSS_JUDAS) {
            return m_judasState == JudasState::DEAD_WITHERED;
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
    JudasState GetJudasState() const { return m_judasState; }
    PilateState GetPilateState() const { return m_pilateState; }
    bool IsPilateMirageActive() const { return m_mirageTimer > 0.0f; }
    Vector2 GetPilateMirage1() const { return m_pilateMirage1; }
    Vector2 GetPilateMirage2() const { return m_pilateMirage2; }

    void StartArenaCombat() {
        m_bossPhase = BossPhase::PHASE_2_ARENA;
        m_whipState = BossWhipState::IDLE;
        m_cloneState = CloneState::IDLE;
        m_judasState = JudasState::IDLE;
        m_pilateState = PilateState::IDLE;
        m_whipTimer = 0.0f;
        m_stateTimer = 0.0f;
        m_pilateTimer = 0.0f;
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

    bool CheckAndClearJustThrewSilver(Vector2& outPos, float& outDir) {
        if (m_justThrewSilver) {
            m_justThrewSilver = false;
            outPos = m_silverTossPos;
            outDir = m_silverTossDir;
            return true;
        }
        return false;
    }

    bool CheckAndClearJustSpawnedShockwave(Vector2& outPos, float& outDir) {
        if (m_justSpawnedShockwave) {
            m_justSpawnedShockwave = false;
            outPos = m_shockwavePos;
            outDir = m_shockwaveDir;
            return true;
        }
        return false;
    }

    bool CheckAndClearJustCastLightning(Vector2& outPos) {
        if (m_justCastLightning) {
            m_justCastLightning = false;
            outPos = m_lightningPos;
            return true;
        }
        return false;
    }

    bool CheckAndClearJustSplashedWater(Vector2& outPos, float& outDir) {
        if (m_justSplashedWater) {
            m_justSplashedWater = false;
            outPos = m_waterSplashPos;
            outDir = m_waterSplashDir;
            return true;
        }
        return false;
    }

    bool CheckAndClearJustFiredStylus(Vector2& outPos, float& outDir) {
        if (m_justFiredStylus) {
            m_justFiredStylus = false;
            outPos = m_stylusPos;
            outDir = m_stylusDir;
            return true;
        }
        return false;
    }

    bool CheckAndClearJustShouted(Vector2& outPos, float& outDir) {
        if (m_justShouted) {
            m_justShouted = false;
            outPos = m_shoutPos;
            outDir = m_shoutDir;
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

    // Mission 3: Money Changer specific
    float m_moneyChangerTimer = 0.0f;
    bool m_moneyChangerAttacking = false;

    // Mission 3: Levite Enforcer specific
    float m_enforcerTimer = 0.0f;
    bool m_enforcerAttacking = false;

    // Mission 3: Judas Iscariot specific
    JudasState m_judasState = JudasState::IDLE;
    float m_judasTimer = 0.0f;
    int m_judasAttackPattern = 0;
    bool m_justThrewSilver = false;
    Vector2 m_silverTossPos = { 0, 0 };
    float m_silverTossDir = 0.0f;
    bool m_justSpawnedShockwave = false;
    Vector2 m_shockwavePos = { 0, 0 };
    float m_shockwaveDir = 0.0f;

    // Mission 3: Caiaphas specific
    bool m_justCastLightning = false;
    Vector2 m_lightningPos = { 0, 0 };
    float m_caiaphasCastTimer = 0.0f;

    // Mission 4: Pontius Pilate specific
    PilateState m_pilateState = PilateState::IDLE;
    float m_pilateTimer = 0.0f;
    int m_pilateAttackPattern = 0;
    Vector2 m_pilateMirage1 = { 0, 0 };
    Vector2 m_pilateMirage2 = { 0, 0 };
    float m_mirageTimer = 0.0f;
    bool m_justSplashedWater = false;
    Vector2 m_waterSplashPos = { 0, 0 };
    float m_waterSplashDir = 0.0f;
    bool m_justFiredStylus = false;
    Vector2 m_stylusPos = { 0, 0 };
    float m_stylusDir = 0.0f;

    // Mission 4: Advocate Tertullus specific
    bool m_justShouted = false;
    Vector2 m_shoutPos = { 0, 0 };
    float m_shoutDir = 0.0f;
    float m_tertullusTimer = 0.0f;

    // Mission 4: Prefect Quintus specific
    float m_prefectRushTimer = 0.0f;
    bool m_prefectRushing = false;

    // Mission 4: Regular enemies specific
    float m_lictorSwingTimer = 0.0f;
    bool m_lictorSwinging = false;
    float m_signiferRallyTimer = 0.0f;

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
    const std::vector<SilverProjectile>& GetSilverCoins() const { return m_silverCoins; }
    const std::vector<BloodShockwave>& GetBloodShockwaves() const { return m_bloodShockwaves; }
    const std::vector<CaiaphasLightning>& GetLightningStrikes() const { return m_lightningStrikes; }
    const std::vector<WaterDroplet>& GetWaterDroplets() const { return m_waterDroplets; }
    const std::vector<StylusBlade>& GetStylusBlades() const { return m_stylusBlades; }
    const std::vector<ShoutWave>& GetShoutWaves() const { return m_shoutWaves; }

private:
    std::vector<Enemy> m_enemies;
    std::vector<EnemyArrow> m_arrows;
    std::vector<EnemyDagger> m_daggers;
    std::vector<DarkProjectile> m_darkProjectiles;
    std::vector<SilverProjectile> m_silverCoins;
    std::vector<BloodShockwave> m_bloodShockwaves;
    std::vector<CaiaphasLightning> m_lightningStrikes;
    std::vector<WaterDroplet> m_waterDroplets;
    std::vector<StylusBlade> m_stylusBlades;
    std::vector<ShoutWave> m_shoutWaves;
    std::vector<DeathParticle> m_particles;

    void SpawnExplosion(Vector2 pos, Color mainColor);
    void SpawnSpark(Vector2 pos, Color sparkColor);
};
