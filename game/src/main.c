/*
 *	ludum dare 40
 *
 */

#include "raylib.h"
#include "raymath.h"

#include <math.h>

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
	Rectangle map[6][6];
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
UpdateGame(Player *gamePlayer);

internal void
DrawGame(TileMap *gameMap, Player *gamePlayer);

internal void
UnloadGame(void);

internal void
UpdateDrawFrame(TileMap *gameMap, Player *gamePlayer);

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
	// TODO(nick): might need to change this to have parameters? look at documentation 
    	emscripten_set_main_loop(UpdateDrawFrame, 0, 1);
#else
    	SetTargetFPS(60);
    
	while (!WindowShouldClose())    // Detect window close button or ESC key
	{
		UpdateDrawFrame(&GlobalMap, &GlobalPlayer);
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
		gameMap->tileWidth = floor(gameScreen->width / 6.0f);
		gameMap->tileHeight = floor(gameScreen->height / 6.0f);
		int x;
		int y;
		for (x = 0; x < len(gameMap->map); ++x)
		{
			for (y = 0; y < len2d(gameMap->map); ++y)
			{
				gameMap->map[x][y].width = (gameMap->tileWidth) - 5;
				gameMap->map[x][y].height = (gameMap->tileHeight) - 5;
				//gameMap->map[x][y].width = 20;
				//gameMap->map[x][y].height = 20;
				// NOTE(nick): might need to add an offset value?
				gameMap->map[x][y].x = (gameMap->tileWidth * x);
				gameMap->map[x][y].y = (gameMap->tileHeight * y);
			}
		}
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
			gamePlayer->acceleration.x = PLAYER_SPEED_INCREMENT;
		}
		if (IsKeyDown(KEY_LEFT))
		{
			gamePlayer->acceleration.x = -PLAYER_SPEED_INCREMENT;
		}
		if (IsKeyDown(KEY_UP))
		{
			gamePlayer->acceleration.y = PLAYER_SPEED_INCREMENT;
		}
		if (IsKeyDown(KEY_DOWN))
		{
			gamePlayer->acceleration.y = -PLAYER_SPEED_INCREMENT;
		}

		gamePlayer->velocity = Vector2Add(gamePlayer->acceleration, gamePlayer->velocity);
		float magnitude = Vector2Length(gamePlayer->velocity);
		if (magnitude > gamePlayer->maxVelocity)
		{
			Vector2Divide(&gamePlayer->velocity, magnitude);
		}
		gamePlayer->position = Vector2Add(gamePlayer->position, gamePlayer->velocity);
	}
}

internal void
DrawGame(TileMap *gameMap, Player *gamePlayer)
{
    BeginDrawing();
    ClearBackground(RAYWHITE);

	if (!gameOver)
	{
		DrawRectangleRec(gamePlayer->rectangle, gamePlayer->color);
		/*// draw tile map
		{
			int x;
			int y;
			Color tempColor = RED;
			for (x = 0; x < len(gameMap->map); ++x)
			{
				for (y = 0; y < len2d(gameMap->map); ++y)
				{	
					// TODO(nick): remove for testing
					Rectangle temp = gameMap->map[x][y];
					tempColor = RED;
					if (((x + 1)) % 2 == 0)
					{
						tempColor = GREEN;
					}
					DrawRectangleRec(gameMap->map[x][y], tempColor);
				}
			}
		}
		*/
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
UpdateDrawFrame(TileMap *gameMap, Player *gamePlayer)
{
	UpdateGame(gamePlayer);
    DrawGame(gameMap, gamePlayer);
}