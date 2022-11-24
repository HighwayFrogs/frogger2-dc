#include <isltex.h>

#include <islpad.h>
#include <islutil.h>
#include "Shell.h"
#include "overlays.h"

#include "types2d.h"
#include "psxsprite.h"
#include "sprite.h"
#include "layout.h"
#include "maths.h"
#include "main.h"
#include "menus.h"
#include "options.h"
#include "hud.h"
#include "menus.h"
#include "gte.h"

#define OVERLAY_X 640.0/4096.0
#define OVERLAY_Y 480.0/4096.0

extern OPTIONSOBJECTS options;
extern SPRITEOVERLAY *frogLogo;

void DrawSprite ( SPRITEOVERLAY *spr )
{
	int					atbdx,atbdy;
	int					w,h;
	TextureType			*tPtr;
	float				x0,y0,x1,y1,x2,y2,x3,y3,xs,ys;
	u_char				r,b,g,alpha;

	atbdx = (float)(spr->xPos>>3)*1.25;//-256;
	atbdy = (spr->yPos/17)*2;//-120;
	tPtr = spr->tex;
	
	if((spr->a == 255) && (!(spr->flags & (SPRITE_ADDITIVE | SPRITE_SUBTRACTIVE))))
	{
		alpha = 0;
//		r = (spr->r * 128) >> 8;
//		g = (spr->g * 128) >> 8;
//		b = (spr->b * 128) >> 8;
		r = spr->r;
		g = spr->g;
		b = spr->b;
	}
	else
	{
		alpha = 2;
//		r = spr->r;
//		g = spr->g;
//		b = spr->b;
		r = (spr->r * spr->a) >> 9;
		g = (spr->g * spr->a) >> 9;
		b = (spr->b * spr->a) >> 9;
	}
		
	if(spr->tex == NULL)
	{
		alpha = spr->a;
		x0 = atbdx;
		y0 = atbdy;
		x1 = atbdx + (spr->width>>3)*1.25;
		y1 = atbdy;
		x2 = atbdx;
		y2 = atbdy + (spr->height/17)*2;
		x3 = atbdx + (spr->width>>3)*1.25;
		y3 = atbdy + (spr->height/17)*2;
		
		vertices_SpritesNoTex[0].fX = x0;
		vertices_SpritesNoTex[0].fY = y0;
		vertices_SpritesNoTex[0].u.fZ = spr->num + 1.0;
		vertices_SpritesNoTex[0].uBaseRGB.dwPacked = RGBA(r,g,b,alpha);

		vertices_SpritesNoTex[1].fX = x1;
		vertices_SpritesNoTex[1].fY = y1;
		vertices_SpritesNoTex[1].u.fZ = spr->num + 1.0;
		vertices_SpritesNoTex[1].uBaseRGB.dwPacked = RGBA(r,g,b,alpha);

		vertices_SpritesNoTex[2].fX = x2;
		vertices_SpritesNoTex[2].fY = y2;
		vertices_SpritesNoTex[2].u.fZ = spr->num + 1.0;
		vertices_SpritesNoTex[2].uBaseRGB.dwPacked = RGBA(r,g,b,alpha);

		vertices_SpritesNoTex[3].fX = x3;
		vertices_SpritesNoTex[3].fY = y3;
		vertices_SpritesNoTex[3].u.fZ = spr->num + 1.0;
		vertices_SpritesNoTex[3].uBaseRGB.dwPacked = RGBA(r,g,b,alpha);

		if(spr->flags & SPRITE_SUBTRACTIVE)
			kmStartStrip(&vertexBufferDesc, &StripHead_SpritesNoTex_Sub);	
		else
			kmStartStrip(&vertexBufferDesc, &StripHead_SpritesNoTex);	

		kmSetVertex(&vertexBufferDesc, &vertices_SpritesNoTex[0], KM_VERTEXTYPE_00, sizeof(KMVERTEX_00));
		kmSetVertex(&vertexBufferDesc, &vertices_SpritesNoTex[1], KM_VERTEXTYPE_00, sizeof(KMVERTEX_00));
		kmSetVertex(&vertexBufferDesc, &vertices_SpritesNoTex[2], KM_VERTEXTYPE_00, sizeof(KMVERTEX_00));	
		kmSetVertex(&vertexBufferDesc, &vertices_SpritesNoTex[3], KM_VERTEXTYPE_00, sizeof(KMVERTEX_00));	
		kmEndStrip(&vertexBufferDesc);
		
	}
	else
	{		
		w = tPtr->w-1;
		h = tPtr->h-1;

		if(spr->width==4096)
		{	// NEW! 1:1 texels to pixels
			x0 = atbdx;
			y0 = atbdy;
			x1 = atbdx+w;
			y1 = atbdy;
			x2 = atbdx;
			y2 = atbdy+h;
			x3 = atbdx+w;
			y3 = atbdy+h;
		}
		else
		{	// Original random scaling method (slightly tidier)
			x0 = atbdx;
			y0 = atbdy;
			x1 = atbdx + ((float)(spr->width>>3)) * 1.25;
			y1 = atbdy;
			x2 = atbdx;
			y2 = atbdy + (spr->height/17) * 2;
			x3 = atbdx + ((float)(spr->width>>3)) * 1.25;
			y3 = atbdy + (spr->height/17) * 2;
		}

		alpha = spr->a;
//		alpha = 255;

//		r = spr->r;
//		g = spr->g;
//		b = spr->b;
		
		vertices_GT4[0].fX = x0;
		vertices_GT4[0].fY = y0;
		vertices_GT4[0].u.fZ = spr->num + 1.0;
		vertices_GT4[0].fU = (float)0;
		vertices_GT4[0].fV = (float)0;
		vertices_GT4[0].uBaseRGB.dwPacked = RGBA(r,g,b,alpha);

		vertices_GT4[1].fX = x1;
		vertices_GT4[1].fY = y1;
		vertices_GT4[1].u.fZ = spr->num + 1.0;
		vertices_GT4[1].fU = (float)1;
		vertices_GT4[1].fV = (float)0;
		vertices_GT4[1].uBaseRGB.dwPacked = RGBA(r,g,b,alpha);

		vertices_GT4[2].fX = x2;
		vertices_GT4[2].fY = y2;
		vertices_GT4[2].u.fZ = spr->num + 1.0;
		vertices_GT4[2].fU = (float)0;
		vertices_GT4[2].fV = (float)1;
		vertices_GT4[2].uBaseRGB.dwPacked = RGBA(r,g,b,alpha);

		vertices_GT4[3].fX = x3;
		vertices_GT4[3].fY = y3;
		vertices_GT4[3].u.fZ = spr->num + 1.0;
		vertices_GT4[3].fU = (float)1;
		vertices_GT4[3].fV = (float)1;
		vertices_GT4[3].uBaseRGB.dwPacked = RGBA(r,g,b,alpha);

		if(spr->tex->colourKey)
		{
			kmChangeStripTextureSurface(&StripHead_Sprites,KM_IMAGE_PARAM1,spr->tex->surfacePtr);
			kmStartStrip(&vertexBufferDesc, &StripHead_Sprites);	
		}
		else
		{
			if(spr->flags & SPRITE_ADDITIVE)
			{
				kmChangeStripTextureSurface(&StripHead_Sprites_Add,KM_IMAGE_PARAM1,spr->tex->surfacePtr);
				kmStartStrip(&vertexBufferDesc, &StripHead_Sprites_Add);	
			}
			else if(spr->flags & SPRITE_SUBTRACTIVE)
			{
				kmChangeStripTextureSurface(&StripHead_Sprites_Sub,KM_IMAGE_PARAM1,spr->tex->surfacePtr);
				kmStartStrip(&vertexBufferDesc, &StripHead_Sprites_Sub);	
			}
			else
			{
				kmChangeStripTextureSurface(&StripHead_Sprites,KM_IMAGE_PARAM1,spr->tex->surfacePtr);
				kmStartStrip(&vertexBufferDesc, &StripHead_Sprites);	
			}
		}
		
		kmSetVertex(&vertexBufferDesc, &vertices_GT4[0], KM_VERTEXTYPE_03, sizeof(KMVERTEX_03));
		kmSetVertex(&vertexBufferDesc, &vertices_GT4[1], KM_VERTEXTYPE_03, sizeof(KMVERTEX_03));
		kmSetVertex(&vertexBufferDesc, &vertices_GT4[2], KM_VERTEXTYPE_03, sizeof(KMVERTEX_03));	
		kmSetVertex(&vertexBufferDesc, &vertices_GT4[3], KM_VERTEXTYPE_03, sizeof(KMVERTEX_03));	
		kmEndStrip(&vertexBufferDesc);
	}
}

