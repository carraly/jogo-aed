#include "obstacles.h"
#include <stdlib.h>

static const Color OBSTACLE_COLORS[3] = {
    (Color){ 225, 45, 35, 255 },
    (Color){ 115, 45, 170, 255 },
    (Color){ 250, 205, 30, 255 }
};

static const Color OBSTACLE_OUTLINE = (Color){ 25, 25, 25, 255 };

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
            node->w = OBS_SMALL_WIDTH;
            node->h = OBS_SMALL_HEIGHT;
            node->y = GROUND_Y - OBS_SMALL_HEIGHT;
            break;
        case OBS_TALL:
            node->w = OBS_TALL_WIDTH;
            node->h = OBS_TALL_HEIGHT;
            node->y = GROUND_Y - OBS_TALL_HEIGHT;
            break;
        case OBS_AIR:
            node->w = OBS_AIR_WIDTH;
            node->h = OBS_AIR_HEIGHT;
            node->y = OBS_AIR_TOP;
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
}

void ObstaclesFree(ObstacleSystem *obs)
{
    FreeList(&obs->pending);
    FreeList(&obs->active);
}

void ObstaclesReset(ObstacleSystem *obs)
{
    ObstaclesFree(obs);
    GenerateWave(obs);
    SpawnNext(obs);
}

void ObstaclesUpdate(ObstacleSystem *obs, float dt, float speed)
{
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

bool ObstaclesCollide(const ObstacleSystem *obs, Rectangle box)
{
    for (ObstacleNode *node = obs->active.head; node; node = node->next)
    {
        Rectangle rect = { node->x, node->y, node->w, node->h };
        if (CheckCollisionRecs(box, rect)) return true;
    }
    return false;
}

void ObstaclesDraw(const ObstacleSystem *obs)
{
    for (ObstacleNode *node = obs->active.head; node; node = node->next)
    {
        Rectangle rect = { node->x, node->y, node->w, node->h };

        DrawRectangleRec(rect, OBSTACLE_COLORS[node->type]);
        DrawRectangleLinesEx(rect, 3, OBSTACLE_OUTLINE);
    }
}
