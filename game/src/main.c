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
#define PLAYER_SPEED        2.0f
#define PLAYER_SPEED_INCREMENT 0.25f
#define PLAYER_SPEED_DECAY 0.95f
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

typedef struct _collisionInfo
{
	bool hasCollided;
	Vector2 move;
} CollisionInfo;

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
global_variable Camera2D GlobalCamera;
global_variable Player GlobalPlayer;

//------------------------------------------------------------------------------------
// Module Functions Declaration (local)
//------------------------------------------------------------------------------------
internal void
InitGame(Screen *gameScreen, Camera2D *gameCamera, TileMap* gameMap, Player *gamePlayer, TileTypes *gameTileTypes);

internal void
UpdateGame(Player *gamePlayer);

internal void
DrawGame(TileMap *gameMap, Player *gamePlayer, TileTypes *tileTypes, Camera2D *gameCamera);

internal void
UnloadGame(void);

internal void
UpdateDrawFrame(TileMap *gameMap, Player *gamePlayer, TileTypes *tileTypes, Camera2D *gameCamera);

internal void
UpdatePlayerPosition(Player *gamePlayer);

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
		UpdateDrawFrame(&GlobalMap, &GlobalPlayer, &GlobalTileTypes, &GlobalCamera);
	}
#endif

    	// De-Initialization
    	UnloadGame();         // Unload loaded data (textures, sounds, models...)
    
    	CloseWindow();        // Close window and OpenGL context
    	return 0;
}

internal void
InitGame(Screen *gameScreen, Camera2D *gameCamera, TileMap* gameMap, Player *gamePlayer, TileTypes *gameTileTypes)
{
	// camera setup
	{
		gameCamera->target.x   = 0.0f;
		gameCamera->target.y   = 0.0f;
		gameCamera->rotation = 0.0f;
		gameCamera->zoom = 1.0f;

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
		gamePlayer->rectangle.height = gamePlayer->rectangle.width = PLAYER_BASE_SIZE;
		gamePlayer->rectangle.x = gamePlayer->position.x = 64;
		gamePlayer->rectangle.y = gamePlayer->position.y = 64;
		gamePlayer->velocity.x = gamePlayer->velocity.y = 0;
		gamePlayer->color = WHITE;
		gamePlayer->maxVelocity = PLAYER_SPEED;
	}
}

internal void
UpdateGame(Player *gamePlayer)
{
	UpdatePlayerPosition(gamePlayer);
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
DrawGame(TileMap *gameMap, Player *gamePlayer, TileTypes *tileTypes, Camera2D *gameCamera)
{
    BeginDrawing();
	gameCamera->target.x = gamePlayer->rectangle.x;
	gameCamera->target.y = gamePlayer->rectangle.y;
	gameCamera->offset.x = -gamePlayer->rectangle.x + GlobalScreen.width / 2;
	gameCamera->offset.y = -gamePlayer->rectangle.y + GlobalScreen.height / 2;
	ClearBackground(RAYWHITE);
	Begin2dMode(*gameCamera);

	if (!gameOver)
	{
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
		DrawRectangleRec(gamePlayer->rectangle, gamePlayer->color);
	}

	End2dMode();
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
UpdateDrawFrame(TileMap *gameMap, Player *gamePlayer, TileTypes *tileTypes, Camera2D *gameCamera)
{
	UpdateGame(gamePlayer);
	DrawGame(gameMap, gamePlayer, tileTypes, gameCamera);
}

// Updates the player's position based on the keyboard input
internal void
UpdatePlayerPosition(Player *gamePlayer)
{
	Vector2 acceleration;
	acceleration.x = 0;
	acceleration.y = 0;
	// update player input
	if (IsKeyDown(KEY_RIGHT)) 
	{
		acceleration.x += PLAYER_SPEED_INCREMENT;
	}
	if (IsKeyDown(KEY_LEFT))
	{
		acceleration.x -= PLAYER_SPEED_INCREMENT;
	}
	if (IsKeyDown(KEY_UP))
	{
		acceleration.y -= PLAYER_SPEED_INCREMENT;
	}
	if (IsKeyDown(KEY_DOWN))
	{
		acceleration.y += PLAYER_SPEED_INCREMENT;
	}
	if (!IsKeyDown(KEY_UP) && !IsKeyDown(KEY_DOWN))
	{
		gamePlayer->velocity.y *= PLAYER_SPEED_DECAY;
	}
	if (!IsKeyDown(KEY_LEFT) && !IsKeyDown(KEY_RIGHT))
	{
		gamePlayer->velocity.x *= PLAYER_SPEED_DECAY;
	}

	gamePlayer->velocity = Vector2Add(acceleration, gamePlayer->velocity);

	float magnitude = Vector2Length(gamePlayer->velocity);
	if (magnitude > gamePlayer->maxVelocity)
	{
		Vector2Divide(&gamePlayer->velocity, magnitude);
	}
	gamePlayer->position = Vector2Add(gamePlayer->position, gamePlayer->velocity);
	gamePlayer->rectangle.x = gamePlayer->position.x;
	gamePlayer->rectangle.y = gamePlayer->position.y;
}

// Output vector 3 details the move r2 has to make to no longer collide with r1.  The z of the vector 3 will be true (positive int) if it has collided, false (0) if it has not.
internal Vector3
RectCollision3(Vector2 r1tl, Vector2 r1br, Vector2 r2tl, Vector2 r2br)
{
	Vector2 r1Center = Vector2Add(r1tl, r1br);
	Vector2Divide(&r1Center, 2);
	Vector2 r2Center = Vector2Add(r2tl, r2br);
	Vector2Divide(&r2Center, 2);
	Vector2 difference = Vector2Subtract(r1Center, r2Center);
	difference.x = fabs(difference.x);
	difference.y = fabs(difference.y);
	Vector2 r1Size = Vector2Subtract(r1br, r1tl);
	Vector2 r2Size = Vector2Subtract(r2br, r2tl);
	Vector2 avgSize = Vector2Add(r1Size, r2Size);
	Vector2Divide(&avgSize, 2);
	//return difference.x < avgSize.x && difference.y < avgSize.y;
	Vector3 move;
	move.z = (difference.x < avgSize.x && difference.y < avgSize.y);
	if (!move.z) 
	{
		move.x = 0;
		move.y = 0;
		return move;
	}
	
	Vector2 d = Vector2Subtract(avgSize, difference);
	if (d.x > d.y) 
	{
		move.y = (r2Center.y > r1Center.y) ? d.y : -d.y;
	}
	else 
	{
		move.x = (r2Center.x > r1Center.x) ? d.x : -d.x;
	}

	return move;
}

internal bool 
RectCollision(Vector2 r1tl, Vector2 r1br, Vector2 r2tl, Vector2 r2br) 
{
	return RectCollision3(r1tl, r1br, r2tl, r2br).z;
}

internal Vector2
RectCollisionMove(Vector2 r1tl, Vector2 r1br, Vector2 r2tl, Vector2 r2br)
{
	Vector3 move = RectCollision3(r1tl, r1br, r2tl, r2br);
	return (Vector2){move.x, move.y};
}

