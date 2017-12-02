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
#define len2d(array)(sizeof(array[0])/sizeof(array[0][0]))
#define assert(expression) if(!(expression)) {*(int *)0 = 0;}
#define NotImplemented assert(!"NotImplemented")

//----------------------------------------------------------------------------------
// Types and Structures Definition
//----------------------------------------------------------------------------------

// NOTE(nick): could probably replace with v2?
typedef struct _screen
{
	int width;
	int height;
} Screen;

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
global_variable char *windowTitle = "ludum dare 40";

global_variable bool gameOver;
global_variable bool pause;
global_variable bool victory;

global_variable Screen GlobalScreen;
global_variable TileMap GlobalMap;
global_variable Camera GlobalCamera;
global_variable Player GlobalPlayer;

//------------------------------------------------------------------------------------
// Module Functions Declaration (local)
//------------------------------------------------------------------------------------
internal void
InitGame(Screen *gameScreen, Camera *gameCamera, TileMap* gameMap, Player *gamePlayer);

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
	GlobalScreen.width = 1280;
	GlobalScreen.height = 720;
	// Initialization
    	InitWindow(GlobalScreen.width, GlobalScreen.height, windowTitle);
    	InitGame(&GlobalScreen, &GlobalCamera, &GlobalMap, &GlobalPlayer);

#if defined(PLATFORM_WEB)
    	emscripten_set_main_loop(UpdateDrawFrame, 0, 1);
#else
    	SetTargetFPS(60);
    
	while (!WindowShouldClose())    // Detect window close button or ESC key
	{
		UpdateDrawFrame();
	}
#endif

    	// De-Initialization
    	UnloadGame();         // Unload loaded data (textures, sounds, models...)
    
    	CloseWindow();        // Close window and OpenGL context
    	return 0;
}

internal void
InitGame(Screen *gameScreen, Camera *gameCamera, TileMap* gameMap, Player *gamePlayer)
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
		gameMap->width = floor(gameScreen->width / 6.0f);
		gameMap->length = floor(gameScreen->height / 6.0f);
		int x;
		int y;
		for (x = 0; x < len(gameMap->map); ++x)
		{
			for (y = 0; y < len2d(gameMap->map); ++y)
			{
				Rectangle temp = gameMap->map[x][y];	
			}
		}
	}
}

internal void
UpdateGame(void)
{
	// TODO
	NotImplemented;
}

internal void
DrawGame(void)
{
    	BeginDrawing();
    	ClearBackground(RAYWHITE);

	if (!gameOver)
	{
		NotImplemented;
	}
        
    	EndDrawing();
}

internal void
UnloadGame(void)
{
    	// TODO: Unload all dynamic loaded data (textures, sounds, models...)
	NotImplemented;
}

// Update and Draw (one frame)
internal void
UpdateDrawFrame(void)
{
	UpdateGame();
    	DrawGame();
}
