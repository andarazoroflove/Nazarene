#include "BonusStage.h"
#include <cmath>
#include <algorithm>
#include <cstdlib>
#include <cstdio>

static constexpr float LANE_Y[3] = { 82.0f, 128.0f, 174.0f };
static constexpr float SUMMIT_X = 56.0f;

BonusStage::BonusStage() {
    Reset();
}

void BonusStage::Reset() {
    m_timer = 30.0f;
    m_aimLane = 1;
    m_tossCooldown = 0.0f;
    m_nextItemFish = false;
    m_fedCount = 0;
    m_totalVillagers = 28;
    m_summitBasket = 3;
    m_combo = 0;
    m_comboTimer = 0.0f;
    m_allFed = false;
    m_stageOver = false;
    m_rewardGiven = false;
    m_transitionDelay = 0.0f;
    m_bannerAlpha = 1.0f;
    m_elapsedTime = 0.0f;
    m_spawnIndex = 0;

    m_villagers.clear();
    m_projectiles.clear();
    m_particles.clear();
    m_popups.clear();
}

void BonusStage::SpawnWaveVillager(int lane, VillagerType type, float speed) {
    BonusVillager v;
    v.lane = lane;
    v.type = type;
    v.speed = speed;
    v.position = { 330.0f, LANE_Y[lane] };
    v.state = VillagerState::APPROACHING;
    v.animTimer = 0.0f;
    v.walkFrame = 0;
    v.fedTimer = 0.0f;
    v.cheerHop = 0.0f;
    m_villagers.push_back(v);
}

void BonusStage::UpdateSpawns(float dt) {
    if (m_stageOver) return;
    m_elapsedTime += dt;

    struct SpawnDef {
        float time;
        int lane;
        VillagerType type;
        float speed;
    };

    // Paced 28-villager arcade script: starts readable, builds momentum, climax between 25.8s and 28.2s
    static const SpawnDef spawns[28] = {
        // Wave 1: Gentle Opening (0.6s - 6.5s) - 4 villagers
        {  0.6f, 1, VillagerType::YOUTH,   44.0f },
        {  2.2f, 2, VillagerType::BEGGAR,  46.0f },
        {  3.8f, 0, VillagerType::ELDER,   40.0f },
        {  5.4f, 1, VillagerType::WOMAN,   42.0f },

        // Wave 2: Dual Lanes (7.0s - 15.5s) - 7 villagers
        {  7.2f, 0, VillagerType::YOUTH,   46.0f },
        {  8.5f, 2, VillagerType::BEGGAR,  48.0f },
        { 10.0f, 1, VillagerType::WOMAN,   45.0f },
        { 11.4f, 2, VillagerType::YOUTH,   50.0f },
        { 12.8f, 0, VillagerType::ELDER,   42.0f },
        { 14.0f, 1, VillagerType::BEGGAR,  48.0f },
        { 15.2f, 2, VillagerType::WOMAN,   46.0f },

        // Wave 3: Triple Lane Cascade (16.5s - 23.5s) - 7 villagers
        { 16.6f, 0, VillagerType::YOUTH,   50.0f },
        { 17.8f, 1, VillagerType::ELDER,   46.0f },
        { 18.9f, 2, VillagerType::BEGGAR,  52.0f },
        { 20.0f, 0, VillagerType::WOMAN,   48.0f },
        { 21.0f, 1, VillagerType::YOUTH,   54.0f },
        { 22.0f, 2, VillagerType::BEGGAR,  54.0f },
        { 23.0f, 0, VillagerType::ELDER,   50.0f },

        // Wave 4: The Climax Multitude Swarm! (24.2s - 28.2s) - 10 villagers
        { 24.2f, 1, VillagerType::WOMAN,   54.0f },
        { 24.8f, 2, VillagerType::YOUTH,   56.0f },
        { 25.4f, 0, VillagerType::BEGGAR,  52.0f },
        { 25.9f, 1, VillagerType::ELDER,   52.0f },
        { 26.4f, 2, VillagerType::WOMAN,   56.0f },
        { 26.9f, 0, VillagerType::YOUTH,   56.0f },
        { 27.3f, 1, VillagerType::BEGGAR,  58.0f },
        { 27.7f, 2, VillagerType::ELDER,   54.0f },
        { 28.0f, 0, VillagerType::WOMAN,   58.0f },
        { 28.2f, 1, VillagerType::BEGGAR,  60.0f }
    };

    while (m_spawnIndex < 28 && m_elapsedTime >= spawns[m_spawnIndex].time) {
        const auto& s = spawns[m_spawnIndex];
        SpawnWaveVillager(s.lane, s.type, s.speed);
        m_spawnIndex++;
    }
}