void DrawSpriteOverlayRotating ( SPRITEOVERLAY *spr )
{
	int			atbdx,atbdy, w,h;
	POLY_F4		*f4;
	POLY_FT4	*ft4;
	DR_MODE		*dm;
	TextureType	*tPtr;
	uchar 		alpha,r,g,b;
	int 		depth;
	fixed		cosine,sine,newX,newY;
	fixed 		halfWidth,halfHeight;
	float		x0,x1,x2,x3;
	float		y0,y1,y2,y3;
	uchar		r0,g0,b0;

	cosine = rcos(-spr->angle);
	sine = rsin(-spr->angle);
	halfWidth = spr->width/2;
	halfHeight = spr->height/2;

	depth = 4 - spr->num;

	r = spr->r;
	g = spr->g;
	b = spr->b;
	alpha = 255;

	if(spr->tex == 0)
	{
		x0 = -halfWidth;
		x1 = halfWidth;
		x2 = -halfWidth;
		x3 = halfWidth;

		y0 = -halfHeight;
		y1 = -halfHeight;
		y2 = halfHeight;
		y3 = halfHeight;

		newX = FMul(x0,cosine) + FMul(y0,sine);
		newY = FMul(y0,cosine) - FMul(x0,sine);

		x0 = (newX + atbdx + halfWidth)/8;
		y0 = (newY + atbdy + halfHeight)/(17);

		newX = FMul(x1,cosine) + FMul(y1,sine);
		newY = FMul(y1,cosine) - FMul(x1,sine);

		x1 = (newX + atbdx + halfWidth)/8;
		y1 = (newY + atbdy + halfHeight)/(17);

		newX = FMul(x2,cosine) + FMul(y2,sine);
		newY = FMul(y2,cosine) - FMul(x2,sine);

		x2 = (newX + atbdx + halfWidth)/8; 
		y2 = (newY + atbdy + halfHeight)/(17);

		newX = FMul(x3,cosine) + FMul(y3,sine);
		newY = FMul(y3,cosine) - FMul(x3,sine);

		x3 = (newX + atbdx + halfWidth)/8; 
		y3 = (newY + atbdy + halfHeight)/(17);

		r0 = r*2;
		g0 = g*2;
		b0 = b*2;
		
//ma		if(alpha)
//ma			setSemiTrans(f4,1);

//ma		ENDPRIM(f4, depth, POLY_F4);

/*ma	if(alpha)
		{
			BEGINPRIM(dm, DR_MODE);
			if(spr->flags & SPRITE_SUBTRACTIVE)
				SetDrawMode(dm, 0,1, ((SEMITRANS_SUB-1)<<5),0);
			else
				SetDrawMode(dm, 0,1, ((SEMITRANS_ADD-1)<<5),0);
			ENDPRIM(dm, depth, DR_MODE);
		}
*/		

		vertices_SpritesNoTex[0].fX = x0;
		vertices_SpritesNoTex[0].fY = y0;
		vertices_SpritesNoTex[0].u.fZ = spr->num + 1.0;
		vertices_SpritesNoTex[0].uBaseRGB.dwPacked = RGBA(r,g,b,alpha);

		vertices_SpritesNoTex[1].fX = x1;
		vertices_SpritesNoTex[1].fY = y1;
		vertices_SpritesNoTex[1].u.fZ = spr->num + 1.0;
		vertices_SpritesNoTex[1].uBaseRGB.dwPacked = RGBA(r,g,b,alpha);

		vertices_SpritesNoTex[2].fX = x2;
		vertices_SpritesNoTex[2].fY = y2;
		vertices_SpritesNoTex[2].u.fZ = spr->num + 1.0;
		vertices_SpritesNoTex[2].uBaseRGB.dwPacked = RGBA(r,g,b,alpha);

		vertices_SpritesNoTex[3].fX = x3;
		vertices_SpritesNoTex[3].fY = y3;
		vertices_SpritesNoTex[3].u.fZ = spr->num + 1.0;
		vertices_SpritesNoTex[3].uBaseRGB.dwPacked = RGBA(r,g,b,alpha);

		kmStartStrip(&vertexBufferDesc, &StripHead_SpritesNoTex);	
		kmSetVertex(&vertexBufferDesc, &vertices_SpritesNoTex[0], KM_VERTEXTYPE_00, sizeof(KMVERTEX_00));
		kmSetVertex(&vertexBufferDesc, &vertices_SpritesNoTex[1], KM_VERTEXTYPE_00, sizeof(KMVERTEX_00));
		kmSetVertex(&vertexBufferDesc, &vertices_SpritesNoTex[2], KM_VERTEXTYPE_00, sizeof(KMVERTEX_00));	
		kmSetVertex(&vertexBufferDesc, &vertices_SpritesNoTex[3], KM_VERTEXTYPE_00, sizeof(KMVERTEX_00));	
		kmEndStrip(&vertexBufferDesc);
	}
	else
	{
//		atbdx = (float)(spr->xPos>>3)*1.25;//-256;
//		atbdy = (spr->yPos/17)*2;//-120;

//		atbdx = spr->xPos-2048;
//		atbdy = spr->yPos-(120*17);
		atbdx = spr->xPos;//-96;
		atbdy = spr->yPos;//-96;
		tPtr = spr->tex;

		x0 = -halfWidth;
		x1 = halfWidth;
		x2 = -halfWidth;
		x3 = +halfWidth;
		
		y0 = -halfHeight;
		y1 = -halfHeight;
		y2 = halfHeight;
		y3 = halfHeight;

		newX = FMul(x0,cosine) + FMul(y0,sine);
		newY = FMul(y0,cosine) - FMul(x0,sine);

		x0 = (newX + atbdx + halfWidth);// /8; 
		y0 = (newY + atbdy + halfHeight);// /(17);

		newX = FMul(x1,cosine) + FMul(y1,sine);
		newY = FMul(y1,cosine) - FMul(x1,sine);

		x1 = (newX + atbdx + halfWidth);// /8; 
		y1 = (newY + atbdy + halfHeight);//(17);

		newX = FMul(x2,cosine) + FMul(y2,sine);
		newY = FMul(y2,cosine) - FMul(x2,sine);

		x2 = (newX + atbdx + halfWidth);// /8; 
		y2 = (newY + atbdy + halfHeight);// /(17);

		newX = FMul(x3,cosine) + FMul(y3,sine);
		newY = FMul(y3,cosine) - FMul(x3,sine);

		x3 = (newX + atbdx + halfWidth);// /8; 
		y3 = (newY + atbdy + halfHeight);// /(17);
	
		r0 = r;//( spr->r * 128 ) >> 8;
		g0 = g;//( spr->g * 128 ) >> 8;
		b0 = b;//( spr->b * 128 ) >> 8;

		x0 *= OVERLAY_X;
		x1 *= OVERLAY_X;
		x2 *= OVERLAY_X;
		x3 *= OVERLAY_X;

		y0 *= OVERLAY_Y;
		y1 *= OVERLAY_Y;
		y2 *= OVERLAY_Y;
		y3 *= OVERLAY_Y;
		
		vertices_GT4[0].fX = x0;
		vertices_GT4[0].fY = y0;
		vertices_GT4[0].u.fZ = spr->num + 1.0;
		vertices_GT4[0].fU = (float)0;
		vertices_GT4[0].fV = (float)0;
		vertices_GT4[0].uBaseRGB.dwPacked = RGBA(r,g,b,alpha);

		vertices_GT4[1].fX = x1;
		vertices_GT4[1].fY = y1;
		vertices_GT4[1].u.fZ = spr->num + 1.0;
		vertices_GT4[1].fU = (float)1;
		vertices_GT4[1].fV = (float)0;
		vertices_GT4[1].uBaseRGB.dwPacked = RGBA(r,g,b,alpha);

		vertices_GT4[2].fX = x2;
		vertices_GT4[2].fY = y2;
		vertices_GT4[2].u.fZ = spr->num + 1.0;
		vertices_GT4[2].fU = (float)0;
		vertices_GT4[2].fV = (float)1;
		vertices_GT4[2].uBaseRGB.dwPacked = RGBA(r,g,b,alpha);

		vertices_GT4[3].fX = x3;
		vertices_GT4[3].fY = y3;
		vertices_GT4[3].u.fZ = spr->num + 1.0;
		vertices_GT4[3].fU = (float)1;
		vertices_GT4[3].fV = (float)1;
		vertices_GT4[3].uBaseRGB.dwPacked = RGBA(r,g,b,alpha);

		if(spr->tex->colourKey)
		{
			kmChangeStripTextureSurface(&StripHead_Sprites,KM_IMAGE_PARAM1,spr->tex->surfacePtr);
			kmStartStrip(&vertexBufferDesc, &StripHead_Sprites);	
		}
		else
		{
			kmChangeStripTextureSurface(&StripHead_Sprites_Add,KM_IMAGE_PARAM1,spr->tex->surfacePtr);
			kmStartStrip(&vertexBufferDesc, &StripHead_Sprites_Add);	
		}
	
		kmSetVertex(&vertexBufferDesc, &vertices_GT4[0], KM_VERTEXTYPE_03, sizeof(KMVERTEX_03));
		kmSetVertex(&vertexBufferDesc, &vertices_GT4[1], KM_VERTEXTYPE_03, sizeof(KMVERTEX_03));
		kmSetVertex(&vertexBufferDesc, &vertices_GT4[2], KM_VERTEXTYPE_03, sizeof(KMVERTEX_03));	
		kmSetVertex(&vertexBufferDesc, &vertices_GT4[3], KM_VERTEXTYPE_03, sizeof(KMVERTEX_03));	
		kmEndStrip(&vertexBufferDesc);
	}
}

