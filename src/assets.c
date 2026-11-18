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

    snprintf(path, sizeof path, "%sassets/character/character-running.png", base);
    LoadPointTexture(&assets->playerRun, path);

    snprintf(path, sizeof path, "%sassets/character/character-idle.png", base);
    LoadPointTexture(&assets->playerIdle, path);

    snprintf(path, sizeof path, "%sassets/character/character-jumping.png", base);
    LoadPointTexture(&assets->playerJump, path);

    snprintf(path, sizeof path, "%sassets/character/character-sliding.png", base);
    LoadPointTexture(&assets->playerSlide, path);

    snprintf(path, sizeof path, "%sassets/obstacles/obstacle1.png", base);
    LoadPointTexture(&assets->obstacle1, path);

    snprintf(path, sizeof path, "%sassets/obstacles/obstacle-tall1.png", base);
    LoadPointTexture(&assets->obstacleTall, path);

    snprintf(path, sizeof path, "%sassets/obstacles/obstacle-long1.png", base);
    LoadPointTexture(&assets->obstacleLong, path);

    snprintf(path, sizeof path, "%sassets/obstacles/obstacle-air1.png", base);
    LoadPointTexture(&assets->obstacleAir, path);
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
    UnloadTexture(assets->obstacleTall);
    UnloadTexture(assets->obstacleLong);
    UnloadTexture(assets->obstacleAir);
    UnloadTexture(assets->playerRun);
    UnloadTexture(assets->playerIdle);
    UnloadTexture(assets->playerJump);
    UnloadTexture(assets->playerSlide);
}
