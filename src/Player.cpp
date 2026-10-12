#include "Player.h"
#include <cmath>
#include <algorithm>

Player::Player() {
    m_lives = PLAYER_INITIAL_LIVES;
    m_score = 0;
    Reset(false);
}

void Player::Reset(bool resetLives) {
    if (resetLives) {
        m_lives = PLAYER_INITIAL_LIVES;
        m_score = 0;
    }
    m_bossHitsTaken = 0;
    m_shatteredHearts = 0;
    m_hp = PLAYER_MAX_HP;
    m_position = { 32.0f, GROUND_Y - 30.0f };
    m_velocity = { 0.0f, 0.0f };
    m_facingLeft = false;
    m_onGround = true;
    m_onOneWayPlatform = false;
    m_dropThroughIgnoreTimer = 0.0f;

    m_state = PlayerState::IDLE;
    m_stateTimer = 0.0f;
    m_animTimer = 0.0f;
    m_walkFrame = 0;

    m_invulnerableTimer = 0.0f;
    m_wrathAvailable = true;
    m_wrathTimer = 0.0f;
    m_wrathTriggeredWipe = false;
    m_screenFlashAlpha = 0.0f;

    m_nails.clear();
    m_meleeHitbox = { { 0, 0, 0, 0 }, false, 0.0f };
    m_isPerformingMelee = false;
}

Rectangle Player::GetHurtbox() const {
    if (m_state == PlayerState::CROUCH || m_state == PlayerState::ATTACK_CROUCH) {
        return { m_position.x + 4.0f, m_position.y + 12.0f, 16.0f, 18.0f };
    }
    return { m_position.x + 4.0f, m_position.y + 2.0f, 16.0f, 28.0f };
}

void Player::TakeDamage(int damage, bool inBossFight) {
    if (m_invulnerableTimer > 0.0f || m_state == PlayerState::DIVINE_WRATH || m_state == PlayerState::DEAD) {
        return;
    }

    if (inBossFight) {
        m_bossHitsTaken++;
        // Every other hit taken on the boss level has a 50% chance to permanently shatter a heart
        if ((m_bossHitsTaken % 2 == 0) && (m_shatteredHearts < PLAYER_MAX_HP - 1)) {
            if (GetRandomValue(0, 1) == 0) {
                m_shatteredHearts++;
                AudioManager::PlaySfx(SFX_SHIELD_DEFLECT);
            }
        }
    }

    m_hp -= damage;
    int effMax = GetEffectiveMaxHp();
    if (m_hp > effMax) m_hp = effMax;

    AudioManager::PlaySfx(SFX_PLAYER_HURT);

    if (m_hp <= 0) {
        m_hp = 0;
        m_lives--;
        if (m_lives <= 0) {
            m_state = PlayerState::DEAD;
            m_stateTimer = 0.0f;
        } else {
            // Respawn with restored health (capped by shattered hearts) and renewed wrath
            m_hp = GetEffectiveMaxHp();
            m_wrathAvailable = true;
            if (inBossFight) {
                m_position = { ARENA_MIN_X + 24.0f, GROUND_Y - 30.0f };
                m_facingLeft = false;
            } else {
                m_position.y = GROUND_Y - 30.0f;
            }
            m_velocity = { 0.0f, 0.0f };
            m_invulnerableTimer = 2.5f;
            m_state = PlayerState::IDLE;
        }
    } else {
        // Knockback recoil
        m_state = PlayerState::HURT;
        m_stateTimer = 0.3f;
        m_invulnerableTimer = 1.8f;
        m_velocity.x = m_facingLeft ? 60.0f : -60.0f;
        m_velocity.y = -120.0f;
    }
}

void Player::RestoreHealth(int pips) {
    m_hp += pips;
    int effMax = GetEffectiveMaxHp();
    if (m_hp > effMax) m_hp = effMax;
    m_score += 500;
}

