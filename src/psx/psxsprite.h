#ifndef _PSXSPRITE_H_INCLUDE
#define _PSXSPRITE_H_INCLUDE

void PrintSpriteOverlays ( char num );
void InitTiledBackdrop(char *filename);
void DrawTiledBackdrop(int scroll);
void FreeTiledBackdrop();

extern  TextureType *tileTexture[4];

#endif