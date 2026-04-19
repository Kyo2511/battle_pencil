#ifndef APP_H
#define APP_H

#define MAX_PLAYERS 6
#define MIN_PLAYERS 3
#define NAME_LENGTH 32
#define PENCIL_SIDES 6

typedef enum {
    SCENE_TITLE,
    SCENE_SETUP,
    SCENE_BATTLE,
    SCENE_RESULT
} Scene;

typedef enum {
    FACE_ATTACK,
    FACE_HEAL,
    FACE_NONE
} FaceType;

typedef enum {
    SETUP_PLAYER_COUNT,
    SETUP_PLAYER_NAMES,
    SETUP_HP,
    SETUP_ACTION_POINTS,
    SETUP_PENCIL_FACES,
    SETUP_TURN_ORDER,
    SETUP_DONE
} SetupStep;

typedef struct {
    FaceType type;
    int value;
} PencilFace;

typedef struct {
    char name[NAME_LENGTH];
    int hp;
    PencilFace faces[PENCIL_SIDES];
    int alive;
    int actionPoints;
} Player;

typedef struct {
    Scene currentScene;
    int playerCount;
    Player players[MAX_PLAYERS];
    int currentPlayerIndex;
    int winnerIndex;

    SetupStep setupStep;
    int currentSettingPlayer;

    char inputName[NAME_LENGTH];
    int inputNameLength;

    int inputHp;
    int inputActionPoints;

    int currentFaceIndex;
    FaceType inputFaceType;
    int inputFaceValue;

    int turnOrder[MAX_PLAYERS];
    int currentTurnOrderIndex;

    int rolledFaceIndex;
    FaceType rolledFaceType;
    int rolledFaceValue;
    int selectedTargetIndex;

    int isRolling;
    int rollingFrame;
    int rollingDuration;

    char battleMessage[128];
} AppState;

#endif