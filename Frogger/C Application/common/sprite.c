/*

	This file is part of Frogger2, (c) 1999 Interactive Studios Ltd.


	File		: sprite.c
	Programmer	: Jim Hubbard
	Date		: 25/02/00

----------------------------------------------------------------------------------------------- */

// *ASL* 10/08/2000 - Use the latest Kamui macros
#define _KM_USE_VERTEX_MACRO_
#define _KM_USE_VERTEX_MACRO_L5_

#include <shinobi.h>
#include <kamui2.h>
#include <kamuix.h>


#define F3DEX_GBI_2

#include <isltex.h>
#include <islmem.h>
#include <islutil.h>
//ma#include <stdio.h>

//#include "ultra64.h"
#include "sprite.h"
#ifdef PSX_VERSION
#include "textures.h"
#include "specfx.h"
#endif
#include "maths.h"
#include "newpsx.h"
#include "Shell.h"
#include "Main.h"
#include "gte.h"


#ifdef PC_VERSION
#define MAX_SPRITES	512
#else
#define MAX_SPRITES	512
#endif

#ifdef PSX_VERSION
int SCALEX, SCALEY;
#endif

/*	--------------------------------------------------------------------------------
	Function		: AllocateSprites
	Purpose			: Find a number of sprites and return a sublist
	Parameters		: number of sprites
	Returns			: pointer to first one
	Info			: 
*/
SPRITE *AllocateSprites( int number )
{
	SPRITE *s;
	// Return if allocation is impossible for any reason
	if( (number <= 0) || (sprList.stackPtr-number < 0) || (number >= MAX_SPRITES-sprList.count) ) return NULL;

	// Now we can go and allocate sprites with gay abandon
	while( number-- )
	{
		s = sprList.stack[sprList.stackPtr--];

		s->prev	= &sprList.head;
		s->next	= sprList.head.next;
		sprList.head.next->prev	= s;
		sprList.head.next = s;
		s->draw = 1;

		sprList.count++;
	}

	return sprList.stack[sprList.stackPtr+1];
}


/*	--------------------------------------------------------------------------------
	Function		: DeallocateSprites
	Purpose			: Remove sprites from list and flag as unused
	Parameters		: number of sprites
	Returns			: 
	Info			: 
*/
void DeallocateSprites( SPRITE *head, int number )
{
	SPRITE *s=head, *t;

	if( !s || !s->next || (number<=0) || (sprList.stackPtr+number >= MAX_SPRITES) ) return;

	while( number-- )
	{
		t = s->next;

		s->prev->next	= s->next;
		s->next->prev	= s->prev;
		s->next = s->prev = NULL;
		s->draw = 0;

		sprList.count--;
		sprList.stack[++sprList.stackPtr] = s;

		s = t;
	}
}


/*	--------------------------------------------------------------------------------
	Function		: FreeSpriteList
	Purpose			: Deallocate all sprites and set list to empty
	Parameters		: 
	Returns			: 
	Info			: 
*/
void FreeSpriteList( )
{
	int i;

	if( sprList.array )
	{
		// Remove all sprites in array from sprite list so they don't get removed after deallocation
		for( i=0; i<MAX_SPRITES; i++ )
		{
			if( !sprList.array[i].next ) continue;

			sprList.array[i].prev->next	= sprList.array[i].next;
			sprList.array[i].next->prev	= sprList.array[i].prev;
			sprList.array[i].next = NULL;
			sprList.array[i].draw = 0;
		}

		FREE( sprList.array );
		sprList.array = NULL;
	}

	if( sprList.stack )
	{
		FREE( sprList.stack );
		sprList.stack = NULL;
	}

	sprList.count = 0;
	sprList.stackPtr = 0;
}


/*	--------------------------------------------------------------------------------
	Function		: InitSpriteList
	Purpose			: Set up static array and some bits of data.
	Parameters		: 
	Returns			: 
	Info			: 
*/
void InitSpriteList( )
{
	int i;

	sprList.head.next = sprList.head.prev = &sprList.head;

	// Allocate a big bunch of sprites
	sprList.array = (SPRITE *)MALLOC0( sizeof(SPRITE)*MAX_SPRITES );
	sprList.stack = (SPRITE **)MALLOC0( sizeof(SPRITE*)*MAX_SPRITES );

	// Initially, all sprites are available
	for( i=0; i<MAX_SPRITES; i++ )
		sprList.stack[i] = &sprList.array[i];

	sprList.count = 0;
	sprList.stackPtr = i-1;
	
#ifdef PSX_VERSION
//	if( PALMODE )
//		SCALEY = 4096;
//	else
//		SCALEY = 3840;
	SCALEY = 900*1.35;
	SCALEX = 900*1.35;
#endif
	
}


