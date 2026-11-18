#include "background.h"
#include "assets.h"
#include <math.h>
#define SKY_SCROLL_SPEED 15.0f // Pixels virtuais por segundo (bem devagar)

typedef struct
{
    SlotKind kind;
    int x, y;
} HouseSlot;

typedef struct
{
    int width, height;
    int wallX, wallY, wallW, wallH;
    int slotCount;
    HouseSlot slots[8];
    int hasRailRow;
    int railFirstSlot;
    int railWindowCount;
} HouseDef;

static const HouseDef HOUSE_DEFS[HOUSE_VARIANTS] = {
    {64, 108, 2, 14, 60, 94, 6, {{SLOT_WINDOW_MEDIUM, 4, 27}, {SLOT_WINDOW_MEDIUM, 26, 27}, {SLOT_WINDOW_MEDIUM, 47, 27}, {SLOT_WINDOW_SMALL, 4, 76}, {SLOT_DOOR, 23, 76}, {SLOT_WINDOW_SMALL, 47, 76}}, 1, 0, 3},
    {48, 60, 2, 14, 44, 46, 2, {{SLOT_WINDOW_SMALL, 7, 28}, {SLOT_DOOR, 23, 28}}, 0, 0, 0},
    {64, 60, 2, 14, 60, 46, 3, {{SLOT_WINDOW_SMALL, 6, 28}, {SLOT_WINDOW_SMALL, 23, 28}, {SLOT_DOOR, 39, 28}}, 0, 0, 0},
    {48, 108, 2, 14, 44, 94, 4, {{SLOT_WINDOW_MEDIUM, 10, 28}, {SLOT_WINDOW_MEDIUM, 26, 28}, {SLOT_WINDOW_SMALL, 7, 76}, {SLOT_DOOR, 23, 76}}, 1, 0, 2}};

static const int RAIL_WIDTHS[3][4] = {
    {14, 14, 14, 24},
    {30, 30, 30, 40},
    {57, 57, 57, 66}};

static const int RAIL_HEIGHTS[3][4] = {
    {18, 14, 18, 23},
    {18, 14, 18, 22},
    {18, 14, 18, 26}};

static const Color GROUND_COLOR = (Color){140, 130, 190, 255};
static const Color STRIPE_COLOR = (Color){132, 122, 182, 255};

static const Color WALL_COLORS[] = {
    {241, 34, 34, 255},
    {255, 215, 16, 255},
    {22, 132, 37, 255},
    {255, 153, 217, 255},
    {240, 150, 30, 255},
    {120, 190, 235, 255},
    {40, 180, 170, 255},
    {190, 160, 230, 255},
    {250, 140, 120, 255},
    {150, 220, 170, 255},
    {250, 235, 190, 255},
    {170, 210, 60, 255},
    {140, 70, 170, 255},
    {210, 100, 60, 255},
    {255, 240, 120, 255},
    {70, 130, 200, 255}};
#define WALL_COLOR_COUNT (int)(sizeof(WALL_COLORS) / sizeof(WALL_COLORS[0]))

// Ponto de extensao para evitar combinacoes ruins entre parede e esquadrias.
static bool WallColorLooksOk(int wallIndex, int setNumber)
{
    (void)wallIndex;
    (void)setNumber;
    return true;
}

static int PickWallColor(int previous, int setNumber)
{
    for (int attempt = 0; attempt < WALL_COLOR_COUNT * 2; attempt++)
    {
        int index = GetRandomValue(0, WALL_COLOR_COUNT - 1);
        if (index != previous && WallColorLooksOk(index, setNumber))
            return index;
    }

    return (previous + 1) % WALL_COLOR_COUNT;
}

