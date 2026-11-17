#include "background.h"
#include <math.h>

typedef enum {
    SLOT_WINDOW_SMALL,
    SLOT_WINDOW_MEDIUM,
    SLOT_DOOR
} SlotKind;

typedef struct {
    SlotKind kind;
    int x, y;
} HouseSlot;

typedef struct {
    int width, height;
    int slotCount;
    HouseSlot slots[8];
    int hasRailRow;
    int railFirstSlot;
    int railWindowCount;
} HouseDef;

static const HouseDef HOUSE_DEFS[HOUSE_VARIANTS] = {
    {
        64, 108, 6,
        {
            { SLOT_WINDOW_MEDIUM, 4, 27 },
            { SLOT_WINDOW_MEDIUM, 26, 27 },
            { SLOT_WINDOW_MEDIUM, 47, 27 },
            { SLOT_WINDOW_SMALL, 4, 76 },
            { SLOT_DOOR, 23, 76 },
            { SLOT_WINDOW_SMALL, 47, 76 }
        },
        1, 0, 3
    },
    {
        48, 60, 2,
        {
            { SLOT_WINDOW_SMALL, 7, 28 },
            { SLOT_DOOR, 23, 28 }
        },
        0, 0, 0
    },
    {
        64, 60, 3,
        {
            { SLOT_WINDOW_SMALL, 6, 28 },
            { SLOT_WINDOW_SMALL, 23, 28 },
            { SLOT_DOOR, 39, 28 }
        },
        0, 0, 0
    },
    {
        48, 108, 4,
        {
            { SLOT_WINDOW_MEDIUM, 10, 28 },
            { SLOT_WINDOW_MEDIUM, 26, 28 },
            { SLOT_WINDOW_SMALL, 7, 76 },
            { SLOT_DOOR, 23, 76 }
        },
        1, 0, 2
    }
};

static const int RAIL_WIDTHS[3][4] = {
    { 14, 14, 14, 24 },
    { 30, 30, 30, 40 },
    { 57, 57, 57, 66 }
};

static const int RAIL_HEIGHTS[3][4] = {
    { 18, 14, 18, 23 },
    { 18, 14, 18, 22 },
    { 18, 14, 18, 26 }
};

static const Color GROUND_COLOR = (Color){ 140, 130, 190, 255 };
static const Color STRIPE_COLOR = (Color){ 132, 122, 182, 255 };

static void SpawnHouse(Background *bg)
{
    if (bg->count >= MAX_HOUSES) return;

    int variant = GetRandomValue(0, HOUSE_VARIANTS - 1);
    if (variant == bg->lastVariant)
        variant = (variant + 1) % HOUSE_VARIANTS;
    bg->lastVariant = variant;

    const HouseDef *def = &HOUSE_DEFS[variant];
    House *house = &bg->houses[bg->count];

    house->variant = variant;
    house->setNumber = GetRandomValue(1, ASSET_COUNT);

    int gap = 0;
    if (bg->count > 0 && GetRandomValue(1, 100) > HOUSE_TOUCH_CHANCE)
        gap = GetRandomValue(HOUSE_GAP_MIN_NATIVE, HOUSE_GAP_MAX_NATIVE) * ART_SCALE;

    house->x = bg->rightEdge + gap;
    house->y = GROUND_Y - def->height * ART_SCALE;
    house->hasRail = def->hasRailRow && GetRandomValue(1, 100) <= RAIL_CHANCE;
    house->railStyle = GetRandomValue(1, ASSET_COUNT);

    bg->count++;
    bg->rightEdge = house->x + def->width * ART_SCALE;
}

void BackgroundReset(Background *bg)
{
    bg->count = 0;
    bg->rightEdge = 0.0f;
    bg->scrollX = 0.0f;
    bg->lastVariant = -1;

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

        if (bg->houses[i].x + HOUSE_DEFS[bg->houses[i].variant].width * ART_SCALE < 0.0f)
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

static Texture2D SlotTexture(const Assets *assets, SlotKind kind, int setNumber)
{
    switch (kind)
    {
        case SLOT_WINDOW_SMALL:
            return assets->windowSmall[setNumber - 1];
        case SLOT_WINDOW_MEDIUM:
            return assets->windowMedium[setNumber - 1];
        default:
            return assets->door[setNumber - 1];
    }
}

static Texture2D RailTexture(const Assets *assets, int sizeIndex, int style)
{
    switch (sizeIndex)
    {
        case 0:
            return assets->railSmall[style - 1];
        case 1:
            return assets->railMedium[style - 1];
        default:
            return assets->railBig[style - 1];
    }
}

static void DrawHouse(const House *house, const Assets *assets)
{
    const HouseDef *def = &HOUSE_DEFS[house->variant];
    int houseX = (int)roundf(house->x);
    int houseY = (int)roundf(house->y);

    DrawTextureEx(assets->house[house->variant], (Vector2){ (float)houseX, (float)houseY }, 0.0f, ART_SCALE, WHITE);

    for (int i = 0; i < def->slotCount; i++)
    {
        const HouseSlot *slot = &def->slots[i];
        Texture2D texture = SlotTexture(assets, slot->kind, house->setNumber);

        DrawTextureEx(
            texture,
            (Vector2){ (float)(houseX + slot->x * ART_SCALE), (float)(houseY + slot->y * ART_SCALE) },
            0.0f,
            ART_SCALE,
            WHITE
        );
    }

    if (house->hasRail && def->hasRailRow)
    {
        int sizeIndex = def->railWindowCount - 1;
        if (sizeIndex < 0) sizeIndex = 0;
        if (sizeIndex > 2) sizeIndex = 2;

        int style = house->railStyle - 1;
        int railWidth = RAIL_WIDTHS[sizeIndex][style];
        int railHeight = RAIL_HEIGHTS[sizeIndex][style];

        int firstSlot = def->railFirstSlot;
        int lastSlot = firstSlot + def->railWindowCount - 1;
        int groupLeft = def->slots[firstSlot].x;
        int groupRight = def->slots[lastSlot].x + 12;
        int centerX = (groupLeft + groupRight) / 2;
        int railX = centerX - railWidth / 2;

        int windowY = def->slots[firstSlot].y;
        int windowHeight = def->slots[firstSlot].kind == SLOT_WINDOW_MEDIUM ? 25 : 16;
        int railY = windowY + windowHeight + RAIL_BELOW_WINDOW - railHeight;

        Texture2D texture = RailTexture(assets, sizeIndex, house->railStyle);

        DrawTextureEx(
            texture,
            (Vector2){ (float)(houseX + railX * ART_SCALE), (float)(houseY + railY * ART_SCALE) },
            0.0f,
            ART_SCALE,
            WHITE
        );
    }
}

void BackgroundDrawHouses(const Background *bg, const Assets *assets)
{
    for (int i = 0; i < bg->count; i++)
        DrawHouse(&bg->houses[i], assets);
}

void BackgroundDrawGround(const Background *bg)
{
    DrawRectangle(0, GROUND_Y, SCREEN_WIDTH, SCREEN_HEIGHT - GROUND_Y, GROUND_COLOR);

    int offset = (int)bg->scrollX % GROUND_STRIPE_WIDTH;
    for (int x = -offset; x < SCREEN_WIDTH; x += GROUND_STRIPE_WIDTH)
        DrawRectangle(x, GROUND_Y, GROUND_STRIPE_WIDTH / 2, SCREEN_HEIGHT - GROUND_Y, STRIPE_COLOR);

    DrawRectangle(0, GROUND_Y, SCREEN_WIDTH, 2, (Color){ 110, 100, 160, 255 });
}
