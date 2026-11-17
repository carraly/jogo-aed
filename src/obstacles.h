#ifndef OBSTACLES_H
#define OBSTACLES_H

#include <stdbool.h>
#include "raylib.h"
#include "game.h"
#include "assets.h"

#define WAVE_SIZE 15
#define WAVE_SMALL 4
#define WAVE_TALL 4
#define WAVE_AIR 4
#define WAVE_LONG 3

#if WAVE_SMALL + WAVE_TALL + WAVE_AIR + WAVE_LONG != WAVE_SIZE
#error "A composicao da onda deve somar WAVE_SIZE"
#endif

#define OBS_SMALL_SPRITE_WIDTH (38 * ART_SCALE)
#define OBS_SMALL_SPRITE_HEIGHT (27 * ART_SCALE)
#define OBS_TALL_SPRITE_WIDTH (34 * ART_SCALE)
#define OBS_TALL_SPRITE_HEIGHT (39 * ART_SCALE)
#define OBS_LONG_SPRITE_WIDTH (59 * ART_SCALE)
#define OBS_LONG_SPRITE_HEIGHT (21 * ART_SCALE)
#define OBS_AIR_WIDTH 40
#define OBS_AIR_HEIGHT 100
#define OBS_AIR_TOP (GROUND_Y - 145)

typedef enum {
    OBS_SMALL,
    OBS_TALL,
    OBS_AIR,
    OBS_LONG,
    OBS_TYPE_COUNT
} ObstacleType;

typedef struct ObstacleNode {
    int sortKey;
    ObstacleType type;
    float x, y, w, h;
    struct ObstacleNode *prev, *next;
} ObstacleNode;

typedef struct {
    ObstacleNode *head, *tail;
    int count;
} ObstacleList;

typedef struct {
    ObstacleList pending;
    ObstacleList active;
    float distSinceSpawn;
    float lastObstacleWidth;
    float gapFactor;
} ObstacleSystem;

void ObstaclesInit(ObstacleSystem *obs);
void ObstaclesFree(ObstacleSystem *obs);
void ObstaclesReset(ObstacleSystem *obs);
void ObstaclesUpdate(ObstacleSystem *obs, float dt, float speed);
Rectangle GetObstacleHitbox(const ObstacleNode *node);
bool ObstaclesCollide(const ObstacleSystem *obs, Rectangle box);
void ObstaclesDraw(const ObstacleSystem *obs, const Assets *assets);

#endif
