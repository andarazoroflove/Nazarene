#include "Enemies.h"
#include "Player.h"
#include <cmath>
#include <algorithm>

// --- Enemy Implementation ---
Enemy::Enemy(EnemyType type, Vector2 pos, float patrolMinX, float patrolMaxX)
    : m_type(type), m_position(pos), m_patrolMinX(patrolMinX), m_patrolMaxX(patrolMaxX) {
    m_velocity = { 0.0f, 0.0f };
    m_facingLeft = true;
    m_active = true;
    m_stateTimer = 0.0f;
    m_animTimer = 0.0f;
    m_animFrame = 0;
    m_hurtTimer = 0.0f;
    m_attackHitboxActive = false;
    m_attackHitbox = { 0, 0, 0, 0 };

    m_isDucking = false;
    m_isFiring = false;
    m_arrowCooldown = 1.8f;

    m_isShieldUp = true;

    m_bossPhase = BossPhase::PHASE_1_GATE;
    m_whipState = BossWhipState::PHASE1_STANDBY;
    m_whipTimer = 0.0f;
    m_witherTimer = 0.0f;
    m_whipHitbox = { 0, 0, 0, 0 };
    m_whipSwingsCount = 0;
    m_bossWalkedOff = false;
    m_phase1WalkedOff = false;

    m_thornState = ThornSicariiState::CROUCH;
    m_thornTimer = 0.0f;
    m_thornCooldown = 1.0f;
    m_torchTimer = 0.0f;
    m_torchSwinging = false;
    m_castTimer = 0.0f;
    m_castCooldown = 2.0f;
    m_cloneState = CloneState::IDLE;
    m_cloneActionTimer = 0.0f;
    m_cloneAttackPattern = 0;
    m_cloneBarrierActive = true;
    m_cloneLeapStartY = 0.0f;
    m_justFiredDark = false;
    m_darkProjPos = { 0, 0 };
    m_darkProjVelX = 0.0f;
    m_darkProjVelY = 0.0f;
    m_darkProjIsSpike = false;

    switch (m_type) {
        case EnemyType::PATROL_GUARD:
            m_maxHp = 1;
            m_hp = 1;
            m_velocity.x = -45.0f;
            break;
        case EnemyType::ARCHER:
            m_maxHp = 1;
            m_hp = 1;
            m_isDucking = true;
            m_stateTimer = 1.6f;
            break;
        case EnemyType::HEAVY_LEGIONARY:
            m_maxHp = 2;
            m_hp = 2;
            m_velocity.x = -32.0f;
            break;
        case EnemyType::BOSS_GLADIATOR:
            m_maxHp = BOSS_MAX_HP;
            m_hp = BOSS_MAX_HP;
            m_velocity.x = 0.0f;
            m_bossPhase = BossPhase::PHASE_1_GATE;
            m_whipState = BossWhipState::PHASE1_STANDBY;
            m_facingLeft = true;
            break;
        case EnemyType::SICARII_ASSASSIN:
            m_maxHp = 2;
            m_hp = 2;
            m_sicariiState = SicariiState::WALL_CLING;
            m_sicariiTimer = 0.0f;
            m_sicariiThrowCooldown = 1.2f;
            m_sicariiHopCooldown = 2.0f;
            m_velocity = { 0.0f, 0.0f };
            break;
        case EnemyType::BOSS_TEMPLE_COMMANDER:
            m_maxHp = BOSS_MAX_HP;
            m_hp = BOSS_MAX_HP;
            m_velocity.x = 0.0f;
            m_bossPhase = BossPhase::PHASE_2_ARENA;
            m_whipState = BossWhipState::IDLE;
            m_facingLeft = true;
            break;
        case EnemyType::BOSS_TRIBUNAL_PRAETOR:
            m_maxHp = BOSS_MAX_HP;
            m_hp = BOSS_MAX_HP;
            m_velocity.x = 0.0f;
            m_bossPhase = BossPhase::PHASE_2_ARENA;
            m_whipState = BossWhipState::IDLE;
            m_facingLeft = true;
            break;
        case EnemyType::TORMENTOR_GUARD:
            m_maxHp = 2;
            m_hp = 2;
            m_velocity.x = -40.0f;
            break;
        case EnemyType::GARDEN_THORN_SICARII:
            m_maxHp = 2;
            m_hp = 2;
            m_thornState = ThornSicariiState::CROUCH;
            m_velocity = { 0.0f, 0.0f };
            break;
        case EnemyType::CORRUPTED_PRIEST:
            m_maxHp = 2;
            m_hp = 2;
            m_velocity = { 0.0f, 0.0f };
            m_castCooldown = 1.8f;
            break;
        case EnemyType::BOSS_EVIL_CLONE:
            m_maxHp = BOSS_MAX_HP;
            m_hp = BOSS_MAX_HP;
            m_velocity = { 0.0f, 0.0f };
            m_bossPhase = BossPhase::PHASE_2_ARENA;
            m_cloneState = CloneState::IDLE;
            m_facingLeft = true;
            m_cloneBarrierActive = true;
            break;
        case EnemyType::MONEY_CHANGER:
            m_maxHp = 2;
            m_hp = 2;
            m_velocity.x = -38.0f;
            break;
        case EnemyType::LEVITE_ENFORCER:
            m_maxHp = 3;
            m_hp = 3;
            m_velocity.x = -35.0f;
            break;
        case EnemyType::BOSS_BARABBAS:
            m_maxHp = BOSS_MAX_HP;
            m_hp = BOSS_MAX_HP;
            m_velocity = { 0.0f, 0.0f };
            m_bossPhase = BossPhase::PHASE_2_ARENA;
            m_whipState = BossWhipState::IDLE;
            m_facingLeft = true;
            break;
        case EnemyType::BOSS_CAIAPHAS:
            m_maxHp = BOSS_MAX_HP;
            m_hp = BOSS_MAX_HP;
            m_velocity = { 0.0f, 0.0f };
            m_bossPhase = BossPhase::PHASE_2_ARENA;
            m_whipState = BossWhipState::IDLE;
            m_facingLeft = true;
            break;
        case EnemyType::BOSS_JUDAS:
            m_maxHp = BOSS_MAX_HP;
            m_hp = BOSS_MAX_HP;
            m_velocity = { 0.0f, 0.0f };
            m_bossPhase = BossPhase::PHASE_2_ARENA;
            m_judasState = JudasState::IDLE;
            m_facingLeft = true;
            break;
        case EnemyType::SIGNIFER_GUARD:
            m_maxHp = 2;
            m_hp = 2;
            m_velocity.x = -35.0f;
            break;
        case EnemyType::PRAETORIAN_GUARD:
            m_maxHp = 3;
            m_hp = 3;
            m_velocity.x = -30.0f;
            m_isShieldUp = true;
            break;
        case EnemyType::IMPERIAL_LICTOR:
            m_maxHp = 2;
            m_hp = 2;
            m_velocity.x = -40.0f;
            break;
        case EnemyType::BOSS_PREFECT_QUINTUS:
            m_maxHp = BOSS_MAX_HP;
            m_hp = BOSS_MAX_HP;
            m_velocity = { 0.0f, 0.0f };
            m_bossPhase = BossPhase::PHASE_2_ARENA;
            m_whipState = BossWhipState::IDLE;
            m_facingLeft = true;
            break;
        case EnemyType::BOSS_ADVOCATE_TERTULLUS:
            m_maxHp = BOSS_MAX_HP;
            m_hp = BOSS_MAX_HP;
            m_velocity = { 0.0f, 0.0f };
            m_bossPhase = BossPhase::PHASE_2_ARENA;
            m_whipState = BossWhipState::IDLE;
            m_facingLeft = true;
            break;
        case EnemyType::BOSS_PONTIUS_PILATE:
            m_maxHp = BOSS_MAX_HP;
            m_hp = BOSS_MAX_HP;
            m_velocity = { 0.0f, 0.0f };
            m_bossPhase = BossPhase::PHASE_2_ARENA;
            m_pilateState = PilateState::IDLE;
            m_facingLeft = true;
            break;
        case EnemyType::BOSS_MISSION5_CLONE_GREEN:
            m_maxHp = BOSS_MAX_HP;
            m_hp = BOSS_MAX_HP;
            m_velocity = { 0.0f, 0.0f };
            m_bossPhase = BossPhase::PHASE_2_ARENA;
            m_cloneState = CloneState::IDLE;
            m_cloneActionTimer = 0.0f;
            m_facingLeft = true;
            break;
        case EnemyType::BOSS_DEVIL:
            m_maxHp = 14;
            m_hp = 14;
            m_velocity = { 0.0f, 0.0f };
            m_bossPhase = BossPhase::PHASE_2_ARENA;
            m_devilState = DevilState::IDLE;
            m_devilTimer = 0.0f;
            m_facingLeft = true;
            break;
    }
}

Rectangle Enemy::GetHurtbox() const {
    switch (m_type) {
        case EnemyType::PATROL_GUARD:
            return { m_position.x + 4.0f, m_position.y + 4.0f, 16.0f, 26.0f };
        case EnemyType::ARCHER:
            if (m_isDucking) {
                return { m_position.x + 4.0f, m_position.y + 14.0f, 16.0f, 16.0f };
            }
            return { m_position.x + 4.0f, m_position.y + 4.0f, 16.0f, 26.0f };
        case EnemyType::HEAVY_LEGIONARY:
            return { m_position.x + 4.0f, m_position.y + 4.0f, 20.0f, 28.0f };
        case EnemyType::BOSS_GLADIATOR:
            return { m_position.x + 6.0f, m_position.y + 4.0f, 24.0f, 44.0f };
        case EnemyType::SICARII_ASSASSIN:
            return { m_position.x + 3.0f, m_position.y + 4.0f, 18.0f, 26.0f };
        case EnemyType::BOSS_TEMPLE_COMMANDER:
        case EnemyType::BOSS_TRIBUNAL_PRAETOR:
            return { m_position.x + 6.0f, m_position.y + 4.0f, 24.0f, 44.0f };
        case EnemyType::TORMENTOR_GUARD:
            return { m_position.x + 4.0f, m_position.y + 4.0f, 18.0f, 26.0f };
        case EnemyType::GARDEN_THORN_SICARII:
            return { m_position.x + 3.0f, m_position.y + (m_thornState == ThornSicariiState::CROUCH ? 12.0f : 4.0f), 18.0f, (m_thornState == ThornSicariiState::CROUCH ? 18.0f : 24.0f) };
        case EnemyType::CORRUPTED_PRIEST:
            return { m_position.x + 4.0f, m_position.y + 4.0f, 18.0f, 26.0f };
        case EnemyType::BOSS_EVIL_CLONE:
        case EnemyType::BOSS_MISSION5_CLONE_GREEN:
            if (m_cloneState == CloneState::THROW_CROUCH) {
                return { m_position.x + 4.0f, m_position.y + 12.0f, 20.0f, 18.0f };
            }
            return { m_position.x + 4.0f, m_position.y + 4.0f, 20.0f, 26.0f };
        case EnemyType::BOSS_DEVIL:
            return { m_position.x + 4.0f, m_position.y + 4.0f, 24.0f, 32.0f };
        case EnemyType::MONEY_CHANGER:
            return { m_position.x + 4.0f, m_position.y + 4.0f, 16.0f, 26.0f };
        case EnemyType::LEVITE_ENFORCER:
            return { m_position.x + 4.0f, m_position.y + 4.0f, 16.0f, 26.0f };
        case EnemyType::BOSS_BARABBAS:
            return { m_position.x + 6.0f, m_position.y + 4.0f, 20.0f, 30.0f };
        case EnemyType::BOSS_CAIAPHAS:
            return { m_position.x + 6.0f, m_position.y + 4.0f, 20.0f, 30.0f };
        case EnemyType::BOSS_JUDAS:
            return { m_position.x + 6.0f, m_position.y + 4.0f, 20.0f, 30.0f };
        case EnemyType::SIGNIFER_GUARD:
            return { m_position.x + 4.0f, m_position.y + 4.0f, 18.0f, 26.0f };
        case EnemyType::PRAETORIAN_GUARD:
            return { m_position.x + 4.0f, m_position.y + 4.0f, 20.0f, 28.0f };
        case EnemyType::IMPERIAL_LICTOR:
            return { m_position.x + 4.0f, m_position.y + 4.0f, 18.0f, 26.0f };
        case EnemyType::BOSS_PREFECT_QUINTUS:
            return { m_position.x + 6.0f, m_position.y + 4.0f, 22.0f, 32.0f };
        case EnemyType::BOSS_ADVOCATE_TERTULLUS:
            return { m_position.x + 6.0f, m_position.y + 4.0f, 20.0f, 30.0f };
        case EnemyType::BOSS_PONTIUS_PILATE:
            return { m_position.x + 6.0f, m_position.y + 4.0f, 22.0f, 32.0f };
    }
    return { m_position.x, m_position.y, 16.0f, 28.0f };
}

Rectangle Enemy::GetAttackHitbox() const {
    if (m_type == EnemyType::BOSS_GLADIATOR) {
        return m_whipHitbox;
    }
    return m_attackHitbox;
}

bool Enemy::OnHit(int damage, bool hitFromBehind, bool isHighHit) {
    if (m_hurtTimer > 0.0f || m_hp <= 0) return true;

    // 1. Heavy Legionary Shield Logic
    if (m_type == EnemyType::HEAVY_LEGIONARY) {
        if (m_isShieldUp && !hitFromBehind) {
            AudioManager::PlaySfx(SFX_SHIELD_DEFLECT);
            return true;
        }
    }

    // 2. Boss Armor Logic (Brutus, Commander Malchus, Tribunal Praetor, Barabbas, Caiaphas)
    if (m_type == EnemyType::BOSS_GLADIATOR || m_type == EnemyType::BOSS_TEMPLE_COMMANDER ||
        m_type == EnemyType::BOSS_TRIBUNAL_PRAETOR || m_type == EnemyType::BOSS_BARABBAS ||
        m_type == EnemyType::BOSS_CAIAPHAS) {
        bool isVulnerable = isHighHit || (damage >= 4) || hitFromBehind;
        if (!isVulnerable) {
            AudioManager::PlaySfx(SFX_SHIELD_DEFLECT);
            return true; // Armor deflected hit!
        }
    }

    // 2b. Evil Genius Clone Logic ("Christus Tenebris" & Green Clone)
    if (m_type == EnemyType::BOSS_EVIL_CLONE || m_type == EnemyType::BOSS_MISSION5_CLONE_GREEN) {
        if (m_cloneState == CloneState::KNEEL || m_cloneState == CloneState::WITHERING || m_cloneState == CloneState::DEAD_WITHERED) {
            return true;
        }

        // Vulnerable when channeling wrath, somersaulting in the air, jump headshot, or from behind/melee
        bool isVulnerable = (m_cloneState == CloneState::CHANNEL_WRATH) ||
                            (m_cloneState == CloneState::JUMP_LEAP) ||
                            isHighHit ||
                            hitFromBehind ||
                            (damage >= 4);

        if (!isVulnerable) {
            // Dark aura deflects standard standing frontal nail shots!
            AudioManager::PlaySfx(SFX_SHIELD_DEFLECT);
            return true;
        }
    }

    // 2c. Judas Iscariot Logic (The Traitor)
    if (m_type == EnemyType::BOSS_JUDAS) {
        if (m_judasState == JudasState::KNEEL || m_judasState == JudasState::WITHERING || m_judasState == JudasState::DEAD_WITHERED) {
            return true;
        }

        bool isVulnerable = (m_judasState == JudasState::TOSS_SILVER) ||
                            (m_judasState == JudasState::MONEY_SLAM) ||
                            isHighHit ||
                            hitFromBehind ||
                            (damage >= 4);

        if (!isVulnerable) {
            // Sinister greed aura deflects standard standing frontal nail shots!
            AudioManager::PlaySfx(SFX_SHIELD_DEFLECT);
            return true;
        }
    }

    // 2d. Levite Enforcer Poleaxe Defense
    if (m_type == EnemyType::LEVITE_ENFORCER && m_enforcerAttacking) {
        if (!hitFromBehind && !isHighHit) {
            AudioManager::PlaySfx(SFX_SHIELD_DEFLECT);
            return true;
        }
    }

    // 2e. Praetorian Guard Scutum Shield Defense
    if (m_type == EnemyType::PRAETORIAN_GUARD) {
        if (m_isShieldUp && !hitFromBehind) {
            AudioManager::PlaySfx(SFX_SHIELD_DEFLECT);
            return true;
        }
    }

    // 2f. Praetor Prefect Quintus and Advocate Tertullus Armor Logic
    if (m_type == EnemyType::BOSS_PREFECT_QUINTUS || m_type == EnemyType::BOSS_ADVOCATE_TERTULLUS) {
        bool isVulnerable = isHighHit || (damage >= 4) || hitFromBehind;
        if (!isVulnerable) {
            AudioManager::PlaySfx(SFX_SHIELD_DEFLECT);
            return true;
        }
    }

    // 2g. Pontius Pilate Imperial Judgment Logic
    if (m_type == EnemyType::BOSS_PONTIUS_PILATE) {
        if (m_pilateState == PilateState::KNEEL_REMORSE || m_pilateState == PilateState::WASH_HANDS_CELESTIAL || m_pilateState == PilateState::DEFEATED_REDEEMED) {
            return true;
        }

        // Striking the real Pilate breaks active teleport mirages!
        if (m_mirageTimer > 0.0f) {
            m_mirageTimer = 0.0f;
            AudioManager::PlaySfx(SFX_CLONE_TELEPORT);
        }

        bool isVulnerable = (m_pilateState == PilateState::BASIN_SPLASH) ||
                            (m_pilateState == PilateState::STYLUS_BEAM) ||
                            isHighHit ||
                            hitFromBehind ||
                            (damage >= 4);

        if (!isVulnerable) {
            AudioManager::PlaySfx(SFX_SHIELD_DEFLECT);
            return true;
        }
    }

    // 2h. The Devil (Mount Calvary Boss 2)
    if (m_type == EnemyType::BOSS_DEVIL) {
        if (m_devilState == DevilState::DEFEATED) {
            return true;
        }

        // Vulnerable during tail whip windup/strike, fire surge, jump headshot, from behind, or melee/beam
        bool isVulnerable = (m_devilState == DevilState::TAIL_WHIP_WINDUP) ||
                            (m_devilState == DevilState::TAIL_WHIP_STRIKE) ||
                            (m_devilState == DevilState::FIRE_SURGE) ||
                            (m_devilState == DevilState::TRIDENT_THRUST) ||
                            isHighHit ||
                            hitFromBehind ||
                            (damage >= 4);

        if (!isVulnerable) {
            AudioManager::PlaySfx(SFX_SHIELD_DEFLECT);
            return true;
        }
    }

    // 3. Archer Ducking Protection against standard mid-level shots
    if (m_type == EnemyType::ARCHER && m_isDucking) {
        if (!isHighHit) {
            return true;
        }
    }

    // Apply damage
    m_hp -= damage;
    if (m_hp < 0) m_hp = 0;
    m_hurtTimer = 0.28f;

    if (IsBoss()) {
        AudioManager::PlaySfx(SFX_BOSS_HURT);
        if (m_hp <= 0) {
            m_hp = 0;
            m_whipState = BossWhipState::KNEEL;
            m_cloneState = CloneState::KNEEL;
            m_judasState = JudasState::KNEEL;
            m_pilateState = PilateState::KNEEL_REMORSE;
            m_devilState = DevilState::DEFEATED;
            m_whipTimer = 0.0f;
            m_witherTimer = 0.0f;
            m_judasTimer = 0.0f;
            m_pilateTimer = 0.0f;
            m_devilTimer = 0.0f;
            m_mirageTimer = 0.0f;
            m_attackHitboxActive = false;
            if (m_type == EnemyType::BOSS_JUDAS) {
                AudioManager::PlaySfx(SFX_SILVER_SCATTER);
            }
            if (m_type == EnemyType::BOSS_PONTIUS_PILATE) {
                AudioManager::PlaySfx(SFX_WATER_SPLASH);
                AudioManager::PlaySfx(SFX_ANGEL_CHOIR);
            }
            if (m_type == EnemyType::BOSS_DEVIL) {
                AudioManager::PlaySfx(SFX_DEVIL_LAUGH);
            }
            AudioManager::PlaySfx(SFX_ENEMY_DEFEAT);
        }
    } else {
        if (m_hp <= 0) {
            AudioManager::PlaySfx(SFX_ENEMY_DEFEAT);
        } else {
            AudioManager::PlaySfx(SFX_BOSS_HURT);
        }
    }

    return false;
}

