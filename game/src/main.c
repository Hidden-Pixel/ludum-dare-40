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
#include "item.h"
#include "entity.h"
#include "collision.c"
#include "levelgen.c"
#include "vector2i.h"

#if defined(PLATFORM_WEB)
    #include <emscripten/emscripten.h>
#endif

//------------------------------------------------------------------------------------
// Global Variables Declaration
//------------------------------------------------------------------------------------
global_variable char *windowTitle = "ludum dare 40";

global_variable bool gameOver;
global_variable bool paused;
global_variable bool victory;

global_variable Screen GlobalScreen;
global_variable TileMap GlobalMap;
global_variable TileTypes GlobalTileTypes;
global_variable Camera2D GlobalCamera;

global_variable EntityCollection GlobalEntities;
global_variable ItemCollection GlobalItems;

//------------------------------------------------------------------------------------
// Module Functions Declaration (local)
//------------------------------------------------------------------------------------
internal void
InitGame(Screen *gameScreen, Camera2D *gameCamera, TileMap* gameMap, EntityCollection *gameEntities, ItemCollection *gameItems, TileTypes *gameTileTypes);

internal void
UpdateGame(float detla, TileMap *gameMap, EntityCollection *gameEnemies, ItemCollection *gameItems, TileTypes *tileTypes, Camera2D *gameCamera);

internal void
DrawGame(TileMap *gameMap, EntityCollection *gameEntities, ItemCollection *gameItems, TileTypes *tileTypes, Camera2D *gameCamera);

internal void
UpdateMenu(void);

internal void
DrawHud();

internal void
DrawMenu(Screen screen);

internal void
UnloadGame(void);

internal void
UpdateDrawFrame(void);

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

internal bool 
HandleTileCollisions(TileMap *gameMap, Entity *entity, TileTypes *tileTypes);

internal void
UpdateBulletPosition(float delta, Entity *bullet);

internal void
ResolveEntityCollisions(TileMap *gameMap, EntityCollection *gameEntities);

internal void
ResolvePlayerItemCollision(TileMap *gameMap, Entity *gamePlayer, ItemCollection *gameItems);

//------------------------------------------------------------------------------------
// Program main entry point
//------------------------------------------------------------------------------------
int main(void)
{
	GlobalScreen.width = 1280;
	GlobalScreen.height = 720;
	// Initialization
    InitWindow(GlobalScreen.width, GlobalScreen.height, windowTitle);
    InitGame(&GlobalScreen, &GlobalCamera, &GlobalMap, &GlobalEntities, &GlobalItems, &GlobalTileTypes);
    paused = true;

#if defined(PLATFORM_WEB)
    // TODO(nick): might need to change this to have parameters? look at documentation 
    emscripten_set_main_loop(UpdateDrawFrame, 0, 1);
#else
    SetTargetFPS(60);
	while (!WindowShouldClose())
	{
		UpdateDrawFrame();
	}
#endif

    // De-Initialization
    UnloadGame();
    CloseWindow();

    return 0;
}

internal void
InitGame(Screen *gameScreen, Camera2D *gameCamera, TileMap* gameMap, EntityCollection *gameEntities, ItemCollection *gameItems, TileTypes *gameTileTypes)
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
	}

	// player setup
	{
		Vector2 playerStart = GetTileCenter(gameMap, LEVEL_SIZE / 2, LEVEL_SIZE / 2);
		Entity player =
        {
            .position = {playerStart.x, playerStart.y},
			.velocity = {0, 0},
			.color = WHITE,
			.maxVelocity = PLAYER_SPEED,
			.props.type = PLAYER,
            .width = PLAYER_BASE_SIZE,
            .height = PLAYER_BASE_SIZE,
            .items = {0, 0, 0},
		};
		AddEntity(gameEntities, player);
	}

    // enemies setup
    {
        gameEntities->size = MAX_ENTITIES;
        int i;
        for (i = 0; i < 3; ++i)
        {
			Entity skel =
            {
				.position = {80, 80 + i * 40},
				.velocity = {0, 0},
				.color = PURPLE,
				.maxVelocity = ENEMY_DEFAULT_SPEED,
				.props.type = ENEMY,
				.props.subType = SKELETON,
				.props.attributes = NOENTITYATTRIBUTES,
				.state = 0,
				.sightDistance = 4.0f,
				.counter = 0,
				.height = ENEMY_DEFAULT_SIZE,
				.width = ENEMY_DEFAULT_SIZE
			};
			AddEntity(gameEntities, skel);
        }
    }

    // items setup
    {
        int i;
        gameItems->size = 32;
        gameItems->capacity = 0;
        for (i = 0; i < 5; ++i)
        {
            // TODO(nick): random item generation
            Item item = 
            {
                .position = { 80, 80 + i * 40},
                .color = YELLOW,
                .height = 10,
                .width = 10,
                .rotation = 0.0f,
                .type = PICKUP,
                .subType = HEALTHPACK,
            };
            AddItem(gameItems, item);
        }
    }
}

