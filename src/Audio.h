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
    SFX_COUNT
};

class AudioManager {
public:
    static void Init();
    static void Close();
    static void Update(bool inBossArena, bool isGameOver, bool isVictory);
    static void PlaySfx(SoundEffect sfx);
    static void SetMusicEnabled(bool enabled);
    static bool IsMusicEnabled();
    static void StopAllMusic();

private:
    static Sound s_sounds[SFX_COUNT];
    static Sound s_stageMusic;
    static Sound s_bossMusic;
    static Sound s_continueMusic;
    static bool s_musicEnabled;
    static bool s_initialized;
    static bool s_inBossArena;

    static Wave GenerateSfxWave(SoundEffect sfx);
    static Wave GenerateStageMusicWave();
    static Wave GenerateBossMusicWave();
    static Wave GenerateContinueMusicWave();
};

