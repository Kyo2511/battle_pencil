

#include <raylib.h>
#include <string.h>
#include "scene_setup.h"

static int GetUsedFacePoints(const Player *player, int faceCount)
{
    int total = 0;

    for (int i = 0; i < faceCount; i++) {
        total += player->faces[i].value;
    }

    return total;
}

static const char *GetFaceTypeName(FaceType type)
{
    if (type == FACE_ATTACK) {
        return "ATTACK";
    }
    if (type == FACE_HEAL) {
        return "HEAL";
    }
    return "NONE";
}

static void UpdatePlayerCountStep(AppState *app)
{
    if (IsKeyPressed(KEY_RIGHT)) {
        if (app->playerCount < MAX_PLAYERS) {
            app->playerCount++;
        }
    }

    if (IsKeyPressed(KEY_LEFT)) {
        if (app->playerCount > MIN_PLAYERS) {
            app->playerCount--;
        }
    }

    if (IsKeyPressed(KEY_ENTER)) {
        app->setupStep = SETUP_PLAYER_NAMES;
        app->currentSettingPlayer = 0;
        app->inputName[0] = '\0';
        app->inputNameLength = 0;
    }
}

static void UpdatePlayerNamesStep(AppState *app)
{
    int key = GetCharPressed();

    while (key > 0) {
        if (key >= 32 && key <= 126 && app->inputNameLength < NAME_LENGTH - 1) {
            app->inputName[app->inputNameLength] = (char)key;
            app->inputNameLength++;
            app->inputName[app->inputNameLength] = '\0';
        }
        key = GetCharPressed();
    }

    if (IsKeyPressed(KEY_BACKSPACE) && app->inputNameLength > 0) {
        app->inputNameLength--;
        app->inputName[app->inputNameLength] = '\0';
    }

    if (IsKeyPressed(KEY_ENTER) && app->inputNameLength > 0) {
        strcpy(app->players[app->currentSettingPlayer].name, app->inputName);

        app->currentSettingPlayer++;
        app->inputName[0] = '\0';
        app->inputNameLength = 0;

        if (app->currentSettingPlayer >= app->playerCount) {
            app->setupStep = SETUP_HP;
            app->currentSettingPlayer = 0;
            app->inputHp = 10;
        }
    }
}

static void UpdateHpStep(AppState *app)
{
    if (IsKeyPressed(KEY_RIGHT)) {
        if (app->inputHp < 99) {
            app->inputHp++;
        }
    }

    if (IsKeyPressed(KEY_LEFT)) {
        if (app->inputHp > 1) {
            app->inputHp--;
        }
    }

    if (IsKeyPressed(KEY_ENTER)) {
        app->players[app->currentSettingPlayer].hp = app->inputHp;
        app->players[app->currentSettingPlayer].alive = 1;

        app->currentSettingPlayer++;

        if (app->currentSettingPlayer >= app->playerCount) {
            app->setupStep = SETUP_ACTION_POINTS;
            app->currentSettingPlayer = 0;
            app->inputActionPoints = 20;
        }
        else {
            app->inputHp = 10;
        }
    }
}

static void UpdateActionPointsStep(AppState *app)
{
    if (IsKeyPressed(KEY_RIGHT)) {
        if (app->inputActionPoints < 99) {
            app->inputActionPoints++;
        }
    }

    if (IsKeyPressed(KEY_LEFT)) {
        if (app->inputActionPoints > 1) {
            app->inputActionPoints--;
        }
    }

    if (IsKeyPressed(KEY_ENTER)) {
        app->players[app->currentSettingPlayer].actionPoints = app->inputActionPoints;

        app->currentSettingPlayer++;

        if (app->currentSettingPlayer >= app->playerCount) {
            app->setupStep = SETUP_PENCIL_FACES;
            app->currentSettingPlayer = 0;
            app->currentFaceIndex = 0;
            app->inputFaceType = FACE_ATTACK;
            app->inputFaceValue = 1;
        }
        else {
            app->inputActionPoints = 20;
        }
    }
}

