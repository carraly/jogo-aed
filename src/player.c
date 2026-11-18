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
    if (!player->onGround)
    {
        if (downHeld)
            gravity *= FAST_FALL_MULT;
        else if (jumpHeld && player->vy > -60.0f)
            gravity *= 0.55f;
    }

    player->vy += gravity * dt;
    player->y += player->vy * dt;

    if (player->y >= GROUND_Y)
    {
        player->y = GROUND_Y;
        player->vy = 0.0f;
        player->onGround = true;
    }

    if (player->onGround && !player->ducking)
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
    Texture2D texture;
    Rectangle source = { 0.0f, 0.0f, PLAYER_SPRITE_SIZE, PLAYER_SPRITE_SIZE };
    float feetPad;

    if (player->ducking)
    {
        texture = assets->playerSlide;
        feetPad = SLIDE_FEET_PAD * ART_SCALE;
    }
    else if (!player->onGround)
    {
        texture = assets->playerJump;
        feetPad = JUMP_FEET_PAD * ART_SCALE;
    }
    else
    {
        texture = assets->playerRun;
        feetPad = RUN_FEET_PAD * ART_SCALE;
        source.x = (float)(player->frame * PLAYER_SPRITE_SIZE);
    }

    Rectangle dest = {
        roundf((float)PLAYER_X),
        roundf(player->y - PLAYER_DRAW_SIZE + feetPad),
        PLAYER_DRAW_SIZE,
        PLAYER_DRAW_SIZE
    };

    DrawTexturePro(texture, source, dest, (Vector2){ 0.0f, 0.0f }, 0.0f, WHITE);
}