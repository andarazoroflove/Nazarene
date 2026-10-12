#include "gba_game.h"
#include <stdlib.h>

static GameState s_state = GAME_STATE_TITLE;
static int s_credits = 2;
static int s_title_timer = 0;
static int s_camera_x = 0;

static int s_mission = 1;
static bool s_m1_cleared = false;
static bool s_m2_cleared = false;
static bool s_m3_cleared = false;
static int s_map_timer = 0;

static GbaPlayer s_player;
static GbaNail s_nails[4];
static GbaEnemy s_enemies[14];
static int s_enemy_count = 0;
static GbaBasket s_baskets[4];
static GbaPlatform s_platforms[18];
static int s_platform_count = 0;

static int s_continue_timer = 0;
static int s_continue_countdown = 9;
static int s_boss_cloud_timer = 0;
static bool s_gate_open = false;

static void reset_level(bool resetLives) {
    s_player.x = 24;
    s_player.y = GBA_GROUND_Y - 26;
    s_player.vx = 0;
    s_player.vy = 0;
    s_player.facingLeft = false;
    s_player.onGround = true;
    s_player.onOneWay = false;
    s_player.state = 0;
    s_player.stateTimer = 0;
    s_player.animTimer = 0;
    s_player.walkFrame = 0;
    s_player.hp = 3;
    s_player.shatteredHearts = 0;
    s_player.bossHitsTaken = 0;
    s_player.hasWrath = true;
    s_player.wrathTimer = 0;
    s_player.invulnTimer = 0;
    s_player.meleeActive = false;
    s_player.meleeTimer = 0;
    if (resetLives) {
        s_player.lives = 3;
        s_player.score = 0;
    }

    for (int i = 0; i < 4; ++i) s_nails[i].active = false;

    s_platform_count = 0;
    s_enemy_count = 0;
    s_gate_open = false;

    if (s_mission == 2) {
        // ==============================================================
        // GBA STAGE 2: INSIDE THE TEMPLE OF JERUSALEM
        // Two-level platforms (Tier 1 at 100, Tier 2 at 64), Censers, Sicarii
        // ==============================================================
        // Place 4 Sacred Censers
        s_baskets[0] = (GbaBasket){ 320, 100 - 14, false };
        s_baskets[1] = (GbaBasket){ 660, 64 - 14, false };
        s_baskets[2] = (GbaBasket){ 1100, 64 - 14, false };
        s_baskets[3] = (GbaBasket){ 1500, 100 - 14, false };

        // Two-Level Platform Layout (Tier 1 Y=100, Tier 2 Y=64)
        s_platforms[s_platform_count++] = (GbaPlatform){ 200, 100, 180, 6, true };
        s_platforms[s_platform_count++] = (GbaPlatform){ 260, 64, 140, 6, true };
        s_platforms[s_platform_count++] = (GbaPlatform){ 520, 100, 200, 6, true };
        s_platforms[s_platform_count++] = (GbaPlatform){ 580, 64, 160, 6, true };
        s_platforms[s_platform_count++] = (GbaPlatform){ 940, 100, 220, 6, true };
        s_platforms[s_platform_count++] = (GbaPlatform){ 1020, 64, 160, 6, true };
        s_platforms[s_platform_count++] = (GbaPlatform){ 1360, 100, 200, 6, true };
        s_platforms[s_platform_count++] = (GbaPlatform){ 1420, 64, 150, 6, true };

        // Step blocks
        s_platforms[s_platform_count++] = (GbaPlatform){ 420, GBA_CRATE_Y, 36, 28, false };
        s_platforms[s_platform_count++] = (GbaPlatform){ 860, GBA_CRATE_Y, 36, 28, false };
        s_platforms[s_platform_count++] = (GbaPlatform){ 1260, GBA_CRATE_Y, 36, 28, false };

        // Enemies: Sicarii Assassins (type=4), Guards, Archers, Malchus (type=3)
        // 1. Sicarii Assassin on Entrance Wall (Clinging, type=4)
        s_enemies[s_enemy_count++] = (GbaEnemy){ 260, 36, 4, 1, 1, true, true, 0, 0, 0, false, false, 0, 0, 0 };
        // 2. Heavy Temple Guard
        s_enemies[s_enemy_count++] = (GbaEnemy){ 360, GBA_GROUND_Y - 26, 2, 2, 2, true, true, 0, 0, 0, false, false, 0, 0, 0 };
        // 3. High Tier Archer (at Y=64)
        s_enemies[s_enemy_count++] = (GbaEnemy){ 620, 64 - 26, 1, 1, 1, true, true, 0, 0, 0, false, false, 0, 0, 0 };
        // 4. Sicarii on Column
        s_enemies[s_enemy_count++] = (GbaEnemy){ 700, 40, 4, 1, 1, true, true, 0, 0, 0, false, false, 0, 0, 0 };
        // 5. Tier 1 Guard
        s_enemies[s_enemy_count++] = (GbaEnemy){ 980, 100 - 26, 0, 1, 1, true, true, 0, 0, 0, false, false, 0, 0, 0 };
        // 6. Heavy Guard
        s_enemies[s_enemy_count++] = (GbaEnemy){ 1180, GBA_GROUND_Y - 26, 2, 2, 2, true, true, 0, 0, 0, false, false, 0, 0, 0 };
        // 7. High Sicarii Assassin
        s_enemies[s_enemy_count++] = (GbaEnemy){ 1380, 36, 4, 1, 1, true, true, 0, 0, 0, false, false, 0, 0, 0 };
        // 8. Tier 1 Archer
        s_enemies[s_enemy_count++] = (GbaEnemy){ 1460, 100 - 26, 1, 1, 1, true, true, 0, 0, 0, false, false, 0, 0, 0 };
        // 9. Stage 2 Boss: Commander Malchus
        s_enemies[s_enemy_count++] = (GbaEnemy){ 1980, GBA_GROUND_Y - 42, 3, 12, 12, true, true, 0, 0, 0, false, false, 0, 0, 0 };
    } else {
        // Stage 1 (or 3): Jerusalem Outskirts / Streets
        s_baskets[0] = (GbaBasket){ 340, GBA_GROUND_Y - 14, false };
        s_baskets[1] = (GbaBasket){ 680, GBA_UPPER_Y - 14, false };
        s_baskets[2] = (GbaBasket){ 1120, GBA_UPPER_Y - 14, false };
        s_baskets[3] = (GbaBasket){ 1540, GBA_GROUND_Y - 14, false };

        s_platforms[s_platform_count++] = (GbaPlatform){ 220, GBA_UPPER_Y, 200, 8, true };
        s_platforms[s_platform_count++] = (GbaPlatform){ 540, GBA_UPPER_Y, 240, 8, true };
        s_platforms[s_platform_count++] = (GbaPlatform){ 980, GBA_UPPER_Y, 260, 8, true };
        s_platforms[s_platform_count++] = (GbaPlatform){ 1380, GBA_UPPER_Y, 220, 8, true };

        s_platforms[s_platform_count++] = (GbaPlatform){ 450, GBA_CRATE_Y, 40, 28, false };
        s_platforms[s_platform_count++] = (GbaPlatform){ 880, GBA_CRATE_Y, 48, 28, false };
        s_platforms[s_platform_count++] = (GbaPlatform){ 1280, GBA_CRATE_Y, 40, 28, false };

        s_enemies[s_enemy_count++] = (GbaEnemy){ 280, GBA_GROUND_Y - 26, 0, 1, 1, true, true, 0, 0, 0, false, false, 0, 0, 0 };
        s_enemies[s_enemy_count++] = (GbaEnemy){ 620, GBA_UPPER_Y - 26, 0, 1, 1, true, true, 0, 0, 0, false, false, 0, 0, 0 };
        s_enemies[s_enemy_count++] = (GbaEnemy){ 1040, GBA_UPPER_Y - 26, 0, 1, 1, true, true, 0, 0, 0, false, false, 0, 0, 0 };
        s_enemies[s_enemy_count++] = (GbaEnemy){ 1420, GBA_GROUND_Y - 26, 0, 1, 1, true, true, 0, 0, 0, false, false, 0, 0, 0 };

        s_enemies[s_enemy_count++] = (GbaEnemy){ 480, GBA_GROUND_Y - 26, 1, 1, 1, true, true, 0, 0, 0, false, true, 0, 0, 0 };
        s_enemies[s_enemy_count++] = (GbaEnemy){ 720, GBA_UPPER_Y - 26, 1, 1, 1, true, true, 0, 0, 0, false, false, 0, 0, 0 };
        s_enemies[s_enemy_count++] = (GbaEnemy){ 1200, GBA_UPPER_Y - 26, 1, 1, 1, true, true, 0, 0, 0, false, true, 0, 0, 0 };
        s_enemies[s_enemy_count++] = (GbaEnemy){ 1620, GBA_GROUND_Y - 26, 1, 1, 1, true, true, 0, 0, 0, false, false, 0, 0, 0 };

        s_enemies[s_enemy_count++] = (GbaEnemy){ 820, GBA_GROUND_Y - 26, 2, 2, 2, true, true, 0, 0, 0, false, false, 0, 0, 0 };
        s_enemies[s_enemy_count++] = (GbaEnemy){ 1340, GBA_GROUND_Y - 26, 2, 2, 2, true, true, 0, 0, 0, false, false, 0, 0, 0 };

        s_enemies[s_enemy_count++] = (GbaEnemy){ 1980, GBA_GROUND_Y - 42, 3, 10, 10, true, true, 0, 0, 0, false, false, 0, 0, 0 };
    }

    s_camera_x = 0;
    gba_audio_play_bgm(BGM_STAGE);
}

