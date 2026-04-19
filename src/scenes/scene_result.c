#include <raylib.h>
#include "scene_result.h"

void UpdateResultScene(AppState *app)
{
    if (IsKeyPressed(KEY_ENTER)) {
        app->currentScene = SCENE_TITLE;
    }
}

void DrawResultScene(AppState *app)
{
    ClearBackground(BLACK);

    DrawText("RESULT", 320, 120, 40, WHITE);

    DrawText(
        TextFormat("Winner: %s", app->players[app->winnerIndex].name),
        220, 240, 36, YELLOW
    );

    DrawText("Press ENTER to return to title", 180, 360, 28, LIGHTGRAY);
}