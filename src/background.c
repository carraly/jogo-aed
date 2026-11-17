#include "background.h"

static const Color LAYOUT_COLORS[HOUSE_VARIANTS] = {
    (Color){ 205, 160, 120, 255 },
    (Color){ 180, 140, 150, 255 },
    (Color){ 160, 175, 140, 255 }
};

static const Color WINDOW_COLORS[WINDOW_VARIANTS] = {
    (Color){ 80, 130, 170, 255 },
    (Color){ 110, 160, 190, 255 },
    (Color){ 90, 110, 140, 255 }
};

static const Color DOOR_COLORS[DOOR_VARIANTS] = {
    (Color){ 110, 70, 45, 255 },
    (Color){ 140, 90, 55, 255 },
    (Color){ 90, 60, 50, 255 }
};

static const Color GROUND_COLOR = (Color){ 215, 185, 130, 255 };
static const Color STRIPE_COLOR = (Color){ 190, 160, 105, 255 };

static void SpawnHouse(Background *bg)
{
    if (bg->count >= MAX_HOUSES) return;

    House *house = &bg->houses[bg->count];

    house->width = (float)GetRandomValue(HOUSE_MIN_WIDTH, HOUSE_MAX_WIDTH);
    house->height = (float)GetRandomValue(HOUSE_MIN_HEIGHT, HOUSE_MAX_HEIGHT);
    house->x = bg->rightEdge + GetRandomValue(0, HOUSE_GAP_MAX);
    house->layout = GetRandomValue(0, HOUSE_VARIANTS - 1);
    house->window = GetRandomValue(0, WINDOW_VARIANTS - 1);
    house->door = GetRandomValue(0, DOOR_VARIANTS - 1);

    bg->count++;
    bg->rightEdge = house->x + house->width;
}

void BackgroundReset(Background *bg)
{
    bg->count = 0;
    bg->rightEdge = 0.0f;
    bg->scrollX = 0.0f;

    while (bg->rightEdge < SCREEN_WIDTH + HOUSE_SPAWN_MARGIN)
        SpawnHouse(bg);
}

void BackgroundUpdate(Background *bg, float dt, float speed)
{
    float shift = speed * dt;
    bg->scrollX += shift;

    for (int i = 0; i < bg->count;)
    {
        bg->houses[i].x -= shift;

        if (bg->houses[i].x + bg->houses[i].width < 0.0f)
        {
            for (int j = i; j < bg->count - 1; j++)
                bg->houses[j] = bg->houses[j + 1];
            bg->count--;
        }
        else
        {
            i++;
        }
    }

    if (bg->count == 0) bg->rightEdge = 0.0f;
    else bg->rightEdge -= shift;

    while (bg->rightEdge < SCREEN_WIDTH + HOUSE_SPAWN_MARGIN)
        SpawnHouse(bg);
}

void BackgroundDrawHouses(const Background *bg)
{
    for (int i = 0; i < bg->count; i++)
    {
        const House *house = &bg->houses[i];
        float top = GROUND_Y - house->height;

        DrawRectangle(
            (int)house->x,
            (int)top,
            (int)house->width,
            (int)house->height,
            LAYOUT_COLORS[house->layout]
        );

        float windowX = house->x + house->width * 0.25f - WINDOW_SIZE / 2.0f;
        float windowY = top + house->height * 0.35f - WINDOW_SIZE / 2.0f;
        DrawRectangle((int)windowX, (int)windowY, WINDOW_SIZE, WINDOW_SIZE, WINDOW_COLORS[house->window]);

        float doorX = house->x + house->width * 0.70f - DOOR_WIDTH / 2.0f;
        DrawRectangle((int)doorX, GROUND_Y - DOOR_HEIGHT, DOOR_WIDTH, DOOR_HEIGHT, DOOR_COLORS[house->door]);
    }
}

void BackgroundDrawGround(const Background *bg)
{
    DrawRectangle(0, GROUND_Y, SCREEN_WIDTH, SCREEN_HEIGHT - GROUND_Y, GROUND_COLOR);

    int offset = (int)bg->scrollX % GROUND_STRIPE_WIDTH;
    for (int x = -offset; x < SCREEN_WIDTH; x += GROUND_STRIPE_WIDTH)
        DrawRectangle(x, GROUND_Y, GROUND_STRIPE_WIDTH / 2, SCREEN_HEIGHT - GROUND_Y, STRIPE_COLOR);

    DrawRectangle(0, GROUND_Y, SCREEN_WIDTH, 2, (Color){ 120, 85, 50, 255 });
}
