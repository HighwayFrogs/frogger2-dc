#include "include.h"

#ifndef _TEXTURES_H_INCLUDE
#define _TEXTURES_H_INCLUDE

#define MAX_TEXTURE_BANKS		5

typedef struct _TEXTUREANIM
{
	struct _TEXTUREANIM *next, *prev;			// ptr to next / prev platform
	unsigned short numFrames, frame;
	unsigned long lastTime;
	char txtName[20];
	TextureAnimType *animation;
} TEXTUREANIM;

typedef struct _TEXTUREANIMLIST
{
	int				numEntries;
	TEXTUREANIM	head;
} TEXTUREANIMLIST;

extern char *palNames [ 5 ];

extern TEXTUREANIMLIST textureAnimList;

extern TextureBankType *textureBanks [ MAX_TEXTURE_BANKS ];

void LoadTextureBank			( int textureBank );
void LoadTextureAnimBank ( int textureBank );
void FreeTextureBank			( TextureBankType *textureBank );
void FreeAllTextureBanks	( void );

TEXTUREANIM *CreateTextureAnimation ( long crc, int numframes );
//void CreateTextureAnimation			( char *fileName, TextureType *dummy, int numFrames );
void InitTextureAnimLinkedList	( void );
void FreeTextureAnimList				( void );
void AddTextureAnim							( TEXTUREANIM *textureAnim );
void SubTextureAnim							( TEXTUREANIM *textureAnim );
void AddAnimFrame ( TEXTUREANIM *anim, long crc, short waitTime, int num );

void CopyTexture(TextureType *source, TextureType *dest, int num);

#endif