void Enemy::Update(float dt, const Vector2& playerPos, Level& level) {
    if (m_hp <= 0 && !IsBoss()) return;

    if (m_hurtTimer > 0.0f) {
        m_hurtTimer -= dt;
    }

    m_animTimer += dt;

    // Type-specific update behaviors
    switch (m_type) {
        case EnemyType::PATROL_GUARD: {
            // Patrol movement
            m_position.x += m_velocity.x * dt;
            if (m_position.x <= m_patrolMinX) {
                m_position.x = m_patrolMinX;
                m_velocity.x = 45.0f;
                m_facingLeft = false;
            } else if (m_position.x >= m_patrolMaxX) {
                m_position.x = m_patrolMaxX;
                m_velocity.x = -45.0f;
                m_facingLeft = true;
            }

            // Check if player in front within lunge range (75 px)
            float distToPlayer = playerPos.x - m_position.x;
            bool facingPlayer = (m_facingLeft && distToPlayer < 0) || (!m_facingLeft && distToPlayer > 0);
            if (facingPlayer && fabsf(distToPlayer) < 70.0f && fabsf(playerPos.y - m_position.y) < 24.0f) {
                // Lunge spear thrust!
                m_attackHitboxActive = true;
                float spearX = m_facingLeft ? (m_position.x - 14.0f) : (m_position.x + 16.0f);
                m_attackHitbox = { spearX, m_position.y + 8.0f, 18.0f, 10.0f };
            } else {
                m_attackHitboxActive = false;
            }

            // Walk animation
            if (m_animTimer >= 0.2f) {
                m_animTimer = 0.0f;
                m_animFrame = (m_animFrame + 1) % 2;
            }
            break;
        }

        case EnemyType::ARCHER: {
            // Face the player
            m_facingLeft = (playerPos.x < m_position.x);

            m_stateTimer -= dt;
            if (m_isDucking) {
                if (m_stateTimer <= 0.0f) {
                    // Rise and draw bow
                    m_isDucking = false;
                    m_isFiring = false;
                    m_stateTimer = 0.6f;
                }
            } else if (!m_isFiring) {
                if (m_stateTimer <= 0.25f) {
                    m_isFiring = true;
                    m_justFired = true;
                    m_firedPos = { m_facingLeft ? (m_position.x - 8.0f) : (m_position.x + 20.0f), m_position.y + 12.0f };
                    m_firedVelX = m_facingLeft ? -ARROW_SPEED : ARROW_SPEED;
                }
                if (m_stateTimer <= 0.0f) {
                    // Duck back down
                    m_isDucking = true;
                    m_isFiring = false;
                    m_stateTimer = 1.8f;
                }
            }
            break;
        }

        case EnemyType::HEAVY_LEGIONARY: {
            // Patrol movement
            m_position.x += m_velocity.x * dt;
            if (m_position.x <= m_patrolMinX) {
                m_position.x = m_patrolMinX;
                m_velocity.x = 32.0f;
                m_facingLeft = false;
            } else if (m_position.x >= m_patrolMaxX) {
                m_position.x = m_patrolMaxX;
                m_velocity.x = -32.0f;
                m_facingLeft = true;
            }

            // Attack swing if player close
            float dist = playerPos.x - m_position.x;
            bool facingPlayer = (m_facingLeft && dist < 0) || (!m_facingLeft && dist > 0);
            if (facingPlayer && fabsf(dist) < 32.0f && fabsf(playerPos.y - m_position.y) < 24.0f) {
                m_isShieldUp = false; // Lowers shield to attack!
                m_attackHitboxActive = true;
                float swordX = m_facingLeft ? (m_position.x - 14.0f) : (m_position.x + 18.0f);
                m_attackHitbox = { swordX, m_position.y + 10.0f, 16.0f, 14.0f };
            } else {
                m_isShieldUp = true;
                m_attackHitboxActive = false;
            }

            if (m_animTimer >= 0.25f) {
                m_animTimer = 0.0f;
                m_animFrame = (m_animFrame + 1) % 2;
            }
            break;
        }

        case EnemyType::BOSS_GLADIATOR: {
            // Defeat / Withering states
            if (m_whipState == BossWhipState::KNEEL) {
                m_attackHitboxActive = false;
                m_witherTimer += dt;
                if (m_witherTimer >= 0.9f) {
                    m_whipState = BossWhipState::WITHERING;
                    m_witherTimer = 0.0f;
                }
                break;
            }

            if (m_whipState == BossWhipState::WITHERING) {
                m_attackHitboxActive = false;
                m_witherTimer += dt;
                if (m_witherTimer >= 1.4f) {
                    m_whipState = BossWhipState::DEAD_WITHERED;
                    m_active = false;
                }
                break;
            }

            if (m_whipState == BossWhipState::DEAD_WITHERED) {
                m_active = false;
                break;
            }

            // Phase 1 (Gate Approach) State Machine:
            // Whips twice at the gate, takes headshot damage, then moves off screen right into arena
            if (m_bossPhase == BossPhase::PHASE_1_GATE) {
                switch (m_whipState) {
                    case BossWhipState::PHASE1_STANDBY: {
                        m_facingLeft = true;
                        m_attackHitboxActive = false;
                        if (playerPos.x >= 1940.0f) {
                            m_whipState = BossWhipState::PHASE1_WHIP_1_WINDUP;
                            m_whipTimer = 0.0f;
                        }
                        break;
                    }
                    case BossWhipState::PHASE1_WHIP_1_WINDUP: {
                        m_facingLeft = true;
                        m_whipTimer += dt;
                        if (m_whipTimer >= 0.40f) {
                            m_whipTimer = 0.0f;
                            m_whipState = BossWhipState::PHASE1_WHIP_1_CRACK;
                            AudioManager::PlaySfx(SFX_WHIP_CRACK);
                        }
                        break;
                    }
                    case BossWhipState::PHASE1_WHIP_1_CRACK: {
                        m_facingLeft = true;
                        m_whipTimer += dt;
                        m_attackHitboxActive = true;
                        float whipX = m_position.x - 95.0f;
                        m_whipHitbox = { whipX, m_position.y + 18.0f, 95.0f, 10.0f };
                        if (m_whipTimer >= 0.35f) {
                            m_whipTimer = 0.0f;
                            m_attackHitboxActive = false;
                            m_whipState = BossWhipState::PHASE1_PAUSE;
                        }
                        break;
                    }
                    case BossWhipState::PHASE1_PAUSE: {
                        m_facingLeft = true;
                        m_attackHitboxActive = false;
                        m_whipTimer += dt;
                        if (m_whipTimer >= 0.35f) {
                            m_whipTimer = 0.0f;
                            m_whipState = BossWhipState::PHASE1_WHIP_2_WINDUP;
                        }
                        break;
                    }
                    case BossWhipState::PHASE1_WHIP_2_WINDUP: {
                        m_facingLeft = true;
                        m_whipTimer += dt;
                        if (m_whipTimer >= 0.40f) {
                            m_whipTimer = 0.0f;
                            m_whipState = BossWhipState::PHASE1_WHIP_2_CRACK;
                            AudioManager::PlaySfx(SFX_WHIP_CRACK);
                        }
                        break;
                    }
                    case BossWhipState::PHASE1_WHIP_2_CRACK: {
                        m_facingLeft = true;
                        m_whipTimer += dt;
                        m_attackHitboxActive = true;
                        float whipX = m_position.x - 95.0f;
                        m_whipHitbox = { whipX, m_position.y + 18.0f, 95.0f, 10.0f };
                        if (m_whipTimer >= 0.35f) {
                            m_whipTimer = 0.0f;
                            m_attackHitboxActive = false;
                            m_whipState = BossWhipState::PHASE1_WALK_OFF;
                            m_facingLeft = false; // Turn towards arena
                            AudioManager::PlaySfx(SFX_DING_DING);
                        }
                        break;
                    }
                    case BossWhipState::PHASE1_WALK_OFF: {
                        m_facingLeft = false; // Face right towards arena
                        m_attackHitboxActive = false;
                        m_velocity.x = 65.0f;
                        m_position.x += m_velocity.x * dt;

                        if (m_animTimer >= 0.2f) {
                            m_animTimer = 0.0f;
                            m_animFrame = (m_animFrame + 1) % 2;
                        }

                        if (m_position.x >= ARENA_GATE_X + 60.0f) {
                            m_phase1WalkedOff = true;
                            m_position.x = ARENA_MAX_X - 60.0f;
                            m_velocity.x = 0.0f;
                            m_facingLeft = true;
                        }
                        break;
                    }
                    default:
                        break;
                }
                break;
            }

            // Phase 2 (Arena Screen Combat)
            // Face the player while in combat
            m_facingLeft = (playerPos.x < m_position.x);

            float dist = fabsf(playerPos.x - m_position.x);

            // Boss Whip State Machine
            switch (m_whipState) {
                case BossWhipState::IDLE: {
                    m_attackHitboxActive = false;
                    // Tactical advance or spacing
                    if (dist > 85.0f) {
                        float dir = m_facingLeft ? -1.0f : 1.0f;
                        m_position.x += dir * 46.0f * dt;
                    } else if (dist < 46.0f) {
                        float dir = m_facingLeft ? 1.0f : -1.0f;
                        m_position.x += dir * 28.0f * dt;
                    }
                    // Clamp to arena bounds
                    if (m_position.x < ARENA_MIN_X + 16.0f) m_position.x = ARENA_MIN_X + 16.0f;
                    if (m_position.x > ARENA_MAX_X - 35.0f) m_position.x = ARENA_MAX_X - 35.0f;

                    // Stride animation
                    if (m_animTimer >= 0.2f) {
                        m_animTimer = 0.0f;
                        m_animFrame = (m_animFrame + 1) % 2;
                    }

                    m_whipTimer += dt;
                    bool playerInAir = (playerPos.y < GROUND_Y - 24.0f);
                    float triggerTime = (dist < 85.0f) ? 0.6f : 0.95f;

                    if (m_whipTimer >= triggerTime) {
                        m_whipTimer = 0.0f;
                        if (playerInAir || dist < 60.0f) {
                            m_whipState = BossWhipState::OVERHEAD_SLAM;
                        } else {
                            m_whipState = BossWhipState::WINDUP;
                        }
                    }
                    break;
                }

                case BossWhipState::WINDUP: {
                    m_whipTimer += dt;
                    if (m_whipTimer >= 0.40f) {
                        m_whipTimer = 0.0f;
                        m_whipState = BossWhipState::HORIZONTAL_CRACK;
                        AudioManager::PlaySfx(SFX_WHIP_CRACK);
                    }
                    break;
                }

                case BossWhipState::HORIZONTAL_CRACK: {
                    m_whipTimer += dt;
                    m_attackHitboxActive = true;
                    // Horizontal whip lash extending 95 pixels at chest height!
                    float whipX = m_facingLeft ? (m_position.x - 95.0f) : (m_position.x + 24.0f);
                    m_whipHitbox = { whipX, m_position.y + 18.0f, 95.0f, 10.0f };

                    if (m_whipTimer >= 0.35f) {
                        m_whipTimer = 0.0f;
                        m_attackHitboxActive = false;
                        m_whipState = BossWhipState::RECOVERY;
                    }
                    break;
                }

                case BossWhipState::OVERHEAD_SLAM: {
                    m_whipTimer += dt;
                    if (m_whipTimer >= 0.40f && !m_attackHitboxActive) {
                        m_attackHitboxActive = true;
                        AudioManager::PlaySfx(SFX_WHIP_SLAM);
                        float slamX = m_facingLeft ? (m_position.x - 55.0f) : (m_position.x + 30.0f);
                        m_whipHitbox = { slamX, m_position.y + 10.0f, 42.0f, 44.0f };
                    }
                    if (m_whipTimer >= 0.75f) {
                        m_whipTimer = 0.0f;
                        m_attackHitboxActive = false;
                        m_whipState = BossWhipState::RECOVERY;
                    }
                    break;
                }

                case BossWhipState::RECOVERY: {
                    m_attackHitboxActive = false;
                    m_whipTimer += dt;
                    if (m_whipTimer >= 0.80f) {
                        m_whipTimer = 0.0f;
                        m_whipState = BossWhipState::IDLE;
                    }
                    break;
                }

                default:
                    break;
            }
            break;
        }

        case EnemyType::SICARII_ASSASSIN: {
            if (m_sicariiState != SicariiState::WALL_CLING && m_sicariiState != SicariiState::WALL_CRAWL) {
                m_facingLeft = (playerPos.x < m_position.x);
            }

            m_sicariiTimer += dt;
            if (m_sicariiThrowCooldown > 0.0f) m_sicariiThrowCooldown -= dt;
            if (m_sicariiHopCooldown > 0.0f) m_sicariiHopCooldown -= dt;

            switch (m_sicariiState) {
                case SicariiState::WALL_CLING: {
                    m_attackHitboxActive = false;
                    float dist = fabsf(playerPos.x - m_position.x);
                    if (dist < 150.0f) {
                        m_sicariiState = SicariiState::WALL_CRAWL;
                        m_sicariiTimer = 0.0f;
                        m_animTimer = 0.0f;
                    }
                    break;
                }

                case SicariiState::WALL_CRAWL: {
                    m_attackHitboxActive = false;
                    m_position.y += 65.0f * dt;
                    if (m_animTimer >= 0.15f) {
                        m_animTimer = 0.0f;
                        m_animFrame = (m_animFrame + 1) % 2;
                    }

                    if (m_position.y >= 130.0f || (playerPos.y - m_position.y) < 45.0f) {
                        m_sicariiState = SicariiState::HOP;
                        m_sicariiTimer = 0.0f;
                        float hopDir = (playerPos.x < m_position.x) ? -1.0f : 1.0f;
                        m_velocity.x = hopDir * 110.0f;
                        m_velocity.y = -180.0f;
                        m_facingLeft = (hopDir < 0.0f);
                        AudioManager::PlaySfx(SFX_HIGH_JUMP);
                    }
                    break;
                }

                case SicariiState::HOP: {
                    m_attackHitboxActive = false;
                    m_position.x += m_velocity.x * dt;
                    m_position.y += m_velocity.y * dt;
                    m_velocity.y += GRAVITY * dt;
                    if (m_velocity.y > MAX_FALL_SPEED) m_velocity.y = MAX_FALL_SPEED;

                    float prevY = m_position.y - m_velocity.y * dt;
                    float ledgeY = 0.0f;
                    if (m_velocity.y >= 0.0f) {
                        if (level.CheckOneWayLanding(GetHurtbox(), prevY, ledgeY)) {
                            m_position.y = ledgeY - 30.0f;
                            m_velocity = { 0, 0 };
                            m_sicariiState = SicariiState::STAND;
                            m_sicariiTimer = 0.0f;
                        } else if (level.IsOnGround(GetHurtbox(), ledgeY)) {
                            m_position.y = ledgeY - 30.0f;
                            m_velocity = { 0, 0 };
                            m_sicariiState = SicariiState::STAND;
                            m_sicariiTimer = 0.0f;
                        }
                    }
                    break;
                }

                case SicariiState::STAND: {
                    m_attackHitboxActive = false;
                    float dist = fabsf(playerPos.x - m_position.x);
                    float yDiff = playerPos.y - m_position.y;

                    if (fabsf(yDiff) > 28.0f && m_sicariiHopCooldown <= 0.0f && dist < 120.0f) {
                        m_sicariiState = SicariiState::HOP;
                        m_sicariiHopCooldown = 2.5f;
                        float hopDir = (playerPos.x < m_position.x) ? -1.0f : 1.0f;
                        m_velocity.x = hopDir * 100.0f;
                        m_velocity.y = (yDiff < 0) ? -380.0f : -120.0f;
                        AudioManager::PlaySfx(SFX_HIGH_JUMP);
                        break;
                    }

                    if (dist < 32.0f && fabsf(yDiff) < 22.0f) {
                        m_sicariiState = SicariiState::SLASH;
                        m_sicariiTimer = 0.0f;
                        m_attackHitboxActive = true;
                        float slashX = m_facingLeft ? (m_position.x - 14.0f) : (m_position.x + 18.0f);
                        m_attackHitbox = { slashX, m_position.y + 8.0f, 20.0f, 18.0f };
                        AudioManager::PlaySfx(SFX_MELEE_STRIKE);
                        break;
                    }

                    if (dist >= 45.0f && dist <= 200.0f && fabsf(yDiff) < 24.0f && m_sicariiThrowCooldown <= 0.0f) {
                        m_sicariiState = SicariiState::THROW;
                        m_sicariiTimer = 0.0f;
                        m_sicariiThrowCooldown = 2.2f;
                        break;
                    }

                    if (dist > 30.0f) {
                        float stalkDir = (playerPos.x < m_position.x) ? -1.0f : 1.0f;
                        m_position.x += stalkDir * 55.0f * dt;
                    }
                    break;
                }

                case SicariiState::SLASH: {
                    if (m_sicariiTimer <= 0.35f) {
                        m_attackHitboxActive = true;
                        float slashX = m_facingLeft ? (m_position.x - 14.0f) : (m_position.x + 18.0f);
                        m_attackHitbox = { slashX, m_position.y + 8.0f, 20.0f, 18.0f };
                    } else {
                        m_attackHitboxActive = false;
                        if (m_sicariiTimer >= 0.55f) {
                            m_sicariiState = SicariiState::STAND;
                            m_sicariiTimer = 0.0f;
                        }
                    }
                    break;
                }

                case SicariiState::THROW: {
                    m_attackHitboxActive = false;
                    if (m_sicariiTimer >= 0.18f && !m_justThrewDagger && m_sicariiTimer < 0.30f) {
                        m_justThrewDagger = true;
                        m_daggerPos = { m_facingLeft ? (m_position.x - 6.0f) : (m_position.x + 20.0f), m_position.y + 12.0f };
                        m_daggerVelX = m_facingLeft ? -190.0f : 190.0f;
                        AudioManager::PlaySfx(SFX_NAIL_THROW);
                    }
                    if (m_sicariiTimer >= 0.45f) {
                        m_sicariiState = SicariiState::STAND;
                        m_sicariiTimer = 0.0f;
                    }
                    break;
                }

                case SicariiState::HURT: {
                    m_attackHitboxActive = false;
                    if (m_sicariiTimer >= 0.3f) {
                        m_sicariiState = SicariiState::STAND;
                        m_sicariiTimer = 0.0f;
                    }
                    break;
                }
            }
            break;
        }

        case EnemyType::BOSS_TEMPLE_COMMANDER: {
            m_facingLeft = (playerPos.x < m_position.x);

            if (m_hp <= 0) {
                m_attackHitboxActive = false;
                m_whipState = BossWhipState::KNEEL;
                m_witherTimer += dt;
                if (m_witherTimer >= 1.2f) {
                    m_whipState = BossWhipState::WITHERING;
                }
                if (m_witherTimer >= 4.0f) {
                    m_whipState = BossWhipState::DEAD_WITHERED;
                }
                break;
            }

            m_whipTimer += dt;
            switch (m_whipState) {
                case BossWhipState::IDLE: {
                    m_attackHitboxActive = false;
                    if (m_whipTimer >= 0.8f) {
                        m_whipState = BossWhipState::WINDUP;
                        m_whipTimer = 0.0f;
                    }
                    break;
                }
                case BossWhipState::WINDUP: {
                    float dir = m_facingLeft ? -1.0f : 1.0f;
                    m_position.x += dir * 42.0f * dt;
                    if (m_animTimer >= 0.2f) {
                        m_animTimer = 0.0f;
                        m_animFrame = (m_animFrame + 1) % 2;
                    }

                    float dist = fabsf(playerPos.x - m_position.x);
                    if (dist < 40.0f || m_whipTimer >= 1.8f) {
                        m_whipState = BossWhipState::HORIZONTAL_CRACK;
                        m_whipTimer = 0.0f;
                        AudioManager::PlaySfx(SFX_WHIP_CRACK);
                    }
                    break;
                }
                case BossWhipState::HORIZONTAL_CRACK: {
                    m_attackHitboxActive = true;
                    float slashX = m_facingLeft ? (m_position.x - 22.0f) : (m_position.x + 24.0f);
                    m_attackHitbox = { slashX, m_position.y + 10.0f, 28.0f, 26.0f };

                    if (m_whipTimer >= 0.4f) {
                        m_attackHitboxActive = false;
                        m_whipState = BossWhipState::RECOVERY;
                        m_whipTimer = 0.0f;
                    }
                    break;
                }
                case BossWhipState::RECOVERY: {
                    m_attackHitboxActive = false;
                    if (m_whipTimer >= 0.7f) {
                        m_whipState = BossWhipState::IDLE;
                        m_whipTimer = 0.0f;
                    }
                    break;
                }
                default:
                    m_whipState = BossWhipState::IDLE;
                    break;
            }
            break;
        }

        case EnemyType::BOSS_TRIBUNAL_PRAETOR: {
            m_facingLeft = (playerPos.x < m_position.x);

            if (m_hp <= 0) {
                m_attackHitboxActive = false;
                m_whipState = BossWhipState::KNEEL;
                m_witherTimer += dt;
                if (m_witherTimer >= 1.2f) {
                    m_whipState = BossWhipState::WITHERING;
                }
                if (m_witherTimer >= 4.0f) {
                    m_whipState = BossWhipState::DEAD_WITHERED;
                }
                break;
            }

            m_whipTimer += dt;
            switch (m_whipState) {
                case BossWhipState::IDLE: {
                    m_attackHitboxActive = false;
                    if (m_whipTimer >= 0.65f) {
                        m_whipState = BossWhipState::WINDUP;
                        m_whipTimer = 0.0f;
                    }
                    break;
                }
                case BossWhipState::WINDUP: {
                    float dir = m_facingLeft ? -1.0f : 1.0f;
                    m_position.x += dir * 46.0f * dt;
                    if (m_animTimer >= 0.18f) {
                        m_animTimer = 0.0f;
                        m_animFrame = (m_animFrame + 1) % 2;
                    }

                    float dist = fabsf(playerPos.x - m_position.x);
                    if (dist < 46.0f || m_whipTimer >= 1.5f) {
                        // Alternate between broad gladius slash and imperial shield slam
                        m_whipSwingsCount++;
                        if (m_whipSwingsCount % 2 == 1) {
                            m_whipState = BossWhipState::HORIZONTAL_CRACK;
                            AudioManager::PlaySfx(SFX_WHIP_CRACK);
                        } else {
                            m_whipState = BossWhipState::OVERHEAD_SLAM;
                            AudioManager::PlaySfx(SFX_SHIELD_DEFLECT);
                        }
                        m_whipTimer = 0.0f;
                    }
                    break;
                }
                case BossWhipState::HORIZONTAL_CRACK: {
                    m_attackHitboxActive = true;
                    float slashX = m_facingLeft ? (m_position.x - 24.0f) : (m_position.x + 24.0f);
                    m_attackHitbox = { slashX, m_position.y + 8.0f, 30.0f, 28.0f };

                    if (m_whipTimer >= 0.38f) {
                        m_attackHitboxActive = false;
                        m_whipState = BossWhipState::RECOVERY;
                        m_whipTimer = 0.0f;
                    }
                    break;
                }
                case BossWhipState::OVERHEAD_SLAM: {
                    m_attackHitboxActive = true;
                    float slamX = m_facingLeft ? (m_position.x - 26.0f) : (m_position.x + 22.0f);
                    m_attackHitbox = { slamX, m_position.y + 12.0f, 32.0f, 32.0f };

                    if (m_whipTimer >= 0.44f) {
                        m_attackHitboxActive = false;
                        m_whipState = BossWhipState::RECOVERY;
                        m_whipTimer = 0.0f;
                    }
                    break;
                }
                case BossWhipState::RECOVERY: {
                    m_attackHitboxActive = false;
                    if (m_whipTimer >= 0.65f) {
                        m_whipState = BossWhipState::IDLE;
                        m_whipTimer = 0.0f;
                    }
                    break;
                }
                default:
                    m_whipState = BossWhipState::IDLE;
                    break;
            }
            break;
        }

        case EnemyType::TORMENTOR_GUARD: {
            // Patrol movement
            m_position.x += m_velocity.x * dt;
            if (m_position.x <= m_patrolMinX) {
                m_position.x = m_patrolMinX;
                m_velocity.x = 40.0f;
                m_facingLeft = false;
            } else if (m_position.x >= m_patrolMaxX) {
                m_position.x = m_patrolMaxX;
                m_velocity.x = -40.0f;
                m_facingLeft = true;
            }

            // Torch swing timer
            if (m_torchTimer > 0.0f) m_torchTimer -= dt;

            float distToPlayer = playerPos.x - m_position.x;
            bool facingPlayer = (m_facingLeft && distToPlayer < 0) || (!m_facingLeft && distToPlayer > 0);

            if (facingPlayer && fabsf(distToPlayer) < 65.0f && fabsf(playerPos.y - m_position.y) < 24.0f) {
                if (m_torchTimer <= 0.0f) {
                    m_torchSwinging = true;
                    m_torchTimer = 1.3f;
                    AudioManager::PlaySfx(SFX_TORCH_SWING);
                }
            }

            if (m_torchSwinging) {
                m_attackHitboxActive = true;
                float tx = m_facingLeft ? (m_position.x - 16.0f) : (m_position.x + 18.0f);
                m_attackHitbox = { tx, m_position.y + 6.0f, 22.0f, 18.0f };
                if (m_torchTimer <= 0.9f) {
                    m_torchSwinging = false;
                    m_attackHitboxActive = false;
                }
            } else {
                m_attackHitboxActive = false;
            }

            // Walk animation
            if (m_animTimer >= 0.2f) {
                m_animTimer = 0.0f;
                m_animFrame = (m_animFrame + 1) % 2;
            }
            break;
        }

        case EnemyType::GARDEN_THORN_SICARII: {
            m_facingLeft = (playerPos.x < m_position.x);
            m_thornTimer += dt;

            switch (m_thornState) {
                case ThornSicariiState::CROUCH: {
                    m_attackHitboxActive = false;
                    float dist = fabsf(playerPos.x - m_position.x);
                    if (dist < 130.0f && m_thornTimer >= 0.4f) {
                        m_thornState = ThornSicariiState::LEAP;
                        m_thornTimer = 0.0f;
                        float hopDir = (playerPos.x < m_position.x) ? -1.0f : 1.0f;
                        m_velocity.x = hopDir * 115.0f;
                        m_velocity.y = -220.0f;
                        m_facingLeft = (hopDir < 0.0f);
                        AudioManager::PlaySfx(SFX_HIGH_JUMP);
                    }
                    break;
                }
                case ThornSicariiState::LEAP: {
                    m_attackHitboxActive = false;
                    m_position.x += m_velocity.x * dt;
                    m_position.y += m_velocity.y * dt;
                    m_velocity.y += GRAVITY * dt;
                    if (m_velocity.y > MAX_FALL_SPEED) m_velocity.y = MAX_FALL_SPEED;

                    float prevY = m_position.y - m_velocity.y * dt;
                    float ledgeY = 0.0f;
                    if (m_velocity.y >= 0.0f) {
                        if (level.CheckOneWayLanding(GetHurtbox(), prevY, ledgeY) || level.IsOnGround(GetHurtbox(), ledgeY)) {
                            m_position.y = ledgeY - 28.0f;
                            m_velocity = { 0, 0 };
                            m_thornState = ThornSicariiState::SLASH;
                            m_thornTimer = 0.0f;
                            AudioManager::PlaySfx(SFX_MELEE_STRIKE);
                        }
                    }
                    break;
                }
                case ThornSicariiState::SLASH: {
                    if (m_thornTimer <= 0.35f) {
                        m_attackHitboxActive = true;
                        float sx = m_facingLeft ? (m_position.x - 14.0f) : (m_position.x + 16.0f);
                        m_attackHitbox = { sx, m_position.y + 8.0f, 22.0f, 18.0f };
                    } else {
                        m_attackHitboxActive = false;
                        if (m_thornTimer >= 0.65f) {
                            m_thornState = ThornSicariiState::CROUCH;
                            m_thornTimer = 0.0f;
                        }
                    }
                    break;
                }
                case ThornSicariiState::HURT: {
                    m_attackHitboxActive = false;
                    if (m_thornTimer >= 0.3f) {
                        m_thornState = ThornSicariiState::CROUCH;
                        m_thornTimer = 0.0f;
                    }
                    break;
                }
            }
            break;
        }

        case EnemyType::CORRUPTED_PRIEST: {
            m_facingLeft = (playerPos.x < m_position.x);
            m_castCooldown -= dt;

            if (m_castCooldown <= 0.3f && !m_justFiredDark) {
                m_justFiredDark = true;
                m_darkProjPos = { m_facingLeft ? (m_position.x - 10.0f) : (m_position.x + 22.0f), m_position.y + 10.0f };
                m_darkProjVelX = m_facingLeft ? -150.0f : 150.0f;
                m_darkProjVelY = 0.0f;
                m_darkProjIsSpike = false;
                AudioManager::PlaySfx(SFX_DARK_WRATH);
            }

            if (m_castCooldown <= 0.0f) {
                m_castCooldown = 2.4f;
                m_justFiredDark = false;
            }
            break;
        }

        case EnemyType::BOSS_MISSION5_CLONE_GREEN:
        case EnemyType::BOSS_EVIL_CLONE: {
            m_facingLeft = (playerPos.x < m_position.x);

            if (m_hp <= 0) {
                m_attackHitboxActive = false;
                m_cloneState = CloneState::KNEEL;
                m_witherTimer += dt;
                if (m_witherTimer >= 1.2f) {
                    m_cloneState = CloneState::WITHERING;
                }
                if (m_witherTimer >= 4.0f) {
                    m_cloneState = CloneState::DEAD_WITHERED;
                }
                break;
            }

            float dist = fabsf(playerPos.x - m_position.x);

            switch (m_cloneState) {
                case CloneState::IDLE: {
                    m_attackHitboxActive = false;
                    if (dist > 85.0f) {
                        float dir = m_facingLeft ? -1.0f : 1.0f;
                        m_position.x += dir * 55.0f * dt;
                    } else if (dist < 42.0f) {
                        float dir = m_facingLeft ? 1.0f : -1.0f;
                        m_position.x += dir * 35.0f * dt;
                    }

                    if (m_position.x < level.GetArenaMinX() + 16.0f) m_position.x = level.GetArenaMinX() + 16.0f;
                    if (m_position.x > level.GetArenaMaxX() - 35.0f) m_position.x = level.GetArenaMaxX() - 35.0f;

                    if (m_animTimer >= 0.18f) {
                        m_animTimer = 0.0f;
                        m_animFrame = (m_animFrame + 1) % 2;
                    }

                    m_cloneActionTimer += dt;
                    if (m_cloneActionTimer >= 0.75f) {
                        m_cloneActionTimer = 0.0f;
                        m_cloneAttackPattern = (m_cloneAttackPattern + 1) % 5;

                        if (m_cloneAttackPattern == 0) {
                            m_cloneState = CloneState::THROW_STAND;
                        } else if (m_cloneAttackPattern == 1) {
                            m_cloneState = CloneState::JUMP_LEAP;
                            m_velocity.x = (m_facingLeft ? -120.0f : 120.0f);
                            m_velocity.y = -340.0f;
                            m_cloneLeapStartY = m_position.y;
                            AudioManager::PlaySfx(SFX_HIGH_JUMP);
                        } else if (m_cloneAttackPattern == 2) {
                            m_cloneState = CloneState::THROW_CROUCH;
                        } else if (m_cloneAttackPattern == 3) {
                            m_cloneState = CloneState::CHANNEL_WRATH;
                            AudioManager::PlaySfx(SFX_CLONE_TELEPORT);
                        } else {
                            if (dist < 45.0f) {
                                m_cloneState = CloneState::MELEE_STRIKE;
                                AudioManager::PlaySfx(SFX_MELEE_STRIKE);
                            } else {
                                m_cloneState = CloneState::THROW_STAND;
                            }
                        }
                    }
                    break;
                }

                case CloneState::THROW_STAND: {
                    m_cloneActionTimer += dt;
                    if (m_cloneActionTimer >= 0.12f && !m_justFiredDark) {
                        m_justFiredDark = true;
                        m_darkProjPos = { m_facingLeft ? (m_position.x - 8.0f) : (m_position.x + 24.0f), m_position.y + 12.0f };
                        m_darkProjVelX = m_facingLeft ? -240.0f : 240.0f;
                        m_darkProjVelY = 0.0f;
                        m_darkProjIsSpike = false;
                        AudioManager::PlaySfx(SFX_NAIL_THROW);
                    }
                    if (m_cloneActionTimer >= 0.35f) {
                        m_cloneState = CloneState::IDLE;
                        m_cloneActionTimer = 0.0f;
                        m_justFiredDark = false;
                    }
                    break;
                }

                case CloneState::THROW_CROUCH: {
                    m_cloneActionTimer += dt;
                    if (m_cloneActionTimer >= 0.12f && !m_justFiredDark) {
                        m_justFiredDark = true;
                        m_darkProjPos = { m_facingLeft ? (m_position.x - 8.0f) : (m_position.x + 24.0f), m_position.y + 20.0f };
                        m_darkProjVelX = m_facingLeft ? -240.0f : 240.0f;
                        m_darkProjVelY = 0.0f;
                        m_darkProjIsSpike = false;
                        AudioManager::PlaySfx(SFX_NAIL_THROW);
                    }
                    if (m_cloneActionTimer >= 0.35f) {
                        m_cloneState = CloneState::IDLE;
                        m_cloneActionTimer = 0.0f;
                        m_justFiredDark = false;
                    }
                    break;
                }

                case CloneState::JUMP_LEAP: {
                    m_position.x += m_velocity.x * dt;
                    m_position.y += m_velocity.y * dt;
                    m_velocity.y += GRAVITY * dt;
                    if (m_velocity.y > MAX_FALL_SPEED) m_velocity.y = MAX_FALL_SPEED;

                    if (m_position.y >= GROUND_Y - 30.0f) {
                        m_position.y = GROUND_Y - 30.0f;
                        m_velocity = { 0, 0 };
                        m_cloneState = CloneState::IDLE;
                        m_cloneActionTimer = 0.0f;
                    }
                    break;
                }

                case CloneState::CHANNEL_WRATH: {
                    m_cloneActionTimer += dt;
                    // At 0.8s: launch dark floor spike erupting at player's feet!
                    if (m_cloneActionTimer >= 0.8f && !m_justFiredDark) {
                        m_justFiredDark = true;
                        m_darkProjPos = { playerPos.x, GROUND_Y - 14.0f };
                        m_darkProjVelX = 0.0f;
                        m_darkProjVelY = -50.0f;
                        m_darkProjIsSpike = true;
                        AudioManager::PlaySfx(SFX_DARK_SPIKE);
                    }
                    if (m_cloneActionTimer >= 1.6f) {
                        m_cloneState = CloneState::IDLE;
                        m_cloneActionTimer = 0.0f;
                        m_justFiredDark = false;
                    }
                    break;
                }

                case CloneState::MELEE_STRIKE: {
                    m_cloneActionTimer += dt;
                    if (m_cloneActionTimer <= 0.25f) {
                        m_attackHitboxActive = true;
                        float mx = m_facingLeft ? (m_position.x - 14.0f) : (m_position.x + 20.0f);
                        m_attackHitbox = { mx, m_position.y + 8.0f, 22.0f, 18.0f };
                    } else {
                        m_attackHitboxActive = false;
                        if (m_cloneActionTimer >= 0.38f) {
                            m_cloneState = CloneState::IDLE;
                            m_cloneActionTimer = 0.0f;
                        }
                    }
                    break;
                }

                default:
                    m_cloneState = CloneState::IDLE;
                    break;
            }
            break;
        }

        case EnemyType::BOSS_DEVIL: {
            if (m_devilState == DevilState::DEFEATED) {
                m_attackHitboxActive = false;
                m_witherTimer += dt;
                if (m_witherTimer >= 2.0f) {
                    m_active = false;
                }
                break;
            }

            m_facingLeft = (playerPos.x < m_position.x);
            float dist = fabsf(playerPos.x - m_position.x);

            switch (m_devilState) {
                case DevilState::IDLE: {
                    m_attackHitboxActive = false;
                    if (dist > 75.0f) {
                        float dir = m_facingLeft ? -1.0f : 1.0f;
                        m_position.x += dir * 45.0f * dt;
                    } else if (dist < 38.0f) {
                        float dir = m_facingLeft ? 1.0f : -1.0f;
                        m_position.x += dir * 32.0f * dt;
                    }

                    if (m_position.x < level.GetArenaMinX() + 16.0f) m_position.x = level.GetArenaMinX() + 16.0f;
                    if (m_position.x > level.GetArenaMaxX() - 35.0f) m_position.x = level.GetArenaMaxX() - 35.0f;

                    if (m_animTimer >= 0.20f) {
                        m_animTimer = 0.0f;
                        m_animFrame = (m_animFrame + 1) % 2;
                    }

                    m_devilTimer += dt;
                    if (m_devilTimer >= 0.80f) {
                        m_devilTimer = 0.0f;
                        m_devilAttackPattern = (m_devilAttackPattern + 1) % 4;

                        if (m_devilAttackPattern == 0) {
                            m_devilState = DevilState::TRIDENT_THRUST;
                            AudioManager::PlaySfx(SFX_MELEE_STRIKE);
                        } else if (m_devilAttackPattern == 1) {
                            m_devilState = DevilState::TAIL_WHIP_WINDUP;
                        } else if (m_devilAttackPattern == 2) {
                            m_devilState = DevilState::FIRE_SURGE;
                        } else {
                            m_devilState = DevilState::TAIL_WHIP_WINDUP;
                        }
                    }
                    break;
                }

                case DevilState::TRIDENT_THRUST: {
                    m_devilTimer += dt;
                    if (m_devilTimer <= 0.30f) {
                        m_attackHitboxActive = true;
                        float tx = m_facingLeft ? (m_position.x - 24.0f) : (m_position.x + 20.0f);
                        m_attackHitbox = { tx, m_position.y + 10.0f, 28.0f, 20.0f };
                    } else {
                        m_attackHitboxActive = false;
                        if (m_devilTimer >= 0.50f) {
                            m_devilState = DevilState::IDLE;
                            m_devilTimer = 0.0f;
                        }
                    }
                    break;
                }

                case DevilState::TAIL_WHIP_WINDUP: {
                    m_attackHitboxActive = false;
                    m_devilTimer += dt;
                    if (m_devilTimer >= 0.35f) {
                        m_devilState = DevilState::TAIL_WHIP_STRIKE;
                        m_devilTimer = 0.0f;
                        AudioManager::PlaySfx(SFX_WHIP_CRACK);
                    }
                    break;
                }

                case DevilState::TAIL_WHIP_STRIKE: {
                    m_devilTimer += dt;
                    if (m_devilTimer <= 0.28f) {
                        m_attackHitboxActive = true;
                        float wx = m_facingLeft ? (m_position.x - 52.0f) : (m_position.x + 16.0f);
                        m_attackHitbox = { wx, m_position.y + 14.0f, 56.0f, 16.0f };
                    } else {
                        m_attackHitboxActive = false;
                        if (m_devilTimer >= 0.45f) {
                            m_devilState = DevilState::IDLE;
                            m_devilTimer = 0.0f;
                        }
                    }
                    break;
                }

                case DevilState::FIRE_SURGE: {
                    m_devilTimer += dt;
                    if (m_devilTimer >= 0.15f && !m_justFiredDark) {
                        m_justFiredDark = true;
                        m_darkProjPos = { m_facingLeft ? (m_position.x - 8.0f) : (m_position.x + 24.0f), m_position.y + 14.0f };
                        m_darkProjVelX = m_facingLeft ? -220.0f : 220.0f;
                        m_darkProjVelY = 0.0f;
                        m_darkProjIsSpike = false;
                        AudioManager::PlaySfx(SFX_TORCH_SWING);
                    }
                    if (m_devilTimer >= 0.45f) {
                        m_devilState = DevilState::IDLE;
                        m_devilTimer = 0.0f;
                        m_justFiredDark = false;
                    }
                    break;
                }

                default:
                    m_devilState = DevilState::IDLE;
                    break;
            }
            break;
        }

        case EnemyType::MONEY_CHANGER: {
            m_position.x += m_velocity.x * dt;
            if (m_position.x <= m_patrolMinX) {
                m_position.x = m_patrolMinX;
                m_velocity.x = 38.0f;
                m_facingLeft = false;
            } else if (m_position.x >= m_patrolMaxX) {
                m_position.x = m_patrolMaxX;
                m_velocity.x = -38.0f;
                m_facingLeft = true;
            }

            float dist = playerPos.x - m_position.x;
            bool facing = (m_facingLeft && dist < 0) || (!m_facingLeft && dist > 0);
            if (facing && fabsf(dist) < 75.0f && fabsf(playerPos.y - m_position.y) < 24.0f) {
                m_moneyChangerAttacking = true;
                m_attackHitboxActive = true;
                float sx = m_facingLeft ? (m_position.x - 14.0f) : (m_position.x + 16.0f);
                m_attackHitbox = { sx, m_position.y + 8.0f, 18.0f, 14.0f };
            } else {
                m_moneyChangerAttacking = false;
                m_attackHitboxActive = false;
            }

            if (m_animTimer >= 0.2f) {
                m_animTimer = 0.0f;
                m_animFrame = (m_animFrame + 1) % 2;
            }
            break;
        }

        case EnemyType::LEVITE_ENFORCER: {
            m_position.x += m_velocity.x * dt;
            if (m_position.x <= m_patrolMinX) {
                m_position.x = m_patrolMinX;
                m_velocity.x = 35.0f;
                m_facingLeft = false;
            } else if (m_position.x >= m_patrolMaxX) {
                m_position.x = m_patrolMaxX;
                m_velocity.x = -35.0f;
                m_facingLeft = true;
            }

            float dist = playerPos.x - m_position.x;
            bool facing = (m_facingLeft && dist < 0) || (!m_facingLeft && dist > 0);
            if (facing && fabsf(dist) < 65.0f && fabsf(playerPos.y - m_position.y) < 24.0f) {
                m_enforcerAttacking = true;
                m_attackHitboxActive = true;
                float px = m_facingLeft ? (m_position.x - 16.0f) : (m_position.x + 18.0f);
                m_attackHitbox = { px, m_position.y + 6.0f, 20.0f, 18.0f };
            } else {
                m_enforcerAttacking = false;
                m_attackHitboxActive = false;
            }

            if (m_animTimer >= 0.2f) {
                m_animTimer = 0.0f;
                m_animFrame = (m_animFrame + 1) % 2;
            }
            break;
        }

        case EnemyType::BOSS_BARABBAS: {
            m_facingLeft = (playerPos.x < m_position.x);

            if (m_hp <= 0) {
                m_attackHitboxActive = false;
                m_whipState = BossWhipState::KNEEL;
                m_witherTimer += dt;
                if (m_witherTimer >= 1.2f) {
                    m_whipState = BossWhipState::WITHERING;
                }
                if (m_witherTimer >= 4.0f) {
                    m_whipState = BossWhipState::DEAD_WITHERED;
                }
                break;
            }

            float dist = fabsf(playerPos.x - m_position.x);

            switch (m_whipState) {
                case BossWhipState::IDLE: {
                    m_attackHitboxActive = false;
                    if (dist > 65.0f) {
                        float dir = m_facingLeft ? -1.0f : 1.0f;
                        m_position.x += dir * 60.0f * dt;
                    }

                    if (m_position.x < level.GetArenaMinX() + 16.0f) m_position.x = level.GetArenaMinX() + 16.0f;
                    if (m_position.x > level.GetArenaMaxX() - 35.0f) m_position.x = level.GetArenaMaxX() - 35.0f;

                    if (m_animTimer >= 0.18f) {
                        m_animTimer = 0.0f;
                        m_animFrame = (m_animFrame + 1) % 2;
                    }

                    m_whipTimer += dt;
                    if (m_whipTimer >= 0.75f) {
                        m_whipTimer = 0.0f;
                        m_whipSwingsCount = (m_whipSwingsCount + 1) % 3;

                        if (m_whipSwingsCount == 0 || m_whipSwingsCount == 2) {
                            m_whipState = BossWhipState::HORIZONTAL_CRACK; // Double scimitar slash
                            AudioManager::PlaySfx(SFX_MELEE_STRIKE);
                        } else {
                            m_whipState = BossWhipState::OVERHEAD_SLAM; // Leap & ground slam
                            m_velocity.y = -320.0f;
                            AudioManager::PlaySfx(SFX_HIGH_JUMP);
                        }
                    }
                    break;
                }

                case BossWhipState::HORIZONTAL_CRACK: {
                    m_whipTimer += dt;
                    float dir = m_facingLeft ? -1.0f : 1.0f;
                    m_position.x += dir * 45.0f * dt;

                    if (m_whipTimer <= 0.32f) {
                        m_attackHitboxActive = true;
                        float sx = m_facingLeft ? (m_position.x - 18.0f) : (m_position.x + 22.0f);
                        m_attackHitbox = { sx, m_position.y + 6.0f, 24.0f, 20.0f };
                    } else {
                        m_attackHitboxActive = false;
                        if (m_whipTimer >= 0.45f) {
                            m_whipState = BossWhipState::IDLE;
                            m_whipTimer = 0.0f;
                        }
                    }
                    break;
                }

                case BossWhipState::OVERHEAD_SLAM: {
                    m_position.y += m_velocity.y * dt;
                    m_velocity.y += GRAVITY * dt;
                    if (m_velocity.y > MAX_FALL_SPEED) m_velocity.y = MAX_FALL_SPEED;

                    float dir = m_facingLeft ? -1.0f : 1.0f;
                    m_position.x += dir * 50.0f * dt;

                    if (m_position.y >= GROUND_Y - 32.0f) {
                        m_position.y = GROUND_Y - 32.0f;
                        m_velocity.y = 0.0f;
                        m_attackHitboxActive = true;
                        m_attackHitbox = { m_position.x - 12.0f, m_position.y + 12.0f, 48.0f, 18.0f };
                        m_whipTimer += dt;
                        if (m_whipTimer >= 0.25f) {
                            m_attackHitboxActive = false;
                            m_whipState = BossWhipState::IDLE;
                            m_whipTimer = 0.0f;
                        }
                    }
                    break;
                }

                default:
                    m_whipState = BossWhipState::IDLE;
                    break;
            }
            break;
        }

        case EnemyType::BOSS_CAIAPHAS: {
            m_facingLeft = (playerPos.x < m_position.x);

            if (m_hp <= 0) {
                m_attackHitboxActive = false;
                m_whipState = BossWhipState::KNEEL;
                m_witherTimer += dt;
                if (m_witherTimer >= 1.2f) {
                    m_whipState = BossWhipState::WITHERING;
                }
                if (m_witherTimer >= 4.0f) {
                    m_whipState = BossWhipState::DEAD_WITHERED;
                }
                break;
            }

            float dist = fabsf(playerPos.x - m_position.x);

            switch (m_whipState) {
                case BossWhipState::IDLE: {
                    m_attackHitboxActive = false;
                    // Keep distance from Jesus
                    if (dist < 55.0f) {
                        float dir = m_facingLeft ? 1.0f : -1.0f;
                        m_position.x += dir * 45.0f * dt;
                    }

                    if (m_position.x < level.GetArenaMinX() + 16.0f) m_position.x = level.GetArenaMinX() + 16.0f;
                    if (m_position.x > level.GetArenaMaxX() - 35.0f) m_position.x = level.GetArenaMaxX() - 35.0f;

                    m_whipTimer += dt;
                    if (m_whipTimer >= 1.1f) {
                        m_whipTimer = 0.0f;
                        m_whipState = BossWhipState::OVERHEAD_SLAM; // Staff raise & lightning strike
                    }
                    break;
                }

                case BossWhipState::OVERHEAD_SLAM: {
                    m_whipTimer += dt;
                    if (m_whipTimer >= 0.40f && !m_justCastLightning) {
                        m_justCastLightning = true;
                        m_lightningPos = { playerPos.x, 0.0f };
                        AudioManager::PlaySfx(SFX_THUNDER);
                    }
                    if (m_whipTimer >= 0.95f) {
                        m_whipState = BossWhipState::IDLE;
                        m_whipTimer = 0.0f;
                        m_justCastLightning = false;
                    }
                    break;
                }

                default:
                    m_whipState = BossWhipState::IDLE;
                    break;
            }
            break;
        }

        case EnemyType::BOSS_JUDAS: {
            m_facingLeft = (playerPos.x < m_position.x);

            if (m_hp <= 0) {
                m_attackHitboxActive = false;
                m_judasState = JudasState::KNEEL;
                m_witherTimer += dt;
                if (m_witherTimer >= 1.2f) {
                    m_judasState = JudasState::WITHERING;
                }
                if (m_witherTimer >= 4.0f) {
                    m_judasState = JudasState::DEAD_WITHERED;
                }
                break;
            }

            float dist = fabsf(playerPos.x - m_position.x);

            switch (m_judasState) {
                case JudasState::IDLE: {
                    m_attackHitboxActive = false;
                    if (dist > 85.0f) {
                        float dir = m_facingLeft ? -1.0f : 1.0f;
                        m_position.x += dir * 55.0f * dt;
                    } else if (dist < 38.0f) {
                        float dir = m_facingLeft ? 1.0f : -1.0f;
                        m_position.x += dir * 40.0f * dt;
                    }

                    if (m_position.x < level.GetArenaMinX() + 16.0f) m_position.x = level.GetArenaMinX() + 16.0f;
                    if (m_position.x > level.GetArenaMaxX() - 35.0f) m_position.x = level.GetArenaMaxX() - 35.0f;

                    if (m_animTimer >= 0.18f) {
                        m_animTimer = 0.0f;
                        m_animFrame = (m_animFrame + 1) % 2;
                    }

                    m_judasTimer += dt;
                    if (m_judasTimer >= 0.72f) {
                        m_judasTimer = 0.0f;
                        m_judasAttackPattern = (m_judasAttackPattern + 1) % 5;

                        if (m_judasAttackPattern == 0 || m_judasAttackPattern == 4) {
                            m_judasState = JudasState::TOSS_SILVER;
                        } else if (m_judasAttackPattern == 1) {
                            m_judasState = JudasState::SHADOW_DASH;
                            m_velocity.x = (m_facingLeft ? -220.0f : 220.0f);
                            AudioManager::PlaySfx(SFX_TRAITOR_DASH);
                        } else if (m_judasAttackPattern == 2) {
                            m_judasState = JudasState::DAGGER_STAB;
                            AudioManager::PlaySfx(SFX_MELEE_STRIKE);
                        } else {
                            m_judasState = JudasState::MONEY_SLAM;
                            AudioManager::PlaySfx(SFX_WHIP_SLAM);
                        }
                    }
                    break;
                }

                case JudasState::TOSS_SILVER: {
                    m_judasTimer += dt;
                    if (m_judasTimer >= 0.12f && !m_justThrewSilver) {
                        m_justThrewSilver = true;
                        m_silverTossPos = { m_facingLeft ? (m_position.x - 8.0f) : (m_position.x + 22.0f), m_position.y + 12.0f };
                        m_silverTossDir = m_facingLeft ? -1.0f : 1.0f;
                        AudioManager::PlaySfx(SFX_SILVER_SCATTER);
                    }
                    if (m_judasTimer >= 0.38f) {
                        m_judasState = JudasState::IDLE;
                        m_judasTimer = 0.0f;
                        m_justThrewSilver = false;
                    }
                    break;
                }

                case JudasState::SHADOW_DASH: {
                    m_judasTimer += dt;
                    m_position.x += m_velocity.x * dt;
                    if (m_position.x < level.GetArenaMinX() + 16.0f) m_position.x = level.GetArenaMinX() + 16.0f;
                    if (m_position.x > level.GetArenaMaxX() - 35.0f) m_position.x = level.GetArenaMaxX() - 35.0f;

                    if (m_judasTimer >= 0.24f || dist < 32.0f) {
                        m_judasState = JudasState::DAGGER_STAB;
                        m_judasTimer = 0.0f;
                        AudioManager::PlaySfx(SFX_MELEE_STRIKE);
                    }
                    break;
                }

                case JudasState::DAGGER_STAB: {
                    m_judasTimer += dt;
                    if (m_judasTimer <= 0.22f) {
                        m_attackHitboxActive = true;
                        float sx = m_facingLeft ? (m_position.x - 14.0f) : (m_position.x + 20.0f);
                        m_attackHitbox = { sx, m_position.y + 8.0f, 22.0f, 16.0f };
                    } else {
                        m_attackHitboxActive = false;
                        if (m_judasTimer >= 0.35f) {
                            m_judasState = JudasState::IDLE;
                            m_judasTimer = 0.0f;
                        }
                    }
                    break;
                }

                case JudasState::MONEY_SLAM: {
                    m_judasTimer += dt;
                    if (m_judasTimer >= 0.15f && !m_justSpawnedShockwave) {
                        m_justSpawnedShockwave = true;
                        m_shockwavePos = { m_position.x + 12.0f, GROUND_Y - 12.0f };
                        m_shockwaveDir = m_facingLeft ? -1.0f : 1.0f;
                    }
                    if (m_judasTimer >= 0.42f) {
                        m_judasState = JudasState::IDLE;
                        m_judasTimer = 0.0f;
                        m_justSpawnedShockwave = false;
                    }
                    break;
                }

                default:
                    m_judasState = JudasState::IDLE;
                    break;
            }
            break;
        }

        case EnemyType::SIGNIFER_GUARD: {
            m_position.x += m_velocity.x * dt;
            if (m_position.x <= m_patrolMinX) {
                m_position.x = m_patrolMinX;
                m_velocity.x = 35.0f;
                m_facingLeft = false;
            } else if (m_position.x >= m_patrolMaxX) {
                m_position.x = m_patrolMaxX;
                m_velocity.x = -35.0f;
                m_facingLeft = true;
            }

            m_signiferRallyTimer += dt;
            if (m_signiferRallyTimer >= 2.4f) {
                m_signiferRallyTimer = 0.0f;
                AudioManager::PlaySfx(SFX_ROMAN_HORN);
                m_attackHitboxActive = true;
                float sx = m_facingLeft ? (m_position.x - 14.0f) : (m_position.x + 18.0f);
                m_attackHitbox = { sx, m_position.y + 6.0f, 22.0f, 20.0f };
            } else if (m_signiferRallyTimer > 0.4f) {
                m_attackHitboxActive = false;
            }

            if (m_animTimer >= 0.2f) {
                m_animTimer = 0.0f;
                m_animFrame = (m_animFrame + 1) % 2;
            }
            break;
        }

        case EnemyType::PRAETORIAN_GUARD: {
            m_position.x += m_velocity.x * dt;
            if (m_position.x <= m_patrolMinX) {
                m_position.x = m_patrolMinX;
                m_velocity.x = 30.0f;
                m_facingLeft = false;
            } else if (m_position.x >= m_patrolMaxX) {
                m_position.x = m_patrolMaxX;
                m_velocity.x = -30.0f;
                m_facingLeft = true;
            }

            float dist = playerPos.x - m_position.x;
            bool facing = (m_facingLeft && dist < 0) || (!m_facingLeft && dist > 0);
            if (facing && fabsf(dist) < 65.0f && fabsf(playerPos.y - m_position.y) < 24.0f) {
                m_isShieldUp = false;
                m_attackHitboxActive = true;
                float px = m_facingLeft ? (m_position.x - 18.0f) : (m_position.x + 20.0f);
                m_attackHitbox = { px, m_position.y + 6.0f, 22.0f, 20.0f };
            } else {
                m_isShieldUp = true;
                m_attackHitboxActive = false;
            }

            if (m_animTimer >= 0.2f) {
                m_animTimer = 0.0f;
                m_animFrame = (m_animFrame + 1) % 2;
            }
            break;
        }

        case EnemyType::IMPERIAL_LICTOR: {
            m_position.x += m_velocity.x * dt;
            if (m_position.x <= m_patrolMinX) {
                m_position.x = m_patrolMinX;
                m_velocity.x = 40.0f;
                m_facingLeft = false;
            } else if (m_position.x >= m_patrolMaxX) {
                m_position.x = m_patrolMaxX;
                m_velocity.x = -40.0f;
                m_facingLeft = true;
            }

            float dist = playerPos.x - m_position.x;
            bool facing = (m_facingLeft && dist < 0) || (!m_facingLeft && dist > 0);
            if (facing && fabsf(dist) < 55.0f && fabsf(playerPos.y - m_position.y) < 24.0f) {
                m_lictorSwinging = true;
                m_attackHitboxActive = true;
                float lx = m_facingLeft ? (m_position.x - 16.0f) : (m_position.x + 18.0f);
                m_attackHitbox = { lx, m_position.y + 4.0f, 20.0f, 24.0f };
            } else {
                m_lictorSwinging = false;
                m_attackHitboxActive = false;
            }

            if (m_animTimer >= 0.2f) {
                m_animTimer = 0.0f;
                m_animFrame = (m_animFrame + 1) % 2;
            }
            break;
        }

        case EnemyType::BOSS_PREFECT_QUINTUS: {
            m_facingLeft = (playerPos.x < m_position.x);

            if (m_hp <= 0) {
                m_attackHitboxActive = false;
                m_whipState = BossWhipState::KNEEL;
                m_witherTimer += dt;
                if (m_witherTimer >= 1.2f) {
                    m_whipState = BossWhipState::WITHERING;
                }
                if (m_witherTimer >= 4.0f) {
                    m_whipState = BossWhipState::DEAD_WITHERED;
                }
                break;
            }

            float dist = fabsf(playerPos.x - m_position.x);

            switch (m_whipState) {
                case BossWhipState::IDLE: {
                    m_attackHitboxActive = false;
                    m_prefectRushing = false;
                    if (dist > 60.0f) {
                        float dir = m_facingLeft ? -1.0f : 1.0f;
                        m_position.x += dir * 65.0f * dt;
                    }

                    if (m_position.x < level.GetArenaMinX() + 16.0f) m_position.x = level.GetArenaMinX() + 16.0f;
                    if (m_position.x > level.GetArenaMaxX() - 35.0f) m_position.x = level.GetArenaMaxX() - 35.0f;

                    if (m_animTimer >= 0.18f) {
                        m_animTimer = 0.0f;
                        m_animFrame = (m_animFrame + 1) % 2;
                    }

                    m_whipTimer += dt;
                    if (m_whipTimer >= 0.8f) {
                        m_whipTimer = 0.0f;
                        m_whipSwingsCount = (m_whipSwingsCount + 1) % 3;

                        if (m_whipSwingsCount == 0) {
                            m_whipState = BossWhipState::HORIZONTAL_CRACK; // Double Gladius Slash
                            AudioManager::PlaySfx(SFX_MELEE_STRIKE);
                        } else if (m_whipSwingsCount == 1) {
                            m_whipState = BossWhipState::WINDUP; // Shield Rush charge across arena!
                            m_prefectRushing = true;
                            m_prefectRushTimer = 0.0f;
                            AudioManager::PlaySfx(SFX_ROMAN_HORN);
                        } else {
                            m_whipState = BossWhipState::OVERHEAD_SLAM; // Leap down-thrust
                            m_velocity.y = -330.0f;
                            AudioManager::PlaySfx(SFX_HIGH_JUMP);
                        }
                    }
                    break;
                }

                case BossWhipState::HORIZONTAL_CRACK: {
                    m_whipTimer += dt;
                    float dir = m_facingLeft ? -1.0f : 1.0f;
                    m_position.x += dir * 45.0f * dt;

                    if (m_whipTimer <= 0.30f) {
                        m_attackHitboxActive = true;
                        float sx = m_facingLeft ? (m_position.x - 18.0f) : (m_position.x + 22.0f);
                        m_attackHitbox = { sx, m_position.y + 6.0f, 24.0f, 20.0f };
                    } else {
                        m_attackHitboxActive = false;
                        if (m_whipTimer >= 0.45f) {
                            m_whipState = BossWhipState::IDLE;
                            m_whipTimer = 0.0f;
                        }
                    }
                    break;
                }

                case BossWhipState::WINDUP: { // Shield Rush
                    m_prefectRushTimer += dt;
                    float rushSpeed = 190.0f;
                    float dir = m_facingLeft ? -1.0f : 1.0f;
                    m_position.x += dir * rushSpeed * dt;

                    m_attackHitboxActive = true;
                    float rx = m_facingLeft ? (m_position.x - 14.0f) : (m_position.x + 18.0f);
                    m_attackHitbox = { rx, m_position.y + 6.0f, 20.0f, 22.0f };

                    if (m_position.x <= level.GetArenaMinX() + 16.0f || m_position.x >= level.GetArenaMaxX() - 35.0f || m_prefectRushTimer >= 0.65f) {
                        m_attackHitboxActive = false;
                        m_prefectRushing = false;
                        m_whipState = BossWhipState::IDLE;
                        m_whipTimer = 0.0f;
                    }
                    break;
                }

                case BossWhipState::OVERHEAD_SLAM: {
                    m_position.y += m_velocity.y * dt;
                    m_velocity.y += GRAVITY * dt;
                    if (m_velocity.y > MAX_FALL_SPEED) m_velocity.y = MAX_FALL_SPEED;

                    float dir = m_facingLeft ? -1.0f : 1.0f;
                    m_position.x += dir * 55.0f * dt;

                    if (m_position.y >= GROUND_Y - 32.0f) {
                        m_position.y = GROUND_Y - 32.0f;
                        m_velocity.y = 0.0f;
                        m_attackHitboxActive = true;
                        m_attackHitbox = { m_position.x - 14.0f, m_position.y + 12.0f, 48.0f, 18.0f };
                        m_whipTimer += dt;
                        if (m_whipTimer >= 0.25f) {
                            m_attackHitboxActive = false;
                            m_whipState = BossWhipState::IDLE;
                            m_whipTimer = 0.0f;
                        }
                    }
                    break;
                }

                default:
                    m_whipState = BossWhipState::IDLE;
                    break;
            }
            break;
        }

        case EnemyType::BOSS_ADVOCATE_TERTULLUS: {
            m_facingLeft = (playerPos.x < m_position.x);

            if (m_hp <= 0) {
                m_attackHitboxActive = false;
                m_whipState = BossWhipState::KNEEL;
                m_witherTimer += dt;
                if (m_witherTimer >= 1.2f) {
                    m_whipState = BossWhipState::WITHERING;
                }
                if (m_witherTimer >= 4.0f) {
                    m_whipState = BossWhipState::DEAD_WITHERED;
                }
                break;
            }

            float dist = fabsf(playerPos.x - m_position.x);

            switch (m_whipState) {
                case BossWhipState::IDLE: {
                    m_attackHitboxActive = false;
                    if (dist < 75.0f) {
                        float dir = m_facingLeft ? 1.0f : -1.0f;
                        m_position.x += dir * 50.0f * dt;
                    } else if (dist > 120.0f) {
                        float dir = m_facingLeft ? -1.0f : 1.0f;
                        m_position.x += dir * 50.0f * dt;
                    }

                    if (m_position.x < level.GetArenaMinX() + 16.0f) m_position.x = level.GetArenaMinX() + 16.0f;
                    if (m_position.x > level.GetArenaMaxX() - 35.0f) m_position.x = level.GetArenaMaxX() - 35.0f;

                    if (m_animTimer >= 0.18f) {
                        m_animTimer = 0.0f;
                        m_animFrame = (m_animFrame + 1) % 2;
                    }

                    m_tertullusTimer += dt;
                    if (m_tertullusTimer >= 0.85f) {
                        m_tertullusTimer = 0.0f;
                        m_whipSwingsCount = (m_whipSwingsCount + 1) % 3;

                        if (m_whipSwingsCount == 0) {
                            m_whipState = BossWhipState::HORIZONTAL_CRACK; // Sonic Shout Wave
                            m_justShouted = true;
                            m_shoutPos = { m_position.x + (m_facingLeft ? -8.0f : 20.0f), m_position.y + 8.0f };
                            m_shoutDir = m_facingLeft ? -1.0f : 1.0f;
                            AudioManager::PlaySfx(SFX_ROMAN_HORN);
                        } else if (m_whipSwingsCount == 1) {
                            m_whipState = BossWhipState::WINDUP; // Decree Scroll throw
                            m_justThrewDagger = true;
                            m_daggerPos = { m_position.x + (m_facingLeft ? -8.0f : 20.0f), m_position.y + 10.0f };
                            m_daggerVelX = (m_facingLeft ? -190.0f : 190.0f);
                            AudioManager::PlaySfx(SFX_NAIL_THROW);
                        } else {
                            m_whipState = BossWhipState::OVERHEAD_SLAM; // Magistrate Rod Strike
                            AudioManager::PlaySfx(SFX_FASCES_STRIKE);
                        }
                    }
                    break;
                }

                case BossWhipState::HORIZONTAL_CRACK: {
                    m_tertullusTimer += dt;
                    if (m_tertullusTimer >= 0.35f) {
                        m_whipState = BossWhipState::IDLE;
                        m_tertullusTimer = 0.0f;
                    }
                    break;
                }

                case BossWhipState::WINDUP: {
                    m_tertullusTimer += dt;
                    if (m_tertullusTimer >= 0.32f) {
                        m_whipState = BossWhipState::IDLE;
                        m_tertullusTimer = 0.0f;
                    }
                    break;
                }

                case BossWhipState::OVERHEAD_SLAM: {
                    m_tertullusTimer += dt;
                    if (m_tertullusTimer <= 0.25f) {
                        m_attackHitboxActive = true;
                        float rx = m_facingLeft ? (m_position.x - 16.0f) : (m_position.x + 20.0f);
                        m_attackHitbox = { rx, m_position.y + 6.0f, 22.0f, 22.0f };
                    } else {
                        m_attackHitboxActive = false;
                        if (m_tertullusTimer >= 0.40f) {
                            m_whipState = BossWhipState::IDLE;
                            m_tertullusTimer = 0.0f;
                        }
                    }
                    break;
                }

                default:
                    m_whipState = BossWhipState::IDLE;
                    break;
            }
            break;
        }

        case EnemyType::BOSS_PONTIUS_PILATE: {
            if (m_mirageTimer > 0.0f) {
                m_mirageTimer -= dt;
            }

            if (m_hp <= 0) {
                m_attackHitboxActive = false;
                m_mirageTimer = 0.0f;

                if (m_pilateState != PilateState::WASH_HANDS_CELESTIAL && m_pilateState != PilateState::DEFEATED_REDEEMED) {
                    m_pilateState = PilateState::KNEEL_REMORSE;
                }

                m_pilateTimer += dt;
                if (m_pilateTimer >= 1.5f && m_pilateState == PilateState::KNEEL_REMORSE) {
                    m_pilateState = PilateState::WASH_HANDS_CELESTIAL;
                    AudioManager::PlaySfx(SFX_WATER_SPLASH);
                }
                if (m_pilateTimer >= 4.5f) {
                    m_pilateState = PilateState::DEFEATED_REDEEMED;
                }
                break;
            }

            m_facingLeft = (playerPos.x < m_position.x);
            float dist = fabsf(playerPos.x - m_position.x);

            switch (m_pilateState) {
                case PilateState::IDLE: {
                    m_attackHitboxActive = false;
                    if (dist > 70.0f) {
                        float dir = m_facingLeft ? -1.0f : 1.0f;
                        m_position.x += dir * 55.0f * dt;
                    }

                    if (m_position.x < level.GetArenaMinX() + 16.0f) m_position.x = level.GetArenaMinX() + 16.0f;
                    if (m_position.x > level.GetArenaMaxX() - 35.0f) m_position.x = level.GetArenaMaxX() - 35.0f;

                    if (m_animTimer >= 0.18f) {
                        m_animTimer = 0.0f;
                        m_animFrame = (m_animFrame + 1) % 2;
                    }

                    m_pilateTimer += dt;
                    if (m_pilateTimer >= 0.75f) {
                        m_pilateTimer = 0.0f;
                        m_pilateAttackPattern = (m_pilateAttackPattern + 1) % 4;

                        if (m_pilateAttackPattern == 0) {
                            m_pilateState = PilateState::BASIN_SPLASH;
                        } else if (m_pilateAttackPattern == 1) {
                            m_pilateState = PilateState::STYLUS_BEAM;
                        } else if (m_pilateAttackPattern == 2) {
                            m_pilateState = PilateState::SWORD_SLASH;
                            AudioManager::PlaySfx(SFX_MELEE_STRIKE);
                        } else {
                            m_pilateState = PilateState::TELEPORT_MIRAGE;
                            AudioManager::PlaySfx(SFX_CLONE_TELEPORT);
                            m_mirageTimer = 2.5f;
                            float aMin = level.GetArenaMinX() + 20.0f;
                            float aMax = level.GetArenaMaxX() - 40.0f;
                            m_pilateMirage1 = { aMin + 30.0f, GROUND_Y - 32.0f };
                            m_pilateMirage2 = { aMax - 30.0f, GROUND_Y - 32.0f };
                        }
                    }
                    break;
                }

                case PilateState::BASIN_SPLASH: {
                    m_pilateTimer += dt;
                    if (m_pilateTimer >= 0.15f && !m_justSplashedWater) {
                        m_justSplashedWater = true;
                        m_waterSplashPos = { m_position.x + (m_facingLeft ? -10.0f : 24.0f), m_position.y + 12.0f };
                        m_waterSplashDir = m_facingLeft ? -1.0f : 1.0f;
                        AudioManager::PlaySfx(SFX_WATER_SPLASH);
                    }
                    if (m_pilateTimer >= 0.45f) {
                        m_pilateState = PilateState::IDLE;
                        m_pilateTimer = 0.0f;
                        m_justSplashedWater = false;
                    }
                    break;
                }

                case PilateState::STYLUS_BEAM: {
                    m_pilateTimer += dt;
                    if (m_pilateTimer >= 0.18f && !m_justFiredStylus) {
                        m_justFiredStylus = true;
                        m_stylusPos = { m_position.x + (m_facingLeft ? -12.0f : 24.0f), GROUND_Y - 14.0f };
                        m_stylusDir = m_facingLeft ? -1.0f : 1.0f;
                        AudioManager::PlaySfx(SFX_PILATE_JUDGMENT);
                    }
                    if (m_pilateTimer >= 0.50f) {
                        m_pilateState = PilateState::IDLE;
                        m_pilateTimer = 0.0f;
                        m_justFiredStylus = false;
                    }
                    break;
                }

                case PilateState::SWORD_SLASH: {
                    m_pilateTimer += dt;
                    if (m_pilateTimer <= 0.26f) {
                        m_attackHitboxActive = true;
                        float sx = m_facingLeft ? (m_position.x - 18.0f) : (m_position.x + 22.0f);
                        m_attackHitbox = { sx, m_position.y + 6.0f, 26.0f, 22.0f };
                    } else {
                        m_attackHitboxActive = false;
                        if (m_pilateTimer >= 0.40f) {
                            m_pilateState = PilateState::IDLE;
                            m_pilateTimer = 0.0f;
                        }
                    }
                    break;
                }

                case PilateState::TELEPORT_MIRAGE: {
                    m_pilateTimer += dt;
                    if (m_pilateTimer >= 0.35f) {
                        m_pilateState = PilateState::IDLE;
                        m_pilateTimer = 0.0f;
                    }
                    break;
                }

                default:
                    m_pilateState = PilateState::IDLE;
                    break;
            }
            break;
        }
    }
}