static void UpdatePencilFacesStep(AppState *app)
{
    int usedPoints = GetUsedFacePoints(&app->players[app->currentSettingPlayer], app->currentFaceIndex);
    int remainingPoints = app->players[app->currentSettingPlayer].actionPoints - usedPoints;

    if (IsKeyPressed(KEY_UP)) {
        if (app->inputFaceType > FACE_ATTACK) {
            app->inputFaceType--;
        }
    }

    if (IsKeyPressed(KEY_DOWN)) {
        if (app->inputFaceType < FACE_NONE) {
            app->inputFaceType++;
        }
    }

    if (app->inputFaceType == FACE_NONE) {
        app->inputFaceValue = 0;
    }
    else {
        if (remainingPoints <= 0) {
            app->inputFaceValue = 0;
        }
        else {
            if (app->inputFaceValue < 1) {
                app->inputFaceValue = 1;
            }
            if (app->inputFaceValue > remainingPoints) {
                app->inputFaceValue = remainingPoints;
            }
        }

        if (IsKeyPressed(KEY_RIGHT)) {
            if (app->inputFaceValue < remainingPoints) {
                app->inputFaceValue++;
            }
        }

        if (IsKeyPressed(KEY_LEFT)) {
            if (app->inputFaceValue > 1) {
                app->inputFaceValue--;
            }
        }
    }

    if (IsKeyPressed(KEY_ENTER)) {
        if (app->inputFaceType == FACE_NONE || app->inputFaceValue <= remainingPoints) {
            app->players[app->currentSettingPlayer].faces[app->currentFaceIndex].type = app->inputFaceType;

            if (app->inputFaceType == FACE_NONE) {
                app->players[app->currentSettingPlayer].faces[app->currentFaceIndex].value = 0;
            }
            else {
                app->players[app->currentSettingPlayer].faces[app->currentFaceIndex].value = app->inputFaceValue;
            }

            app->currentFaceIndex++;
            app->inputFaceType = FACE_ATTACK;
            app->inputFaceValue = 1;

            if (app->currentFaceIndex >= PENCIL_SIDES) {
                app->currentFaceIndex = 0;
                app->currentSettingPlayer++;

                if (app->currentSettingPlayer >= app->playerCount) {
                    app->setupStep = SETUP_TURN_ORDER;
                    app->currentSettingPlayer = 0;
                }
            }
        }
    }
}

static void DrawPlayerCountStep(AppState *app)
{
    DrawText("SETUP SCENE", 250, 100, 40, WHITE);
    DrawText("Select Player Count", 240, 200, 30, WHITE);
    DrawText(TextFormat("%d", app->playerCount), 390, 280, 50, YELLOW);
    DrawText("LEFT / RIGHT : Change", 240, 380, 24, LIGHTGRAY);
    DrawText("ENTER : Confirm", 280, 420, 24, LIGHTGRAY);
}

static void DrawPlayerNamesStep(AppState *app)
{
    DrawText("SETUP SCENE", 250, 80, 40, WHITE);
    DrawText(TextFormat("Player %d Name", app->currentSettingPlayer + 1), 260, 180, 30, WHITE);

    DrawRectangle(220, 250, 360, 50, WHITE);
    DrawRectangle(223, 253, 354, 44, DARKBLUE);
    DrawText(app->inputName, 240, 262, 30, YELLOW);

    DrawText("Type name and press ENTER", 220, 340, 24, LIGHTGRAY);
    DrawText("BACKSPACE : Delete", 250, 390, 24, LIGHTGRAY);

    for (int i = 0; i < app->currentSettingPlayer; i++) {
        DrawText(TextFormat("Player %d: %s", i + 1, app->players[i].name), 220, 450 + i * 25, 20, WHITE);
    }
}

