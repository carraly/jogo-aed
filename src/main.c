#include "raylib.h"
#include "game.h"
#include "strings.h"
#include "player.h"
#include "obstacles.h"
#include "background.h"
#include "ranking.h"
#include "assets.h"

#include <stdio.h>
#include <string.h>
#include <time.h>

#define MENU_IDLE_X 60
#define MENU_IDLE_SCALE 4.0f

typedef struct {
    GameState state;
    Player player;
    ObstacleSystem obstacles;
    Background background;
    Assets assets;
    RankList ranking;
    float elapsed;
    float score;
    float speed;
    float gameOverTimer;
    float cursorTimer;
    int menuSelection;
    bool rankingAfterRun;
    RankEntry *highlight;
    int highlightPosition;
    char name[NAME_MAX_LENGTH + 1];
    int nameLength;
    bool exitRequested;
} Game;

static void StartRun(Game *game)
{
    game->elapsed = 0.0f;
    game->score = 0.0f;
    game->speed = SPEED_START;

    PlayerReset(&game->player);
    BackgroundReset(&game->background);
    ObstaclesReset(&game->obstacles);

    game->state = STATE_PLAYING;
}

static void OpenRanking(Game *game, bool afterRun)
{
    game->rankingAfterRun = afterRun;
    if (!afterRun)
    {
        game->highlight = NULL;
        game->highlightPosition = 0;
    }
    game->state = STATE_RANKING;
}

static void UpdateMenu(Game *game)
{
    if (IsKeyPressed(KEY_UP))
        game->menuSelection = (game->menuSelection + 2) % 3;
    if (IsKeyPressed(KEY_DOWN))
        game->menuSelection = (game->menuSelection + 1) % 3;

    if (IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_SPACE))
    {
        if (game->menuSelection == 0) StartRun(game);
        else if (game->menuSelection == 1) OpenRanking(game, false);
        else game->exitRequested = true;
    }
}

static void UpdatePlaying(Game *game, float dt)
{
    game->elapsed += dt;
    game->speed = SPEED_START + SPEED_ACCEL * game->elapsed;
    if (game->speed > SPEED_MAX) game->speed = SPEED_MAX;

    game->score += game->speed * dt * SCORE_FACTOR;

    PlayerUpdate(&game->player, dt);
    BackgroundUpdate(&game->background, dt, game->speed);
    ObstaclesUpdate(&game->obstacles, dt, game->speed);

    if (ObstaclesCollide(&game->obstacles, PlayerHitbox(&game->player)))
    {
        game->gameOverTimer = 0.0f;
        game->state = STATE_GAME_OVER;
    }
}

static void UpdateGameOver(Game *game, float dt)
{
    game->gameOverTimer += dt;

    if (game->gameOverTimer >= GAME_OVER_LOCK &&
        (IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_SPACE)))
    {
        game->nameLength = 0;
        game->name[0] = '\0';
        game->cursorTimer = 0.0f;
        game->state = STATE_NAME_ENTRY;
    }
}

static void UpdateNameEntry(Game *game, float dt)
{
    int character;
    while ((character = GetCharPressed()) > 0)
    {
        if (character >= 'a' && character <= 'z') character -= 32;

        bool valid = (character >= 'A' && character <= 'Z') ||
                     (character >= '0' && character <= '9') ||
                     character == '_';

        if (valid && game->nameLength < NAME_MAX_LENGTH)
        {
            game->name[game->nameLength++] = (char)character;
            game->name[game->nameLength] = '\0';
        }
    }

    if (IsKeyPressed(KEY_BACKSPACE) && game->nameLength > 0)
    {
        game->nameLength--;
        game->name[game->nameLength] = '\0';
    }

    game->cursorTimer += dt;
    if (game->cursorTimer >= 1.0f) game->cursorTimer -= 1.0f;

    if (IsKeyPressed(KEY_ENTER))
    {
        const char *name = game->nameLength > 0 ? game->name : FALLBACK_NAME;

        game->highlight = RankingInsert(
            &game->ranking,
            name,
            (int)game->score,
            &game->highlightPosition
        );

        char path[512];
        RankingGetPath(path, sizeof path);
        RankingSave(&game->ranking, path);

        OpenRanking(game, true);
    }
}

static void UpdateRanking(Game *game)
{
    if (IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_SPACE) || IsKeyPressed(KEY_ESCAPE))
        game->state = STATE_MENU;
}

static void DrawScene(const Game *game)
{
    ClearBackground(SKY_COLOR);
    BackgroundDrawHouses(&game->background, &game->assets);
    BackgroundDrawGround(&game->background);
    ObstaclesDraw(&game->obstacles, &game->assets);
    PlayerDraw(&game->player, &game->assets);
}

static void DrawMenu(const Game *game)
{
    ClearBackground(SKY_COLOR);
    BackgroundDrawHouses(&game->background, &game->assets);
    BackgroundDrawGround(&game->background);

    DrawTextureEx(
        game->assets.playerIdle,
        (Vector2){ (float)MENU_IDLE_X, (float)(GROUND_Y - PLAYER_SPRITE_SIZE * MENU_IDLE_SCALE) },
        0.0f,
        MENU_IDLE_SCALE,
        WHITE
    );

    int titleWidth = MeasureText(TITLE_TEXT, 50);
    DrawText(TITLE_TEXT, (SCREEN_WIDTH - titleWidth) / 2, 80, 50, DARKBROWN);

    const char *options[3] = { MENU_PLAY, MENU_RANKING, MENU_EXIT };
    for (int i = 0; i < 3; i++)
    {
        int width = MeasureText(options[i], 30);
        Color color = i == game->menuSelection ? MAROON : DARKGRAY;
        DrawText(options[i], (SCREEN_WIDTH - width) / 2, 210 + i * 50, 30, color);
    }
}