void Enemy::Draw() {
    if (m_hp <= 0 && m_type != EnemyType::BOSS_GLADIATOR) return;
    if (m_whipState == BossWhipState::EXITED) return;

    bool hurtFlash = (m_hurtTimer > 0.0f);

    switch (m_type) {
        case EnemyType::PATROL_GUARD: {
            SpriteId spr = m_attackHitboxActive ? SPR_GUARD_ATTACK : (m_animFrame == 0 ? SPR_GUARD_WALK_0 : SPR_GUARD_WALK_1);
            if (hurtFlash) spr = SPR_GUARD_HURT;
            GraphicsManager::DrawDropShadow(m_position.x + 14.0f, m_position.y + 31.0f, 18.0f, 5.0f);
            GraphicsManager::DrawSprite(spr, m_position.x, m_position.y, m_facingLeft);
            break;
        }

        case EnemyType::ARCHER: {
            SpriteId spr = SPR_ARCHER_DUCK;
            if (!m_isDucking) {
                spr = m_isFiring ? SPR_ARCHER_FIRE : SPR_ARCHER_STAND;
            }
            if (hurtFlash) spr = SPR_ARCHER_HURT;
            GraphicsManager::DrawDropShadow(m_position.x + 14.0f, m_position.y + 31.0f, 18.0f, 5.0f);
            GraphicsManager::DrawSprite(spr, m_position.x, m_position.y, m_facingLeft);
            break;
        }

        case EnemyType::HEAVY_LEGIONARY: {
            SpriteId spr = !m_isShieldUp ? SPR_HEAVY_ATTACK : (m_animFrame == 0 ? SPR_HEAVY_WALK_0 : SPR_HEAVY_WALK_1);
            if (hurtFlash) spr = SPR_HEAVY_HURT;
            GraphicsManager::DrawDropShadow(m_position.x + 15.0f, m_position.y + 31.0f, 22.0f, 5.5f);
            GraphicsManager::DrawSprite(spr, m_position.x, m_position.y, m_facingLeft);
            break;
        }

        case EnemyType::BOSS_GLADIATOR: {
            if (m_whipState == BossWhipState::DEAD_WITHERED) return;

            SpriteId spr = SPR_BOSS_IDLE;
            if (m_whipState == BossWhipState::IDLE || m_whipState == BossWhipState::PHASE1_STANDBY || m_whipState == BossWhipState::PHASE1_PAUSE) {
                spr = (m_animFrame == 0 ? SPR_BOSS_WALK_0 : SPR_BOSS_WALK_1);
            } else if (m_whipState == BossWhipState::WINDUP || m_whipState == BossWhipState::PHASE1_WHIP_1_WINDUP || m_whipState == BossWhipState::PHASE1_WHIP_2_WINDUP) {
                spr = SPR_BOSS_WHIP_PREPARE;
            } else if (m_whipState == BossWhipState::HORIZONTAL_CRACK || m_whipState == BossWhipState::PHASE1_WHIP_1_CRACK || m_whipState == BossWhipState::PHASE1_WHIP_2_CRACK) {
                spr = SPR_BOSS_WHIP_CRACK;
            } else if (m_whipState == BossWhipState::OVERHEAD_SLAM) {
                spr = SPR_BOSS_WHIP_SLAM;
            } else if (m_whipState == BossWhipState::PHASE1_WALK_OFF || m_whipState == BossWhipState::WALK_OFF) {
                spr = (m_animFrame == 0 ? SPR_BOSS_WALK_0 : SPR_BOSS_WALK_1);
            } else if (m_whipState == BossWhipState::KNEEL) {
                spr = SPR_BOSS_KNEEL;
            } else if (m_whipState == BossWhipState::WITHERING) {
                spr = (m_witherTimer < 0.7f) ? SPR_BOSS_WITHER_0 : SPR_BOSS_WITHER_1;
            }

            if (hurtFlash && m_whipState != BossWhipState::KNEEL && m_whipState != BossWhipState::WITHERING) {
                spr = SPR_BOSS_HURT;
            }

            if (m_whipState != BossWhipState::WITHERING) {
                GraphicsManager::DrawDropShadow(m_position.x + 22.0f, m_position.y + 53.0f, 32.0f, 7.0f);
            }
            GraphicsManager::DrawSprite(spr, m_position.x, m_position.y, m_facingLeft);

            // Draw Whip Visual Extension during attacks (Thick, segmented, highly visible!)
            bool isCrack = (m_whipState == BossWhipState::HORIZONTAL_CRACK || m_whipState == BossWhipState::PHASE1_WHIP_1_CRACK || m_whipState == BossWhipState::PHASE1_WHIP_2_CRACK);
            if (isCrack) {
                float startX = m_facingLeft ? m_position.x + 4.0f : m_position.x + 38.0f;
                float endX = m_facingLeft ? m_position.x - 90.0f : m_position.x + 120.0f;
                float whipY = m_position.y + 20.0f;

                // Heavy braided leather body (5px thick) with bright glowing energy crack core (2.5px)
                DrawLineEx({ startX, whipY }, { endX, whipY }, 5.0f, Color{ 90, 50, 25, 255 });
                DrawLineEx({ startX, whipY }, { endX, whipY }, 2.5f, Color{ 255, 210, 60, 240 });

                // Segmented bronze studs along the whip
                float step = 14.0f;
                float minX = std::min(startX, endX);
                float maxX = std::max(startX, endX);
                for (float sx = minX + 6.0f; sx <= maxX - 6.0f; sx += step) {
                    DrawCircle(static_cast<int>(sx), static_cast<int>(whipY), 2.5f, COLOR_BRONZE);
                }

                // Heavy spiked steel morningstar head at tip
                GraphicsManager::DrawSprite(SPR_WHIP_HEAD, endX - 7.0f, whipY - 7.0f);

                // Sonic crack shockwave impact rings & sparks at tip!
                DrawCircleLines(static_cast<int>(endX), static_cast<int>(whipY), 9.0f, COLOR_GOLD_BRIGHT);
                DrawCircleLines(static_cast<int>(endX), static_cast<int>(whipY), 13.0f, Color{ 255, 255, 255, 200 });
                GraphicsManager::DrawSpark(endX, whipY - 4.0f, 2.5f, Color{ 255, 255, 255, 255 });
            } else if (m_whipState == BossWhipState::OVERHEAD_SLAM && m_attackHitboxActive) {
                float slamX = m_facingLeft ? m_position.x - 38.0f : m_position.x + 55.0f;
                Vector2 startP = { m_position.x + 18.0f, m_position.y + 6.0f };
                Vector2 endP = { slamX, GROUND_Y };

                // Heavy arcing whip slam (5px thick)
                DrawLineEx(startP, endP, 5.0f, Color{ 90, 50, 25, 255 });
                DrawLineEx(startP, endP, 2.5f, Color{ 255, 210, 60, 230 });
                GraphicsManager::DrawSprite(SPR_WHIP_HEAD, slamX - 7.0f, GROUND_Y - 10.0f);

                // Heavy ground impact shockwave & dust puff
                DrawCircle(static_cast<int>(slamX), static_cast<int>(GROUND_Y - 2.0f), 8.0f, Color{ 235, 220, 190, 210 });
                DrawCircleLines(static_cast<int>(slamX), static_cast<int>(GROUND_Y - 2.0f), 14.0f, COLOR_GOLD_BRIGHT);
                DrawCircleLines(static_cast<int>(slamX), static_cast<int>(GROUND_Y - 2.0f), 18.0f, Color{ 255, 255, 255, 160 });
            }
            break;
        }

        case EnemyType::SICARII_ASSASSIN: {
            SpriteId spr = SPR_SICARII_STAND;
            switch (m_sicariiState) {
                case SicariiState::WALL_CLING:
                    spr = SPR_SICARII_WALL_CLING;
                    break;
                case SicariiState::WALL_CRAWL:
                    spr = (m_animFrame == 0 ? SPR_SICARII_WALL_CRAWL_0 : SPR_SICARII_WALL_CRAWL_1);
                    break;
                case SicariiState::HOP:
                    spr = SPR_SICARII_HOP;
                    break;
                case SicariiState::STAND:
                    spr = SPR_SICARII_STAND;
                    break;
                case SicariiState::SLASH:
                    spr = SPR_SICARII_SLASH;
                    break;
                case SicariiState::THROW:
                    spr = SPR_SICARII_THROW;
                    break;
                case SicariiState::HURT:
                    spr = SPR_SICARII_HURT;
                    break;
            }
            if (hurtFlash) spr = SPR_SICARII_HURT;

            if (m_sicariiState != SicariiState::WALL_CLING && m_sicariiState != SicariiState::WALL_CRAWL) {
                GraphicsManager::DrawDropShadow(m_position.x + 14.0f, m_position.y + 31.0f, 18.0f, 5.0f);
            }
            GraphicsManager::DrawSprite(spr, m_position.x, m_position.y, m_facingLeft);

            // Double blade slash streak
            if (m_sicariiState == SicariiState::SLASH && m_attackHitboxActive) {
                float slashX = m_facingLeft ? m_position.x - 4.0f : m_position.x + 24.0f;
                float slashY = m_position.y + 14.0f;
                DrawCircleLines(static_cast<int>(slashX), static_cast<int>(slashY), 12.0f, COLOR_SICARII_BLADE);
                GraphicsManager::DrawSpark(slashX, slashY, 2.0f, Color{ 255, 255, 255, 220 });
            }
            break;
        }

        case EnemyType::BOSS_TEMPLE_COMMANDER: {
            if (m_whipState == BossWhipState::DEAD_WITHERED) return;

            SpriteId spr = SPR_TEMPLE_BOSS_IDLE;
            if (m_whipState == BossWhipState::IDLE || m_whipState == BossWhipState::PHASE1_STANDBY || m_whipState == BossWhipState::PHASE1_PAUSE) {
                spr = (m_animFrame == 0 ? SPR_TEMPLE_BOSS_WALK_0 : SPR_TEMPLE_BOSS_WALK_1);
            } else if (m_whipState == BossWhipState::WINDUP || m_whipState == BossWhipState::PHASE1_WHIP_1_WINDUP || m_whipState == BossWhipState::PHASE1_WHIP_2_WINDUP) {
                spr = SPR_TEMPLE_BOSS_SLASH;
            } else if (m_whipState == BossWhipState::HORIZONTAL_CRACK || m_whipState == BossWhipState::PHASE1_WHIP_1_CRACK || m_whipState == BossWhipState::PHASE1_WHIP_2_CRACK) {
                spr = SPR_TEMPLE_BOSS_SLAM;
            } else if (m_whipState == BossWhipState::OVERHEAD_SLAM) {
                spr = SPR_TEMPLE_BOSS_SLAM;
            } else if (m_whipState == BossWhipState::PHASE1_WALK_OFF || m_whipState == BossWhipState::WALK_OFF) {
                spr = (m_animFrame == 0 ? SPR_TEMPLE_BOSS_WALK_0 : SPR_TEMPLE_BOSS_WALK_1);
            } else if (m_whipState == BossWhipState::KNEEL || m_whipState == BossWhipState::WITHERING) {
                spr = SPR_TEMPLE_BOSS_KNEEL;
            }

            if (hurtFlash && m_whipState != BossWhipState::KNEEL && m_whipState != BossWhipState::WITHERING) {
                spr = SPR_TEMPLE_BOSS_HURT;
            }

            if (m_whipState != BossWhipState::WITHERING) {
                GraphicsManager::DrawDropShadow(m_position.x + 22.0f, m_position.y + 53.0f, 32.0f, 7.0f);
            }
            GraphicsManager::DrawSprite(spr, m_position.x, m_position.y, m_facingLeft);

            // High golden ceremonial slash gleam
            if (m_attackHitboxActive) {
                float fx = m_facingLeft ? m_position.x - 12.0f : m_position.x + 40.0f;
                DrawCircleLines(static_cast<int>(fx), static_cast<int>(m_position.y + 24.0f), 16.0f, COLOR_TEMPLE_GOLD);
                GraphicsManager::DrawSpark(fx, m_position.y + 20.0f, 3.0f, Color{ 255, 255, 255, 255 });
            }
            break;
        }

        case EnemyType::BOSS_TRIBUNAL_PRAETOR: {
            if (m_whipState == BossWhipState::DEAD_WITHERED) return;

            SpriteId spr = SPR_TRIBUNAL_IDLE;
            if (m_whipState == BossWhipState::IDLE || m_whipState == BossWhipState::PHASE1_STANDBY || m_whipState == BossWhipState::PHASE1_PAUSE) {
                spr = (m_animFrame == 0 ? SPR_TRIBUNAL_WALK_0 : SPR_TRIBUNAL_WALK_1);
            } else if (m_whipState == BossWhipState::WINDUP) {
                spr = SPR_TRIBUNAL_SLASH;
            } else if (m_whipState == BossWhipState::HORIZONTAL_CRACK) {
                spr = SPR_TRIBUNAL_SLASH;
            } else if (m_whipState == BossWhipState::OVERHEAD_SLAM) {
                spr = SPR_TRIBUNAL_SLAM;
            } else if (m_whipState == BossWhipState::KNEEL || m_whipState == BossWhipState::WITHERING) {
                spr = SPR_TRIBUNAL_KNEEL;
            }

            if (hurtFlash && m_whipState != BossWhipState::KNEEL && m_whipState != BossWhipState::WITHERING) {
                spr = SPR_TRIBUNAL_HURT;
            }

            if (m_whipState != BossWhipState::WITHERING) {
                GraphicsManager::DrawDropShadow(m_position.x + 22.0f, m_position.y + 53.0f, 32.0f, 7.0f);
            }
            GraphicsManager::DrawSprite(spr, m_position.x, m_position.y, m_facingLeft);

            // Imperial energy arc on attacks
            if (m_attackHitboxActive) {
                float fx = m_facingLeft ? m_position.x - 14.0f : m_position.x + 42.0f;
                DrawCircleLines(static_cast<int>(fx), static_cast<int>(m_position.y + 22.0f), 18.0f, COLOR_GOLD_BRIGHT);
                DrawCircleLines(static_cast<int>(fx), static_cast<int>(m_position.y + 22.0f), 12.0f, COLOR_ROMAN_CRIMSON);
                GraphicsManager::DrawSpark(fx, m_position.y + 18.0f, 3.5f, Color{ 255, 255, 255, 255 });
            }
            break;
        }

        case EnemyType::TORMENTOR_GUARD: {
            SpriteId spr = m_attackHitboxActive ? SPR_TORMENTOR_ATTACK : (m_animFrame == 0 ? SPR_TORMENTOR_WALK_0 : SPR_TORMENTOR_WALK_1);
            if (hurtFlash) spr = SPR_TORMENTOR_HURT;
            GraphicsManager::DrawDropShadow(m_position.x + 14.0f, m_position.y + 31.0f, 18.0f, 5.0f);
            GraphicsManager::DrawSprite(spr, m_position.x, m_position.y, m_facingLeft);

            // Blazing torch flame glow
            float torchX = m_facingLeft ? m_position.x - 4.0f : m_position.x + 24.0f;
            float torchY = m_position.y + 8.0f;
            DrawCircle(static_cast<int>(torchX), static_cast<int>(torchY), 5.0f, Color{ 255, 160, 40, 180 });
            GraphicsManager::DrawSpark(torchX, torchY, 2.0f, Color{ 255, 230, 80, 220 });
            break;
        }

        case EnemyType::GARDEN_THORN_SICARII: {
            SpriteId spr = SPR_THORN_SICARII_CROUCH;
            if (m_thornState == ThornSicariiState::LEAP) {
                spr = SPR_THORN_SICARII_LEAP;
            } else if (m_thornState == ThornSicariiState::SLASH) {
                spr = SPR_THORN_SICARII_SLASH;
            }
            if (hurtFlash) spr = SPR_THORN_SICARII_HURT;

            if (m_thornState != ThornSicariiState::LEAP) {
                GraphicsManager::DrawDropShadow(m_position.x + 14.0f, m_position.y + 31.0f, 18.0f, 5.0f);
            }
            GraphicsManager::DrawSprite(spr, m_position.x, m_position.y, m_facingLeft);

            // Thorn blade gleam on slash
            if (m_attackHitboxActive) {
                float sx = m_facingLeft ? m_position.x - 8.0f : m_position.x + 24.0f;
                DrawCircleLines(static_cast<int>(sx), static_cast<int>(m_position.y + 14.0f), 12.0f, Color{ 80, 240, 120, 220 });
                GraphicsManager::DrawSpark(sx, m_position.y + 12.0f, 2.0f, Color{ 255, 255, 255, 220 });
            }
            break;
        }

        case EnemyType::CORRUPTED_PRIEST: {
            SpriteId spr = (m_castCooldown <= 0.6f) ? SPR_CORRUPTED_PRIEST_CAST : SPR_CORRUPTED_PRIEST_STAND;
            if (hurtFlash) spr = SPR_CORRUPTED_PRIEST_HURT;

            GraphicsManager::DrawDropShadow(m_position.x + 14.0f, m_position.y + 31.0f, 18.0f, 5.0f);
            GraphicsManager::DrawSprite(spr, m_position.x, m_position.y, m_facingLeft);

            // Dark magic casting orb aura
            if (m_castCooldown <= 0.6f) {
                float orbX = m_facingLeft ? m_position.x - 4.0f : m_position.x + 22.0f;
                float orbY = m_position.y + 8.0f;
                DrawCircle(static_cast<int>(orbX), static_cast<int>(orbY), 4.0f, Color{ 140, 40, 190, 200 });
                DrawCircleLines(static_cast<int>(orbX), static_cast<int>(orbY), 7.0f, Color{ 220, 100, 255, 230 });
            }
            break;
        }

        case EnemyType::BOSS_EVIL_CLONE: {
            if (m_cloneState == CloneState::DEAD_WITHERED) return;

            SpriteId spr = SPR_CLONE_IDLE;
            if (m_cloneState == CloneState::IDLE) {
                spr = (m_animFrame == 0 ? SPR_CLONE_WALK_0 : SPR_CLONE_WALK_1);
            } else if (m_cloneState == CloneState::THROW_STAND) {
                spr = SPR_CLONE_THROW_STAND;
            } else if (m_cloneState == CloneState::THROW_CROUCH) {
                spr = SPR_CLONE_THROW_CROUCH;
            } else if (m_cloneState == CloneState::JUMP_LEAP) {
                spr = SPR_CLONE_JUMP;
            } else if (m_cloneState == CloneState::CHANNEL_WRATH) {
                spr = SPR_CLONE_CHANNEL;
            } else if (m_cloneState == CloneState::MELEE_STRIKE) {
                spr = SPR_CLONE_MELEE_STRIKE;
            } else if (m_cloneState == CloneState::KNEEL || m_cloneState == CloneState::WITHERING) {
                spr = SPR_CLONE_KNEEL;
            }

            if (hurtFlash && m_cloneState != CloneState::KNEEL && m_cloneState != CloneState::WITHERING) {
                spr = SPR_CLONE_HURT;
            }

            // Drop shadow
            if (m_cloneState != CloneState::JUMP_LEAP && m_cloneState != CloneState::WITHERING) {
                GraphicsManager::DrawDropShadow(m_position.x + 14.0f, m_position.y + 31.0f, 22.0f, 5.0f);
            }

            // Draw Dark Aura around Clone
            if (m_cloneState != CloneState::WITHERING && m_cloneState != CloneState::KNEEL) {
                float pulse = 0.5f + 0.5f * sinf(static_cast<float>(GetTime()) * 12.0f);
                Color auraCol = ColorAlpha(Color{ 130, 25, 180, 255 }, 0.25f + pulse * 0.25f);
                DrawRectangleLines(static_cast<int>(m_position.x + 2.0f), static_cast<int>(m_position.y + 2.0f), 24, 28, auraCol);
            }

            GraphicsManager::DrawSprite(spr, m_position.x, m_position.y, m_facingLeft);

            // Channeling Wrath dark energy rings
            if (m_cloneState == CloneState::CHANNEL_WRATH) {
                float pulseR = 12.0f + 16.0f * sinf(static_cast<float>(GetTime()) * 16.0f);
                DrawCircleLines(static_cast<int>(m_position.x + 14.0f), static_cast<int>(m_position.y + 16.0f), fabsf(pulseR), Color{ 200, 60, 255, 230 });
                DrawCircleLines(static_cast<int>(m_position.x + 14.0f), static_cast<int>(m_position.y + 16.0f), fabsf(pulseR) * 0.6f, Color{ 255, 50, 100, 200 });
                GraphicsManager::DrawSpark(m_position.x + 14.0f, m_position.y + 10.0f, 3.0f, Color{ 255, 200, 255, 255 });
            }

            // Melee strike streak
            if (m_cloneState == CloneState::MELEE_STRIKE && m_attackHitboxActive) {
                float mx = m_facingLeft ? m_position.x - 8.0f : m_position.x + 26.0f;
                DrawCircleLines(static_cast<int>(mx), static_cast<int>(m_position.y + 14.0f), 14.0f, Color{ 180, 30, 80, 240 });
            }
            break;
        }

        case EnemyType::BOSS_MISSION5_CLONE_GREEN: {
            if (m_cloneState == CloneState::DEAD_WITHERED) return;

            SpriteId spr = SPR_CLONE_GREEN_IDLE;
            if (m_cloneState == CloneState::IDLE) {
                spr = (m_animFrame == 0 ? SPR_CLONE_GREEN_WALK_0 : SPR_CLONE_GREEN_IDLE);
            } else if (m_cloneState == CloneState::THROW_STAND || m_cloneState == CloneState::THROW_CROUCH) {
                spr = SPR_CLONE_GREEN_ATTACK;
            } else if (m_cloneState == CloneState::JUMP_LEAP) {
                spr = SPR_CLONE_GREEN_WALK_0;
            } else if (m_cloneState == CloneState::CHANNEL_WRATH) {
                spr = SPR_CLONE_GREEN_ATTACK;
            } else if (m_cloneState == CloneState::MELEE_STRIKE) {
                spr = SPR_CLONE_GREEN_ATTACK;
            } else if (m_cloneState == CloneState::KNEEL || m_cloneState == CloneState::WITHERING) {
                spr = SPR_CLONE_GREEN_KNEEL;
            }

            if (hurtFlash && m_cloneState != CloneState::KNEEL && m_cloneState != CloneState::WITHERING) {
                spr = SPR_CLONE_GREEN_HURT;
            }

            // Drop shadow
            if (m_cloneState != CloneState::JUMP_LEAP && m_cloneState != CloneState::WITHERING) {
                GraphicsManager::DrawDropShadow(m_position.x + 14.0f, m_position.y + 31.0f, 22.0f, 5.0f);
            }

            // Glowing Emerald Hellfire Aura around Green Clone
            if (m_cloneState != CloneState::WITHERING && m_cloneState != CloneState::KNEEL) {
                float pulse = 0.5f + 0.5f * sinf(static_cast<float>(GetTime()) * 14.0f);
                Color auraCol = ColorAlpha(Color{ 40, 255, 90, 255 }, 0.35f + pulse * 0.35f);
                DrawRectangleLines(static_cast<int>(m_position.x + 1.0f), static_cast<int>(m_position.y + 1.0f), 26, 30, auraCol);
                GraphicsManager::DrawSpark(m_position.x + 14.0f + 10.0f * sinf(static_cast<float>(GetTime()) * 8.0f), m_position.y + 6.0f, 2.5f, Color{ 140, 255, 170, 240 });
            }

            GraphicsManager::DrawSprite(spr, m_position.x, m_position.y, m_facingLeft);

            // Channeling Wrath emerald flame ring
            if (m_cloneState == CloneState::CHANNEL_WRATH) {
                float pulseR = 14.0f + 14.0f * sinf(static_cast<float>(GetTime()) * 18.0f);
                DrawCircleLines(static_cast<int>(m_position.x + 14.0f), static_cast<int>(m_position.y + 16.0f), fabsf(pulseR), Color{ 50, 255, 100, 240 });
                GraphicsManager::DrawSpark(m_position.x + 14.0f, m_position.y + 10.0f, 3.5f, Color{ 200, 255, 210, 255 });
            }

            if (m_cloneState == CloneState::MELEE_STRIKE && m_attackHitboxActive) {
                float mx = m_facingLeft ? m_position.x - 8.0f : m_position.x + 26.0f;
                DrawCircleLines(static_cast<int>(mx), static_cast<int>(m_position.y + 14.0f), 15.0f, Color{ 40, 240, 90, 250 });
            }
            break;
        }

        case EnemyType::BOSS_DEVIL: {
            if (m_devilState == DevilState::DEFEATED && m_witherTimer >= 1.9f) return;

            SpriteId spr = SPR_DEVIL_IDLE;
            if (m_devilState == DevilState::IDLE) {
                spr = (m_animFrame == 0 ? SPR_DEVIL_WALK_0 : SPR_DEVIL_IDLE);
            } else if (m_devilState == DevilState::TRIDENT_THRUST || m_devilState == DevilState::FIRE_SURGE) {
                spr = SPR_DEVIL_ATTACK;
            } else if (m_devilState == DevilState::TAIL_WHIP_WINDUP || m_devilState == DevilState::TAIL_WHIP_STRIKE) {
                spr = SPR_DEVIL_TAIL_WHIP;
            } else if (m_devilState == DevilState::DEFEATED) {
                spr = SPR_DEVIL_DIE;
            }

            if (hurtFlash && m_devilState != DevilState::DEFEATED) {
                spr = SPR_DEVIL_HURT;
            }

            if (m_devilState != DevilState::DEFEATED) {
                GraphicsManager::DrawDropShadow(m_position.x + 16.0f, m_position.y + 35.0f, 26.0f, 6.0f);
            }

            // Devil Hellfire Aura
            if (m_devilState != DevilState::DEFEATED) {
                float pulse = 0.5f + 0.5f * sinf(static_cast<float>(GetTime()) * 10.0f);
                Color fireCol = ColorAlpha(Color{ 230, 40, 20, 255 }, 0.25f + pulse * 0.25f);
                DrawRectangleLines(static_cast<int>(m_position.x + 2.0f), static_cast<int>(m_position.y + 2.0f), 28, 34, fireCol);
                GraphicsManager::DrawSpark(m_position.x + 16.0f + 12.0f * sinf(static_cast<float>(GetTime()) * 6.0f), m_position.y + 4.0f, 2.5f, Color{ 255, 140, 40, 230 });
            }

            GraphicsManager::DrawSprite(spr, m_position.x, m_position.y, m_facingLeft);

            // Secret Tail Whip Attack visual slash streak
            if (m_devilState == DevilState::TAIL_WHIP_STRIKE && m_attackHitboxActive) {
                float wx1 = m_facingLeft ? (m_position.x + 4.0f) : (m_position.x + 24.0f);
                float wx2 = m_facingLeft ? (m_position.x - 50.0f) : (m_position.x + 70.0f);
                DrawLineEx({ wx1, m_position.y + 22.0f }, { wx2, m_position.y + 22.0f }, 4.0f, Color{ 240, 30, 20, 240 });
                DrawLineEx({ wx1, m_position.y + 20.0f }, { wx2, m_position.y + 20.0f }, 2.0f, Color{ 255, 200, 50, 255 });
                GraphicsManager::DrawSpark(wx2, m_position.y + 21.0f, 4.0f, Color{ 255, 230, 100, 255 });
            }

            // Trident thrust gleam
            if (m_devilState == DevilState::TRIDENT_THRUST && m_attackHitboxActive) {
                float tx = m_facingLeft ? (m_position.x - 20.0f) : (m_position.x + 36.0f);
                GraphicsManager::DrawSpark(tx, m_position.y + 14.0f, 3.5f, Color{ 255, 210, 60, 255 });
            }

            // Defeat explosion embers
            if (m_devilState == DevilState::DEFEATED) {
                float emberT = m_witherTimer * 5.0f;
                DrawCircle(static_cast<int>(m_position.x + 16.0f), static_cast<int>(m_position.y + 20.0f), 6.0f + emberT * 8.0f, ColorAlpha(Color{ 255, 60, 20, 200 }, 1.0f - m_witherTimer * 0.5f));
                GraphicsManager::DrawSpark(m_position.x + 16.0f, m_position.y + 12.0f, 5.0f, Color{ 255, 220, 80, 255 });
            }
            break;
        }

        case EnemyType::MONEY_CHANGER: {
            SpriteId spr = m_moneyChangerAttacking ? SPR_MONEY_CHANGER_ATTACK : (m_animFrame == 0 ? SPR_MONEY_CHANGER_WALK_0 : SPR_MONEY_CHANGER_WALK_1);
            if (hurtFlash) spr = SPR_MONEY_CHANGER_HURT;
            GraphicsManager::DrawDropShadow(m_position.x + 12.0f, m_position.y + 31.0f, 18.0f, 5.0f);
            GraphicsManager::DrawSprite(spr, m_position.x, m_position.y, m_facingLeft);
            break;
        }

        case EnemyType::LEVITE_ENFORCER: {
            SpriteId spr = m_enforcerAttacking ? SPR_LEVITE_ENFORCER_ATTACK : (m_animFrame == 0 ? SPR_LEVITE_ENFORCER_WALK_0 : SPR_LEVITE_ENFORCER_WALK_1);
            if (hurtFlash) spr = SPR_LEVITE_ENFORCER_HURT;
            GraphicsManager::DrawDropShadow(m_position.x + 12.0f, m_position.y + 31.0f, 18.0f, 5.0f);
            GraphicsManager::DrawSprite(spr, m_position.x, m_position.y, m_facingLeft);
            break;
        }

        case EnemyType::BOSS_BARABBAS: {
            if (m_whipState == BossWhipState::DEAD_WITHERED) return;

            SpriteId spr = SPR_BARABBAS_IDLE;
            if (m_whipState == BossWhipState::IDLE) {
                spr = (m_animFrame == 0 ? SPR_BARABBAS_WALK_0 : SPR_BARABBAS_WALK_1);
            } else if (m_whipState == BossWhipState::HORIZONTAL_CRACK) {
                spr = SPR_BARABBAS_SLASH;
            } else if (m_whipState == BossWhipState::OVERHEAD_SLAM) {
                spr = SPR_BARABBAS_SLAM;
            } else if (m_whipState == BossWhipState::KNEEL || m_whipState == BossWhipState::WITHERING) {
                spr = SPR_BARABBAS_KNEEL;
            }

            if (hurtFlash && m_whipState != BossWhipState::KNEEL && m_whipState != BossWhipState::WITHERING) {
                spr = SPR_BARABBAS_HURT;
            }

            GraphicsManager::DrawDropShadow(m_position.x + 16.0f, m_position.y + 35.0f, 24.0f, 6.0f);
            GraphicsManager::DrawSprite(spr, m_position.x, m_position.y, m_facingLeft);

            // Double slash crescent effect
            if (m_whipState == BossWhipState::HORIZONTAL_CRACK && m_attackHitboxActive) {
                float sx = m_facingLeft ? m_position.x - 10.0f : m_position.x + 30.0f;
                DrawCircleLines(static_cast<int>(sx), static_cast<int>(m_position.y + 14.0f), 16.0f, Color{ 230, 45, 45, 230 });
            }
            break;
        }

        case EnemyType::BOSS_CAIAPHAS: {
            if (m_whipState == BossWhipState::DEAD_WITHERED) return;

            SpriteId spr = SPR_CAIAPHAS_STAND;
            if (m_whipState == BossWhipState::OVERHEAD_SLAM) {
                spr = SPR_CAIAPHAS_CAST;
            } else if (m_whipState == BossWhipState::KNEEL || m_whipState == BossWhipState::WITHERING) {
                spr = SPR_CAIAPHAS_KNEEL;
            }

            if (hurtFlash && m_whipState != BossWhipState::KNEEL && m_whipState != BossWhipState::WITHERING) {
                spr = SPR_CAIAPHAS_HURT;
            }

            GraphicsManager::DrawDropShadow(m_position.x + 16.0f, m_position.y + 35.0f, 24.0f, 6.0f);
            GraphicsManager::DrawSprite(spr, m_position.x, m_position.y, m_facingLeft);

            // Radiant sacred sparks when casting lightning
            if (m_whipState == BossWhipState::OVERHEAD_SLAM) {
                float pulse = 0.5f + 0.5f * sinf(static_cast<float>(GetTime()) * 20.0f);
                DrawCircleLines(static_cast<int>(m_position.x + 16.0f), static_cast<int>(m_position.y + 4.0f), 12.0f + pulse * 6.0f, COLOR_GOLD_BRIGHT);
                GraphicsManager::DrawSpark(m_position.x + 16.0f, m_position.y + 2.0f, 3.5f, WHITE);
            }
            break;
        }

        case EnemyType::BOSS_JUDAS: {
            if (m_judasState == JudasState::DEAD_WITHERED) return;

            SpriteId spr = SPR_JUDAS_IDLE;
            if (m_judasState == JudasState::IDLE) {
                spr = (m_animFrame == 0 ? SPR_JUDAS_WALK_0 : SPR_JUDAS_WALK_1);
            } else if (m_judasState == JudasState::TOSS_SILVER || m_judasState == JudasState::MONEY_SLAM) {
                spr = SPR_JUDAS_TOSS_SILVER;
            } else if (m_judasState == JudasState::SHADOW_DASH) {
                spr = SPR_JUDAS_DASH;
            } else if (m_judasState == JudasState::DAGGER_STAB) {
                spr = SPR_JUDAS_STAB;
            } else if (m_judasState == JudasState::KNEEL || m_judasState == JudasState::WITHERING) {
                spr = SPR_JUDAS_KNEEL;
            }

            if (hurtFlash && m_judasState != JudasState::KNEEL && m_judasState != JudasState::WITHERING) {
                spr = SPR_JUDAS_HURT;
            }

            // Shadow dash motion lines
            if (m_judasState == JudasState::SHADOW_DASH) {
                float dir = m_facingLeft ? 1.0f : -1.0f;
                DrawLineEx({ m_position.x + dir * 18.0f, m_position.y + 16.0f }, { m_position.x, m_position.y + 16.0f }, 3.0f, Color{ 40, 50, 35, 180 });
                DrawLineEx({ m_position.x + dir * 26.0f, m_position.y + 24.0f }, { m_position.x + dir * 6.0f, m_position.y + 24.0f }, 2.0f, Color{ 140, 110, 40, 150 });
            }

            GraphicsManager::DrawDropShadow(m_position.x + 16.0f, m_position.y + 35.0f, 24.0f, 6.0f);
            GraphicsManager::DrawSprite(spr, m_position.x, m_position.y, m_facingLeft);

            // Silver coin sparkle on toss
            if (m_judasState == JudasState::TOSS_SILVER) {
                float handX = m_facingLeft ? m_position.x - 4.0f : m_position.x + 28.0f;
                GraphicsManager::DrawSpark(handX, m_position.y + 14.0f, 3.0f, COLOR_SILVER_SHEEN);
                GraphicsManager::DrawSpark(handX + 4.0f, m_position.y + 10.0f, 2.0f, WHITE);
            }

            // Dagger thrust gleam
            if (m_judasState == JudasState::DAGGER_STAB && m_attackHitboxActive) {
                float dx = m_facingLeft ? m_position.x - 12.0f : m_position.x + 32.0f;
                GraphicsManager::DrawSpark(dx, m_position.y + 14.0f, 3.5f, WHITE);
            }
            break;
        }

        case EnemyType::SIGNIFER_GUARD: {
            SpriteId spr = (m_signiferRallyTimer <= 0.4f && m_attackHitboxActive) ? SPR_SIGNIFER_ATTACK : (m_animFrame == 0 ? SPR_SIGNIFER_WALK_0 : SPR_SIGNIFER_WALK_1);
            if (hurtFlash) spr = SPR_SIGNIFER_HURT;
            GraphicsManager::DrawDropShadow(m_position.x + 14.0f, m_position.y + 31.0f, 18.0f, 5.0f);
            GraphicsManager::DrawSprite(spr, m_position.x, m_position.y, m_facingLeft);
            break;
        }

        case EnemyType::PRAETORIAN_GUARD: {
            SpriteId spr = !m_isShieldUp ? SPR_PRAETORIAN_SHIELD_BASH : (m_animFrame == 0 ? SPR_PRAETORIAN_WALK_0 : SPR_PRAETORIAN_WALK_1);
            if (hurtFlash) spr = SPR_PRAETORIAN_HURT;
            GraphicsManager::DrawDropShadow(m_position.x + 15.0f, m_position.y + 31.0f, 22.0f, 5.5f);
            GraphicsManager::DrawSprite(spr, m_position.x, m_position.y, m_facingLeft);
            break;
        }

        case EnemyType::IMPERIAL_LICTOR: {
            SpriteId spr = m_lictorSwinging ? SPR_LICTOR_FASCES_SWING : (m_animFrame == 0 ? SPR_LICTOR_WALK_0 : SPR_LICTOR_WALK_1);
            if (hurtFlash) spr = SPR_LICTOR_HURT;
            GraphicsManager::DrawDropShadow(m_position.x + 14.0f, m_position.y + 31.0f, 18.0f, 5.0f);
            GraphicsManager::DrawSprite(spr, m_position.x, m_position.y, m_facingLeft);
            break;
        }

        case EnemyType::BOSS_PREFECT_QUINTUS: {
            if (m_whipState == BossWhipState::DEAD_WITHERED) return;

            SpriteId spr = SPR_PREFECT_IDLE;
            if (m_whipState == BossWhipState::IDLE) {
                spr = (m_animFrame == 0 ? SPR_PREFECT_WALK_0 : SPR_PREFECT_WALK_1);
            } else if (m_whipState == BossWhipState::HORIZONTAL_CRACK) {
                spr = SPR_PREFECT_SLASH;
            } else if (m_whipState == BossWhipState::WINDUP) {
                spr = SPR_PREFECT_SHIELD_RUSH;
            } else if (m_whipState == BossWhipState::OVERHEAD_SLAM) {
                spr = SPR_PREFECT_SLASH;
            } else if (m_whipState == BossWhipState::KNEEL || m_whipState == BossWhipState::WITHERING) {
                spr = SPR_PREFECT_KNEEL;
            }

            if (hurtFlash && m_whipState != BossWhipState::KNEEL && m_whipState != BossWhipState::WITHERING) {
                spr = SPR_PREFECT_HURT;
            }

            if (m_prefectRushing) {
                float dir = m_facingLeft ? 1.0f : -1.0f;
                DrawLineEx({ m_position.x + dir * 16.0f, m_position.y + 18.0f }, { m_position.x, m_position.y + 18.0f }, 3.0f, Color{ 220, 180, 80, 190 });
            }

            GraphicsManager::DrawDropShadow(m_position.x + 16.0f, m_position.y + 35.0f, 26.0f, 6.0f);
            GraphicsManager::DrawSprite(spr, m_position.x, m_position.y, m_facingLeft);
            break;
        }

        case EnemyType::BOSS_ADVOCATE_TERTULLUS: {
            if (m_whipState == BossWhipState::DEAD_WITHERED) return;

            SpriteId spr = SPR_ADVOCATE_IDLE;
            if (m_whipState == BossWhipState::IDLE) {
                spr = (m_animFrame == 0 ? SPR_ADVOCATE_IDLE : SPR_ADVOCATE_CAST);
            } else if (m_whipState == BossWhipState::HORIZONTAL_CRACK) {
                spr = SPR_ADVOCATE_SHOUT;
            } else if (m_whipState == BossWhipState::WINDUP) {
                spr = SPR_ADVOCATE_CAST;
            } else if (m_whipState == BossWhipState::OVERHEAD_SLAM) {
                spr = SPR_ADVOCATE_SHOUT;
            } else if (m_whipState == BossWhipState::KNEEL || m_whipState == BossWhipState::WITHERING) {
                spr = SPR_ADVOCATE_KNEEL;
            }

            if (hurtFlash && m_whipState != BossWhipState::KNEEL && m_whipState != BossWhipState::WITHERING) {
                spr = SPR_ADVOCATE_HURT;
            }

            GraphicsManager::DrawDropShadow(m_position.x + 16.0f, m_position.y + 35.0f, 24.0f, 6.0f);
            GraphicsManager::DrawSprite(spr, m_position.x, m_position.y, m_facingLeft);
            break;
        }

        case EnemyType::BOSS_PONTIUS_PILATE: {
            if (m_pilateState == PilateState::DEFEATED_REDEEMED) return;

            if (m_mirageTimer > 0.0f) {
                float mirageAlpha = 0.65f + 0.2f * sinf(m_mirageTimer * 12.0f);
                Color mirageTint = ColorAlpha(Color{ 180, 220, 255, 255 }, mirageAlpha);
                SpriteId mSpr = (m_animFrame == 0 ? SPR_PILATE_WALK_0 : SPR_PILATE_WALK_1);

                GraphicsManager::DrawDropShadow(m_pilateMirage1.x + 16.0f, m_pilateMirage1.y + 35.0f, 24.0f, 6.0f);
                GraphicsManager::DrawSprite(mSpr, m_pilateMirage1.x, m_pilateMirage1.y, m_facingLeft, mirageTint);

                GraphicsManager::DrawDropShadow(m_pilateMirage2.x + 16.0f, m_pilateMirage2.y + 35.0f, 24.0f, 6.0f);
                GraphicsManager::DrawSprite(mSpr, m_pilateMirage2.x, m_pilateMirage2.y, !m_facingLeft, mirageTint);
            }

            SpriteId spr = SPR_PILATE_IDLE;
            if (m_pilateState == PilateState::IDLE || m_pilateState == PilateState::APPROACH) {
                spr = (m_animFrame == 0 ? SPR_PILATE_WALK_0 : SPR_PILATE_WALK_1);
            } else if (m_pilateState == PilateState::BASIN_SPLASH) {
                spr = SPR_PILATE_BASIN_SPLASH;
            } else if (m_pilateState == PilateState::STYLUS_BEAM) {
                spr = SPR_PILATE_STYLUS_BEAM;
            } else if (m_pilateState == PilateState::SWORD_SLASH) {
                spr = SPR_PILATE_SLASH;
            } else if (m_pilateState == PilateState::TELEPORT_MIRAGE) {
                spr = SPR_PILATE_TELEPORT;
            } else if (m_pilateState == PilateState::KNEEL_REMORSE || m_pilateState == PilateState::WASH_HANDS_CELESTIAL) {
                spr = SPR_PILATE_KNEEL_WASH;
            }

            if (hurtFlash && m_pilateState != PilateState::KNEEL_REMORSE && m_pilateState != PilateState::WASH_HANDS_CELESTIAL) {
                spr = SPR_PILATE_HURT;
            }

            if (m_pilateState == PilateState::WASH_HANDS_CELESTIAL) {
                float haloPulse = 1.0f + 0.15f * sinf(m_pilateTimer * 8.0f);
                float haloR = 24.0f * haloPulse;
                DrawCircleLines(static_cast<int>(m_position.x + 16.0f), static_cast<int>(m_position.y + 16.0f), haloR, COLOR_GOLD_BRIGHT);
                DrawCircleLines(static_cast<int>(m_position.x + 16.0f), static_cast<int>(m_position.y + 16.0f), haloR + 6.0f, Color{ 255, 255, 230, 180 });
                GraphicsManager::DrawSpark(m_position.x + 16.0f, m_position.y + 12.0f, 4.0f, COLOR_GOLD_BRIGHT);
            }

            GraphicsManager::DrawDropShadow(m_position.x + 16.0f, m_position.y + 35.0f, 26.0f, 6.0f);
            GraphicsManager::DrawSprite(spr, m_position.x, m_position.y, m_facingLeft);
            break;
        }
    }
}

