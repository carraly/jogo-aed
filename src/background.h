#ifndef BACKGROUND_H
#define BACKGROUND_H

#include <stdbool.h>
#include "raylib.h"
#include "game.h"
#include "assets.h"

#define MAX_HOUSES 32
#define HOUSE_VARIANTS 4
#define HOUSE_SPAWN_MARGIN 50

#define HOUSE_TOUCH_CHANCE 50
#define HOUSE_GAP_MIN_NATIVE 4
#define HOUSE_GAP_MAX_NATIVE 20

#define RAIL_CHANCE 70
#define RAIL_BELOW_WINDOW 11
#define GROUND_STRIPE_WIDTH 80

typedef struct {
    int variant;
    int setNumber;
    float x;
    float y;
    bool hasRail;
    int railStyle;
} House;

typedef struct {
    House houses[MAX_HOUSES];
    int count;
    float rightEdge;
    float scrollX;
    int lastVariant;
} Background;

void BackgroundReset(Background *bg);
void BackgroundUpdate(Background *bg, float dt, float speed);
void BackgroundDrawHouses(const Background *bg, const Assets *assets);
void BackgroundDrawGround(const Background *bg);

#endif
