#include "raylib.h"

int main(void)
{
    const int screenWidth = 800;
    const int screenHeight = 450;

    InitWindow(screenWidth, screenHeight, "jogo-aed");

    SetTargetFPS(60);

    while (!WindowShouldClose())
    {
        BeginDrawing();

        ClearBackground(RAYWHITE);
        DrawText("jogo-aed com Raylib funcionando!", 190, 200, 20, DARKGREEN);

        EndDrawing();
    }

    CloseWindow();

    return 0;
}
