#include "raylib.h"
#include "Constants.h"
#include "Input.h"
#include "Audio.h"
#include "Graphics.h"
#include "Level.h"
#include "Player.h"
#include "Enemies.h"
#include "BonusStage.h"
#include <cmath>
#include <cstdio>
#include <algorithm>

enum class GameState {
    TITLE,
    MISSION_MAP,
    GAMEPLAY,
    BONUS_STAGE,
    GAMEOVER,
    VICTORY
};

int main() {
    // 1. Initialize Raylib Window
    SetConfigFlags(FLAG_VSYNC_HINT | FLAG_WINDOW_RESIZABLE);
    InitWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "The Nazarene: Way of the Cross");
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
    BonusStage bonusStage;
    InputState input;

    GameState gameState = GameState::TITLE;
    Camera2D camera = { { 0.0f, 0.0f }, { 0.0f, 0.0f }, 0.0f, 1.0f };

    int credits = 0;
    float coinNoticeTimer = 0.0f;
    float continueCountdown = 9.9f;

    int missionNumber = 1; // 1 = Jerusalem Campaign, 2 = Gethsemane Outskirts
    int currentArea = 1;   // 1, 2, or 3
    bool area1Cleared = false;
    bool area2Cleared = false;
    bool area3Cleared = false;
    float missionMapTimer = 0.0f;

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
    float rainbowAlpha = 0.0f;

    auto GetCurrentStageId = [&]() {
        return (missionNumber == 1) ? currentArea : ((missionNumber == 2) ? (3 + currentArea) : (6 + currentArea));
    };

    auto StartMission = [&](int mNum, int aNum) {
        missionNumber = mNum;
        currentArea = aNum;
        int stageId = GetCurrentStageId();
        level.Reset(stageId);
        player.Reset(mNum == 1 && aNum == 1);
        enemyManager.Reset(stageId);
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
        rainbowAlpha = 0.0f;
    };

    auto ResetGame = [&](bool fullReset) {
        if (fullReset) {
            missionNumber = 1;
            currentArea = 1;
            area1Cleared = false;
            area2Cleared = false;
            area3Cleared = false;
        }
        StartMission(missionNumber, currentArea);
    };

    int crtMode = 0; // 0 = Arcade CRT (Scanlines + Mask), 1 = Soft Scanlines, 2 = Sharp Pixels
    float crtNoticeTimer = 0.0f;

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

        // Toggle Arcade CRT Display Mode (F10)
        if (IsKeyPressed(KEY_F10)) {
            crtMode = (crtMode + 1) % 3;
            crtNoticeTimer = 2.5f;
            AudioManager::PlaySfx(SFX_COIN_INSERT);
        }
        if (crtNoticeTimer > 0.0f) crtNoticeTimer -= dt;

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
                        AudioManager::PlaySfx(SFX_COIN_INSERT);
                        missionNumber = 1;
                        currentArea = 1;
                        area1Cleared = false;
                        area2Cleared = false;
                        area3Cleared = false;
                        gameState = GameState::MISSION_MAP;
                        missionMapTimer = 0.0f;
                    } else {
                        coinNoticeTimer = 2.5f;
                        AudioManager::PlaySfx(SFX_SHIELD_DEFLECT);
                    }
                }
                break;
            }

            case GameState::MISSION_MAP: {
                missionMapTimer += dt;
                // Allow pressing Start, Jump, Attack to immediately embark, or auto-start after 4.2 seconds
                if (missionMapTimer >= 0.5f && (input.startPressed || input.jumpPressed || input.attackPressed || missionMapTimer >= 4.2f)) {
                    StartMission(missionNumber, currentArea);
                    gameState = GameState::GAMEPLAY;
                    AudioManager::PlaySfx(SFX_VICTORY);
                }
                break;
            }

            case GameState::GAMEPLAY: {
                bool screenFrozen = player.IsWrathScreenFrozen();
                int currentStage = GetCurrentStageId();

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

                // Stage 3, Stage 6, & Stage 9: As soon as player finds the boss / approaches gate, clouds of heaven roll in!
                if (currentStage == 3 || currentStage == 6 || currentStage == 9) {
                    if (player.GetPosition().x >= (level.GetArenaGateX() - 100.0f) || level.IsArenaLockedDown()) {
                        cloudCoverage = std::min(1.0f, cloudCoverage + 0.65f * dt);
                    }
                }

                // When entering the arena:
                // Jesus starts on the LEFT side of the boss fight:
                if (level.IsArenaLockedDown() && !arenaSetupDone) {
                    arenaSetupDone = true;
                    player.SetPosition({ level.GetArenaMinX() + 24.0f, GROUND_Y - 30.0f });
                    player.SetFacingLeft(false); // Face right towards Boss

                    // Boss is positioned on the right side of the arena:
                    if (boss) {
                        boss->SetPosition({ level.GetArenaMaxX() - 60.0f, GROUND_Y - 48.0f });
                        boss->SetFacingLeft(true); // Face left towards Jesus
                        boss->StartArenaCombat();
                    }
                    camera.target.x = level.GetArenaMinX() - 10.0f;
                }

                // Check "FINISH HIM" trigger: when Boss is down to 1 HP pip ("one square")
                if (boss && boss->GetHp() == 1 && !finishHimTriggered) {
                    finishHimTriggered = true;
                    finishHimBannerTimer = 3.5f;
                    AudioManager::PlaySfx(SFX_FINISH_HIM);
                    AudioManager::PlaySfx(SFX_THUNDER);
                }

                // Update storm clouds and lightning
                if (finishHimTriggered || ((currentStage == 3 || currentStage == 6 || currentStage == 9) && cloudCoverage > 0.0f)) {
                    if (finishHimTriggered) {
                        cloudCoverage = std::min(1.0f, cloudCoverage + 0.85f * dt);
                    }

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

                    // Boss withers into the ground; clouds part & divine beam descends
                    if (ascensionSequenceTimer >= 1.5f) {
                        cloudsPartAmount = std::min(1.0f, cloudsPartAmount + 0.75f * dt);
                        divineBeamIntensity = std::min(1.0f, divineBeamIntensity + 0.85f * dt);

                        if (currentStage == 3 || currentStage == 6 || currentStage == 9) {
                            rainbowAlpha = std::min(1.0f, rainbowAlpha + 0.80f * dt);
                        }

                        if (!ascensionStarted) {
                            ascensionStarted = true;
                            AudioManager::PlaySfx(SFX_HOLY_BEAM);
                            AudioManager::PlaySfx(SFX_ANGEL_CHOIR);
                            // In Stage 3, Stage 6, & Stage 9, Jesus ascends diagonally towards opposite side of screen!
                            bool diagonal = (currentStage == 3 || currentStage == 6 || currentStage == 9);
                            bool toRight = (player.GetPosition().x < level.GetArenaMinX() + (level.GetArenaMaxX() - level.GetArenaMinX()) * 0.5f);
                            player.StartAscension(diagonal, toRight);
                        }
                    }

                    if (ascensionStarted) {
                        player.UpdateAscension(dt);

                        if (ascensionSequenceTimer >= 5.2f) {
                            celestialFadeAlpha = std::min(1.0f, celestialFadeAlpha + 0.75f * dt);
                        }

                        if (ascensionSequenceTimer >= 7.2f || celestialFadeAlpha >= 1.0f) {
                            if (missionNumber == 1) {
                                if (currentArea == 1) {
                                    area1Cleared = true;
                                    currentArea = 2;
                                    gameState = GameState::MISSION_MAP;
                                    missionMapTimer = 0.0f;
                                } else if (currentArea == 2) {
                                    area2Cleared = true;
                                    currentArea = 3;
                                    gameState = GameState::MISSION_MAP;
                                    missionMapTimer = 0.0f;
                                } else {
                                    // Stage 3 cleared! Launch BONUS STAGE!
                                    area3Cleared = true;
                                    gameState = GameState::BONUS_STAGE;
                                    bonusStage.Reset();
                                }
                            } else if (missionNumber == 2) {
                                // Mission 2: Progression to Gethsemane
                                if (currentArea == 1) {
                                    area1Cleared = true;
                                    currentArea = 2;
                                    gameState = GameState::MISSION_MAP;
                                    missionMapTimer = 0.0f;
                                } else if (currentArea == 2) {
                                    area2Cleared = true;
                                    currentArea = 3;
                                    gameState = GameState::MISSION_MAP;
                                    missionMapTimer = 0.0f;
                                } else {
                                    // Stage 6 cleared! Advance to Mission 3: The Last Supper!
                                    missionNumber = 3;
                                    currentArea = 1;
                                    area1Cleared = false;
                                    area2Cleared = false;
                                    area3Cleared = false;
                                    gameState = GameState::MISSION_MAP;
                                    missionMapTimer = 0.0f;
                                }
                            } else {
                                // Mission 3: Journey to the Upper Room & The Last Supper
                                if (currentArea == 1) {
                                    area1Cleared = true;
                                    currentArea = 2;
                                    gameState = GameState::MISSION_MAP;
                                    missionMapTimer = 0.0f;
                                } else if (currentArea == 2) {
                                    area2Cleared = true;
                                    currentArea = 3;
                                    gameState = GameState::MISSION_MAP;
                                    missionMapTimer = 0.0f;
                                } else {
                                    // Stage 9 cleared! Judas Iscariot defeated and redeemed at the Last Supper!
                                    area3Cleared = true;
                                    gameState = GameState::VICTORY;
                                    victoryTimer = 0.0f;
                                }
                            }
                        }
                    }
                }

                // Gate Warning Notice
                if (!level.IsArenaUnlocked() && fabsf(player.GetPosition().x - level.GetArenaGateX()) < 80.0f) {
                    gateNoticeTimer = 2.0f;
                }
                if (gateNoticeTimer > 0.0f) gateNoticeTimer -= dt;

                // Update Camera Horizontal Follow
                float targetCamX = player.GetPosition().x - (VIRTUAL_WIDTH / 2.0f) + 16.0f;
                if (!level.IsArenaLockedDown()) {
                    // Before arena lockdown: clamp camera between 0 and gate
                    float maxCamX = level.GetArenaGateX() - VIRTUAL_WIDTH + 60.0f;
                    if (targetCamX < 0.0f) targetCamX = 0.0f;
                    if (targetCamX > maxCamX) targetCamX = maxCamX;
                } else {
                    // Inside arena: clamp camera to arena bounds
                    float minArenaCam = level.GetArenaMinX() - 10.0f;
                    float maxArenaCam = level.GetArenaMaxX() - VIRTUAL_WIDTH;
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

            case GameState::BONUS_STAGE: {
                int pLives = player.GetLives();
                int pScore = player.GetScore();
                bonusStage.Update(dt, input, pLives, pScore);
                if (pLives > player.GetLives()) player.AddLives(pLives - player.GetLives());
                if (pScore > player.GetScore()) player.AddScore(pScore - player.GetScore());

                if (bonusStage.IsFinished()) {
                    // Advance to Mission 2!
                    missionNumber = 2;
                    currentArea = 1;
                    area1Cleared = false;
                    area2Cleared = false;
                    area3Cleared = false;
                    gameState = GameState::MISSION_MAP;
                    missionMapTimer = 0.0f;
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
                        player.Resurrect(inBoss, level.GetArenaMinX() + 24.0f);

                        if (inBoss) {
                            camera.target.x = level.GetArenaMinX() - 10.0f;
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
                // After 4.0 seconds or immediately on button press, return back to main screen
                if (victoryTimer >= 4.0f || input.startPressed || input.jumpPressed || input.attackPressed || input.coinPressed) {
                    ResetGame(true);
                    gameState = GameState::TITLE;
                }
                break;
            }
        }

        // Audio System Update
        bool inArena = level.IsArenaLockedDown();
        bool bossWithering = (enemyManager.GetBoss() && enemyManager.GetBoss()->IsWithering());
        int curStage = GetCurrentStageId();
        if (!bossWithering && !ascensionStarted) {
            AudioManager::Update(inArena, gameState == GameState::GAMEOVER, gameState == GameState::VICTORY, curStage, gameState == GameState::BONUS_STAGE);
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
        } else if (gameState == GameState::MISSION_MAP) {
            // Authentic Shinobi Arcade Jerusalem & Gethsemane Mission Map Screens
            GraphicsManager::DrawMissionMap(missionNumber, currentArea, area1Cleared, area2Cleared, area3Cleared, static_cast<float>(GetTime()));
        } else if (gameState == GameState::BONUS_STAGE) {
            // Sega Shinobi-Style 30-Second Bonus Stage: Feeding the 5,000 on the Mount of Beatitudes
            bonusStage.Draw();
        } else {
            int curStage = GetCurrentStageId();

            // Gameplay / Victory / Gameover In-World Rendering
            // 1. Parallax Sky & Midground (Supports all Stages 1 through 6)
            GraphicsManager::DrawParallaxBackground(camera.target.x, static_cast<float>(GetTime()), curStage);

            // 1b. Apocalyptic Storm Clouds (rolling in when Boss is at 1 HP pip or final stages)
            if (cloudCoverage > 0.0f) {
                GraphicsManager::DrawStormClouds(cloudCoverage, static_cast<float>(GetTime()), lightningFlashTimer > 0.0f);
            }

            // 1c. Parted Clouds & Divine Ascension Light Pillar
            if (divineBeamIntensity > 0.0f || cloudsPartAmount > 0.0f) {
                float jesusScreenX = player.GetPosition().x - camera.target.x + 14.0f;
                GraphicsManager::DrawDivineAscensionBeam(jesusScreenX, divineBeamIntensity, cloudsPartAmount, static_cast<float>(GetTime()));
            }

            // Rainbow Arc (Stage 3 & Stage 6 Finale)
            if (rainbowAlpha > 0.0f) {
                GraphicsManager::DrawRainbow(rainbowAlpha, static_cast<float>(GetTime()));
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
                GraphicsManager::DrawFinishHimBanner(static_cast<float>(GetTime()), bannerA);
            }

            // 4. MAME Arcade HUD (Top Screen) - 16-Bit Architecture
            DrawRectangle(0, 0, VIRTUAL_WIDTH, 22, Color{ 14, 12, 18, 240 });
            DrawLine(0, 21, VIRTUAL_WIDTH, 21, COLOR_ROMAN_GOLD);
            DrawLine(0, 22, VIRTUAL_WIDTH, 22, Color{ 35, 24, 14, 255 });

            // 1UP Marquee (Blinking)
            bool oneUpBlink = (static_cast<int>(GetTime() * 4.0f) % 2 == 0);
            Color oneUpCol = oneUpBlink ? COLOR_CRIMSON_BRIGHT : COLOR_ROMAN_CRIMSON;
            DrawText("1UP", 8, 4, 10, oneUpCol);

            // Player Score (Gold Digits)
            char scoreText[32];
            snprintf(scoreText, sizeof(scoreText), "%06d", player.GetScore());
            DrawText(scoreText, 32, 4, 10, COLOR_GOLD_BRIGHT);

            // Arcade Vitality Meter (Segmented Jeweled Roman Life Bar)
            DrawText("LIFE", 94, 4, 10, Color{ 220, 220, 230, 255 });
            int effMax = player.GetEffectiveMaxHp();
            int curHp = player.GetHp();
            bool lowHpFlash = (curHp == 1 && (static_cast<int>(GetTime() * 8.0f) % 2 == 0));

            // Gold Outer Inset Frame
            DrawRectangle(122, 4, 34, 12, Color{ 24, 18, 28, 255 });
            DrawRectangleLines(122, 4, 34, 12, COLOR_ROMAN_GOLD);

            for (int i = 0; i < PLAYER_MAX_HP; ++i) {
                int px = 124 + i * 10;
                int py = 6;
                if (i < curHp) {
                    Color pipCol = lowHpFlash ? WHITE : COLOR_CRIMSON_BRIGHT;
                    DrawRectangle(px, py, 8, 8, pipCol);
                    DrawRectangle(px, py, 8, 2, Color{ 255, 220, 220, 255 }); // Specular gleam
                    DrawRectangleLines(px, py, 8, 8, Color{ 110, 15, 22, 255 });
                } else if (i < effMax) {
                    DrawRectangle(px, py, 8, 8, Color{ 40, 32, 45, 255 });
                    DrawRectangleLines(px, py, 8, 8, Color{ 85, 75, 90, 255 });
                } else {
                    // Shattered socket on boss level
                    DrawRectangle(px, py, 8, 8, Color{ 25, 8, 12, 255 });
                    DrawLine(px, py, px + 7, py + 7, Color{ 170, 25, 35, 255 });
                    DrawLine(px + 7, py, px, py + 7, Color{ 170, 25, 35, 255 });
                }
            }

            // Divine Wrath Super Capsule
            DrawText("WRATH", 166, 4, 10, Color{ 220, 220, 230, 255 });
            if (player.HasWrathAvailable()) {
                bool wPulse = (static_cast<int>(GetTime() * 6.0f) % 2 == 0);
                Color wrathGlow = wPulse ? Color{ 80, 220, 255, 255 } : Color{ 40, 160, 255, 255 };
                DrawRectangle(206, 4, 14, 12, Color{ 15, 35, 60, 255 });
                DrawRectangleLines(206, 4, 14, 12, wrathGlow);
                GraphicsManager::DrawSprite(SPR_ICON_WRATH, 208.0f, 5.0f);
            } else {
                DrawRectangle(206, 4, 14, 12, Color{ 25, 22, 30, 255 });
                DrawRectangleLines(206, 4, 14, 12, Color{ 75, 70, 80, 255 });
            }

            // Stage Pickups: Baskets (1), Censers (2, 3), Pitchers (4), Wreaths (5), Chalices (6), Bitter Herbs (7), Matzot (8), Holy Grail (9)
            SpriteId hudPickupSpr = SPR_ICON_BASKET;
            if (curStage == 2 || curStage == 3) hudPickupSpr = SPR_CENSER_GOLD;
            else if (curStage == 4) hudPickupSpr = SPR_SILVER_PITCHER;
            else if (curStage == 5) hudPickupSpr = SPR_OLIVE_WREATH;
            else if (curStage == 6) hudPickupSpr = SPR_SACRED_CHALICE;
            else if (curStage == 7) hudPickupSpr = SPR_BITTER_HERBS;
            else if (curStage == 8) hudPickupSpr = SPR_PASSOVER_MATZOT;
            else if (curStage == 9) hudPickupSpr = SPR_HOLY_GRAIL;
            GraphicsManager::DrawSprite(hudPickupSpr, 230.0f, 2.0f);

            char basketText[16];
            snprintf(basketText, sizeof(basketText), "%d/%d", level.GetBasketsCollectedCount(), level.GetTotalBasketsCount());
            Color bCol = level.AreAllBasketsCollected() ? Color{ 80, 255, 120, 255 } : COLOR_ROMAN_GOLD;
            DrawText(basketText, 252, 4, 10, bCol);

            // Lives Counter
            char livesText[16];
            snprintf(livesText, sizeof(livesText), "x%d", player.GetLives());
            DrawText("RES", 282, 4, 10, Color{ 210, 190, 160, 255 });
            DrawText(livesText, 304, 4, 10, COLOR_GOLD_BRIGHT);

            // 5. Boss Health Bar (Bottom Screen when inside Arena)
            if (level.IsArenaLockedDown()) {
                Enemy* b = enemyManager.GetBoss();
                if (b && !b->IsWitheredDead()) {
                    int bw = VIRTUAL_WIDTH - 60;
                    int bx = (VIRTUAL_WIDTH - bw) / 2;
                    int by = VIRTUAL_HEIGHT - 20;

                    DrawRectangle(bx, by, bw, 16, Color{ 16, 12, 22, 245 });
                    DrawRectangleLines(bx, by, bw, 16, COLOR_ROMAN_GOLD);
                    DrawRectangleLines(bx + 1, by + 1, bw - 2, 14, Color{ 60, 42, 18, 255 });

                    const char* bossName = "CENTURION BRUTUS";
                    if (curStage == 2) bossName = "COMMANDER MALCHUS";
                    else if (curStage == 3) bossName = "PRAETOR LONGINUS";
                    else if (curStage == 4) bossName = "TORMENTOR DECURION";
                    else if (curStage == 5) bossName = "SHADOW INQUISITOR";
                    else if (curStage == 6) bossName = "CHRISTUS TENEBRIS";
                    else if (curStage == 7) bossName = "BARABBAS THE ZEALOT";
                    else if (curStage == 8) bossName = "HIGH PRIEST CAIAPHAS";
                    else if (curStage == 9) bossName = "JUDAS ISCARIOT";

                    DrawText(bossName, bx + 6, by + 3, 10, COLOR_GOLD_BRIGHT);

                    int maxBossPips = b->GetMaxHp();
                    int curBossPips = b->GetHp();
                    int barStartX = bx + bw - maxBossPips * 9 - 6;

                    for (int p = 0; p < maxBossPips; ++p) {
                        int px = barStartX + p * 9;
                        int py = by + 3;
                        if (p < curBossPips) {
                            DrawRectangle(px, py, 7, 10, COLOR_ROMAN_CRIMSON);
                            DrawRectangle(px, py, 7, 2, Color{ 255, 120, 130, 255 });
                            DrawRectangleLines(px, py, 7, 10, Color{ 90, 15, 22, 255 });
                        } else {
                            DrawRectangle(px, py, 7, 10, Color{ 35, 28, 38, 255 });
                            DrawRectangleLines(px, py, 7, 10, Color{ 70, 60, 75, 255 });
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
                if (curStage == 2) msg = "COLLECT ALL 4 SACRED CENSERS TO UNLOCK VEIL!";
                else if (curStage == 3) msg = "COLLECT ALL 4 SACRED RELICS TO UNLOCK PORTCULLIS!";
                else if (curStage == 4) msg = "COLLECT ALL 4 SILVER PITCHERS TO UNLOCK RAVINE BRIDGE!";
                else if (curStage == 5) msg = "COLLECT ALL 4 OLIVE WREATHS TO UNLOCK MAZE GATE!";
                else if (curStage == 6) msg = "COLLECT ALL 4 SACRED CHALICES TO UNLOCK GETHSEMANE GROVE!";
                else if (curStage == 7) msg = "COLLECT ALL 4 BITTER HERBS TO UNLOCK HIDEOUT GATE!";
                else if (curStage == 8) msg = "COLLECT ALL 4 PASSOVER MATZOT TO UNLOCK AUDIENCE GATE!";
                else if (curStage == 9) msg = "COLLECT ALL 4 HOLY GRAILS TO UNLOCK CENACLE SANCTUARY!";

                int mw = MeasureText(msg, 10);
                DrawRectangle((VIRTUAL_WIDTH - mw) / 2 - 6, 40, mw + 12, 16, Color{ 20, 15, 25, 230 });
                DrawRectangleLines((VIRTUAL_WIDTH - mw) / 2 - 6, 40, mw + 12, 16, COLOR_ROMAN_CRIMSON);
                DrawText(msg, (VIRTUAL_WIDTH - mw) / 2, 44, 10, COLOR_ROMAN_GOLD);
            }

            // Phase 1 Walk-Off Arrow: Prompts Jesus to follow Boss into arena as Boss exits screen 1!
            Enemy* b = enemyManager.GetBoss();
            bool showPhase1Follow = (b && (b->GetBossWhipState() == BossWhipState::PHASE1_WALK_OFF || b->HasPhase1WalkedOff()) && !level.IsArenaLockedDown());
            if (showPhase1Follow) {
                float bob = sinf(static_cast<float>(GetTime()) * 9.0f) * 4.0f;
                float alpha = 0.85f + 0.15f * sinf(static_cast<float>(GetTime()) * 12.0f);
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
                float flashTime = static_cast<float>(GetTime()) * 5.0f;
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
                bool promptBlink = (static_cast<int>(static_cast<float>(GetTime()) * 3.5f) % 2 == 0);
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
                const char* vicTitle = (missionNumber == 3) ? "THE LAST SUPPER RESTORED!" : "CAMPAIGN VICTORY!";
                int vw = MeasureText(vicTitle, 18);
                DrawText(vicTitle, (VIRTUAL_WIDTH - vw) / 2, 40, 18, COLOR_ROMAN_GOLD);

                const char* vicSub1 = (missionNumber == 3) ?
                    "JUDAS REPENTS - THE 30 PIECES OF SILVER SCATTERED" :
                    "GETHSEMANE CONQUERED - THE EVIL CLONE OVERCOME!";
                DrawText(vicSub1, (VIRTUAL_WIDTH - MeasureText(vicSub1, 10)) / 2, 68, 10, COLOR_ROBE_WHITE);

                const char* vicSub2 = (missionNumber == 3) ?
                    "THIS IS MY BODY, GIVEN FOR YOU - DO THIS IN REMEMBRANCE" :
                    "THE NAZARENE'S DIVINE MISSION IS FULFILLED";
                DrawText(vicSub2, (VIRTUAL_WIDTH - MeasureText(vicSub2, 10)) / 2, 88, 10, COLOR_GOLD_BRIGHT);

                if (missionNumber == 3) {
                    const char* vicSub3 = "PEACE UNTO ALL NATIONS - LOVE CONQUERS ALL";
                    DrawText(vicSub3, (VIRTUAL_WIDTH - MeasureText(vicSub3, 10)) / 2, 106, 10, COLOR_ROMAN_STONE);
                }

                char vicScore[48];
                int bonusPts = (missionNumber == 3) ? 100000 : 50000;
                snprintf(vicScore, sizeof(vicScore), "FINAL SCORE: %06d", player.GetScore() + bonusPts);
                DrawText(vicScore, (VIRTUAL_WIDTH - MeasureText(vicScore, 10)) / 2, 130, 10, COLOR_ROMAN_GOLD);

                const char* contPrompt = "RETURNING TO MAIN SCREEN...";
                DrawText(contPrompt, (VIRTUAL_WIDTH - MeasureText(contPrompt, 10)) / 2, 162, 10, Color{ 200, 200, 215, 255 });
            }
        }

        EndTextureMode();

        // --- Render Scaled Buffer to Main Window with Aspect Ratio Preservation ---
        BeginDrawing();
        ClearBackground(Color{ 10, 8, 14, 255 });

        int screenW = GetScreenWidth();
        int screenH = GetScreenHeight();

        // Calculate maximum integer or proportional scale preserving 320:224 aspect ratio
        float scale = std::min((float)screenW / VIRTUAL_WIDTH, (float)screenH / VIRTUAL_HEIGHT);
        int scaledW = static_cast<int>(VIRTUAL_WIDTH * scale);
        int scaledH = static_cast<int>(VIRTUAL_HEIGHT * scale);
        int offsetX = (screenW - scaledW) / 2;
        int offsetY = (screenH - scaledH) / 2;

        // 1. MAME Arcade Cabinet Bezel on Pillarboxes
        if (offsetX > 6) {
            Color bezelDark = Color{ 22, 18, 26, 255 };
            DrawRectangle(0, 0, offsetX, screenH, bezelDark);
            DrawRectangle(offsetX + scaledW, 0, screenW - (offsetX + scaledW), screenH, bezelDark);

            // Cabinet speaker grilles
            for (int gy = 40; gy < screenH - 40; gy += 16) {
                if (offsetX > 36) {
                    DrawRectangle(offsetX - 26, gy, 18, 4, Color{ 12, 10, 16, 255 });
                    DrawRectangle(offsetX + scaledW + 8, gy, 18, 4, Color{ 12, 10, 16, 255 });
                }
            }

            // Coin door marquee on right side
            if (screenW - (offsetX + scaledW) >= 110) {
                int cx = offsetX + scaledW + 14;
                int cy = screenH - 110;
                DrawRectangle(cx, cy, 90, 65, Color{ 14, 12, 18, 255 });
                DrawRectangleLines(cx, cy, 90, 65, Color{ 85, 70, 50, 255 });
                DrawRectangle(cx + 30, cy + 18, 30, 22, Color{ 235, 110, 30, 255 });
                DrawRectangleLines(cx + 30, cy + 18, 30, 22, Color{ 255, 190, 80, 255 });
                DrawText("25c", cx + 37, cy + 24, 10, WHITE);
                DrawText("INSERT COIN", cx + 12, cy + 46, 10, COLOR_ROMAN_GOLD);
            }

            // Left side system badge
            if (offsetX >= 110) {
                DrawText("SEGA SYSTEM 16", 12, 22, 10, COLOR_ROMAN_GOLD);
                DrawText("MAME ARCADE", 12, 36, 10, Color{ 170, 175, 195, 255 });
                DrawText("F10: CRT MODE", 12, screenH - 36, 10, Color{ 130, 135, 150, 255 });
            }
        }

        // 2. Draw Game Texture
        Rectangle sourceRec = { 0.0f, 0.0f, (float)targetBuffer.texture.width, -(float)targetBuffer.texture.height };
        Rectangle destRec = { (float)offsetX, (float)offsetY, (float)scaledW, (float)scaledH };
        DrawTexturePro(targetBuffer.texture, sourceRec, destRec, { 0.0f, 0.0f }, 0.0f, WHITE);

        // 3. Authentic Arcade CRT Raster Scanlines & Aperture Grille Mask
        if (crtMode == 0) { // Arcade CRT
            float lineStep = (float)scaledH / (float)VIRTUAL_HEIGHT;
            for (int y = 0; y < VIRTUAL_HEIGHT; ++y) {
                int sy = static_cast<int>(offsetY + y * lineStep + lineStep * 0.45f);
                DrawRectangle(offsetX, sy, scaledW, 1, Color{ 0, 0, 0, 85 });
            }
            // Vertical phosphor lines
            for (int x = 0; x < scaledW; x += 3) {
                DrawRectangle(offsetX + x, offsetY, 1, scaledH, Color{ 0, 0, 0, 26 });
            }
            // Cathode Vignette Shadow on borders
            DrawRectangleGradientV(offsetX, offsetY, scaledW, 12, Color{ 0, 0, 0, 110 }, BLANK);
            DrawRectangleGradientV(offsetX, offsetY + scaledH - 12, scaledW, 12, BLANK, Color{ 0, 0, 0, 110 });
            DrawRectangleGradientH(offsetX, offsetY, 12, scaledH, Color{ 0, 0, 0, 110 }, BLANK);
            DrawRectangleGradientH(offsetX + scaledW - 12, offsetY, 12, scaledH, BLANK, Color{ 0, 0, 0, 110 });
        } else if (crtMode == 1) { // Soft Scanlines
            float lineStep = (float)scaledH / (float)VIRTUAL_HEIGHT;
            for (int y = 0; y < VIRTUAL_HEIGHT; ++y) {
                int sy = static_cast<int>(offsetY + y * lineStep + lineStep * 0.45f);
                DrawRectangle(offsetX, sy, scaledW, 1, Color{ 0, 0, 0, 48 });
            }
        }

        // Inner 3D CRT Glass Frame Border
        DrawRectangleLinesEx({ (float)offsetX - 2, (float)offsetY - 2, (float)scaledW + 4, (float)scaledH + 4 }, 2.0f, Color{ 48, 38, 56, 255 });
        DrawRectangleLinesEx({ (float)offsetX - 1, (float)offsetY - 1, (float)scaledW + 2, (float)scaledH + 2 }, 1.0f, Color{ 14, 10, 18, 255 });

        // CRT Mode OSD Notification
        if (crtNoticeTimer > 0.0f) {
            const char* cMsg = (crtMode == 0) ? "CRT FILTER: ARCADE MONITOR" :
                               (crtMode == 1) ? "CRT FILTER: SOFT SCANLINES" : "CRT FILTER: OFF (SHARP PIXELS)";
            int tw = MeasureText(cMsg, 10);
            int tx = offsetX + (scaledW - tw) / 2;
            int ty = offsetY + scaledH - 30;
            float a = std::min(1.0f, crtNoticeTimer);
            DrawRectangle(tx - 8, ty - 3, tw + 16, 16, ColorAlpha(Color{ 10, 8, 14, 240 }, a));
            DrawRectangleLines(tx - 8, ty - 3, tw + 16, 16, ColorAlpha(COLOR_GOLD_BRIGHT, a));
            DrawText(cMsg, tx, ty, 10, ColorAlpha(COLOR_GOLD_BRIGHT, a));
        }

        EndDrawing();
    }

    // Subsystem Shutdown
    UnloadRenderTexture(targetBuffer);
    GraphicsManager::Close();
    AudioManager::Close();
    CloseWindow();

    return 0;
}
