#include "BonusStage.h"
#include <cmath>
#include <algorithm>
#include <cstdlib>

static constexpr float LANE_Y[3] = { 88.0f, 132.0f, 176.0f };
static constexpr float SUMMIT_X = 52.0f; // Crowd surge threshold

BonusStage::BonusStage() {
    Reset();
}

void BonusStage::Reset() {
    m_timer = 30.0f;
    m_aimLane = 1;
    m_aimLeadX = 180.0f;
    m_tossCooldown = 0.0f;
    m_nextItemFish = false;
    m_fedCount = 0;
    m_totalVillagers = 28;
    m_allFed = false;
    m_crowdSurged = false;
    m_stageOver = false;
    m_rewardGiven = false;
    m_transitionDelay = 0.0f;
    m_bannerAlpha = 1.0f;
    m_elapsedTime = 0.0f;
    m_spawnIndex = 0;

    m_villagers.clear();
    m_projectiles.clear();
    m_particles.clear();
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

    static const SpawnDef spawns[28] = {
        // Phase 1: Intro (0s - 7s) - 4 villagers
        {  0.8f, 1, VillagerType::YOUTH,   42.0f },
        {  2.4f, 2, VillagerType::BEGGAR,  45.0f },
        {  4.0f, 0, VillagerType::ELDER,   38.0f },
        {  5.6f, 1, VillagerType::WOMAN,   40.0f },

        // Phase 2: Steady advance (7s - 16s) - 7 villagers
        {  7.6f, 0, VillagerType::YOUTH,   44.0f },
        {  8.9f, 2, VillagerType::BEGGAR,  48.0f },
        { 10.4f, 1, VillagerType::WOMAN,   43.0f },
        { 11.8f, 2, VillagerType::YOUTH,   47.0f },
        { 13.2f, 0, VillagerType::ELDER,   41.0f },
        { 14.5f, 1, VillagerType::BEGGAR,  46.0f },
        { 15.8f, 2, VillagerType::WOMAN,   45.0f },

        // Phase 3: Fast escalation (16s - 23s) - 7 villagers
        { 17.0f, 0, VillagerType::YOUTH,   48.0f },
        { 18.2f, 1, VillagerType::ELDER,   44.0f },
        { 19.3f, 2, VillagerType::BEGGAR,  50.0f },
        { 20.4f, 0, VillagerType::WOMAN,   46.0f },
        { 21.4f, 1, VillagerType::YOUTH,   52.0f },
        { 22.3f, 2, VillagerType::BEGGAR,  52.0f },
        { 23.2f, 0, VillagerType::ELDER,   48.0f },

        // Phase 4: The Climax Swarm! (24s - 28.2s) - 10 villagers rushing summit!
        { 24.1f, 1, VillagerType::WOMAN,   52.0f },
        { 24.7f, 2, VillagerType::YOUTH,   54.0f },
        { 25.3f, 0, VillagerType::BEGGAR,  50.0f },
        { 25.8f, 1, VillagerType::ELDER,   50.0f },
        { 26.3f, 2, VillagerType::WOMAN,   54.0f },
        { 26.8f, 0, VillagerType::YOUTH,   54.0f },
        { 27.2f, 1, VillagerType::BEGGAR,  56.0f },
        { 27.6f, 2, VillagerType::ELDER,   52.0f },
        { 27.9f, 0, VillagerType::WOMAN,   55.0f },
        { 28.2f, 1, VillagerType::BEGGAR,  58.0f }
    };

    while (m_spawnIndex < 28 && m_elapsedTime >= spawns[m_spawnIndex].time) {
        const auto& s = spawns[m_spawnIndex];
        SpawnWaveVillager(s.lane, s.type, s.speed);
        m_spawnIndex++;
    }
}