#ifdef PSX_VERSION

extern int spriteRotNum;
extern int spriteNum;

#define MAX_SPRITEWIDTH		160

int Print3DSprite(SPRITE *spr);
int Print3DSpriteRotating(SPRITE *spr);

KMSURFACEDESC *lastSurface;


/* --------------------------------------------------------------------------------
   Function	: PrintSprites
   Purpose : print all sprites
   Parameters : 
   Returns : 
   Info	: 
*/

void PrintSprites()
{
	SPRITE *cur;
	unsigned long colour;

	for (cur = sprList.head.next; cur != &sprList.head; cur = cur->next)
	{
		if (!cur)
			break;
		if (!cur->draw)
			continue;
		if (!(cur->flags & SPRITE_FLAGS_ROTATE))
		{
			Print3DSprite(cur);
		}
		else
		{
			if (gameState.mode != PAUSE_MODE)
			{
				cur->angle += (cur->angleInc * gameSpeed)>>12;
				if (cur->angle >= 4096) 
					cur->angle -= 4096;
				else if (cur->angle < 0)
					cur->angle += 4096;
			}
			Print3DSpriteRotating(cur);
		}
 	}
}


/* --------------------------------------------------------------------------------
   Function	: Print3DSprite
   Purpose : print a 3d sprite
   Parameters : sprite structure pointer
   Returns : 1 sprite printed, else 0
   Info	: 
*/