internal void
UpdateGame(float delta, TileMap *gameMap, EntityCollection *gameEntities, ItemCollection *gameItems, TileTypes *tileTypes, Camera2D *gameCamera)
{
    UpdateEntitiesPosition(delta, gameMap, gameEntities, tileTypes, gameCamera);
	ResolveEntityCollisions(gameMap, gameEntities);
    ResolvePlayerItemCollision(gameMap, &gameEntities->list[PLAYER_INDEX], gameItems);
}

internal void
UpdateMenu(void)
{
    if (IsKeyPressed(KEY_SPACE) || IsKeyPressed(KEY_ENTER))
    {
        paused = false;
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
DrawMenu(Screen gameScreen)
{
    BeginDrawing();
	    ClearBackground(RAYWHITE);
        DrawText("PRESS SPACE OR ENTER TO START THE GAME!", 450, 300, 20, LIGHTGRAY);
    EndDrawing();
}

internal void
DrawGame(TileMap *gameMap, EntityCollection *gameEntities, ItemCollection *gameItems, TileTypes *tileTypes, Camera2D *gameCamera)
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
		DrawRectangle(gamePlayer->position.x - gamePlayer->width / 2,
                      gamePlayer->position.y - gamePlayer->height / 2,
                      gamePlayer->width, gamePlayer->height,
                      gamePlayer->color);

        // draw enemies
        int i;
        for (i = (PLAYER_INDEX + 1); i < gameEntities->capacity; ++i)
        {
            if (gameEntities->list[i].props.type != NOENTITYTYPE)
            {
                DrawRectangle(gameEntities->list[i].position.x - gameEntities->list[i].width / 2,
                              gameEntities->list[i].position.y - gameEntities->list[i].height / 2,
                              gameEntities->list[i].width, gameEntities->list[i].height,
                              gameEntities->list[i].color);
            }
        }

        // draw items
        for (i = 0; i < gameItems->size; ++i)
        {
            if (gameItems->list[i].type != NOITEMTYPE) 
            {
                DrawRectangle(gameItems->list[i].position.x - gameItems->list[i].width / 2,
                              gameItems->list[i].position.y - gameItems->list[i].height / 2,
                              gameItems->list[i].width, gameItems->list[i].height,
                              gameItems->list[i].color);
            }
        }
	}
	End2dMode();
    DrawHud();
	EndDrawing();
}

internal void
DrawHud()
{
    DrawText(FormatText("Health: %03i", 100), 20, 20, 20, RED);
}

internal void
UnloadGame(void)
{
    // TODO: Unload all dynamic loaded data (textures, sounds, models...)
	//NotImplemented;
}

// Update and Draw (one frame)
internal void
UpdateDrawFrame(void)
{
    if (paused) 
    {
        DrawMenu(GlobalScreen);
        UpdateMenu();
    }
    else
    {
        UpdateGame(1, &GlobalMap, &GlobalEntities, &GlobalItems, &GlobalTileTypes, &GlobalCamera);
	    DrawGame(&GlobalMap, &GlobalEntities, &GlobalItems, &GlobalTileTypes, &GlobalCamera);
    }
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
    if (dist > 0.01 && dist/gameMap->tileWidth <= gameEnemy->sightDistance)
    {
		Vector2Scale(&tileDifference, gameEnemy->maxVelocity/dist);
		gameEnemy->position = Vector2Add(gameEnemy->position, tileDifference);
	}
	else
    {
		float x = 0, y = 0;
		Vector2i blk;
		switch (gameEnemy->state)
        {
            case 0:
                x = gameEnemy->maxVelocity / 2;
                blk = GetTileAtLocation(gameMap, gameEnemy->position);
                if (GlobalTileTypes.tiles[gameMap->map[blk.x + 1][blk.y]].wall) {
                    gameEnemy->state = (rand() % 2) * 2 + 1;
                    gameEnemy->counter = 0;
                }
                break;
            case 1:
                y = gameEnemy->maxVelocity / 2;
                blk = GetTileAtLocation(gameMap, gameEnemy->position);
                if (GlobalTileTypes.tiles[gameMap->map[blk.x][blk.y + 1]].wall) {
                    gameEnemy->state = (rand() % 2) * 2;
                    gameEnemy->counter = 0;
                }
                break;
            case 2:
                x = -gameEnemy->maxVelocity / 2;
                blk = GetTileAtLocation(gameMap, gameEnemy->position);
                if (GlobalTileTypes.tiles[gameMap->map[blk.x - 1][blk.y]].wall) {
                    gameEnemy->state = (rand() % 2) * 2 + 1;
                    gameEnemy->counter = 0;
                }
                break;
            case 3:
                y = -gameEnemy->maxVelocity / 2;
                blk = GetTileAtLocation(gameMap, gameEnemy->position);
                if (GlobalTileTypes.tiles[gameMap->map[blk.x][blk.y - 1]].wall) {
                    gameEnemy->state = (rand() % 2) * 2;
                    gameEnemy->counter = 0;
                }
                break;
		}
		gameEnemy->position = Vector2Add(gameEnemy->position, (Vector2) { x, y });
		gameEnemy->counter++;
		if (gameEnemy->counter >= 10 && rand() % 200 == 44)
        {
			gameEnemy->counter = 0;
			gameEnemy->state = rand() % 6;
		}
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
    for (i = 0; i < gameEntities->capacity; ++i)
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

			case WEAPON:
			{
				switch(gameEntities->list[i].props.subType)
				{
					case BULLET:
						UpdateBulletPosition(delta, &gameEntities->list[i]);
						break;
					default:
						validEntity = false;
						break;
				}
			} break;

            default:
            {
				validEntity = false;
            } break;
        }
		if (validEntity)
        {
			bool collisionWithTile = HandleTileCollisions(gameMap, &gameEntities->list[i], tileTypes);
			bool removed = HandleEntityActions(gameMap, gameEntities, i, collisionWithTile);
			// if the entity died, removed, etc reprocess the current index
			if (removed) {
				i--;
			}
		}
    }
}

