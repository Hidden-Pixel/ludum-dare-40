#include <stdlib.h>
#include "raymath.h"

#define global_variable static
#define internal	static
#define local_persist   static
#define LEVEL_SIZE 256
#define ISLAND_MAX_SIZE 30
#define ISLAND_MIN_SIZE 3

internal void
GenerateLevel(int numIslands, int paths, int level[256][256]) {
	
	int q, w;
	for (q = 0; q < LEVEL_SIZE; q++) {
		for (w = 0; w < LEVEL_SIZE; w++) {
			level[q][w] = 0;
		}
	}

	int islandProps[1000][10];
	int i;
	for (i = 0; i < numIslands; i++) {
		bool islandMade = false;
		do {
			bool overlap = false;
			int width = ISLAND_MIN_SIZE + rand() % (ISLAND_MAX_SIZE - ISLAND_MIN_SIZE);
			int height = ISLAND_MIN_SIZE + rand() % (ISLAND_MAX_SIZE - ISLAND_MIN_SIZE);
			int maxX = LEVEL_SIZE - width;
			int maxY = LEVEL_SIZE - height;
			int pX = rand() % maxX;
			int pY = rand() % maxY;
			int x, y;
			for (x = -1; x < width + 1; x++) {
				for (y = -1; y < height + 1; y++) {
					if (pX + x >= 0 && pX + x < LEVEL_SIZE && pY + y >= 0 && pY + y < LEVEL_SIZE) {
						if (level[pX + x][pY + y] > 0) {
							x = width;
							y = height;
							overlap = true;
							break;
						}
					}
				}
			}
			if (overlap) {
				continue;
			}


			for (x = 0; x < width; x++) {
				for (y = 0; y < height; y++) {
					level[pX + x][pY + y] = 1;
				}
			}

			islandMade = true;
		} while (!islandMade);
	}
	
	for (i = 0; i < paths; i++) {
		bool pathMade = false;
		do {
			int sX, sY;
			sX = rand() % LEVEL_SIZE;
			sY = rand() % LEVEL_SIZE;
			int dir = rand() % 4;
			
			if (level[sX][sY] == 0) {
				continue;
			}
			//initial point is on land
			bool newLandFound = false;
			bool leftOldLand = false;
			int iX = sX, iY = sY;
			do {
				switch (dir) {
				case 0:
					sX++;
					break;
				case 1:
					sY++;
					break;
				case 2:
					sX--;
					break;
				case 3:
					sY--;
					break;
				}
				if (sX < 0 || sX >= LEVEL_SIZE || sY < 0 || sY >= LEVEL_SIZE) {
					break;
				}
				if (level[sX][sY] > 0) {
					if (leftOldLand) {
						newLandFound = true;
					}
				}
				else
				{
					leftOldLand = true;
				}
			} while (!newLandFound);
			if (!newLandFound) {
				continue;
			}
			sX = iX;
			sY = iY;
			leftOldLand = false;
			newLandFound = false;
			//new land found make the path
			do {
				switch (dir) {
				case 0:
					sX++;
					break;
				case 1:
					sY++;
					break;
				case 2:
					sX--;
					break;
				case 3:
					sY--;
					break;
				}
				if (sX < 0 || sX >= LEVEL_SIZE || sY < 0 || sY >= LEVEL_SIZE) {
					break;
				}
				if (level[sX][sY] > 0) {
					if (leftOldLand) {
						newLandFound = true;
					}
				}
				else
				{
					level[sX][sY] = 3;
					leftOldLand = true;
				}
			} while (!newLandFound);
			pathMade = true;
		} while (!pathMade);
	}
}