int Print3DSprite(SPRITE *spr)
{
	static POLY_FT4 *pp;

	LONG		width;
	LONG		height;
	LONG 		distancescale;
	LONG 		spritez;
	long		x0,y0,z0;
	long		x1,y1,z1;
	long		x2,y2,z2;
	long		x3,y3,z3;
	VERT 		tempVect;
	uchar		r,g,b;
	KMUINT32	rgba;

	KMSTRIPHEAD	*strip;

	tempVect.vx = -spr->pos.vx;
	tempVect.vy = -spr->pos.vy;
	tempVect.vz = spr->pos.vz;

// 	pp = (POLY_FT4 *)(currentDisplayPage->primPtr);
// 	setPolyFT4(pp);
// 	setRGB0(pp, (colour&255),((colour>>8)&255),((colour>>16)&255));

	width = (spr->texture->w);
	gte_SetLDDQB(0);			// clear offset control reg (C2_DQB)
	gte_ldv0(&tempVect)	;		//vert);
	gte_SetLDDQA(width);		// shove sprite width into control reg (C2_DQA)
	gte_rtps();					// do the rtps
	x0 = screenxy[2].vx;
	y0 = screenxy[2].vy;
	gte_stsz(&spritez);		//depth);		// get order table z

	if((spritez < 20) || (spritez >= fog.max)) 
		return 0;

	width = (spr->scaleX*SCALEX) / spritez;
	if(width < 2 || width > MAX_SPRITEWIDTH ) // To stop very small (flickery) and very big (slow) sprites
		return 0;	// better max-distance check // JIM: Not really. A good thing to do but not a substitute for an actual distance check.

 	x1 = x3 = x0 + width;
 	x0 = x2 = x0 - width;
   	if(x1 < 0)
   		return 0;
   	if(x0 > 640)
   		return 0;

	// Scaled height of sprite. This is different from width - probably due to screen res.
	// NOTE: Random multiply that looks more or less OK :)
	height = (spr->scaleY*SCALEY) / spritez;

	y2 = y3 = y0 + height;
	y1 = y0 = y0 - height;
   	if(y2 < 0)
   		return 0;
   	if(y0 > 480)
   		return 0;

	r = spr->r;	
	g = spr->g;	
	b = spr->b;	
	
	if(spr->flags & SPRITE_TRANSLUCENT )//Only do additive if the 'sprite' fades or is translucent
	{
		// SL: modge the RGBs accordingly
	  	r = ((spr->a*(short)spr->r)/255);
	  	g = ((spr->a*(short)spr->g)/255);
	  	b = ((spr->a*(short)spr->b)/255);
	}
	if(spr->flags & SPRITE_SUBTRACTIVE )//Subtractive for shadows
	{
		// SL: modge the RGBs accordingly
	  	r = ((spr->a*(short)spr->r)/255);
	  	g = ((spr->a*(short)spr->g)/255);
	  	b = ((spr->a*(short)spr->b)/255);
	}
	if(spr->flags & SPRITE_ADDITIVE )//Subtractive for shadows
	{
		// SL: modge the RGBs accordingly
	  	r = ((spr->a*(short)spr->r)/255);
	  	g = ((spr->a*(short)spr->g)/255);
	  	b = ((spr->a*(short)spr->b)/255);
	}
	
	rgba = RGBA(r,g,b,spr->a);

	spritez = spritez >> 2;

	vertices_GT4[0].fX = (float)x0;
	vertices_GT4[0].fY = (float)y0;
	vertices_GT4[0].u.fZ = 1.0 / ((float)spritez);
	vertices_GT4[0].fU = (float)1;
	vertices_GT4[0].fV = (float)0;
	vertices_GT4[0].uBaseRGB.dwPacked = rgba;

	vertices_GT4[1].fX = (float)x1;
	vertices_GT4[1].fY = (float)y1;
	vertices_GT4[1].u.fZ = 1.0 / ((float)spritez);
	vertices_GT4[1].fU = (float)0;
	vertices_GT4[1].fV = (float)0;
	vertices_GT4[1].uBaseRGB.dwPacked = rgba;

	vertices_GT4[2].fX = (float)x2;
	vertices_GT4[2].fY = (float)y2;
	vertices_GT4[2].u.fZ = 1.0 / ((float)spritez);
	vertices_GT4[2].fU = (float)1;
	vertices_GT4[2].fV = (float)1;
	vertices_GT4[2].uBaseRGB.dwPacked = rgba;

	vertices_GT4[3].fX = (float)x3;
	vertices_GT4[3].fY = (float)y3;
	vertices_GT4[3].u.fZ = 1.0 / ((float)spritez);
	vertices_GT4[3].fU = (float)0;
	vertices_GT4[3].fV = (float)1;
	vertices_GT4[3].uBaseRGB.dwPacked = rgba;

	if(spr->texture->colourKey)
	{
		kmChangeStripTextureSurface(&StripHead_Sprites, KM_IMAGE_PARAM1, spr->texture->surfacePtr);
		kmStartStrip(&vertexBufferDesc, &StripHead_Sprites);
	}
	else
	{
		if(spr->flags & SPRITE_SUBTRACTIVE)
		{
			kmChangeStripTextureSurface(&StripHead_Sprites_Sub,KM_IMAGE_PARAM1,spr->texture->surfacePtr);
			kmStartStrip(&vertexBufferDesc, &StripHead_Sprites_Sub);	
		}
		else
		{
			kmChangeStripTextureSurface(&StripHead_Sprites_Add,KM_IMAGE_PARAM1,spr->texture->surfacePtr);
			kmStartStrip(&vertexBufferDesc, &StripHead_Sprites_Add);	
		}
	}

	kmSetVertex(&vertexBufferDesc, &vertices_GT4[0], KM_VERTEXTYPE_03, sizeof(KMVERTEX_03));
	kmSetVertex(&vertexBufferDesc, &vertices_GT4[1], KM_VERTEXTYPE_03, sizeof(KMVERTEX_03));
	kmSetVertex(&vertexBufferDesc, &vertices_GT4[2], KM_VERTEXTYPE_03, sizeof(KMVERTEX_03));	
	kmSetVertex(&vertexBufferDesc, &vertices_GT4[3], KM_VERTEXTYPE_03, sizeof(KMVERTEX_03));	
	kmEndStrip(&vertexBufferDesc);

	return 1;
}


/* --------------------------------------------------------------------------------
   Function	: Print3DSpriteRotating
   Purpose : print a 3d rotating sprite
   Parameters : sprite structure pointer
   Returns : 1 sprite printed, else 0
   Info	: 
*/

