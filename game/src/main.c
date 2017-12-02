/*
 *	ludum dare 40
 *
 */

#include "raylib.h"

#include <math.h>

#if defined(PLATFORM_WEB)
    #include <emscripten/emscripten.h>
#endif

//----------------------------------------------------------------------------------
// Defines
//----------------------------------------------------------------------------------
#define PLAYER_BASE_SIZE    20.0f
#define PLAYER_SPEED        6.0f
#define PLAYER_MAX_SHOOTS   10

#define global_variable static
#define internal	static
#define local_persist   static

#define len(array)(sizeof(array)/sizeof(array[0]))

//----------------------------------------------------------------------------------
// Types and Structures Definition
//----------------------------------------------------------------------------------

typedef struct _titleMap
{
	Rectangle map[6][6];
	int length;
	int width;
} TileMap;

typedef struct _player
{
    Vector2 position;
    Vector2 speed;
    float acceleration;
    float rotation;
    Vector3 collider;
    Color color;
} Player;

typedef struct _shoot
{
    Vector2 position;
    Vector2 speed;
    float radius;
    float rotation;
    int lifeSpawn;
    bool active;
    Color color;
} Shoot;

//------------------------------------------------------------------------------------
// Global Variables Declaration
//------------------------------------------------------------------------------------
global_variable int screenWidth = 800;
global_variable int screenHeight = 450;
global_variable char *windowTitle = "ludum dare 40";

global_variable bool gameOver;
global_variable bool pause;
global_variable bool victory;

global_variable Camera GlobalCamera;
global_variable TileMap GlobalMap;
global_variable Player GlobalPlayer;

//------------------------------------------------------------------------------------
// Module Functions Declaration (local)
//------------------------------------------------------------------------------------
internal void
InitGame(Camera *gameCamera, TileMap* gameMap, Player *gamePlayer);

internal void
UpdateGame(void);

internal void
DrawGame(void);

internal void
UnloadGame(void);

internal void
UpdateDrawFrame(void);

//------------------------------------------------------------------------------------
// Program main entry point
//------------------------------------------------------------------------------------
int main(void)
{
    // Initialization
    InitWindow(screenWidth, screenHeight, windowTitle);

    InitGame(&GlobalCamera, &GlobalMap, &GlobalPlayer);

#if defined(PLATFORM_WEB)
    emscripten_set_main_loop(UpdateDrawFrame, 0, 1);
#else
    SetTargetFPS(60);
    
    // Main game loop
    while (!WindowShouldClose())    // Detect window close button or ESC key
    {
        // Update and Draw
        UpdateDrawFrame();
    }
#endif

    // De-Initialization
    UnloadGame();         // Unload loaded data (textures, sounds, models...)
    
    CloseWindow();        // Close window and OpenGL context
    return 0;
}

internal void
InitGame(Camera *gameCamera, TileMap *gameMap, Player *gamePlayer)
{
	// camera setup
	{
		gameCamera->position.x = 16.0f;
		gameCamera->position.y = 14.0f;
		gameCamera->position.z = 16.0f;

		gameCamera->target.x   = 0.0f;
		gameCamera->target.y   = 0.0f;
		gameCamera->target.z   = 0.0f;

		gameCamera->up.x	   = 0.0f;
		gameCamera->up.y	   = 1.0f;
		gameCamera->up.z	   = 0.0f;

		gameCamera->fovy	   = 45.0f;
	}

	// tile map setup
	{

	}
}

internal void
UpdateGame(void)
{
       // TODO	
}

internal void
DrawGame(void)
{
    BeginDrawing();
    ClearBackground(RAYWHITE);

    if (!gameOver)
    {
    }
        
    EndDrawing();
}

internal void
UnloadGame(void)
{
    // TODO: Unload all dynamic loaded data (textures, sounds, models...)
}

// Update and Draw (one frame)
internal void
UpdateDrawFrame(void)
{
    UpdateGame();
    DrawGame();
}
