/************************************************************************************
	ISL PSX LIBRARY	(c) 1999 Interactive Studios Ltd.

	isltex.c			Texture and VRAM management

************************************************************************************/
   
#include <stddef.h>
#include <string.h>
//ma#include <stdio.h>
#include "isltex.h"
#include "islutil.h"
#include "islfont.h"

extern psFont	*font;

#define MAXTEXBANKS			50

static TextureBankType *texBank[MAXTEXBANKS];

USHORT			*bitmapPtr,*twiddlePtr;

static int textureSetSPRPointers(NSPRITE *pHeader, short *maxWidth, short *maxHeight)
{
	int n = 0;
	
	*maxWidth = *maxHeight = 0;
	while(1)
	{
		pHeader[n].pal = (unsigned short *)((unsigned long)pHeader+((unsigned long)pHeader[n].pal));
		pHeader[n].image = (unsigned char *)((unsigned long)pHeader+((unsigned long)pHeader[n].image));
		if(*maxWidth < pHeader[n].w)
			*maxWidth = pHeader[n].w;
		if(*maxHeight < pHeader[n].h)
			*maxHeight = pHeader[n].h;
		if(pHeader[n].flags & NLASTSPR)
			return(++n);
		n++;
	}
}


/**************************************************************************
	FUNCTION:	textureLoadBank()
	PURPOSE:	Load texture bank
	PARAMETERS:	Filename
	RETURNS:	Ptr to texture bank info
**************************************************************************/

extern char texurestring[32];

TextureBankType *textureLoadBank(char *sFile)
{
 	TextureBankType	*newBank = NULL;
	int				loop,j,i;
	ULONG			tempCRC;
	int				filelength;
	int				num,width,height;
	NSPRITE			*pNSprite = NULL;
	int				numTextures;
	short			maxWidth,maxHeight;
	int				retry = 0;

	while((!pNSprite)&&(retry < 10))
	{
		pNSprite = (NSPRITE *)fileLoad(sFile, &filelength);
		if(pNSprite	== NULL)
			retry++;
	}
	numTextures = textureSetSPRPointers(pNSprite,&maxWidth,&maxHeight);

	bitmapPtr = Align32Malloc(maxWidth*maxHeight*sizeof(USHORT));
	twiddlePtr = Align32Malloc(maxWidth*maxHeight*sizeof(USHORT));

	// convert and load textures
	for(loop=0;loop<numTextures;loop++)
	{
//		if(utilStr2CRC("AI_CIRCLE") == pNSprite[loop].crc)
//		{
//			pNSprite[loop].flags |= NALPHA;
//		}	
		if(!((pNSprite[loop].flags & NSPLIT)||(pNSprite[loop].flags & NBITSTREAM)))
			loadNSPRITEIntoSurface(&pNSprite[loop]);
		else
			utilPrintf("Split texture !");
	}
		
	Align32Free(pNSprite);	
	Align32Free(bitmapPtr);
	Align32Free(twiddlePtr);

	return newBank;
}

/**************************************************************************
	FUNCTION:	textureDownloadFromBank()
	PURPOSE:	Upload/find given texture in bank
	PARAMETERS:	Texture bank info ptr, index of texture
	RETURNS:	Sprite info ptr
**************************************************************************/

TextureType *textureDownloadFromBank(TextureBankType *bank, int idx)
{
	long	b,y;
	NSPRITE	*pNSprite = bank->pNSprite;

	b = 1<<(idx & 7);
	y = idx>>3;
	if(!(bank->used[y] & b))
	{
		if(pNSprite != NULL)
		{
			bank->used[y] |= b;
//			textureDownLoad(pNSprite+idx, (bank->texture)+idx);
		}
		else
			return NULL;
	}
	return ((bank->texture)+idx);
}


/**************************************************************************
	FUNCTION:	textureDownloadBank()
	PURPOSE:	Download entire texture bank to VRAM
	PARAMETERS:	Texture bank info ptr
	RETURNS:	
**************************************************************************/

void textureDownloadBank(TextureBankType *bank)
{
	int	loop;

	for(loop=0; loop<bank->numTextures; loop++)
		textureDownloadFromBank(bank, loop);
}


/**************************************************************************
	FUNCTION:	textureDestroyBank()
	PURPOSE:	Free system RAM data for given texture bank
	PARAMETERS:	Texture bank info ptr
	RETURNS:	
**************************************************************************/

void textureDestroyBank(TextureBankType *bank)
{
	if(bank==NULL)
		return;

	Align32Free(bank->pNSprite);
	bank->pNSprite = NULL;
}


/**************************************************************************
	FUNCTION:	textureUnloadBank()
	PURPOSE:	Unload given texture bank from VRAM
	PARAMETERS:	Texture bank info ptr
	RETURNS:	
**************************************************************************/

/*
void textureUnloadBank(TextureBankType *bank)
{
	unsigned long	loop, y,b, numTex;
	
	if (bank==NULL)
		return;

	// free up texture space in vram
	numTex = bank->numTextures;
	for(loop=0; loop<numTex; loop++)
	{
		b = 1<<(loop & 7);
		y = loop>>3;
		if (bank->used[y] & b)
			textureUnload(bank->texture+loop);
	}

	// remove internal reference to bank

	for(loop=0; loop<MAXTEXBANKS; loop++)
		if (texBank[loop]==bank)
			texBank[loop] = NULL;

	// free up ram space
	syFree(bank->texture);
	syFree(bank->CRC);
	syFree(bank);
}
*/