// --- EnemyManager Implementation ---
EnemyManager::EnemyManager() {
    Reset(1);
}

void EnemyManager::Reset(int stageId) {
    m_enemies.clear();
    m_arrows.clear();
    m_daggers.clear();
    m_darkProjectiles.clear();
    m_silverCoins.clear();
    m_bloodShockwaves.clear();
    m_lightningStrikes.clear();
    m_waterDroplets.clear();
    m_stylusBlades.clear();
    m_shoutWaves.clear();
    m_particles.clear();
    SpawnStageEnemies(stageId);
}

void EnemyManager::SpawnBossDirect(EnemyType type, Vector2 pos, float minX, float maxX) {
    m_enemies.emplace_back(type, pos, minX, maxX);
}

void EnemyManager::ClearAllEnemies() {
    m_enemies.clear();
    m_arrows.clear();
    m_daggers.clear();
    m_darkProjectiles.clear();
    m_silverCoins.clear();
    m_bloodShockwaves.clear();
    m_lightningStrikes.clear();
    m_waterDroplets.clear();
    m_stylusBlades.clear();
    m_shoutWaves.clear();
}

void EnemyManager::SpawnStageEnemies(int stageId) {
    m_enemies.clear();
    m_arrows.clear();
    m_daggers.clear();
    m_darkProjectiles.clear();
    m_silverCoins.clear();
    m_bloodShockwaves.clear();
    m_lightningStrikes.clear();
    m_waterDroplets.clear();
    m_stylusBlades.clear();
    m_shoutWaves.clear();

    if (stageId == 13) {
        // =========================================================================
        // STAGE 13: GOLGOTHA SUMMIT / MOUNT CALVARY (Single-board Climax Boss Arena)
        // 1. Evil Clone Jesus (Glowing Green Emerald Hellfire)
        // 2. The Devil (Secret demon tail whip attack)
        // 3. Hallucination Pilate (Disagreement with false mercy)
        // =========================================================================
        m_enemies.emplace_back(EnemyType::BOSS_MISSION5_CLONE_GREEN, Vector2{ 360.0f, GROUND_Y - 32.0f }, 40.0f, 440.0f);
    } else if (stageId == 2) {
        // =========================================================================
        // STAGE 2: INSIDE THE TEMPLE OF JERUSALEM
        // Features Sicarii Shadow Assassins crawling from columns/walls and hopping
        // across 2-level stacked platforms (TEMPLE_TIER_1_Y and TEMPLE_TIER_2_Y)
        // =========================================================================

        // 1. Sicarii Assassin on Entrance Pillar (Clings to column, crawls down)
        m_enemies.emplace_back(EnemyType::SICARII_ASSASSIN, Vector2{ 220.0f, 48.0f }, 150.0f, 320.0f);

        // 2. Heavy Temple Guard (Patrols ground floor before Tier 1 Balcony)
        m_enemies.emplace_back(EnemyType::HEAVY_LEGIONARY, Vector2{ 380.0f, GROUND_Y - 32.0f }, 320.0f, 440.0f);

        // 3. High Gallery Archer (Standing on Tier 2 Gallery at Y = 96)
        m_enemies.emplace_back(EnemyType::ARCHER, Vector2{ 560.0f, TEMPLE_TIER_2_Y - 30.0f }, 560.0f, 560.0f);

        // 4. Sicarii Assassin on Column between Balconies (Crawls down to Tier 1)
        m_enemies.emplace_back(EnemyType::SICARII_ASSASSIN, Vector2{ 780.0f, 52.0f }, 720.0f, 880.0f);

        // 5. Tier 1 Mezzanine Guard
        m_enemies.emplace_back(EnemyType::PATROL_GUARD, Vector2{ 920.0f, TEMPLE_TIER_1_Y - 30.0f }, 860.0f, 980.0f);

        // 6. Heavy Temple Guard (Patrolling near Sacred Censer #2)
        m_enemies.emplace_back(EnemyType::HEAVY_LEGIONARY, Vector2{ 1120.0f, GROUND_Y - 32.0f }, 1060.0f, 1180.0f);

        // 7. Sicarii Shadow Assassin on High Column (Hops between Tier 2 and Tier 1)
        m_enemies.emplace_back(EnemyType::SICARII_ASSASSIN, Vector2{ 1320.0f, 48.0f }, 1260.0f, 1420.0f);

        // 8. Tier 2 Gallery Archer (Guarding high incense altar)
        m_enemies.emplace_back(EnemyType::ARCHER, Vector2{ 1500.0f, TEMPLE_TIER_2_Y - 30.0f }, 1500.0f, 1500.0f);

        // 9. Tier 1 Mezzanine Guard
        m_enemies.emplace_back(EnemyType::PATROL_GUARD, Vector2{ 1680.0f, TEMPLE_TIER_1_Y - 30.0f }, 1620.0f, 1740.0f);

        // 10. Heavy Temple Guard at the Inner Veil Archway
        m_enemies.emplace_back(EnemyType::HEAVY_LEGIONARY, Vector2{ 1880.0f, GROUND_Y - 32.0f }, 1820.0f, 1940.0f);

        // 11. Stage 2 Boss: Commander Malchus (High Priest's Guard Commander)
        m_enemies.emplace_back(EnemyType::BOSS_TEMPLE_COMMANDER, Vector2{ 2080.0f, GROUND_Y - 48.0f }, ARENA_MIN_X + 20.0f, ARENA_MAX_X - 40.0f);
    } else if (stageId == 3) {
        // =========================================================================
        // STAGE 3: GOLGOTHA / ROMAN FORTRESS ANTONIA (Shorter, intense ascent)
        // =========================================================================
        // 1. Heavy Legionary at the base of the rocky ascent
        m_enemies.emplace_back(EnemyType::HEAVY_LEGIONARY, Vector2{ 200.0f, GROUND_Y - 32.0f }, 140.0f, 260.0f);

        // 2. Sicarii Shadow Assassin on first high rock pillar
        m_enemies.emplace_back(EnemyType::SICARII_ASSASSIN, Vector2{ 300.0f, 48.0f }, 240.0f, 380.0f);

        // 3. Heavy Legionary patrolling below siege ramp
        m_enemies.emplace_back(EnemyType::HEAVY_LEGIONARY, Vector2{ 490.0f, GROUND_Y - 32.0f }, 430.0f, 550.0f);

        // 4. Archer on high siege ramp battlement
        m_enemies.emplace_back(EnemyType::ARCHER, Vector2{ 580.0f, TEMPLE_TIER_2_Y - 30.0f }, 580.0f, 580.0f);

        // 5. Sicarii Shadow Assassin leaping across crags
        m_enemies.emplace_back(EnemyType::SICARII_ASSASSIN, Vector2{ 700.0f, 48.0f }, 640.0f, 780.0f);

        // 6. Sicarii Shadow Assassin on high citadel pillar
        m_enemies.emplace_back(EnemyType::SICARII_ASSASSIN, Vector2{ 850.0f, 48.0f }, 800.0f, 920.0f);

        // 7. Patrol Guard on citadel mezzanine approach
        m_enemies.emplace_back(EnemyType::PATROL_GUARD, Vector2{ 930.0f, TEMPLE_TIER_1_Y - 30.0f }, 870.0f, 990.0f);

        // 8. Heavy Legionary guarding the Antonia Iron Portcullis
        m_enemies.emplace_back(EnemyType::HEAVY_LEGIONARY, Vector2{ 1040.0f, GROUND_Y - 32.0f }, 980.0f, 1080.0f);

        // 9. Stage 3 Final Boss: Supreme Legatus / Tribunal Praetor Longinus in the Summit Arena
        m_enemies.emplace_back(EnemyType::BOSS_TRIBUNAL_PRAETOR, Vector2{ 1460.0f, GROUND_Y - 48.0f }, 1160.0f, 1500.0f);
    } else if (stageId == 4) {
        // =========================================================================
        // STAGE 4: VALLEY OF THE KIDRON (Dark Ravine & Moonlight Footbridges)
        // =========================================================================
        // 1. Starting Tormentor Guard on ravine floor
        m_enemies.emplace_back(EnemyType::TORMENTOR_GUARD, Vector2{ 180.0f, GROUND_Y - 30.0f }, 110.0f, 240.0f);

        // 2. Heavy Legionary patrolling beneath footbridge 1
        m_enemies.emplace_back(EnemyType::HEAVY_LEGIONARY, Vector2{ 340.0f, GROUND_Y - 32.0f }, 280.0f, 400.0f);

        // 3. Archer on high suspended footbridge over chasm
        m_enemies.emplace_back(EnemyType::ARCHER, Vector2{ 540.0f, 7.0f * TILE_SIZE - 30.0f }, 540.0f, 540.0f);

        // 4. Tormentor Guard on chasm floor
        m_enemies.emplace_back(EnemyType::TORMENTOR_GUARD, Vector2{ 720.0f, GROUND_Y - 30.0f }, 660.0f, 780.0f);

        // 5. Sicarii Assassin on high rock tomb ledge
        m_enemies.emplace_back(EnemyType::SICARII_ASSASSIN, Vector2{ 880.0f, 48.0f }, 820.0f, 960.0f);

        // 6. Heavy Legionary guarding lower path
        m_enemies.emplace_back(EnemyType::HEAVY_LEGIONARY, Vector2{ 1060.0f, GROUND_Y - 32.0f }, 1000.0f, 1140.0f);

        // 7. Tormentor Guard near gate approach
        m_enemies.emplace_back(EnemyType::TORMENTOR_GUARD, Vector2{ 1280.0f, GROUND_Y - 30.0f }, 1220.0f, 1360.0f);

        // 8. Archer on rock parapet
        m_enemies.emplace_back(EnemyType::ARCHER, Vector2{ 1500.0f, 6.0f * TILE_SIZE - 30.0f }, 1500.0f, 1500.0f);

        // 9. Heavy Legionary before Ravine Portcullis
        m_enemies.emplace_back(EnemyType::HEAVY_LEGIONARY, Vector2{ 1740.0f, GROUND_Y - 32.0f }, 1680.0f, 1820.0f);

        // 10. Tormentor Guard guarding portcullis
        m_enemies.emplace_back(EnemyType::TORMENTOR_GUARD, Vector2{ 1960.0f, GROUND_Y - 30.0f }, 1900.0f, 2040.0f);

        // 11. Stage 4 Boss: Tormentor Decurion Brutus in Ravine Arena
        m_enemies.emplace_back(EnemyType::BOSS_GLADIATOR, Vector2{ 2080.0f, GROUND_Y - 48.0f }, ARENA_MIN_X + 20.0f, ARENA_MAX_X - 40.0f);
    } else if (stageId == 5) {
        // =========================================================================
        // STAGE 5: ROYAL CYPRESS HEDGE MAZE (Labyrinth & Marble Terraces)
        // =========================================================================
        // 1. Thorn Sicarii crouching atop hedge arch
        m_enemies.emplace_back(EnemyType::GARDEN_THORN_SICARII, Vector2{ 220.0f, 7.0f * TILE_SIZE - 28.0f }, 160.0f, 280.0f);

        // 2. Heavy Legionary patrolling marble promenade
        m_enemies.emplace_back(EnemyType::HEAVY_LEGIONARY, Vector2{ 380.0f, GROUND_Y - 32.0f }, 320.0f, 440.0f);

        // 3. Corrupted Priest casting dark orbs by marble fountain
        m_enemies.emplace_back(EnemyType::CORRUPTED_PRIEST, Vector2{ 560.0f, GROUND_Y - 30.0f }, 500.0f, 620.0f);

        // 4. Thorn Sicarii ambushing from high Tier 2 hedge
        m_enemies.emplace_back(EnemyType::GARDEN_THORN_SICARII, Vector2{ 760.0f, 6.0f * TILE_SIZE - 28.0f }, 700.0f, 820.0f);

        // 5. Archer atop cypress terrace
        m_enemies.emplace_back(EnemyType::ARCHER, Vector2{ 920.0f, 6.0f * TILE_SIZE - 30.0f }, 920.0f, 920.0f);

        // 6. Heavy Legionary near fountain
        m_enemies.emplace_back(EnemyType::HEAVY_LEGIONARY, Vector2{ 1120.0f, GROUND_Y - 32.0f }, 1060.0f, 1180.0f);

        // 7. Corrupted Priest casting from urn alcove
        m_enemies.emplace_back(EnemyType::CORRUPTED_PRIEST, Vector2{ 1340.0f, GROUND_Y - 30.0f }, 1280.0f, 1400.0f);

        // 8. Thorn Sicarii atop labyrinth wall
        m_enemies.emplace_back(EnemyType::GARDEN_THORN_SICARII, Vector2{ 1520.0f, 6.0f * TILE_SIZE - 28.0f }, 1460.0f, 1580.0f);

        // 9. Heavy Legionary before maze gate
        m_enemies.emplace_back(EnemyType::HEAVY_LEGIONARY, Vector2{ 1740.0f, GROUND_Y - 32.0f }, 1680.0f, 1800.0f);

        // 10. Corrupted Priest guarding high plaza
        m_enemies.emplace_back(EnemyType::CORRUPTED_PRIEST, Vector2{ 1960.0f, GROUND_Y - 30.0f }, 1900.0f, 2020.0f);

        // 11. Stage 5 Boss: High Inquisitor Malchus in the Marble Plaza Arena
        m_enemies.emplace_back(EnemyType::BOSS_TEMPLE_COMMANDER, Vector2{ 2080.0f, GROUND_Y - 48.0f }, ARENA_MIN_X + 20.0f, ARENA_MAX_X - 40.0f);
    } else if (stageId == 6) {
        // =========================================================================
        // STAGE 6: THE GARDEN OF GETHSEMANE (Sacred Olive Grove & Stone of Agony)
        // =========================================================================
        // 1. Thorn Sicarii crouching on first olive tree branch
        m_enemies.emplace_back(EnemyType::GARDEN_THORN_SICARII, Vector2{ 220.0f, 8.0f * TILE_SIZE - 28.0f }, 160.0f, 280.0f);

        // 2. Tormentor Guard walking through nocturnal grove
        m_enemies.emplace_back(EnemyType::TORMENTOR_GUARD, Vector2{ 380.0f, GROUND_Y - 30.0f }, 320.0f, 440.0f);

        // 3. Corrupted Priest casting dark orbs from stone terrace
        m_enemies.emplace_back(EnemyType::CORRUPTED_PRIEST, Vector2{ 540.0f, 9.0f * TILE_SIZE - 30.0f }, 480.0f, 600.0f);

        // 4. Thorn Sicarii on high olive branch canopy
        m_enemies.emplace_back(EnemyType::GARDEN_THORN_SICARII, Vector2{ 720.0f, 7.0f * TILE_SIZE - 28.0f }, 660.0f, 780.0f);

        // 5. Heavy Legionary guarding mossy stone path
        m_enemies.emplace_back(EnemyType::HEAVY_LEGIONARY, Vector2{ 920.0f, GROUND_Y - 32.0f }, 860.0f, 980.0f);

        // 6. Tormentor Guard with torch before sanctuary gate
        m_enemies.emplace_back(EnemyType::TORMENTOR_GUARD, Vector2{ 1140.0f, GROUND_Y - 30.0f }, 1080.0f, 1200.0f);

        // 7. Corrupted Priest at sanctuary arch
        m_enemies.emplace_back(EnemyType::CORRUPTED_PRIEST, Vector2{ 1340.0f, GROUND_Y - 30.0f }, 1280.0f, 1400.0f);

        // 8. Thorn Sicarii on lantern terrace
        m_enemies.emplace_back(EnemyType::GARDEN_THORN_SICARII, Vector2{ 1500.0f, 8.0f * TILE_SIZE - 28.0f }, 1440.0f, 1560.0f);

        // 9. FINAL BOSS: Christus Tenebris (Evil Genius Clone of Jesus)
        // Fighting under the ancient Gethsemane Olive Tree and Stone of Agony Altar!
        m_enemies.emplace_back(EnemyType::BOSS_EVIL_CLONE, Vector2{ 1920.0f, GROUND_Y - 32.0f }, 1640.0f, 2000.0f);
    } else if (stageId == 7) {
        // =========================================================================
        // STAGE 7: SILOAM BAZAAR & AQUEDUCTS (Siloam Market Stalls & Bandit Redoubt)
        // =========================================================================
        // 1. Money Changer swinging balance-scale flail
        m_enemies.emplace_back(EnemyType::MONEY_CHANGER, Vector2{ 220.0f, GROUND_Y - 30.0f }, 150.0f, 280.0f);

        // 2. Patrol Guard near spice stall
        m_enemies.emplace_back(EnemyType::PATROL_GUARD, Vector2{ 420.0f, GROUND_Y - 30.0f }, 360.0f, 480.0f);

        // 3. Archer on high awning platform
        m_enemies.emplace_back(EnemyType::ARCHER, Vector2{ 600.0f, 6.0f * TILE_SIZE - 30.0f }, 600.0f, 600.0f);

        // 4. Sicarii Assassin on high aqueduct awning
        m_enemies.emplace_back(EnemyType::SICARII_ASSASSIN, Vector2{ 720.0f, 48.0f }, 660.0f, 800.0f);

        // 5. Money Changer guarding pottery square
        m_enemies.emplace_back(EnemyType::MONEY_CHANGER, Vector2{ 920.0f, GROUND_Y - 30.0f }, 860.0f, 980.0f);

        // 6. Heavy Legionary patrolling middle bazaar
        m_enemies.emplace_back(EnemyType::HEAVY_LEGIONARY, Vector2{ 1140.0f, GROUND_Y - 32.0f }, 1080.0f, 1200.0f);

        // 7. Money Changer at spice depot
        m_enemies.emplace_back(EnemyType::MONEY_CHANGER, Vector2{ 1380.0f, GROUND_Y - 30.0f }, 1320.0f, 1440.0f);

        // 8. Sicarii Assassin ambushing from upper aqueduct
        m_enemies.emplace_back(EnemyType::SICARII_ASSASSIN, Vector2{ 1540.0f, 48.0f }, 1480.0f, 1620.0f);

        // 9. Archer on high stone terrace
        m_enemies.emplace_back(EnemyType::ARCHER, Vector2{ 1720.0f, 7.0f * TILE_SIZE - 30.0f }, 1720.0f, 1720.0f);

        // 10. Heavy Legionary guarding bandit hideout entrance
        m_enemies.emplace_back(EnemyType::HEAVY_LEGIONARY, Vector2{ 1960.0f, GROUND_Y - 32.0f }, 1900.0f, 2020.0f);

        // 11. Stage 7 Boss: Barabbas the Zealot (Dual Scimitars & Ground Slam)
        m_enemies.emplace_back(EnemyType::BOSS_BARABBAS, Vector2{ 2280.0f, GROUND_Y - 48.0f }, ARENA_MIN_X + 20.0f, ARENA_MAX_X - 40.0f);
    } else if (stageId == 8) {
        // =========================================================================
        // STAGE 8: MOUNT ZION & PALACE OF CAIAPHAS (Marble Balustrades & Council Hall)
        // =========================================================================
        // 1. Levite Enforcer with ceremonial poleaxe halberd
        m_enemies.emplace_back(EnemyType::LEVITE_ENFORCER, Vector2{ 240.0f, GROUND_Y - 32.0f }, 180.0f, 320.0f);

        // 2. Heavy Legionary in outer colonnade
        m_enemies.emplace_back(EnemyType::HEAVY_LEGIONARY, Vector2{ 440.0f, GROUND_Y - 32.0f }, 380.0f, 500.0f);

        // 3. Archer on royal mezzanine
        m_enemies.emplace_back(EnemyType::ARCHER, Vector2{ 640.0f, 6.0f * TILE_SIZE - 30.0f }, 640.0f, 640.0f);

        // 4. Corrupted Priest casting dark orbs
        m_enemies.emplace_back(EnemyType::CORRUPTED_PRIEST, Vector2{ 820.0f, GROUND_Y - 30.0f }, 760.0f, 900.0f);

        // 5. Levite Enforcer patrolling high council terrace
        m_enemies.emplace_back(EnemyType::LEVITE_ENFORCER, Vector2{ 1020.0f, GROUND_Y - 32.0f }, 960.0f, 1100.0f);

        // 6. Heavy Legionary near grand portico
        m_enemies.emplace_back(EnemyType::HEAVY_LEGIONARY, Vector2{ 1240.0f, GROUND_Y - 32.0f }, 1180.0f, 1300.0f);

        // 7. Archer on balustrade
        m_enemies.emplace_back(EnemyType::ARCHER, Vector2{ 1460.0f, 7.0f * TILE_SIZE - 30.0f }, 1460.0f, 1460.0f);

        // 8. Corrupted Priest casting from brazier plinth
        m_enemies.emplace_back(EnemyType::CORRUPTED_PRIEST, Vector2{ 1680.0f, GROUND_Y - 30.0f }, 1620.0f, 1760.0f);

        // 9. Levite Enforcer guarding judgment gate
        m_enemies.emplace_back(EnemyType::LEVITE_ENFORCER, Vector2{ 1960.0f, GROUND_Y - 32.0f }, 1900.0f, 2040.0f);

        // 10. Stage 8 Boss: High Priest Caiaphas (Wrathful Lightning & Scepter Strike)
        m_enemies.emplace_back(EnemyType::BOSS_CAIAPHAS, Vector2{ 2280.0f, GROUND_Y - 48.0f }, ARENA_MIN_X + 20.0f, ARENA_MAX_X - 40.0f);
    } else if (stageId == 9) {
        // =========================================================================
        // STAGE 9: THE UPPER ROOM / CENACLE (The Last Supper Sanctuary)
        // =========================================================================
        // 1. Levite Enforcer at entrance vestibule
        m_enemies.emplace_back(EnemyType::LEVITE_ENFORCER, Vector2{ 220.0f, GROUND_Y - 32.0f }, 160.0f, 280.0f);

        // 2. Money Changer on timber mezzanine
        m_enemies.emplace_back(EnemyType::MONEY_CHANGER, Vector2{ 420.0f, GROUND_Y - 30.0f }, 360.0f, 500.0f);

        // 3. Corrupted Priest by arched window
        m_enemies.emplace_back(EnemyType::CORRUPTED_PRIEST, Vector2{ 620.0f, GROUND_Y - 30.0f }, 560.0f, 700.0f);

        // 4. Levite Enforcer patrolling timber planks
        m_enemies.emplace_back(EnemyType::LEVITE_ENFORCER, Vector2{ 820.0f, GROUND_Y - 32.0f }, 760.0f, 900.0f);

        // 5. Money Changer before communion chamber
        m_enemies.emplace_back(EnemyType::MONEY_CHANGER, Vector2{ 1020.0f, GROUND_Y - 30.0f }, 960.0f, 1100.0f);

        // 6. Corrupted Priest guarding upper passage
        m_enemies.emplace_back(EnemyType::CORRUPTED_PRIEST, Vector2{ 1220.0f, GROUND_Y - 30.0f }, 1160.0f, 1300.0f);

        // 7. Levite Enforcer before the Cenacle sanctuary gate
        m_enemies.emplace_back(EnemyType::LEVITE_ENFORCER, Vector2{ 1380.0f, GROUND_Y - 32.0f }, 1320.0f, 1420.0f);

        // 8. FINAL BOSS: Judas Iscariot (The Traitor - 30 Pieces of Silver, Shadow Dash, Remorse Defeat)
        // Arena: 1460 to 1860, Communion Table & overhead beams
        m_enemies.emplace_back(EnemyType::BOSS_JUDAS, Vector2{ 1660.0f, GROUND_Y - 32.0f }, 1480.0f, 1840.0f);
    } else if (stageId == 10) {
        // =========================================================================
        // STAGE 10: VIA DOLOROSA / STREETS OF JUDGMENT
        // Escorted under Roman garrison watch; Signifers, Praetorians, Lictors
        // =========================================================================
        // 1. Signifer Guard on cobblestone street
        m_enemies.emplace_back(EnemyType::SIGNIFER_GUARD, Vector2{ 220.0f, GROUND_Y - 30.0f }, 150.0f, 280.0f);

        // 2. Praetorian Guard near wooden barricade
        m_enemies.emplace_back(EnemyType::PRAETORIAN_GUARD, Vector2{ 420.0f, GROUND_Y - 32.0f }, 360.0f, 480.0f);

        // 3. Archer on high wooden scaffold
        m_enemies.emplace_back(EnemyType::ARCHER, Vector2{ 600.0f, 6.0f * TILE_SIZE - 30.0f }, 600.0f, 600.0f);

        // 4. Imperial Lictor swinging fasces rods
        m_enemies.emplace_back(EnemyType::IMPERIAL_LICTOR, Vector2{ 780.0f, GROUND_Y - 30.0f }, 720.0f, 840.0f);

        // 5. Signifer Guard rallying legionaries
        m_enemies.emplace_back(EnemyType::SIGNIFER_GUARD, Vector2{ 960.0f, GROUND_Y - 30.0f }, 900.0f, 1020.0f);

        // 6. Praetorian Guard with scutum shield
        m_enemies.emplace_back(EnemyType::PRAETORIAN_GUARD, Vector2{ 1160.0f, GROUND_Y - 32.0f }, 1100.0f, 1220.0f);

        // 7. Imperial Lictor at garrison barricade
        m_enemies.emplace_back(EnemyType::IMPERIAL_LICTOR, Vector2{ 1380.0f, GROUND_Y - 30.0f }, 1320.0f, 1440.0f);

        // 8. Archer on Roman stone arch
        m_enemies.emplace_back(EnemyType::ARCHER, Vector2{ 1560.0f, 7.0f * TILE_SIZE - 30.0f }, 1560.0f, 1560.0f);

        // 9. Praetorian Guard before judgment gate
        m_enemies.emplace_back(EnemyType::PRAETORIAN_GUARD, Vector2{ 1760.0f, GROUND_Y - 32.0f }, 1700.0f, 1820.0f);

        // 10. Imperial Lictor guarding the Praetor's gate
        m_enemies.emplace_back(EnemyType::IMPERIAL_LICTOR, Vector2{ 1960.0f, GROUND_Y - 30.0f }, 1900.0f, 2020.0f);

        // 11. Stage 10 Boss: Praetor Prefect Quintus (Shield Rush & Gladius Flurry)
        m_enemies.emplace_back(EnemyType::BOSS_PREFECT_QUINTUS, Vector2{ 2280.0f, GROUND_Y - 48.0f }, ARENA_MIN_X + 20.0f, ARENA_MAX_X - 40.0f);
    } else if (stageId == 11) {
        // =========================================================================
        // STAGE 11: THE GABBATHA COURT / LITHOSTROTOS TRIBUNAL
        // Imperial Roman Courtroom, Mosaic Pavements, Eagle Pedestals
        // =========================================================================
        // 1. Imperial Lictor at outer basilica portico
        m_enemies.emplace_back(EnemyType::IMPERIAL_LICTOR, Vector2{ 240.0f, GROUND_Y - 30.0f }, 180.0f, 320.0f);

        // 2. Praetorian Guard on mosaic floor
        m_enemies.emplace_back(EnemyType::PRAETORIAN_GUARD, Vector2{ 440.0f, GROUND_Y - 32.0f }, 380.0f, 500.0f);

        // 3. Archer on high magistrate balcony
        m_enemies.emplace_back(EnemyType::ARCHER, Vector2{ 640.0f, 6.0f * TILE_SIZE - 30.0f }, 640.0f, 640.0f);

        // 4. Signifer Guard beside imperial eagle standard
        m_enemies.emplace_back(EnemyType::SIGNIFER_GUARD, Vector2{ 820.0f, GROUND_Y - 30.0f }, 760.0f, 900.0f);

        // 5. Praetorian Guard patrolling magistrate terrace
        m_enemies.emplace_back(EnemyType::PRAETORIAN_GUARD, Vector2{ 1020.0f, GROUND_Y - 32.0f }, 960.0f, 1100.0f);

        // 6. Imperial Lictor in assembly hall
        m_enemies.emplace_back(EnemyType::IMPERIAL_LICTOR, Vector2{ 1240.0f, GROUND_Y - 30.0f }, 1180.0f, 1300.0f);

        // 7. Archer on balustrade gallery
        m_enemies.emplace_back(EnemyType::ARCHER, Vector2{ 1460.0f, 7.0f * TILE_SIZE - 30.0f }, 1460.0f, 1460.0f);

        // 8. Signifer Guard rallying court sentries
        m_enemies.emplace_back(EnemyType::SIGNIFER_GUARD, Vector2{ 1680.0f, GROUND_Y - 30.0f }, 1620.0f, 1760.0f);

        // 9. Praetorian Guard before tribunal gate
        m_enemies.emplace_back(EnemyType::PRAETORIAN_GUARD, Vector2{ 1960.0f, GROUND_Y - 32.0f }, 1900.0f, 2040.0f);

        // 10. Stage 11 Boss: Imperial Advocate Tertullus (Sonic Shouts & Legal Decrees)
        m_enemies.emplace_back(EnemyType::BOSS_ADVOCATE_TERTULLUS, Vector2{ 2280.0f, GROUND_Y - 48.0f }, ARENA_MIN_X + 20.0f, ARENA_MAX_X - 40.0f);
    } else if (stageId == 12) {
        // =========================================================================
        // STAGE 12: THE PRAETORIUM THRONE ROOM & THE BEMA OF PILATE
        // Regal Imperial Palace, Gold Columns, Purple Velvet Canopies
        // =========================================================================
        // 1. Elite Praetorian Guard at imperial entrance
        m_enemies.emplace_back(EnemyType::PRAETORIAN_GUARD, Vector2{ 220.0f, GROUND_Y - 32.0f }, 160.0f, 280.0f);

        // 2. Imperial Lictor on vestibule dais
        m_enemies.emplace_back(EnemyType::IMPERIAL_LICTOR, Vector2{ 420.0f, GROUND_Y - 30.0f }, 360.0f, 500.0f);

        // 3. Signifer Guard with gilded eagle staff
        m_enemies.emplace_back(EnemyType::SIGNIFER_GUARD, Vector2{ 620.0f, GROUND_Y - 30.0f }, 560.0f, 700.0f);

        // 4. Praetorian Guard under purple velvet canopy
        m_enemies.emplace_back(EnemyType::PRAETORIAN_GUARD, Vector2{ 820.0f, GROUND_Y - 32.0f }, 760.0f, 900.0f);

        // 5. Imperial Lictor before throne antechamber
        m_enemies.emplace_back(EnemyType::IMPERIAL_LICTOR, Vector2{ 1020.0f, GROUND_Y - 30.0f }, 960.0f, 1100.0f);

        // 6. Signifer Guard at throne approach
        m_enemies.emplace_back(EnemyType::SIGNIFER_GUARD, Vector2{ 1220.0f, GROUND_Y - 30.0f }, 1160.0f, 1300.0f);

        // 7. Elite Praetorian Guard before Praetorium Throne Gate
        m_enemies.emplace_back(EnemyType::PRAETORIAN_GUARD, Vector2{ 1380.0f, GROUND_Y - 32.0f }, 1320.0f, 1420.0f);

        // 8. FINAL CAMPAIGN BOSS: Pontius Pilate (Imperial Governor of Judea)
        // Handwashing Water Basin, Golden Stylus Judgment Decree, Mirage Teleport, Remorse Defeat!
        // Arena: 1460 to 1860, Ivory Throne & Golden Basin
        m_enemies.emplace_back(EnemyType::BOSS_PONTIUS_PILATE, Vector2{ 1660.0f, GROUND_Y - 32.0f }, 1480.0f, 1840.0f);
    } else {
        // =========================================================================
        // STAGE 1: JERUSALEM OUTSKIRTS & CITADEL APPROACH
        // =========================================================================
        // 1. Starting Street Grunt (Patrols open street before Crate 1)
        m_enemies.emplace_back(EnemyType::PATROL_GUARD, Vector2{ 150.0f, GROUND_Y - 30.0f }, 90.0f, 205.0f);

        // 2. Upper Balcony Guard
        m_enemies.emplace_back(EnemyType::PATROL_GUARD, Vector2{ 420.0f, UPPER_TIER_Y - 30.0f }, 390.0f, 550.0f);

        // 3. Auxiliary Archer on High Aqueduct
        m_enemies.emplace_back(EnemyType::ARCHER, Vector2{ 720.0f, UPPER_TIER_Y - 30.0f }, 720.0f, 720.0f);

        // 4. Ground Heavy Legionary (Guarding lower path before aqueduct crate)
        m_enemies.emplace_back(EnemyType::HEAVY_LEGIONARY, Vector2{ 680.0f, GROUND_Y - 32.0f }, 620.0f, 750.0f);

        // 5. Midfield Grunt Guard
        m_enemies.emplace_back(EnemyType::PATROL_GUARD, Vector2{ 880.0f, GROUND_Y - 30.0f }, 820.0f, 925.0f);

        // 6. Rooftop Terrace Archer (Guarding Basket 3)
        m_enemies.emplace_back(EnemyType::ARCHER, Vector2{ 1380.0f, UPPER_TIER_Y - 30.0f }, 1380.0f, 1380.0f);

        // 7. Upper Tier Guard
        m_enemies.emplace_back(EnemyType::PATROL_GUARD, Vector2{ 1180.0f, UPPER_TIER_Y - 30.0f }, 1100.0f, 1240.0f);

        // 8. Ground Heavy Legionary (Market gate)
        m_enemies.emplace_back(EnemyType::HEAVY_LEGIONARY, Vector2{ 1280.0f, GROUND_Y - 32.0f }, 1220.0f, 1360.0f);

        // 9. Garrison Checkpoint Guard
        m_enemies.emplace_back(EnemyType::PATROL_GUARD, Vector2{ 1680.0f, GROUND_Y - 30.0f }, 1620.0f, 1780.0f);

        // 10. Heavy Legionary guarding near Basket 4
        m_enemies.emplace_back(EnemyType::HEAVY_LEGIONARY, Vector2{ 1880.0f, GROUND_Y - 32.0f }, 1850.0f, 1935.0f);

        // 11. Rampart Archer before Arena
        m_enemies.emplace_back(EnemyType::ARCHER, Vector2{ 2000.0f, UPPER_TIER_Y - 30.0f }, 2000.0f, 2000.0f);

        // 12. Stage 1 Boss: Centurion Brutus (Ken-Oh equivalent)
        // Spawns before the gate (at x = 2080.0f) in Phase 1 (takes 2 whip swings, moves into arena)
        m_enemies.emplace_back(EnemyType::BOSS_GLADIATOR, Vector2{ 2080.0f, GROUND_Y - 48.0f }, ARENA_MIN_X + 20.0f, ARENA_MAX_X - 40.0f);
    }
}