void Player::Resurrect(bool inBossArena) {
    m_hp = PLAYER_MAX_HP;
    m_lives = PLAYER_INITIAL_LIVES;
    m_shatteredHearts = 0;
    m_bossHitsTaken = 0;
    m_wrathAvailable = true;
    m_invulnerableTimer = 3.0f;
    m_state = PlayerState::IDLE;
    m_stateTimer = 0.0f;
    m_velocity = { 0.0f, 0.0f };
    m_dropThroughIgnoreTimer = 0.0f;
    m_nails.clear();
    m_meleeHitbox = { { 0, 0, 0, 0 }, false, 0.0f };
    m_isPerformingMelee = false;

    if (inBossArena) {
        m_position = { ARENA_MIN_X + 24.0f, GROUND_Y - 30.0f };
        m_facingLeft = false;
    } else {
        m_position = { 32.0f, GROUND_Y - 30.0f };
        m_facingLeft = false;
    }
}

void Player::FireNail(bool crouch) {
    PlayerNail nail;
    float spawnX = m_facingLeft ? m_position.x - 4.0f : m_position.x + 20.0f;
    float spawnY = crouch ? (m_position.y + 18.0f) : (m_position.y + 11.0f);
    nail.position = { spawnX, spawnY };
    nail.velocityX = m_facingLeft ? -NAIL_SPEED : NAIL_SPEED;
    nail.active = true;
    nail.crouchShot = crouch;
    nail.spinTimer = 0.0f;

    m_nails.push_back(nail);
    AudioManager::PlaySfx(SFX_NAIL_THROW);
}

void Player::TriggerMelee() {
    m_isPerformingMelee = true;
    m_meleeHitbox.active = true;
    m_meleeHitbox.timer = 0.22f;

    float hitX = m_facingLeft ? (m_position.x - 18.0f) : (m_position.x + 18.0f);
    float hitY = (m_state == PlayerState::CROUCH) ? (m_position.y + 12.0f) : (m_position.y + 4.0f);
    m_meleeHitbox.rect = { hitX, hitY, 20.0f, 22.0f };

    AudioManager::PlaySfx(SFX_MELEE_STRIKE);
}

void Player::StartDivineWrath() {
    m_wrathAvailable = false;
    m_state = PlayerState::DIVINE_WRATH;
    m_wrathTimer = 0.0f;
    m_wrathTriggeredWipe = false;
    m_screenFlashAlpha = 0.0f;
    m_velocity = { 0.0f, 0.0f };

    AudioManager::PlaySfx(SFX_HOLY_BEAM);
}

bool Player::IsWrathScreenFrozen() const {
    return m_state == PlayerState::DIVINE_WRATH;
}

void Player::UpdateWrath(float dt) {
    m_wrathTimer += dt;

    // Phase 1 (0.0s - 0.5s): Holy beam descends
    // Phase 2 (0.5s - 1.0s): Lion transformation and roar
    if (m_wrathTimer >= 0.5f && m_wrathTimer - dt < 0.5f) {
        AudioManager::PlaySfx(SFX_LION_ROAR);
    }

    // Phase 3 (1.0s): Full white screen flash & board wipe trigger
    if (m_wrathTimer >= 1.0f && !m_wrathTriggeredWipe) {
        m_wrathTriggeredWipe = true;
        m_screenFlashAlpha = 1.0f;
    }

    // Phase 4 (1.25s): Resume gameplay while white flash fades out over 1.5 - 2.0s
    if (m_wrathTimer >= 1.25f) {
        m_state = PlayerState::IDLE;
        m_stateTimer = 0.0f;
    }
}

