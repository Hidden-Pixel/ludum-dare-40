/*
 * main.h
 *
 */

#define global_variable     static
#define internal            static
#define local_persist       static

#define len(array)(sizeof(array)/sizeof(array[0]))
#define len2d(array)(sizeof(array[0])/sizeof(array[0][0]))
#define assert(expression) if(!(expression)) {*(int *)0 = 0;}
#define NotImplemented assert(!"NotImplemented")

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

typedef struct _tileProps
{
	Color color;
	bool wall;
} TileProps;

typedef struct _tileTypes
{
	TileProps tiles[20];
} TileTypes;