void EnemyManager::SpawnExplosion(Vector2 pos, Color mainColor) {
    for (int i = 0; i < 16; ++i) {
        float angle = (float)rand() / (float)RAND_MAX * 6.28318f;
        float speed = 40.0f + (float)rand() / (float)RAND_MAX * 90.0f;
        DeathParticle p;
        p.position = pos;
        p.velocity = { cosf(angle) * speed, sinf(angle) * speed };
        p.color = (i % 2 == 0) ? mainColor : Color{ 255, 230, 100, 255 };
        p.life = 0.45f;
        p.maxLife = 0.45f;
        m_particles.push_back(p);
    }
}

void EnemyManager::SpawnSpark(Vector2 pos, Color sparkColor) {
    for (int i = 0; i < 6; ++i) {
        float angle = (float)rand() / (float)RAND_MAX * 6.28318f;
        float speed = 60.0f + (float)rand() / (float)RAND_MAX * 80.0f;
        DeathParticle p;
        p.position = pos;
        p.velocity = { cosf(angle) * speed, sinf(angle) * speed };
        p.color = sparkColor;
        p.life = 0.2f;
        p.maxLife = 0.2f;
        m_particles.push_back(p);
    }
}

void EnemyManager::Update(float dt, const Vector2& playerPos, Level& level, bool screenFrozen, bool triggerWrathWipe) {
    // 1. Update particles
    for (auto& p : m_particles) {
        p.position.x += p.velocity.x * dt;
        p.position.y += p.velocity.y * dt;
        p.life -= dt;
    }
    m_particles.erase(std::remove_if(m_particles.begin(), m_particles.end(), [](const DeathParticle& p) { return p.life <= 0.0f; }), m_particles.end());

    // 2. Handle Divine Wrath Board Wipe
    if (triggerWrathWipe) {
        for (auto& enemy : m_enemies) {
            if (enemy.IsActive() && !enemy.IsDefeated()) {
                // If standard enemy on screen, eliminate!
                if (!enemy.IsBoss()) {
                    enemy.OnHit(99, true, true);
                    SpawnExplosion(enemy.GetPosition(), COLOR_ROMAN_CRIMSON);
                } else {
                    // Boss takes 4 pips damage from Wrath
                    enemy.OnHit(4, true, true);
                    SpawnExplosion(enemy.GetPosition(), COLOR_ROMAN_GOLD);
                }
            }
        }
    }

    if (screenFrozen) return; // Freeze enemy movement during wrath sequence

    // 3. Update enemies
    for (auto& enemy : m_enemies) {
        if (enemy.IsActive() && (!enemy.IsDefeated() || enemy.IsBoss())) {
            enemy.Update(dt, playerPos, level);

            Vector2 arrowPos;
            float arrowVelX;
            if (enemy.CheckAndClearJustFired(arrowPos, arrowVelX)) {
                m_arrows.push_back({ arrowPos, arrowVelX, true });
                AudioManager::PlaySfx(SFX_NAIL_THROW);
            }

            Vector2 daggerPos;
            float daggerVelX;
            if (enemy.CheckAndClearJustThrewDagger(daggerPos, daggerVelX)) {
                m_daggers.push_back({ daggerPos, daggerVelX, true, 0.0f });
                AudioManager::PlaySfx(SFX_NAIL_THROW);
            }

            Vector2 darkPos;
            float dVelX, dVelY;
            bool isSpike;
            if (enemy.CheckAndClearJustFiredDark(darkPos, dVelX, dVelY, isSpike)) {
                m_darkProjectiles.push_back({ darkPos, dVelX, dVelY, isSpike, true, 0.0f });
            }

            Vector2 silverPos;
            float silverDir;
            if (enemy.CheckAndClearJustThrewSilver(silverPos, silverDir)) {
                // Judas throws 3 pieces of silver in an escalating arc
                m_silverCoins.push_back({ silverPos, { silverDir * 180.0f, -80.0f }, true, 0.0f, 3 });
                m_silverCoins.push_back({ silverPos, { silverDir * 140.0f, -140.0f }, true, 0.0f, 3 });
                m_silverCoins.push_back({ silverPos, { silverDir * 100.0f, -200.0f }, true, 0.0f, 3 });
                AudioManager::PlaySfx(SFX_SILVER_SCATTER);
            }

            Vector2 shockPos;
            float shockDir;
            if (enemy.CheckAndClearJustSpawnedShockwave(shockPos, shockDir)) {
                m_bloodShockwaves.push_back({ shockPos, shockDir * 170.0f, true, 0.0f });
                AudioManager::PlaySfx(SFX_WHIP_SLAM);
            }

            Vector2 lightningPos;
            if (enemy.CheckAndClearJustCastLightning(lightningPos)) {
                m_lightningStrikes.push_back({ lightningPos, 0.0f, true });
                AudioManager::PlaySfx(SFX_THUNDER);
            }

            Vector2 waterPos;
            float waterDir;
            if (enemy.CheckAndClearJustSplashedWater(waterPos, waterDir)) {
                // Splash 3 water droplets in an arc
                m_waterDroplets.push_back({ waterPos, { waterDir * 140.0f, -120.0f }, true, 0.0f });
                m_waterDroplets.push_back({ waterPos, { waterDir * 190.0f, -160.0f }, true, 0.0f });
                m_waterDroplets.push_back({ waterPos, { waterDir * 90.0f, -180.0f }, true, 0.0f });
            }

            Vector2 stylusPos;
            float stylusDir;
            if (enemy.CheckAndClearJustFiredStylus(stylusPos, stylusDir)) {
                m_stylusBlades.push_back({ stylusPos, stylusDir * 230.0f, true, 0.0f });
            }

            Vector2 shoutPos;
            float shoutDir;
            if (enemy.CheckAndClearJustShouted(shoutPos, shoutDir)) {
                m_shoutWaves.push_back({ shoutPos, shoutDir * 160.0f, true, 0.0f, 1.0f });
            }
        }
    }

    // 4. Update arrows
    for (auto& arrow : m_arrows) {
        if (arrow.active) {
            arrow.position.x += arrow.velocityX * dt;
            if (arrow.position.x < playerPos.x - 240.0f || arrow.position.x > playerPos.x + 240.0f) {
                arrow.active = false;
            }
        }
    }
    m_arrows.erase(std::remove_if(m_arrows.begin(), m_arrows.end(), [](const EnemyArrow& a) { return !a.active; }), m_arrows.end());

    // 5. Update daggers
    for (auto& dagger : m_daggers) {
        if (dagger.active) {
            dagger.position.x += dagger.velocityX * dt;
            dagger.spinTimer += dt;
            if (dagger.position.x < playerPos.x - 260.0f || dagger.position.x > playerPos.x + 260.0f) {
                dagger.active = false;
            }
        }
    }
    m_daggers.erase(std::remove_if(m_daggers.begin(), m_daggers.end(), [](const EnemyDagger& d) { return !d.active; }), m_daggers.end());

    // 6. Update Dark Projectiles (Priest dark orbs & Evil Clone dark spikes)
    for (auto& dp : m_darkProjectiles) {
        if (dp.active) {
            dp.timer += dt;
            dp.position.x += dp.velocityX * dt;
            dp.position.y += dp.velocityY * dt;
            if (dp.isSpike) {
                if (dp.timer >= 0.85f) dp.active = false;
            } else {
                if (dp.position.x < playerPos.x - 260.0f || dp.position.x > playerPos.x + 260.0f || dp.timer >= 2.8f) {
                    dp.active = false;
                }
            }
        }
    }
    m_darkProjectiles.erase(std::remove_if(m_darkProjectiles.begin(), m_darkProjectiles.end(), [](const DarkProjectile& d) { return !d.active; }), m_darkProjectiles.end());

    // 7. Update Judas Silver Coins (Bouncing 30 pieces of silver)
    for (auto& coin : m_silverCoins) {
        if (coin.active) {
            coin.timer += dt;
            coin.position.x += coin.velocity.x * dt;
            coin.position.y += coin.velocity.y * dt;
            coin.velocity.y += 440.0f * dt; // Gravity

            // Floor bounce
            if (coin.position.y >= GROUND_Y - 4.0f) {
                coin.position.y = GROUND_Y - 4.0f;
                if (coin.bouncesLeft > 0) {
                    coin.velocity.y = -coin.velocity.y * 0.55f;
                    coin.velocity.x *= 0.75f;
                    coin.bouncesLeft--;
                    AudioManager::PlaySfx(SFX_COIN_RICOCHET);
                } else {
                    coin.velocity = { 0, 0 };
                }
            }

            if (coin.timer >= 2.6f || coin.position.x < playerPos.x - 300.0f || coin.position.x > playerPos.x + 300.0f) {
                coin.active = false;
            }
        }
    }
    m_silverCoins.erase(std::remove_if(m_silverCoins.begin(), m_silverCoins.end(), [](const SilverProjectile& c) { return !c.active; }), m_silverCoins.end());

    // 8. Update Blood Shockwaves (Purse slam shockwave)
    for (auto& sw : m_bloodShockwaves) {
        if (sw.active) {
            sw.timer += dt;
            sw.position.x += sw.velocityX * dt;
            if (sw.timer >= 1.4f || sw.position.x < playerPos.x - 320.0f || sw.position.x > playerPos.x + 320.0f) {
                sw.active = false;
            }
        }
    }
    m_bloodShockwaves.erase(std::remove_if(m_bloodShockwaves.begin(), m_bloodShockwaves.end(), [](const BloodShockwave& s) { return !s.active; }), m_bloodShockwaves.end());

    // 9. Update Caiaphas Lightning Strikes
    for (auto& ls : m_lightningStrikes) {
        if (ls.active) {
            ls.timer += dt;
            if (ls.timer >= 0.95f) {
                ls.active = false;
            }
        }
    }
    m_lightningStrikes.erase(std::remove_if(m_lightningStrikes.begin(), m_lightningStrikes.end(), [](const CaiaphasLightning& l) { return !l.active; }), m_lightningStrikes.end());

    // 10. Update Water Droplets (Pilate water basin splashes)
    for (auto& drop : m_waterDroplets) {
        if (drop.active) {
            drop.timer += dt;
            drop.position.x += drop.velocity.x * dt;
            drop.position.y += drop.velocity.y * dt;
            drop.velocity.y += 380.0f * dt; // Gravity

            if (drop.position.y >= GROUND_Y - 4.0f) {
                drop.active = false;
            }

            if (drop.timer >= 2.0f || drop.position.x < playerPos.x - 300.0f || drop.position.x > playerPos.x + 300.0f) {
                drop.active = false;
            }
        }
    }
    m_waterDroplets.erase(std::remove_if(m_waterDroplets.begin(), m_waterDroplets.end(), [](const WaterDroplet& d) { return !d.active; }), m_waterDroplets.end());

    // 11. Update Stylus Blades (Pilate golden decree floor beam)
    for (auto& sb : m_stylusBlades) {
        if (sb.active) {
            sb.timer += dt;
            sb.position.x += sb.velocityX * dt;
            if (sb.timer >= 2.0f || sb.position.x < playerPos.x - 320.0f || sb.position.x > playerPos.x + 320.0f) {
                sb.active = false;
            }
        }
    }
    m_stylusBlades.erase(std::remove_if(m_stylusBlades.begin(), m_stylusBlades.end(), [](const StylusBlade& s) { return !s.active; }), m_stylusBlades.end());

    // 12. Update Shout Waves (Advocate Tertullus sonic decree shout)
    for (auto& sw : m_shoutWaves) {
        if (sw.active) {
            sw.timer += dt;
            sw.position.x += sw.velocityX * dt;
            sw.scale += dt * 0.8f;
            if (sw.timer >= 1.6f || sw.position.x < playerPos.x - 300.0f || sw.position.x > playerPos.x + 300.0f) {
                sw.active = false;
            }
        }
    }
    m_shoutWaves.erase(std::remove_if(m_shoutWaves.begin(), m_shoutWaves.end(), [](const ShoutWave& w) { return !w.active; }), m_shoutWaves.end());
}

