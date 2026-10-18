#include "Audio.h"
#include <cmath>
#include <cstdlib>
#include <vector>

Sound AudioManager::s_sounds[SFX_COUNT] = {};
Sound AudioManager::s_stageMusicM1 = {};
Sound AudioManager::s_stageMusicM2 = {};
Sound AudioManager::s_stageMusicM3 = {};
Sound AudioManager::s_stageMusicM4 = {};
Sound AudioManager::s_stageMusicM5 = {};
Sound AudioManager::s_skeeBallMusic = {};
Sound AudioManager::s_bonusMusic = {};
Sound AudioManager::s_bossMusic = {};
Sound AudioManager::s_evilCloneMusic = {};
Sound AudioManager::s_judasBossMusic = {};
Sound AudioManager::s_pilateBossMusic = {};
Sound AudioManager::s_continueMusic = {};
bool AudioManager::s_musicEnabled = true;
bool AudioManager::s_initialized = false;
bool AudioManager::s_inBossArena = false;

static inline float ClampFloat(float val, float minVal, float maxVal) {
    if (val < minVal) return minVal;
    if (val > maxVal) return maxVal;
    return val;
}

// Generate sound effects procedurally
Wave AudioManager::GenerateSfxWave(SoundEffect sfx) {
    constexpr unsigned int sampleRate = 44100;
    float duration = 0.2f;

    switch (sfx) {
        case SFX_NAIL_THROW: duration = 0.12f; break;
        case SFX_JUMP: duration = 0.15f; break;
        case SFX_HIGH_JUMP: duration = 0.28f; break;
        case SFX_MELEE_STRIKE: duration = 0.18f; break;
        case SFX_SHIELD_DEFLECT: duration = 0.22f; break;
        case SFX_WHIP_CRACK: duration = 0.16f; break;
        case SFX_WHIP_SLAM: duration = 0.35f; break;
        case SFX_ENEMY_DEFEAT: duration = 0.26f; break;
        case SFX_BASKET_PICKUP: duration = 0.36f; break;
        case SFX_PLAYER_HURT: duration = 0.25f; break;
        case SFX_HOLY_BEAM: duration = 0.85f; break;
        case SFX_LION_ROAR: duration = 0.95f; break;
        case SFX_BOSS_HURT: duration = 0.28f; break;
        case SFX_VICTORY: duration = 1.4f; break;
        case SFX_COIN_INSERT: duration = 0.32f; break;
        case SFX_DING_DING: duration = 0.55f; break;
        case SFX_FINISH_HIM: duration = 1.85f; break;
        case SFX_ANGEL_CHOIR: duration = 4.80f; break;
        case SFX_THUNDER: duration = 1.40f; break;
        case SFX_BREAD_TOSS: duration = 0.16f; break;
        case SFX_FISH_TOSS: duration = 0.18f; break;
        case SFX_CROWD_CHEER: duration = 0.85f; break;
        case SFX_BONUS_WIN: duration = 2.40f; break;
        case SFX_DARK_SPIKE: duration = 0.22f; break;
        case SFX_DARK_WRATH: duration = 1.60f; break;
        case SFX_CLONE_TELEPORT: duration = 0.32f; break;
        case SFX_TORCH_SWING: duration = 0.24f; break;
        case SFX_SILVER_SCATTER: duration = 0.45f; break;
        case SFX_COIN_RICOCHET: duration = 0.22f; break;
        case SFX_TRAITOR_DASH: duration = 0.28f; break;
        case SFX_ROMAN_HORN: duration = 0.70f; break;
        case SFX_WATER_SPLASH: duration = 0.35f; break;
        case SFX_FASCES_STRIKE: duration = 0.30f; break;
        case SFX_PILATE_JUDGMENT: duration = 0.85f; break;
        case SFX_DEVIL_LAUGH: duration = 1.35f; break;
        case SFX_SKEE_BALL_ROLL: duration = 0.50f; break;
        case SFX_SKEE_BALL_SCORE: duration = 0.45f; break;
        case SFX_RAINBOW_CHIME: duration = 1.15f; break;
        default: duration = 0.2f; break;
    }

    unsigned int frameCount = static_cast<unsigned int>(sampleRate * duration);
    short* buffer = (short*)malloc(frameCount * sizeof(short));

    constexpr float PI2 = 6.28318530718f;

    for (unsigned int i = 0; i < frameCount; ++i) {
        float t = static_cast<float>(i) / sampleRate;
        float progress = static_cast<float>(i) / frameCount;
        float sample = 0.0f;

        switch (sfx) {
            case SFX_NAIL_THROW: {
                // High velocity whoosh + metallic ping
                float env = 1.0f - progress;
                float freq = 1300.0f - 900.0f * progress;
                float tone = sinf(PI2 * freq * t);
                float noise = ((float)rand() / (float)RAND_MAX * 2.0f - 1.0f) * 0.4f;
                sample = (tone * 0.6f + noise) * env;
                break;
            }
            case SFX_JUMP: {
                // Rising chirp
                float env = sinf(progress * 3.14159f);
                float freq = 200.0f + 500.0f * (progress * progress);
                sample = sinf(PI2 * freq * t) * env;
                break;
            }
            case SFX_HIGH_JUMP: {
                // Heroic ascending high somersault leap
                float env = 1.0f - progress * 0.8f;
                float freq = 180.0f + 750.0f * progress;
                float tone1 = sinf(PI2 * freq * t);
                float tone2 = sinf(PI2 * (freq * 1.5f) * t) * 0.35f;
                sample = (tone1 + tone2) * env;
                break;
            }
            case SFX_MELEE_STRIKE: {
                // Heavy punch impact + slap
                float env = expf(-progress * 12.0f);
                float punchFreq = 160.0f - 110.0f * progress;
                float punch = sinf(PI2 * punchFreq * t);
                float crunch = ((float)rand() / (float)RAND_MAX * 2.0f - 1.0f) * expf(-progress * 25.0f);
                sample = (punch * 0.7f + crunch * 0.6f) * env;
                break;
            }
            case SFX_SHIELD_DEFLECT: {
                // Metallic ring
                float env = expf(-progress * 16.0f);
                float tone1 = sinf(PI2 * 2700.0f * t);
                float tone2 = sinf(PI2 * 3950.0f * t) * 0.6f;
                sample = (tone1 + tone2) * env;
                break;
            }
            case SFX_WHIP_CRACK: {
                // Sharp snap noise + hiss
                float crackEnv = expf(-progress * 30.0f);
                float noise = ((float)rand() / (float)RAND_MAX * 2.0f - 1.0f);
                float whipHiss = sinf(PI2 * 900.0f * t) * expf(-progress * 15.0f);
                sample = noise * crackEnv + whipHiss * 0.4f;
                break;
            }
            case SFX_WHIP_SLAM: {
                // Heavy bass slam + whip snap
                float boomEnv = expf(-progress * 8.0f);
                float boom = sinf(PI2 * (80.0f - 40.0f * progress) * t) * boomEnv;
                float crack = ((float)rand() / (float)RAND_MAX * 2.0f - 1.0f) * expf(-progress * 40.0f);
                sample = boom * 0.8f + crack * 0.5f;
                break;
            }
            case SFX_ENEMY_DEFEAT: {
                // Arcade explosion pop
                float env = expf(-progress * 10.0f);
                float noise = ((float)rand() / (float)RAND_MAX * 2.0f - 1.0f);
                float tone = sinf(PI2 * (280.0f - 200.0f * progress) * t);
                sample = (noise * 0.7f + tone * 0.4f) * env;
                break;
            }
            case SFX_BASKET_PICKUP: {
                // Upward major arpeggio chime (C6, E6, G6, C7)
                float noteDuration = duration / 4.0f;
                int noteIndex = static_cast<int>(t / noteDuration);
                if (noteIndex > 3) noteIndex = 3;
                float freqs[4] = { 1046.50f, 1318.51f, 1567.98f, 2093.00f };
                float noteT = fmodf(t, noteDuration);
                float env = 1.0f - (noteT / noteDuration);
                sample = sinf(PI2 * freqs[noteIndex] * t) * env;
                break;
            }
            case SFX_PLAYER_HURT: {
                // Distortion buzz
                float env = expf(-progress * 8.0f);
                float tone = sinf(PI2 * 110.0f * t);
                if (tone > 0.4f) tone = 0.4f;
                if (tone < -0.4f) tone = -0.4f;
                sample = tone * 2.0f * env;
                break;
            }
            case SFX_HOLY_BEAM: {
                // Celestial chord: D4, F#4, A4, D5 with chorus
                float env = sinf(progress * 3.14159f);
                float ch1 = sinf(PI2 * 587.33f * t);
                float ch2 = sinf(PI2 * 739.99f * t);
                float ch3 = sinf(PI2 * 880.00f * t);
                float ch4 = sinf(PI2 * 1174.66f * t);
                float shimmer = 0.5f + 0.5f * sinf(PI2 * 8.0f * t);
                sample = (ch1 + ch2 + ch3 + ch4) * 0.25f * env * shimmer;
                break;
            }
            case SFX_LION_ROAR: {
                // Modulated low frequency roar
                float env = sinf(progress * 3.14159f);
                float mod = sinf(PI2 * 28.0f * t) * 40.0f;
                float baseFreq = 95.0f - 30.0f * progress + mod;
                float growl = sinf(PI2 * baseFreq * t);
                float breath = ((float)rand() / (float)RAND_MAX * 2.0f - 1.0f) * 0.4f;
                sample = (growl * 0.7f + breath) * env;
                break;
            }
            case SFX_BOSS_HURT: {
                // Deep metallic grunt
                float env = expf(-progress * 11.0f);
                float tone = sinf(PI2 * 130.0f * t);
                float ring = sinf(PI2 * 1450.0f * t) * 0.4f;
                sample = (tone + ring) * env;
                break;
            }
            case SFX_VICTORY: {
                // Multi-note triumphant fanfare
                float noteDuration = duration / 7.0f;
                int noteIndex = static_cast<int>(t / noteDuration);
                if (noteIndex > 6) noteIndex = 6;
                // G4, C5, E5, G5, E5, G5, C6
                float freqs[7] = { 392.00f, 523.25f, 659.25f, 783.99f, 659.25f, 783.99f, 1046.50f };
                float noteT = fmodf(t, noteDuration);
                float env = 1.0f - (noteT / noteDuration) * 0.7f;
                float tone = sinf(PI2 * freqs[noteIndex] * t);
                float harm = sinf(PI2 * (freqs[noteIndex] * 2.0f) * t) * 0.25f;
                sample = (tone + harm) * env * 0.8f;
                break;
            }
            case SFX_COIN_INSERT: {
                // Authentic dual-chime arcade coin drop (Bb6 followed by Eb7)
                float tSplit = 0.08f;
                float env = (t < tSplit) ? expf(-t * 22.0f) : expf(-(t - tSplit) * 14.0f);
                float freq = (t < tSplit) ? 1864.66f : 2489.02f;
                float tone = sinf(PI2 * freq * t);
                float bell = sinf(PI2 * (freq * 2.04f) * t) * 0.35f;
                sample = (tone + bell) * env * 0.85f;
                break;
            }
            case SFX_DING_DING: {
                // Two bright, happy arcade bell chimes ("Ding-Ding!")
                // First ding: 0.0s to 0.25s (E6: 1318.5 Hz + harmonic 2637 Hz)
                // Second ding: 0.25s to 0.55s (A6: 1760.0 Hz + harmonic 3520 Hz)
                if (t < 0.25f) {
                    float dingT = t;
                    float env = expf(-dingT * 12.0f);
                    float tone1 = sinf(PI2 * 1318.51f * dingT);
                    float tone2 = sinf(PI2 * 2637.02f * dingT) * 0.4f;
                    sample = (tone1 + tone2) * env * 0.85f;
                } else {
                    float dingT = t - 0.25f;
                    float env = expf(-dingT * 10.0f);
                    float tone1 = sinf(PI2 * 1760.00f * dingT);
                    float tone2 = sinf(PI2 * 3520.00f * dingT) * 0.45f;
                    sample = (tone1 + tone2) * env * 0.90f;
                }
                break;
            }
            case SFX_FINISH_HIM: {
                // Speech synthesis of deep booming voice saying "FINISH HIM" with resonant echo delay
                auto VoiceSample = [sampleRate, PI2](float vt) -> float {
                    if (vt < 0.0f || vt >= 1.25f) return 0.0f;
                    float s = 0.0f;
                    // Syllable 1: "FI" (0.0s to 0.28s)
                    if (vt < 0.28f) {
                        float pt = vt / 0.28f;
                        float fNoise = ((float)rand() / (float)RAND_MAX * 2.0f - 1.0f) * (1.0f - pt * 0.8f);
                        float pitch = 88.0f - 8.0f * pt;
                        float glottal = (fmodf(vt * pitch, 1.0f) < 0.35f ? 1.0f : -0.3f);
                        // Formants for "ih" (F1 ~ 340 Hz, F2 ~ 2100 Hz)
                        float formant1 = sinf(PI2 * 340.0f * vt) * 0.6f;
                        float formant2 = sinf(PI2 * 2100.0f * vt) * 0.35f;
                        float env = sinf(pt * 3.14159f);
                        s = (glottal * (formant1 + formant2) + fNoise * 0.35f) * env;
                    }
                    // Syllable 2: "NISH" (0.28s to 0.68s)
                    else if (vt < 0.68f) {
                        float st = (vt - 0.28f) / 0.40f;
                        float pitch = 84.0f - 6.0f * st;
                        float glottal = (fmodf(vt * pitch, 1.0f) < 0.35f ? 1.0f : -0.3f);
                        float formant1 = sinf(PI2 * 360.0f * vt) * 0.5f;
                        float formant2 = sinf(PI2 * 1800.0f * vt) * 0.35f;
                        // Sibilant "SH" noise at end of syllable
                        float shNoise = ((float)rand() / (float)RAND_MAX * 2.0f - 1.0f);
                        float shBand = sinf(PI2 * 3600.0f * vt) * shNoise;
                        float env = (st < 0.7f) ? sinf(st / 0.7f * 3.14159f * 0.5f) : (1.0f - (st - 0.7f) / 0.3f);
                        float vowelPart = glottal * (formant1 + formant2);
                        float sibilantPart = shBand * (st > 0.5f ? (st - 0.5f) * 3.0f : 0.0f);
                        s = (vowelPart * 0.7f + sibilantPart * 0.6f) * env;
                    }
                    // Syllable 3: "HIM" (0.74s to 1.20s)
                    else if (vt >= 0.74f && vt < 1.20f) {
                        float ht = (vt - 0.74f) / 0.46f;
                        float pitch = 80.0f - 12.0f * ht; // Deep pitch drop
                        float glottal = (fmodf(vt * pitch, 1.0f) < 0.3f ? 1.0f : -0.35f);
                        float formant1 = sinf(PI2 * 300.0f * vt) * 0.6f;
                        float formant2 = sinf(PI2 * 1900.0f * vt) * 0.3f;
                        float nasalM = sinf(PI2 * 180.0f * vt) * 0.4f;
                        float env = (ht < 0.2f) ? (ht / 0.2f) : (1.0f - (ht - 0.2f) / 0.8f);
                        s = glottal * (formant1 + formant2 + (ht > 0.5f ? nasalM : 0.0f)) * env;
                    }
                    return s;
                };

                // Base direct voice + 3 deep echoes
                float dry = VoiceSample(t);
                float echo1 = VoiceSample(t - 0.22f) * 0.48f;
                float echo2 = VoiceSample(t - 0.44f) * 0.28f;
                float echo3 = VoiceSample(t - 0.66f) * 0.15f;
                sample = (dry * 0.85f + echo1 + echo2 + echo3) * 1.15f;
                break;
            }
            case SFX_ANGEL_CHOIR: {
                // Heavenly polyphonic choir singing sacred hymn with organ chords & chimes
                float chordRoots[4] = { 261.63f, 349.23f, 392.00f, 523.25f }; // C4, F4, G4, C5
                float chordThirds[4]= { 329.63f, 440.00f, 493.88f, 659.25f }; // E4, A4, B4, E5
                float chordFifths[4]= { 392.00f, 523.25f, 587.33f, 783.99f }; // G4, C5, D5, G5
                int chordIdx = std::min(3, static_cast<int>(t / 1.2f));

                float fRoot = chordRoots[chordIdx];
                float fThird = chordThirds[chordIdx];
                float fFifth = chordFifths[chordIdx];

                // Celestial vocal formant & vibrato (5.5 Hz)
                float vib = 1.0f + 0.006f * sinf(PI2 * 5.5f * t);
                float env = sinf(progress * 3.14159f); // Smooth swell and fade

                // Polyphonic vocal harmonics
                float vRoot  = sinf(PI2 * fRoot  * vib * t) + 0.4f * sinf(PI2 * fRoot  * 2.0f * vib * t);
                float vThird = sinf(PI2 * fThird * vib * t) + 0.35f* sinf(PI2 * fThird * 2.0f * vib * t);
                float vFifth = sinf(PI2 * fFifth * vib * t) + 0.35f* sinf(PI2 * fFifth * 2.0f * vib * t);
                float vOct   = sinf(PI2 * fRoot  * 2.0f * vib * t) * 0.45f;
                // High celestial formant ("Ahhh" cathedral singer formant ~2800 Hz)
                float vFormant = sinf(PI2 * 2780.0f * t) * 0.12f * (sinf(PI2 * fRoot * t) > 0.0f ? 1.0f : 0.0f);

                // High glistening harp / bell chimes
                float chimeTime = fmodf(t, 0.4f);
                float chimeFreq = 1046.5f + (static_cast<int>(t * 5.0f) % 5) * 220.0f;
                float chime = sinf(PI2 * chimeFreq * chimeTime) * expf(-chimeTime * 9.0f) * 0.25f;

                sample = ((vRoot + vThird + vFifth + vOct) * 0.22f + vFormant + chime) * env * 1.15f;
                break;
            }
            case SFX_THUNDER: {
                // Low rolling thunder rumble with lightning crackle
                float env = expf(-t * 1.8f);
                float rumble = sinf(PI2 * (55.0f + 15.0f * sinf(t * 8.0f)) * t) * 0.6f;
                float sub = sinf(PI2 * 38.0f * t) * 0.4f;
                float noise = ((float)rand() / (float)RAND_MAX * 2.0f - 1.0f) * 0.35f;
                if (t < 0.12f) noise *= 2.0f; // Initial lightning crack
                sample = (rumble + sub + noise) * env * 0.95f;
                break;
            }
            case SFX_BREAD_TOSS: {
                // Soft airy swoosh with warm impact
                float env = 1.0f - progress;
                float freq = 360.0f - 180.0f * progress;
                float tone = sinf(PI2 * freq * t);
                float whoosh = ((float)rand() / (float)RAND_MAX * 2.0f - 1.0f) * 0.35f * expf(-progress * 12.0f);
                sample = (tone * 0.6f + whoosh) * env;
                break;
            }
            case SFX_FISH_TOSS: {
                // Aquatic shimmer & zip
                float env = expf(-progress * 8.0f);
                float freq = 750.0f + 350.0f * sinf(progress * 18.0f);
                float tone = sinf(PI2 * freq * t);
                float splash = ((float)rand() / (float)RAND_MAX * 2.0f - 1.0f) * 0.25f * env;
                sample = (tone * 0.7f + splash);
                break;
            }
            case SFX_CROWD_CHEER: {
                // Joyful celestial chime + chorus harmonic
                float env = expf(-progress * 4.5f);
                float c5 = sinf(PI2 * 523.25f * t);
                float e5 = sinf(PI2 * 659.25f * t) * 0.7f;
                float g5 = sinf(PI2 * 783.99f * t) * 0.6f;
                float c6 = sinf(PI2 * 1046.50f * t) * 0.4f;
                sample = (c5 + e5 + g5 + c6) * 0.35f * env;
                break;
            }
            case SFX_BONUS_WIN: {
                // Triumphant ascending arcade fanfare
                int step = static_cast<int>(progress * 5.0f);
                float notes[5] = { 523.25f, 659.25f, 783.99f, 1046.50f, 1318.51f };
                float f = notes[step < 5 ? step : 4];
                float tone = sinf(PI2 * f * t) * 0.6f + sinf(PI2 * (f * 2.0f) * t) * 0.2f;
                float env = (1.0f - fmodf(progress * 5.0f, 1.0f) * 0.4f) * (1.0f - progress * 0.3f);
                sample = tone * env;
                break;
            }
            case SFX_DARK_SPIKE: {
                // Cursed iron spike with dark resonance
                float env = expf(-progress * 14.0f);
                float freq = 820.0f - 300.0f * progress;
                float tone = sinf(PI2 * freq * t) + sinf(PI2 * (freq * 0.73f) * t) * 0.5f;
                sample = tone * env * 0.8f;
                break;
            }
            case SFX_DARK_WRATH: {
                // Low sub-bass earthquake rumble + dark detonation
                float env = expf(-t * 2.2f);
                float rumble = sinf(PI2 * (42.0f + 18.0f * sinf(t * 12.0f)) * t) * 0.75f;
                float sub = sinf(PI2 * 30.0f * t) * 0.5f;
                float dist = ((float)rand() / (float)RAND_MAX * 2.0f - 1.0f) * 0.45f;
                sample = (rumble + sub + dist) * env;
                break;
            }
            case SFX_CLONE_TELEPORT: {
                // Phaser sweep down with sinister edge
                float env = 1.0f - progress * 0.7f;
                float sweepFreq = 1600.0f - 1200.0f * (progress * progress);
                float tone = sinf(PI2 * sweepFreq * t + sinf(PI2 * 35.0f * t) * 2.5f);
                sample = tone * env * 0.6f;
                break;
            }
            case SFX_TORCH_SWING: {
                // Fiery whoosh with crackling embers
                float env = sinf(progress * 3.14159f);
                float whoosh = sinf(PI2 * (220.0f - 80.0f * progress) * t) * 0.5f;
                float embers = ((float)rand() / (float)RAND_MAX * 2.0f - 1.0f) * 0.45f * env;
                sample = (whoosh + embers) * env;
                break;
            }
            case SFX_SILVER_SCATTER: {
                // Multi-coin metallic ping cascade with silver resonance (30 pieces of silver)
                float s = 0.0f;
                float baseFreqs[5] = { 2100.0f, 2600.0f, 3150.0f, 3700.0f, 4400.0f };
                for (int c = 0; c < 5; ++c) {
                    float coinStart = c * 0.06f;
                    if (t >= coinStart) {
                        float ct = t - coinStart;
                        float cEnv = expf(-ct * 28.0f);
                        float f = baseFreqs[c] + 120.0f * (c % 2 == 0 ? 1.0f : -1.0f);
                        float ping = sinf(PI2 * f * ct) + 0.35f * sinf(PI2 * (f * 2.1f) * ct);
                        s += ping * cEnv * 0.3f;
                    }
                }
                sample = s;
                break;
            }
            case SFX_COIN_RICOCHET: {
                // High-frequency metallic ping bouncing off stone floor
                float env = expf(-progress * 22.0f);
                float freq = 3400.0f + 600.0f * expf(-progress * 30.0f);
                float tone1 = sinf(PI2 * freq * t);
                float tone2 = sinf(PI2 * (freq * 1.62f) * t) * 0.4f;
                sample = (tone1 + tone2) * env * 0.75f;
                break;
            }
            case SFX_TRAITOR_DASH: {
                // Rapid dark shadow whoosh with phasing resonance
                float env = sinf(progress * 3.14159f);
                float freq = 450.0f - 220.0f * progress;
                float phaseMod = sinf(PI2 * 28.0f * t) * 1.5f;
                float whoosh = sinf(PI2 * freq * t + phaseMod) * 0.5f;
                float wind = ((float)rand() / (float)RAND_MAX * 2.0f - 1.0f) * 0.35f * env;
                sample = (whoosh + wind) * env;
                break;
            }
            case SFX_ROMAN_HORN: {
                // Imperial Roman Cornu / Tuba brass fanfare blast
                float env = (progress < 0.12f) ? (progress / 0.12f) : (1.0f - (progress - 0.12f) / 0.88f);
                float f1 = 220.0f; // A3
                float f2 = 330.0f; // E4 (perfect fifth)
                float f3 = 440.0f; // A4 (octave)
                float f0 = 110.0f; // A2 (deep drone)
                float vib = 1.0f + 0.008f * sinf(PI2 * 5.0f * t);
                float tone1 = sinf(PI2 * f1 * vib * t);
                float tone2 = sinf(PI2 * f2 * vib * t) * 0.7f;
                float tone3 = sinf(PI2 * f3 * vib * t) * 0.4f;
                float drone = sinf(PI2 * f0 * t) * 0.5f;
                float brassBuzz = (fmodf(t * f1, 1.0f) - 0.5f) * 0.3f;
                sample = (tone1 + tone2 + tone3 + drone + brassBuzz) * env * 0.8f;
                break;
            }
            case SFX_WATER_SPLASH: {
                // Caustic water splash / Pilate basin washing
                float env = expf(-progress * 9.0f);
                float noise = ((float)rand() / (float)RAND_MAX * 2.0f - 1.0f);
                float drop1 = sinf(PI2 * (850.0f - 400.0f * progress) * t) * 0.4f;
                float drop2 = sinf(PI2 * (600.0f - 300.0f * progress) * t) * 0.3f;
                float bubbleMod = 1.0f + 0.4f * sinf(PI2 * 35.0f * t);
                sample = (noise * 0.45f + drop1 + drop2) * bubbleMod * env * 0.9f;
                break;
            }
            case SFX_FASCES_STRIKE: {
                // Heavy bundle of wooden lictor rods strike
                float env = expf(-progress * 18.0f);
                float impact = sinf(PI2 * (150.0f - 70.0f * progress) * t) * 0.7f;
                float woodClick = sinf(PI2 * 780.0f * t) * 0.5f * expf(-progress * 35.0f);
                float rattle = ((float)rand() / (float)RAND_MAX * 2.0f - 1.0f) * 0.35f * expf(-progress * 22.0f);
                sample = (impact + woodClick + rattle) * env;
                break;
            }
            case SFX_PILATE_JUDGMENT: {
                // Piercing golden stylus decree beam / imperial condemnation
                float env = (progress < 0.08f) ? (progress / 0.08f) : expf(-(progress - 0.08f) * 4.5f);
                float sweepFreq = 1600.0f - 900.0f * (progress * progress);
                float beam = sinf(PI2 * sweepFreq * t);
                float subThrum = sinf(PI2 * 65.0f * t) * 0.6f;
                float holyRing = sinf(PI2 * 2100.0f * t) * 0.3f * expf(-progress * 8.0f);
                float buzz = ((float)rand() / (float)RAND_MAX * 2.0f - 1.0f) * 0.2f * env;
                sample = (beam * 0.55f + subThrum + holyRing + buzz) * env * 0.95f;
                break;
            }
            case SFX_DEVIL_LAUGH: {
                // Sinister guttural demon cackle with cavernous sub-bass echo
                float laughPhase = fmodf(t, 0.32f) / 0.32f;
                int laughIdx = static_cast<int>(t / 0.32f);
                if (laughIdx < 4) {
                    float basePitch = 95.0f - laughIdx * 10.0f;
                    float pitchDrop = basePitch * (1.0f - laughPhase * 0.4f);
                    float glottal = (fmodf(laughPhase * 0.32f * pitchDrop, 1.0f) < 0.4f ? 1.0f : -0.3f);
                    float formant1 = sinf(PI2 * 320.0f * t) * 0.6f;
                    float formant2 = sinf(PI2 * 1150.0f * t) * 0.4f;
                    float subBass = sinf(PI2 * (pitchDrop * 0.5f) * t) * 0.5f;
                    float growl = ((float)rand() / (float)RAND_MAX * 2.0f - 1.0f) * 0.3f;
                    float env = sinf(laughPhase * 3.14159f) * (1.0f - progress * 0.4f);
                    sample = (glottal * (formant1 + formant2) + subBass + growl) * env * 1.1f;
                }
                break;
            }
            case SFX_SKEE_BALL_ROLL: {
                // Heavy wooden ball rumbling across smooth wooden arcade lane
                float env = sinf(progress * 3.14159f);
                float rumble = sinf(PI2 * (90.0f + 25.0f * sinf(PI2 * 14.0f * t)) * t) * 0.55f;
                float friction = ((float)rand() / (float)RAND_MAX * 2.0f - 1.0f) * 0.35f;
                float seam = (fmodf(t, 0.11f) < 0.015f) ? sinf(PI2 * 480.0f * t) * 0.45f : 0.0f;
                sample = (rumble + friction + seam) * env * 0.85f;
                break;
            }
            case SFX_SKEE_BALL_SCORE: {
                // Classic arcade carnival target bell ring (100 pt ding!)
                float env = expf(-progress * 9.0f);
                float tone1 = sinf(PI2 * 1760.0f * t); // A6
                float tone2 = sinf(PI2 * 2637.0f * t) * 0.6f; // E7
                float tone3 = sinf(PI2 * 3520.0f * t) * 0.35f; // A7
                float chime = sinf(PI2 * 5274.0f * t) * 0.2f * expf(-progress * 20.0f);
                sample = (tone1 + tone2 + tone3 + chime) * env * 0.85f;
                break;
            }
            case SFX_RAINBOW_CHIME: {
                // Shimmering celestial rainbow harp arpeggio
                float notes[7] = { 523.25f, 659.25f, 783.99f, 987.77f, 1046.50f, 1318.51f, 1567.98f };
                int noteIdx = static_cast<int>(progress * 7.0f);
                if (noteIdx > 6) noteIdx = 6;
                float f = notes[noteIdx];
                float noteProgress = fmodf(progress * 7.0f, 1.0f);
                float env = expf(-noteProgress * 5.0f) * (1.0f - progress * 0.2f);
                float vib = 1.0f + 0.01f * sinf(PI2 * 6.0f * t);
                float bell = sinf(PI2 * f * vib * t) + 0.4f * sinf(PI2 * f * 2.0f * vib * t);
                float shimmer = sinf(PI2 * (f * 3.0f) * t) * 0.2f;
                sample = (bell + shimmer) * env * 0.9f;
                break;
            }
            default:
                sample = 0.0f;
                break;
        }

        sample = ClampFloat(sample, -1.0f, 1.0f);
        buffer[i] = static_cast<short>(sample * 30000.0f);
    }

    Wave wave = {};
    wave.frameCount = frameCount;
    wave.sampleRate = sampleRate;
    wave.sampleSize = 16;
    wave.channels = 1;
    wave.data = buffer;

    return wave;
}

