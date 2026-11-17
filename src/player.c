#include "player.h"

static const Color RUN_COLORS[RUN_FRAMES] = {
    (Color){ 150, 90, 50, 255 },
    (Color){ 175, 105, 60, 255 }
};

static const Color DUCK_COLORS[DUCK_FRAMES] = {
    (Color){ 60, 120, 60, 255 },
    (Color){ 85, 145, 75, 255 }
};

static const Color JUMP_COLOR = (Color){ 190, 130, 80, 255 };

void PlayerReset(Player *player)
{
    player->y = GROUND_Y;
    player->vy = 0.0f;
    player->onGround = true;
    player->ducking = false;
    player->animTimer = 0.0f;
    player->frame = 0;
}

void PlayerUpdate(Player *player, float dt)
{
    bool jumpHeld = IsKeyDown(KEY_UP) || IsKeyDown(KEY_SPACE);
    bool downHeld = IsKeyDown(KEY_DOWN);
    bool jumpPressed = IsKeyPressed(KEY_UP) || IsKeyPressed(KEY_SPACE);

    if (jumpPressed && player->onGround && !downHeld)
    {
        player->vy = -JUMP_VELOCITY;
        player->onGround = false;
    }

    if (!player->onGround && player->vy < 0.0f && !jumpHeld && player->vy < -JUMP_CUT_VELOCITY)
        player->vy = -JUMP_CUT_VELOCITY;

    player->ducking = downHeld;

    float gravity = GRAVITY;
    if (!player->onGround && downHeld) gravity *= FAST_FALL_MULT;

    player->vy += gravity * dt;
    player->y += player->vy * dt;

    if (player->y >= GROUND_Y)
    {
        player->y = GROUND_Y;
        player->vy = 0.0f;
        player->onGround = true;
    }

    player->animTimer += dt;
    if (player->animTimer >= ANIM_FRAME_TIME)
    {
        player->animTimer -= ANIM_FRAME_TIME;
        player->frame++;
    }

    if (!player->onGround) player->frame = 0;
}

Rectangle PlayerHitbox(const Player *player)
{
    float height = player->ducking ? HITBOX_DUCK_HEIGHT : HITBOX_STAND_HEIGHT;
    return (Rectangle){
        PLAYER_X + HITBOX_OFFSET_X,
        player->y - height,
        HITBOX_WIDTH,
        height
    };
}

void PlayerDraw(const Player *player)
{
    float height = player->ducking ? HITBOX_DUCK_HEIGHT : PLAYER_DRAW_SIZE;
    Color color;

    if (!player->onGround) color = JUMP_COLOR;
    else if (player->ducking) color = DUCK_COLORS[player->frame % DUCK_FRAMES];
    else color = RUN_COLORS[player->frame % RUN_FRAMES];

    DrawRectangle(PLAYER_X, (int)(player->y - height), PLAYER_DRAW_SIZE, (int)height, color);
}