void BonusStage::AddScorePopup(float x, float y, int pts, const char* label, Color col) {
    BonusScorePopup p;
    p.position = { x, y };
    p.score = pts;
    if (label && label[0] != '\0') {
        snprintf(p.text, sizeof(p.text), "%s +%d", label, pts);
    } else {
        snprintf(p.text, sizeof(p.text), "+%d", pts);
    }
    p.color = col;
    p.alpha = 1.0f;
    p.life = 0.9f;
    m_popups.push_back(p);
}

void BonusStage::TossItem() {
    if (m_tossCooldown > 0.0f || m_stageOver) return;

    BreadFishItem item;
    // Launch from Jesus's basket
    float startY = LANE_Y[m_aimLane] + 10.0f;
    item.position = { 50.0f, startY };
    item.velocity = { 520.0f, 0.0f }; // High-speed horizontal flight down the lane
    item.targetLane = m_aimLane;
    item.isFish = m_nextItemFish;
    item.active = true;
    item.spin = 0.0f;
    item.life = 0.75f;

    m_projectiles.push_back(item);
    m_tossCooldown = 0.11f; // Ultra-crisp arcade firing rate

    if (m_nextItemFish) {
        AudioManager::PlaySfx(SFX_FISH_TOSS);
    } else {
        AudioManager::PlaySfx(SFX_BREAD_TOSS);
    }
    m_nextItemFish = !m_nextItemFish;
}

