#ifndef GAME_H
#define GAME_H
#define SIDEWALK_Y 336  // Altura onde terminam as casas e começa o calçadão
#define GROUND_Y   392

#include "raylib.h"

#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 450

#define SPEED_START 170.0f
#define SPEED_ACCEL 4.0f
#define SPEED_MAX 800.0f
#define SCORE_FACTOR 0.05f

#define GAP_TIME 0.75f
#define GAP_FACTOR_MIN 1.0f
#define GAP_FACTOR_MAX 1.6f

#define GAME_OVER_LOCK 0.5f
#define NAME_MAX_LENGTH 10

#define SKY_COLOR (Color){ 200, 225, 245, 255 }
#define OVERLAY_COLOR (Color){ 0, 0, 0, 140 }

typedef enum {
    STATE_MENU,
    STATE_PLAYING,
    STATE_GAME_OVER,
    STATE_NAME_ENTRY,
    STATE_RANKING
} GameState;

#endif