int Print3DSpriteRotating(SPRITE *spr)
{
	POLY_FT4 *pp;
	LONG width, height, spritez;
	VERT tempVect;
	long		x0,y0,z0;
	long		x1,y1,z1;
	long		x2,y2,z2;
	long		x3,y3,z3;
	uchar		r,g,b;
	KMUINT32	rgba;

	fixed cosine,sine,newX,newY;
	int atbdx, atbdy;

	cosine = rcos(spr->angle);
	sine = rsin(spr->angle);

	tempVect.vx = -spr->pos.vx;
	tempVect.vy = -spr->pos.vy;
	tempVect.vz = spr->pos.vz;

// ------------- scary scaling-and-transform-in-one-go code from the Action Man people...
	width = spr->texture->w;
	gte_SetLDDQB(0);		// clear offset control reg (C2_DQB)

	gte_ldv0(&tempVect);
	gte_SetLDDQA(width);	// shove sprite width into control reg (C2_DQA)
	gte_rtps();				// do the rtps
	x0 = screenxy[2].vx;
	y0 = screenxy[2].vy;	
//	gte_stsxy(&pp->x0);		// get screen x and y
	gte_stsz(&spritez);		// get screen z
// ------------- end of scaling-and-transform


	// depth check
	if (spritez <= 0 || spritez >= fog.max)
		return 0;

	// clip sprite width
	width = (spr->scaleX*SCALEX) / spritez;
	if( width < 2 || width > MAX_SPRITEWIDTH )
		return 0;			// better max-distance check
							// JIM: Not really. A good thing to do but not a substitute for an actual distance check.

	// Scaled height of sprite. This is different from width - probably due to screen res.
	// NOTE: Random multiply that looks more or less OK :)
	height = (spr->scaleY*SCALEY) / spritez;

	atbdx = x0;
	atbdy = y0;

	x0 = -width;
	x1 = width;
	x2 = -width;
	x3 = width;

	y0 = -height;
	y1 = -height;
	y2 = height;
	y3 = height;

	newX = FMul(x0,cosine) + FMul(y0,sine);
	newY = FMul(y0,cosine) - FMul(x0,sine);
	x0 = newX + atbdx;
	y0 = newY + atbdy;

	newX = FMul(x1,cosine) + FMul(y1,sine);
	newY = FMul(y1,cosine) - FMul(x1,sine);
	x1 = newX + atbdx;
	y1 = newY + atbdy;

	newX = FMul(x2,cosine) + FMul(y2,sine);
	newY = FMul(y2,cosine) - FMul(x2,sine);
	x2 = newX + atbdx;
	y2 = newY + atbdy;

	newX = FMul(x3,cosine) + FMul(y3,sine);
	newY = FMul(y3,cosine) - FMul(x3,sine);
	x3 = newX + atbdx;
	y3 = newY + atbdy;

	spritez = spritez >> 2;

	{
		int	xmax, xmin, xd;

		xmax = x0;
		xmax = max(xmax, x1);
		xmax = max(xmax, x2);
		xmax = max(xmax, x3);
		xmin = x0;
		xmin = min(xmin, x1);
		xmin = min(xmin, x2);
		xmin = min(xmin, x3);
		xd = xmax - xmin;
		if (xd > MAX_SPRITEWIDTH)
			return 1;
	}

	// completely off at the LHS or RHS?
	if ((x0 > 640.0f && x1 > 640.0f && x2 > 640.0f && x3 > 640.0f) ||
		(x0 <   0.0f && x1 <   0.0f && x2 <   0.0f && x3 <   0.0f))
	{
		return 0;
	}

	// completely off at the TOP or BOTTOM?
	if ((y0 > 480.0f && y1 > 480.0f && y2 > 480.0f && y3 > 480.0f) ||
		(y0 <   0.0f && y1 <   0.0f && y2 <   0.0f && y3 <   0.0f))
	{
		return 0;
	}

	if( spr->flags & SPRITE_TRANSLUCENT )
	{
		r = ((spr->a*(short)spr->r)/255);
		g = ((spr->a*(short)spr->g)/255);
		b = ((spr->a*(short)spr->b)/255);
	}
	else
	{
		r = spr->r;
		g = spr->g;
		b = spr->b;
	}
	rgba = RGBA(r,g,b,spr->a);
		
	vertices_GT4[0].fX = x0;
	vertices_GT4[0].fY = y0;
	vertices_GT4[0].u.fZ = 1.0 / ((float)spritez);
	vertices_GT4[0].fU = 1.0;
	vertices_GT4[0].fV = 0.0;
	vertices_GT4[0].uBaseRGB.dwPacked = rgba;

	vertices_GT4[1].fX = x1;
	vertices_GT4[1].fY = y1;
	vertices_GT4[1].u.fZ = 1.0 / ((float)spritez);
	vertices_GT4[1].fU = 0.0;
	vertices_GT4[1].fV = 0.0;
	vertices_GT4[1].uBaseRGB.dwPacked = rgba;

	vertices_GT4[2].fX = x2;
	vertices_GT4[2].fY = y2;
	vertices_GT4[2].u.fZ = 1.0 / ((float)spritez);
	vertices_GT4[2].fU = 1.0;
	vertices_GT4[2].fV = 1.0;
	vertices_GT4[2].uBaseRGB.dwPacked = rgba;

	vertices_GT4[3].fX = x3;
	vertices_GT4[3].fY = y3;
	vertices_GT4[3].u.fZ = 1.0 / ((float)spritez);
	vertices_GT4[3].fU = 0.0;
	vertices_GT4[3].fV = 1.0;
	vertices_GT4[3].uBaseRGB.dwPacked = rgba;

	if(spr->texture->colourKey)
	{
		kmChangeStripTextureSurface(&StripHead_Sprites,KM_IMAGE_PARAM1,spr->texture->surfacePtr);
		kmStartStrip(&vertexBufferDesc, &StripHead_Sprites);
	}
	else
	{
		kmChangeStripTextureSurface(&StripHead_Sprites_Add,KM_IMAGE_PARAM1,spr->texture->surfacePtr);
		kmStartStrip(&vertexBufferDesc, &StripHead_Sprites_Add);	
	}	

	kmSetVertex(&vertexBufferDesc, &vertices_GT4[0], KM_VERTEXTYPE_03, sizeof(KMVERTEX_03));
	kmSetVertex(&vertexBufferDesc, &vertices_GT4[1], KM_VERTEXTYPE_03, sizeof(KMVERTEX_03));
	kmSetVertex(&vertexBufferDesc, &vertices_GT4[2], KM_VERTEXTYPE_03, sizeof(KMVERTEX_03));	
	kmSetVertex(&vertexBufferDesc, &vertices_GT4[3], KM_VERTEXTYPE_03, sizeof(KMVERTEX_03));	
	kmEndStrip(&vertexBufferDesc);

	return 1;
}