void BonusStage::TossItem() {
    if (m_tossCooldown > 0.0f || m_stageOver) return;

    // Find lead target in the selected lane
    float targetX = 180.0f;
    float closestDist = 999.0f;
    for (const auto& v : m_villagers) {
        if (v.lane == m_aimLane && v.state == VillagerState::APPROACHING) {
            float d = v.position.x - 36.0f;
            if (d > 0.0f && d < closestDist) {
                closestDist = d;
                // Target lead position ahead of moving villager
                targetX = std::max(60.0f, v.position.x - 12.0f);
            }
        }
    }

    BreadFishItem item;
    item.startPos = { 46.0f, 150.0f };
    item.targetPos = { targetX, LANE_Y[m_aimLane] + 12.0f };
    item.position = item.startPos;
    item.progress = 0.0f;
    item.flightDuration = 0.26f;
    item.targetLane = m_aimLane;
    item.isFish = m_nextItemFish;
    item.active = true;
    item.spinTimer = 0.0f;

    m_projectiles.push_back(item);
    m_tossCooldown = 0.13f; // Fast rapid toss rate

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

        // Aim Controls
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

        if (input.attackPressed || input.jumpPressed) {
            TossItem();
        }

        if (m_tossCooldown > 0.0f) m_tossCooldown -= dt;
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

    // Update Villagers
    for (auto& v : m_villagers) {
        if (v.state == VillagerState::APPROACHING) {
            v.position.x -= v.speed * dt;
            v.animTimer += dt * 8.0f;
            v.walkFrame = static_cast<int>(v.animTimer) % 2;

            // Check if crowd reached the summit before being fed!
            if (v.position.x <= SUMMIT_X && !m_stageOver) {
                v.state = VillagerState::SURGED;
                m_crowdSurged = true;
                m_stageOver = true;
                m_transitionDelay = 3.0f;
                AudioManager::PlaySfx(SFX_PLAYER_HURT);
            }
        } else if (v.state == VillagerState::FED) {
            v.fedTimer += dt;
        }
    }

    // Update Projectiles
    for (auto& p : m_projectiles) {
        if (!p.active) continue;
        p.spinTimer += dt * 18.0f;
        p.progress += dt / p.flightDuration;

        float pr = std::min(1.0f, p.progress);
        float arcH = 42.0f * sinf(pr * 3.14159f);

        p.position.x = p.startPos.x + (p.targetPos.x - p.startPos.x) * pr;
        p.position.y = p.startPos.y + (p.targetPos.y - p.startPos.y) * pr - arcH;

        if (p.progress >= 1.0f) {
            p.active = false;

            // Check collision with approaching villagers in the target lane
            for (auto& v : m_villagers) {
                if (v.lane == p.targetLane && v.state == VillagerState::APPROACHING) {
                    float dist = fabsf(v.position.x - p.targetPos.x);
                    if (dist < 26.0f) {
                        v.state = VillagerState::FED;
                        m_fedCount++;
                        playerScore += 200;
                        AudioManager::PlaySfx(SFX_CROWD_CHEER);

                        // Burst of golden blessing sparkles
                        for (int k = 0; k < 10; ++k) {
                            StarburstParticle sp;
                            sp.position = { v.position.x + 8.0f, v.position.y + 8.0f };
                            float ang = ((float)rand() / (float)RAND_MAX) * 6.28318f;
                            float spd = 40.0f + ((float)rand() / (float)RAND_MAX) * 80.0f;
                            sp.velocity = { cosf(ang) * spd, sinf(ang) * spd };
                            sp.color = (k % 2 == 0) ? COLOR_GOLD_BRIGHT : Color{ 255, 255, 230, 255 };
                            sp.alpha = 1.0f;
                            sp.life = 0.5f + ((float)rand() / (float)RAND_MAX) * 0.3f;
                            m_particles.push_back(sp);
                        }
                        break;
                    }
                }
            }
        }
    }

    // Check if all villagers fed!
    if (m_fedCount >= m_totalVillagers && !m_stageOver) {
        m_allFed = true;
        m_stageOver = true;
        m_transitionDelay = 4.2f;
    }

    // Reward Extra Life & Bonus Score upon completion
    if (m_allFed && !m_rewardGiven) {
        m_rewardGiven = true;
        playerScore += 10000;
        playerLives += 1;
        AudioManager::PlaySfx(SFX_BONUS_WIN);
    }

    // Update Particles
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
}

