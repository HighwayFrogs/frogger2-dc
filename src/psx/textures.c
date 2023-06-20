
#include "include.h"
#include "textures.h"
#include "main.h"

int numTextureBanks = 0;

char *palNames [ 5 ] = { "BFG01", "BFG02", "BFG03", "BFG04", "BFG05"};
//char *palNames [ 5 ] = { "GREENBABY.PAL", "YELLOWBABY.PAL", "BLUEBABY.PAL", "PINKBABY.PAL", "REDBABY.PAL"};


TEXTUREANIMLIST textureAnimList;

TextureBankType *textureBanks [ MAX_TEXTURE_BANKS ] = { NULL, NULL, NULL, NULL, NULL };

//void CreateTextureAnimation( char *fileName, TextureType *dummy, int numFrames );
//void InitTextureAnimLinkedList( void );
//void FreeTextureAnimList( void );
//void AddTextureAnim( TEXTUREANIM *textureAnim );
//void SubTextureAnim( TEXTUREANIM *textureAnim );

void LoadTextureBank ( int textureBank )
{
	int fileLength, counter, counter1;
	char fileName[256];
	char titFileName[256];
	unsigned char *p, *textureAnims;
	
	TEXTUREANIM *textureAnim;

	short numAnimations = 0;

	titFileName[0] = 0;
	
	switch ( textureBank )
	{
		case GARDEN_TEX_BANK: sprintf ( fileName, "TEXTURES\\GARDEN.SPT" );	break;
		case ANCIENT_TEX_BANK: sprintf ( fileName, "TEXTURES\\ANCIENTS.SPT" ); break;
		case SPACE_TEX_BANK: sprintf ( fileName, "TEXTURES\\SPACE.SPT" ); break;
		case CITY_TEX_BANK: sprintf ( fileName, "TEXTURES\\CITY.SPT" ); break;
		case SUBTERRANEAN_TEX_BANK: sprintf ( fileName, "TEXTURES\\SUB.SPT" ); break;
		case LABORATORY_TEX_BANK: sprintf ( fileName, "TEXTURES\\LAB.SPT" ); break;
		case HALLOWEEN_TEX_BANK: sprintf ( fileName, "TEXTURES\\HALLOWEEN.SPT" ); break;
		case SUPERRETRO_TEX_BANK: sprintf ( fileName, "TEXTURES\\SUPER.SPT" ); break;
		case FRONTEND_TEX_BANK: sprintf ( fileName, "TEXTURES\\HUB.SPT" ); break;
		case INGAMEGENERIC_TEX_BANK: sprintf ( fileName, "TEXTURES\\GENERIC.SPT" ); break;
		case TITLES_TEX_BANK: sprintf ( fileName, "TEXTURES\\TITLES.SPT" ); break;
		case LOADING_TEX_BANK: sprintf ( fileName, "TEXTURES\\LOADING.SPT" ); break;
//		case LOADING_TEX_BANK: return;
	}

	if ( numTextureBanks < MAX_TEXTURE_BANKS )
	{
		textureLoadBank(fileName);
//		textureBanks [ numTextureBanks ] = textureLoadBank ( fileName );
//		textureDownloadBank ( textureBanks [ numTextureBanks ]   );
//		textureDestroyBank  ( textureBanks [ numTextureBanks++ ] );
	}
	else
	{
		utilPrintf("Error Loading: %s, Max Texture Banks Reached", fileName );
	}
}

void LoadTextureAnimBank ( int textureBank )
{
	TextureType *dummyTexture;
	int fileLength, counter, counter1, numframes, waitTime;
	long crc, dummyCrc;
	char fileName[256], dummyString [ 256 ];
	char titFileName[256];
	unsigned long *p, *textureAnims;

	TEXTUREANIM *textureAnim;
	short numAnimations = 0;

	switch ( textureBank )
	{
		case GARDEN_TEX_BANK: sprintf ( titFileName, "GARDEN.TIT" ); break;
		case ANCIENT_TEX_BANK: sprintf ( titFileName, "ANCIENTS.TIT" ); break;
		case SPACE_TEX_BANK: sprintf ( titFileName, "SPACE.TIT" ); break;
		case SUBTERRANEAN_TEX_BANK: sprintf ( titFileName, "SUB.TIT" ); break;
		case LABORATORY_TEX_BANK: sprintf ( titFileName, "LAB.TIT" ); break;
		case HALLOWEEN_TEX_BANK: sprintf ( titFileName, "HALLOWEEN.TIT" ); break;
		case SUPERRETRO_TEX_BANK: sprintf ( titFileName, "RETRO.TIT" ); break;
		case FRONTEND_TEX_BANK: sprintf ( titFileName, "HUB.TIT" ); break;
		case INGAMEGENERIC_TEX_BANK: sprintf ( titFileName, "GENERIC.TIT" ); break;
		default: return;
	}

	textureAnims = (unsigned long *)FindStakFileInAllBanks ( titFileName, &fileLength );

	if ( !textureAnims )
		return;
	
	p = textureAnims;
	numAnimations = *p; p++;

	for ( counter = 0; counter < numAnimations; counter++ )
	{
		numframes = *p;	p++;
		crc = *p; p++;

		textureAnim = CreateTextureAnimation( crc, numframes );

		for ( counter1 = 0; counter1 < numframes; counter1++ )
		{
			crc = *p;
			p++;
			waitTime = *p;

			p++;
			AddAnimFrame ( textureAnim, crc, waitTime, counter1 );
		}
	}
}

