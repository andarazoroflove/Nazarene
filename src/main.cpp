#include "raylib.h"
#include "Constants.h"
#include "Input.h"
#include "Audio.h"
#include "Graphics.h"
#include "Level.h"
#include "Player.h"
#include "Enemies.h"
#include "BonusStage.h"
#include "Mission5Graphics.h"
#include "Shaders.h"
#include <cmath>
#include <cstdio>
#include <algorithm>

enum class GameState {
    TITLE,
    SECRET_SELECT,
    MISSION_MAP,
    GAMEPLAY,
    BONUS_STAGE,
    GAMEOVER,
    VICTORY,
    MERCY_DIALOGUE,
    CRUCIFIXION_CUTSCENE,
    SKEE_BALL_HEAVEN
};

enum class SecretSelectStep {
    MISSION_SELECT = 0,
    STAGE_SELECT = 1
};

#if defined(__linux__)
#include <csignal>
static void CleanExitSignalHandler(int sig) {
    (void)sig;
    CloseWindow();
    exit(0);
}
#endif

int main(int argc, char* argv[]) {
    (void)argc;
    (void)argv;
#if defined(__linux__)
    std::signal(SIGINT, CleanExitSignalHandler);
    std::signal(SIGTERM, CleanExitSignalHandler);
#endif
    // Set working directory to application directory (critical for Anbernic / PortMaster / Linux)
    ChangeDirectory(GetApplicationDirectory());

    // 1. Initialize Raylib Window
    SetConfigFlags(FLAG_VSYNC_HINT | FLAG_WINDOW_RESIZABLE);
    InitWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "The Nazarene: Way of the Cross");
    SetExitKey(KEY_NULL);
#ifndef __linux__
    SetWindowMinSize(VIRTUAL_WIDTH * 2, VIRTUAL_HEIGHT * 2);