void gba_game_init(void) {
    s_state = GAME_STATE_TITLE;
    s_credits = 2;
    s_title_timer = 0;
    gba_audio_stop_bgm();
}

static void fire_nail(bool crouch) {
    for (int i = 0; i < 4; ++i) {
        if (!s_nails[i].active) {
            s_nails[i].active = true;
            s_nails[i].x = s_player.facingLeft ? (s_player.x - 4) : (s_player.x + 18);
            s_nails[i].y = crouch ? (s_player.y + 16) : (s_player.y + 8);
            s_nails[i].vx = s_player.facingLeft ? -6 : 6;
            gba_audio_play_sfx(SFX_NAIL_THROW);
            break;
        }
    }
}

static void trigger_divine_wrath(void) {
    if (!s_player.hasWrath) return;
    s_player.hasWrath = false;
    s_player.state = 8; // Lion of Judah
    s_player.wrathTimer = 60; // 1 second screen freeze
    gba_audio_play_sfx(SFX_DIVINE_WRATH);

    // Wipe all visible regular enemies on screen
    for (int i = 0; i < s_enemy_count; ++i) {
        if (!s_enemies[i].active || s_enemies[i].type == 3) continue;
        int sx = s_enemies[i].x - s_camera_x;
        if (sx >= -20 && sx <= GBA_SCREEN_WIDTH + 20) {
            s_enemies[i].active = false;
            s_player.score += 200;
        }
    }

    // Heavy damage to Boss Brutus if visible
    for (int i = 0; i < s_enemy_count; ++i) {
        if (s_enemies[i].type == 3 && s_enemies[i].active) {
            s_enemies[i].hp -= 3;
            s_enemies[i].hurtTimer = 15;
            if (s_enemies[i].hp <= 0) s_enemies[i].hp = 1; // Leave for final strike
        }
    }
}

