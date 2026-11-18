#include "obstacles.h"
#include <math.h>
#include <stdlib.h>

#ifndef DEBUG_HITBOXES
#define DEBUG_HITBOXES 0
#endif

typedef struct {
    float x, y, w, h;
} HitboxDef;

static const HitboxDef OBSTACLE_HITBOXES[OBS_TYPE_COUNT] = {
    { 3.0f, 3.0f, 30.0f, 23.0f },
    { 3.0f, 3.0f, 28.0f, 35.0f },
    { 0.0f, 0.0f, 0.0f, 0.0f },
    { 4.0f, 4.0f, 51.0f, 16.0f }
};

static void ListAppend(ObstacleList *list, ObstacleNode *node)
{
    node->prev = list->tail;
    node->next = NULL;
    if (list->tail) list->tail->next = node;
    else list->head = node;
    list->tail = node;
    list->count++;
}

static void ListPrepend(ObstacleList *list, ObstacleNode *node)
{
    node->prev = NULL;
    node->next = list->head;
    if (list->head) list->head->prev = node;
    else list->tail = node;
    list->head = node;
    list->count++;
}

static void ListInsertAfter(ObstacleList *list, ObstacleNode *position, ObstacleNode *node)
{
    node->prev = position;
    node->next = position->next;
    if (position->next) position->next->prev = node;
    else list->tail = node;
    position->next = node;
    list->count++;
}

static void ListRemove(ObstacleList *list, ObstacleNode *node)
{
    if (node->prev) node->prev->next = node->next;
    else list->head = node->next;
    if (node->next) node->next->prev = node->prev;
    else list->tail = node->prev;
    list->count--;
}

static void FreeList(ObstacleList *list)
{
    ObstacleNode *node = list->head;
    while (node)
    {
        ObstacleNode *next = node->next;
        free(node);
        node = next;
    }
    list->head = NULL;
    list->tail = NULL;
    list->count = 0;
}

static ObstacleNode *NewObstacle(ObstacleType type)
{
    ObstacleNode *node = malloc(sizeof *node);
    node->sortKey = GetRandomValue(0, 9999);
    node->animOffset = GetRandomValue(0, AIR_FRAMES - 1);
    node->type = type;
    node->x = 0.0f;
    node->y = 0.0f;
    node->w = 0.0f;
    node->h = 0.0f;
    node->prev = NULL;
    node->next = NULL;
    return node;
}

static void InsertionSortAscending(ObstacleList *list)
{
    ObstacleNode *node = list->head ? list->head->next : NULL;

    while (node)
    {
        ObstacleNode *next = node->next;
        ObstacleNode *position = node->prev;

        while (position && position->sortKey > node->sortKey)
            position = position->prev;

        if (position != node->prev)
        {
            ListRemove(list, node);
            if (position) ListInsertAfter(list, position, node);
            else ListPrepend(list, node);
        }

        node = next;
    }
}

static void GenerateWave(ObstacleSystem *obs)
{
    for (int i = 0; i < WAVE_SMALL; i++)
        ListAppend(&obs->pending, NewObstacle(OBS_SMALL));
    for (int i = 0; i < WAVE_TALL; i++)
        ListAppend(&obs->pending, NewObstacle(OBS_TALL));
    for (int i = 0; i < WAVE_AIR; i++)
        ListAppend(&obs->pending, NewObstacle(OBS_AIR));
    for (int i = 0; i < WAVE_LONG; i++)
        ListAppend(&obs->pending, NewObstacle(OBS_LONG));

    InsertionSortAscending(&obs->pending);
}

static void SpawnNext(ObstacleSystem *obs)
{
    if (!obs->pending.head) GenerateWave(obs);

    ObstacleNode *node = obs->pending.head;
    ListRemove(&obs->pending, node);

    node->x = SCREEN_WIDTH;

    switch (node->type)
    {
        case OBS_SMALL:
            node->w = OBS_SMALL_SPRITE_WIDTH;
            node->h = OBS_SMALL_SPRITE_HEIGHT;
            node->y = GROUND_Y - OBS_SMALL_SPRITE_HEIGHT;
            break;
        case OBS_TALL:
            node->w = OBS_TALL_SPRITE_WIDTH;
            node->h = OBS_TALL_SPRITE_HEIGHT;
            node->y = GROUND_Y - OBS_TALL_SPRITE_HEIGHT;
            break;
        case OBS_AIR:
            node->w = OBS_AIR_WIDTH;
            node->h = OBS_AIR_HEIGHT;
            node->y = OBS_AIR_TOP;
            break;
        case OBS_LONG:
            node->w = OBS_LONG_SPRITE_WIDTH;
            node->h = OBS_LONG_SPRITE_HEIGHT;
            node->y = GROUND_Y - OBS_LONG_SPRITE_HEIGHT;
            break;
        default:
            break;
    }

    ListAppend(&obs->active, node);

    obs->lastObstacleWidth = node->w;
    obs->gapFactor = GAP_FACTOR_MIN +
        GetRandomValue(0, (int)((GAP_FACTOR_MAX - GAP_FACTOR_MIN) * 1000.0f)) / 1000.0f;
    obs->distSinceSpawn = 0.0f;

    if (!obs->pending.head) GenerateWave(obs);
}