#endif
    SetTargetFPS(TARGET_FPS);

    // Embedded Gamepad Controller Database for Anbernic RG353V and Linux Handhelds
    static const char* s_embeddedGamepadDb =
        "19000000010000000100000001000000,ANBERNIC-keys,a:b0,b:b1,x:b2,y:b3,back:b8,start:b9,guide:b6,leftshoulder:b4,rightshoulder:b5,lefttrigger:b6,righttrigger:b7,dpup:h0.1,dpdown:h0.4,dpleft:h0.8,dpright:h0.2,leftx:a0,lefty:a1,platform:Linux,\n"
        "19000000010000000001000000010000,ANBERNIC-keys,a:b0,b:b1,x:b3,y:b2,back:b8,guide:b6,start:b7,leftshoulder:b4,rightshoulder:b5,dpup:h0.1,dpleft:h0.8,dpdown:h0.4,dpright:h0.2,leftx:a0,lefty:a1,lefttrigger:b9,righttrigger:b10,platform:Linux,\n"
        "19000000010000000100000001010000,Anbernic RG353V,platform:Linux,a:b0,b:b1,x:b2,y:b3,back:b8,start:b9,guide:b10,leftshoulder:b4,rightshoulder:b5,lefttrigger:b6,righttrigger:b7,leftstick:b11,rightstick:b12,dpup:h0.1,dpdown:h0.4,dpleft:h0.8,dpright:h0.2,leftx:a0,lefty:a1,rightx:a2,righty:a3,\n"
        "19000000010000000100000002010000,retrogame_joypad,platform:Linux,a:b0,b:b1,x:b2,y:b3,back:b8,start:b9,guide:b10,leftshoulder:b4,rightshoulder:b5,lefttrigger:b6,righttrigger:b7,leftstick:b11,rightstick:b12,dpup:h0.1,dpdown:h0.4,dpleft:h0.8,dpright:h0.2,leftx:a0,lefty:a1,rightx:a2,righty:a3,\n"
        "19000000010000000200000002000000,retrogame_joypad,platform:Linux,a:b0,b:b1,x:b2,y:b3,back:b8,start:b9,leftshoulder:b4,rightshoulder:b5,dpup:b10,dpdown:b11,dpleft:b12,dpright:b13,leftx:a0,lefty:a1,\n"
        "19000000010000000100000000010000,retrogame_joypad,platform:Linux,a:b0,b:b1,x:b2,y:b3,back:b8,start:b9,leftshoulder:b4,rightshoulder:b5,dpup:h0.1,dpdown:h0.4,dpleft:h0.8,dpright:h0.2,leftx:a0,lefty:a1,\n"
        "03000000091200000031000000010000,OpenSimHardware OSH PB Controller,platform:Linux,a:b0,b:b1,x:b2,y:b3,back:b8,start:b9,leftshoulder:b4,rightshoulder:b5,dpup:h0.1,dpdown:h0.4,dpleft:h0.8,dpright:h0.2,leftx:a0,lefty:a1,\n"
        "19000000010000000100000000000000,GO-Advance Gamepad,platform:Linux,a:b0,b:b1,x:b2,y:b3,back:b8,start:b9,leftshoulder:b4,rightshoulder:b5,lefttrigger:b6,righttrigger:b7,dpup:h0.1,dpdown:h0.4,dpleft:h0.8,dpright:h0.2,leftx:a0,lefty:a1,\n"
        "19000000010000000100000002000000,GO-Advance Gamepad (rev 1.1),platform:Linux,a:b0,b:b1,x:b2,y:b3,back:b8,start:b9,leftshoulder:b4,rightshoulder:b5,lefttrigger:b6,righttrigger:b7,dpup:b10,dpdown:b11,dpleft:b12,dpright:b13,leftx:a0,lefty:a1,\n"
        "030000005e0400008e02000010010000,Microsoft X-Box 360 pad,platform:Linux,a:b0,b:b1,x:b2,y:b3,back:b6,start:b7,guide:b8,leftshoulder:b4,rightshoulder:b5,leftstick:b9,rightstick:b10,leftx:a0,lefty:a1,rightx:a3,righty:a4,lefttrigger:a2,righttrigger:a5,dpup:h0.1,dpright:h0.2,dpdown:h0.4,dpleft:h0.8,\n";
    SetGamepadMappings(s_embeddedGamepadDb);
    if (FileExists("gamecontrollerdb.txt")) {
        char* fileMappings = LoadFileText("gamecontrollerdb.txt");
        if (fileMappings) {
            SetGamepadMappings(fileMappings);
            UnloadFileText(fileMappings);
        }
    }

    // 2. Initialize Subsystems
    AudioManager::Init();
    GraphicsManager::Init();

    // 3. Create Off-screen 320x224 Render Texture (Integer / Aspect Ratio Scaling)
    RenderTexture2D targetBuffer = LoadRenderTexture(VIRTUAL_WIDTH, VIRTUAL_HEIGHT);
    SetTextureFilter(targetBuffer.texture, TEXTURE_FILTER_POINT);

    // 3b. Load Hardware Arcade CRT Shader (Executive Order 16-bit arcade standard)
    Shader crtShader = LoadShaderFromMemory(nullptr, CRT_FRAGMENT_SHADER);
    int scanlineLoc = (crtShader.id > 0) ? GetShaderLocation(crtShader, "scanlineIntensity") : -1;
    int curvatureLoc = (crtShader.id > 0) ? GetShaderLocation(crtShader, "curvature") : -1;
    int bloomLoc = (crtShader.id > 0) ? GetShaderLocation(crtShader, "bloom") : -1;
    int timeLoc = (crtShader.id > 0) ? GetShaderLocation(crtShader, "time") : -1;
    int resLoc = (crtShader.id > 0) ? GetShaderLocation(crtShader, "resolution") : -1;

    float scanlineVal = 0.32f;
    float curvatureVal = 0.012f;
    float bloomVal = 0.16f;
    if (crtShader.id > 0) {
        if (scanlineLoc >= 0) SetShaderValue(crtShader, scanlineLoc, &scanlineVal, SHADER_UNIFORM_FLOAT);
        if (curvatureLoc >= 0) SetShaderValue(crtShader, curvatureLoc, &curvatureVal, SHADER_UNIFORM_FLOAT);
        if (bloomLoc >= 0) SetShaderValue(crtShader, bloomLoc, &bloomVal, SHADER_UNIFORM_FLOAT);
    }

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
    float stageTime = 180.0f; // 3-minute authentic arcade countdown timer
    SecretSelectStep secretStep = SecretSelectStep::MISSION_SELECT;
    int secretMissionIndex = 3; // 0 = M1, 1 = M2, 2 = M3, 3 = M4 (defaults to Mission IV)
    int secretStageIndex = 0;   // 0 = Board 1, 1 = Board 2, 2 = Board 3 (or 3 = Bonus for M1)

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

    // Mission 5: Golgotha Summit Climax tracking
    int m5BossStep = 0; // 0 = Green Clone, 1 = Devil, 2 = God/Mercy, 3 = Hallucination Stars, 4 = Pilate Battle
    bool m5DevilSpawned = false;
    bool m5PilateSpawned = false;
    float m5MercyTimer = 0.0f;
    int m5MercyChoice = 1; // 0 = Accept Mercy, 1 = Refuse False Mercy
    float m5CrucifixionTimer = 0.0f;
    float m5HallucinationTimer = 0.0f;
    float m5SkeeBallTimer = 0.0f;
    float m5SkeeBallFadeBlack = 0.0f;
    int m5SkeeBallScore = 0;
    int m5SkeeBallTickets = 0;
    int m5SkeeBallRolls = 0;
    float m5RollBallTimer = 0.0f;

    auto GetCurrentStageId = [&]() {
        if (missionNumber == 1) return currentArea;
        if (missionNumber == 2) return 3 + currentArea;
        if (missionNumber == 3) return 6 + currentArea;
        if (missionNumber == 4) return 9 + currentArea;
        return 13; // Mission 5 is Stage 13!
    };

    auto StartMission = [&](int mNum, int aNum) {
        missionNumber = mNum;
        currentArea = aNum;
        int stageId = GetCurrentStageId();
        level.Reset(stageId);
        player.Reset(mNum == 1 && aNum == 1);
        enemyManager.Reset(stageId);
        camera.target.x = 0.0f;
        stageTime = 180.0f;
        gateNoticeTimer = 0.0f;
        victoryTimer = 0.0f;
        gameOverTimer = 0.0f;
        continueCountdown = 9.9f;
        arenaSetupDone = false;
        finishHimTriggered = false;
        finishHimBannerTimer = 0.0f;
        cloudCoverage = (stageId == 13) ? 1.0f : 0.0f;
        lightningTimer = (stageId == 13) ? 2.2f : 3.5f;
        lightningFlashTimer = 0.0f;
        ascensionSequenceTimer = 0.0f;
        ascensionStarted = false;
        cloudsPartAmount = 0.0f;
        divineBeamIntensity = 0.0f;
        celestialFadeAlpha = 0.0f;
        rainbowAlpha = 0.0f;

        m5BossStep = 0;
        m5DevilSpawned = false;
        m5PilateSpawned = false;
        m5MercyTimer = 0.0f;
        m5MercyChoice = 1;
        m5CrucifixionTimer = 0.0f;
        m5HallucinationTimer = 0.0f;
        m5SkeeBallTimer = 0.0f;
        m5SkeeBallFadeBlack = 0.0f;
        m5SkeeBallScore = 0;
        m5SkeeBallTickets = 0;
        m5SkeeBallRolls = 0;
        m5RollBallTimer = 0.0f;
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

                // Secret Mission Select Trigger: Both Triggers on controller (or Q+E / [ ] / F1 on keyboard)
                if (input.secretSelectPressed) {
                    gameState = GameState::SECRET_SELECT;
                    secretStep = SecretSelectStep::MISSION_SELECT;
                    secretMissionIndex = 3;
                    secretStageIndex = 0;
                    AudioManager::PlaySfx(SFX_HOLY_BEAM);
                    break;
                }

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

            case GameState::SECRET_SELECT: {
                if (secretStep == SecretSelectStep::MISSION_SELECT) {
                    // Exit back to Title Screen
                    if (input.cancelPressed || input.secretSelectPressed) {
                        gameState = GameState::TITLE;
                        AudioManager::PlaySfx(SFX_SHIELD_DEFLECT);
                        break;
                    }

                    // 5-Mission Grid navigation:
                    // Row 0: [0: MISSION I]    [1: MISSION II]
                    // Row 1: [2: MISSION III]  [3: MISSION IV]
                    // Row 2: [4: MISSION V (Wide Card)]
                    if (input.leftPressed) {
                        if (secretMissionIndex == 1) secretMissionIndex = 0;
                        else if (secretMissionIndex == 0) secretMissionIndex = 1;
                        else if (secretMissionIndex == 3) secretMissionIndex = 2;
                        else if (secretMissionIndex == 2) secretMissionIndex = 3;
                        AudioManager::PlaySfx(SFX_COIN_RICOCHET);
                    } else if (input.rightPressed) {
                        if (secretMissionIndex == 0) secretMissionIndex = 1;
                        else if (secretMissionIndex == 1) secretMissionIndex = 0;
                        else if (secretMissionIndex == 2) secretMissionIndex = 3;
                        else if (secretMissionIndex == 3) secretMissionIndex = 2;
                        AudioManager::PlaySfx(SFX_COIN_RICOCHET);
                    } else if (input.upPressed) {
                        if (secretMissionIndex == 4) secretMissionIndex = 2;
                        else if (secretMissionIndex == 2) secretMissionIndex = 0;
                        else if (secretMissionIndex == 3) secretMissionIndex = 1;
                        else if (secretMissionIndex == 0 || secretMissionIndex == 1) secretMissionIndex = 4;
                        AudioManager::PlaySfx(SFX_COIN_RICOCHET);
                    } else if (input.downPressed) {
                        if (secretMissionIndex == 0) secretMissionIndex = 2;
                        else if (secretMissionIndex == 1) secretMissionIndex = 3;
                        else if (secretMissionIndex == 2 || secretMissionIndex == 3) secretMissionIndex = 4;
                        else if (secretMissionIndex == 4) secretMissionIndex = 0;
                        AudioManager::PlaySfx(SFX_COIN_RICOCHET);
                    }

                    // Confirm Mission -> Proceed to Stage Select
                    bool confirmPressed = input.startPressed || input.attackPressed || (input.jumpPressed && !input.cancelPressed);
                    if (confirmPressed) {
                        secretStep = SecretSelectStep::STAGE_SELECT;
                        secretStageIndex = 0;
                        AudioManager::PlaySfx(SFX_COIN_INSERT);
                    }
                } else {
                    // STAGE_SELECT (Sub-board 1, 2, or 3)
                    // Cancel / Back to Mission Select
                    if (input.cancelPressed) {
                        secretStep = SecretSelectStep::MISSION_SELECT;
                        AudioManager::PlaySfx(SFX_SHIELD_DEFLECT);
                        break;
                    }
                    if (input.secretSelectPressed) {
                        gameState = GameState::TITLE;
                        AudioManager::PlaySfx(SFX_SHIELD_DEFLECT);
                        break;
                    }

                    int maxStages = (secretMissionIndex == 4) ? 1 : ((secretMissionIndex == 0) ? 4 : 3);

                    // Navigate sub-boards 1, 2, 3 (and Beatitudes bonus for M1)
                    if (input.leftPressed || input.upPressed) {
                        secretStageIndex = (secretStageIndex - 1 + maxStages) % maxStages;
                        AudioManager::PlaySfx(SFX_COIN_RICOCHET);
                    } else if (input.rightPressed || input.downPressed) {
                        secretStageIndex = (secretStageIndex + 1) % maxStages;
                        AudioManager::PlaySfx(SFX_COIN_RICOCHET);
                    }

                    // Confirm Stage -> Embark!
                    bool confirmPressed = input.startPressed || input.attackPressed || (input.jumpPressed && !input.cancelPressed);
                    if (confirmPressed) {
                        credits = std::max(credits, 1);

                        if (secretMissionIndex == 0 && secretStageIndex == 3) {
                            gameState = GameState::BONUS_STAGE;
                            bonusStage.Reset();
                            AudioManager::PlaySfx(SFX_COIN_INSERT);
                        } else {
                            missionNumber = secretMissionIndex + 1;
                            currentArea = secretStageIndex + 1;
                            area1Cleared = (currentArea > 1);
                            area2Cleared = (currentArea > 2);
                            area3Cleared = false;
                            StartMission(missionNumber, currentArea);
                            gameState = GameState::GAMEPLAY;
                            AudioManager::PlaySfx(SFX_VICTORY);
                        }
                    }
                }
                break;
            }

            case GameState::MISSION_MAP: {
                // Select button on controller instantly ends mission and returns to main screen
                if (input.coinPressed) {
                    AudioManager::StopAllMusic();
                    AudioManager::PlaySfx(SFX_SHIELD_DEFLECT);
                    ResetGame(true);
                    gameState = GameState::TITLE;
                    break;
                }

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
                // Select button on controller instantly ends level and returns to main screen
                if (input.coinPressed) {
                    AudioManager::StopAllMusic();
                    AudioManager::PlaySfx(SFX_SHIELD_DEFLECT);
                    ResetGame(true);
                    gameState = GameState::TITLE;
                    break;
                }

                bool screenFrozen = player.IsWrathScreenFrozen();
                int currentStage = GetCurrentStageId();

                // Authentic Arcade Stage Countdown Timer
                if (stageTime > 0.0f && !player.IsDead() && !player.IsAscending() && !screenFrozen) {
                    stageTime -= dt;
                }

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

                // Stage 3, 6, 9, 12, & 13: As soon as player finds the boss / approaches gate, clouds of heaven roll in!
                if (currentStage == 3 || currentStage == 6 || currentStage == 9 || currentStage == 12 || currentStage == 13) {
                    if (currentStage == 13 || player.GetPosition().x >= (level.GetArenaGateX() - 100.0f) || level.IsArenaLockedDown()) {
                        cloudCoverage = std::min(1.0f, cloudCoverage + 0.65f * dt);
                    }
                }

                // When entering the arena:
                // Jesus starts on the LEFT side of the boss fight:
                if (level.IsArenaLockedDown() && !arenaSetupDone) {
                    arenaSetupDone = true;
                    if (currentStage == 13) {
                        player.SetPosition({ 80.0f, GROUND_Y - 30.0f });
                        player.SetFacingLeft(false);
                        if (boss) {
                            boss->SetPosition({ 380.0f, GROUND_Y - 32.0f });
                            boss->SetFacingLeft(true);
                            boss->StartArenaCombat();
                        }
                        camera.target.x = 0.0f;
                    } else {
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
                }

                // Stage 13 Golgotha Summit Boss Gauntlet Progression:
                // 1. Green Clone -> 2. The Devil -> 3. False Mercy -> 4. Hallucination Pilate
                if (currentStage == 13) {
                    if (m5BossStep == 0) {
                        if (boss && boss->GetType() == EnemyType::BOSS_MISSION5_CLONE_GREEN && (boss->IsDefeated() || boss->IsWitheredDead())) {
                            m5BossStep = 1;
                            enemyManager.ClearAllEnemies();
                            enemyManager.SpawnBossDirect(EnemyType::BOSS_DEVIL, Vector2{ 380.0f, GROUND_Y - 36.0f }, 40.0f, 440.0f);
                            Enemy* dev = enemyManager.GetBoss();
                            if (dev) {
                                dev->SetFacingLeft(true);
                                dev->StartArenaCombat();
                            }
                            finishHimTriggered = false;
                            finishHimBannerTimer = 0.0f;
                            AudioManager::PlaySfx(SFX_DEVIL_LAUGH);
                            AudioManager::PlaySfx(SFX_THUNDER);
                        }
                    } else if (m5BossStep == 1) {
                        if (boss && boss->GetType() == EnemyType::BOSS_DEVIL && boss->IsDefeated()) {
                            m5BossStep = 2;
                            enemyManager.ClearAllEnemies();
                            gameState = GameState::MERCY_DIALOGUE;
                            m5MercyTimer = 0.0f;
                            m5MercyChoice = 1;
                            cloudsPartAmount = 0.0f;
                            divineBeamIntensity = 0.0f;
                            rainbowAlpha = 0.0f;
                            finishHimTriggered = false;
                            finishHimBannerTimer = 0.0f;
                            AudioManager::StopAllMusic();
                            AudioManager::PlaySfx(SFX_RAINBOW_CHIME);
                            AudioManager::PlaySfx(SFX_ANGEL_CHOIR);
                        }
                    } else if (m5BossStep == 3) {
                        m5HallucinationTimer -= dt;
                        if (m5HallucinationTimer <= 0.0f) {
                            m5BossStep = 4;
                            AudioManager::PlaySfx(SFX_DING_DING);
                        }
                    }
                }

                // Check "FINISH HIM" trigger: when Boss is down to 1 HP pip ("one square")
                if (boss && boss->GetHp() == 1 && !finishHimTriggered) {
                    finishHimTriggered = true;
                    finishHimBannerTimer = 3.5f;
                    AudioManager::PlaySfx(SFX_FINISH_HIM);
                    AudioManager::PlaySfx(SFX_THUNDER);
                }

                // Update storm clouds and lightning
                if (finishHimTriggered || ((currentStage == 3 || currentStage == 6 || currentStage == 9 || currentStage == 12 || currentStage == 13) && cloudCoverage > 0.0f)) {
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

                        if (currentStage == 3 || currentStage == 6 || currentStage == 9 || currentStage == 12 || currentStage == 13) {
                            rainbowAlpha = std::min(1.0f, rainbowAlpha + 0.80f * dt);
                        }

                        if (!ascensionStarted) {
                            ascensionStarted = true;
                            AudioManager::PlaySfx(SFX_HOLY_BEAM);
                            AudioManager::PlaySfx(SFX_ANGEL_CHOIR);
                            // In Stage 3, 6, 9, 12, & 13, Jesus ascends diagonally towards opposite side of screen!
                            bool diagonal = (currentStage == 3 || currentStage == 6 || currentStage == 9 || currentStage == 12 || currentStage == 13);
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
                            } else if (missionNumber == 3) {
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
                                    // Stage 9 cleared! Advance to Mission 4: The Judgment of Pilate!
                                    missionNumber = 4;
                                    currentArea = 1;
                                    area1Cleared = false;
                                    area2Cleared = false;
                                    area3Cleared = false;
                                    gameState = GameState::MISSION_MAP;
                                    missionMapTimer = 0.0f;
                                }
                            } else if (missionNumber == 4) {
                                // Mission 4: The Trial and Judgment before Pontius Pilate
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
                                    // Stage 12 cleared! Advance to Mission 5: Golgotha Summit!
                                    missionNumber = 5;
                                    currentArea = 1;
                                    area1Cleared = false;
                                    area2Cleared = false;
                                    area3Cleared = false;
                                    gameState = GameState::MISSION_MAP;
                                    missionMapTimer = 0.0f;
                                }
                            } else {
                                // Mission 5 (Stage 13) Golgotha Summit Victory -> Skee-Ball in Heaven!
                                gameState = GameState::SKEE_BALL_HEAVEN;
                                m5SkeeBallTimer = 0.0f;
                                m5SkeeBallScore = 0;
                                m5SkeeBallTickets = 0;
                                m5SkeeBallRolls = 0;
                                m5RollBallTimer = 0.0f;
                                m5SkeeBallFadeBlack = 0.0f;
                                AudioManager::PlaySkeeBallMusic();
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
                // Select button on controller instantly ends bonus level and returns to main screen
                if (input.coinPressed) {
                    AudioManager::StopAllMusic();
                    AudioManager::PlaySfx(SFX_SHIELD_DEFLECT);
                    ResetGame(true);
                    gameState = GameState::TITLE;
                    break;
                }

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

            case GameState::MERCY_DIALOGUE: {
                m5MercyTimer += dt;
                cloudsPartAmount = std::min(1.0f, cloudsPartAmount + 0.4f * dt);
                divineBeamIntensity = std::min(1.0f, divineBeamIntensity + 0.5f * dt);
                rainbowAlpha = std::min(1.0f, rainbowAlpha + 0.6f * dt);

                if (m5MercyTimer >= 1.2f) {
                    if (input.upPressed || input.downPressed || input.leftPressed || input.rightPressed) {
                        m5MercyChoice = 1 - m5MercyChoice;
                        AudioManager::PlaySfx(SFX_COIN_RICOCHET);
                    }

                    bool confirm = input.startPressed || input.attackPressed || (input.jumpPressed && !input.cancelPressed);
                    if (confirm) {
                        if (m5MercyChoice == 0) {
                            // Jesus agrees: God morphs into Pilate and crucifies him -> Game Over
                            gameState = GameState::CRUCIFIXION_CUTSCENE;
                            m5CrucifixionTimer = 0.0f;
                            AudioManager::PlaySfx(SFX_DEVIL_LAUGH);
                            AudioManager::PlaySfx(SFX_THUNDER);
                        } else {
                            // Jesus disagrees: Refuses false mercy!
                            // Jesus has been hallucinating (stars around his head appear when Pilate morphs and suddenly clear for the fight)
                            gameState = GameState::GAMEPLAY;
                            m5BossStep = 3;
                            m5HallucinationTimer = 2.0f;
                            enemyManager.ClearAllEnemies();
                            enemyManager.SpawnBossDirect(EnemyType::BOSS_PONTIUS_PILATE, Vector2{ 380.0f, GROUND_Y - 32.0f }, 40.0f, 440.0f);
                            Enemy* newPilate = enemyManager.GetBoss();
                            if (newPilate) {
                                newPilate->SetFacingLeft(true);
                                newPilate->StartArenaCombat();
                            }
                            AudioManager::PlaySfx(SFX_CLONE_TELEPORT);
                            AudioManager::PlaySfx(SFX_PILATE_JUDGMENT);
                            AudioManager::PlaySfx(SFX_DING_DING);
                        }
                    }
                }
                break;
            }

            case GameState::CRUCIFIXION_CUTSCENE: {
                m5CrucifixionTimer += dt;
                if (m5CrucifixionTimer >= 6.5f || (m5CrucifixionTimer >= 2.0f && (input.startPressed || input.attackPressed || input.jumpPressed))) {
                    gameState = GameState::GAMEOVER;
                    continueCountdown = 9.9f;
                }
                break;
            }

            case GameState::SKEE_BALL_HEAVEN: {
                m5SkeeBallTimer += dt;
                m5RollBallTimer += dt;

                // Roll a ball into the 100 ring every 1.8s
                if (m5RollBallTimer >= 1.8f) {
                    m5RollBallTimer = 0.0f;
                    m5SkeeBallScore += 100;
                    m5SkeeBallTickets += 10;
                    m5SkeeBallRolls++;
                    AudioManager::PlaySfx(SFX_SKEE_BALL_ROLL);
                    AudioManager::PlaySfx(SFX_SKEE_BALL_SCORE);
                    AudioManager::PlaySfx(SFX_CROWD_CHEER);
                }

                if (m5SkeeBallTimer >= 12.0f || (m5SkeeBallTimer >= 3.0f && (input.startPressed || input.attackPressed || input.jumpPressed))) {
                    m5SkeeBallFadeBlack += dt * 0.9f;
                    if (m5SkeeBallFadeBlack >= 1.0f) {
                        AudioManager::StopAllMusic();
                        ResetGame(true);
                        gameState = GameState::TITLE;
                    }
                }
                break;
            }
        }

        // Audio System Update
        bool inArena = level.IsArenaLockedDown();
        bool bossWithering = (enemyManager.GetBoss() && enemyManager.GetBoss()->IsWithering());
        int curStage = GetCurrentStageId();
        if (!bossWithering && !ascensionStarted && gameState != GameState::SECRET_SELECT && gameState != GameState::MERCY_DIALOGUE && gameState != GameState::CRUCIFIXION_CUTSCENE && gameState != GameState::SKEE_BALL_HEAVEN) {
            AudioManager::Update(inArena, gameState == GameState::GAMEOVER, gameState == GameState::VICTORY, curStage, gameState == GameState::BONUS_STAGE);
        } else if (gameState == GameState::SECRET_SELECT || gameState == GameState::MERCY_DIALOGUE || gameState == GameState::CRUCIFIXION_CUTSCENE) {
            AudioManager::StopAllMusic();
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
        } else if (gameState == GameState::SECRET_SELECT) {
            // Visual Two-Step Secret Mission & Stage Select Showcase
            GraphicsManager::DrawSecretSelectScreen(
                static_cast<int>(secretStep),
                secretMissionIndex,
                secretStageIndex,
                credits,
                static_cast<float>(GetTime())
            );
        } else if (gameState == GameState::MISSION_MAP) {
            // Authentic Shinobi Arcade Jerusalem & Gethsemane Mission Map Screens
            GraphicsManager::DrawMissionMap(missionNumber, currentArea, area1Cleared, area2Cleared, area3Cleared, static_cast<float>(GetTime()));
        } else if (gameState == GameState::BONUS_STAGE) {
            // Sega Shinobi-Style 30-Second Bonus Stage: Feeding the 5,000 on the Mount of Beatitudes
            bonusStage.Draw();
        } else if (gameState == GameState::MERCY_DIALOGUE) {
            // Mount Calvary with rainbow bridge and descending Almighty
            GraphicsManager::DrawParallaxBackground(0.0f, static_cast<float>(GetTime()), 13);
            Mission5Graphics::DrawRainbowBridge(rainbowAlpha, static_cast<float>(GetTime()));

            // Divine beam & parted clouds
            float beamX = 260.0f;
            GraphicsManager::DrawDivineAscensionBeam(beamX, divineBeamIntensity, cloudsPartAmount, static_cast<float>(GetTime()));

            // Draw God descending on rainbow bridge
            float descendProgress = std::min(1.0f, m5MercyTimer / 1.8f);
            float godX = 280.0f - (1.0f - descendProgress) * 40.0f;
            float godY = 55.0f + descendProgress * 65.0f;
            Mission5Graphics::DrawGodDescend(Vector2{ godX, godY }, static_cast<float>(GetTime()));

            // Draw Jesus standing on the summit facing God
            GraphicsManager::DrawDropShadow(120.0f + 14.0f, GROUND_Y - 2.0f, 22.0f, 5.0f);
            GraphicsManager::DrawSprite(SPR_PLAYER_IDLE, 120.0f, GROUND_Y - 32.0f, false);

            // Draw Dialogue & Choice
            if (m5MercyTimer >= 1.2f) {
                Mission5Graphics::DrawMercyDialogue(m5MercyChoice == 0, static_cast<float>(GetTime()));
            }
        } else if (gameState == GameState::CRUCIFIXION_CUTSCENE) {
            float fade = std::min(1.0f, m5CrucifixionTimer / 3.0f);
            Mission5Graphics::DrawCrucifixionScene(static_cast<float>(GetTime()), fade);
        } else if (gameState == GameState::SKEE_BALL_HEAVEN) {
            Mission5Graphics::DrawHeavenSkeeBallScene(
                static_cast<float>(GetTime()),
                m5SkeeBallScore,
                m5RollBallTimer / 1.8f,
                m5SkeeBallRolls,
                (m5RollBallTimer >= 1.6f || m5RollBallTimer < 0.25f),
                m5SkeeBallTickets
            );
            if (m5SkeeBallFadeBlack > 0.0f) {
                DrawRectangle(0, 0, VIRTUAL_WIDTH, VIRTUAL_HEIGHT, ColorAlpha(BLACK, m5SkeeBallFadeBlack));
            }
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

            // Stage 13 Atmospheric Rain
            if (curStage == 13 && m5BossStep != 2) {
                Mission5Graphics::DrawRain(static_cast<float>(GetTime()), 1.0f);
            }

            // Stage 13 Hallucination Stars
            if (curStage == 13 && m5BossStep == 3) {
                float sx = player.GetPosition().x - camera.target.x + 14.0f;
                float sy = player.GetPosition().y + 4.0f;
                float clearProg = 1.0f - std::max(0.0f, m5HallucinationTimer / 2.0f);
                Mission5Graphics::DrawHallucinationStars(Vector2{ sx, sy }, static_cast<float>(GetTime()), clearProg);
            }

            // Stage 13 Angels descending & singing while Jesus ascends
            if (curStage == 13 && ascensionStarted) {
                float sx = player.GetPosition().x - camera.target.x + 14.0f;
                float sy = player.GetPosition().y;
                Mission5Graphics::DrawAscendingAngels(Vector2{ sx, sy }, ascensionSequenceTimer);
            }

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

            // 4. Authentic Sega System 16 Floating Arcade HUD (Transparent over action, drop shadow, yellow timer)
            GraphicsManager::DrawArcadeHud(
                player.GetScore(),
                player.GetLives(),
                player.GetHp(),
                PLAYER_MAX_HP,
                player.GetShatteredHearts(),
                player.HasWrathAvailable(),
                level.GetBasketsCollectedCount(),
                level.GetTotalBasketsCount(),
                curStage,
                stageTime,
                static_cast<float>(GetTime())
            );

            // 5. Boss Health Bar (Top Screen under HUD when inside Arena)
            if (level.IsArenaLockedDown()) {
                Enemy* b = enemyManager.GetBoss();
                if (b && !b->IsWitheredDead()) {
                    int bw = VIRTUAL_WIDTH - 60;
                    int bx = (VIRTUAL_WIDTH - bw) / 2;
                    int by = 22;

                    DrawRectangle(bx, by, bw, 14, Color{ 16, 12, 22, 220 });
                    DrawRectangleLines(bx, by, bw, 14, COLOR_ROMAN_GOLD);
                    DrawRectangleLines(bx + 1, by + 1, bw - 2, 12, Color{ 60, 42, 18, 255 });

                    const char* bossName = "CENTURION BRUTUS";
                    if (curStage == 2) bossName = "COMMANDER MALCHUS";
                    else if (curStage == 3) bossName = "PRAETOR LONGINUS";
                    else if (curStage == 4) bossName = "TORMENTOR DECURION";
                    else if (curStage == 5) bossName = "SHADOW INQUISITOR";
                    else if (curStage == 6) bossName = "CHRISTUS TENEBRIS";
                    else if (curStage == 7) bossName = "BARABBAS THE ZEALOT";
                    else if (curStage == 8) bossName = "HIGH PRIEST CAIAPHAS";
                    else if (curStage == 9) bossName = "JUDAS ISCARIOT";
                    else if (curStage == 10) bossName = "PREFECT QUINTUS";
                    else if (curStage == 11) bossName = "ADVOCATE TERTULLUS";
                    else if (curStage == 12) bossName = "PONTIUS PILATE";
                    else if (curStage == 13) {
                        if (m5BossStep == 0) bossName = "CHRISTUS TENEBRIS";
                        else if (m5BossStep == 1) bossName = "THE DEVIL";
                        else bossName = "PILATE (HALLUCINATION)";
                    }

                    DrawText(bossName, bx + 6, by + 2, 10, COLOR_GOLD_BRIGHT);

                    int maxBossPips = b->GetMaxHp();
                    int curBossPips = b->GetHp();
                    int barStartX = bx + bw - maxBossPips * 9 - 6;

                    for (int p = 0; p < maxBossPips; ++p) {
                        int px = barStartX + p * 9;
                        int py = by + 2;
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
                else if (curStage == 10) msg = "COLLECT ALL 4 IMPERIAL SCROLLS TO UNLOCK BARRICADE!";
                else if (curStage == 11) msg = "COLLECT ALL 4 CAESAR SEALS TO UNLOCK TRIBUNAL GATE!";
                else if (curStage == 12) msg = "COLLECT ALL 4 GOLDEN EWERS TO UNLOCK PRAETORIUM THRONE!";
                else if (curStage == 13) msg = "DEFEAT THE ADVERSARIES UPON GOLGOTHA!";

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
                const char* vicTitle = (missionNumber == 5) ? "GOLGOTHA CONQUERED - ASCENSION ATTAINED!" :
                                       ((missionNumber == 4) ? "THE TRIAL ENDED - PILATE'S JUDGMENT OVERCOME!" :
                                       ((missionNumber == 3) ? "THE LAST SUPPER RESTORED!" : "CAMPAIGN VICTORY!"));
                int vw = MeasureText(vicTitle, 16);
                DrawText(vicTitle, (VIRTUAL_WIDTH - vw) / 2, 40, 16, COLOR_ROMAN_GOLD);

                const char* vicSub1 = (missionNumber == 5) ?
                    "DEATH IS SWALLOWED UP IN VICTORY - THE POWERS OF DARKNESS BROKEN!" :
                    ((missionNumber == 4) ?
                    "PILATE WASHES HIS HANDS IN REMORSE - 'WHAT IS TRUTH?' ANSWERED!" :
                    ((missionNumber == 3) ?
                    "JUDAS REPENTS - THE 30 PIECES OF SILVER SCATTERED" :
                    "GETHSEMANE CONQUERED - THE EVIL CLONE OVERCOME!"));
                DrawText(vicSub1, (VIRTUAL_WIDTH - MeasureText(vicSub1, 10)) / 2, 68, 10, COLOR_ROBE_WHITE);

                const char* vicSub2 = (missionNumber == 5) ?
                    "THE HEAVENS OPEN - CHERUBIM AND SERAPHIM SING TRIUMPHANT HOSANNAS!" :
                    ((missionNumber == 4) ?
                    "THE INNOCENT LAMB ASCENDS TRIUMPHANT OVER EMPIRES AND POWERS" :
                    ((missionNumber == 3) ?
                    "THIS IS MY BODY, GIVEN FOR YOU - DO THIS IN REMEMBRANCE" :
                    "THE NAZARENE'S DIVINE MISSION IS FULFILLED"));
                DrawText(vicSub2, (VIRTUAL_WIDTH - MeasureText(vicSub2, 10)) / 2, 88, 10, COLOR_GOLD_BRIGHT);

                const char* vicSub3 = (missionNumber == 5) ?
                    "I AM THE RESURRECTION AND THE LIFE - TIME FOR SKEE-BALL IN GLORY!" :
                    ((missionNumber == 4) ?
                    "MY KINGDOM IS NOT OF THIS WORLD - GLORY FOREVERMORE!" :
                    ((missionNumber == 3) ?
                    "PEACE UNTO ALL NATIONS - LOVE CONQUERS ALL" : nullptr));
                if (vicSub3) {
                    DrawText(vicSub3, (VIRTUAL_WIDTH - MeasureText(vicSub3, 10)) / 2, 106, 10, COLOR_ROMAN_STONE);
                }

                char vicScore[48];
                int bonusPts = (missionNumber == 5) ? 250000 : ((missionNumber == 4) ? 150000 : ((missionNumber == 3) ? 100000 : 50000));
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

        // 1. Authentic Sega System 16 / MAME Arcade Cabinet Illustrated Bezel on Pillarboxes
        GraphicsManager::DrawArcadeCabinetBezel(
            screenW,
            screenH,
            offsetX,
            offsetY,
            scaledW,
            scaledH,
            crtMode,
            credits,
            static_cast<float>(GetTime())
        );

        // 2. Draw Game Texture with hardware CRT Shader (Executive Order standard)
        Rectangle sourceRec = { 0.0f, 0.0f, (float)targetBuffer.texture.width, -(float)targetBuffer.texture.height };
        Rectangle destRec = { (float)offsetX, (float)offsetY, (float)scaledW, (float)scaledH };

        if (crtMode == 0 && crtShader.id > 0) {
            float curTime = static_cast<float>(GetTime());
            Vector2 resVec = { (float)scaledW, (float)scaledH };
            float sIntensity = 0.32f;
            float cCurv = 0.012f;
            float bBloom = 0.16f;
            if (scanlineLoc >= 0) SetShaderValue(crtShader, scanlineLoc, &sIntensity, SHADER_UNIFORM_FLOAT);
            if (curvatureLoc >= 0) SetShaderValue(crtShader, curvatureLoc, &cCurv, SHADER_UNIFORM_FLOAT);
            if (bloomLoc >= 0) SetShaderValue(crtShader, bloomLoc, &bBloom, SHADER_UNIFORM_FLOAT);
            if (timeLoc >= 0) SetShaderValue(crtShader, timeLoc, &curTime, SHADER_UNIFORM_FLOAT);
            if (resLoc >= 0) SetShaderValue(crtShader, resLoc, &resVec, SHADER_UNIFORM_VEC2);

            BeginShaderMode(crtShader);
            DrawTexturePro(targetBuffer.texture, sourceRec, destRec, { 0.0f, 0.0f }, 0.0f, WHITE);
            EndShaderMode();
        } else if (crtMode == 1 && crtShader.id > 0) {
            float curTime = static_cast<float>(GetTime());
            Vector2 resVec = { (float)scaledW, (float)scaledH };
            float sIntensity = 0.18f;
            float cCurv = 0.0f; // flat screen
            float bBloom = 0.08f;
            if (scanlineLoc >= 0) SetShaderValue(crtShader, scanlineLoc, &sIntensity, SHADER_UNIFORM_FLOAT);
            if (curvatureLoc >= 0) SetShaderValue(crtShader, curvatureLoc, &cCurv, SHADER_UNIFORM_FLOAT);
            if (bloomLoc >= 0) SetShaderValue(crtShader, bloomLoc, &bBloom, SHADER_UNIFORM_FLOAT);
            if (timeLoc >= 0) SetShaderValue(crtShader, timeLoc, &curTime, SHADER_UNIFORM_FLOAT);
            if (resLoc >= 0) SetShaderValue(crtShader, resLoc, &resVec, SHADER_UNIFORM_VEC2);

            BeginShaderMode(crtShader);
            DrawTexturePro(targetBuffer.texture, sourceRec, destRec, { 0.0f, 0.0f }, 0.0f, WHITE);
            EndShaderMode();
        } else {
            // Raw sharp pixels
            DrawTexturePro(targetBuffer.texture, sourceRec, destRec, { 0.0f, 0.0f }, 0.0f, WHITE);
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
    if (crtShader.id > 0) UnloadShader(crtShader);
    UnloadRenderTexture(targetBuffer);
    GraphicsManager::Close();
    AudioManager::Close();
    CloseWindow();

    return 0;
}