// Procedural 16-bit arcade stage soundtrack (Phrygian dominant mode, driving percussion)
Wave AudioManager::GenerateStageMusicWave() {
    constexpr unsigned int sampleRate = 44100;
    constexpr float bpm = 124.0f;
    constexpr float beatSec = 60.0f / bpm;
    constexpr int totalBeats = 16; // 4 bars of 4/4
    float totalDuration = totalBeats * beatSec;
    unsigned int frameCount = static_cast<unsigned int>(sampleRate * totalDuration);

    short* buffer = (short*)malloc(frameCount * sizeof(short));
    constexpr float PI2 = 6.28318530718f;

    // Melody notes in D Phrygian Dominant (D4, Eb4, F#4, G4, A4, Bb4, C5)
    constexpr float D3 = 146.83f;
    constexpr float Eb3 = 155.56f;
    constexpr float Fs3 = 185.00f;
    constexpr float G3 = 196.00f;
    constexpr float A3 = 220.00f;
    (void)A3;

    constexpr float D4 = 293.66f;
    constexpr float Eb4 = 311.13f;
    constexpr float Fs4 = 369.99f;
    constexpr float G4 = 392.00f;
    constexpr float A4 = 440.00f;
    constexpr float Bb4 = 466.16f;

    // 16-beat bassline pattern
    float bassNotes[16] = {
        D3, D3, Eb3, D3,   D3, Fs3, Eb3, D3,
        D3, D3, G3, Fs3,   Eb3, D3, Eb3, D3
    };

    // 16-beat lead melody
    float leadNotes[16] = {
        D4, D4, Fs4, G4,   A4, Bb4, A4, G4,
        Fs4, G4, Fs4, Eb4, D4, Fs4, Eb4, D4
    };

    for (unsigned int i = 0; i < frameCount; ++i) {
        float t = static_cast<float>(i) / sampleRate;
        float beatProgress = fmodf(t, beatSec) / beatSec;
        int currentBeat = static_cast<int>(t / beatSec) % totalBeats;

        // 1. Kick drum on beats 0, 2, 4, 6, 8, 10, 12, 14
        float kick = 0.0f;
        if (currentBeat % 2 == 0) {
            float kickEnv = expf(-beatProgress * 22.0f);
            float kickFreq = 120.0f - 80.0f * beatProgress;
            kick = sinf(PI2 * kickFreq * beatProgress * beatSec) * kickEnv * 0.7f;
        }

        // 2. Snare / Clack on beats 1, 3, 5, 7, 9, 11, 13, 15
        float snare = 0.0f;
        if (currentBeat % 2 == 1) {
            float snareEnv = expf(-beatProgress * 18.0f);
            float noise = ((float)rand() / (float)RAND_MAX * 2.0f - 1.0f);
            snare = noise * snareEnv * 0.45f;
        }

        // 3. Hi-hat on 16th notes (4 times per beat)
        float sixteenth = fmodf(t, beatSec / 4.0f) / (beatSec / 4.0f);
        float hat = ((float)rand() / (float)RAND_MAX * 2.0f - 1.0f) * expf(-sixteenth * 35.0f) * 0.15f;

        // 4. Bass synth (bouncy saw-like tone)
        float bassFreq = bassNotes[currentBeat];
        float bassEnv = 1.0f - beatProgress * 0.6f;
        float bPhase = fmodf(t * bassFreq, 1.0f);
        float bassWave = (bPhase * 2.0f - 1.0f) * 0.35f * bassEnv;

        // 5. Lead melody (oriental reed / flute tone)
        float leadFreq = leadNotes[currentBeat];
        float leadEnv = sinf(beatProgress * 3.14159f);
        float leadSine = sinf(PI2 * leadFreq * t);
        float leadHarm = sinf(PI2 * (leadFreq * 2.0f) * t) * 0.3f;
        float lead = (leadSine + leadHarm) * 0.3f * leadEnv;

        float mix = (kick + snare + hat + bassWave + lead) * 0.6f;
        mix = ClampFloat(mix, -1.0f, 1.0f);
        buffer[i] = static_cast<short>(mix * 26000.0f);
    }

    Wave wave = {};
    wave.frameCount = frameCount;
    wave.sampleRate = sampleRate;
    wave.sampleSize = 16;
    wave.channels = 1;
    wave.data = buffer;

    return wave;
}

