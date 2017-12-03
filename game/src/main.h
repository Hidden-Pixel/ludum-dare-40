/*
 * main.h
 *
 */
#ifndef __MAIN_H__
#define __MAIN_H__ 1

#include "raylib.h"

#define global_variable     static
#define internal            static
#define local_persist       static

#define len(array)(sizeof(array)/sizeof(array[0]))
#define len2d(array)(sizeof(array[0])/sizeof(array[0][0]))
#define assert(expression) if(!(expression)) {*(int *)0 = 0;}
#define NotImplemented assert(!"NotImplemented")

#define LEVEL_SIZE 256

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