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
            return { m_position.x + 6.0f, m_position.y + 4.0f, 24.0f, 44.0f };
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

    // 2. Boss Armor Logic (Brutus and Commander Malchus)
    if (m_type == EnemyType::BOSS_GLADIATOR || m_type == EnemyType::BOSS_TEMPLE_COMMANDER) {
        bool isVulnerable = isHighHit || (damage >= 4) || hitFromBehind;
        if (!isVulnerable) {
            AudioManager::PlaySfx(SFX_SHIELD_DEFLECT);
            return true; // Armor deflected hit!
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

    if (m_type == EnemyType::BOSS_GLADIATOR || m_type == EnemyType::BOSS_TEMPLE_COMMANDER) {
        AudioManager::PlaySfx(SFX_BOSS_HURT);
        if (m_hp <= 0) {
            m_hp = 0;
            m_whipState = BossWhipState::KNEEL;
            m_whipTimer = 0.0f;
            m_witherTimer = 0.0f;
            m_attackHitboxActive = false;
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
    m_particles.clear();
    SpawnStageEnemies(stageId);
}

void EnemyManager::SpawnStageEnemies(int stageId) {
    m_enemies.clear();
    m_arrows.clear();
    m_daggers.clear();

    if (stageId == 2) {
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
        // STAGE 3: GOLGOTHA / ROMAN FORTRESS ANTONIA
        // =========================================================================
        m_enemies.emplace_back(EnemyType::SICARII_ASSASSIN, Vector2{ 260.0f, 48.0f }, 200.0f, 340.0f);
        m_enemies.emplace_back(EnemyType::HEAVY_LEGIONARY, Vector2{ 420.0f, GROUND_Y - 32.0f }, 360.0f, 480.0f);
        m_enemies.emplace_back(EnemyType::ARCHER, Vector2{ 640.0f, UPPER_TIER_Y - 30.0f }, 640.0f, 640.0f);
        m_enemies.emplace_back(EnemyType::SICARII_ASSASSIN, Vector2{ 860.0f, 48.0f }, 800.0f, 960.0f);
        m_enemies.emplace_back(EnemyType::HEAVY_LEGIONARY, Vector2{ 1100.0f, GROUND_Y - 32.0f }, 1040.0f, 1160.0f);
        m_enemies.emplace_back(EnemyType::ARCHER, Vector2{ 1320.0f, UPPER_TIER_Y - 30.0f }, 1320.0f, 1320.0f);
        m_enemies.emplace_back(EnemyType::PATROL_GUARD, Vector2{ 1560.0f, GROUND_Y - 30.0f }, 1500.0f, 1620.0f);
        m_enemies.emplace_back(EnemyType::SICARII_ASSASSIN, Vector2{ 1780.0f, 48.0f }, 1720.0f, 1860.0f);
        m_enemies.emplace_back(EnemyType::BOSS_GLADIATOR, Vector2{ 2080.0f, GROUND_Y - 48.0f }, ARENA_MIN_X + 20.0f, ARENA_MAX_X - 40.0f);
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

    // Draw particles
    for (const auto& p : m_particles) {
        float alpha = p.life / p.maxLife;
        Color c = p.color;
        c.a = static_cast<unsigned char>(alpha * 255);
        DrawRectangle(static_cast<int>(p.position.x), static_cast<int>(p.position.y), 2, 2, c);
    }
}