void FreeTextureBank(TextureBankType *textureBank)
{
//	textureDestroyBank(textureBank);
//	textureUnloadBank(textureBank);
}

void FreeAllTextureBanks(void)
{
	int i,c;
	KMSTATUS status;
	PKMUINT32 pSizeOfTexture;
	PKMUINT32 pMaxBlockSizeOfTexture;

	utilPrintf("Freeing All Texture Banks : TEXTURE\n");

//	kmGetFreeTextureMem(&pSizeOfTexture,&pMaxBlockSizeOfTexture);

	for(c=0;c<DCKnumTextures;c++)
	{
		status = kmFreeTexture(&DCKtextureList[c].surface);
		if(status != KMSTATUS_SUCCESS)
			i=c;
	}
	DCKnumTextures = 0;	

//	kmGetFreeTextureMem(&pSizeOfTexture,&pMaxBlockSizeOfTexture);
	
	for(c=0;c<MAX_TEXTURE_BANKS;c++)
	{
//ma		if(textureBanks[c])
//ma			FreeTextureBank(textureBanks [c]);
		// ENDIF - if ( textureBanks[c] )
		textureBanks[c] = NULL;
	}
	// ENDIF - 	for ( c = o; c < MAX_TEXTURE_BANKS; c++ )
	numTextureBanks = 0;
	//textureDestroy();
}
TEXTUREANIM *CreateTextureAnimation ( long crc, int numframes )
{
	int fileLength, counter, n;

	unsigned char *p;

	char line[255];
	char tempName[255];

	signed char type[20];

	TEXTUREANIM *textureAnim = NULL;

	textureAnim = MALLOC0 ( sizeof ( TEXTUREANIM ) );

	if ( !textureAnim )
	{
		utilPrintf ( "Could Not Malloc Texture Anim.\n" );
		return NULL;
	}
	// ENDIF

	AddTextureAnim ( textureAnim );

	textureAnim->numFrames = numframes;
	textureAnim->frame = 0;
	textureAnim->lastTime = 0;
	textureAnim->animation = MALLOC0 ( sizeof(TextureAnimType) + (sizeof(TextureType *) * textureAnim->numFrames ) );

	if ( !textureAnim->animation )
	{
		utilPrintf ( "Could Not Malloc Animation.\n" );
		FREE ( textureAnim );
		return NULL;
	}
	// ENDIF

	textureAnim->animation->dest = FindTexture2(crc);
	// find texture failed.. use default anim texture..
	if (textureAnim->animation->dest == NULL)
		textureAnim->animation->dest = &DCKtextureList[0];
	textureAnim->animation->dest->animated = TRUE;

	textureAnim->animation->anim = (TextureType **)((unsigned char *)textureAnim->animation + sizeof(TextureAnimType));
	textureAnim->animation->waitTimes = (short *)MALLOC0( sizeof(short)*numframes );

/*	for ( counter = 0; counter < textureAnim->numFrames; counter++ )
	{
		utilPrintf("Counter : %d\n", counter );

		if ( counter < 9 )
			sprintf( type, "%s%d", fileName, counter+1 );
		else
			sprintf( type, "%s%d", fileName, counter+1 );

		utilPrintf("Trying To Load Texture : %s\n", type );
//		textureAnim->animation->anim [ counter ] = textureFindCRCInAllBanks ( utilStr2CRC ( type ) );

		if ( !textureAnim->animation->anim [ counter ] )
			utilPrintf("Could Not Find Texture : %s\n", type );
	}
	// ENDIF*/

	return textureAnim;
}


void AddAnimFrame ( TEXTUREANIM *anim, long crc, short waitTime, int num )
{
	anim->animation->waitTimes[num] = waitTime;
//	anim->animation->anim [ num ] = textureFindCRCInAllBanks ( crc );
	anim->animation->anim [ num ] = FindTexture2(crc);
	if ( !anim->animation->anim [ num ] )
	{
		anim->animation->anim [ num ] = &DCKtextureList[0];
		utilPrintf("Could Not Find Texture : %lu\n", crc );
	}
	// ENDIF
}