// Procedural boss battle track (faster, intense, heavy rhythm)
Wave AudioManager::GenerateBossMusicWave() {
    constexpr unsigned int sampleRate = 44100;
    constexpr float bpm = 148.0f;
    constexpr float beatSec = 60.0f / bpm;
    constexpr int totalBeats = 16;
    float totalDuration = totalBeats * beatSec;
    unsigned int frameCount = static_cast<unsigned int>(sampleRate * totalDuration);

    short* buffer = (short*)malloc(frameCount * sizeof(short));
    constexpr float PI2 = 6.28318530718f;

    constexpr float E3 = 164.81f;
    constexpr float F3 = 174.61f;
    constexpr float G3 = 196.00f;
    constexpr float Bb3 = 233.08f;
    constexpr float B3 = 246.94f;

    constexpr float E4 = 329.63f;
    constexpr float F4 = 349.23f;
    constexpr float Gs4 = 415.30f;
    constexpr float A4 = 440.00f;
    constexpr float B4 = 493.88f;

    float bassNotes[16] = {
        E3, E3, F3, E3,    G3, F3, E3, Bb3,
        E3, E3, F3, E3,    B3, Bb3, G3, F3
    };

    float leadNotes[16] = {
        E4, Gs4, B4, A4,   Gs4, F4, E4, F4,
        E4, B4, A4, Gs4,   F4, Gs4, F4, E4
    };

    for (unsigned int i = 0; i < frameCount; ++i) {
        float t = static_cast<float>(i) / sampleRate;
        float beatProgress = fmodf(t, beatSec) / beatSec;
        int currentBeat = static_cast<int>(t / beatSec) % totalBeats;

        // Double kick drum for boss urgency
        float subBeat = fmodf(t, beatSec / 2.0f) / (beatSec / 2.0f);
        float kick = sinf(PI2 * (140.0f - 90.0f * subBeat) * subBeat * beatSec) * expf(-subBeat * 24.0f) * 0.65f;

        // Heavy snare
        float snare = 0.0f;
        if (currentBeat % 2 == 1) {
            snare = ((float)rand() / (float)RAND_MAX * 2.0f - 1.0f) * expf(-beatProgress * 15.0f) * 0.5f;
        }

        // Fast synth bass
        float bassFreq = bassNotes[currentBeat];
        float bPhase = fmodf(t * bassFreq, 1.0f);
        float bass = (bPhase > 0.5f ? 1.0f : -1.0f) * 0.28f * (1.0f - beatProgress * 0.5f);

        // Dramatic lead
        float leadFreq = leadNotes[currentBeat];
        float lead = sinf(PI2 * leadFreq * t) * 0.35f * sinf(beatProgress * 3.14159f);

        float mix = (kick + snare + bass + lead) * 0.6f;
        mix = ClampFloat(mix, -1.0f, 1.0f);
        buffer[i] = static_cast<short>(mix * 26000.0f);
    }

    Wave wave = {};
    wave.frameCount = frameCount;
    wave.sampleRate = sampleRate;
    wave.sampleSize = 16;
    wave.channels = 1;
    wave.data = buffer;

    return wave;
}