static void DrawHpStep(AppState *app)
{
    DrawText("SETUP SCENE", 250, 80, 40, WHITE);
    DrawText("HP SETUP", 310, 140, 30, YELLOW);

    DrawText(
        TextFormat("Player %d: %s", app->currentSettingPlayer + 1,
                   app->players[app->currentSettingPlayer].name),
        180, 220, 28, WHITE
    );

    DrawText("Set HP with LEFT / RIGHT", 220, 290, 24, LIGHTGRAY);

    DrawRectangle(320, 340, 160, 60, WHITE);
    DrawRectangle(323, 343, 154, 54, DARKBLUE);
    DrawText(TextFormat("%d", app->inputHp), 375, 355, 32, YELLOW);

    DrawText("ENTER : Confirm", 280, 440, 24, LIGHTGRAY);

    for (int i = 0; i < app->currentSettingPlayer; i++) {
        DrawText(TextFormat("Done - %s : %d HP", app->players[i].name, app->players[i].hp), 220, 500 + i * 25, 20, WHITE);
    }
}

static void DrawActionPointsStep(AppState *app)
{
    DrawText("SETUP SCENE", 250, 80, 40, WHITE);
    DrawText("ACTION POINT SETUP", 210, 140, 30, YELLOW);

    DrawText(
        TextFormat("Player %d: %s", app->currentSettingPlayer + 1,
                   app->players[app->currentSettingPlayer].name),
        180, 220, 28, WHITE
    );

    DrawText("Set Action Points with LEFT / RIGHT", 170, 290, 24, LIGHTGRAY);

    DrawRectangle(320, 340, 160, 60, WHITE);
    DrawRectangle(323, 343, 154, 54, DARKBLUE);
    DrawText(TextFormat("%d", app->inputActionPoints), 375, 355, 32, YELLOW);

    DrawText("ENTER : Confirm", 280, 440, 24, LIGHTGRAY);

    for (int i = 0; i < app->currentSettingPlayer; i++) {
        DrawText(TextFormat("Done - %s : %d pt", app->players[i].name,
                            app->players[i].actionPoints), 220, 500 + i * 25, 20, WHITE);
    }
}

static void DrawPencilFacesStep(AppState *app)
{
    int usedPoints = GetUsedFacePoints(&app->players[app->currentSettingPlayer], app->currentFaceIndex);
    int remainingPoints = app->players[app->currentSettingPlayer].actionPoints - usedPoints;
    const char *faceName = GetFaceTypeName(app->inputFaceType);

    DrawText("SETUP SCENE", 250, 30, 40, WHITE);
    DrawText("PENCIL FACE SETUP", 210, 80, 30, YELLOW);

    DrawText(
        TextFormat("Player %d: %s", app->currentSettingPlayer + 1,
                   app->players[app->currentSettingPlayer].name),
        180, 130, 28, WHITE
    );

    DrawText(TextFormat("Face %d / 6", app->currentFaceIndex + 1), 320, 170, 28, LIGHTGRAY);
    DrawText(TextFormat("Action Points: %d", app->players[app->currentSettingPlayer].actionPoints), 240, 220, 24, WHITE);
    DrawText(TextFormat("Used: %d", usedPoints), 300, 255, 24, WHITE);
    DrawText(TextFormat("Remaining: %d", remainingPoints), 260, 290, 24, YELLOW);

    DrawText("Type: UP / DOWN", 250, 340, 24, LIGHTGRAY);
    DrawText("Value: LEFT / RIGHT", 220, 375, 24, LIGHTGRAY);
    DrawText("ENTER : Confirm Face", 230, 410, 24, LIGHTGRAY);

    DrawRectangle(250, 455, 300, 105, WHITE);
    DrawRectangle(253, 458, 294, 99, DARKBLUE);
    DrawText(TextFormat("TYPE : %s", faceName), 290, 475, 26, YELLOW);

    if (app->inputFaceType == FACE_NONE) {
        DrawText("VALUE: 0", 330, 510, 26, GRAY);
    }
    else if (remainingPoints <= 0) {
        DrawText("VALUE: NO POINTS", 255, 510, 26, RED);
    }
    else {
        DrawText(TextFormat("VALUE: %d", app->inputFaceValue), 320, 510, 26, YELLOW);
    }
}

