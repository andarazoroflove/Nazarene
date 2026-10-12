#include "raylib.h"
#include "Constants.h"
#include "Input.h"
#include "Audio.h"
#include "Graphics.h"
#include "Level.h"
#include "Player.h"
#include "Enemies.h"
#include <cmath>
#include <cstdio>
#include <algorithm>

enum class GameState {
    TITLE,
    GAMEPLAY,
    GAMEOVER,
    VICTORY
};

int main() {
    // 1. Initialize Raylib Window
    SetConfigFlags(FLAG_VSYNC_HINT | FLAG_WINDOW_RESIZABLE);
    InitWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "The Nazarene: Way of the Cross (Mission 1)");
    SetExitKey(KEY_NULL);
    SetWindowMinSize(VIRTUAL_WIDTH * 2, VIRTUAL_HEIGHT * 2);
    SetTargetFPS(TARGET_FPS);

    // 2. Initialize Subsystems
    AudioManager::Init();
    GraphicsManager::Init();

    // 3. Create Off-screen 320x224 Render Texture (Integer / Aspect Ratio Scaling)
    RenderTexture2D targetBuffer = LoadRenderTexture(VIRTUAL_WIDTH, VIRTUAL_HEIGHT);
    SetTextureFilter(targetBuffer.texture, TEXTURE_FILTER_POINT);

    // 4. Game Systems
    Level level;
    Player player;
    EnemyManager enemyManager;
    InputState input;

    GameState gameState = GameState::TITLE;
    Camera2D camera = { { 0.0f, 0.0f }, { 0.0f, 0.0f }, 0.0f, 1.0f };

    int credits = 0;
    float coinNoticeTimer = 0.0f;
    float continueCountdown = 9.9f;

    float gateNoticeTimer = 0.0f;
    float victoryTimer = 0.0f;
    float gameOverTimer = 0.0f;

    bool arenaSetupDone = false;
    bool finishHimTriggered = false;
    float finishHimBannerTimer = 0.0f;
    float cloudCoverage = 0.0f;
    float lightningTimer = 3.5f;
    float lightningFlashTimer = 0.0f;

    float ascensionSequenceTimer = 0.0f;
    bool ascensionStarted = false;
    float cloudsPartAmount = 0.0f;
    float divineBeamIntensity = 0.0f;
    float celestialFadeAlpha = 0.0f;

    auto ResetGame = [&](bool fullReset) {
        level.Reset();
        player.Reset(fullReset);
        enemyManager.Reset();
        camera.target.x = 0.0f;
        gateNoticeTimer = 0.0f;
        victoryTimer = 0.0f;
        gameOverTimer = 0.0f;
        continueCountdown = 9.9f;
        arenaSetupDone = false;
        finishHimTriggered = false;
        finishHimBannerTimer = 0.0f;
        cloudCoverage = 0.0f;
        lightningTimer = 3.5f;
        lightningFlashTimer = 0.0f;
        ascensionSequenceTimer = 0.0f;
        ascensionStarted = false;
        cloudsPartAmount = 0.0f;
        divineBeamIntensity = 0.0f;
        celestialFadeAlpha = 0.0f;
    };

    // 5. Main Game Loop
    while (!WindowShouldClose()) {
        float dt = GetFrameTime();
        if (dt > 0.05f) dt = 0.05f; // Clamp delta time to avoid physics tunneling

        // Unified Input
        InputHandler::Update(input);

        // Toggle Fullscreen
        if (input.toggleFullscreen) {
            ToggleFullscreen();
        }

        // --- State Machine Updates ---
        switch (gameState) {
            case GameState::TITLE: {
                if (coinNoticeTimer > 0.0f) coinNoticeTimer -= dt;

                // Add Coin: Select button on controller OR C / 5 on keyboard
                if (input.coinPressed) {
                    credits++;
                    AudioManager::PlaySfx(SFX_COIN_INSERT);
                    coinNoticeTimer = 2.0f;
                }

                // Start: Start button on controller OR Enter / 1 on keyboard
                if (input.startPressed) {
                    if (credits > 0) {
                        credits--;
                        AudioManager::PlaySfx(SFX_VICTORY);
                        ResetGame(true);
                        gameState = GameState::GAMEPLAY;
                    } else {
                        coinNoticeTimer = 2.5f;
                        AudioManager::PlaySfx(SFX_SHIELD_DEFLECT);
                    }
                }
                break;
            }

            case GameState::GAMEPLAY: {
                bool screenFrozen = player.IsWrathScreenFrozen();

                // Check distance to closest enemy for auto-melee check
                bool enemyInFront = false;
                float closestDist = enemyManager.GetClosestEnemyDistance(player.GetPosition(), player.IsFacingLeft(), enemyInFront);

                // Update Player
                player.Update(dt, input, level, closestDist, enemyInFront);

                // Update Level
                level.Update(dt, player.GetPosition());

                // Update Enemies
                bool wrathWipe = player.ShouldTriggerWrathBoardClear();
                enemyManager.Update(dt, player.GetPosition(), level, screenFrozen, wrathWipe);
                if (wrathWipe) {
                    player.ClearWrathBoardClearTrigger();
                }

                // Check Combat Interactions
                if (!screenFrozen) {
                    enemyManager.CheckPlayerAttacks(player);
                    enemyManager.CheckPlayerDamage(player, level.IsArenaLockedDown());
                }

                Enemy* boss = enemyManager.GetBoss();

                // If Centurion Brutus has moved into the arena in Phase 1, unlock the gate so Jesus can follow
                if (boss && (boss->GetBossWhipState() == BossWhipState::PHASE1_WALK_OFF || boss->HasPhase1WalkedOff()) && !level.IsArenaUnlocked()) {
                    level.UnlockArena();
                }

                // When entering the Centurion arena:
                // Jesus starts on the LEFT side of the Centurion fight:
                if (level.IsArenaLockedDown() && !arenaSetupDone) {
                    arenaSetupDone = true;
                    player.SetPosition({ ARENA_MIN_X + 24.0f, GROUND_Y - 30.0f });
                    player.SetFacingLeft(false); // Face right towards Brutus

                    // Brutus is positioned on the right side of the arena:
                    if (boss) {
                        boss->SetPosition({ ARENA_MAX_X - 60.0f, GROUND_Y - 48.0f });
                        boss->SetFacingLeft(true); // Face left towards Jesus
                        boss->StartArenaCombat();
                    }
                    camera.target.x = ARENA_MIN_X - 10.0f;
                }

                // Check "FINISH HIM" trigger: when Brutus is down to 1 HP pip ("one square")
                if (boss && boss->GetHp() == 1 && !finishHimTriggered) {
                    finishHimTriggered = true;
                    finishHimBannerTimer = 3.5f;
                    AudioManager::PlaySfx(SFX_FINISH_HIM);
                    AudioManager::PlaySfx(SFX_THUNDER);
                }

                // Update storm clouds and lightning
                if (finishHimTriggered) {
                    cloudCoverage = std::min(1.0f, cloudCoverage + 0.85f * dt);

                    lightningTimer -= dt;
                    if (lightningTimer <= 0.0f) {
                        lightningFlashTimer = 0.12f;
                        lightningTimer = 2.8f + (float)(rand() % 30) / 10.0f;
                        AudioManager::PlaySfx(SFX_THUNDER);
                    }
                    if (lightningFlashTimer > 0.0f) {
                        lightningFlashTimer -= dt;
                    }

                    if (finishHimBannerTimer > 0.0f) {
                        finishHimBannerTimer -= dt;
                    }
                }

                // Check Withering Death & Heavenly Ascension sequence
                if (boss && boss->IsWithering()) {
                    ascensionSequenceTimer += dt;
                    AudioManager::StopAllMusic(); // Hushed reverent atmosphere

                    // Brutus withers into the ground; clouds part & divine beam descends
                    if (ascensionSequenceTimer >= 1.5f) {
                        cloudsPartAmount = std::min(1.0f, cloudsPartAmount + 0.75f * dt);
                        divineBeamIntensity = std::min(1.0f, divineBeamIntensity + 0.85f * dt);

                        if (!ascensionStarted) {
                            ascensionStarted = true;
                            AudioManager::PlaySfx(SFX_HOLY_BEAM);
                            AudioManager::PlaySfx(SFX_ANGEL_CHOIR);
                            player.StartAscension();
                        }
                    }

                    if (ascensionStarted) {
                        player.UpdateAscension(dt);

                        if (ascensionSequenceTimer >= 5.0f) {
                            celestialFadeAlpha = std::min(1.0f, celestialFadeAlpha + 0.75f * dt);
                        }

                        if (ascensionSequenceTimer >= 6.8f || celestialFadeAlpha >= 1.0f) {
                            gameState = GameState::VICTORY;
                            victoryTimer = 0.0f;
                        }
                    }
                }

                // Gate Warning Notice
                if (!level.IsArenaUnlocked() && fabsf(player.GetPosition().x - ARENA_GATE_X) < 80.0f) {
                    gateNoticeTimer = 2.0f;
                }
                if (gateNoticeTimer > 0.0f) gateNoticeTimer -= dt;

                // Update Camera Horizontal Follow
                float targetCamX = player.GetPosition().x - (VIRTUAL_WIDTH / 2.0f) + 16.0f;
                if (!level.IsArenaLockedDown()) {
                    // Before arena lockdown: clamp camera between 0 and gate
                    float maxCamX = ARENA_GATE_X - VIRTUAL_WIDTH + 60.0f;
                    if (targetCamX < 0.0f) targetCamX = 0.0f;
                    if (targetCamX > maxCamX) targetCamX = maxCamX;
                } else {
                    // Inside arena: clamp camera to arena bounds
                    float minArenaCam = ARENA_MIN_X - 10.0f;
                    float maxArenaCam = ARENA_MAX_X - VIRTUAL_WIDTH;
                    if (targetCamX < minArenaCam) targetCamX = minArenaCam;
                    if (targetCamX > maxArenaCam) targetCamX = maxArenaCam;
                }
                // Smooth camera damp
                camera.target.x += (targetCamX - camera.target.x) * 12.0f * dt;

                // Check Game Over
                if (player.IsDead()) {
                    gameOverTimer += dt;
                    if (gameOverTimer >= 1.5f) {
                        gameState = GameState::GAMEOVER;
                        continueCountdown = 9.9f;
                    }
                }
                break;
            }

            case GameState::GAMEOVER: {
                continueCountdown -= dt;

                // Insert coin during Continue / Resurrect countdown
                if (input.coinPressed) {
                    credits++;
                    AudioManager::PlaySfx(SFX_COIN_INSERT);
                }

                // Press Start to Resurrect with another credit!
                if (input.startPressed || ((input.jumpPressed || input.attackPressed) && credits > 0)) {
                    if (credits > 0) {
                        credits--;
                        AudioManager::PlaySfx(SFX_HOLY_BEAM);
                        bool inBoss = level.IsArenaLockedDown();
                        player.Resurrect(inBoss);

                        if (inBoss) {
                            camera.target.x = ARENA_MIN_X - 10.0f;
                            Enemy* b = enemyManager.GetBoss();
                            if (b) {
                                b->StartArenaCombat();
                            }
                        }

                        gameState = GameState::GAMEPLAY;
                        gameOverTimer = 0.0f;
                        continueCountdown = 9.9f;
                    }
                }

                // If countdown expires, return to Title screen
                if (continueCountdown <= 0.0f) {
                    ResetGame(true);
                    gameState = GameState::TITLE;
                    continueCountdown = 9.9f;
                }
                break;
            }

            case GameState::VICTORY: {
                victoryTimer += dt;
                // After 2.5 seconds or immediately on button press, return back to main screen
                if (victoryTimer >= 2.5f || input.startPressed || input.jumpPressed || input.attackPressed || input.coinPressed) {
                    ResetGame(true);
                    gameState = GameState::TITLE;
                }
                break;
            }
        }

        // Audio System Update
        bool inArena = level.IsArenaLockedDown();
        bool bossWithering = (enemyManager.GetBoss() && enemyManager.GetBoss()->IsWithering());
        if (!bossWithering && !ascensionStarted) {
            AudioManager::Update(inArena, gameState == GameState::GAMEOVER, gameState == GameState::VICTORY);
        }

        // --- Render to 320x224 Off-Screen Target Texture ---
        BeginTextureMode(targetBuffer);
        ClearBackground(BLACK);

        if (gameState == GameState::TITLE) {
            // Draw Title Splash Screen (1024x571 scaled cleanly to 320x224)
            GraphicsManager::DrawTitleSplash(VIRTUAL_WIDTH, VIRTUAL_HEIGHT);

            // Warning Notice if player pressed Start without coins
            if (coinNoticeTimer > 0.0f && credits == 0) {
                const char* alert = "! INSERT COIN FIRST (SELECT / C) !";
                int aw = MeasureText(alert, 10);
                DrawRectangle((VIRTUAL_WIDTH - aw) / 2 - 8, 150, aw + 16, 16, Color{ 60, 10, 15, 230 });
                DrawRectangleLines((VIRTUAL_WIDTH - aw) / 2 - 8, 150, aw + 16, 16, COLOR_ROMAN_CRIMSON);
                DrawText(alert, (VIRTUAL_WIDTH - aw) / 2, 154, 10, COLOR_GOLD_BRIGHT);
            } else {
                // Subtle arcade prompt pill
                const char* hint = "[SELECT] INSERT COIN    [START] PLAY";
                int hw = MeasureText(hint, 10);
                DrawRectangle((VIRTUAL_WIDTH - hw) / 2 - 6, 152, hw + 12, 14, Color{ 0, 0, 0, 160 });
                DrawText(hint, (VIRTUAL_WIDTH - hw) / 2, 155, 10, COLOR_ROMAN_STONE);
            }

            // Blinking Action Prompt
            bool blink = (static_cast<int>(GetTime() * 2.8f) % 2 == 0);
            if (credits == 0) {
                if (blink) {
                    const char* coinPrompt = "INSERT COIN (SELECT BUTTON / C)";
                    int cpw = MeasureText(coinPrompt, 10);
                    DrawRectangle((VIRTUAL_WIDTH - cpw) / 2 - 8, 172, cpw + 16, 16, Color{ 0, 0, 0, 190 });
                    DrawRectangleLines((VIRTUAL_WIDTH - cpw) / 2 - 8, 172, cpw + 16, 16, COLOR_ROMAN_GOLD);
                    DrawText(coinPrompt, (VIRTUAL_WIDTH - cpw) / 2, 176, 10, COLOR_ROMAN_GOLD);
                }
            } else {
                if (blink) {
                    const char* startPrompt = "PRESS START BUTTON TO PLAY!";
                    int spw = MeasureText(startPrompt, 10);
                    DrawRectangle((VIRTUAL_WIDTH - spw) / 2 - 8, 172, spw + 16, 16, Color{ 0, 0, 0, 190 });
                    DrawRectangleLines((VIRTUAL_WIDTH - spw) / 2 - 8, 172, spw + 16, 16, COLOR_GOLD_BRIGHT);
                    DrawText(startPrompt, (VIRTUAL_WIDTH - spw) / 2, 176, 10, COLOR_GOLD_BRIGHT);
                }
            }

            // Bottom Arcade Status Bar (Sleek dark Roman-bordered bar)
            DrawRectangle(0, VIRTUAL_HEIGHT - 22, VIRTUAL_WIDTH, 22, Color{ 10, 8, 14, 230 });
            DrawLine(0, VIRTUAL_HEIGHT - 22, VIRTUAL_WIDTH, VIRTUAL_HEIGHT - 22, COLOR_ROMAN_GOLD);

            // Detected Controller Status (Left)
            char padStr[64];
            if (InputHandler::IsGamepadActive()) {
                snprintf(padStr, sizeof(padStr), "PAD: %.18s", InputHandler::GetActiveControllerName());
                DrawText(padStr, 8, VIRTUAL_HEIGHT - 16, 10, Color{ 80, 255, 140, 255 });
            } else {
                DrawText("PAD: NONE (KEYBOARD)", 8, VIRTUAL_HEIGHT - 16, 10, Color{ 160, 160, 175, 255 });
            }

            // Credits Display (Right)
            char credStr[32];
            snprintf(credStr, sizeof(credStr), "CREDIT %02d", credits);
            int csw = MeasureText(credStr, 10);
            DrawText(credStr, VIRTUAL_WIDTH - csw - 8, VIRTUAL_HEIGHT - 16, 10, COLOR_ROMAN_GOLD);
        } else {
            // Gameplay / Victory / Gameover In-World Rendering
            // 1. Parallax Sky & Midground
            GraphicsManager::DrawParallaxBackground(camera.target.x, static_cast<float>(GetTime()));

            // 1b. Apocalyptic Storm Clouds (rolling in when Brutus is at 1 HP pip)
            if (cloudCoverage > 0.0f) {
                GraphicsManager::DrawStormClouds(cloudCoverage, GetTime(), lightningFlashTimer > 0.0f);
            }

            // 1c. Parted Clouds & Divine Ascension Light Pillar
            if (divineBeamIntensity > 0.0f || cloudsPartAmount > 0.0f) {
                float jesusScreenX = player.GetPosition().x - camera.target.x + 14.0f;
                GraphicsManager::DrawDivineAscensionBeam(jesusScreenX, divineBeamIntensity, cloudsPartAmount, GetTime());
            }

            // 2. World Space Layer (Level, Enemies, Player)
            BeginMode2D(camera);
            level.Draw(camera.target.x);
            enemyManager.Draw();
            player.Draw();
            EndMode2D();

            // 3. Divine Wrath Screen Flash Overlay
            float flashAlpha = player.GetScreenFlashAlpha();
            if (flashAlpha > 0.0f) {
                unsigned char fa = static_cast<unsigned char>(flashAlpha * 255);
                DrawRectangle(0, 0, VIRTUAL_WIDTH, VIRTUAL_HEIGHT, Color{ 255, 255, 255, fa });
            }

            // 3b. "FINISH HIM!" Arcade Banner
            if (finishHimBannerTimer > 0.0f) {
                float bannerA = std::min(1.0f, finishHimBannerTimer);
                GraphicsManager::DrawFinishHimBanner(GetTime(), bannerA);
            }

            // 4. Arcade HUD (Top Screen)
            DrawRectangle(0, 0, VIRTUAL_WIDTH, 20, COLOR_HUD_BG);
            DrawLine(0, 20, VIRTUAL_WIDTH, 20, COLOR_ROMAN_DARK);

            // Score
            char scoreText[32];
            snprintf(scoreText, sizeof(scoreText), "SCORE %06d", player.GetScore());
            DrawText(scoreText, 8, 5, 10, COLOR_ROMAN_GOLD);

            // Player Health Pips (Hearts)
            DrawText("LIFE", 110, 5, 10, COLOR_ROBE_WHITE);
            int effMax = player.GetEffectiveMaxHp();
            for (int i = 0; i < PLAYER_MAX_HP; ++i) {
                float hx = 138.0f + i * 10.0f;
                float hy = 6.0f;
                if (i < player.GetHp()) {
                    GraphicsManager::DrawSprite(SPR_ICON_HEART, hx, hy);
                } else if (i < effMax) {
                    DrawRectangleLines(static_cast<int>(hx), static_cast<int>(hy), 8, 8, Color{ 90, 80, 80, 255 });
                } else {
                    // Shattered unrefillable heart slot on boss level
                    int ix = static_cast<int>(hx);
                    int iy = static_cast<int>(hy);
                    DrawRectangle(ix, iy, 8, 8, Color{ 45, 10, 14, 230 });
                    DrawRectangleLines(ix, iy, 8, 8, Color{ 160, 30, 40, 255 });
                    DrawLine(ix + 1, iy + 1, ix + 7, iy + 7, Color{ 235, 35, 45, 255 });
                    DrawLine(ix + 7, iy + 1, ix + 1, iy + 7, Color{ 235, 35, 45, 255 });
                }
            }

            // Divine Wrath Ninjutsu Icon
            DrawText("WRATH", 180, 5, 10, COLOR_ROBE_WHITE);
            if (player.HasWrathAvailable()) {
                GraphicsManager::DrawSprite(SPR_ICON_WRATH, 218.0f, 5.0f);
            } else {
                DrawRectangleLines(218, 5, 10, 10, Color{ 80, 75, 70, 255 });
            }

            // Loaves and Fishes Baskets (X/4)
            GraphicsManager::DrawSprite(SPR_ICON_BASKET, 246.0f, 2.0f);
            char basketText[16];
            snprintf(basketText, sizeof(basketText), "%d/%d", level.GetBasketsCollectedCount(), level.GetTotalBasketsCount());
            DrawText(basketText, 268, 5, 10, level.AreAllBasketsCollected() ? Color{ 80, 255, 120, 255 } : COLOR_ROMAN_GOLD);

            // Lives
            char livesText[16];
            snprintf(livesText, sizeof(livesText), "x%d", player.GetLives());
            DrawText(livesText, 298, 5, 10, COLOR_ROBE_WHITE);

            // 5. Boss Health Bar: Named "BRUTUS" (Bottom Screen when inside Arena)
            if (level.IsArenaLockedDown()) {
                Enemy* b = enemyManager.GetBoss();
                if (b && !b->IsWitheredDead()) {
                    DrawRectangle(50, VIRTUAL_HEIGHT - 18, VIRTUAL_WIDTH - 100, 14, COLOR_HUD_BG);
                    DrawRectangleLines(50, VIRTUAL_HEIGHT - 18, VIRTUAL_WIDTH - 100, 14, COLOR_ROMAN_CRIMSON);

                    DrawText("BRUTUS", 56, VIRTUAL_HEIGHT - 16, 10, COLOR_ROMAN_GOLD);

                    int maxBossPips = b->GetMaxHp();
                    int curBossPips = b->GetHp();
                    int barStartX = 135;
                    int pipW = 7;
                    for (int p = 0; p < maxBossPips; ++p) {
                        if (p < curBossPips) {
                            DrawRectangle(barStartX + p * 8, VIRTUAL_HEIGHT - 15, pipW, 8, COLOR_ROMAN_CRIMSON);
                        } else {
                            DrawRectangleLines(barStartX + p * 8, VIRTUAL_HEIGHT - 15, pipW, 8, Color{ 70, 60, 60, 255 });
                        }
                    }
                }
            }

            // 5b. Smooth Celestial Golden Bloom / Fade to Heaven
            if (celestialFadeAlpha > 0.0f) {
                DrawRectangle(0, 0, VIRTUAL_WIDTH, VIRTUAL_HEIGHT, ColorAlpha(COLOR_GOLD_BRIGHT, celestialFadeAlpha));
                DrawRectangle(0, 0, VIRTUAL_WIDTH, VIRTUAL_HEIGHT, ColorAlpha(WHITE, celestialFadeAlpha * 0.75f));
            }

            // 6. In-game Alerts & Exit Arrow
            if (gateNoticeTimer > 0.0f && !level.IsArenaUnlocked()) {
                const char* msg = "COLLECT ALL 4 BASKETS TO UNLOCK GATE!";
                int mw = MeasureText(msg, 10);
                DrawRectangle((VIRTUAL_WIDTH - mw) / 2 - 6, 40, mw + 12, 16, Color{ 20, 15, 25, 230 });
                DrawRectangleLines((VIRTUAL_WIDTH - mw) / 2 - 6, 40, mw + 12, 16, COLOR_ROMAN_CRIMSON);
                DrawText(msg, (VIRTUAL_WIDTH - mw) / 2, 44, 10, COLOR_ROMAN_GOLD);
            }

            // Phase 1 Walk-Off Arrow: Prompts Jesus to follow Brutus into the arena as Brutus exits screen 1!
            Enemy* b = enemyManager.GetBoss();
            bool showPhase1Follow = (b && (b->GetBossWhipState() == BossWhipState::PHASE1_WALK_OFF || b->HasPhase1WalkedOff()) && !level.IsArenaLockedDown());
            if (showPhase1Follow) {
                float bob = sinf(GetTime() * 9.0f) * 4.0f;
                float alpha = 0.85f + 0.15f * sinf(GetTime() * 12.0f);
                Color arrowCol = ColorAlpha(COLOR_GOLD_BRIGHT, alpha);
                Color bgCol = ColorAlpha(BLACK, 0.75f);

                int bx = VIRTUAL_WIDTH - 104 + static_cast<int>(bob);
                int by = 30;
                DrawRectangle(bx, by, 98, 18, bgCol);
                DrawRectangleLines(bx, by, 98, 18, arrowCol);
                DrawText("FOLLOW", bx + 6, by + 4, 10, arrowCol);
                DrawText(">>", bx + 74, by + 4, 10, COLOR_ROBE_WHITE);
            }

            // 7. Game Over Screen (Authentic 1987 Arcade Shinobi-Style Resurrect Countdown)
            if (gameState == GameState::GAMEOVER) {
                DrawRectangle(0, 0, VIRTUAL_WIDTH, VIRTUAL_HEIGHT, Color{ 22, 6, 12, 235 });

                // Dramatic flashing "RESURRECT??" title with arcade glow
                float flashTime = GetTime() * 5.0f;
                bool flashOn = (static_cast<int>(flashTime) % 2 == 0);
                Color resColor = flashOn ? COLOR_GOLD_BRIGHT : COLOR_ROBE_WHITE;
                Color glowColor = flashOn ? Color{ 245, 55, 65, 220 } : Color{ 160, 20, 30, 160 };

                const char* resText = "RESURRECT??";
                int resW = MeasureText(resText, 22);
                int rx = (VIRTUAL_WIDTH - resW) / 2;
                int ry = 34;

                // Pulsing glow aura
                DrawText(resText, rx - 1, ry, 22, glowColor);
                DrawText(resText, rx + 1, ry, 22, glowColor);
                DrawText(resText, rx, ry - 1, 22, glowColor);
                DrawText(resText, rx, ry + 1, 22, glowColor);
                DrawText(resText, rx, ry, 22, resColor);

                // Subtitle
                const char* subText = "THOU HAST FALLEN - SHALT THOU RISE AGAIN?";
                int subW = MeasureText(subText, 10);
                DrawText(subText, (VIRTUAL_WIDTH - subW) / 2, 62, 10, COLOR_ROMAN_GOLD);

                // Arcade Countdown Timer
                int countInt = static_cast<int>(continueCountdown);
                if (countInt < 0) countInt = 0;
                char countStr[16];
                snprintf(countStr, sizeof(countStr), "%d", countInt);
                Color countCol = (countInt <= 3) ? COLOR_CRIMSON_BRIGHT : COLOR_GOLD_BRIGHT;
                int numW = MeasureText(countStr, 34);
                int nx = (VIRTUAL_WIDTH - numW) / 2;
                int ny = 88;
                DrawText(countStr, nx + 1, ny + 1, 34, BLACK);
                DrawText(countStr, nx, ny, 34, countCol);

                // Blinking Action Prompt Pill
                bool promptBlink = (static_cast<int>(GetTime() * 3.5f) % 2 == 0);
                if (credits > 0) {
                    if (promptBlink) {
                        const char* prompt = "PRESS START TO RESURRECT!";
                        int pw = MeasureText(prompt, 10);
                        int px = (VIRTUAL_WIDTH - pw) / 2;
                        int py = 138;
                        DrawRectangle(px - 8, py - 3, pw + 16, 16, Color{ 0, 0, 0, 190 });
                        DrawRectangleLines(px - 8, py - 3, pw + 16, 16, COLOR_GOLD_BRIGHT);
                        DrawText(prompt, px, py, 10, COLOR_GOLD_BRIGHT);
                    }
                } else {
                    if (promptBlink) {
                        const char* prompt = "INSERT COIN TO RESURRECT (SELECT / C)";
                        int pw = MeasureText(prompt, 10);
                        int px = (VIRTUAL_WIDTH - pw) / 2;
                        int py = 138;
                        DrawRectangle(px - 8, py - 3, pw + 16, 16, Color{ 0, 0, 0, 190 });
                        DrawRectangleLines(px - 8, py - 3, pw + 16, 16, COLOR_ROMAN_GOLD);
                        DrawText(prompt, px, py, 10, COLOR_ROMAN_GOLD);
                    }
                }

                // Score
                char finalScore[32];
                snprintf(finalScore, sizeof(finalScore), "SCORE %06d", player.GetScore());
                DrawText(finalScore, (VIRTUAL_WIDTH - MeasureText(finalScore, 10)) / 2, 168, 10, COLOR_ROMAN_STONE);

                // Bottom Status Bar
                DrawRectangle(0, VIRTUAL_HEIGHT - 20, VIRTUAL_WIDTH, 20, Color{ 12, 6, 10, 230 });
                DrawLine(0, VIRTUAL_HEIGHT - 20, VIRTUAL_WIDTH, VIRTUAL_HEIGHT - 20, COLOR_ROMAN_GOLD);
                char credStr[32];
                snprintf(credStr, sizeof(credStr), "CREDIT %02d", credits);
                DrawText(credStr, VIRTUAL_WIDTH - MeasureText(credStr, 10) - 8, VIRTUAL_HEIGHT - 15, 10, COLOR_ROMAN_GOLD);
            }

            // 8. Victory Screen
            if (gameState == GameState::VICTORY) {
                DrawRectangle(0, 0, VIRTUAL_WIDTH, VIRTUAL_HEIGHT, Color{ 12, 22, 16, 230 });
                const char* vicTitle = "MISSION 1 COMPLETE!";
                int vw = MeasureText(vicTitle, 20);
                DrawText(vicTitle, (VIRTUAL_WIDTH - vw) / 2, 45, 20, COLOR_ROMAN_GOLD);

                const char* vicSub1 = "BRUTUS HAS WITHERED INTO THE DUST!";
                DrawText(vicSub1, (VIRTUAL_WIDTH - MeasureText(vicSub1, 10)) / 2, 75, 10, COLOR_ROBE_WHITE);

                const char* vicSub2 = "THE NAZARENE HAST ASCENDED INTO HEAVEN";
                DrawText(vicSub2, (VIRTUAL_WIDTH - MeasureText(vicSub2, 10)) / 2, 95, 10, COLOR_GOLD_BRIGHT);

                char vicScore[48];
                snprintf(vicScore, sizeof(vicScore), "VICTORY SCORE: %06d", player.GetScore() + 10000);
                DrawText(vicScore, (VIRTUAL_WIDTH - MeasureText(vicScore, 10)) / 2, 125, 10, COLOR_ROMAN_GOLD);

                const char* contPrompt = "RETURNING TO MAIN SCREEN...";
                DrawText(contPrompt, (VIRTUAL_WIDTH - MeasureText(contPrompt, 10)) / 2, 160, 10, Color{ 200, 200, 215, 255 });
            }
        }

        EndTextureMode();

        // --- Render Scaled Buffer to Main Window with Aspect Ratio Preservation ---
        BeginDrawing();
        ClearBackground(BLACK);

        int screenW = GetScreenWidth();
        int screenH = GetScreenHeight();

        // Calculate maximum integer or proportional scale preserving 320:224 aspect ratio
        float scale = std::min((float)screenW / VIRTUAL_WIDTH, (float)screenH / VIRTUAL_HEIGHT);
        int scaledW = static_cast<int>(VIRTUAL_WIDTH * scale);
        int scaledH = static_cast<int>(VIRTUAL_HEIGHT * scale);
        int offsetX = (screenW - scaledW) / 2;
        int offsetY = (screenH - scaledH) / 2;

        // Source rectangle: note negative height because OpenGL textures are inverted vertically
        Rectangle sourceRec = { 0.0f, 0.0f, (float)targetBuffer.texture.width, -(float)targetBuffer.texture.height };
        Rectangle destRec = { (float)offsetX, (float)offsetY, (float)scaledW, (float)scaledH };

        DrawTexturePro(targetBuffer.texture, sourceRec, destRec, { 0.0f, 0.0f }, 0.0f, WHITE);

        EndDrawing();
    }

    // Subsystem Shutdown
    UnloadRenderTexture(targetBuffer);
    GraphicsManager::Close();
    AudioManager::Close();
    CloseWindow();

    return 0;
}
