#pragma once

#include "raylib.h"

enum SoundEffect {
    SFX_NAIL_THROW = 0,
    SFX_JUMP,
    SFX_HIGH_JUMP,
    SFX_MELEE_STRIKE,
    SFX_SHIELD_DEFLECT,
    SFX_WHIP_CRACK,
    SFX_WHIP_SLAM,
    SFX_ENEMY_DEFEAT,
    SFX_BASKET_PICKUP,
    SFX_PLAYER_HURT,
    SFX_HOLY_BEAM,
    SFX_LION_ROAR,
    SFX_BOSS_HURT,
    SFX_VICTORY,
    SFX_COIN_INSERT,
    SFX_DING_DING,
    SFX_FINISH_HIM,
    SFX_ANGEL_CHOIR,
    SFX_THUNDER,
    SFX_BREAD_TOSS,
    SFX_FISH_TOSS,
    SFX_CROWD_CHEER,
    SFX_BONUS_WIN,
    SFX_DARK_SPIKE,
    SFX_DARK_WRATH,
    SFX_CLONE_TELEPORT,
    SFX_TORCH_SWING,
    SFX_SILVER_SCATTER,
    SFX_COIN_RICOCHET,
    SFX_TRAITOR_DASH,
    SFX_ROMAN_HORN,
    SFX_WATER_SPLASH,
    SFX_FASCES_STRIKE,
    SFX_PILATE_JUDGMENT,
    SFX_DEVIL_LAUGH,
    SFX_SKEE_BALL_ROLL,
    SFX_SKEE_BALL_SCORE,
    SFX_RAINBOW_CHIME,
    SFX_COUNT
};

class AudioManager {
public:
    static void Init();
    static void Close();
    static void Update(bool inBossArena, bool isGameOver, bool isVictory, int stageId = 1, bool isBonusStage = false);
    static void PlaySfx(SoundEffect sfx);
    static void SetMusicEnabled(bool enabled);
    static bool IsMusicEnabled();
    static void StopAllMusic();
    static void PlaySkeeBallMusic();

private:
    static Sound s_sounds[SFX_COUNT];
    static Sound s_stageMusicM1;
    static Sound s_stageMusicM2;
    static Sound s_stageMusicM3;
    static Sound s_stageMusicM4;
    static Sound s_stageMusicM5;
    static Sound s_skeeBallMusic;
    static Sound s_bonusMusic;
    static Sound s_bossMusic;
    static Sound s_evilCloneMusic;
    static Sound s_judasBossMusic;
    static Sound s_pilateBossMusic;
    static Sound s_continueMusic;
    static bool s_musicEnabled;
    static bool s_initialized;
    static bool s_inBossArena;

    static Wave GenerateSfxWave(SoundEffect sfx);
    static Wave GenerateStageMusicWave();
    static Wave GenerateStage2MusicWave();
    static Wave GenerateStage3MusicWave();
    static Wave GenerateStage4MusicWave();
    static Wave GenerateStage5MusicWave();
    static Wave GenerateSkeeBallMusicWave();
    static Wave GenerateBonusMusicWave();
    static Wave GenerateBossMusicWave();
    static Wave GenerateEvilCloneMusicWave();
    static Wave GenerateJudasBossMusicWave();
    static Wave GeneratePilateBossMusicWave();
    static Wave GenerateContinueMusicWave();
};