void EnemyManager::CheckPlayerAttacks(Player& player) {
    auto& nails = player.GetNails();
    const auto& meleeBox = player.GetMeleeHitbox();

    for (auto& enemy : m_enemies) {
        if (!enemy.IsActive() || enemy.IsDefeated() || enemy.IsInDefeatSequence() || enemy.IsBossWalkingOff()) continue;

        Rectangle enemyBox = enemy.GetHurtbox();

        // 1. Check Melee Strike Hitbox
        if (meleeBox.active && CheckCollisionRecs(meleeBox.rect, enemyBox)) {
            bool fromBehind = (player.IsFacingLeft() == enemy.IsFacingLeft());
            bool blocked = enemy.OnHit(1, fromBehind, false);
            if (!blocked) {
                player.AddScore(enemy.IsBoss() ? 200 : 100);
                SpawnExplosion(enemy.GetPosition(), COLOR_ROMAN_GOLD);
            } else {
                SpawnSpark(Vector2{ enemyBox.x + 8.0f, enemyBox.y + 10.0f }, COLOR_ROMAN_GOLD);
            }
            player.ClearMeleeHitbox();
        }

        // 2. Check Player Ranged Nails
        for (auto& nail : nails) {
            if (!nail.active) continue;

            Rectangle nailBox = { nail.position.x, nail.position.y, 8.0f, 4.0f };
            if (CheckCollisionRecs(nailBox, enemyBox)) {
                nail.active = false;

                bool fromBehind = (nail.velocityX > 0 && !enemy.IsFacingLeft()) || (nail.velocityX < 0 && enemy.IsFacingLeft());
                // Is this a high hit (aimed at head) or crouching?
                bool isHigh = (nail.position.y <= enemyBox.y + 14.0f);

                bool blocked = enemy.OnHit(1, fromBehind, isHigh);
                if (!blocked) {
                    player.AddScore(enemy.IsBoss() ? 200 : 100);
                    SpawnExplosion(enemy.GetPosition(), COLOR_ROMAN_GOLD);
                } else {
                    // Metallic ping deflect spark
                    SpawnSpark(nail.position, COLOR_ROMAN_GOLD);
                }
            }
        }
    }
}

