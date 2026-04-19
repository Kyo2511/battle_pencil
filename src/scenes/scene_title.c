#include <raylib.h>
#include "scene_title.h"

void UpdateTitleScene(AppState *app)
{
    if (IsKeyPressed(KEY_ENTER)) {
        app->currentScene = SCENE_SETUP;
    }
}

void DrawTitleScene(AppState *app)
{
    (void)app;

    ClearBackground(BLACK);
    DrawText("BATTLE PENCIL", 250, 200, 40, WHITE);
    DrawText("Press ENTER to Start", 260, 300, 24, GRAY);
}