void UpdateTextureAnimations ( void )
{
	DR_MOVE *siMove;
	RECT	moveRect;

	TEXTUREANIM *cur,*next;

	if ( textureAnimList.numEntries == 0 )
		return;
	
	for ( cur = textureAnimList.head.next; cur != &textureAnimList.head; cur = cur->next )
	{
		if( cur->numFrames > 1 )
		{
			// If special waitTime then advance frame by 1
			if( cur->animation->waitTimes[cur->frame] == -1 )
			{
				cur->lastTime++;

				cur->frame++;
				if( cur->frame >= cur->numFrames )
				{
					cur->frame = 0;
				}
			}
			else
			{
				while( cur->lastTime + cur->animation->waitTimes[cur->frame] < actFrameCount )
				{
					cur->lastTime += cur->animation->waitTimes[cur->frame];
					
					cur->frame++;
					if( cur->frame >= cur->numFrames )
						cur->frame = 0;
				}
			}
			// JH: Copy the required texture into vram.

			cur->animation->dest->surfacePtr = &cur->animation->anim[cur->frame]->surface;
//			CopyTexture ( cur->animation->dest, cur->animation->anim[cur->frame], 0 );
		}
		
	}
}

void InitTextureAnimLinkedList ( void )
{
	textureAnimList.numEntries = 0;
	textureAnimList.head.next = textureAnimList.head.prev = &textureAnimList.head;
}

void AddTextureAnim ( TEXTUREANIM *textureAnim )
{
	if ( textureAnim->next == NULL )
	{
		textureAnimList.numEntries++;
		textureAnim->prev								= &textureAnimList.head;
		textureAnim->next								= textureAnimList.head.next;
		textureAnimList.head.next->prev	= textureAnim;
		textureAnimList.head.next				=	textureAnim;
	}
}

void FreeTextureAnimList ( void )
{
	TEXTUREANIM *cur,*next;

	if( !textureAnimList.numEntries )
		return;

	for ( cur = textureAnimList.head.next; cur != &textureAnimList.head; cur = next )
	{
		next = cur->next;
		SubTextureAnim(cur);
	}
//	FREE(specFXList.head);
}


void SubTextureAnim ( TEXTUREANIM *textureAnim )
{
	if(textureAnim->next == NULL)
		return;

	textureAnim->prev->next = textureAnim->next;
	textureAnim->next->prev = textureAnim->prev;
	textureAnim->next = NULL;
	textureAnimList.numEntries--;

	FREE(textureAnim);
}

/*
TextureType *CreateSpareTextureSpace ( long dummyCrc )
{
	RECT			rect;
	DR_MOVE *siMove;
	RECT	moveRect;

	TextureType *texture, *dummy;

	dummy = ( TextureType* ) MALLOC0 ( sizeof ( TextureType ) );

	texture = textureFindCRCInAllBanks ( dummyCrc );

	if ( !texture )
	{
		utilPrintf ("Could Not Find Replacement Texture.....\n");
		return NULL;
	}
	// ENDIF - !texture

	dummy->handle = textureVRAMalloc ( texture->w, texture->h );

	if ( !dummy->handle )
	{
		utilPrintf("Could Not VRAM Alloc : Width : %d : Height : %d\n", texture->w, texture->h );
		return NULL;
	}
	// ENDIF

	rect.x = VRAM_CALCVRAMX(dummy->handle);
	rect.y = VRAM_CALCVRAMY(dummy->handle);
	rect.w = (texture->w+3)/4;
	rect.h = texture->h;

	dummy->x = VRAM_GETX ( dummy->handle ) * 8;
	dummy->y = rect.y;

	dummy->w = texture->w;
	dummy->h = texture->h;

  dummy->u0 = texture->x;
  dummy->v0 = texture->y;
  dummy->u1 = 255;
  dummy->v1 = texture->y;
  dummy->u2 = texture->x;
  dummy->v2 = 255;
  dummy->u3 = 255;
  dummy->v3 = 255;

	dummy->imageCRC = dummyCrc;

	dummy->tpage = getTPage(
		((texture->flags & NEIGHTBIT) ? 1 : 0),
		0, rect.x, rect.y);

	moveRect.x = VRAM_CALCVRAMX(dummy->handle);
	moveRect.y = VRAM_CALCVRAMY(dummy->handle);
	moveRect.w = dummy->w;
	moveRect.h = dummy->h;

	// check for 256 colour mode
	if(dummy->tpage & (1 << 7))
		moveRect.w *= 2;

	BEGINPRIM		( siMove, DR_MOVE );
	SetDrawMove	( siMove, &moveRect, VRAM_CALCVRAMX(texture->handle),VRAM_CALCVRAMY(texture->handle));
	ENDPRIM			( siMove, 1023, DR_MOVE );


	return dummy;
}
*/

void CopyTexture(TextureType *source, TextureType *dest, int num)
{
	source->surfacePtr = &dest->surface;
	source->animated = TRUE;
}