void EnemyManager::CheckPlayerDamage(Player& player, bool inBossFight) {
    if (player.IsInvulnerable() || player.IsWrathActive() || player.IsDead()) return;

    Rectangle playerBox = player.GetHurtbox();

    // 1. Direct enemy body contact / melee attack hitboxes
    for (const auto& enemy : m_enemies) {
        if (!enemy.IsActive() || enemy.IsDefeated() || enemy.IsInDefeatSequence() || enemy.IsBossWalkingOff()) continue;

        // Active attack hitbox (spear lunge, sword slash, whip crack/slam)
        if (enemy.HasActiveAttackHitbox()) {
            Rectangle attackBox = enemy.GetAttackHitbox();
            if (CheckCollisionRecs(playerBox, attackBox)) {
                player.TakeDamage(1, inBossFight);
                return;
            }
        }

        // Body collision
        Rectangle bodyBox = enemy.GetHurtbox();
        if (CheckCollisionRecs(playerBox, bodyBox)) {
            player.TakeDamage(1, inBossFight);
            return;
        }
    }

    // 2. Enemy projectile arrows
    for (auto& arrow : m_arrows) {
        if (arrow.active) {
            Rectangle arrowBox = { arrow.position.x, arrow.position.y, 10.0f, 4.0f };
            if (CheckCollisionRecs(playerBox, arrowBox)) {
                arrow.active = false;
                player.TakeDamage(1, inBossFight);
                return;
            }
        }
    }

    // 3. Enemy projectile daggers (Sicarii throwing daggers)
    for (auto& dagger : m_daggers) {
        if (dagger.active) {
            Rectangle daggerBox = { dagger.position.x - 4.0f, dagger.position.y - 2.0f, 8.0f, 4.0f };
            if (CheckCollisionRecs(playerBox, daggerBox)) {
                dagger.active = false;
                player.TakeDamage(1, inBossFight);
                return;
            }
        }
    }

    // 4. Dark projectiles (Corrupted Priest dark orbs & Evil Clone dark spikes)
    for (auto& dp : m_darkProjectiles) {
        if (dp.active) {
            Rectangle dpBox = dp.isSpike ? 
                Rectangle{ dp.position.x - 6.0f, dp.position.y - 14.0f, 12.0f, 16.0f } :
                Rectangle{ dp.position.x - 5.0f, dp.position.y - 5.0f, 10.0f, 10.0f };
            if (CheckCollisionRecs(playerBox, dpBox)) {
                dp.active = false;
                player.TakeDamage(1, inBossFight);
                return;
            }
        }
    }

    // 5. Silver Coins (Judas Iscariot 30 pieces of silver)
    for (auto& coin : m_silverCoins) {
        if (coin.active) {
            Rectangle coinBox = { coin.position.x - 4.0f, coin.position.y - 4.0f, 8.0f, 8.0f };
            if (CheckCollisionRecs(playerBox, coinBox)) {
                coin.active = false;
                player.TakeDamage(1, inBossFight);
                return;
            }
        }
    }

    // 6. Blood Shockwaves (Purse ground slam wave)
    for (auto& sw : m_bloodShockwaves) {
        if (sw.active) {
            Rectangle swBox = { sw.position.x - 8.0f, sw.position.y - 12.0f, 16.0f, 16.0f };
            if (CheckCollisionRecs(playerBox, swBox)) {
                sw.active = false;
                player.TakeDamage(1, inBossFight);
                return;
            }
        }
    }

    // 7. Caiaphas Wrathful Lightning Strikes
    for (const auto& ls : m_lightningStrikes) {
        if (ls.active && ls.timer >= 0.35f && ls.timer <= 0.70f) {
            Rectangle boltBox = { ls.position.x - 10.0f, 0.0f, 20.0f, GROUND_Y };
            if (CheckCollisionRecs(playerBox, boltBox)) {
                player.TakeDamage(1, inBossFight);
                return;
            }
        }
    }

    // 8. Pilate Water Droplets
    for (auto& drop : m_waterDroplets) {
        if (drop.active) {
            Rectangle dropBox = { drop.position.x - 4.0f, drop.position.y - 4.0f, 8.0f, 8.0f };
            if (CheckCollisionRecs(playerBox, dropBox)) {
                drop.active = false;
                player.TakeDamage(1, inBossFight);
                return;
            }
        }
    }

    // 9. Pilate Golden Stylus Blades
    for (auto& sb : m_stylusBlades) {
        if (sb.active) {
            Rectangle sbBox = { sb.position.x - 6.0f, sb.position.y - 4.0f, 14.0f, 8.0f };
            if (CheckCollisionRecs(playerBox, sbBox)) {
                sb.active = false;
                player.TakeDamage(1, inBossFight);
                return;
            }
        }
    }

    // 10. Advocate Tertullus Shout Waves
    for (auto& sw : m_shoutWaves) {
        if (sw.active) {
            float size = 16.0f * sw.scale;
            Rectangle swBox = { sw.position.x - size * 0.5f, sw.position.y - size * 0.5f, size, size };
            if (CheckCollisionRecs(playerBox, swBox)) {
                sw.active = false;
                player.TakeDamage(1, inBossFight);
                return;
            }
        }
    }
}

