#ifndef GBA_GAME_H
#define GBA_GAME_H

#include "gba_types.h"
#include "gba_graphics.h"
#include "gba_audio.h"

#define GBA_LEVEL_WIDTH   2200
#define GBA_GROUND_Y      136
#define GBA_UPPER_Y       80
#define GBA_CRATE_Y       108

#define GBA_ARENA_GATE_X  1760
#define GBA_ARENA_MIN_X   1780
#define GBA_ARENA_MAX_X   2160

typedef enum {
    GAME_STATE_TITLE,
    GAME_STATE_PLAYING,
    GAME_STATE_BOSS_INTRO,
    GAME_STATE_BOSS_FIGHT,
    GAME_STATE_VICTORY_ASCENSION,
    GAME_STATE_CONTINUE,
    GAME_STATE_GAME_OVER
} GameState;

typedef struct {
    int x, y;
    int vx, vy;
    bool facingLeft;
    bool onGround;
    bool onOneWay;
    int state;
    int stateTimer;
    int animTimer;
    int walkFrame;
    int hp;
    int shatteredHearts;
    int lives;
    int score;
    int bossHitsTaken;
    bool hasWrath;
    int wrathTimer;
    int invulnTimer;
    bool meleeActive;
    int meleeTimer;
} GbaPlayer;

typedef struct {
    int x, y;
    int vx;
    bool active;
} GbaNail;

typedef struct {
    int x, y;
    int type; // 0=guard, 1=archer, 2=heavy, 3=boss
    int hp;
    int maxHp;
    bool active;
    bool facingLeft;
    int animTimer;
    int animFrame;
    int attackTimer;
    bool isAttacking;
    bool isDucking;
    int hurtTimer;
    int bossState; // 0=stalk, 1=windup, 2=crack, 3=slam, 4=kneel, 5=wither, 6=ash
    int bossStateTimer;
} GbaEnemy;

typedef struct {
    int x, y;
    bool collected;
} GbaBasket;

typedef struct {
    int x, y, w, h;
    bool isOneWay;
} GbaPlatform;

void gba_game_init(void);
void gba_game_update(uint16_t keysDown, uint16_t keysPressed);
void gba_game_draw(void);

#endif // GBA_GAME_H