/**************************************************************************
	FUNCTION:	textureFindCRCInBank()
	PURPOSE:	Find given texture in bank
	PARAMETERS:	Texture bank info ptr, Texture name CRC
	RETURNS:	Texture info ptr
**************************************************************************/

TextureType *textureFindCRCInBank(TextureBankType *bank, unsigned long crc)
{
	unsigned long	numTextures = bank->numTextures;
	unsigned long	loop;

	for(loop=0; loop<numTextures; loop++)
	{
		if (bank->CRC[loop] == crc)
			return textureDownloadFromBank(bank, loop);
	}
	return NULL;
}


/**************************************************************************
	FUNCTION:	textureFindCRCInAllBanks()
	PURPOSE:	Find given texture in all loaded banks
	PARAMETERS:	Texture name CRC
	RETURNS:	Texture info ptr
**************************************************************************/

TextureType *textureFindCRCInAllBanks(unsigned long crc)
{
	int			loop;
	TextureType	*txPtr = NULL;

	for(loop=0; loop<MAXTEXBANKS; loop++)
	{
		if (texBank[loop]!=NULL)
		{
			txPtr = textureFindCRCInBank(texBank[loop], crc);
			if (txPtr!=NULL)
				return txPtr;
		}
	}
	return txPtr;
}


/**************************************************************************
	FUNCTION:	textureReallocTextureBank()
	PURPOSE:	Reallocate texture in system RAM (e.g. to defragement)
	PARAMETERS:	Texture bank info ptr
	RETURNS:	New texture bank info ptr
**************************************************************************/

TextureBankType *textureReallocTextureBank(TextureBankType *txBank)
{
	TextureBankType	*textureBank;
	int				loop;
	TextureType		*txPtr;

	textureBank = Align32Malloc(sizeof(TextureBankType));
	textureBank->numTextures = txBank->numTextures;
	
	txPtr = Align32Malloc(sizeof(TextureType) * txBank->numTextures);
	memcpy(txPtr, txBank->texture, sizeof(TextureType)*txBank->numTextures);
	textureBank->texture = txPtr;

	textureBank->CRC = (unsigned long *)Align32Malloc(textureBank->numTextures*4+(textureBank->numTextures/8)+1);
	memcpy(textureBank->CRC, txBank->CRC, textureBank->numTextures*4+(textureBank->numTextures/8)+1);

	textureBank->pNSprite = txBank->pNSprite;

	textureBank->used = (unsigned char *)(textureBank->CRC+textureBank->numTextures);
	memcpy(textureBank->used, txBank->used, (textureBank->numTextures/8)+1);

	syFree(txBank->texture);
	syFree(txBank->CRC);
	syFree(txBank);

	for(loop = 0; loop < MAXTEXBANKS; loop ++)
	{
		if(texBank[loop] == txBank)
		{
			texBank[loop] = textureBank;
			break;
		}
	}

	return textureBank;
}


/**************************************************************************
	FUNCTION:	textureCreateAnimation
	PURPOSE:	Create an animated texture
	PARAMETERS:	pointer to dummy texture, pointer to array of animated textures, number of frames
	RETURNS:	pointer to animated texture
**************************************************************************/

TextureAnimType *textureCreateAnimation(TextureType *dummy, TextureType **anim, int numFrames)
{
	TextureAnimType	*texAnim;
	int				loop;

	texAnim = Align32Malloc(sizeof(TextureAnimType));

	texAnim->dest = dummy;

	texAnim->anim = Align32Malloc(sizeof(TextureType) * numFrames);

	for(loop = 0; loop < numFrames; loop ++)
	{
		texAnim->anim[loop] = anim[loop];
	}

	return texAnim;
}


/**************************************************************************
	FUNCTION:	textureSetAnimation
	PURPOSE:	Set frame of an animated texture
	PARAMETERS:	Animated texture, frame number
	RETURNS:	
**************************************************************************/

void textureSetAnimation(TextureAnimType *texAnim, int frameNum)
{
/*	DR_MOVE *siMove;
	RECT	moveRect;

	moveRect.x = VRAM_CALCVRAMX(texAnim->anim[frameNum]->handle);
	moveRect.y = VRAM_CALCVRAMY(texAnim->anim[frameNum]->handle);
	moveRect.w = (texAnim->dest->w + 3) / 4;
	moveRect.h = texAnim->dest->h;

	// check for 256 colour mode
	if(texAnim->dest->tpage & (1 << 7))
		moveRect.w *= 2;

	// copy bit of vram
	BEGINPRIM(siMove, DR_MOVE);
	SetDrawMove(siMove, &moveRect, VRAM_CALCVRAMX(texAnim->dest->handle),VRAM_CALCVRAMY(texAnim->dest->handle));
	ENDPRIM(siMove, 1023, DR_MOVE);
*/	
}


/**************************************************************************
	FUNCTION:	textureDestroyAnimation
	PURPOSE:	Destroy an animated texture
	PARAMETERS:	Animated texture
	RETURNS:	
**************************************************************************/

void textureDestroyAnimation(TextureAnimType *texAnim)
{
	syFree(texAnim->anim);
	syFree(texAnim);
}