internal void
UpdateBulletPosition(float delta, Entity *bullet)
{
	Vector2 frameVel = bullet->velocity;
	Vector2Scale(&frameVel, delta);
	bullet->position = Vector2Add(bullet->position, frameVel);
}

internal void
ResolvePlayerItemCollision(TileMap *gameMap, Entity *gamePlayer, ItemCollection *gameItems)
{
    Vector2 diff = Vector2Zero();
    float dist = 0.0f;
    float rad = 0.0f;
    int i;
    for (i = 0; i < gameItems->capacity; ++i)
    {
        // TODO(nick): wrap this up to a function? and use in both as well
        // check if player is close to item
        rad = (gamePlayer->width + gameItems->list[i].width) / 2.0f;
        diff = Vector2Subtract(gamePlayer->position, gameItems->list[i].position);
        if (abs(diff.x) > rad || abs(diff.y) > rad)
        {
            continue;
        }

	    dist = Vector2Length(diff);
        if (dist > rad)
        {
            continue;
        }
        else
        {
            // TODO(nick): pick up item
            switch (gameItems->list[i].type)
            {
                case PICKUP:
                {
                    int i;
                    for (i = 0; i < MAX_ITEM_SLOT; ++i)
                    {
                        if (gamePlayer->items[i].type == NOITEMTYPE)
                        {
                            gamePlayer->items[i] = gameItems->list[i];
                            break;
                        }
                    }
                } break;

                case POWERUP:
                {
                    //TODO apply power up!
                };
            }
            RemoveItem(gameItems, i);
        }
    }
}

internal void 
ResolveEntityCollisions(TileMap *gameMap, EntityCollection *gameEntities)
{
	for (int i = 0; i < gameEntities->capacity - 1; i++) {
		for (int j = i + 1; j < gameEntities->capacity; j++) {
			Entity *e1 = &gameEntities->list[i];
			Entity *e2 = &gameEntities->list[j];
			float rad = (e1->width + e2->width) / 2.0;
			Vector2 diff = Vector2Subtract(e1->position, e2->position);
			//fast check before distance formula
			if (abs(diff.x) > rad || abs(diff.y) > rad)
            {
				continue;
			}
			//possible collision
			
			float dist = Vector2Length(diff);
			if (dist >= rad) {
				continue;
			}
			//definite collision
			
			//how far to push back each entity
			float pushBack = (rad - dist) / 2;

			//normal vector pointing from e2 to e1
			Vector2Divide(&diff, dist);

			//push back vector pointing from e2 to e1
			Vector2Scale(&diff, pushBack);

			e1->position = Vector2Add(e1->position, diff);
			e2->position = Vector2Subtract(e2->position, diff);
		}
	}
}

internal bool 
HandleTileCollisions(TileMap *gameMap, Entity *entity, TileTypes *tileTypes) 
{
	Vector2i currentTile = GetTileAtLocation(gameMap, entity->position);

	// if the entity's velocity is positive in the x it's moving right so start testing tiles to the left
	// if the entity's velocity is postive in the y it's moving down, so start testing tiles above it
	int x = (entity->velocity.x > 0) ? ((int)currentTile.x)-1 : ((int)currentTile.x)+1;
	int xDir = (entity->velocity.x > 0) ? 1 : -1;
	int y = (entity->velocity.y > 0) ? ((int)currentTile.y)-1 : ((int)currentTile.y)+1;
	int yDir = (entity->velocity.y > 0) ? 1 : -1;
	bool collided = false;

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
				collided = true;
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

	return collided;
}