void gba_game_update(uint16_t keysDown, uint16_t keysPressed) {
    gba_audio_update();

    if (s_state == GAME_STATE_TITLE) {
        s_title_timer++;
        if (keysPressed & KEY_SELECT) {
            s_credits++;
            gba_audio_play_sfx(SFX_COIN);
        }
        if ((keysPressed & KEY_START) || (keysPressed & KEY_A)) {
            if (s_credits > 0) {
                s_credits--;
                s_mission = 1;
                s_m1_cleared = false;
                s_m2_cleared = false;
                s_m3_cleared = false;
                s_state = GAME_STATE_MISSION_MAP;
                s_map_timer = 0;
                gba_audio_play_sfx(SFX_COIN);
            }
        }
        return;
    }

    if (s_state == GAME_STATE_MISSION_MAP) {
        s_map_timer++;
        if (s_map_timer > 30 && ((keysPressed & (KEY_START | KEY_A | KEY_B)) || s_map_timer >= 240)) {
            reset_level(s_mission == 1);
            s_state = GAME_STATE_PLAYING;
            gba_audio_play_sfx(SFX_COIN);
        }
        return;
    }

    if (s_state == GAME_STATE_CONTINUE) {
        s_continue_timer++;
        if (s_continue_timer >= 60) {
            s_continue_timer = 0;
            s_continue_countdown--;
            if (s_continue_countdown <= 0) {
                s_state = GAME_STATE_GAME_OVER;
                gba_audio_stop_bgm();
            }
        }
        if (keysPressed & KEY_SELECT) {
            s_credits++;
            gba_audio_play_sfx(SFX_COIN);
        }
        if ((keysPressed & KEY_START) || (keysPressed & KEY_A)) {
            if (s_credits > 0) {
                s_credits--;
                s_state = GAME_STATE_PLAYING;
                reset_level(false);
            }
        }
        return;
    }

    if (s_state == GAME_STATE_GAME_OVER) {
        if ((keysPressed & KEY_START) || (keysPressed & KEY_A)) {
            s_state = GAME_STATE_TITLE;
        }
        return;
    }

    if (s_state == GAME_STATE_VICTORY_ASCENSION) {
        s_player.stateTimer++;
        s_player.state = 9; // Ascending
        if (s_player.stateTimer > 40 && s_player.y > -40) {
            s_player.y -= 1; // Ascend smoothly to heaven
        }
        if (s_player.stateTimer > 180) {
            if (s_mission == 1) {
                s_m1_cleared = true;
                s_mission = 2;
                s_state = GAME_STATE_MISSION_MAP;
                s_map_timer = 0;
            } else if (s_mission == 2) {
                s_m2_cleared = true;
                s_mission = 3;
                s_state = GAME_STATE_MISSION_MAP;
                s_map_timer = 0;
            } else {
                s_m3_cleared = true;
                s_state = GAME_STATE_TITLE;
            }
        }
        return;
    }

    // -----------------------------------------------------------------------
    // GAMEPLAY UPDATE
    // -----------------------------------------------------------------------

    // Check Divine Wrath timer
    if (s_player.state == 8) {
        s_player.wrathTimer--;
        if (s_player.wrathTimer <= 0) {
            s_player.state = 0; // Return to idle
        }
        return; // Screen frozen during wrath
    }

    // Invulnerability timer
    if (s_player.invulnTimer > 0) s_player.invulnTimer--;

    // Melee attack timer
    if (s_player.meleeActive) {
        s_player.meleeTimer--;
        if (s_player.meleeTimer <= 0) s_player.meleeActive = false;
    }

    // Trigger Divine Wrath (L or R trigger or Select)
    if ((keysPressed & KEY_L) || (keysPressed & KEY_R)) {
        trigger_divine_wrath();
    }

    // Player Horizontal Movement
    int moveX = 0;
    if (keysDown & KEY_LEFT) {
        moveX = -2;
        s_player.facingLeft = true;
    } else if (keysDown & KEY_RIGHT) {
        moveX = 2;
        s_player.facingLeft = false;
    }

    // Crouch
    bool isCrouched = (keysDown & KEY_DOWN) && s_player.onGround;

    // Jump & Somersault High Jump
    if ((keysPressed & KEY_A) && s_player.onGround) {
        if (keysDown & KEY_DOWN) {
            // Drop through platform
            if (s_player.onOneWay) {
                s_player.y += 4;
                s_player.onGround = false;
            }
        } else if (keysDown & KEY_UP) {
            // Somersault High Jump between tiers!
            s_player.vy = -7;
            s_player.onGround = false;
            s_player.state = 4; // Somersault
            gba_audio_play_sfx(SFX_JUMP);
        } else {
            // Normal Jump
            s_player.vy = -5;
            s_player.onGround = false;
            s_player.state = 3; // Jump
            gba_audio_play_sfx(SFX_JUMP);
        }
    }

    // Attack (B button)
    if (keysPressed & KEY_B) {
        // Check if enemy is within close melee range (< 24 pixels)
        bool meleeTriggered = false;
        for (int i = 0; i < s_enemy_count; ++i) {
            if (!s_enemies[i].active) continue;
            int dist = abs(s_enemies[i].x - s_player.x);
            if (dist < 26 && abs(s_enemies[i].y - s_player.y) < 24) {
                meleeTriggered = true;
                s_player.meleeActive = true;
                s_player.meleeTimer = 12;
                s_player.state = 6; // Melee strike
                gba_audio_play_sfx(SFX_MELEE_SWING);

                // Hit enemy
                if (s_enemies[i].type == 2 && !s_enemies[i].facingLeft && s_player.facingLeft) {
                    // Heavy Legionary frontal block
                    gba_audio_play_sfx(SFX_SHIELD_DEFLECT);
                } else {
                    s_enemies[i].hp--;
                    s_enemies[i].hurtTimer = 10;
                    gba_audio_play_sfx(SFX_ENEMY_HIT);
                    if (s_enemies[i].hp <= 0 && s_enemies[i].type != 3) {
                        s_enemies[i].active = false;
                        s_player.score += 100;
                    }
                }
                break;
            }
        }

        if (!meleeTriggered) {
            s_player.state = isCrouched ? 5 : 5; // Throw
            fire_nail(isCrouched);
        }
    }

    // Apply movement & physics
    if (!isCrouched) {
        s_player.x += moveX;
        if (moveX != 0 && s_player.onGround) {
            s_player.state = 1; // Walk
            s_player.animTimer++;
            if (s_player.animTimer >= 6) {
                s_player.animTimer = 0;
                s_player.walkFrame = (s_player.walkFrame + 1) % 4;
            }
        } else if (s_player.onGround && !s_player.meleeActive) {
            s_player.state = 0; // Idle
        }
    } else {
        s_player.state = 2; // Crouch
    }

    // Bounds check with Arena Gate
    if (s_player.x < 12) s_player.x = 12;
    if (!s_gate_open && s_player.x > GBA_ARENA_GATE_X - 16) {
        s_player.x = GBA_ARENA_GATE_X - 16;
    }
    if (s_player.x > GBA_LEVEL_WIDTH - 24) s_player.x = GBA_LEVEL_WIDTH - 24;

    // Gravity
    if (!s_player.onGround) {
        s_player.y += s_player.vy;
        if (s_player.vy < 6) s_player.vy += 1;
    }

    // Collision with ground and platforms
    s_player.onGround = false;
    s_player.onOneWay = false;

    // Ground level
    if (s_player.y >= GBA_GROUND_Y - 26) {
        s_player.y = GBA_GROUND_Y - 26;
        s_player.vy = 0;
        s_player.onGround = true;
    }

    // Platforms
    for (int i = 0; i < s_platform_count; ++i) {
        GbaPlatform* p = &s_platforms[i];
        if (s_player.x + 14 >= p->x && s_player.x + 6 <= p->x + p->w) {
            if (s_player.vy >= 0 && (s_player.y + 26 >= p->y) && (s_player.y + 26 <= p->y + 8)) {
                s_player.y = p->y - 26;
                s_player.vy = 0;
                s_player.onGround = true;
                if (p->isOneWay) s_player.onOneWay = true;
                break;
            }
        }
    }

    // Update Nails
    for (int i = 0; i < 4; ++i) {
        if (!s_nails[i].active) continue;
        s_nails[i].x += s_nails[i].vx;
        if (s_nails[i].x < s_camera_x - 16 || s_nails[i].x > s_camera_x + GBA_SCREEN_WIDTH + 16) {
            s_nails[i].active = false;
            continue;
        }

        // Check collision with enemies
        for (int j = 0; j < s_enemy_count; ++j) {
            if (!s_enemies[j].active) continue;
            GbaEnemy* e = &s_enemies[j];
            int ew = (e->type == 3) ? 32 : 18;
            int eh = (e->type == 3) ? 42 : 26;
            if (s_nails[i].x >= e->x && s_nails[i].x <= e->x + ew &&
                s_nails[i].y >= e->y && s_nails[i].y <= e->y + eh) {
                s_nails[i].active = false;

                // Shield reflection check
                if (e->type == 2 && s_nails[i].vx > 0 && e->facingLeft) {
                    gba_audio_play_sfx(SFX_SHIELD_DEFLECT);
                    break;
                }

                // Boss Brutus headshot check!
                if (e->type == 3) {
                    if (s_nails[i].y <= e->y + 14) {
                        e->hp--;
                        e->hurtTimer = 10;
                        gba_audio_play_sfx(SFX_ENEMY_HIT);
                        s_player.score += 200;
                    } else {
                        // Body cuirass absorbs standard shot
                        gba_audio_play_sfx(SFX_SHIELD_DEFLECT);
                    }
                } else {
                    e->hp--;
                    e->hurtTimer = 10;
                    gba_audio_play_sfx(SFX_ENEMY_HIT);
                    if (e->hp <= 0) {
                        e->active = false;
                        s_player.score += 100;
                    }
                }
                break;
            }
        }
    }

    // Check Basket pickups
    int collectedCount = 0;
    for (int i = 0; i < 4; ++i) {
        if (s_baskets[i].collected) {
            collectedCount++;
            continue;
        }
        if (abs(s_player.x - s_baskets[i].x) < 18 && abs(s_player.y - s_baskets[i].y) < 24) {
            s_baskets[i].collected = true;
            collectedCount++;
            if (s_player.hp < 3 - s_player.shatteredHearts) s_player.hp++;
            s_player.score += 500;
            gba_audio_play_sfx(SFX_BASKET_PICKUP);
        }
    }

    if (collectedCount >= 4 && !s_gate_open) {
        s_gate_open = true;
    }

    // Update Enemies & Boss
    for (int i = 0; i < s_enemy_count; ++i) {
        if (!s_enemies[i].active) continue;
        GbaEnemy* e = &s_enemies[i];
        if (e->hurtTimer > 0) e->hurtTimer--;

        int distToPlayer = abs(e->x - s_player.x);

        if (e->type == 0) {
            // Patrol Guard: lunges forward with spear when close
            if (distToPlayer < 40) {
                e->isAttacking = true;
            } else {
                e->isAttacking = false;
                e->animTimer++;
                if (e->animTimer >= 8) {
                    e->animTimer = 0;
                    e->animFrame = (e->animFrame + 1) % 2;
                }
            }
        } else if (e->type == 1) {
            // Syrian Archer: ducks and peeks to fire
            e->attackTimer++;
            if (e->attackTimer > 120) {
                e->attackTimer = 0;
                e->isDucking = !e->isDucking;
            }
        } else if (e->type == 3) {
            // BOSS CENTURION GLADIATOR BRUTUS
            if (e->hp <= 1 && s_boss_cloud_timer == 0) {
                s_boss_cloud_timer = 1;
                gba_audio_play_sfx(SFX_FINISH_HIM);
            }

            if (e->hp <= 0) {
                e->bossState = 4; // Kneel
                e->bossStateTimer++;
                if (e->bossStateTimer > 60 && e->bossState == 4) {
                    e->bossState = 5; // Wither
                }
                if (e->bossStateTimer > 120 && e->bossState == 5) {
                    e->bossState = 6; // Ash
                    s_state = GAME_STATE_VICTORY_ASCENSION;
                    gba_audio_play_bgm(BGM_VICTORY);
                }
                continue;
            }

            // Normal Boss Combat Cycle
            e->attackTimer++;
            if (e->attackTimer < 60) {
                e->bossState = 0; // Stalk / Walk
                if (e->x > s_player.x + 40) e->x -= 1;
                else if (e->x < s_player.x - 40) e->x += 1;
            } else if (e->attackTimer < 80) {
                e->bossState = 1; // Whip Windup
            } else if (e->attackTimer < 100) {
                e->bossState = 2; // Whip Crack!
                if (e->attackTimer == 82) gba_audio_play_sfx(SFX_WHIP_CRACK);
                // Check if whip hits player (mid-height, crouching or high jump avoids!)
                if (s_player.state != 2 && s_player.y >= GBA_GROUND_Y - 28 && s_player.invulnTimer <= 0) {
                    if (s_player.x >= e->x - 80 && s_player.x <= e->x) {
                        s_player.hp--;
                        s_player.invulnTimer = 60;
                        s_player.bossHitsTaken++;
                        // Shattered hearts on every other boss hit
                        if ((s_player.bossHitsTaken % 2 == 0) && (s_player.shatteredHearts < 2)) {
                            s_player.shatteredHearts++;
                        }
                        gba_audio_play_sfx(SFX_PLAYER_HURT);
                        if (s_player.hp <= 0) {
                            s_player.lives--;
                            if (s_player.lives >= 0) {
                                s_state = GAME_STATE_CONTINUE;
                                s_continue_timer = 0;
                                s_continue_countdown = 9;
                                gba_audio_play_bgm(BGM_CONTINUE);
                            } else {
                                s_state = GAME_STATE_GAME_OVER;
                            }
                        }
                    }
                }
            } else {
                e->attackTimer = 0;
            }
        } else if (e->type == 4) {
            // Sicarii Shadow Assassin (Shinobi-style wall-clinging ninja Judean)
            if (e->bossState == 0) { // WALL_CLING
                if (distToPlayer < 75) {
                    e->bossState = 1; // Crawl down column
                }
            } else if (e->bossState == 1) { // WALL_CRAWL
                e->y += 2;
                e->animTimer++;
                if (e->animTimer >= 6) {
                    e->animTimer = 0;
                    e->animFrame = (e->animFrame + 1) % 2;
                }
                int landY = (s_mission == 2 && e->x < 1200) ? (100 - 26) : (GBA_GROUND_Y - 26);
                if (e->y >= landY) {
                    e->y = landY;
                    e->bossState = 2; // HOP
                }
            } else if (e->bossState == 2) { // HOP
                if (e->x > s_player.x) { e->x -= 2; e->facingLeft = true; }
                else { e->x += 2; e->facingLeft = false; }
                e->attackTimer++;
                if (e->attackTimer >= 20) {
                    e->bossState = 3; // STAND & STALK
                    e->attackTimer = 0;
                }
            } else { // STAND & STALK
                if (e->x > s_player.x + 30) { e->x -= 1; e->facingLeft = true; }
                else if (e->x < s_player.x - 30) { e->x += 1; e->facingLeft = false; }
                if (distToPlayer < 24 && s_player.invulnTimer <= 0) {
                    s_player.hp--;
                    s_player.invulnTimer = 40;
                    gba_audio_play_sfx(SFX_PLAYER_HURT);
                }
            }
        }
    }

    // Switch to Boss Music when entering Arena
    if (s_player.x >= GBA_ARENA_MIN_X) {
        gba_audio_play_bgm(BGM_BOSS);
    }

    // Smooth Camera Tracking (keep player centered)
    int targetCamX = s_player.x - (GBA_SCREEN_WIDTH / 2) + 12;
    if (targetCamX < 0) targetCamX = 0;
    if (targetCamX > GBA_LEVEL_WIDTH - GBA_SCREEN_WIDTH) targetCamX = GBA_LEVEL_WIDTH - GBA_SCREEN_WIDTH;
    s_camera_x = targetCamX;
}

