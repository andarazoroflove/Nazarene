#ifndef GBA_AUDIO_H
#define GBA_AUDIO_H

#include "gba_types.h"

typedef enum {
    SFX_NONE = 0,
    SFX_JUMP,
    SFX_NAIL_THROW,
    SFX_MELEE_SWING,
    SFX_ENEMY_HIT,
    SFX_SHIELD_DEFLECT,
    SFX_PLAYER_HURT,
    SFX_BASKET_PICKUP,
    SFX_DIVINE_WRATH,
    SFX_WHIP_CRACK,
    SFX_COIN,
    SFX_FINISH_HIM
} GbaSfx;

typedef enum {
    BGM_NONE = 0,
    BGM_STAGE,
    BGM_BOSS,
    BGM_CONTINUE,
    BGM_VICTORY
} GbaBgm;

void gba_audio_init(void);
void gba_audio_play_sfx(GbaSfx sfx);
void gba_audio_play_bgm(GbaBgm bgm);
void gba_audio_stop_bgm(void);
void gba_audio_update(void);

#endif // GBA_AUDIO_H