void ObstaclesInit(ObstacleSystem *obs)
{
    obs->pending = (ObstacleList){ 0 };
    obs->active = (ObstacleList){ 0 };
    obs->distSinceSpawn = 0.0f;
    obs->lastObstacleWidth = 0.0f;
    obs->gapFactor = GAP_FACTOR_MIN;
    obs->airTime = 0.0f;
}

void ObstaclesFree(ObstacleSystem *obs)
{
    FreeList(&obs->pending);
    FreeList(&obs->active);
}

void ObstaclesReset(ObstacleSystem *obs)
{
    ObstaclesFree(obs);
    obs->airTime = 0.0f;
    GenerateWave(obs);
    SpawnNext(obs);
}

void ObstaclesUpdate(ObstacleSystem *obs, float dt, float speed)
{
    obs->airTime += dt;
    obs->distSinceSpawn += speed * dt;

    float gap = obs->lastObstacleWidth + speed * GAP_TIME * obs->gapFactor;
    if (obs->distSinceSpawn >= gap) SpawnNext(obs);

    ObstacleNode *node = obs->active.head;
    while (node)
    {
        ObstacleNode *next = node->next;
        node->x -= speed * dt;

        if (node->x + node->w < 0.0f)
        {
            ListRemove(&obs->active, node);
            free(node);
        }

        node = next;
    }
}

Rectangle GetObstacleHitbox(const ObstacleNode *node)
{
    if (node->type == OBS_AIR)
    {
        return (Rectangle){
            node->x + AIR_HITBOX_OFFSET_X,
            GROUND_Y - AIR_HITBOX_TOP,
            AIR_HITBOX_WIDTH,
            AIR_HITBOX_TOP - AIR_HITBOX_BOTTOM
        };
    }

    const HitboxDef *hitbox = &OBSTACLE_HITBOXES[node->type];
    return (Rectangle){
        node->x + hitbox->x * ART_SCALE,
        node->y + hitbox->y * ART_SCALE,
        hitbox->w * ART_SCALE,
        hitbox->h * ART_SCALE
    };
}

bool ObstaclesCollide(const ObstacleSystem *obs, Rectangle box)
{
    for (ObstacleNode *node = obs->active.head; node; node = node->next)
    {
        if (CheckCollisionRecs(box, GetObstacleHitbox(node))) return true;
    }
    return false;
}

void ObstaclesDraw(const ObstacleSystem *obs, const Assets *assets)
{
    int airFrame = (int)(obs->airTime * AIR_FPS) % AIR_FRAMES;

    for (ObstacleNode *node = obs->active.head; node; node = node->next)
    {
        if (node->type == OBS_AIR)
        {
            int frame = (airFrame + node->animOffset) % AIR_FRAMES;

            Rectangle source = {
                (float)(frame * AIR_FRAME_SIZE),
                0.0f,
                AIR_FRAME_SIZE,
                AIR_FRAME_SIZE
            };
            Rectangle dest = {
                roundf(node->x),
                roundf(node->y),
                node->w,
                node->h
            };

            DrawTexturePro(assets->obstacleAir, source, dest, (Vector2){ 0.0f, 0.0f }, 0.0f, WHITE);
        }
        else
        {
            Texture2D texture = assets->obstacle1;
            if (node->type == OBS_TALL) texture = assets->obstacleTall;
            else if (node->type == OBS_LONG) texture = assets->obstacleLong;

            DrawTextureEx(
                texture,
                (Vector2){ roundf(node->x), roundf(node->y) },
                0.0f,
                ART_SCALE,
                WHITE
            );
        }

#if DEBUG_HITBOXES
        DrawRectangleLinesEx(GetObstacleHitbox(node), 1.0f, RED);
#endif
    }
}