int Print3D3DSprite ( SPECFX *fx, SVECTOR *vect, unsigned long colour ) 
{
	LONG 			spritez;
	SVECTOR 		tempVect[4];
	float			x0,y0,x1,y1,x2,y2,x3,y3;
	long			z0,z1,z2,z3;
	KMUINT32		rgba;
	TextureType 	*txPtr;
	unsigned char	r,g,b;

	tempVect[0].vx = -vect[0].vx;
	tempVect[0].vy = -vect[0].vy;
	tempVect[0].vz = vect[0].vz;

	tempVect[1].vx = -vect[1].vx;
	tempVect[1].vy = -vect[1].vy;
	tempVect[1].vz = vect[1].vz;

	tempVect[3].vx = -vect[2].vx;
	tempVect[3].vy = -vect[2].vy;
	tempVect[3].vz = vect[2].vz;

	tempVect[2].vx = -vect[3].vx;
	tempVect[2].vy = -vect[3].vy;
	tempVect[2].vz = vect[3].vz;

	gte_SetLDDQB(0);			// clear offset control reg (C2_DQB)

	gte_SetRotMatrix(&GsWSMATRIX);
	gte_SetTransMatrix(&GsWSMATRIX);
		
 	gte_ldv0(&tempVect[0]);		//vert);
 	gte_rtps();					// do the rtps
	x0 = screenxy[2].vx;
	y0 = screenxy[2].vy;
	gte_stsz(&z0);
 
 	gte_ldv0(&tempVect[1]);		//vert);
 	gte_rtps();					// do the rtps
	x1 = screenxy[2].vx;
	y1 = screenxy[2].vy;
 	gte_stsz(&z1);

 	gte_ldv0(&tempVect[2]);		//vert);
 	gte_rtps();					// do the rtps
	x2 = screenxy[2].vx;
	y2 = screenxy[2].vy;
	gte_stsz(&z2);

	gte_ldv0(&tempVect[3]);		//vert);  								
	gte_rtps();					// do the rtps							
	x3 = screenxy[2].vx;
	y3 = screenxy[2].vy;
	gte_stsz(&z3);

	gte_stsz(&spritez);		// get order table z

	spritez += fx->zDepthOff;		//mm allow for z depth to be altered

	spritez = spritez >> 2;

	z0 = (z0 + fx->zDepthOff) >> 2;
	z1 = (z1 + fx->zDepthOff) >> 2;
	z2 = (z2 + fx->zDepthOff) >> 2;
	z3 = (z3 + fx->zDepthOff) >> 2;

	// check to see the poly is on screen

	if((x0 >= 640)&&(x1 >= 640)&&(x2 >= 640)&&(x3 >= 640))
		return 0;
	if((x0 < 0)&&(x1 < 0)&&(x2 < 0)&&(x3 < 0))
		return 0;			
	if((y0 >= 480)&&(y1 >= 480)&&(y2 >= 480)&&(y3 >= 480))
		return 0;
	if((y0 < 0)&&(y1 < 0)&&(y2 < 0)&&(y3 < 0))
		return 0;
	
	if((spritez < 20) || (spritez >= fog.max)) 
		return 0;

	r = fx->r;
	g = fx->g;
	b = fx->b;
	
	if(fx->flags & SPRITE_TRANSLUCENT )//Only do additive if the 'sprite' fades or is translucent
	{
		// SL: modge the RGBs accordingly
	  	r = ((fx->a*(short)fx->r)/255);
	  	g = ((fx->a*(short)fx->g)/255);
	  	b = ((fx->a*(short)fx->b)/255);
	}

	if(fx->flags & SPRITE_SUBTRACTIVE )//Subtractive for shadows
	{
		// SL: modge the RGBs accordingly
	  	r = ((fx->a*(short)fx->r)/255);
	  	g = ((fx->a*(short)fx->g)/255);
	  	b = ((fx->a*(short)fx->b)/255);
	}
	rgba = RGBA(r,g,b,fx->a);

	vertices_GT4[0].fX = x0;
	vertices_GT4[0].fY = y0;
	vertices_GT4[0].u.fZ = 1.0 / ((float)z0);
	vertices_GT4[0].fU = 1.0;
	vertices_GT4[0].fV = 0.0;
	vertices_GT4[0].uBaseRGB.dwPacked = rgba;

	vertices_GT4[1].fX = x1;
	vertices_GT4[1].fY = y1;
	vertices_GT4[1].u.fZ = 1.0 / ((float)z1);
	vertices_GT4[1].fU = 0.0;
	vertices_GT4[1].fV = 0.0;
	vertices_GT4[1].uBaseRGB.dwPacked = rgba;

	vertices_GT4[2].fX = x2;
	vertices_GT4[2].fY = y2;
	vertices_GT4[2].u.fZ = 1.0 / ((float)z2);
	vertices_GT4[2].fU = 1.0;
	vertices_GT4[2].fV = 1.0;
	vertices_GT4[2].uBaseRGB.dwPacked = rgba;

	vertices_GT4[3].fX = x3;
	vertices_GT4[3].fY = y3;
	vertices_GT4[3].u.fZ = 1.0 / ((float)z3);
	vertices_GT4[3].fU = 0.0;
	vertices_GT4[3].fV = 1.0;
	vertices_GT4[3].uBaseRGB.dwPacked = rgba;

	if(fx->tex->colourKey)
	{
		kmChangeStripTextureSurface(&StripHead_Sprites,KM_IMAGE_PARAM1,fx->tex->surfacePtr);
		kmStartStrip(&vertexBufferDesc, &StripHead_Sprites);	
	}
	else
	{
		if(fx->flags & SPRITE_SUBTRACTIVE)
		{
			kmChangeStripTextureSurface(&StripHead_Sprites_Sub,KM_IMAGE_PARAM1,fx->tex->surfacePtr);
			kmStartStrip(&vertexBufferDesc, &StripHead_Sprites_Sub);	
		}
		else
		{
			kmChangeStripTextureSurface(&StripHead_Sprites_Add,KM_IMAGE_PARAM1,fx->tex->surfacePtr);
			kmStartStrip(&vertexBufferDesc, &StripHead_Sprites_Add);	
		}
	}	
	
	kmSetVertex(&vertexBufferDesc, &vertices_GT4[0], KM_VERTEXTYPE_03, sizeof(KMVERTEX_03));
	kmSetVertex(&vertexBufferDesc, &vertices_GT4[1], KM_VERTEXTYPE_03, sizeof(KMVERTEX_03));
	kmSetVertex(&vertexBufferDesc, &vertices_GT4[2], KM_VERTEXTYPE_03, sizeof(KMVERTEX_03));	
	kmSetVertex(&vertexBufferDesc, &vertices_GT4[3], KM_VERTEXTYPE_03, sizeof(KMVERTEX_03));	
	kmEndStrip(&vertexBufferDesc);

	return 1;
}

#endif
