#ifndef BACKGROUND_H
#define BACKGROUND_H

#include "raylib.h"
#include "game.h"

#define MAX_HOUSES 32
#define HOUSE_VARIANTS 3
#define WINDOW_VARIANTS 3
#define DOOR_VARIANTS 3

#define HOUSE_MIN_WIDTH 120
#define HOUSE_MAX_WIDTH 220
#define HOUSE_MIN_HEIGHT 150
#define HOUSE_MAX_HEIGHT 250
#define HOUSE_GAP_MAX 20
#define HOUSE_SPAWN_MARGIN 50

#define WINDOW_SIZE 28
#define DOOR_WIDTH 32
#define DOOR_HEIGHT 56
#define GROUND_STRIPE_WIDTH 80

typedef struct {
    float x;
    float width;
    float height;
    int layout;
    int window;
    int door;
} House;

typedef struct {
    House houses[MAX_HOUSES];
    int count;
    float rightEdge;
    float scrollX;
} Background;

void BackgroundReset(Background *bg);
void BackgroundUpdate(Background *bg, float dt, float speed);
void BackgroundDrawHouses(const Background *bg);
void BackgroundDrawGround(const Background *bg);

#endif