// Procedural authentic 1987 Sega arcade continue theme (Shinobi Yamaha YM2151 FM synth style)
Wave AudioManager::GenerateContinueMusicWave() {
    constexpr unsigned int sampleRate = 44100;
    constexpr float bpm = 136.0f;
    constexpr float beatSec = 60.0f / bpm;
    constexpr int totalBeats = 16; // 4 bars of 4/4
    float totalDuration = totalBeats * beatSec;
    unsigned int frameCount = static_cast<unsigned int>(sampleRate * totalDuration);

    short* buffer = (short*)malloc(frameCount * sizeof(short));
    constexpr float PI2 = 6.28318530718f;

    // Frequencies for FM bass and chords in D minor / modal tension
    constexpr float D2  = 73.42f;
    constexpr float F2  = 87.31f;
    constexpr float G2  = 98.00f;
    constexpr float A2  = 110.00f;
    constexpr float Bb2 = 116.54f;
    constexpr float C3  = 130.81f;
    constexpr float Cs3 = 138.59f;
    constexpr float D3  = 146.83f;

    constexpr float D4  = 293.66f;
    constexpr float E4  = 329.63f;
    constexpr float F4  = 349.23f;
    constexpr float G4  = 392.00f;
    constexpr float A4  = 440.00f;
    constexpr float Bb4 = 466.16f;
    constexpr float Cs5 = 554.37f;

    constexpr float D5  = 587.33f;
    constexpr float E5  = 659.25f;
    constexpr float F5  = 698.46f;
    constexpr float A5  = 880.00f;
    constexpr float C6  = 1046.50f;
    constexpr float Cs6 = 1108.73f;
    constexpr float D6  = 1174.66f;
    constexpr float E6  = 1318.51f;

    // 16-beat slap bass pattern (classic arcade syncopation)
    float bassNotes[16] = {
        D2,  D2,  F2,  D2,     // Bar 1: D minor slap
        G2,  F2,  D2,  Cs3,    // Bar 2: tension chromatic
        Bb2, Bb2, D3,  Bb2,    // Bar 3: Bb turnaround
        C3,  C3,  A2,  Cs3     // Bar 4: leading to Dm loop
    };

    // 16-step tense arpeggio pattern
    float arpNotes[16] = {
        D5, F5, A5, D6,   C6, A5, F5, E5,
        F5, A5, D6, E6,   D6, Cs6, A5, F5
    };

    for (unsigned int i = 0; i < frameCount; ++i) {
        float t = static_cast<float>(i) / sampleRate;
        float beatProgress = fmodf(t, beatSec) / beatSec;
        int currentBeat = static_cast<int>(t / beatSec) % totalBeats;
        int currentBar = currentBeat / 4;
        float barProgress = fmodf(t, beatSec * 4.0f) / beatSec;

        // 1. Kick drum on beats 0, 2, 4, 6, 8, 10, 12, 14
        float kick = 0.0f;
        if (currentBeat % 2 == 0) {
            float kEnv = expf(-beatProgress * 22.0f);
            float kFreq = 145.0f - 95.0f * beatProgress;
            kick = sinf(PI2 * kFreq * beatProgress * beatSec) * kEnv * 0.65f;
        }

        // 2. Snare on beats 1, 3, 5, 7, 9, 11, 13, 15 (punchy Sega Yamaha FM snare)
        float snare = 0.0f;
        if (currentBeat % 2 == 1) {
            float sEnv = expf(-beatProgress * 16.0f);
            float sNoise = ((float)rand() / (float)RAND_MAX * 2.0f - 1.0f);
            float sTone = sinf(PI2 * (210.0f - 110.0f * beatProgress) * beatProgress * beatSec);
            snare = (sNoise * 0.7f + sTone * 0.5f) * sEnv * 0.50f;
        }

        // 3. Ticking 16th-note hi-hat (tension clock)
        float sub16 = fmodf(t, beatSec / 4.0f) / (beatSec / 4.0f);
        int sub16Idx = static_cast<int>(fmodf(t, beatSec * 4.0f) / (beatSec / 4.0f)) % 16;
        float hatNoise = ((float)rand() / (float)RAND_MAX * 2.0f - 1.0f);
        float hat = hatNoise * expf(-sub16 * 38.0f) * 0.16f;

        // 4. FM Slap Bass (Yamaha YM2151 2-operator FM synth)
        float bFreq = bassNotes[currentBeat];
        float bMod = sinf(PI2 * (bFreq * 2.0f) * t) * 1.75f * expf(-sub16 * 14.0f);
        float bCarrier = sinf(PI2 * bFreq * t + bMod);
        float bSnap = (fmodf(t * bFreq, 1.0f) > 0.5f ? 1.0f : -1.0f) * 0.15f;
        float bass = (bCarrier * 0.85f + bSnap) * expf(-sub16 * 7.5f) * 0.42f;

        // 5. Arcade FM Brass Stabs (Dm, Bb, Gm, A7)
        float chordR = D4, chordM = F4, chordT = A4;
        if (currentBar == 1) { chordR = D4; chordM = F4; chordT = Bb4; }      // Bb
        else if (currentBar == 2) { chordR = D4; chordM = G4; chordT = Bb4; } // Gm
        else if (currentBar == 3) { chordR = Cs5 * 0.5f; chordM = E4; chordT = A4; } // A7

        float stab = 0.0f;
        float stabEnv = 0.0f;
        if (barProgress < 0.6f) {
            stabEnv = expf(-(barProgress / 0.6f) * 6.0f);
        } else if (barProgress >= 1.5f && barProgress < 2.1f) {
            stabEnv = expf(-((barProgress - 1.5f) / 0.6f) * 6.0f);
        }
        if (stabEnv > 0.01f) {
            float c1 = sinf(PI2 * chordR * t);
            float c2 = sinf(PI2 * chordM * t);
            float c3 = sinf(PI2 * chordT * t);
            float cBright = sinf(PI2 * (chordT * 2.0f) * t) * 0.25f;
            stab = (c1 + c2 + c3 + cBright) * 0.16f * stabEnv;
        }

        // 6. High 16th-note Tension Arpeggio (countdown chime)
        float arpFreq = arpNotes[sub16Idx];
        float arp = sinf(PI2 * arpFreq * t) * expf(-sub16 * 12.0f) * 0.15f;

        float mix = (kick + snare + hat + bass + stab + arp) * 0.70f;
        mix = ClampFloat(mix, -1.0f, 1.0f);
        buffer[i] = static_cast<short>(mix * 27000.0f);
    }

    Wave wave = {};
    wave.frameCount = frameCount;
    wave.sampleRate = sampleRate;
    wave.sampleSize = 16;
    wave.channels = 1;
    wave.data = buffer;

    return wave;
}

// Procedural Mission 2 Stage soundtrack (Nocturnal Kidron & Gethsemane groove, C minor)
Wave AudioManager::GenerateStage2MusicWave() {
    constexpr unsigned int sampleRate = 44100;
    constexpr float bpm = 126.0f;
    constexpr float beatSec = 60.0f / bpm;
    constexpr int totalBeats = 16;
    float totalDuration = totalBeats * beatSec;
    unsigned int frameCount = static_cast<unsigned int>(sampleRate * totalDuration);

    short* buffer = (short*)malloc(frameCount * sizeof(short));
    constexpr float PI2 = 6.28318530718f;

    // Frequencies in C minor / Dorian (C3, Eb3, F3, G3, Bb3, C4, D4, Eb4, G4)
    constexpr float C3  = 130.81f;
    constexpr float Eb3 = 155.56f;
    constexpr float F3  = 174.61f;
    constexpr float G3  = 196.00f;
    constexpr float Bb3 = 233.08f;
    constexpr float C4  = 261.63f;
    constexpr float D4  = 293.66f;
    constexpr float Eb4 = 311.13f;
    constexpr float F4  = 349.23f;
    constexpr float G4  = 392.00f;

    float bassNotes[16] = {
        C3, C3, Eb3, C3,   F3, Eb3, C3, Bb3,
        C3, C3, G3, F3,    Eb3, F3, D4, C3
    };

    float leadNotes[16] = {
        C4, Eb4, G4, Eb4,  F4, Eb4, D4, C4,
        Eb4, G4, Bb3, C4,  D4, Eb4, D4, C4
    };

    for (unsigned int i = 0; i < frameCount; ++i) {
        float t = static_cast<float>(i) / sampleRate;
        float beatProgress = fmodf(t, beatSec) / beatSec;
        int currentBeat = static_cast<int>(t / beatSec) % totalBeats;

        // 1. Muffled deep nocturnal kick on beats 0, 2, 4, 6, 8, 10, 12, 14
        float kick = 0.0f;
        if (currentBeat % 2 == 0) {
            float kEnv = expf(-beatProgress * 18.0f);
            float kFreq = 110.0f - 75.0f * beatProgress;
            kick = sinf(PI2 * kFreq * beatProgress * beatSec) * kEnv * 0.7f;
        }

        // 2. Crisp rimshot / snare on beats 1, 3, 5, 7, 9, 11, 13, 15
        float snare = 0.0f;
        if (currentBeat % 2 == 1) {
            float sEnv = expf(-beatProgress * 20.0f);
            float noise = ((float)rand() / (float)RAND_MAX * 2.0f - 1.0f);
            snare = noise * sEnv * 0.42f;
        }

        // 3. Night crickets / 16th-note shaker
        float sub16 = fmodf(t, beatSec / 4.0f) / (beatSec / 4.0f);
        float hat = ((float)rand() / (float)RAND_MAX * 2.0f - 1.0f) * expf(-sub16 * 40.0f) * 0.12f;

        // 4. Deep analog bass
        float bFreq = bassNotes[currentBeat];
        float bPhase = fmodf(t * bFreq, 1.0f);
        float bWave = (bPhase > 0.5f ? 1.0f : -1.0f) * 0.32f * (1.0f - beatProgress * 0.4f);

        // 5. Haunting nocturnal lead with slight vibrato
        float lFreq = leadNotes[currentBeat];
        float vibrato = sinf(PI2 * 5.0f * t) * 3.5f;
        float lead = sinf(PI2 * (lFreq + vibrato) * t) * 0.28f * sinf(beatProgress * 3.14159f);

        // 6. Eerie cathedral chime on bar downbeats (beats 0, 8)
        float chime = 0.0f;
        if (currentBeat == 0 || currentBeat == 8) {
            float cProgress = fmodf(t, beatSec * 8.0f) / (beatSec * 8.0f);
            chime = sinf(PI2 * 1046.50f * t) * expf(-cProgress * 8.0f) * 0.18f;
        }

        float mix = (kick + snare + hat + bWave + lead + chime) * 0.65f;
        mix = ClampFloat(mix, -1.0f, 1.0f);
        buffer[i] = static_cast<short>(mix * 26000.0f);
    }

    Wave wave = {};
    wave.frameCount = frameCount;
    wave.sampleRate = sampleRate;
    wave.sampleSize = 16;
    wave.channels = 1;
    wave.data = buffer;
    return wave;
}

