#include <raylib.h>
#include "app.h"
#include "scenes/scene_title.h"
#include "scenes/scene_setup.h"
#include "scenes/scene_battle.h"
#include "scenes/scene_result.h"

int main(void)
{
    AppState app = {0};
    app.currentScene = SCENE_TITLE;
    app.playerCount = 3;
    app.setupStep = SETUP_PLAYER_COUNT;
    app.currentSettingPlayer = 0;
    app.inputName[0] = '\0';
    app.inputNameLength = 0;
    app.inputHp = 10;
    app.inputActionPoints = 20;
    app.currentFaceIndex = 0;
    app.inputFaceType = FACE_ATTACK;
    app.inputFaceValue = 1;
    app.currentTurnOrderIndex = 0;
    for (int i = 0; i < MAX_PLAYERS; i++) {
        app.turnOrder[i] = i;
    }
    app.rolledFaceIndex = -1;
    app.rolledFaceType = FACE_NONE;
    app.rolledFaceValue = 0;
    app.selectedTargetIndex = -1;
    app.isRolling = 0;
    app.rollingFrame = 0;
    app.rollingDuration = 90;
    app.battleMessage[0] = '\0';

    InitWindow(800, 600, "Battle Pencil");
    SetTargetFPS(60);

    while (!WindowShouldClose())
    {
        if (app.currentScene == SCENE_TITLE) {
            UpdateTitleScene(&app);
        } else if (app.currentScene == SCENE_SETUP) {
            UpdateSetupScene(&app);
        } else if (app.currentScene == SCENE_BATTLE) {
            UpdateBattleScene(&app);
        }

        BeginDrawing();

        if (app.currentScene == SCENE_TITLE) {
            DrawTitleScene(&app);
        } else if (app.currentScene == SCENE_SETUP) {
            DrawSetupScene(&app);
        } else if (app.currentScene == SCENE_BATTLE) {
            DrawBattleScene(&app);
        }
        
        EndDrawing();
    }

    CloseWindow();
    return 0;
}