void BonusStage::Update(float dt, const InputState& input, int& playerLives, int& playerScore) {
    if (!m_stageOver) {
        m_timer -= dt;
        if (m_timer <= 0.0f) {
            m_timer = 0.0f;
            m_stageOver = true;
            m_transitionDelay = 3.5f;
        }

        // Aim Controls (D-pad, Analog stick, Keyboard UP/DOWN / W/S)
        if (input.upPressed) {
            if (m_aimLane > 0) {
                m_aimLane--;
                AudioManager::PlaySfx(SFX_JUMP);
            }
        }
        if (input.downPressed) {
            if (m_aimLane < 2) {
                m_aimLane++;
                AudioManager::PlaySfx(SFX_JUMP);
            }
        }

        // Rapid Fire Tossing (Attack or Jump buttons)
        if (input.attackPressed || input.jumpPressed || (input.attackDown && m_tossCooldown <= 0.0f)) {
            TossItem();
        }

        if (m_tossCooldown > 0.0f) m_tossCooldown -= dt;

        if (m_comboTimer > 0.0f) {
            m_comboTimer -= dt;
            if (m_comboTimer <= 0.0f) m_combo = 0;
        }

        if (m_bannerAlpha > 0.0f && m_elapsedTime > 2.0f) {
            m_bannerAlpha -= dt * 1.5f;
            if (m_bannerAlpha < 0.0f) m_bannerAlpha = 0.0f;
        }

        UpdateSpawns(dt);
    } else {
        if (m_transitionDelay > 0.0f) {
            m_transitionDelay -= dt;
        }
    }

    // 1. Update Villagers
    for (auto& v : m_villagers) {
        if (v.state == VillagerState::APPROACHING) {
            v.position.x -= v.speed * dt;
            v.animTimer += dt * 9.0f;
            v.walkFrame = static_cast<int>(v.animTimer) % 2;

            // Check if villager reaches the Summit Overlook
            if (v.position.x <= SUMMIT_X && !m_stageOver) {
                if (m_summitBasket > 0) {
                    // Jesus blesses them directly from the summit basket!
                    m_summitBasket--;
                    v.state = VillagerState::SUMMIT_FED;
                    v.cheerHop = 6.0f;
                    m_fedCount++;
                    m_combo = 0; // Combo breaks
                    playerScore += 100;
                    AddScorePopup(v.position.x + 8.0f, v.position.y - 8.0f, 100, "SUMMIT FED", Color{ 255, 210, 110, 255 });
                    AudioManager::PlaySfx(SFX_BREAD_TOSS);

                    // If all 3 summit loaves are used, the basket empties!
                    if (m_summitBasket <= 0) {
                        m_stageOver = true;
                        m_transitionDelay = 3.0f;
                        AudioManager::PlaySfx(SFX_PLAYER_HURT);
                    }
                }
            }
        } else if (v.state == VillagerState::FED || v.state == VillagerState::SUMMIT_FED) {
            v.fedTimer += dt;
            if (v.cheerHop > 0.0f) {
                v.cheerHop -= dt * 20.0f;
                if (v.cheerHop < 0.0f) v.cheerHop = 0.0f;
            }
        }
    }

    // 2. Update Projectiles & Real-Time Continuous Collision Detection
    for (auto& p : m_projectiles) {
        if (!p.active) continue;

        p.position.x += p.velocity.x * dt;
        p.spin += dt * 16.0f;
        p.life -= dt;

        if (p.life <= 0.0f || p.position.x > VIRTUAL_WIDTH + 20.0f) {
            p.active = false;
            continue;
        }

        // Active collision check against approaching villagers on the target lane
        for (auto& v : m_villagers) {
            if (v.lane == p.targetLane && v.state == VillagerState::APPROACHING) {
                float dx = fabsf(p.position.x - v.position.x);
                float dy = fabsf(p.position.y - (v.position.y + 12.0f));

                if (dx < 20.0f && dy < 16.0f) {
                    // Direct Hit!
                    p.active = false;
                    v.state = VillagerState::FED;
                    v.cheerHop = 10.0f;
                    m_fedCount++;
                    m_combo++;
                    m_comboTimer = 2.2f;

                    int baseScore = 200;
                    int comboBonus = (m_combo > 1) ? ((m_combo - 1) * 50) : 0;
                    int totalScore = baseScore + comboBonus;
                    playerScore += totalScore;

                    char comboLbl[32] = "";
                    if (m_combo >= 5) snprintf(comboLbl, sizeof(comboLbl), "%dX MIRACLE!", m_combo);
                    else if (m_combo >= 2) snprintf(comboLbl, sizeof(comboLbl), "%dX COMBO!", m_combo);

                    Color popCol = (m_combo >= 4) ? COLOR_GOLD_BRIGHT : Color{ 255, 245, 180, 255 };
                    AddScorePopup(v.position.x + 4.0f, v.position.y - 6.0f, totalScore, comboLbl, popCol);

                    AudioManager::PlaySfx(SFX_CROWD_CHEER);

                    // Starburst Particles
                    for (int k = 0; k < 12; ++k) {
                        StarburstParticle sp;
                        sp.position = { v.position.x + 10.0f, v.position.y + 10.0f };
                        float ang = ((float)rand() / (float)RAND_MAX) * 6.28318f;
                        float spd = 35.0f + ((float)rand() / (float)RAND_MAX) * 90.0f;
                        sp.velocity = { cosf(ang) * spd, sinf(ang) * spd };
                        sp.color = (k % 2 == 0) ? COLOR_GOLD_BRIGHT : Color{ 255, 255, 230, 255 };
                        sp.alpha = 1.0f;
                        sp.life = 0.45f + ((float)rand() / (float)RAND_MAX) * 0.35f;
                        m_particles.push_back(sp);
                    }
                    break;
                }
            }
        }
    }

    // 3. Check for Perfect Clear
    if (m_fedCount >= m_totalVillagers && !m_stageOver) {
        m_allFed = true;
        m_stageOver = true;
        m_transitionDelay = 4.2f;
    }

    // 4. Reward Extra Life & Massive Bonus upon completion
    if (m_allFed && !m_rewardGiven) {
        m_rewardGiven = true;
        playerScore += 10000;
        playerLives += 1;
        AudioManager::PlaySfx(SFX_BONUS_WIN);
    }

    // 5. Update Particles
    for (size_t i = 0; i < m_particles.size();) {
        auto& pt = m_particles[i];
        pt.life -= dt;
        if (pt.life <= 0.0f) {
            m_particles.erase(m_particles.begin() + i);
        } else {
            pt.position.x += pt.velocity.x * dt;
            pt.position.y += pt.velocity.y * dt;
            pt.alpha = pt.life / 0.8f;
            ++i;
        }
    }

    // 6. Update Score Popups
    for (size_t i = 0; i < m_popups.size();) {
        auto& pop = m_popups[i];
        pop.life -= dt;
        pop.position.y -= dt * 24.0f; // Float upward
        pop.alpha = pop.life / 0.9f;
        if (pop.life <= 0.0f) {
            m_popups.erase(m_popups.begin() + i);
        } else {
            ++i;
        }
    }
}

