#ifndef OBSTACLES_H
#define OBSTACLES_H

#include <stdbool.h>
#include "raylib.h"
#include "game.h"

#define WAVE_SIZE 15
#define WAVE_SMALL 5
#define WAVE_TALL 5
#define WAVE_AIR 5

#define OBS_SMALL_WIDTH 40
#define OBS_SMALL_HEIGHT 40
#define OBS_TALL_WIDTH 36
#define OBS_TALL_HEIGHT 80
#define OBS_AIR_WIDTH 40
#define OBS_AIR_HEIGHT 100
#define OBS_AIR_TOP (GROUND_Y - 145)

typedef enum {
    OBS_SMALL,
    OBS_TALL,
    OBS_AIR
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
bool ObstaclesCollide(const ObstacleSystem *obs, Rectangle box);
void ObstaclesDraw(const ObstacleSystem *obs);

#endif