void BonusStage::Draw() {
    // 1. Draw Sunset over Galilee Mountain Backdrop
    GraphicsManager::DrawBonusStageBackground(m_elapsedTime);

    // 2. Draw 3 Platform Terraces
    for (int lane = 0; lane < 3; ++lane) {
        float ly = LANE_Y[lane] + 28.0f;
        // Grassy slope / mountain path terrace
        DrawRectangle(40, static_cast<int>(ly), VIRTUAL_WIDTH - 40, 4, Color{ 88, 120, 52, 255 });
        DrawRectangle(40, static_cast<int>(ly + 4), VIRTUAL_WIDTH - 40, 2, Color{ 58, 80, 36, 255 });
    }

    // 3. Draw Lane Reticle / Target Guidance
    float retY = LANE_Y[m_aimLane] + 12.0f;
    bool blink = (static_cast<int>(GetTime() * 8.0f) % 2 == 0);
    Color retCol = blink ? COLOR_GOLD_BRIGHT : Color{ 255, 235, 140, 180 };
    DrawRectangleLines(140, static_cast<int>(retY - 14), 48, 28, retCol);
    DrawLine(164, static_cast<int>(retY - 18), 164, static_cast<int>(retY + 18), retCol);
    DrawLine(136, static_cast<int>(retY), 192, static_cast<int>(retY), retCol);

    // 4. Draw Jesus at the Summit holding the Wicker Basket
    SpriteId jesusSpr = (m_tossCooldown > 0.06f) ? SPR_JESUS_BASKET_THROW : SPR_JESUS_BASKET_AIM;
    GraphicsManager::DrawDropShadow(32.0f, 186.0f, 24.0f, 6.0f);
    GraphicsManager::DrawSprite(jesusSpr, 24.0f, 156.0f, false, WHITE);

    // 5. Draw Approaching / Fed Villagers
    for (const auto& v : m_villagers) {
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
            GraphicsManager::DrawSprite(spr, v.position.x, v.position.y, true, WHITE);
        } else if (v.state == VillagerState::FED) {
            // Sitting down rejoicing with halo
            GraphicsManager::DrawSprite(SPR_VILLAGER_SATISFIED, v.position.x, v.position.y + 4.0f, true, WHITE);
            // Floating halo
            float bob = sinf(v.fedTimer * 5.0f) * 2.0f;
            DrawCircleLines(static_cast<int>(v.position.x + 10.0f), static_cast<int>(v.position.y - 2.0f + bob), 5.0f, COLOR_GOLD_BRIGHT);
        } else {
            // Surged
            GraphicsManager::DrawSprite(SPR_VILLAGER_BEGGAR_RUN_0, v.position.x, v.position.y, true, Color{ 255, 180, 180, 255 });
        }
    }

    // 6. Draw Bread and Fish Projectiles
    for (const auto& p : m_projectiles) {
        if (!p.active) continue;
        SpriteId itemSpr = p.isFish ? SPR_FISH_PROJECTILE : SPR_BREAD_LOAF;
        GraphicsManager::DrawSprite(itemSpr, p.position.x - 6.0f, p.position.y - 6.0f);
        // Golden trail spark
        GraphicsManager::DrawSpark(p.position.x, p.position.y, 2.5f, Color{ 255, 235, 120, 200 });
    }

    // 7. Draw Blessing Starburst Particles
    for (const auto& pt : m_particles) {
        Color c = pt.color;
        c.a = static_cast<unsigned char>(pt.alpha * 255);
        DrawCircle(static_cast<int>(pt.position.x), static_cast<int>(pt.position.y), 2.0f, c);
    }

    // 8. Arcade HUD Header
    DrawRectangle(0, 0, VIRTUAL_WIDTH, 24, COLOR_HUD_BG);
    DrawLine(0, 24, VIRTUAL_WIDTH, 24, COLOR_ROMAN_DARK);

    // Title
    DrawText("BONUS STAGE: FEED THE 5,000", 8, 7, 10, COLOR_ROMAN_GOLD);

    // Big Countdown Timer (Ticking Retro Digits)
    char timeBuf[32];
    snprintf(timeBuf, sizeof(timeBuf), "TIME %05.2f", m_timer);
    Color tColor = (m_timer <= 5.0f) ? Color{ 255, 45, 55, 255 } : COLOR_GOLD_BRIGHT;
    int tw = MeasureText(timeBuf, 10);
    DrawText(timeBuf, (VIRTUAL_WIDTH - tw) / 2, 7, 10, tColor);

    // Multitude Fed Counter
    char fedBuf[32];
    snprintf(fedBuf, sizeof(fedBuf), "FED %d/%d", m_fedCount, m_totalVillagers);
    int fw = MeasureText(fedBuf, 10);
    Color fColor = (m_fedCount >= m_totalVillagers) ? Color{ 80, 255, 120, 255 } : COLOR_ROBE_WHITE;
    DrawText(fedBuf, VIRTUAL_WIDTH - fw - 8, 7, 10, fColor);

    // 9. Banners & Overlays
    if (m_bannerAlpha > 0.0f) {
        const char* bMsg = "FEED THE HUNGRY MULTITUDE!";
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
    } else if (m_crowdSurged) {
        int cw = 220, ch = 44;
        int cx = (VIRTUAL_WIDTH - cw) / 2;
        int cy = 66;
        DrawRectangle(cx, cy, cw, ch, Color{ 24, 12, 16, 240 });
        DrawRectangleLines(cx, cy, cw, ch, COLOR_ROMAN_CRIMSON);
        const char* msg1 = "CROWD SURGED THE SUMMIT!";
        const char* msg2 = "BONUS STAGE OVER";
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