void BonusStage::Draw() {
    // 1. Draw Sunset over Galilee Mountain Backdrop
    GraphicsManager::DrawBonusStageBackground(m_elapsedTime);

    // 2. Draw 3 Beautiful Mountain Terraces
    for (int lane = 0; lane < 3; ++lane) {
        float ly = LANE_Y[lane] + 28.0f;
        // Grassy slope / mountain path terrace with shading
        DrawRectangle(44, static_cast<int>(ly), VIRTUAL_WIDTH - 44, 4, Color{ 88, 120, 52, 255 });
        DrawRectangle(44, static_cast<int>(ly + 4), VIRTUAL_WIDTH - 44, 2, Color{ 58, 80, 36, 255 });
        DrawRectangle(44, static_cast<int>(ly + 6), VIRTUAL_WIDTH - 44, 1, Color{ 38, 52, 24, 255 });

        // Highlight lane active strip
        if (lane == m_aimLane) {
            float pulse = 0.5f + 0.5f * sinf(static_cast<float>(GetTime()) * 8.0f);
            DrawRectangle(44, static_cast<int>(ly - 1), VIRTUAL_WIDTH - 44, 1, ColorAlpha(COLOR_GOLD_BRIGHT, pulse * 0.65f));
        }
    }

    // 3. Draw Jesus at the Summit Pulpit
    float jesusY = LANE_Y[m_aimLane] - 6.0f;
    GraphicsManager::DrawDropShadow(32.0f, jesusY + 30.0f, 26.0f, 7.0f);
    SpriteId jesusSpr = (m_tossCooldown > 0.05f) ? SPR_JESUS_BASKET_THROW : SPR_JESUS_BASKET_AIM;
    GraphicsManager::DrawSprite(jesusSpr, 22.0f, jesusY, false, WHITE);

    // Dynamic targeting reticle tracking oncoming villagers or hovering forward
    float lockX = 180.0f;
    float closestDist = 999.0f;
    for (const auto& v : m_villagers) {
        if (v.lane == m_aimLane && v.state == VillagerState::APPROACHING) {
            float dist = v.position.x - 48.0f;
            if (dist > 0.0f && dist < closestDist) {
                closestDist = dist;
                lockX = v.position.x + 8.0f;
            }
        }
    }
    float retY = LANE_Y[m_aimLane] + 12.0f;
    bool blink = (static_cast<int>(GetTime() * 10.0f) % 2 == 0);
    Color retCol = blink ? COLOR_GOLD_BRIGHT : Color{ 255, 235, 140, 200 };
    
    // Sega Arcade Crosshair
    DrawRectangleLines(static_cast<int>(lockX - 10), static_cast<int>(retY - 10), 20, 20, retCol);
    DrawLine(static_cast<int>(lockX - 14), static_cast<int>(retY), static_cast<int>(lockX - 4), static_cast<int>(retY), retCol);
    DrawLine(static_cast<int>(lockX + 4), static_cast<int>(retY), static_cast<int>(lockX + 14), static_cast<int>(retY), retCol);
    DrawLine(static_cast<int>(lockX), static_cast<int>(retY - 14), static_cast<int>(lockX), static_cast<int>(retY - 4), retCol);
    DrawLine(static_cast<int>(lockX), static_cast<int>(retY + 4), static_cast<int>(lockX), static_cast<int>(retY + 14), retCol);

    // 4. Draw Approaching / Fed Villagers
    for (const auto& v : m_villagers) {
        float vy = v.position.y - v.cheerHop;
        GraphicsManager::DrawDropShadow(v.position.x, v.position.y + 26.0f, 18.0f, 5.0f);

        if (v.state == VillagerState::APPROACHING) {
            SpriteId spr = SPR_VILLAGER_ELDER_RUN_0;
            switch (v.type) {
                case VillagerType::ELDER:
                    spr = (v.walkFrame == 0 ? SPR_VILLAGER_ELDER_RUN_0 : SPR_VILLAGER_ELDER_RUN_1);
                    break;
                case VillagerType::WOMAN:
                    spr = (v.walkFrame == 0 ? SPR_VILLAGER_WOMAN_RUN_0 : SPR_VILLAGER_WOMAN_RUN_1);
                    break;
                case VillagerType::YOUTH:
                    spr = (v.walkFrame == 0 ? SPR_VILLAGER_YOUTH_RUN_0 : SPR_VILLAGER_YOUTH_RUN_1);
                    break;
                case VillagerType::BEGGAR:
                    spr = (v.walkFrame == 0 ? SPR_VILLAGER_BEGGAR_RUN_0 : SPR_VILLAGER_BEGGAR_RUN_1);
                    break;
            }
            GraphicsManager::DrawSprite(spr, v.position.x, vy, true, WHITE);
        } else if (v.state == VillagerState::FED || v.state == VillagerState::SUMMIT_FED) {
            // Overjoyed / feasting with halo
            GraphicsManager::DrawSprite(SPR_VILLAGER_SATISFIED, v.position.x, vy + 4.0f, true, WHITE);
            // Floating glowing halo
            float bob = sinf(v.fedTimer * 6.0f) * 2.5f;
            DrawCircleLines(static_cast<int>(v.position.x + 11.0f), static_cast<int>(vy - 2.0f + bob), 5.0f, COLOR_GOLD_BRIGHT);
            DrawCircle(static_cast<int>(v.position.x + 11.0f), static_cast<int>(vy - 2.0f + bob), 2.0f, Color{ 255, 255, 200, 180 });
        }
    }

    // 5. Draw Bread and Fish Projectiles
    for (const auto& p : m_projectiles) {
        if (!p.active) continue;
        SpriteId itemSpr = p.isFish ? SPR_FISH_PROJECTILE : SPR_BREAD_LOAF;
        GraphicsManager::DrawSprite(itemSpr, p.position.x - 7.0f, p.position.y - 5.0f);
        // Golden comet trail
        GraphicsManager::DrawSpark(p.position.x - 6.0f, p.position.y, 2.5f, Color{ 255, 235, 120, 220 });
        GraphicsManager::DrawSpark(p.position.x - 12.0f, p.position.y, 1.5f, Color{ 255, 195, 60, 160 });
    }

    // 6. Draw Particles
    for (const auto& pt : m_particles) {
        Color c = pt.color;
        c.a = static_cast<unsigned char>(pt.alpha * 255);
        DrawCircle(static_cast<int>(pt.position.x), static_cast<int>(pt.position.y), 2.0f, c);
    }

    // 7. Draw Floating Score Popups
    for (const auto& pop : m_popups) {
        Color c = pop.color;
        c.a = static_cast<unsigned char>(pop.alpha * 255);
        int tw = MeasureText(pop.text, 10);
        DrawText(pop.text, static_cast<int>(pop.position.x - tw / 2) + 1, static_cast<int>(pop.position.y) + 1, 10, Color{ 0, 0, 0, c.a });
        DrawText(pop.text, static_cast<int>(pop.position.x - tw / 2), static_cast<int>(pop.position.y), 10, c);
    }

    // 8. Arcade HUD Header
    DrawRectangle(0, 0, VIRTUAL_WIDTH, 24, COLOR_HUD_BG);
    DrawLine(0, 24, VIRTUAL_WIDTH, 24, COLOR_ROMAN_DARK);

    // Title / Stage name
    DrawText("FEED THE 5,000", 8, 7, 10, COLOR_ROMAN_GOLD);

    // Big Ticking Retro Digits Timer
    char timeBuf[32];
    snprintf(timeBuf, sizeof(timeBuf), "TIME %05.2f", m_timer);
    Color tColor = (m_timer <= 5.0f) ? Color{ 255, 45, 55, 255 } : COLOR_GOLD_BRIGHT;
    int tw = MeasureText(timeBuf, 10);
    DrawText(timeBuf, (VIRTUAL_WIDTH - tw) / 2, 7, 10, tColor);

    // Summit Reserves Meter
    int resX = 145;
    DrawText("RESERVES", resX, 7, 10, Color{ 210, 195, 175, 255 });
    for (int r = 0; r < 3; ++r) {
        Color rCol = (r < m_summitBasket) ? COLOR_GOLD_BRIGHT : Color{ 60, 50, 65, 255 };
        DrawRectangle(resX + 58 + r * 8, 8, 6, 8, rCol);
        DrawRectangleLines(resX + 58 + r * 8, 8, 6, 8, COLOR_ROMAN_DARK);
    }

    // Multitude Fed Counter
    char fedBuf[32];
    snprintf(fedBuf, sizeof(fedBuf), "FED %02d/%02d", m_fedCount, m_totalVillagers);
    int fw = MeasureText(fedBuf, 10);
    Color fColor = (m_fedCount >= m_totalVillagers) ? Color{ 80, 255, 120, 255 } : COLOR_ROBE_WHITE;
    DrawText(fedBuf, VIRTUAL_WIDTH - fw - 8, 7, 10, fColor);

    // 9. Banners & Overlays
    if (m_bannerAlpha > 0.0f) {
        const char* bMsg = "AIM: UP/DOWN | RAPID TOSS: ATTACK/JUMP";
        int bw = MeasureText(bMsg, 10);
        int bx = (VIRTUAL_WIDTH - bw) / 2;
        DrawRectangle(bx - 8, 48, bw + 16, 18, ColorAlpha(Color{ 18, 14, 24, 230 }, m_bannerAlpha));
        DrawRectangleLines(bx - 8, 48, bw + 16, 18, ColorAlpha(COLOR_GOLD_BRIGHT, m_bannerAlpha));
        DrawText(bMsg, bx, 52, 10, ColorAlpha(COLOR_GOLD_BRIGHT, m_bannerAlpha));
    }

    if (m_allFed) {
        // Glorious Perfect Victory Banner!
        int cw = 240, ch = 56;
        int cx = (VIRTUAL_WIDTH - cw) / 2;
        int cy = 60;
        DrawRectangle(cx, cy, cw, ch, Color{ 16, 12, 22, 240 });
        DrawRectangleLines(cx, cy, cw, ch, COLOR_GOLD_BRIGHT);
        DrawRectangleLines(cx + 2, cy + 2, cw - 4, ch - 4, Color{ 80, 255, 120, 255 });

        const char* msg1 = "PERFECT! ALL 28 FED!";
        const char* msg2 = "BONUS: +10,000 PTS";
        const char* msg3 = "+1 EXTRA RESURRECT CREDIT AWARDED!";
        DrawText(msg1, cx + (cw - MeasureText(msg1, 10)) / 2, cy + 8, 10, COLOR_GOLD_BRIGHT);
        DrawText(msg2, cx + (cw - MeasureText(msg2, 10)) / 2, cy + 22, 10, Color{ 255, 235, 140, 255 });
        DrawText(msg3, cx + (cw - MeasureText(msg3, 10)) / 2, cy + 36, 10, Color{ 80, 255, 120, 255 });
    } else if (m_stageOver && m_summitBasket <= 0) {
        int cw = 220, ch = 44;
        int cx = (VIRTUAL_WIDTH - cw) / 2;
        int cy = 66;
        DrawRectangle(cx, cy, cw, ch, Color{ 24, 12, 16, 240 });
        DrawRectangleLines(cx, cy, cw, ch, COLOR_ROMAN_CRIMSON);
        const char* msg1 = "THE BASKET IS EMPTY!";
        const char* msg2 = "BONUS STAGE FINISHED";
        DrawText(msg1, cx + (cw - MeasureText(msg1, 10)) / 2, cy + 10, 10, Color{ 255, 80, 80, 255 });
        DrawText(msg2, cx + (cw - MeasureText(msg2, 10)) / 2, cy + 24, 10, COLOR_ROBE_WHITE);
    } else if (m_stageOver && m_timer <= 0.0f) {
        int cw = 200, ch = 44;
        int cx = (VIRTUAL_WIDTH - cw) / 2;
        int cy = 66;
        DrawRectangle(cx, cy, cw, ch, Color{ 20, 16, 24, 240 });
        DrawRectangleLines(cx, cy, cw, ch, COLOR_ROMAN_GOLD);
        const char* msg1 = "TIME EXPIRED!";
        const char* msg2 = "BONUS STAGE FINISHED";
        DrawText(msg1, cx + (cw - MeasureText(msg1, 10)) / 2, cy + 10, 10, COLOR_GOLD_BRIGHT);
        DrawText(msg2, cx + (cw - MeasureText(msg2, 10)) / 2, cy + 24, 10, COLOR_ROBE_WHITE);
    }
}
