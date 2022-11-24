#include "include.h"

BFF_Header* level_bff = NULL;
FMA_WORLD* fma_world = NULL;

void DrawWorld()
{
	MapDraw_DrawFMA_World(fma_world);
}

