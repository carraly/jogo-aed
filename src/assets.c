#include "assets.h"
#include <stdio.h>

static void LoadPointTexture(Texture2D *texture, const char *path)
{
    *texture = LoadTexture(path);
    SetTextureFilter(*texture, TEXTURE_FILTER_POINT);
}

void AssetsLoad(Assets *assets)
{
    const char *base = GetApplicationDirectory();
    char path[512];

    for (int i = 0; i < ASSET_COUNT; i++)
    {
        snprintf(path, sizeof path, "%sassets/houses/house%d.png", base, i + 1);
        LoadPointTexture(&assets->house[i], path);

        snprintf(path, sizeof path, "%sassets/windows/window-small%d.png", base, i + 1);
        LoadPointTexture(&assets->windowSmall[i], path);

        snprintf(path, sizeof path, "%sassets/windows/window-medium%d.png", base, i + 1);
        LoadPointTexture(&assets->windowMedium[i], path);

        snprintf(path, sizeof path, "%sassets/doors/door%d.png", base, i + 1);
        LoadPointTexture(&assets->door[i], path);

        snprintf(path, sizeof path, "%sassets/rails/rail-small%d.png", base, i + 1);
        LoadPointTexture(&assets->railSmall[i], path);

        snprintf(path, sizeof path, "%sassets/rails/rail-medium%d.png", base, i + 1);
        LoadPointTexture(&assets->railMedium[i], path);

        snprintf(path, sizeof path, "%sassets/rails/rail-big%d.png", base, i + 1);
        LoadPointTexture(&assets->railBig[i], path);
    }

    snprintf(path, sizeof path, "%sassets/obstacles/obstacle1.png", base);
    LoadPointTexture(&assets->obstacle1, path);
}

void AssetsUnload(Assets *assets)
{
    for (int i = 0; i < ASSET_COUNT; i++)
    {
        UnloadTexture(assets->house[i]);
        UnloadTexture(assets->windowSmall[i]);
        UnloadTexture(assets->windowMedium[i]);
        UnloadTexture(assets->door[i]);
        UnloadTexture(assets->railSmall[i]);
        UnloadTexture(assets->railMedium[i]);
        UnloadTexture(assets->railBig[i]);
    }

    UnloadTexture(assets->obstacle1);
}
