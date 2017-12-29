/*
 *  gamestate.h
 *
 */

typedef struct _gameState
{
    bool paused;
    bool gameOver;
    int frameRateTarget;
    int frameRateCurrent;
    int frameCounter;
    int gameTime;
} GameState;
