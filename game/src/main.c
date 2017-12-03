/*
 *	ludum dare 40
 *
 */

#include <math.h>

#include "collision.c"

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

#define COLLISION_BUFFER 10.0f

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

typedef enum _entityType
{
	NONE   = 0x0000,
	PLAYER = 0x0001,
	ENEMY  = 0x0002,
} EntityType;

typedef struct _entity
{
    Vector2 position;
    Vector2 velocity;
    float rotation;
	float maxVelocity;
    Vector3 collider;
	EntityType type;
    Color color;
} Entity;

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
global_variable Camera2D GlobalCamera;
global_variable Entity GlobalPlayer;

//------------------------------------------------------------------------------------
// Module Functions Declaration (local)
//------------------------------------------------------------------------------------
internal void
InitGame(Screen *gameScreen, Camera2D *gameCamera, TileMap* gameMap, Entity *gamePlayer, TileTypes *gameTileTypes);

internal void
UpdateGame(TileMap *gameMap, Entity *gamePlayer, TileTypes *tileTypes);

internal void
DrawGame(TileMap *gameMap, Entity *gamePlayer, TileTypes *tileTypes, Camera2D *gameCamera);

internal void
UnloadGame(void);

internal void
UpdateDrawFrame(TileMap *gameMap, Entity *gamePlayer, TileTypes *tileTypes, Camera2D *gameCamera);

internal void
UpdatePlayerPosition(Entity *gamePlayer);

internal void
UpdateEntityPosition(TileMap *gameMap, Entity *entity, TileTypes *tileTypes);

internal void
SetMapRect(TileMap *gameMap, int x, int y, int w, int h, int type);

internal inline Vector2
GetTileAtLocation(TileMap *gameMap, Vector2 location);

internal void 
HandleTileCollisions(TileMap *gameMap, Entity *entity, TileTypes *tileTypes);

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
InitGame(Screen *gameScreen, Camera2D *gameCamera, TileMap* gameMap, Entity *gamePlayer, TileTypes *gameTileTypes)
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
		gamePlayer->position.x = 64;
		gamePlayer->position.y = 64;
		gamePlayer->velocity.x = gamePlayer->velocity.y = 0;
		gamePlayer->color = WHITE;
		gamePlayer->maxVelocity = PLAYER_SPEED;
		gamePlayer->type = PLAYER;
	}
}

internal void
UpdateGame(TileMap *gameMap, Entity *gamePlayer, TileTypes *tileTypes)
{
	UpdateEntityPosition(gameMap, gamePlayer, tileTypes);
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
DrawGame(TileMap *gameMap, Entity *gamePlayer, TileTypes *tileTypes, Camera2D *gameCamera)
{
    BeginDrawing();
	gameCamera->target.x = (int) gamePlayer->position.x;
	gameCamera->target.y = (int) gamePlayer->position.y;
	gameCamera->offset.x = (int) (-gamePlayer->position.x + GlobalScreen.width / 2);
	gameCamera->offset.y = (int) (-gamePlayer->position.y + GlobalScreen.height / 2);
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
		DrawRectangle(gamePlayer->position.x - PLAYER_BASE_SIZE / 2, gamePlayer->position.y - PLAYER_BASE_SIZE, PLAYER_BASE_SIZE, PLAYER_BASE_SIZE, gamePlayer->color);
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
UpdateDrawFrame(TileMap *gameMap, Entity *gamePlayer, TileTypes *tileTypes, Camera2D *gameCamera)
{
	UpdateGame(gameMap, gamePlayer, tileTypes);
	DrawGame(gameMap, gamePlayer, tileTypes, gameCamera);
}

// Updates the player's position based on the keyboard input
internal void
UpdatePlayerPosition(Entity *gamePlayer)
{
	Vector2 acceleration = Vector2Zero();

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
}

internal inline Vector2
GetTileAtLocation(TileMap *gameMap, Vector2 location)
{
	return (Vector2){(int)(location.x/gameMap->tileWidth), (int)(location.y/gameMap->tileHeight)};
}

internal void
UpdateEnemyPosition(Entity *gameEntity)
{
	NotImplemented;
}

internal void
UpdateEntityPosition(TileMap *gameMap, Entity *entity, TileTypes *tileTypes)
{
	switch (entity->type)
	{
		case PLAYER:
		{
			UpdatePlayerPosition(entity);
		} break;

		default:
		{

		} break;
	}

	HandleTileCollisions(gameMap, entity, tileTypes);
}

internal void 
HandleTileCollisions(TileMap *gameMap, Entity *entity, TileTypes *tileTypes) 
{
	Vector2 currentTile = GetTileAtLocation(gameMap, entity->position);

	// if the entity's velocity is positive in the x it's moving right so start testing tiles to the left
	// if the entity's velocity is postive in the y it's moving down, so start testing tiles above it
	int x = (entity->velocity.x > 0) ? ((int)currentTile.x)-1 : ((int)currentTile.x)+1;
	int xDir = (entity->velocity.x > 0) ? 1 : -1;
	int y = (entity->velocity.y > 0) ? ((int)currentTile.y)-1 : ((int)currentTile.y)+1;
	int yDir = (entity->velocity.y > 0) ? 1 : -1;

	// check all tiles around the entity
	for (int i = 0; i < 3; i++)
	{
		for (int j = 0; j < 3; j++) 
		{
			TileProps tp = tileTypes->tiles[gameMap->map[x+(i*xDir)][y+(j*yDir)]];
			if (!tp.wall)
				continue;

			Vector2 tileTl = (Vector2){gameMap->tileWidth*(x+(i*xDir)), gameMap->tileWidth*(y+(j*yDir))};
			Vector2 tileBr = (Vector2){tileTl.x+gameMap->tileWidth, tileTl.y+gameMap->tileHeight};
			//TODO: Use collision box here, not entity's box
			Vector2 entityTl = (Vector2){entity->position.x-(PLAYER_BASE_SIZE/2), entity->position.y-(COLLISION_BUFFER/2)};
			Vector2 entityBr = (Vector2){entity->position.x+(PLAYER_BASE_SIZE/2), entity->position.y+(COLLISION_BUFFER/2)};
			Vector3 move = RectCollision3(tileTl, tileBr, entityTl, entityBr);
			if (move.z) 
			{
				entity->position = Vector2Add(entity->position, (Vector2){move.x,move.y});
				/*if (move.x > 0) 
				{
					entity->velocity.x = 0;
				}
				if (move.y > 0)
				{
					entity->velocity.y = 0;
				}*/
			}
		}
	}
}