void gba_game_draw(void) {
    if (s_state == GAME_STATE_TITLE) {
        // Title Screen with Judean Sunset background
        gba_draw_parallax_background(0, s_title_timer);
        gba_draw_drop_shadow(120, 110, 80, 14);

        gba_draw_string("THE NAZARENE", 80, 36, COL_TEXT_GOLD);
        gba_draw_string("WAY OF THE CROSS", 68, 48, COL_TEXT_WHITE);

        if ((s_title_timer >> 4) % 2 == 0) {
            gba_draw_string("PRESS START", 84, 82, COL_TEXT_GOLD);
        }

        gba_draw_string("CAMPAIGN: JERUSALEM", 56, 104, COL_TEXT_WHITE);

        // Credits display
        gba_draw_string("CREDIT", 8, 144, COL_TEXT_GOLD);
        gba_draw_number(s_credits, 52, 144, COL_TEXT_GOLD);
        gba_draw_string("SELECT: ADD COIN", 120, 144, COL_TEXT_WHITE);
        return;
    }

    if (s_state == GAME_STATE_MISSION_MAP) {
        gba_draw_mission_map(s_mission, s_m1_cleared, s_m2_cleared, s_m3_cleared, s_map_timer);
        return;
    }

    if (s_state == GAME_STATE_CONTINUE) {
        gba_graphics_clear(COL_TEXT_DARK);
        gba_draw_string("RESSURECT??", 84, 48, (s_continue_timer % 16 < 8) ? COL_TEXT_RED : COL_TEXT_GOLD);
        gba_draw_number(s_continue_countdown, 116, 72, COL_TEXT_GOLD);
        gba_draw_string("CREDIT", 84, 108, COL_TEXT_WHITE);
        gba_draw_number(s_credits, 132, 108, COL_TEXT_GOLD);
        gba_draw_string("START: CONTINUE", 72, 128, COL_TEXT_WHITE);
        return;
    }

    if (s_state == GAME_STATE_GAME_OVER) {
        gba_graphics_clear(COL_TEXT_DARK);
        gba_draw_string("GAME OVER", 92, 70, COL_TEXT_RED);
        gba_draw_string("PRESS START", 84, 100, COL_TEXT_GOLD);
        return;
    }

    // -----------------------------------------------------------------------
    // GAMEPLAY RENDERING
    // -----------------------------------------------------------------------

    // 1. Parallax Backgrounds (Streets or Temple Sanctuary)
    if (s_mission == 2) {
        gba_draw_temple_background(s_camera_x, s_player.stateTimer);
    } else {
        gba_draw_parallax_background(s_camera_x, s_player.stateTimer);
    }

    // 2. Platforms & Crates
    for (int i = 0; i < s_platform_count; ++i) {
        GbaPlatform* p = &s_platforms[i];
        int sx = p->x - s_camera_x;
        if (sx + p->w < 0 || sx >= GBA_SCREEN_WIDTH) continue;
        if (p->isOneWay) {
            // Stone / Marble Balcony ledge
            uint8_t topCol = (s_mission == 2) ? COL_TEMPLE_MARBLE : COL_STONE_LIGHT;
            uint8_t botCol = (s_mission == 2) ? COL_TEMPLE_SHADOW : COL_STONE_DARK;
            gba_draw_rect(sx, p->y, p->w, 4, topCol);
            gba_draw_rect(sx, p->y + 4, p->w, 4, botCol);
        } else {
            // Wooden Crate / Step block
            gba_draw_rect(sx, p->y, p->w, p->h, COL_CRATE_MID);
            gba_draw_rect_outline(sx, p->y, p->w, p->h, COL_CRATE_DARK);
            gba_draw_line(sx, p->y, sx + p->w, p->y + p->h, COL_CRATE_LIGHT);
        }
    }

    // 3. Portcullis Arena Gate / Sanctuary Veil
    int gateSX = GBA_ARENA_GATE_X - s_camera_x;
    if (gateSX >= -20 && gateSX <= GBA_SCREEN_WIDTH + 20) {
        int gateY = s_gate_open ? (GBA_GROUND_Y - 50) : (GBA_GROUND_Y - 26);
        for (int gy = gateY; gy <= GBA_GROUND_Y; gy += 4) {
            gba_draw_line(gateSX, gy, gateSX + 14, gy, (s_mission == 2) ? COL_GOLD_BRIGHT : COL_IRON_GATE);
        }
        for (int gx = gateSX; gx <= gateSX + 14; gx += 3) {
            gba_draw_line(gx, gateY, gx, GBA_GROUND_Y, (s_mission == 2) ? COL_GOLD_BRIGHT : COL_IRON_GATE);
        }
    }

    // 4. Baskets / Sacred Censers
    for (int i = 0; i < 4; ++i) {
        if (s_baskets[i].collected) continue;
        int sx = s_baskets[i].x - s_camera_x;
        if (sx >= -16 && sx <= GBA_SCREEN_WIDTH) {
            if (s_mission == 2) {
                gba_draw_censer(sx, s_baskets[i].y);
            } else {
                gba_draw_basket(sx, s_baskets[i].y);
            }
        }
    }

    // 5. Enemies
    for (int i = 0; i < s_enemy_count; ++i) {
        if (!s_enemies[i].active && s_enemies[i].type != 3) continue;
        GbaEnemy* e = &s_enemies[i];
        int sx = e->x - s_camera_x;
        if (sx < -40 || sx > GBA_SCREEN_WIDTH + 40) continue;

        bool hurt = (e->hurtTimer > 0);
        if (e->type == 0) {
            gba_draw_guard(sx, e->y, e->facingLeft, e->isAttacking, e->animFrame, hurt);
        } else if (e->type == 1) {
            gba_draw_archer(sx, e->y, e->facingLeft, e->isDucking, false, hurt);
        } else if (e->type == 2) {
            gba_draw_heavy(sx, e->y, e->facingLeft, false, e->animFrame, hurt);
        } else if (e->type == 3) {
            gba_draw_boss(sx, e->y, e->facingLeft, e->bossState, e->animFrame, hurt);
        } else if (e->type == 4) {
            gba_draw_sicarii(sx, e->y, e->facingLeft, e->bossState, e->animFrame, hurt);
        }
    }

    // 6. Player Jesus (Flickers when invulnerable)
    if (s_player.invulnTimer == 0 || (s_player.invulnTimer % 4 < 2)) {
        int psx = s_player.x - s_camera_x;
        gba_draw_player(psx, s_player.y, s_player.facingLeft, s_player.state, s_player.walkFrame, s_player.invulnTimer > 0);
    }

    // 7. Projectiles (Nails)
    for (int i = 0; i < 4; ++i) {
        if (!s_nails[i].active) continue;
        int nsx = s_nails[i].x - s_camera_x;
        gba_draw_nail(nsx, s_nails[i].y, s_nails[i].vx < 0);
    }

    // 8. HUD DISPLAY (Life, Score, Baskets/Censers, Boss Health)
    // Score
    gba_draw_string("1UP", 8, 4, COL_TEXT_GOLD);
    gba_draw_number(s_player.score, 32, 4, COL_TEXT_WHITE);

    // Hearts (Player HP)
    gba_draw_string("LIFE", 8, 14, COL_TEXT_GOLD);
    for (int h = 0; h < 3; ++h) {
        int hx = 36 + h * 9;
        if (h >= 3 - s_player.shatteredHearts) {
            // Shattered Heart (Unrefillable boss hit)
            gba_draw_char('X', hx, 14, COL_TEXT_RED);
        } else if (h < s_player.hp) {
            // Full Heart
            gba_draw_rect(hx, 15, 6, 5, COL_HEART_RED);
            gba_set_pixel(hx + 1, 14, COL_HEART_RED);
            gba_set_pixel(hx + 4, 14, COL_HEART_RED);
        } else {
            // Empty Heart
            gba_draw_rect_outline(hx, 15, 6, 5, COL_HEART_BORDER);
        }
    }

    // Baskets / Censers collected
    int bCount = 0;
    for (int i = 0; i < 4; ++i) if (s_baskets[i].collected) bCount++;
    const char* objLabel = (s_mission == 2) ? "CENSERS" : "BASKETS";
    gba_draw_string(objLabel, 150, 4, COL_TEXT_GOLD);
    gba_draw_number(bCount, 200, 4, (bCount >= 4) ? COL_TEXT_GOLD : COL_TEXT_WHITE);
    gba_draw_string("/4", 208, 4, COL_TEXT_WHITE);

    // Divine Wrath Icon
    if (s_player.hasWrath) {
        gba_draw_string("WRATH [L/R]", 150, 14, COL_TEXT_GOLD);
    }

    // Boss Health Bar in Arena
    if (s_player.x >= GBA_ARENA_MIN_X) {
        for (int i = 0; i < s_enemy_count; ++i) {
            if (s_enemies[i].type == 3 && s_enemies[i].active) {
                const char* bossName = (s_mission == 2) ? "MALCHUS" : "BRUTUS";
                gba_draw_string(bossName, 80, 148, COL_TEXT_RED);
                int hp = s_enemies[i].hp;
                for (int b = 0; b < 10; ++b) {
                    uint8_t col = (b < hp) ? COL_HEART_RED : COL_TEXT_DARK;
                    gba_draw_rect(128 + b * 6, 149, 4, 6, col);
                }
                break;
            }
        }
    }
}