void Player::Update(float dt, const InputState& input, Level& level, float closestEnemyDistX, bool enemyInFront) {
    // Smoothly fade out screen flash back to normal colors over ~1.8 seconds
    if (m_screenFlashAlpha > 0.0f) {
        m_screenFlashAlpha -= dt * 0.55f; // ~1.82 seconds total fade duration
        if (m_screenFlashAlpha < 0.0f) m_screenFlashAlpha = 0.0f;
    }

    // Invulnerability timer
    if (m_invulnerableTimer > 0.0f) {
        m_invulnerableTimer -= dt;
    }

    // Drop through timer
    if (m_dropThroughIgnoreTimer > 0.0f) {
        m_dropThroughIgnoreTimer -= dt;
    }

    // Handle Divine Wrath state
    if (m_state == PlayerState::DIVINE_WRATH) {
        UpdateWrath(dt);
        return;
    }

    // Handle Ascension finale state
    if (m_state == PlayerState::ASCENDING) {
        UpdateAscension(dt);
        return;
    }

    // Update active nails
    for (auto& nail : m_nails) {
        if (nail.active) {
            nail.position.x += nail.velocityX * dt;
            nail.spinTimer += dt;
            // Despawn off screen bounds
            if (nail.position.x < m_position.x - 240.0f || nail.position.x > m_position.x + 240.0f) {
                nail.active = false;
            }
        }
    }
    // Cleanup inactive nails
    m_nails.erase(std::remove_if(m_nails.begin(), m_nails.end(), [](const PlayerNail& n) { return !n.active; }), m_nails.end());

    // Update melee hitbox timer
    if (m_meleeHitbox.active) {
        m_meleeHitbox.timer -= dt;
        if (m_meleeHitbox.timer <= 0.0f) {
            m_meleeHitbox.active = false;
            m_isPerformingMelee = false;
        }
    }

    // Handle hurt recovery
    if (m_state == PlayerState::HURT) {
        m_stateTimer -= dt;
        m_velocity.y += GRAVITY * dt;
        m_position.x += m_velocity.x * dt;
        m_position.y += m_velocity.y * dt;

        float groundY = 0.0f;
        if (level.IsOnGround(GetHurtbox(), groundY)) {
            m_position.y = groundY - 30.0f;
            m_velocity.y = 0.0f;
            m_onGround = true;
        }
        if (m_stateTimer <= 0.0f) {
            m_state = PlayerState::IDLE;
        }
        return;
    }

    // Check Divine Wrath input
    if (input.wrathPressed && m_wrathAvailable && m_state != PlayerState::DIVINE_WRATH) {
        StartDivineWrath();
        return;
    }

    // Save previous Y for one-way platform landing check
    float prevY = m_position.y;

    // --- State & Movement Logic ---
    bool isAirborne = (m_state == PlayerState::JUMP || m_state == PlayerState::HIGH_JUMP || m_state == PlayerState::DROP_THROUGH || !m_onGround);

    // Attack Action
    if (input.attackPressed) {
        bool inCloseRange = (closestEnemyDistX < MELEE_TRIGGER_DISTANCE && enemyInFront);

        if (inCloseRange) {
            TriggerMelee();
            if (m_state == PlayerState::CROUCH || (input.down && m_onGround)) m_state = PlayerState::ATTACK_CROUCH;
            else if (isAirborne) m_state = PlayerState::ATTACK_AIR;
            else m_state = PlayerState::ATTACK_STAND;
            m_stateTimer = 0.22f;
        } else {
            bool isCrouched = (m_state == PlayerState::CROUCH || (input.down && m_onGround));
            FireNail(isCrouched);
            if (isCrouched) m_state = PlayerState::ATTACK_CROUCH;
            else if (isAirborne) m_state = PlayerState::ATTACK_AIR;
            else m_state = PlayerState::ATTACK_STAND;
            m_stateTimer = 0.22f;
        }
    }

    // Recover from attack states
    if (m_state == PlayerState::ATTACK_STAND || m_state == PlayerState::ATTACK_CROUCH || m_state == PlayerState::ATTACK_AIR) {
        m_stateTimer -= dt;
        if (m_stateTimer <= 0.0f) {
            m_state = m_onGround ? (input.down ? PlayerState::CROUCH : PlayerState::IDLE) : PlayerState::JUMP;
        }
    }

    // Jump handling (when on ground)
    if (m_onGround && input.jumpPressed) {
        bool wantsDropThrough = (input.down || m_state == PlayerState::CROUCH || m_state == PlayerState::ATTACK_CROUCH);
        bool isOnDroppableSurface = (m_onOneWayPlatform || level.IsDropThroughPossible(GetHurtbox())) && (m_position.y + 30.0f < GROUND_Y - 6.0f);

        if (input.up) {
            // Dual-Plane High Jump (Super-leap to reach upper platform tier)
            m_velocity.y = PLAYER_HIGH_JUMP_SPEED;
            m_state = PlayerState::HIGH_JUMP;
            m_onGround = false;
            m_onOneWayPlatform = false;
            AudioManager::PlaySfx(SFX_HIGH_JUMP);
        } else if (wantsDropThrough && isOnDroppableSurface) {
            // Drop Through one-way ledge down to lower tier / street
            m_dropThroughIgnoreTimer = 0.35f;
            m_position.y += 6.0f;
            m_velocity.y = 100.0f;
            m_state = PlayerState::DROP_THROUGH;
            m_onGround = false;
            m_onOneWayPlatform = false;
            AudioManager::PlaySfx(SFX_JUMP);
        } else {
            // Normal Jump (from standing, or jumping up out of crouch on solid ground)
            m_velocity.y = PLAYER_NORMAL_JUMP_SPEED;
            m_state = PlayerState::JUMP;
            m_onGround = false;
            m_onOneWayPlatform = false;
            AudioManager::PlaySfx(SFX_JUMP);
        }
    }

    // Movement & Crouch State Logic
    if (!m_onGround) {
        // Airborne movement: can steer in midair unless in hit recoil or dead
        if (m_state != PlayerState::HURT && m_state != PlayerState::DEAD) {
            if (input.left) {
                m_velocity.x = -PLAYER_WALK_SPEED;
                m_facingLeft = true;
            } else if (input.right) {
                m_velocity.x = PLAYER_WALK_SPEED;
                m_facingLeft = false;
            } else {
                m_velocity.x = 0.0f;
            }
            if (m_state != PlayerState::HIGH_JUMP && m_state != PlayerState::DROP_THROUGH && 
                m_state != PlayerState::ATTACK_AIR && m_state != PlayerState::ATTACK_STAND && m_state != PlayerState::ATTACK_CROUCH) {
                m_state = PlayerState::JUMP;
            }
        }
    } else {
        // On Ground
        if (input.down && !input.jumpPressed && m_state != PlayerState::ATTACK_STAND) {
            // Down held: Crouch!
            if (m_state != PlayerState::ATTACK_CROUCH) {
                m_state = PlayerState::CROUCH;
            }
            m_velocity.x = 0.0f;
            // Can aim/face left or right while crouched
            if (input.left) {
                m_facingLeft = true;
            } else if (input.right) {
                m_facingLeft = false;
            }
        } else {
            // Not holding down: Stand up if currently crouched!
            if (m_state == PlayerState::CROUCH) {
                m_state = PlayerState::IDLE;
            }

            // Normal ground movement
            if (m_state != PlayerState::ATTACK_CROUCH) {
                if (input.left) {
                    m_velocity.x = -PLAYER_WALK_SPEED;
                    m_facingLeft = true;
                    if (m_state != PlayerState::ATTACK_STAND) {
                        m_state = PlayerState::WALK;
                    }
                } else if (input.right) {
                    m_velocity.x = PLAYER_WALK_SPEED;
                    m_facingLeft = false;
                    if (m_state != PlayerState::ATTACK_STAND) {
                        m_state = PlayerState::WALK;
                    }
                } else {
                    m_velocity.x = 0.0f;
                    if (m_state != PlayerState::ATTACK_STAND) {
                        m_state = PlayerState::IDLE;
                    }
                }
            } else {
                m_velocity.x = 0.0f;
            }
        }
    }

    // Apply gravity
    if (!m_onGround) {
        m_velocity.y += GRAVITY * dt;
        if (m_velocity.y > MAX_FALL_SPEED) m_velocity.y = MAX_FALL_SPEED;
    }

    // Integrate Position
    m_position.x += m_velocity.x * dt;
    m_position.y += m_velocity.y * dt;

    // Check Wall / World Bounds Collision
    float adjustedX = m_position.x;
    if (level.CheckWallCollision(GetHurtbox(), adjustedX, m_facingLeft ? -1 : 1)) {
        m_position.x = adjustedX;
    }

    // Check Ground / Platform Landing
    m_onGround = false;

    // 1. One-way upper platforms (only when descending/resting and not dropping through)
    if (m_velocity.y >= 0.0f && m_dropThroughIgnoreTimer <= 0.0f) {
        float ledgeY = 0.0f;
        if (level.CheckOneWayLanding(GetHurtbox(), prevY, ledgeY)) {
            m_position.y = ledgeY - 30.0f;
            m_velocity.y = 0.0f;
            m_onGround = true;
            m_onOneWayPlatform = true;
            if (m_state == PlayerState::JUMP || m_state == PlayerState::HIGH_JUMP || m_state == PlayerState::DROP_THROUGH) {
                m_state = input.down ? PlayerState::CROUCH : (m_velocity.x != 0.0f ? PlayerState::WALK : PlayerState::IDLE);
            }
        }
    }

    // 2. Solid Wooden Crates (can jump on and stand on top of them; cannot drop through)
    if (!m_onGround && m_velocity.y >= 0.0f) {
        float crateLedgeY = 0.0f;
        if (level.CheckCrateLanding(GetHurtbox(), prevY, crateLedgeY)) {
            m_position.y = crateLedgeY - 30.0f;
            m_velocity.y = 0.0f;
            m_onGround = true;
            m_onOneWayPlatform = false; // Crates are solid boxes, cannot be dropped through!
            if (m_state == PlayerState::JUMP || m_state == PlayerState::HIGH_JUMP || m_state == PlayerState::DROP_THROUGH) {
                m_state = input.down ? PlayerState::CROUCH : (m_velocity.x != 0.0f ? PlayerState::WALK : PlayerState::IDLE);
            }
        }
    }

    // 3. Base Ground
    float groundY = 0.0f;
    if (!m_onGround && level.IsOnGround(GetHurtbox(), groundY)) {
        m_position.y = groundY - 30.0f;
        m_velocity.y = 0.0f;
        m_onGround = true;
        m_onOneWayPlatform = false;
        if (m_state == PlayerState::JUMP || m_state == PlayerState::HIGH_JUMP || m_state == PlayerState::DROP_THROUGH) {
            m_state = input.down ? PlayerState::CROUCH : (m_velocity.x != 0.0f ? PlayerState::WALK : PlayerState::IDLE);
        }
    }

    // If walked or dropped off a ledge without jumping, enter falling jump state
    if (!m_onGround) {
        m_onOneWayPlatform = false;
        if (m_state == PlayerState::WALK || m_state == PlayerState::IDLE || m_state == PlayerState::CROUCH) {
            m_state = PlayerState::JUMP;
        }
    }

    // Update animations
    m_animTimer += dt;
    if (m_state == PlayerState::WALK) {
        if (m_animTimer >= 0.12f) {
            m_animTimer = 0.0f;
            m_walkFrame = (m_walkFrame + 1) % 4;
        }
    } else {
        m_walkFrame = 0;
    }

    // Pickups check
    if (level.CheckBasketPickup(GetHurtbox())) {
        RestoreHealth(1);
        AudioManager::PlaySfx(SFX_BASKET_PICKUP);
    }
}

