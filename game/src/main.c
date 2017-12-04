/*
 *	ludum dare 40
 *  main.c
 *
 */
#include "raylib.h"
#include "raymath.h"
#include <stdlib.h>
#include <math.h>

#include "main.h"
#include "entity.h"
#include "collision.c"
#include "levelgen.c"
#include "vector2i.h"

#if defined(PLATFORM_WEB)
    #include <emscripten/emscripten.h>
#endif

//----------------------------------------------------------------------------------
// Defines
//----------------------------------------------------------------------------------
#define PLAYER_INDEX            0
#define PLAYER_BASE_SIZE        20.0f
#define PLAYER_SPEED            8.0f
#define PLAYER_SPEED_INCREMENT  0.25f
#define PLAYER_SPEED_DECAY      0.95f

#define ENEMY_DEFAULT_SIZE              20.0f
#define ENEMY_DEFAULT_SPEED             1.5f
#define ENEMY_DEFAULT_SPEED_INCREMENT   0.25f
#define ENEMY_DEFAULT_SPEED_DECAY       0.97f

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

global_variable EntityCollection GlobalEntities;

//------------------------------------------------------------------------------------
// Module Functions Declaration (local)
//------------------------------------------------------------------------------------
internal void
InitGame(Screen *gameScreen, Camera2D *gameCamera, TileMap* gameMap, EntityCollection *globalEntities, TileTypes *gameTileTypes);

internal void
UpdateGame(float detla, TileMap *gameMap, EntityCollection *gameEnemies, TileTypes *tileTypes, Camera2D *gameCamera);

internal void
DrawGame(TileMap *gameMap, EntityCollection *gameEntities, TileTypes *tileTypes, Camera2D *gameCamera);

internal void
UnloadGame(void);

internal void
UpdateDrawFrame(TileMap *gameMap, EntityCollection *gameEntities, TileTypes *tileTypes, Camera2D *gameCamera);

internal void
UpdateEntitiesPosition(float delta, TileMap *gameMap, EntityCollection *gameEntities, TileTypes *tileTypes, Camera2D *gameCamera);

internal void
UpdatePlayerPosition(float delta, Entity *gamePlayer, Camera2D *gameCamera);

internal void
UpdateEnemyPosition(float delta, Entity gamePlayer, Entity *gameEnemy, TileMap *);

internal void
SetMapRect(TileMap *gameMap, int x, int y, int w, int h, int type);

internal inline Vector2i
GetTileAtLocation(TileMap *gameMap, Vector2 location);

internal inline Vector2
GetTileCenter(TileMap *gameMap, int tileX, int tileY);

internal void 
HandleTileCollisions(TileMap *gameMap, Entity *entity, TileTypes *tileTypes);

internal int 
AddEntity(EntityCollection *collection, Entity entity);

internal void
RemoveEntity(EntityCollection *collection, int entityIx);

//------------------------------------------------------------------------------------
// Program main entry point
//------------------------------------------------------------------------------------
int main(void)
{
	GlobalScreen.width = 1280;
	GlobalScreen.height = 720;
	// Initialization
    InitWindow(GlobalScreen.width, GlobalScreen.height, windowTitle);
    InitGame(&GlobalScreen, &GlobalCamera, &GlobalMap, &GlobalEntities, &GlobalTileTypes);

#if defined(PLATFORM_WEB)
	// TODO(nick): might need to change this to have parameters? look at documentation 
    emscripten_set_main_loop(UpdateDrawFrame, 0, 1);
#else
    SetTargetFPS(60);
	while (!WindowShouldClose())
	{
		UpdateDrawFrame(&GlobalMap, &GlobalEntities, &GlobalTileTypes, &GlobalCamera);
	}
#endif

    // De-Initialization
    UnloadGame();
    CloseWindow();

    return 0;
}

internal void
InitGame(Screen *gameScreen, Camera2D *gameCamera, TileMap* gameMap, EntityCollection *gameEntities, TileTypes *gameTileTypes)
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
		GenerateLevel(50, 100, gameMap->map);
		gameMap->tileWidth = floor(gameScreen->width / 32.0);
		gameMap->tileHeight = gameMap->tileWidth;
		SetMapRect(gameMap, 1, 1, 5, 5, 1);
		SetMapRect(gameMap, 1, 8, 6, 4, 1);
		SetMapRect(gameMap, 3, 5, 1, 4, 3);
	}

	// player setup
	{
		Vector2 playerStart = GetTileCenter(gameMap, 1, 1);
		gameEntities->list[PLAYER_INDEX].position.x = playerStart.x;
		gameEntities->list[PLAYER_INDEX].position.y = playerStart.y;
		gameEntities->list[PLAYER_INDEX].velocity.x = 0;
        gameEntities->list[PLAYER_INDEX].velocity.y = 0;
		gameEntities->list[PLAYER_INDEX].color = WHITE;
		gameEntities->list[PLAYER_INDEX].maxVelocity = PLAYER_SPEED;
		gameEntities->list[PLAYER_INDEX].props.type = PLAYER;
		gameEntities->list[PLAYER_INDEX].width = PLAYER_BASE_SIZE;
		gameEntities->list[PLAYER_INDEX].height = PLAYER_BASE_SIZE;
	}

    // enemies setup
    {
        // TODO(nick): figure out a way to spawn x amount of enemies near the player
        gameEntities->size = MAX_ENTITIES;
        int i;
        for (i = PLAYER_INDEX + 1; i < 2; ++i)
        {
			Vector2 enemyStart = GetTileCenter(gameMap, 4, 4);
            gameEntities->list[i].position.x = enemyStart.x;
            gameEntities->list[i].position.y = enemyStart.y;
            gameEntities->list[i].velocity.x = 0;
            gameEntities->list[i].velocity.y = 0;
            gameEntities->list[i].color = PURPLE;
            gameEntities->list[i].maxVelocity = ENEMY_DEFAULT_SPEED;
            gameEntities->list[i].props.type = ENEMY; 
            gameEntities->list[i].props.subType = SKELETON;
            gameEntities->list[i].props.attributes = NOATTRIBUTES;
			gameEntities->list[i].height = ENEMY_DEFAULT_SIZE;
			gameEntities->list[i].width = ENEMY_DEFAULT_SIZE;
        }
    }
}

