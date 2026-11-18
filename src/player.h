#ifndef PLAYER_H
#define PLAYER_H

#include <stdbool.h>
#include "raylib.h"
#include "game.h"
#include "assets.h"

#define PLAYER_X 100
#define PLAYER_SPRITE_SIZE 32
#define PLAYER_SCALE 2
#define PLAYER_DRAW_SIZE (PLAYER_SPRITE_SIZE * PLAYER_SCALE)

#define GRAVITY 2400.0f
#define JUMP_VELOCITY 720.0f
#define JUMP_CUT_VELOCITY 280.0f
#define FAST_FALL_MULT 2.5f

#define HITBOX_WIDTH 36
#define HITBOX_STAND_HEIGHT 56
#define HITBOX_DUCK_HEIGHT 32
#define HITBOX_OFFSET_X 14

#define RUN_FRAMES 8
#define RUN_FPS 8

#define RUN_FEET_PAD 2
#define JUMP_FEET_PAD 5
#define SLIDE_FEET_PAD 5

typedef struct {
    float y;
    float vy;
    bool onGround;
    bool ducking;
    float animTimer;
    int frame;
} Player;

void PlayerReset(Player *player);
void PlayerUpdate(Player *player, float dt);
Rectangle PlayerHitbox(const Player *player);
void PlayerDraw(const Player *player, const Assets *assets);

#endif
