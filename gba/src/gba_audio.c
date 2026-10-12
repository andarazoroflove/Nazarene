#include "gba_audio.h"

static GbaBgm s_current_bgm = BGM_NONE;
static uint16_t s_music_timer = 0;
static uint16_t s_music_step = 0;

// Conversion helper: Hz -> GBA sound frequency register value (rate = 2048 - 131072 / freq)
static inline uint16_t hz_to_rate(uint16_t hz) {
    if (hz == 0) return 0;
    uint32_t r = 2048 - (131072 / hz);
    return (uint16_t)(r > 2047 ? 2047 : r);
}

// Stage melody notes in D Phrygian Dominant (D4, Eb4, F#4, G4, A4, Bb4, C5, D5)
static const uint16_t s_stage_melody[] = {
    294, 294, 311, 370, 392, 370, 311, 294,
    370, 392, 440, 466, 440, 392, 370, 294,
    294, 370, 294, 370, 311, 294, 262, 294,
    392, 392, 370, 311, 294, 311, 370, 294
};

// Stage bassline
static const uint16_t s_stage_bass[] = {
    147, 147, 147, 147, 156, 156, 185, 147,
    147, 147, 147, 147, 156, 156, 185, 147,
    147, 185, 147, 185, 156, 147, 131, 147,
    196, 196, 185, 156, 147, 156, 185, 147
};

// Boss intense battle theme
static const uint16_t s_boss_melody[] = {
    370, 370, 440, 370, 466, 370, 440, 370,
    523, 466, 440, 370, 311, 294, 311, 370,
    370, 392, 440, 466, 523, 466, 440, 392,
    370, 311, 294, 311, 370, 440, 370, 294
};

// Continue jingle (Shinobi Yamaha YM2151 FM synth continue motif)
static const uint16_t s_continue_notes[] = {
    440, 392, 370, 311, 294, 294, 370, 440
};

void gba_audio_init(void) {
    // Turn on sound circuit
    REG_SOUNDCNT_X = 0x0080;
    // Master volume L and R to max (7)
    REG_SOUNDCNT_L = 0x0077;
    // Mix PSG channels 1, 2, 4 into left and right
    REG_SOUNDCNT_H = 0x000F;

    s_current_bgm = BGM_NONE;
    s_music_timer = 0;
    s_music_step = 0;
}

void gba_audio_play_sfx(GbaSfx sfx) {
    switch (sfx) {
        case SFX_JUMP:
            // Channel 1: upward frequency sweep
            REG_SOUND1CNT_L = 0x0012; // Sweep time 1, direction up (0), shift 2
            REG_SOUND1CNT_H = 0xB380; // Volume 11, env down, 50% duty
            REG_SOUND1CNT_X = 0x8000 | hz_to_rate(240); // Initial freq 240Hz, trigger
            break;

        case SFX_NAIL_THROW:
            // Channel 4: crisp noise whip/whoosh
            REG_SOUND4CNT_L = 0x9100; // Volume 9, env down fast
            REG_SOUND4CNT_H = 0x8023; // Noise ratio & clock, trigger
            break;

        case SFX_MELEE_SWING:
            // Channel 1: fast pitch drop swoosh
            REG_SOUND1CNT_L = 0x001B; // Sweep time 1, direction down, shift 3
            REG_SOUND1CNT_H = 0xC280; // Volume 12, env down
            REG_SOUND1CNT_X = 0x8000 | hz_to_rate(520);
            break;

        case SFX_ENEMY_HIT:
            // Channel 1 punch + Channel 4 noise pop
            REG_SOUND1CNT_L = 0x002B;
            REG_SOUND1CNT_H = 0xF240; // 25% duty, Volume 15
            REG_SOUND1CNT_X = 0x8000 | hz_to_rate(160);
            REG_SOUND4CNT_L = 0xA200;
            REG_SOUND4CNT_H = 0x8035;
            break;

        case SFX_SHIELD_DEFLECT:
            // Channel 2: high metallic ping
            REG_SOUND2CNT_L = 0xE180; // Volume 14, quick fade, 50% duty
            REG_SOUND2CNT_H = 0x8000 | hz_to_rate(1320); // High pitch ring
            break;

        case SFX_PLAYER_HURT:
            // Channel 1: heavy hurt buzz
            REG_SOUND1CNT_L = 0x003B;
            REG_SOUND1CNT_H = 0xF440;
            REG_SOUND1CNT_X = 0x8000 | hz_to_rate(110);
            break;

        case SFX_BASKET_PICKUP:
            // Channel 2: bright dual sparkle chime
            REG_SOUND2CNT_L = 0xD280;
            REG_SOUND2CNT_H = 0x8000 | hz_to_rate(880);
            break;

        case SFX_DIVINE_WRATH:
            // Channel 4 thunder boom + Channel 1 power chord
            REG_SOUND4CNT_L = 0xF700; // Max volume, long rumble
            REG_SOUND4CNT_H = 0x8054;
            REG_SOUND1CNT_L = 0x0000;
            REG_SOUND1CNT_H = 0xF580;
            REG_SOUND1CNT_X = 0x8000 | hz_to_rate(147);
            break;

        case SFX_WHIP_CRACK:
            // Channel 4 sharp leather crack
            REG_SOUND4CNT_L = 0xF100;
            REG_SOUND4CNT_H = 0x8011;
            break;

        case SFX_COIN:
            // High double ding
            REG_SOUND2CNT_L = 0xD280;
            REG_SOUND2CNT_H = 0x8000 | hz_to_rate(988); // B5
            break;

        case SFX_FINISH_HIM:
            // Deep resonant gong
            REG_SOUND1CNT_L = 0x002B;
            REG_SOUND1CNT_H = 0xF680;
            REG_SOUND1CNT_X = 0x8000 | hz_to_rate(73); // D2
            REG_SOUND4CNT_L = 0xB500;
            REG_SOUND4CNT_H = 0x8055;
            break;

        default:
            break;
    }
}