internal void
UpdateGame(float delta, TileMap *gameMap, EntityCollection *gameEntities, TileTypes *tileTypes, Camera2D *gameCamera)
{
    UpdateEntitiesPosition(delta, gameMap, gameEntities, tileTypes, gameCamera);
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
DrawGame(TileMap *gameMap, EntityCollection *gameEntities, TileTypes *tileTypes, Camera2D *gameCamera)
{
    BeginDrawing();
    Entity *gamePlayer = &gameEntities->list[PLAYER_INDEX];
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
			int bX = -gameCamera->offset.x / gameMap->tileWidth;
			int bY = -gameCamera->offset.y / gameMap->tileHeight;
			int eX = bX + 1 + GlobalScreen.width / gameMap->tileWidth;
			int eY = bY + 1 + GlobalScreen.height / gameMap->tileHeight;
			bX = max(min(bX, LEVEL_SIZE), 0);
			bY = max(min(bY, LEVEL_SIZE), 0);
			eX = max(min(eX, LEVEL_SIZE), 0);
			eY = max(min(eY, LEVEL_SIZE), 0);
			for (x = bX; x < eX; ++x)
			{
				for (y = bY; y < eY; ++y)
				{	
					TileProps tile = tileTypes->tiles[gameMap->map[x][y]];
					DrawRectangle(x * gameMap->tileWidth, y * gameMap->tileHeight, gameMap->tileWidth-1, gameMap->tileHeight-1, tile.color);
				}
			}
		}

        // draw player
		DrawRectangle(gamePlayer->position.x - gamePlayer->width / 2, gamePlayer->position.y - gamePlayer->height / 2, gamePlayer->width, gamePlayer->height, gamePlayer->color);

        // draw enemies
        int i;
        for (i = (PLAYER_INDEX + 1); i < gameEntities->size; ++i)
        {
            DrawRectangle(gameEntities->list[i].position.x - gameEntities->list[i].width / 2, gameEntities->list[i].position.y - gameEntities->list[i].height / 2, gameEntities->list[i].width, gameEntities->list[i].height, gameEntities->list[i].color);
        }
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
UpdateDrawFrame(TileMap *gameMap, EntityCollection *gameEntities, TileTypes *tileTypes, Camera2D *gameCamera)
{
	UpdateGame(1, gameMap, gameEntities, tileTypes, gameCamera);
	DrawGame(gameMap, gameEntities, tileTypes, gameCamera);
}

// Updates the player's position based on the keyboard input
internal void
UpdatePlayerPosition(float delta, Entity *gamePlayer, Camera2D *gameCamera)
{
	Vector2 acceleration;
	acceleration.x = 0;
	acceleration.y = 0;
	// update player input
	if (IsKeyDown(KEY_RIGHT) || IsKeyDown(KEY_D)) 
	{
		acceleration.x += PLAYER_SPEED_INCREMENT;
	}
	if (IsKeyDown(KEY_LEFT) || IsKeyDown(KEY_A))
	{
		acceleration.x -= PLAYER_SPEED_INCREMENT;
	}
	if (IsKeyDown(KEY_UP) || IsKeyDown(KEY_W))
	{
		acceleration.y -= PLAYER_SPEED_INCREMENT;
	}
	if (IsKeyDown(KEY_DOWN) || IsKeyDown(KEY_S))
	{
		acceleration.y += PLAYER_SPEED_INCREMENT;
	}
	if (!IsKeyDown(KEY_UP) && !IsKeyDown(KEY_W) && !IsKeyDown(KEY_DOWN) && !IsKeyDown(KEY_S))
	{
		gamePlayer->velocity.y *= PLAYER_SPEED_DECAY;
	}
	if (!IsKeyDown(KEY_LEFT) && !IsKeyDown(KEY_A) && !IsKeyDown(KEY_RIGHT) && !IsKeyDown(KEY_D))
	{
		gamePlayer->velocity.x *= PLAYER_SPEED_DECAY;
	}
	
	Vector2Scale(&acceleration, delta);
	gamePlayer->velocity = Vector2Add(acceleration, gamePlayer->velocity);
	float magnitude = Vector2Length(gamePlayer->velocity);
	if (magnitude > gamePlayer->maxVelocity)
	{
		Vector2Scale(&gamePlayer->velocity, gamePlayer->maxVelocity/magnitude);
	}

	Vector2 frameVel = gamePlayer->velocity;
	Vector2Scale(&frameVel, delta);
	gamePlayer->position = Vector2Add(gamePlayer->position, frameVel);
	Vector2 mousePosition = GetMousePosition();
	mousePosition.x -= gameCamera->offset.x;
	mousePosition.y -= gameCamera->offset.y;
	gamePlayer->rotation = Vector2Angle(gamePlayer->position, mousePosition);
	gamePlayer->direction = mousePosition;
}

internal void
UpdateEnemyPosition(float delta, Entity gamePlayer, Entity *gameEnemy, TileMap *gameMap)
{
    Vector2 tileDifference = Vector2Subtract(gamePlayer.position, gameEnemy->position);
	  float dist = Vector2Length(tileDifference);
    if (dist/gameMap->tileWidth <= 5)
    {
		Vector2Scale(&tileDifference, gameEnemy->maxVelocity/dist);
		gameEnemy->position = Vector2Add(gameEnemy->position, tileDifference);
    }
}

internal inline Vector2i
GetTileAtLocation(TileMap *gameMap, Vector2 location)
{
    return (Vector2i){(location.x/gameMap->tileWidth), (int)(location.y/gameMap->tileHeight)};
}


internal inline Vector2
GetTileCenter(TileMap *gameMap, int tileX, int tileY)
{
	return (Vector2){gameMap->tileWidth*tileX+(gameMap->tileWidth/2), gameMap->tileWidth*tileY+(gameMap->tileHeight/2)};
}

internal void
UpdateEntitiesPosition(float delta, TileMap *gameMap, EntityCollection *gameEntities, TileTypes *tileTypes, Camera2D *gameCamera)
{
    int i;
    for (i = 0; i < gameEntities->size; ++i)
    {
		bool validEntity = true;
        switch (gameEntities->list[i].props.type)
        {
            case PLAYER:
            {
                if (i == PLAYER_INDEX)
                {
                    UpdatePlayerPosition(delta, &gameEntities->list[i], gameCamera);
                }
                else
                {
                    InvalidCodePath;
                }
            } break;

            case ENEMY:
            {
                UpdateEnemyPosition(delta, gameEntities->list[PLAYER_INDEX], &gameEntities->list[i], gameMap);
            } break;

            default:
            {
				validEntity = false;
            } break;
        }
		if (validEntity) {
			HandleTileCollisions(gameMap, &gameEntities->list[i], tileTypes);
		}
    }
}


internal void 
HandleTileCollisions(TileMap *gameMap, Entity *entity, TileTypes *tileTypes) 
{
	Vector2i currentTile = GetTileAtLocation(gameMap, entity->position);

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
			int testX = max(min(x+i*xDir, LEVEL_SIZE), 0);
			int testY = max(min(y+j*yDir, LEVEL_SIZE), 0);
			TileProps tp = tileTypes->tiles[gameMap->map[testX][testY]];
			if (!tp.wall)
				continue;

			Vector2 tileTl = (Vector2){gameMap->tileWidth*testX, gameMap->tileWidth*testY};
			Vector2 tileBr = (Vector2){tileTl.x+gameMap->tileWidth, tileTl.y+gameMap->tileHeight};
			Vector2 entityTl = (Vector2){entity->position.x-(entity->width/2), entity->position.y-(entity->height/2)};
			Vector2 entityBr = (Vector2){entity->position.x+(entity->width/2), entity->position.y+(entity->height/2)};
			Vector3 move = RectCollision3(tileTl, tileBr, entityTl, entityBr);
			if (move.z) 
			{
				entity->position = Vector2Add(entity->position, (Vector2){move.x,move.y});
				if (move.x != 0)
				{
					entity->velocity.x = 0;
				}
				if (move.y != 0)
				{
					entity->velocity.y = 0;
				}
			}
		}
	}
}

internal int 
AddEntity(EntityCollection *collection, Entity entity)
{
	if (collection->capacity >= MAX_ENTITIES)
		InvalidCodePath;

	collection->list[collection->capacity] = entity;
	collection->capacity++;
	return collection->capacity-1;
}

internal void
RemoveEntity(EntityCollection *collection, int entityIx)
{
	collection->list[entityIx] = collection->list[collection->capacity-1];
	collection->list[collection->capacity-1].props.type = NOTYPE;
	collection->list[collection->capacity-1].props.subType = NOSUBTYPE;
	collection->list[collection->capacity-1].props.attributes = NOATTRIBUTES;
	collection->capacity--;
}