// Procedural Stage 3 music (Bazaar / Caiaphas / Upper Room: 132 BPM Middle-Eastern / Phrygian arcade theme)
Wave AudioManager::GenerateStage3MusicWave() {
    constexpr unsigned int sampleRate = 44100;
    constexpr float bpm = 132.0f;
    constexpr float beatSec = 60.0f / bpm;
    constexpr int totalBeats = 16;
    float totalDuration = totalBeats * beatSec;
    unsigned int frameCount = static_cast<unsigned int>(sampleRate * totalDuration);

    short* buffer = (short*)malloc(frameCount * sizeof(short));
    constexpr float PI2 = 6.28318530718f;

    // D Phrygian Dominant (D3, Eb3, F#3, G3, A3, Bb3, C4, D4, Eb4, F#4)
    constexpr float D3  = 146.83f;
    constexpr float Eb3 = 155.56f;
    constexpr float Fs3 = 185.00f;
    constexpr float G3  = 196.00f;
    constexpr float A3  = 220.00f;
    constexpr float Bb3 = 233.08f;
    constexpr float C4  = 261.63f;
    constexpr float D4  = 293.66f;
    constexpr float Eb4 = 311.13f;
    constexpr float Fs4 = 369.99f;
    constexpr float G4  = 392.00f;
    constexpr float A4  = 440.00f;

    float bassNotes[16] = {
        D3, D3, Eb3, D3,   Fs3, G3, Fs3, Eb3,
        D3, D3, Bb3, A3,   G3, Fs3, Eb3, D3
    };

    float leadNotes[16] = {
        D4, Fs4, G4, Fs4,  Eb4, D4, Eb4, Fs4,
        A4, G4, Fs4, Eb4,  D4, Eb4, Fs4, D4
    };

    for (unsigned int i = 0; i < frameCount; ++i) {
        float t = static_cast<float>(i) / sampleRate;
        float beatProgress = fmodf(t, beatSec) / beatSec;
        int currentBeat = static_cast<int>(t / beatSec) % totalBeats;

        // 1. Darbuka-style syncopated arcade bass drum
        float subBeat = fmodf(t, beatSec / 2.0f) / (beatSec / 2.0f);
        float kick = 0.0f;
        if (currentBeat % 2 == 0 || currentBeat == 7 || currentBeat == 15) {
            float kEnv = expf(-subBeat * 20.0f);
            float kFreq = 125.0f - 80.0f * subBeat;
            kick = sinf(PI2 * kFreq * subBeat * beatSec) * kEnv * 0.7f;
        }

        // 2. Darbuka "Tek" / crisp rim snare
        float snare = 0.0f;
        if (currentBeat % 2 == 1 || (currentBeat % 4 == 2 && beatProgress > 0.5f)) {
            float sEnv = expf(-beatProgress * 22.0f);
            float noise = ((float)rand() / (float)RAND_MAX * 2.0f - 1.0f);
            snare = noise * sEnv * 0.45f;
        }

        // 3. Shaker / Tambourine jingle
        float sub16 = fmodf(t, beatSec / 4.0f) / (beatSec / 4.0f);
        float tambourine = ((float)rand() / (float)RAND_MAX * 2.0f - 1.0f) * expf(-sub16 * 35.0f) * 0.15f;

        // 4. Driving Oud/Lute synth bass
        float bFreq = bassNotes[currentBeat];
        float bPhase = fmodf(t * bFreq, 1.0f);
        float bWave = (bPhase > 0.45f ? 1.0f : -1.0f) * 0.32f * (1.0f - beatProgress * 0.45f);

        // 5. Eastern modal lead melody
        float lFreq = leadNotes[currentBeat];
        float vibrato = sinf(PI2 * 6.0f * t) * 4.0f;
        float lead = sinf(PI2 * (lFreq + vibrato) * t) * 0.30f * sinf(beatProgress * 3.14159f);

        // 6. Occasional bronze finger cymbal ping (every 4 beats)
        float cymbal = 0.0f;
        if (currentBeat % 4 == 0) {
            float cProg = fmodf(t, beatSec * 4.0f) / (beatSec * 4.0f);
            cymbal = (sinf(PI2 * 2800.0f * t) + 0.5f * sinf(PI2 * 3600.0f * t)) * expf(-cProg * 12.0f) * 0.16f;
        }

        float mix = (kick + snare + tambourine + bWave + lead + cymbal) * 0.65f;
        mix = ClampFloat(mix, -1.0f, 1.0f);
        buffer[i] = static_cast<short>(mix * 26000.0f);
    }

    Wave wave = {};
    wave.frameCount = frameCount;
    wave.sampleRate = sampleRate;
    wave.sampleSize = 16;
    wave.channels = 1;
    wave.data = buffer;
    return wave;
}

// Procedural Judas Iscariot final boss track (164 BPM Frenetic arcade climax, 30 silver coins clinking pulse)
Wave AudioManager::GenerateJudasBossMusicWave() {
    constexpr unsigned int sampleRate = 44100;
    constexpr float bpm = 164.0f;
    constexpr float beatSec = 60.0f / bpm;
    constexpr int totalBeats = 16;
    float totalDuration = totalBeats * beatSec;
    unsigned int frameCount = static_cast<unsigned int>(sampleRate * totalDuration);

    short* buffer = (short*)malloc(frameCount * sizeof(short));
    constexpr float PI2 = 6.28318530718f;

    // Dark tense tones in F# minor / Locrian tension
    constexpr float Fs2 = 92.50f;
    constexpr float G2  = 98.00f;
    constexpr float A2  = 110.00f;
    constexpr float B2  = 123.47f;
    constexpr float C3  = 130.81f;
    constexpr float D3  = 146.83f;
    constexpr float E3  = 164.81f;
    constexpr float Fs3 = 185.00f;

    constexpr float Fs4 = 369.99f;
    constexpr float G4  = 392.00f;
    constexpr float A4  = 440.00f;
    constexpr float C5  = 523.25f;
    constexpr float D5  = 587.33f;
    constexpr float Fs5 = 739.99f;

    float bassNotes[16] = {
        Fs2, Fs2, G2, Fs2,  A2, G2, Fs2, C3,
        Fs2, Fs2, D3, C3,   B2, A2, G2, Fs2
    };

    float leadNotes[16] = {
        Fs4, G4, A4, C5,    D5, C5, A4, G4,
        Fs4, A4, C5, D5,    Fs5, D5, C5, Fs4
    };

    for (unsigned int i = 0; i < frameCount; ++i) {
        float t = static_cast<float>(i) / sampleRate;
        float beatProgress = fmodf(t, beatSec) / beatSec;
        int currentBeat = static_cast<int>(t / beatSec) % totalBeats;

        // 1. Relentless double-kick arcade drums
        float subBeat = fmodf(t, beatSec / 2.0f) / (beatSec / 2.0f);
        float kick = sinf(PI2 * (155.0f - 105.0f * subBeat) * subBeat * beatSec) * expf(-subBeat * 26.0f) * 0.7f;

        // 2. Heavy explosive snare on alternating beats
        float snare = 0.0f;
        if (currentBeat % 2 == 1) {
            float sEnv = expf(-beatProgress * 22.0f);
            float noise = ((float)rand() / (float)RAND_MAX * 2.0f - 1.0f);
            snare = noise * sEnv * 0.55f;
        }

        // 3. 30 Pieces of Silver clinking pulses (16th-note high metallic pings)
        float sub16 = fmodf(t, beatSec / 4.0f) / (beatSec / 4.0f);
        float coinFreq = 3200.0f + (currentBeat % 4) * 400.0f;
        float coinPing = sinf(PI2 * coinFreq * t) * expf(-sub16 * 30.0f) * 0.18f;

        // 4. Galloping 16th-note synth bass
        float bFreq = bassNotes[currentBeat];
        float bPhase = fmodf(t * bFreq, 1.0f);
        float bass = (bPhase > 0.5f ? 1.0f : -1.0f) * 0.35f * (1.0f - sub16 * 0.4f);

        // 5. Frenetic arcade lead synth
        float lFreq = leadNotes[currentBeat];
        float leadPhase = fmodf(t * lFreq, 1.0f);
        float leadWave = (leadPhase < 0.5f ? leadPhase * 2.0f : 2.0f - leadPhase * 2.0f) - 0.5f; // Triangle
        float lead = leadWave * 0.38f * (1.0f - beatProgress * 0.3f);

        float mix = (kick + snare + coinPing + bass + lead) * 0.65f;
        mix = ClampFloat(mix, -1.0f, 1.0f);
        buffer[i] = static_cast<short>(mix * 27000.0f);
    }

    Wave wave = {};
    wave.frameCount = frameCount;
    wave.sampleRate = sampleRate;
    wave.sampleSize = 16;
    wave.channels = 1;
    wave.data = buffer;
    return wave;
}

// Procedural Stage 4 track (126 BPM Roman Imperial Procession / Military March)
Wave AudioManager::GenerateStage4MusicWave() {
    constexpr unsigned int sampleRate = 44100;
    constexpr float bpm = 126.0f;
    constexpr float beatSec = 60.0f / bpm;
    constexpr int totalBeats = 16; // 4 bars of 4/4
    float totalDuration = totalBeats * beatSec;
    unsigned int frameCount = static_cast<unsigned int>(sampleRate * totalDuration);

    short* buffer = (short*)malloc(frameCount * sizeof(short));
    constexpr float PI2 = 6.28318530718f;

    // D minor / Dorian brass frequencies
    constexpr float D2  = 73.42f;
    constexpr float E2  = 82.41f;
    constexpr float F2  = 87.31f;
    constexpr float G2  = 98.00f;
    constexpr float A2  = 110.00f;
    constexpr float Bb2 = 116.54f;
    constexpr float C3  = 130.81f;
    constexpr float D3  = 146.83f;

    constexpr float D4  = 293.66f;
    constexpr float F4  = 349.23f;
    constexpr float G4  = 392.00f;
    constexpr float A4  = 440.00f;
    constexpr float Bb4 = 466.16f;
    constexpr float C5  = 523.25f;
    constexpr float D5  = 587.33f;

    float bassNotes[16] = {
        D2, D2, F2, G2,    A2, A2, Bb2, A2,
        G2, G2, F2, E2,    D2, F2, A2, D3
    };

    float leadNotes[16] = {
        D4, F4, A4, D5,    C5, A4, G4, A4,
        Bb4, A4, G4, F4,   G4, A4, F4, D4
    };

    for (unsigned int i = 0; i < frameCount; ++i) {
        float t = static_cast<float>(i) / sampleRate;
        float beatProgress = fmodf(t, beatSec) / beatSec;
        int currentBeat = static_cast<int>(t / beatSec) % totalBeats;

        // 1. Heavy Roman legionary march timpani / field kick on 0 and 2
        float kick = 0.0f;
        if (currentBeat % 2 == 0) {
            float kEnv = expf(-beatProgress * 14.0f);
            float kFreq = 85.0f - 40.0f * beatProgress;
            kick = sinf(PI2 * kFreq * beatProgress * beatSec) * kEnv * 0.75f;
        }

        // 2. Military snare drum cadence with 16th-note rolls
        float sub16 = fmodf(t, beatSec / 4.0f) / (beatSec / 4.0f);
        int subStep = static_cast<int>(fmodf(t, beatSec) / (beatSec / 4.0f));
        float snare = 0.0f;
        if (currentBeat % 2 == 1 || subStep == 2 || subStep == 3) {
            float sEnv = expf(-sub16 * 18.0f);
            float noise = ((float)rand() / (float)RAND_MAX * 2.0f - 1.0f);
            float sVol = (currentBeat % 2 == 1) ? 0.45f : 0.22f;
            snare = noise * sEnv * sVol;
        }

        // 3. Legionary bronze shield cadence & crash
        float shieldClick = 0.0f;
        if (currentBeat % 4 == 0 && beatProgress < 0.15f) {
            float cEnv = expf(-beatProgress * 28.0f);
            shieldClick = (sinf(PI2 * 2400.0f * t) + 0.4f * sinf(PI2 * 3400.0f * t)) * cEnv * 0.20f;
        }

        // 4. Low Roman brass tuba/cornu ostinato
        float bFreq = bassNotes[currentBeat];
        float bPhase = fmodf(t * bFreq, 1.0f);
        float bWave = (bPhase * 2.0f - 1.0f) * 0.32f * (1.0f - beatProgress * 0.4f);

        // 5. Stoic Roman trumpet lead melody
        float lFreq = leadNotes[currentBeat];
        float vib = sinf(PI2 * 5.2f * t) * 3.5f;
        float leadPhase = fmodf(t * (lFreq + vib), 1.0f);
        float leadWave = (leadPhase < 0.5f ? 1.0f : -1.0f) * 0.25f + sinf(PI2 * (lFreq + vib) * t) * 0.15f;
        float lead = leadWave * sinf(beatProgress * 3.14159f);

        float mix = (kick + snare + shieldClick + bWave + lead) * 0.65f;
        mix = ClampFloat(mix, -1.0f, 1.0f);
        buffer[i] = static_cast<short>(mix * 26000.0f);
    }

    Wave wave = {};
    wave.frameCount = frameCount;
    wave.sampleRate = sampleRate;
    wave.sampleSize = 16;
    wave.channels = 1;
    wave.data = buffer;
    return wave;
}

