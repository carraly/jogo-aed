#include "player.h"
#include <math.h>

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
    bool wasOnGround = player->onGround;

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

    if (player->onGround)
    {
        if (!wasOnGround)
        {
            player->animTimer = 0.0f;
            player->frame = 0;
        }

        player->animTimer += dt;
        float frameTime = 1.0f / RUN_FPS;

        while (player->animTimer >= frameTime)
        {
            player->animTimer -= frameTime;
            player->frame = (player->frame + 1) % RUN_FRAMES;
        }
    }
    else
    {
        player->frame = 0;
    }
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

void PlayerDraw(const Player *player, const Assets *assets)
{
    float destWidth = PLAYER_DRAW_SIZE * DUCK_SQUEEZE_X;
    float destHeight = PLAYER_DRAW_SIZE;

    if (player->ducking) destHeight *= DUCK_SQUEEZE_Y;

    float destX = PLAYER_X - (destWidth - PLAYER_DRAW_SIZE) / 2.0f;
    float feetPad = PLAYER_FEET_PAD * ART_SCALE;
    if (player->ducking) feetPad *= DUCK_SQUEEZE_Y;
    float destY = player->y - destHeight + feetPad;

    Rectangle source = {
        (float)(player->frame * PLAYER_SPRITE_SIZE),
        0.0f,
        PLAYER_SPRITE_SIZE,
        PLAYER_SPRITE_SIZE
    };
    Rectangle dest = { roundf(destX), roundf(destY), destWidth, destHeight };

    DrawTexturePro(assets->playerRun, source, dest, (Vector2){ 0.0f, 0.0f }, 0.0f, WHITE);
}