void Player::Draw() {
    // Invulnerability flicker
    if (m_invulnerableTimer > 0.0f) {
        int flicker = static_cast<int>(m_invulnerableTimer * 20.0f);
        if (flicker % 2 == 0) return;
    }

    // Draw active nails with radiant divine cometary trail
    for (const auto& nail : m_nails) {
        if (nail.active) {
            float dir = nail.velocityX > 0 ? 1.0f : -1.0f;
            float tailX = nail.position.x - dir * 18.0f;
            float headX = nail.position.x + dir * 8.0f;
            float py = nail.position.y + 6.0f;

            // Radiant Holy Trail (Outer golden aura + inner brilliant core)
            DrawLineEx({ tailX, py }, { headX, py }, 4.5f, Color{ 255, 215, 60, 160 });
            DrawLineEx({ tailX + dir * 6.0f, py }, { headX, py }, 2.0f, Color{ 255, 255, 240, 240 });

            // Trailing spark halo
            DrawCircleGradient(static_cast<int>(nail.position.x + 9.0f), static_cast<int>(py), 7.0f, Color{ 255, 240, 130, 140 }, Color{ 255, 180, 40, 0 });

            // Sprite
            GraphicsManager::DrawSprite(SPR_NAIL, nail.position.x, nail.position.y, nail.velocityX < 0);
        }
    }

    // Draw Melee strike slash effect
    if (m_isPerformingMelee && m_meleeHitbox.active) {
        float progress = 1.0f - (m_meleeHitbox.timer / 0.22f);
        GraphicsManager::DrawMeleeSlashEffect(m_position.x + (m_facingLeft ? 4.0f : 20.0f), m_position.y + 12.0f, m_facingLeft, progress);
    }

    // Divine Wrath Visuals
    if (m_state == PlayerState::DIVINE_WRATH) {
        // Draw celestial beam descending
        float beamAlpha = 0.0f;
        if (m_wrathTimer < 0.5f) {
            beamAlpha = m_wrathTimer / 0.5f;
        } else if (m_wrathTimer < 1.1f) {
            beamAlpha = 1.0f;
        } else {
            beamAlpha = 1.0f - (m_wrathTimer - 1.1f) / 0.4f;
        }
        GraphicsManager::DrawDivineBeam(m_position.x + 12.0f, beamAlpha);

        // Transformation to Lion of Judah
        if (m_wrathTimer >= 0.5f && m_wrathTimer < 1.35f) {
            GraphicsManager::DrawDropShadow(m_position.x + 12.0f, m_position.y + 31.0f, 26.0f, 6.0f);
            GraphicsManager::DrawDivineLionAura(m_position.x + 12.0f, m_position.y + 12.0f, m_wrathTimer);
            GraphicsManager::DrawSprite(SPR_LION_OF_JUDAH, m_position.x - 4.0f, m_position.y - 2.0f, m_facingLeft);
            return;
        }
    }

    // Select sprite based on state
    SpriteId spr = SPR_PLAYER_IDLE;
    switch (m_state) {
        case PlayerState::IDLE:
            spr = SPR_PLAYER_IDLE;
            break;
        case PlayerState::WALK:
            spr = static_cast<SpriteId>(SPR_PLAYER_WALK_0 + m_walkFrame);
            break;
        case PlayerState::CROUCH:
            spr = SPR_PLAYER_CROUCH;
            break;
        case PlayerState::JUMP:
        case PlayerState::DROP_THROUGH:
            spr = SPR_PLAYER_JUMP;
            break;
        case PlayerState::HIGH_JUMP: {
            // Spinning somersault
            int somersaultFrame = static_cast<int>(m_animTimer * 16.0f) % 4;
            spr = static_cast<SpriteId>(SPR_PLAYER_SOMERSAULT_0 + somersaultFrame);
            break;
        }
        case PlayerState::ATTACK_STAND:
            spr = m_isPerformingMelee ? SPR_PLAYER_MELEE_STRIKE : SPR_PLAYER_ATTACK_STAND;
            break;
        case PlayerState::ATTACK_CROUCH:
            spr = SPR_PLAYER_ATTACK_CROUCH;
            break;
        case PlayerState::ATTACK_AIR:
            spr = m_isPerformingMelee ? SPR_PLAYER_MELEE_STRIKE : SPR_PLAYER_ATTACK_STAND;
            break;
        case PlayerState::HURT:
            spr = SPR_PLAYER_HURT;
            break;
        case PlayerState::ASCENDING:
            spr = SPR_PLAYER_ASCEND;
            break;
        default:
            spr = SPR_PLAYER_IDLE;
            break;
    }

    // Ground drop shadow (omitted during holy ascension and death)
    if (m_state != PlayerState::ASCENDING && m_state != PlayerState::DEAD) {
        float shadowY = m_position.y + 31.0f;
        GraphicsManager::DrawDropShadow(m_position.x + 12.0f, shadowY, 20.0f, 5.0f);
    }

    GraphicsManager::DrawSprite(spr, m_position.x, m_position.y, m_facingLeft);

    // If Ascending: Draw divine golden halo around head and ascending light motes
    if (m_state == PlayerState::ASCENDING) {
        float haloPulse = 0.85f + 0.15f * sinf(m_stateTimer * 6.0f);
        DrawCircleGradient(static_cast<int>(m_position.x + 12.0f), static_cast<int>(m_position.y + 6.0f), 18.0f * haloPulse, ColorAlpha(COLOR_GOLD_BRIGHT, 0.5f), ColorAlpha(COLOR_GOLD_BRIGHT, 0.0f));
    }
}

void Player::StartAscension() {
    m_state = PlayerState::ASCENDING;
    m_stateTimer = 0.0f;
    m_invulnerableTimer = 9999.0f;
    m_velocity = { 0.0f, 0.0f };
    m_facingLeft = false; // Turn majestically towards heaven
}

void Player::UpdateAscension(float dt) {
    if (m_state != PlayerState::ASCENDING) return;
    m_stateTimer += dt;
    // For first 1.2s: Jesus stands solemnly enveloped in the holy beam
    // After 1.2s: Jesus smoothly begins ascending upward through the beam
    if (m_stateTimer > 1.2f) {
        float ascendSpeed = 26.0f + (m_stateTimer - 1.2f) * 12.0f;
        m_position.y -= ascendSpeed * dt;
    }
}