static void DrawPlaying(const Game *game)
{
    DrawScene(game);

    const char *hud = TextFormat(SCORE_TEXT, (int)game->score);
    DrawText(hud, SCREEN_WIDTH - MeasureText(hud, 20) - 20, 20, 20, DARKBROWN);
}

static void DrawGameOver(const Game *game)
{
    DrawRectangle(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, OVERLAY_COLOR);

    const char *title = GAME_OVER_TITLE;
    DrawText(title, (SCREEN_WIDTH - MeasureText(title, 50)) / 2, 140, 50, RAYWHITE);

    const char *score = TextFormat(GAME_OVER_SCORE, (int)game->score);
    DrawText(score, (SCREEN_WIDTH - MeasureText(score, 30)) / 2, 220, 30, RAYWHITE);

    const char *hint = GAME_OVER_HINT;
    DrawText(hint, (SCREEN_WIDTH - MeasureText(hint, 20)) / 2, 290, 20, LIGHTGRAY);
}

static void DrawNameEntry(const Game *game)
{
    DrawRectangle(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, OVERLAY_COLOR);

    const char *prompt = NAME_PROMPT;
    DrawText(prompt, (SCREEN_WIDTH - MeasureText(prompt, 30)) / 2, 120, 30, RAYWHITE);

    const char *score = TextFormat(GAME_OVER_SCORE, (int)game->score);
    DrawText(score, (SCREEN_WIDTH - MeasureText(score, 24)) / 2, 175, 24, RAYWHITE);

    int boxX = 200;
    int boxY = 225;
    int boxWidth = 400;
    int boxHeight = 50;

    DrawRectangle(boxX, boxY, boxWidth, boxHeight, (Color){ 255, 255, 255, 220 });
    DrawRectangleLines(boxX, boxY, boxWidth, boxHeight, DARKGRAY);
    DrawText(game->name, boxX + 15, boxY + 10, 30, BLACK);

    if (game->cursorTimer < 0.5f)
    {
        int cursorX = boxX + 15 + MeasureText(game->name, 30) + 2;
        DrawRectangle(cursorX, boxY + 12, 2, 26, BLACK);
    }

    const char *hint = NAME_HINT;
    DrawText(hint, (SCREEN_WIDTH - MeasureText(hint, 20)) / 2, 310, 20, LIGHTGRAY);
}

static void DrawRanking(const Game *game)
{
    ClearBackground((Color){ 245, 235, 210, 255 });

    const char *title = RANKING_TITLE;
    DrawText(title, (SCREEN_WIDTH - MeasureText(title, 36)) / 2, 25, 36, DARKBROWN);

    if (game->ranking.count == 0)
    {
        const char *empty = RANKING_EMPTY;
        DrawText(empty, (SCREEN_WIDTH - MeasureText(empty, 24)) / 2, 120, 24, DARKGRAY);
    }

    int y = 80;
    int position = 1;
    RankEntry *entry = game->ranking.head;

    for (; entry && position <= 10; entry = entry->next, position++)
    {
        bool highlighted = game->rankingAfterRun && entry == game->highlight;
        const char *line = TextFormat("%d. %s  %d", position, entry->name, entry->score);
        DrawText(line, 250, y, 24, highlighted ? MAROON : DARKGRAY);
        y += 28;
    }

    if (game->rankingAfterRun && game->highlight && game->highlightPosition > 10)
    {
        const char *line = TextFormat(
            "%d. %s  %d",
            game->highlightPosition,
            game->highlight->name,
            game->highlight->score
        );
        DrawText(line, 250, y, 24, MAROON);
    }

    const char *back = RANKING_BACK;
    DrawText(back, (SCREEN_WIDTH - MeasureText(back, 20)) / 2, 410, 20, DARKGRAY);
}

int main(void)
{
    Game game = { 0 };
    game.state = STATE_MENU;

    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Corrida na Bahia");
    SetExitKey(KEY_NULL);
    SetTargetFPS(60);
    SetRandomSeed((unsigned int)time(NULL));

    AssetsLoad(&game.assets);
    PlayerReset(&game.player);
    BackgroundReset(&game.background);
    ObstaclesInit(&game.obstacles);
    RankingInit(&game.ranking);

    char rankingPath[512];
    RankingGetPath(rankingPath, sizeof rankingPath);
    RankingLoad(&game.ranking, rankingPath);

    while (!WindowShouldClose() && !game.exitRequested)
    {
        float dt = GetFrameTime();
        if (dt > 1.0f / 30.0f) dt = 1.0f / 30.0f;

        switch (game.state)
        {
            case STATE_MENU: UpdateMenu(&game); break;
            case STATE_PLAYING: UpdatePlaying(&game, dt); break;
            case STATE_GAME_OVER: UpdateGameOver(&game, dt); break;
            case STATE_NAME_ENTRY: UpdateNameEntry(&game, dt); break;
            case STATE_RANKING: UpdateRanking(&game); break;
        }

        BeginDrawing();

        switch (game.state)
        {
            case STATE_MENU:
                DrawMenu(&game);
                break;
            case STATE_PLAYING:
                DrawPlaying(&game);
                break;
            case STATE_GAME_OVER:
                DrawPlaying(&game);
                DrawGameOver(&game);
                break;
            case STATE_NAME_ENTRY:
                DrawPlaying(&game);
                DrawNameEntry(&game);
                break;
            case STATE_RANKING:
                DrawRanking(&game);
                break;
        }

        EndDrawing();
    }

    ObstaclesFree(&game.obstacles);
    RankingFree(&game.ranking);
    AssetsUnload(&game.assets);
    CloseWindow();

    return 0;
}