float EnemyManager::GetClosestEnemyDistance(const Vector2& playerPos, bool playerFacingLeft, bool& outInFront) const {
    float closestDist = 9999.0f;
    outInFront = false;

    for (const auto& enemy : m_enemies) {
        if (!enemy.IsActive() || enemy.IsDefeated()) continue;

        Vector2 ePos = enemy.GetPosition();
        // Check vertical proximity (within ~24 px)
        if (fabsf(ePos.y - playerPos.y) < 28.0f) {
            float dx = ePos.x - playerPos.x;
            bool inFront = (playerFacingLeft && dx < 0.0f) || (!playerFacingLeft && dx > 0.0f);
            float dist = fabsf(dx);
            if (dist < closestDist) {
                closestDist = dist;
                outInFront = inFront;
            }
        }
    }

    return closestDist;
}

bool EnemyManager::IsBossActive() const {
    for (const auto& enemy : m_enemies) {
        if (enemy.IsBoss() && enemy.IsActive() && !enemy.IsDefeated()) {
            return true;
        }
    }
    return false;
}

Enemy* EnemyManager::GetBoss() const {
    for (const auto& enemy : m_enemies) {
        if (enemy.IsBoss()) {
            return const_cast<Enemy*>(&enemy);
        }
    }
    return nullptr;
}

void EnemyManager::Draw() {
    // Draw enemies
    for (auto& enemy : m_enemies) {
        if (enemy.IsActive()) {
            enemy.Draw();
        }
    }

    // Draw arrows with high-visibility scarlet & white speed streak
    for (const auto& arrow : m_arrows) {
        if (arrow.active) {
            float dir = arrow.velocityX > 0 ? 1.0f : -1.0f;
            float tailX = arrow.position.x - dir * 20.0f;
            float headX = arrow.position.x + dir * 6.0f;
            float py = arrow.position.y + 5.0f;

            // Highly visible scarlet & white tracer streak
            DrawLineEx({ tailX, py }, { headX, py }, 4.0f, Color{ 225, 30, 40, 180 });
            DrawLineEx({ tailX + dir * 8.0f, py }, { headX, py }, 2.0f, Color{ 255, 255, 255, 230 });

            GraphicsManager::DrawSprite(SPR_ARROW, arrow.position.x, arrow.position.y, arrow.velocityX < 0);
        }
    }

    // Draw spinning daggers
    for (const auto& dagger : m_daggers) {
        if (dagger.active) {
            float dir = dagger.velocityX > 0 ? 1.0f : -1.0f;
            float tailX = dagger.position.x - dir * 14.0f;
            float headX = dagger.position.x + dir * 4.0f;
            DrawLineEx({ tailX, dagger.position.y + 4.0f }, { headX, dagger.position.y + 4.0f }, 2.0f, COLOR_SICARII_BLADE);
            GraphicsManager::DrawSprite(SPR_SICARII_DAGGER, dagger.position.x - 4.0f, dagger.position.y, dagger.velocityX < 0);
            GraphicsManager::DrawSpark(dagger.position.x, dagger.position.y + 4.0f, 1.5f, Color{ 255, 255, 255, 220 });
        }
    }

    // Draw Dark Projectiles (Priest dark orbs & Evil Clone spikes)
    for (const auto& dp : m_darkProjectiles) {
        if (dp.active) {
            if (dp.isSpike) {
                // Rising dark crystal spike
                GraphicsManager::DrawSprite(SPR_DARK_SPIKE, dp.position.x - 8.0f, dp.position.y - 16.0f);
                DrawCircleLines(static_cast<int>(dp.position.x), static_cast<int>(dp.position.y - 8.0f), 8.0f, Color{ 220, 80, 255, 200 });
            } else {
                // Streaking dark purple orb
                float dir = dp.velocityX > 0 ? 1.0f : -1.0f;
                DrawLineEx({ dp.position.x - dir * 12.0f, dp.position.y + 4.0f }, { dp.position.x + dir * 4.0f, dp.position.y + 4.0f }, 3.0f, Color{ 140, 30, 200, 180 });
                GraphicsManager::DrawSprite(SPR_DARK_ORB, dp.position.x - 6.0f, dp.position.y);
                GraphicsManager::DrawSpark(dp.position.x, dp.position.y + 4.0f, 2.0f, Color{ 240, 180, 255, 240 });
            }
        }
    }

    // Draw Silver Coins (Judas Iscariot 30 pieces of silver)
    for (const auto& coin : m_silverCoins) {
        if (coin.active) {
            GraphicsManager::DrawSprite(SPR_SILVER_COIN_PROJECTILE, coin.position.x - 4.0f, coin.position.y - 4.0f);
            GraphicsManager::DrawSpark(coin.position.x, coin.position.y, 2.0f, COLOR_SILVER_SHEEN);
        }
    }

    // Draw Blood Shockwaves
    for (const auto& sw : m_bloodShockwaves) {
        if (sw.active) {
            DrawCircleGradient(static_cast<int>(sw.position.x), static_cast<int>(sw.position.y), 10.0f, Color{ 200, 30, 40, 120 }, Color{ 0, 0, 0, 0 });
            GraphicsManager::DrawSprite(SPR_BLOOD_SHOCKWAVE, sw.position.x - 8.0f, sw.position.y - 8.0f, sw.velocityX < 0);
        }
    }

    // Draw Caiaphas Lightning Strikes
    for (const auto& ls : m_lightningStrikes) {
        if (ls.active) {
            if (ls.timer < 0.35f) {
                // Warning indicator beam
                float alphaPct = ls.timer / 0.35f;
                unsigned char a = static_cast<unsigned char>(alphaPct * 140);
                DrawRectangle(static_cast<int>(ls.position.x - 4.0f), 0, 8, static_cast<int>(GROUND_Y), Color{ 255, 230, 100, a });
            } else if (ls.timer <= 0.70f) {
                // Active lightning strike!
                DrawLineEx({ ls.position.x, 0.0f }, { ls.position.x, GROUND_Y }, 4.0f, Color{ 255, 255, 255, 240 });
                DrawLineEx({ ls.position.x - 2.0f, 0.0f }, { ls.position.x + 2.0f, GROUND_Y }, 8.0f, Color{ 210, 180, 255, 140 });
                GraphicsManager::DrawSprite(SPR_CAIAPHAS_LIGHTNING, ls.position.x - 8.0f, GROUND_Y - 96.0f);
                GraphicsManager::DrawSpark(ls.position.x, GROUND_Y - 4.0f, 6.0f, Color{ 255, 255, 200, 255 });
            }
        }
    }

    // Draw Water Droplets (Pilate Basin Splash)
    for (const auto& drop : m_waterDroplets) {
        if (drop.active) {
            DrawCircleGradient(static_cast<int>(drop.position.x), static_cast<int>(drop.position.y), 4.0f, COLOR_BASIN_WATER, Color{ 255, 255, 255, 0 });
            GraphicsManager::DrawSprite(SPR_WATER_DROPLET, drop.position.x - 4.0f, drop.position.y - 4.0f);
        }
    }

    // Draw Stylus Blades (Pilate Golden Stylus Beam)
    for (const auto& sb : m_stylusBlades) {
        if (sb.active) {
            float dir = sb.velocityX > 0 ? 1.0f : -1.0f;
            DrawLineEx({ sb.position.x - dir * 18.0f, sb.position.y }, { sb.position.x + dir * 6.0f, sb.position.y }, 3.0f, COLOR_PRAETORIUM_GOLD);
            GraphicsManager::DrawSprite(SPR_GOLDEN_STYLUS_BLADE, sb.position.x - 7.0f, sb.position.y - 4.0f, sb.velocityX < 0);
            GraphicsManager::DrawSpark(sb.position.x, sb.position.y, 2.5f, COLOR_GOLD_BRIGHT);
        }
    }

    // Draw Shout Waves (Advocate Tertullus Sonic Wave)
    for (const auto& sw : m_shoutWaves) {
        if (sw.active) {
            float alphaPct = 1.0f - (sw.timer / 1.6f);
            if (alphaPct < 0.0f) alphaPct = 0.0f;
            Color waveColor = Color{ 240, 230, 180, static_cast<unsigned char>(alphaPct * 200) };
            DrawCircleLines(static_cast<int>(sw.position.x), static_cast<int>(sw.position.y), 10.0f * sw.scale, waveColor);
            GraphicsManager::DrawSprite(SPR_SHOUT_WAVE, sw.position.x - 8.0f, sw.position.y - 8.0f, sw.velocityX < 0);
        }
    }

    // Draw particles
    for (const auto& p : m_particles) {
        float alpha = p.life / p.maxLife;
        Color c = p.color;
        c.a = static_cast<unsigned char>(alpha * 255);
        DrawRectangle(static_cast<int>(p.position.x), static_cast<int>(p.position.y), 2, 2, c);
    }
}
