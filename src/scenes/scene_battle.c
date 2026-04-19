#include <raylib.h>
#include <stdio.h>
#include "scene_battle.h"

static int GetTurnOrderIndex(AppState *app, int playerIndex)
{
    for (int i = 0; i < app->playerCount; i++) {
        if (app->turnOrder[i] == playerIndex) {
            return i;
        }
    }

    return 0;
}

static int GetNextAlivePlayerIndex(AppState *app, int currentIndex)
{
    int startOrderIndex = GetTurnOrderIndex(app, currentIndex);

    for (int offset = 1; offset < app->playerCount; offset++) {
        int orderIndex = (startOrderIndex + offset) % app->playerCount;
        int playerIndex = app->turnOrder[orderIndex];

        if (app->players[playerIndex].alive) {
            return playerIndex;
        }
    }

    return currentIndex;
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

static int IsValidTarget(AppState *app, int attackerIndex, int targetIndex)
{
    if (targetIndex < 0 || targetIndex >= app->playerCount) {
        return 0;
    }
    if (targetIndex == attackerIndex) {
        return 0;
    }
    if (!app->players[targetIndex].alive) {
        return 0;
    }
    return 1;
}

static int GetFirstAliveTarget(AppState *app, int attackerIndex)
{
    for (int i = 0; i < app->playerCount; i++) {
        if (i != attackerIndex && app->players[i].alive) {
            return i;
        }
    }

    return -1;
}

static int GetNextTarget(AppState *app, int attackerIndex, int currentTarget, int direction)
{
    int candidate = currentTarget;

    if (!IsValidTarget(app, attackerIndex, candidate)) {
        candidate = attackerIndex;
    }

    for (int step = 0; step < app->playerCount; step++) {
        candidate += direction;

        if (candidate < 0) {
            candidate = app->playerCount - 1;
        }
        else if (candidate >= app->playerCount) {
            candidate = 0;
        }

        if (IsValidTarget(app, attackerIndex, candidate)) {
            return candidate;
        }
    }

    return -1;
}

static int CountAlivePlayers(AppState *app)
{
    int count = 0;

    for (int i = 0; i < app->playerCount; i++) {
        if (app->players[i].alive) {
            count++;
        }
    }

    return count;
}

void UpdateBattleScene(AppState *app)
{
    Player *player = &app->players[app->currentPlayerIndex];

    if (!IsValidTarget(app, app->currentPlayerIndex, app->selectedTargetIndex)) {
        app->selectedTargetIndex = GetFirstAliveTarget(app, app->currentPlayerIndex);
    }

    if (!app->isRolling) {
        if (IsKeyPressed(KEY_UP)) {
            int nextTarget = GetNextTarget(app, app->currentPlayerIndex, app->selectedTargetIndex, -1);
            if (nextTarget != -1) {
                app->selectedTargetIndex = nextTarget;
            }
        }

        if (IsKeyPressed(KEY_DOWN)) {
            int nextTarget = GetNextTarget(app, app->currentPlayerIndex, app->selectedTargetIndex, 1);
            if (nextTarget != -1) {
                app->selectedTargetIndex = nextTarget;
            }
        }

        if (IsKeyPressed(KEY_ENTER)) {
            app->isRolling = 1;
            app->rollingFrame = 0;
            app->battleMessage[0] = '\0';
        }
    }
    else {
        app->rollingFrame++;

        if (app->rollingFrame % 3 == 0) {
            app->rolledFaceIndex = GetRandomValue(0, PENCIL_SIDES - 1);
            app->rolledFaceType = player->faces[app->rolledFaceIndex].type;
            app->rolledFaceValue = player->faces[app->rolledFaceIndex].value;
        }

        if (app->rollingFrame >= app->rollingDuration) {
            app->isRolling = 0;

            if (app->rolledFaceType == FACE_ATTACK) {
                int targetIndex = app->selectedTargetIndex;

                if (IsValidTarget(app, app->currentPlayerIndex, targetIndex)) {
                    app->players[targetIndex].hp -= app->rolledFaceValue;

                    if (app->players[targetIndex].hp <= 0) {
                        app->players[targetIndex].hp = 0;
                        app->players[targetIndex].alive = 0;

                        snprintf(
                            app->battleMessage,
                            sizeof(app->battleMessage),
                            "%s dealt %d damage to %s. %s was defeated.",
                            player->name,
                            app->rolledFaceValue,
                            app->players[targetIndex].name,
                            app->players[targetIndex].name
                        );
                    }
                    else {
                        snprintf(
                            app->battleMessage,
                            sizeof(app->battleMessage),
                            "%s dealt %d damage to %s.",
                            player->name,
                            app->rolledFaceValue,
                            app->players[targetIndex].name
                        );
                    }
                }
                else {
                    snprintf(
                        app->battleMessage,
                        sizeof(app->battleMessage),
                        "%s attacked, but there was no valid target.",
                        player->name
                    );
                }
            }
            else if (app->rolledFaceType == FACE_HEAL) {
                player->hp += app->rolledFaceValue;

                snprintf(
                    app->battleMessage,
                    sizeof(app->battleMessage),
                    "%s healed %d HP.",
                    player->name,
                    app->rolledFaceValue
                );
            }
            else {
                snprintf(
                    app->battleMessage,
                    sizeof(app->battleMessage),
                    "%s rolled NONE.",
                    player->name
                );
            }

            if (CountAlivePlayers(app) <= 1) {
                app->winnerIndex = app->currentPlayerIndex;
                app->currentScene = SCENE_RESULT;
                return;
            }

            app->currentPlayerIndex = GetNextAlivePlayerIndex(app, app->currentPlayerIndex);
            app->selectedTargetIndex = GetFirstAliveTarget(app, app->currentPlayerIndex);
        }
    }
}

void DrawBattleScene(AppState *app)
{
    ClearBackground(BLACK);

    DrawText("BATTLE SCENE", 40, 30, 40, WHITE);

    DrawText(
        TextFormat("Current Player: %s", app->players[app->currentPlayerIndex].name),
        40, 100, 30, YELLOW
    );

    DrawText(
        TextFormat("HP: %d", app->players[app->currentPlayerIndex].hp),
        40, 145, 28, WHITE
    );

    if (app->selectedTargetIndex >= 0) {
        DrawText(
            TextFormat("Target: %s", app->players[app->selectedTargetIndex].name),
            40, 190, 28, ORANGE
        );
        DrawText("UP / DOWN : Change target", 40, 230, 24, LIGHTGRAY);
    }
    else {
        DrawText("Target: none", 40, 190, 28, GRAY);
    }
    
    if (app->isRolling) {
        DrawText("Rolling...", 40, 270, 24, ORANGE);
    } else {
        DrawText("ENTER : Roll pencil", 40, 270, 24, LIGHTGRAY);
    }

    if (app->rolledFaceIndex >= 0) {
        DrawText(
            TextFormat("Rolled Face: %d", app->rolledFaceIndex + 1),
            40, 330, 28, WHITE
        );

        DrawText(
            TextFormat("Type: %s", GetFaceTypeName(app->rolledFaceType)),
            40, 370, 28, YELLOW
        );

        DrawText(
            TextFormat("Value: %d", app->rolledFaceValue),
            40, 410, 28, YELLOW
        );
    }

    if (app->battleMessage[0] != '\0') {
    DrawText(app->battleMessage, 40, 460, 20, LIGHTGRAY);
}

    DrawText("All Players", 560, 30, 28, LIGHTGRAY);

    for (int i = 0; i < app->playerCount; i++) {
        int x = 520;
        int y = 80 + i * 80;
        Color nameColor = WHITE;
        Color hpColor = WHITE;

        if (!app->players[i].alive) {
            nameColor = GRAY;
            hpColor = GRAY;
        }
        else if (i == app->currentPlayerIndex) {
            nameColor = YELLOW;
            hpColor = YELLOW;
        }
        else if (i == app->selectedTargetIndex) {
            nameColor = ORANGE;
            hpColor = ORANGE;
        }

        DrawRectangleLines(x - 10, y - 10, 240, 55, DARKGRAY);
        DrawText(app->players[i].name, x, y, 24, nameColor);
        DrawText(TextFormat("HP: %d", app->players[i].hp), x, y + 28, 22, hpColor);
    }

    DrawText("Turn Order", 560, 500, 24, LIGHTGRAY);

    for (int i = 0; i < app->playerCount; i++) {
        int playerIndex = app->turnOrder[i];
        DrawText(
            TextFormat("%d. %s", i + 1, app->players[playerIndex].name),
            520, 540 + i * 28, 20, WHITE
        );
    }
}