#ifndef ASSETS_H
#define ASSETS_H

#include "raylib.h"

#define ART_SCALE 2
#define ASSET_COUNT 4

typedef struct {
    Texture2D house[ASSET_COUNT];
    Texture2D windowSmall[ASSET_COUNT];
    Texture2D windowMedium[ASSET_COUNT];
    Texture2D door[ASSET_COUNT];
    Texture2D railSmall[ASSET_COUNT];
    Texture2D railMedium[ASSET_COUNT];
    Texture2D railBig[ASSET_COUNT];
    Texture2D playerRun;
    Texture2D playerIdle;
    Texture2D obstacle1;
    Texture2D obstacleTall;
    Texture2D obstacleLong;
} Assets;

void AssetsLoad(Assets *assets);
void AssetsUnload(Assets *assets);

#endif
