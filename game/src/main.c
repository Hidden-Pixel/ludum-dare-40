/*
 *	ludum dare 40
 *
 */

#include <math.h>

#include "raylib.h"
#include "raymath.h"


#if defined(PLATFORM_WEB)
    #include <emscripten/emscripten.h>
#endif

//----------------------------------------------------------------------------------
// Defines
//----------------------------------------------------------------------------------
#define PLAYER_BASE_SIZE    20.0f
#define PLAYER_SPEED        6.0f
#define PLAYER_SPEED_INCREMENT 0.5f
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
	int map[20][20];
	int tileHeight;
	int tileWidth;
} TileMap;

typedef struct _player
{
    Vector2 position;
    Vector2 velocity;
    Vector2 acceleration;
    float rotation;
	float maxVelocity;
    Vector3 collider;
    Color color;
	Rectangle rectangle;
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

typedef struct _tileProps
{
	Color color;
	bool wall;
} TileProps;

typedef struct _tileTypes
{
	TileProps tiles[20];
} TileTypes;

//------------------------------------------------------------------------------------
// Global Variables Declaration
//------------------------------------------------------------------------------------
global_variable char *windowTitle = "ludum dare 40";

global_variable bool gameOver;
global_variable bool pause;
global_variable bool victory;

global_variable Screen GlobalScreen;
global_variable TileMap GlobalMap;
global_variable TileTypes GlobalTileTypes;
global_variable Camera GlobalCamera;
global_variable Player GlobalPlayer;

//------------------------------------------------------------------------------------
// Module Functions Declaration (local)
//------------------------------------------------------------------------------------
internal void
InitGame(Screen *gameScreen, Camera *gameCamera, TileMap* gameMap, Player *gamePlayer, TileTypes *gameTileTypes);

internal void
UpdateGame(Player *gamePlayer);

internal void
DrawGame(TileMap *gameMap, Player *gamePlayer, TileTypes *tileTypes);

internal void
UnloadGame(void);

internal void
UpdateDrawFrame(TileMap *gameMap, Player *gamePlayer, TileTypes *tileTypes);

internal void
SetMapRect(TileMap *gameMap, int x, int y, int w, int h, int type);

//------------------------------------------------------------------------------------
// Program main entry point
//------------------------------------------------------------------------------------
int main(void)
{	
	GlobalScreen.width = 1280;
	GlobalScreen.height = 720;
	// Initialization
    	InitWindow(GlobalScreen.width, GlobalScreen.height, windowTitle);
    	InitGame(&GlobalScreen, &GlobalCamera, &GlobalMap, &GlobalPlayer, &GlobalTileTypes);

#if defined(PLATFORM_WEB)
	// TODO(nick): might need to change this to have parameters? look at documentation 
    	emscripten_set_main_loop(UpdateDrawFrame, 0, 1);
#else
    	SetTargetFPS(60);
    
	while (!WindowShouldClose())    // Detect window close button or ESC key
	{
		UpdateDrawFrame(&GlobalMap, &GlobalPlayer, &GlobalTileTypes);
	}
#endif

    	// De-Initialization
    	UnloadGame();         // Unload loaded data (textures, sounds, models...)
    
    	CloseWindow();        // Close window and OpenGL context
    	return 0;
}

internal void
InitGame(Screen *gameScreen, Camera *gameCamera, TileMap* gameMap, Player *gamePlayer, TileTypes *gameTileTypes)
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

	// tile types setup
	{
		gameTileTypes->tiles[0].color = BLACK;
		gameTileTypes->tiles[1].color = DARKGREEN;
		gameTileTypes->tiles[2].color = GRAY;
		gameTileTypes->tiles[3].color = BROWN;

		gameTileTypes->tiles[0].wall = true;
		gameTileTypes->tiles[2].wall = true;
	}

	// tile map setup
	{
		gameMap->tileWidth = floor(gameScreen->width / 32.0);
		gameMap->tileHeight = gameMap->tileWidth;
		SetMapRect(gameMap, 1, 1, 5, 5, 1);
		SetMapRect(gameMap, 1, 8, 6, 4, 1);
		SetMapRect(gameMap, 3, 5, 1, 4, 3);
	}

	// player setup
	{
		gamePlayer->rectangle.height = gamePlayer->rectangle.width = 50;
		gamePlayer->rectangle.x = gamePlayer->rectangle.y = 0;
		gamePlayer->color = BLACK;
		gamePlayer->maxVelocity = PLAYER_SPEED;
	}
}

internal void
UpdateGame(Player *gamePlayer)
{
	// update player input
	{
		if (IsKeyDown(KEY_RIGHT)) 
		{
			gamePlayer->acceleration.x += PLAYER_SPEED_INCREMENT;
		}
		if (IsKeyDown(KEY_LEFT))
		{
			gamePlayer->acceleration.x -= PLAYER_SPEED_INCREMENT;
		}
		if (IsKeyDown(KEY_UP))
		{
			gamePlayer->acceleration.y -= PLAYER_SPEED_INCREMENT;
		}
		if (IsKeyDown(KEY_DOWN))
		{
			gamePlayer->acceleration.y += PLAYER_SPEED_INCREMENT;
		}

		gamePlayer->velocity = Vector2Add(gamePlayer->acceleration, gamePlayer->velocity);
		Vector2Scale(&gamePlayer->velocity, 0.9);
		float magnitude = Vector2Length(gamePlayer->velocity);
		if (magnitude > gamePlayer->maxVelocity)
		{
			Vector2Divide(&gamePlayer->velocity, magnitude);
		}
		gamePlayer->position = Vector2Add(gamePlayer->position, gamePlayer->velocity);
		gamePlayer->rectangle.x = gamePlayer->position.x;
		gamePlayer->rectangle.y = gamePlayer->position.y;
		gamePlayer->acceleration.x = gamePlayer->acceleration.y = 0;
	}
}

internal void
SetMapRect(TileMap *gameMap, int x, int y, int w, int h, int type)
{
	int a;
	int b;
	for (a = x; a < x+w; a++) {
		for (b = y; b < y+h; b++) {
			gameMap->map[a][b] = type;
		}
	}
}

internal void
DrawGame(TileMap *gameMap, Player *gamePlayer, TileTypes *tileTypes)
{
    BeginDrawing();
    ClearBackground(RAYWHITE);

	if (!gameOver)
	{
		DrawRectangleRec(gamePlayer->rectangle, gamePlayer->color);
		// draw tile map
		{
			int x;
			int y;
			for (x = 0; x < len(gameMap->map); ++x)
			{
				for (y = 0; y < len2d(gameMap->map); ++y)
				{	
					// TODO(nick): remove for testing
					TileProps tile = tileTypes->tiles[gameMap->map[x][y]];
					DrawRectangle(x * gameMap->tileWidth, y * gameMap->tileHeight, gameMap->tileWidth, gameMap->tileHeight, tile.color);
				}
			}
		}
	}
        
    	EndDrawing();
}

internal void
UnloadGame(void)
{
    	// TODO: Unload all dynamic loaded data (textures, sounds, models...)
	//NotImplemented;
}

// Update and Draw (one frame)
internal void
UpdateDrawFrame(TileMap *gameMap, Player *gamePlayer, TileTypes *tileTypes)
{
	UpdateGame(gamePlayer);
    	DrawGame(gameMap, gamePlayer, tileTypes);
}