void gba_audio_play_bgm(GbaBgm bgm) {
    if (s_current_bgm == bgm) return;
    s_current_bgm = bgm;
    s_music_timer = 0;
    s_music_step = 0;
}

void gba_audio_stop_bgm(void) {
    s_current_bgm = BGM_NONE;
    REG_SOUND1CNT_H = 0;
    REG_SOUND2CNT_L = 0;
}

void gba_audio_update(void) {
    if (s_current_bgm == BGM_NONE) return;

    s_music_timer++;
    uint16_t tempo = (s_current_bgm == BGM_BOSS) ? 7 : 10;

    if (s_music_timer >= tempo) {
        s_music_timer = 0;

        if (s_current_bgm == BGM_STAGE) {
            uint16_t melNote = s_stage_melody[s_music_step % 32];
            uint16_t bassNote = s_stage_bass[s_music_step % 32];

            // Melody on Channel 1
            REG_SOUND1CNT_L = 0;
            REG_SOUND1CNT_H = 0x8280; // 50% duty, medium volume
            REG_SOUND1CNT_X = 0x8000 | hz_to_rate(melNote);

            // Bass on Channel 2
            REG_SOUND2CNT_L = 0x6280;
            REG_SOUND2CNT_H = 0x8000 | hz_to_rate(bassNote);

            // Percussion snare on beats 2 and 4
            if ((s_music_step % 4) == 2) {
                REG_SOUND4CNT_L = 0x5100;
                REG_SOUND4CNT_H = 0x8032;
            } else if ((s_music_step % 2) == 0) {
                // Hi-hat
                REG_SOUND4CNT_L = 0x3100;
                REG_SOUND4CNT_H = 0x8011;
            }

            s_music_step++;
        } else if (s_current_bgm == BGM_BOSS) {
            uint16_t melNote = s_boss_melody[s_music_step % 32];

            REG_SOUND1CNT_L = 0;
            REG_SOUND1CNT_H = 0x9280;
            REG_SOUND1CNT_X = 0x8000 | hz_to_rate(melNote);

            REG_SOUND2CNT_L = 0x7240;
            REG_SOUND2CNT_H = 0x8000 | hz_to_rate(147); // Heavy D pedal

            // Heavy driving snare
            if ((s_music_step % 2) == 1) {
                REG_SOUND4CNT_L = 0x6100;
                REG_SOUND4CNT_H = 0x8033;
            }

            s_music_step++;
        } else if (s_current_bgm == BGM_CONTINUE) {
            uint16_t note = s_continue_notes[s_music_step % 8];
            REG_SOUND1CNT_L = 0;
            REG_SOUND1CNT_H = 0x9380;
            REG_SOUND1CNT_X = 0x8000 | hz_to_rate(note);
            s_music_step++;
        } else if (s_current_bgm == BGM_VICTORY) {
            // Majestic ascending choral notes
            uint16_t vNote = 294 + (s_music_step % 16) * 24;
            REG_SOUND1CNT_L = 0;
            REG_SOUND1CNT_H = 0x8480;
            REG_SOUND1CNT_X = 0x8000 | hz_to_rate(vNote);
            s_music_step++;
        }
    }
}