// Procedural Pontius Pilate Boss track (160 BPM Imperial Judgment & Tribunal Climax)
Wave AudioManager::GeneratePilateBossMusicWave() {
    constexpr unsigned int sampleRate = 44100;
    constexpr float bpm = 160.0f;
    constexpr float beatSec = 60.0f / bpm;
    constexpr int totalBeats = 16;
    float totalDuration = totalBeats * beatSec;
    unsigned int frameCount = static_cast<unsigned int>(sampleRate * totalDuration);

    short* buffer = (short*)malloc(frameCount * sizeof(short));
    constexpr float PI2 = 6.28318530718f;

    // Dark C minor / Phrygian tension frequencies
    constexpr float C2  = 65.41f;
    constexpr float Eb2 = 77.78f;
    constexpr float F2  = 87.31f;
    constexpr float G2  = 98.00f;
    constexpr float Ab2 = 103.83f;
    constexpr float Bb2 = 116.54f;
    constexpr float C3  = 130.81f;

    constexpr float C4  = 261.63f;
    constexpr float Eb4 = 311.13f;
    constexpr float F4  = 349.23f;
    constexpr float G4  = 392.00f;
    constexpr float Ab4 = 415.30f;
    constexpr float Bb4 = 466.16f;
    constexpr float C5  = 523.25f;
    constexpr float D5  = 587.33f;

    float bassNotes[16] = {
        C2, C2, Eb2, F2,    G2, Ab2, G2, F2,
        Eb2, Eb2, F2, G2,   Ab2, G2, F2, C2
    };

    float leadNotes[16] = {
        C4, Eb4, G4, C5,    Ab4, G4, F4, G4,
        Eb4, F4, G4, Bb4,   Ab4, G4, Eb4, C4
    };

    for (unsigned int i = 0; i < frameCount; ++i) {
        float t = static_cast<float>(i) / sampleRate;
        float beatProgress = fmodf(t, beatSec) / beatSec;
        int currentBeat = static_cast<int>(t / beatSec) % totalBeats;

        // 1. Driving four-on-the-floor imperial kick + sub drop
        float kEnv = expf(-beatProgress * 22.0f);
        float kick = sinf(PI2 * (160.0f - 110.0f * beatProgress) * beatProgress * beatSec) * kEnv * 0.70f;

        // 2. High-intensity military snare on beats 1, 3, 5, 7, 9, 11, 13, 15
        float snare = 0.0f;
        if (currentBeat % 2 == 1) {
            float sEnv = expf(-beatProgress * 15.0f);
            float noise = ((float)rand() / (float)RAND_MAX * 2.0f - 1.0f);
            snare = noise * sEnv * 0.48f;
        }

        // 3. Washing Basin water splash shimmer on beat 3 of each 4-beat measure
        float sub16 = fmodf(t, beatSec / 4.0f) / (beatSec / 4.0f);
        float waterShimmer = 0.0f;
        if (currentBeat % 4 == 3) {
            float wEnv = expf(-sub16 * 14.0f);
            float dropFreq = 2600.0f + 600.0f * sinf(PI2 * 24.0f * t);
            waterShimmer = sinf(PI2 * dropFreq * t) * wEnv * 0.22f;
        }

        // 4. Aggressive saw bass
        float bFreq = bassNotes[currentBeat];
        float bPhase = fmodf(t * bFreq, 1.0f);
        float bass = (bPhase * 2.0f - 1.0f) * 0.36f * (1.0f - beatProgress * 0.45f);

        // 5. Sharp imperial judgment brass synth lead
        float lFreq = leadNotes[currentBeat];
        float vib = sinf(PI2 * 5.8f * t) * 4.0f;
        float leadPhase = fmodf(t * (lFreq + vib), 1.0f);
        float lead = (leadPhase < 0.5f ? 1.0f : -1.0f) * 0.28f * (1.0f - beatProgress * 0.35f);

        float mix = (kick + snare + waterShimmer + bass + lead) * 0.65f;
        mix = ClampFloat(mix, -1.0f, 1.0f);
        buffer[i] = static_cast<short>(mix * 27000.0f);
    }

    Wave wave = {};
    wave.frameCount = frameCount;
    wave.sampleRate = sampleRate;
    wave.sampleSize = 16;
    wave.channels = 1;
    wave.data = buffer;
    return wave;
}

// Procedural Bonus Stage track (Upbeat, fast 148 BPM Sega arcade FM synth style)
Wave AudioManager::GenerateBonusMusicWave() {
    constexpr unsigned int sampleRate = 44100;
    constexpr float bpm = 148.0f;
    constexpr float beatSec = 60.0f / bpm;
    constexpr int totalBeats = 16;
    float totalDuration = totalBeats * beatSec;
    unsigned int frameCount = static_cast<unsigned int>(sampleRate * totalDuration);

    short* buffer = (short*)malloc(frameCount * sizeof(short));
    constexpr float PI2 = 6.28318530718f;

    // Joyous G Major pentatonic frequencies
    constexpr float G2 = 98.00f;
    constexpr float B2 = 123.47f;
    constexpr float D3 = 146.83f;
    constexpr float E3 = 164.81f;
    constexpr float G3 = 196.00f;

    constexpr float G4 = 392.00f;
    constexpr float A4 = 440.00f;
    constexpr float B4 = 493.88f;
    constexpr float D5 = 587.33f;
    constexpr float E5 = 659.25f;
    constexpr float G5 = 783.99f;

    float bassNotes[16] = {
        G2, G2, B2, D3,    E3, D3, B2, G2,
        G2, G2, D3, E3,    G3, E3, D3, B2
    };

    float leadNotes[16] = {
        G4, B4, D5, E5,    D5, B4, A4, G4,
        B4, D5, E5, G5,    E5, D5, B4, G4
    };

    for (unsigned int i = 0; i < frameCount; ++i) {
        float t = static_cast<float>(i) / sampleRate;
        float beatProgress = fmodf(t, beatSec) / beatSec;
        int currentBeat = static_cast<int>(t / beatSec) % totalBeats;

        // 1. Punchy four-on-the-floor arcade kick
        float kEnv = expf(-beatProgress * 24.0f);
        float kick = sinf(PI2 * (150.0f - 100.0f * beatProgress) * beatProgress * beatSec) * kEnv * 0.7f;

        // 2. Handclap / Snare on 1, 3, 5, 7, 9, 11, 13, 15
        float snare = 0.0f;
        if (currentBeat % 2 == 1) {
            float sEnv = expf(-beatProgress * 16.0f);
            float noise = ((float)rand() / (float)RAND_MAX * 2.0f - 1.0f);
            snare = noise * sEnv * 0.5f;
        }

        // 3. Fast open/closed hi-hats
        float sub16 = fmodf(t, beatSec / 4.0f) / (beatSec / 4.0f);
        float hat = ((float)rand() / (float)RAND_MAX * 2.0f - 1.0f) * expf(-sub16 * 32.0f) * 0.18f;

        // 4. Slap FM bassline
        float bFreq = bassNotes[currentBeat];
        float bPhase = fmodf(t * bFreq, 1.0f);
        float bWave = (bPhase * 2.0f - 1.0f) * 0.35f * (1.0f - beatProgress * 0.5f);

        // 5. Bright bell-like arpeggiated lead
        float lFreq = leadNotes[currentBeat];
        float lead = (sinf(PI2 * lFreq * t) + sinf(PI2 * lFreq * 2.0f * t) * 0.4f) * 0.30f * sinf(beatProgress * 3.14159f);

        float mix = (kick + snare + hat + bWave + lead) * 0.65f;
        mix = ClampFloat(mix, -1.0f, 1.0f);
        buffer[i] = static_cast<short>(mix * 26000.0f);
    }

    Wave wave = {};
    wave.frameCount = frameCount;
    wave.sampleRate = sampleRate;
    wave.sampleSize = 16;
    wave.channels = 1;
    wave.data = buffer;
    return wave;
}

// Procedural Evil Clone Boss track (Intense, 160 BPM Dark Baroque counterpoint)
Wave AudioManager::GenerateEvilCloneMusicWave() {
    constexpr unsigned int sampleRate = 44100;
    constexpr float bpm = 160.0f;
    constexpr float beatSec = 60.0f / bpm;
    constexpr int totalBeats = 16;
    float totalDuration = totalBeats * beatSec;
    unsigned int frameCount = static_cast<unsigned int>(sampleRate * totalDuration);

    short* buffer = (short*)malloc(frameCount * sizeof(short));
    constexpr float PI2 = 6.28318530718f;

    // D minor Harmonic frequencies
    constexpr float D3  = 146.83f;
    constexpr float F3  = 174.61f;
    constexpr float G3  = 196.00f;
    constexpr float A3  = 220.00f;
    constexpr float Bb3 = 233.08f;
    constexpr float Cs4 = 277.18f;

    constexpr float D4  = 293.66f;
    constexpr float E4  = 329.63f;
    constexpr float F4  = 349.23f;
    constexpr float G4  = 392.00f;
    constexpr float A4  = 440.00f;
    constexpr float Bb4 = 466.16f;
    constexpr float Cs5 = 554.37f;
    constexpr float D5  = 587.33f;

    float bassNotes[16] = {
        D3, D3, F3, D3,    G3, F3, D3, Cs4,
        Bb3, Bb3, A3, G3,  F3, G3, A3, Cs4
    };

    float leadNotes[16] = {
        D4, F4, A4, D5,    Cs5, Bb4, A4, G4,
        F4, E4, D4, F4,    A4, Cs5, D5, Cs5
    };

    for (unsigned int i = 0; i < frameCount; ++i) {
        float t = static_cast<float>(i) / sampleRate;
        float beatProgress = fmodf(t, beatSec) / beatSec;
        int currentBeat = static_cast<int>(t / beatSec) % totalBeats;

        // Double-time aggressive kick drum
        float subBeat = fmodf(t, beatSec / 2.0f) / (beatSec / 2.0f);
        float kick = sinf(PI2 * (160.0f - 110.0f * subBeat) * subBeat * beatSec) * expf(-subBeat * 22.0f) * 0.7f;

        // Heavy industrial snare
        float snare = 0.0f;
        if (currentBeat % 2 == 1) {
            float sEnv = expf(-beatProgress * 15.0f);
            float noise = ((float)rand() / (float)RAND_MAX * 2.0f - 1.0f);
            snare = noise * sEnv * 0.55f;
        }

        // Tense rapid 16th hats
        float sub16 = fmodf(t, beatSec / 4.0f) / (beatSec / 4.0f);
        float hat = ((float)rand() / (float)RAND_MAX * 2.0f - 1.0f) * expf(-sub16 * 45.0f) * 0.16f;

        // Distorted sawtooth bass
        float bFreq = bassNotes[currentBeat];
        float bPhase = fmodf(t * bFreq, 1.0f);
        float bWave = (bPhase * 2.0f - 1.0f);
        bWave = ClampFloat(bWave * 1.5f, -1.0f, 1.0f) * 0.35f * (1.0f - beatProgress * 0.5f);

        // Furious classical lead synth
        float lFreq = leadNotes[currentBeat];
        float lPhase = fmodf(t * lFreq, 1.0f);
        float lead = (lPhase > 0.5f ? 0.8f : -0.8f) * 0.28f * sinf(beatProgress * 3.14159f);

        float mix = (kick + snare + hat + bWave + lead) * 0.65f;
        mix = ClampFloat(mix, -1.0f, 1.0f);
        buffer[i] = static_cast<short>(mix * 26000.0f);
    }

    Wave wave = {};
    wave.frameCount = frameCount;
    wave.sampleRate = sampleRate;
    wave.sampleSize = 16;
    wave.channels = 1;
    wave.data = buffer;
    return wave;
}