static void SpawnHouse(Background *bg)
{
    if (bg->count >= MAX_HOUSES)
        return;

    int variant = GetRandomValue(0, HOUSE_VARIANTS - 1);
    if (variant == bg->lastVariant)
        variant = (variant + 1) % HOUSE_VARIANTS;
    bg->lastVariant = variant;

    const HouseDef *def = &HOUSE_DEFS[variant];
    House *house = &bg->houses[bg->count];

    house->variant = variant;
    house->setNumber = GetRandomValue(1, ASSET_COUNT);
    house->wallColor = PickWallColor(
        bg->count > 0 ? bg->houses[bg->count - 1].wallColor : -1,
        house->setNumber);

    house->x = bg->rightEdge;
    house->y = GROUND_Y - def->height * ART_SCALE;
    house->hasRail = def->hasRailRow && GetRandomValue(1, 100) <= RAIL_CHANCE;
    house->railStyle = GetRandomValue(1, ASSET_COUNT);

    for (int i = 0; i < def->slotCount; i++)
    {
        if (def->slots[i].kind == SLOT_DOOR)
        {
            house->slotKinds[i] = SLOT_DOOR;
            continue;
        }

        bool sameFloorFound = false;
        for (int j = 0; j < i; j++)
        {
            if (def->slots[j].kind != SLOT_DOOR && def->slots[j].y == def->slots[i].y)
            {
                house->slotKinds[i] = house->slotKinds[j];
                sameFloorFound = true;
                break;
            }
        }

        if (!sameFloorFound)
        {
            int roll = GetRandomValue(0, 2);
            if (roll == 0)
                house->slotKinds[i] = SLOT_WINDOW_SMALL;
            else if (roll == 1)
                house->slotKinds[i] = SLOT_WINDOW_MEDIUM;
            else
                house->slotKinds[i] = SLOT_WINDOW_BIG;
        }

    }

    bg->count++;
    bg->rightEdge = house->x + def->width * ART_SCALE;
}

void BackgroundReset(Background *bg)
{
    bg->count = 0;
    bg->rightEdge = 0.0f;
    bg->scrollX = 0.0f;
    bg->skyScrollX = 0.0f;
    bg->lastVariant = -1;

    while (bg->rightEdge < SCREEN_WIDTH + HOUSE_SPAWN_MARGIN)
        SpawnHouse(bg);
}

void BackgroundUpdate(Background *bg, float dt, float speed)
{
    float shift = speed * dt;
    bg->scrollX += shift;
    bg->skyScrollX += SKY_SCROLL_SPEED * dt;

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

    if (bg->count == 0)
        bg->rightEdge = 0.0f;
    else
        bg->rightEdge -= shift;

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
    case SLOT_WINDOW_BIG:
        return assets->windowBig[setNumber - 1];
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

    DrawTextureEx(assets->house[house->variant], (Vector2){(float)houseX, (float)houseY}, 0.0f, ART_SCALE, WHITE);

    DrawRectangle(
        houseX + def->wallX * ART_SCALE,
        houseY + def->wallY * ART_SCALE,
        def->wallW * ART_SCALE,
        def->wallH * ART_SCALE,
        WALL_COLORS[house->wallColor]);

    for (int i = 0; i < def->slotCount; i++)
    {
        const HouseSlot *slot = &def->slots[i];
        Texture2D texture = SlotTexture(assets, house->slotKinds[i], house->setNumber);

        DrawTextureEx(
            texture,
            (Vector2){(float)(houseX + slot->x * ART_SCALE), (float)(houseY + slot->y * ART_SCALE)},
            0.0f,
            ART_SCALE,
            WHITE);
    }

    if (house->hasRail && def->hasRailRow)
    {
        int sizeIndex = def->railWindowCount - 1;
        if (sizeIndex < 0)
            sizeIndex = 0;
        if (sizeIndex > 2)
            sizeIndex = 2;

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
        int windowHeight = house->slotKinds[firstSlot] == SLOT_WINDOW_MEDIUM ? 25 : 16;
        int railY = windowY + windowHeight + RAIL_BELOW_WINDOW - railHeight;

        Texture2D texture = RailTexture(assets, sizeIndex, house->railStyle);

        DrawTextureEx(
            texture,
            (Vector2){(float)(houseX + railX * ART_SCALE), (float)(houseY + railY * ART_SCALE)},
            0.0f,
            ART_SCALE,
            WHITE);
    }
}

void BackgroundDrawSky(const Background *bg, const Assets *assets)
{
    if (assets->sky.id == 0)
        return;

    int tileWidth = assets->sky.width * ART_SCALE;
    if (tileWidth <= 0)
        return;

    int offset = (int)bg->skyScrollX % tileWidth;

    for (int x = -offset; x < SCREEN_WIDTH; x += tileWidth)
    {
        DrawTextureEx(
            assets->sky,
            (Vector2){(float)x, 0.0f},
            0.0f,
            ART_SCALE,
            WHITE);
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

    DrawRectangle(0, GROUND_Y, SCREEN_WIDTH, 2, (Color){110, 100, 160, 255});
}