static void DrawTurnOrderStep(AppState *app)
{
    DrawText("SETUP SCENE", 250, 60, 40, WHITE);
    DrawText("TURN ORDER SETUP", 220, 120, 30, YELLOW);

    DrawText("LEFT / RIGHT : Select Position", 180, 180, 24, LIGHTGRAY);
    DrawText("UP / DOWN : Swap", 250, 220, 24, LIGHTGRAY);
    DrawText("ENTER : Confirm", 270, 260, 24, LIGHTGRAY);

    for (int i = 0; i < app->playerCount; i++) {
        int playerIndex = app->turnOrder[i];
        int x = 180;
        int y = 330 + i * 45;

        if (i == app->currentTurnOrderIndex) {
            DrawRectangle(x - 20, y - 5, 440, 35, YELLOW);
            DrawRectangle(x - 17, y - 2, 434, 29, DARKBLUE);
            DrawText(
                TextFormat("%d. %s", i + 1, app->players[playerIndex].name),
                x, y, 24, YELLOW
            );
        } else {
            DrawText(
                TextFormat("%d. %s", i + 1, app->players[playerIndex].name),
                x, y, 24, WHITE
            );
        }
    }
}

void UpdateSetupScene(AppState *app)
{
    if (app->setupStep == SETUP_PLAYER_COUNT) {
        UpdatePlayerCountStep(app);
    }
    else if (app->setupStep == SETUP_PLAYER_NAMES) {
        UpdatePlayerNamesStep(app);
    }
    else if (app->setupStep == SETUP_HP) {
        UpdateHpStep(app);
    }
    else if (app->setupStep == SETUP_ACTION_POINTS) {
        UpdateActionPointsStep(app);
    }
    else if (app->setupStep == SETUP_PENCIL_FACES) {
        UpdatePencilFacesStep(app);
    }
    else if (app->setupStep == SETUP_TURN_ORDER) {
        if (IsKeyPressed(KEY_LEFT)) {
            if (app->currentTurnOrderIndex > 0) {
                app->currentTurnOrderIndex--;
            }
        }

        if (IsKeyPressed(KEY_RIGHT)) {
            if (app->currentTurnOrderIndex < app->playerCount - 1) {
                app->currentTurnOrderIndex++;
            }
        }

        if (IsKeyPressed(KEY_UP)) {
            if (app->currentTurnOrderIndex > 0) {
                int temp = app->turnOrder[app->currentTurnOrderIndex];
                app->turnOrder[app->currentTurnOrderIndex] = app->turnOrder[app->currentTurnOrderIndex - 1];
                app->turnOrder[app->currentTurnOrderIndex - 1] = temp;
                app->currentTurnOrderIndex--;
            }
        }

        if (IsKeyPressed(KEY_DOWN)) {
            if (app->currentTurnOrderIndex < app->playerCount - 1) {
                int temp = app->turnOrder[app->currentTurnOrderIndex];
                app->turnOrder[app->currentTurnOrderIndex] = app->turnOrder[app->currentTurnOrderIndex + 1];
                app->turnOrder[app->currentTurnOrderIndex + 1] = temp;
                app->currentTurnOrderIndex++;
            }
        }

        if (IsKeyPressed(KEY_ENTER)) {
            app->currentPlayerIndex = app->turnOrder[0];
            app->currentScene = SCENE_BATTLE;
        }
    }
}

void DrawSetupScene(AppState *app)
{
    ClearBackground(DARKBLUE);

    if (app->setupStep == SETUP_PLAYER_COUNT) {
        DrawPlayerCountStep(app);
    }
    else if (app->setupStep == SETUP_PLAYER_NAMES) {
        DrawPlayerNamesStep(app);
    }
    else if (app->setupStep == SETUP_HP) {
        DrawHpStep(app);
    }
    else if (app->setupStep == SETUP_ACTION_POINTS) {
        DrawActionPointsStep(app);
    }
    else if (app->setupStep == SETUP_PENCIL_FACES) {
        DrawPencilFacesStep(app);
    }
    else if (app->setupStep == SETUP_TURN_ORDER) {
        DrawTurnOrderStep(app);
    }
}