// Mission 5: Golgotha Summit Climax battle theme (Apocalyptic minor mode, thunderous percussion)
Wave AudioManager::GenerateStage5MusicWave() {
    constexpr unsigned int sampleRate = 44100;
    constexpr float bpm = 138.0f;
    constexpr float beatSec = 60.0f / bpm;
    constexpr int totalBeats = 16;
    float totalDuration = totalBeats * beatSec;
    unsigned int frameCount = static_cast<unsigned int>(sampleRate * totalDuration);

    short* buffer = (short*)malloc(frameCount * sizeof(short));
    constexpr float PI2 = 6.28318530718f;

    // C Minor / C Harmonic Minor scale
    constexpr float C3  = 130.81f;
    constexpr float Eb3 = 155.56f;
    constexpr float F3  = 174.61f;
    constexpr float G3  = 196.00f;
    constexpr float Ab3 = 207.65f;
    constexpr float B3  = 246.94f;

    constexpr float C4  = 261.63f;
    constexpr float D4  = 293.66f;
    constexpr float Eb4 = 311.13f;
    constexpr float F4  = 349.23f;
    constexpr float G4  = 392.00f;
    constexpr float Ab4 = 415.30f;
    constexpr float B4  = 493.88f;
    constexpr float C5  = 523.25f;

    float bassNotes[16] = {
        C3, C3, Eb3, C3,   F3, Eb3, C3, B3,
        Ab3, Ab3, G3, F3,  Eb3, F3, G3, B3
    };

    float leadNotes[16] = {
        C4, Eb4, G4, C5,   B4, Ab4, G4, F4,
        Eb4, D4, C4, Eb4,  G4, B4, C5, B4
    };

    for (unsigned int i = 0; i < frameCount; ++i) {
        float t = static_cast<float>(i) / sampleRate;
        float beatProgress = fmodf(t, beatSec) / beatSec;
        int currentBeat = static_cast<int>(t / beatSec) % totalBeats;

        // Double-kick drum pattern
        float subBeat = fmodf(t, beatSec / 2.0f) / (beatSec / 2.0f);
        float kick = sinf(PI2 * (170.0f - 120.0f * subBeat) * subBeat * beatSec) * expf(-subBeat * 24.0f) * 0.72f;

        // Heavy apocalyptic snare
        float snare = 0.0f;
        if (currentBeat % 2 == 1) {
            float sEnv = expf(-beatProgress * 14.0f);
            float noise = ((float)rand() / (float)RAND_MAX * 2.0f - 1.0f);
            snare = noise * sEnv * 0.58f;
        }

        // Rapid 16th hats
        float sub16 = fmodf(t, beatSec / 4.0f) / (beatSec / 4.0f);
        float hat = ((float)rand() / (float)RAND_MAX * 2.0f - 1.0f) * expf(-sub16 * 48.0f) * 0.16f;

        // Menacing saw bass
        float bFreq = bassNotes[currentBeat];
        float bPhase = fmodf(t * bFreq, 1.0f);
        float bWave = (bPhase * 2.0f - 1.0f) * 0.38f * (1.0f - beatProgress * 0.45f);

        // Gothic cathedral organ chords
        float chordRoot = bassNotes[currentBeat] * 2.0f;
        float chordThird = chordRoot * 1.20f;
        float chordFifth = chordRoot * 1.50f;
        float organ = (sinf(PI2 * chordRoot * t) + 0.6f * sinf(PI2 * chordThird * t) + 0.5f * sinf(PI2 * chordFifth * t)) * 0.18f;

        // Soaring arcade lead synth
        float lFreq = leadNotes[currentBeat];
        float lPhase = fmodf(t * lFreq, 1.0f);
        float lead = (lPhase > 0.5f ? 0.75f : -0.75f) * 0.28f * sinf(beatProgress * 3.14159f);

        float mix = (kick + snare + hat + bWave + organ + lead) * 0.65f;
        mix = ClampFloat(mix, -1.0f, 1.0f);
        buffer[i] = static_cast<short>(mix * 26000.0f);
    }

    Wave wave = {};
    wave.frameCount = frameCount;
    wave.sampleRate = sampleRate;
    wave.sampleSize = 16;
    wave.channels = 1;
    wave.data = buffer;
    return wave;
}

// Heavenly Skee-Ball Victory Theme (Bouncy, joyful arcade carnival in C Major)
Wave AudioManager::GenerateSkeeBallMusicWave() {
    constexpr unsigned int sampleRate = 44100;
    constexpr float bpm = 126.0f;
    constexpr float beatSec = 60.0f / bpm;
    constexpr int totalBeats = 16;
    float totalDuration = totalBeats * beatSec;
    unsigned int frameCount = static_cast<unsigned int>(sampleRate * totalDuration);

    short* buffer = (short*)malloc(frameCount * sizeof(short));
    constexpr float PI2 = 6.28318530718f;

    constexpr float C3 = 130.81f;
    constexpr float G3 = 196.00f;
    constexpr float A3 = 220.00f;
    constexpr float F3 = 174.61f;

    constexpr float C4 = 261.63f;
    constexpr float D4 = 293.66f;
    constexpr float E4 = 329.63f;
    constexpr float F4 = 349.23f;
    constexpr float G4 = 392.00f;
    constexpr float A4 = 440.00f;
    constexpr float B4 = 493.88f;
    constexpr float C5 = 523.25f;
    constexpr float D5 = 587.33f;
    constexpr float E5 = 659.25f;

    float bassNotes[16] = {
        C3, C3, G3, G3,   A3, A3, F3, F3,
        C3, C3, G3, G3,   F3, G3, C3, C3
    };

    float leadNotes[16] = {
        C4, E4, G4, C5,   B4, A4, G4, E4,
        F4, A4, C5, E5,   D5, B4, C5, C5
    };

    for (unsigned int i = 0; i < frameCount; ++i) {
        float t = static_cast<float>(i) / sampleRate;
        float beatProgress = fmodf(t, beatSec) / beatSec;
        int currentBeat = static_cast<int>(t / beatSec) % totalBeats;

        // Bouncy kick on 0, 2, 4, ...
        float kick = 0.0f;
        if (currentBeat % 2 == 0) {
            float kEnv = expf(-beatProgress * 20.0f);
            kick = sinf(PI2 * (130.0f - 90.0f * beatProgress) * beatProgress * beatSec) * kEnv * 0.65f;
        }

        // Crisp clap / snare on 1, 3, 5, ...
        float snare = 0.0f;
        if (currentBeat % 2 == 1) {
            float sEnv = expf(-beatProgress * 18.0f);
            float noise = ((float)rand() / (float)RAND_MAX * 2.0f - 1.0f);
            snare = noise * sEnv * 0.45f;
        }

        // Bubbly bass
        float bFreq = bassNotes[currentBeat];
        float bWave = sinf(PI2 * bFreq * t) * 0.4f * expf(-beatProgress * 4.5f);

        // Glockenspiel / celesta chime lead
        float lFreq = leadNotes[currentBeat];
        float lEnv = expf(-beatProgress * 5.0f);
        float bell1 = sinf(PI2 * lFreq * t);
        float bell2 = sinf(PI2 * (lFreq * 2.0f) * t) * 0.35f;
        float bell3 = sinf(PI2 * (lFreq * 3.0f) * t) * 0.15f;
        float lead = (bell1 + bell2 + bell3) * lEnv * 0.35f;

        // Cheerful organ chords
        float chord = (sinf(PI2 * bFreq * 2.0f * t) + sinf(PI2 * bFreq * 2.5f * t) + sinf(PI2 * bFreq * 3.0f * t)) * 0.14f * (1.0f - beatProgress * 0.3f);

        float mix = (kick + snare + bWave + lead + chord) * 0.70f;
        mix = ClampFloat(mix, -1.0f, 1.0f);
        buffer[i] = static_cast<short>(mix * 25000.0f);
    }

    Wave wave = {};
    wave.frameCount = frameCount;
    wave.sampleRate = sampleRate;
    wave.sampleSize = 16;
    wave.channels = 1;
    wave.data = buffer;
    return wave;
}

void AudioManager::Init() {
    if (s_initialized) return;

    InitAudioDevice();

    // Generate SFX waves
    for (int i = 0; i < SFX_COUNT; ++i) {
        Wave wave = GenerateSfxWave(static_cast<SoundEffect>(i));
        s_sounds[i] = LoadSoundFromWave(wave);
        UnloadWave(wave);
    }

    // Generate looping music waves
    Wave m1Wave = GenerateStageMusicWave();
    s_stageMusicM1 = LoadSoundFromWave(m1Wave);
    UnloadWave(m1Wave);

    Wave m2Wave = GenerateStage2MusicWave();
    s_stageMusicM2 = LoadSoundFromWave(m2Wave);
    UnloadWave(m2Wave);

    Wave m3Wave = GenerateStage3MusicWave();
    s_stageMusicM3 = LoadSoundFromWave(m3Wave);
    UnloadWave(m3Wave);

    Wave m4Wave = GenerateStage4MusicWave();
    s_stageMusicM4 = LoadSoundFromWave(m4Wave);
    UnloadWave(m4Wave);

    Wave m5Wave = GenerateStage5MusicWave();
    s_stageMusicM5 = LoadSoundFromWave(m5Wave);
    UnloadWave(m5Wave);

    Wave skeeBallWave = GenerateSkeeBallMusicWave();
    s_skeeBallMusic = LoadSoundFromWave(skeeBallWave);
    UnloadWave(skeeBallWave);

    Wave bonusWave = GenerateBonusMusicWave();
    s_bonusMusic = LoadSoundFromWave(bonusWave);
    UnloadWave(bonusWave);

    Wave bossMusicWave = GenerateBossMusicWave();
    s_bossMusic = LoadSoundFromWave(bossMusicWave);
    UnloadWave(bossMusicWave);

    Wave evilCloneWave = GenerateEvilCloneMusicWave();
    s_evilCloneMusic = LoadSoundFromWave(evilCloneWave);
    UnloadWave(evilCloneWave);

    Wave judasWave = GenerateJudasBossMusicWave();
    s_judasBossMusic = LoadSoundFromWave(judasWave);
    UnloadWave(judasWave);

    Wave pilateWave = GeneratePilateBossMusicWave();
    s_pilateBossMusic = LoadSoundFromWave(pilateWave);
    UnloadWave(pilateWave);

    Wave continueMusicWave = GenerateContinueMusicWave();
    s_continueMusic = LoadSoundFromWave(continueMusicWave);
    UnloadWave(continueMusicWave);

    // Set volumes
    SetSoundVolume(s_stageMusicM1, 0.45f);
    SetSoundVolume(s_stageMusicM2, 0.48f);
    SetSoundVolume(s_stageMusicM3, 0.48f);
    SetSoundVolume(s_stageMusicM4, 0.50f);
    SetSoundVolume(s_stageMusicM5, 0.60f);
    SetSoundVolume(s_skeeBallMusic, 0.55f);
    SetSoundVolume(s_bonusMusic, 0.50f);
    SetSoundVolume(s_bossMusic, 0.55f);
    SetSoundVolume(s_evilCloneMusic, 0.58f);
    SetSoundVolume(s_judasBossMusic, 0.58f);
    SetSoundVolume(s_pilateBossMusic, 0.60f);
    SetSoundVolume(s_continueMusic, 0.55f);

    s_initialized = true;
    s_inBossArena = false;
}

