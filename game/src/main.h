/*
 * main.h
 *
 */

#ifndef __MAIN_H__
#define __MAIN_H__ 1

#define global_variable     static
#define internal            static
#define local_persist       static

#define len(array)(sizeof(array)/sizeof(array[0]))
#define len2d(array)(sizeof(array[0])/sizeof(array[0][0]))
#define assert(expression) if(!(expression)) {*(int *)0 = 0;}
#define NotImplemented assert(!"NotImplemented")
#define InvalidCodePath assert(!"InvalidCodePath")

//----------------------------------------------------------------------------------
// Defines
//----------------------------------------------------------------------------------
#define PLAYER_INDEX            0
#define PLAYER_BASE_SIZE        20.0f
#define PLAYER_SPEED            8.0f
#define PLAYER_SPEED_INCREMENT  0.25f
#define PLAYER_SPEED_DECAY      0.95f
#define PLAYER_BASE_HEALTH      100
#define PLAYER_BASE_DAMAGE      20

#define ENEMY_DEFAULT_SIZE              20.0f
#define ENEMY_DEFAULT_SPEED             1.5f
#define ENEMY_DEFAULT_SPEED_INCREMENT   0.25f
#define ENEMY_DEFAULT_SPEED_DECAY       0.97f
#define ENEMY_BASE_HEALTH               100
#define ENEMY_BASE_DAMAGE               5
#define ENEMY_HEALTH                    3

#define BULLET_DEFAULT_SIZE 5.0f
#define BULLET_DEFAULT_SPEED 10.0f

#define LEVEL_SIZE 100

#define FRAME_RATE_TARGET 60

#ifndef max
internal inline int
max (int a, int b) 
{
	return (a > b) ? a : b;
}
#endif

#ifndef min
internal inline int 
min(int a, int b) 
{
	return (a < b) ? a : b;
}
#endif

typedef struct _screen
{
	int width;
	int height;
} Screen;

typedef struct _titleMap
{
    int map[LEVEL_SIZE][LEVEL_SIZE];
	int tileHeight;
	int tileWidth;
} TileMap;

typedef struct _tileProps
{
	Color color;
	bool wall;
} TileProps;

typedef struct _tileTypes
{
	TileProps tiles[20];
} TileTypes;

#endif