void PrintSpriteOverlays ( char num )
{
	int counter,j;
	SPRITEOVERLAY *cur;

	for ( counter = 0; counter < spriteOverlayList.numEntries; counter++ )
	{
		cur = &spriteOverlayList.block [ counter ];

		if(frogLogo == cur)
			j=8;

		if(cur->draw)
		{
			// Go to destination, if specified
			fixed spd = FMul(gameSpeed, cur->speed)>>12;

			if( Fabs(cur->xPosTo-cur->xPos) )
			{
				cur->xPos += (cur->xPosTo > cur->xPos)?spd:-spd;
				
				if( Fabs(cur->xPosTo-cur->xPos) < Fabs(spd) )
				{
					cur->speed = 0;
					cur->xPos = cur->xPosTo;
				}
			}
			else if( Fabs(cur->yPosTo-cur->yPos) )
			{
				cur->yPos += (cur->yPosTo > cur->yPos)?spd:-spd;

				if( Fabs(cur->yPosTo-cur->yPos) < Fabs(spd) )
				{
					cur->speed = 0;
					cur->yPos = cur->yPosTo;
				}
			}
		
/*ma		if ( !cur->angle )
			{
				DrawSprite ( cur );
			}
			else
			{
				DrawSpriteOverlayRotating ( cur );
			}
*/			
			if (( cur->angle )||(cur == arcadeHud.timeHandOver)||(cur == arcadeHud.timeHeadOver))
			{
				DrawSpriteOverlayRotating ( cur );
			}
			else
			{
				DrawSprite ( cur );
			}

			// ENDELSEIF
		}
		// ENDIF


	}
	// ENDFOR
}