void AudioManager::Close() {
    if (!s_initialized) return;

    for (int i = 0; i < SFX_COUNT; ++i) {
        if (s_sounds[i].frameCount > 0) {
            UnloadSound(s_sounds[i]);
        }
    }

    if (s_stageMusicM1.frameCount > 0) UnloadSound(s_stageMusicM1);
    if (s_stageMusicM2.frameCount > 0) UnloadSound(s_stageMusicM2);
    if (s_stageMusicM3.frameCount > 0) UnloadSound(s_stageMusicM3);
    if (s_stageMusicM4.frameCount > 0) UnloadSound(s_stageMusicM4);
    if (s_stageMusicM5.frameCount > 0) UnloadSound(s_stageMusicM5);
    if (s_skeeBallMusic.frameCount > 0) UnloadSound(s_skeeBallMusic);
    if (s_bonusMusic.frameCount > 0) UnloadSound(s_bonusMusic);
    if (s_bossMusic.frameCount > 0) UnloadSound(s_bossMusic);
    if (s_evilCloneMusic.frameCount > 0) UnloadSound(s_evilCloneMusic);
    if (s_judasBossMusic.frameCount > 0) UnloadSound(s_judasBossMusic);
    if (s_pilateBossMusic.frameCount > 0) UnloadSound(s_pilateBossMusic);
    if (s_continueMusic.frameCount > 0) UnloadSound(s_continueMusic);

    CloseAudioDevice();
    s_initialized = false;
}

void AudioManager::PlaySfx(SoundEffect sfx) {
    if (!s_initialized) return;
    if (sfx >= 0 && sfx < SFX_COUNT) {
        PlaySound(s_sounds[sfx]);
    }
}

void AudioManager::SetMusicEnabled(bool enabled) {
    s_musicEnabled = enabled;
    if (!s_musicEnabled) {
        StopAllMusic();
    }
}

bool AudioManager::IsMusicEnabled() {
    return s_musicEnabled;
}

void AudioManager::StopAllMusic() {
    if (!s_initialized) return;
    if (IsSoundPlaying(s_stageMusicM1)) StopSound(s_stageMusicM1);
    if (IsSoundPlaying(s_stageMusicM2)) StopSound(s_stageMusicM2);
    if (IsSoundPlaying(s_stageMusicM3)) StopSound(s_stageMusicM3);
    if (IsSoundPlaying(s_stageMusicM4)) StopSound(s_stageMusicM4);
    if (IsSoundPlaying(s_stageMusicM5)) StopSound(s_stageMusicM5);
    if (IsSoundPlaying(s_skeeBallMusic)) StopSound(s_skeeBallMusic);
    if (IsSoundPlaying(s_bonusMusic)) StopSound(s_bonusMusic);
    if (IsSoundPlaying(s_bossMusic)) StopSound(s_bossMusic);
    if (IsSoundPlaying(s_evilCloneMusic)) StopSound(s_evilCloneMusic);
    if (IsSoundPlaying(s_judasBossMusic)) StopSound(s_judasBossMusic);
    if (IsSoundPlaying(s_pilateBossMusic)) StopSound(s_pilateBossMusic);
    if (IsSoundPlaying(s_continueMusic)) StopSound(s_continueMusic);
}

void AudioManager::PlaySkeeBallMusic() {
    if (!s_initialized || !s_musicEnabled) return;
    StopAllMusic();
    if (!IsSoundPlaying(s_skeeBallMusic)) {
        PlaySound(s_skeeBallMusic);
    }
}

void AudioManager::Update(bool inBossArena, bool isGameOver, bool isVictory, int stageId, bool isBonusStage) {
    if (!s_initialized || !s_musicEnabled) return;

    if (isVictory) {
        StopAllMusic();
        return;
    }

    if (isGameOver) {
        if (IsSoundPlaying(s_stageMusicM1)) StopSound(s_stageMusicM1);
        if (IsSoundPlaying(s_stageMusicM2)) StopSound(s_stageMusicM2);
        if (IsSoundPlaying(s_stageMusicM3)) StopSound(s_stageMusicM3);
        if (IsSoundPlaying(s_stageMusicM4)) StopSound(s_stageMusicM4);
        if (IsSoundPlaying(s_stageMusicM5)) StopSound(s_stageMusicM5);
        if (IsSoundPlaying(s_skeeBallMusic)) StopSound(s_skeeBallMusic);
        if (IsSoundPlaying(s_bonusMusic)) StopSound(s_bonusMusic);
        if (IsSoundPlaying(s_bossMusic)) StopSound(s_bossMusic);
        if (IsSoundPlaying(s_evilCloneMusic)) StopSound(s_evilCloneMusic);
        if (IsSoundPlaying(s_judasBossMusic)) StopSound(s_judasBossMusic);
        if (IsSoundPlaying(s_pilateBossMusic)) StopSound(s_pilateBossMusic);

        if (!IsSoundPlaying(s_continueMusic)) {
            PlaySound(s_continueMusic);
        }
        return;
    }

    if (IsSoundPlaying(s_continueMusic)) {
        StopSound(s_continueMusic);
    }

    // Handle Bonus Stage music
    if (isBonusStage) {
        if (IsSoundPlaying(s_stageMusicM1)) StopSound(s_stageMusicM1);
        if (IsSoundPlaying(s_stageMusicM2)) StopSound(s_stageMusicM2);
        if (IsSoundPlaying(s_stageMusicM3)) StopSound(s_stageMusicM3);
        if (IsSoundPlaying(s_stageMusicM4)) StopSound(s_stageMusicM4);
        if (IsSoundPlaying(s_bossMusic)) StopSound(s_bossMusic);
        if (IsSoundPlaying(s_evilCloneMusic)) StopSound(s_evilCloneMusic);
        if (IsSoundPlaying(s_judasBossMusic)) StopSound(s_judasBossMusic);
        if (IsSoundPlaying(s_pilateBossMusic)) StopSound(s_pilateBossMusic);

        if (!IsSoundPlaying(s_bonusMusic)) {
            PlaySound(s_bonusMusic);
        }
        return;
    }

    if (IsSoundPlaying(s_bonusMusic)) {
        StopSound(s_bonusMusic);
    }

    if (stageId == 13) {
        if (IsSoundPlaying(s_stageMusicM1)) StopSound(s_stageMusicM1);
        if (IsSoundPlaying(s_stageMusicM2)) StopSound(s_stageMusicM2);
        if (IsSoundPlaying(s_stageMusicM3)) StopSound(s_stageMusicM3);
        if (IsSoundPlaying(s_stageMusicM4)) StopSound(s_stageMusicM4);
        if (IsSoundPlaying(s_bossMusic)) StopSound(s_bossMusic);
        if (IsSoundPlaying(s_evilCloneMusic)) StopSound(s_evilCloneMusic);
        if (IsSoundPlaying(s_judasBossMusic)) StopSound(s_judasBossMusic);
        if (IsSoundPlaying(s_pilateBossMusic)) StopSound(s_pilateBossMusic);
        if (IsSoundPlaying(s_skeeBallMusic)) StopSound(s_skeeBallMusic);

        if (!IsSoundPlaying(s_stageMusicM5)) {
            PlaySound(s_stageMusicM5);
        }
        return;
    }

    if (IsSoundPlaying(s_stageMusicM5)) {
        StopSound(s_stageMusicM5);
    }

    s_inBossArena = inBossArena;

    if (s_inBossArena) {
        if (IsSoundPlaying(s_stageMusicM1)) StopSound(s_stageMusicM1);
        if (IsSoundPlaying(s_stageMusicM2)) StopSound(s_stageMusicM2);
        if (IsSoundPlaying(s_stageMusicM3)) StopSound(s_stageMusicM3);
        if (IsSoundPlaying(s_stageMusicM4)) StopSound(s_stageMusicM4);

        if (stageId == 12) {
            // Pontius Pilate final boss theme!
            if (IsSoundPlaying(s_bossMusic)) StopSound(s_bossMusic);
            if (IsSoundPlaying(s_evilCloneMusic)) StopSound(s_evilCloneMusic);
            if (IsSoundPlaying(s_judasBossMusic)) StopSound(s_judasBossMusic);
            if (!IsSoundPlaying(s_pilateBossMusic)) {
                PlaySound(s_pilateBossMusic);
            }
        } else if (stageId == 9) {
            // Judas Iscariot final boss theme!
            if (IsSoundPlaying(s_bossMusic)) StopSound(s_bossMusic);
            if (IsSoundPlaying(s_evilCloneMusic)) StopSound(s_evilCloneMusic);
            if (IsSoundPlaying(s_pilateBossMusic)) StopSound(s_pilateBossMusic);
            if (!IsSoundPlaying(s_judasBossMusic)) {
                PlaySound(s_judasBossMusic);
            }
        } else if (stageId == 6) {
            // Evil Clone final boss theme!
            if (IsSoundPlaying(s_bossMusic)) StopSound(s_bossMusic);
            if (IsSoundPlaying(s_judasBossMusic)) StopSound(s_judasBossMusic);
            if (IsSoundPlaying(s_pilateBossMusic)) StopSound(s_pilateBossMusic);
            if (!IsSoundPlaying(s_evilCloneMusic)) {
                PlaySound(s_evilCloneMusic);
            }
        } else {
            if (IsSoundPlaying(s_evilCloneMusic)) StopSound(s_evilCloneMusic);
            if (IsSoundPlaying(s_judasBossMusic)) StopSound(s_judasBossMusic);
            if (IsSoundPlaying(s_pilateBossMusic)) StopSound(s_pilateBossMusic);
            if (!IsSoundPlaying(s_bossMusic)) {
                PlaySound(s_bossMusic);
            }
        }
    } else {
        if (IsSoundPlaying(s_bossMusic)) StopSound(s_bossMusic);
        if (IsSoundPlaying(s_evilCloneMusic)) StopSound(s_evilCloneMusic);
        if (IsSoundPlaying(s_judasBossMusic)) StopSound(s_judasBossMusic);
        if (IsSoundPlaying(s_pilateBossMusic)) StopSound(s_pilateBossMusic);

        if (stageId >= 10) {
            // Mission 4 Stage theme
            if (IsSoundPlaying(s_stageMusicM1)) StopSound(s_stageMusicM1);
            if (IsSoundPlaying(s_stageMusicM2)) StopSound(s_stageMusicM2);
            if (IsSoundPlaying(s_stageMusicM3)) StopSound(s_stageMusicM3);
            if (!IsSoundPlaying(s_stageMusicM4)) {
                PlaySound(s_stageMusicM4);
            }
        } else if (stageId >= 7) {
            // Mission 3 Stage theme
            if (IsSoundPlaying(s_stageMusicM1)) StopSound(s_stageMusicM1);
            if (IsSoundPlaying(s_stageMusicM2)) StopSound(s_stageMusicM2);
            if (IsSoundPlaying(s_stageMusicM4)) StopSound(s_stageMusicM4);
            if (!IsSoundPlaying(s_stageMusicM3)) {
                PlaySound(s_stageMusicM3);
            }
        } else if (stageId >= 4) {
            // Mission 2 Stage theme
            if (IsSoundPlaying(s_stageMusicM1)) StopSound(s_stageMusicM1);
            if (IsSoundPlaying(s_stageMusicM3)) StopSound(s_stageMusicM3);
            if (IsSoundPlaying(s_stageMusicM4)) StopSound(s_stageMusicM4);
            if (!IsSoundPlaying(s_stageMusicM2)) {
                PlaySound(s_stageMusicM2);
            }
        } else {
            // Mission 1 Stage theme
            if (IsSoundPlaying(s_stageMusicM2)) StopSound(s_stageMusicM2);
            if (IsSoundPlaying(s_stageMusicM3)) StopSound(s_stageMusicM3);
            if (IsSoundPlaying(s_stageMusicM4)) StopSound(s_stageMusicM4);
            if (!IsSoundPlaying(s_stageMusicM1)) {
                PlaySound(s_stageMusicM1);
            }
        }
    }
}