TextureType *tileTexture[4] = {NULL,NULL,NULL,NULL};
void InitTiledBackdrop(char *filename)
{
	char tempStr[32];
	int i;

	for(i = 0;i < 4;i++)
	{
		sprintf(tempStr,"%s0%d",filename,i+1);
		tileTexture[i] = FindTexture(tempStr);
	}
}

int xTile = 5;
int yTile = 4;
void DrawTiledBackdrop(int scroll)
{
	int x,y;
	int			atbdx,atbdy, w,h;
	POLY_FT4	*ft4;
	int xRes = 640,yRes = 480;
	static int xScroll = 0;
	static int yScroll = 0;
	TextureType *tex;
	
	w = tileTexture[0]->w-1;
	h = tileTexture[0]->h-1;

	for(x = -4;x < xTile + 4;x++)
	{
		for(y = -2;y < yTile + 2;y++)
		{
			tex = tileTexture[(((x + 4) MOD 2) + ((y + 2) MOD 2) * 2 + ((x + 4)/2) * 2) MOD 4];

			atbdx = (x*xRes)/xTile - xRes/2 + xScroll;
			atbdy = (y*yRes)/yTile - yRes/2 + yScroll;
			atbdx += 320;
			atbdy += 240;

			vertices_GT4[0].fX = atbdx;
			vertices_GT4[0].fY = atbdy;
			vertices_GT4[0].u.fZ = 0.00001;
			vertices_GT4[0].fU = (float)0;
			vertices_GT4[0].fV = (float)0;
			vertices_GT4[0].uBaseRGB.dwPacked = RGBA(255,255,255,255);

			vertices_GT4[1].fX = atbdx + xRes/xTile + 1;
			vertices_GT4[1].fY = atbdy;
			vertices_GT4[1].u.fZ = 0.00001;
			vertices_GT4[1].fU = (float)1;
			vertices_GT4[1].fV = (float)0;
			vertices_GT4[1].uBaseRGB.dwPacked = RGBA(255,255,255,255);

			vertices_GT4[2].fX = atbdx;
			vertices_GT4[2].fY = atbdy + yRes/yTile;
			vertices_GT4[2].u.fZ = 0.00001;
			vertices_GT4[2].fU = (float)0;
			vertices_GT4[2].fV = (float)1;
			vertices_GT4[2].uBaseRGB.dwPacked = RGBA(255,255,255,255);

			vertices_GT4[3].fX = atbdx + xRes/xTile + 1;
			vertices_GT4[3].fY = atbdy + yRes/yTile;
			vertices_GT4[3].u.fZ = 0.00001;
			vertices_GT4[3].fU = (float)1;
			vertices_GT4[3].fV = (float)1;
			vertices_GT4[3].uBaseRGB.dwPacked = RGBA(255,255,255,255);

			kmChangeStripTextureSurface(&StripHead_Sprites,KM_IMAGE_PARAM1,tex->surfacePtr);

			kmStartStrip(&vertexBufferDesc, &StripHead_Sprites);	
			kmSetVertex(&vertexBufferDesc, &vertices_GT4[0], KM_VERTEXTYPE_03, sizeof(KMVERTEX_03));
			kmSetVertex(&vertexBufferDesc, &vertices_GT4[1], KM_VERTEXTYPE_03, sizeof(KMVERTEX_03));
			kmSetVertex(&vertexBufferDesc, &vertices_GT4[2], KM_VERTEXTYPE_03, sizeof(KMVERTEX_03));	
			kmSetVertex(&vertexBufferDesc, &vertices_GT4[3], KM_VERTEXTYPE_03, sizeof(KMVERTEX_03));	
			kmEndStrip(&vertexBufferDesc);	
		}
	}
	if(scroll)
	{
		xScroll++;//= FMul(gameSpeed,xScrollSpeed);
		if(xScroll > ((xRes*4)/xTile))
			xScroll -= ((xRes*4)/xTile);
		yScroll++;//= FMul(gameSpeed,yScrollSpeed);
		if(yScroll > ((yRes*2)/yTile))
			yScroll -= ((yRes*2)/yTile);
	}
}

void FreeTiledBackdrop()
{
	tileTexture[0] = NULL;
}
