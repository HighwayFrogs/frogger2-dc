/******************************************************************************************
	AM2 PS   (c) 1999-2001 ISL

	psfont.c:			Font support
******************************************************************************************/

#include "include.h"
#include "main.h"

KMSTRIPHEAD		*fontStripHeadPtr;

KMSTRIPCONTEXT	fontStripContext;
KMSTRIPHEAD		fontStripHead;
KMSTRIPCONTEXT	smallFontStripContext;
KMSTRIPHEAD		smallFontStripHead;
KMVERTEX_03		fontVertices[] =
{
{ KM_VERTEXPARAM_NORMAL,      0,  32, 1.0f, 0.0f,   0.125f, RGBA(255,255,255,255), 0 },
{ KM_VERTEXPARAM_NORMAL,      0,   0, 1.0f, 0.0f,   0.0f,   RGBA(255,255,255,255), 0 },
{ KM_VERTEXPARAM_NORMAL,     32,  32, 1.0f, 0.125f, 0.125f, RGBA(255,255,255,255), 0 },
{ KM_VERTEXPARAM_ENDOFSTRIP, 32,   0, 1.0f, 0.125f, 0.0f,   RGBA(255,255,255,255), 0 }
};

KMSTRIPCONTEXT	buttonStripContext;
KMSTRIPHEAD		buttonStripHead;
KMVERTEX_03		buttonVertices[] =
{
{ KM_VERTEXPARAM_NORMAL,      0,  32, 1.0f, 0.0f,   0.125f, RGBA(255,255,255,255), 0 },
{ KM_VERTEXPARAM_NORMAL,      0,   0, 1.0f, 0.0f,   0.0f,   RGBA(255,255,255,255), 0 },
{ KM_VERTEXPARAM_NORMAL,     32,  32, 1.0f, 0.125f, 0.125f, RGBA(255,255,255,255), 0 },
{ KM_VERTEXPARAM_ENDOFSTRIP, 32,   0, 1.0f, 0.125f, 0.0f,   RGBA(255,255,255,255), 0 }
};

#define ANG2FIX(a)		(((a)*4096)/360)

#define BITMAP_XD		256.0
#define BITMAP_YD		1024.0
#define U_OFFSET		32.0/BITMAP_XD
#define V_OFFSET		32.0/BITMAP_YD
#define U_OFFSET_SMALL	21.0/BITMAP_XD
#define V_OFFSET_SMALL	21.0/BITMAP_YD


TextureType	*buttonSprites[6];

unsigned char *alphabet = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789!@#$%^&*()-=+[]{}:;\"'|\\,<.>/?"
					"\xe0\xe8\xec\xf2\xf9\xc0\xc8\xcc\xd2\xd9\xe1\xe9\xed\xf3\xfa\xfd\xc1\xc9\xcd\xd3\xda\xdd\xe2\xea\xee\xf4\xfb\xc2\xca\xce\xd4\xdb\xe3\xf1\xf5\xc3\xd1\xd5\xe4\xeb\xef\xf6\xfc\xff\xc4\xcb\xcf\xd6\xdc\xe5\xc5\xe6\xc6\xe7\xc7\xf0\xd0\xf8\xd8\xbf\xa1\xdf";

//unsigned char *alphabet = "©¡ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789!@#$%^&*()-=+[]{}:;\"'|\\,<.>/?"
//					"\xe0\xe8\xec\xf2\xf9\xc0\xc8\xcc\xd2\xd9\xe1\xe9\xed\xf3\xfa\xfd\xc1\xc9\xcd\xd3\xda\xdd\xe2\xea\xee\xf4\xfb\xc2\xca\xce\xd4\xdb\xe3\xf1\xf5\xc3\xd1\xd5\xe4\xeb\xef\xf6\xfc\xff\xc4\xcb\xcf\xd6\xdc\xe5\xc5\xe6\xc6\xe7\xc7\xf0\xd0\xf8\xd8\xbf\xa1\xdf";

char	fontwidths[255] = {26,21,19,21,17,17,20,22,
						   10,14,23,17,24,20,21,20,
						   22,22,19,18,20,22,32,25,
						   23,21,						   
						         18,19,16,19,17,16,
						   18,19,10,12,19,10,29,19,
						   19,20,20,18,16,15,19,19,
						   26,22,19,18,
						               21,10,17,16,
						   20,19,21,18,20,21,
						                     10,22,
						   17,20,27,15,23,18,13,13,
						   10,16,16,13,13,11,11,10,
						   10,13,10,14, 9,10,16,10,
						   16, 9,14,17,17,10,18,19,

						   26,17,11,21,20,17,18,10,
						   18,19,20,26,17,20,21,20,
						   24,17,18,11,18,19,26,17,
						   11,21,20,17,19,18,26,20,
						   21,17,17,12,18,19,20,26,
						   17,12,21,20,17,26,25,31,
						   16,19,14,14,18,22,15,11,
						   20,23,
};

char	smallfontwidths[255] = {
						   17,13,12,14,11,11,13,15,
						    7, 8,15,11,16,13,14,13,
						   14,15,12,12,13,14,21,16,
						   15,13,						   
						         11,12,10,12,11,10,
						   12,11, 6, 8,12, 6,18,12,
						   12,12,12,11,10, 9,12,12,
						   16,14,12,11,
						               13, 7,11,10,
						   13,12,14,12,13,14,
						                      6,14,
						   10,14,18, 9,15,12, 9, 9,
						    7,10,10, 8, 8, 7, 7, 6,
						    6, 8, 6, 9, 6, 6,10, 6,
						   10, 6,10,11,11, 7,12,12,

						   17,12, 7,14,13,11,11, 6,
						   12,12,13,17,11, 7,14,13,
						   16,11,11, 7,12,12,17,11,
						    7,14,13,11,12,12,17,13,
						   14,11,12, 8,13,12,13,17,
						   11, 8,14,13,11,17,16,20,
						   10,13, 9, 9,12,14,10, 6,
						   13,19,
};
						   
#define OVERLAY_X 640.0/4096.0
#define OVERLAY_Y 480.0/4096.0

static void fontDownload(psFont *font, char *fontdata, int character)
{
}

/**************************************************************************
	FUNCTION:	fontLoad()
	PURPOSE:	Load font into VRAM ready for use
	PARAMETERS:	Filename
	RETURNS:	Ptr to font info
**************************************************************************/

psFont *fontLoad(char *fontname)
{
	int				loop;
	unsigned char	*fontdata;
	psFont			*fontptr;
	char			*c,name[64];
	char			*widths;
	
	fontptr = (psFont*)syMalloc(sizeof(psFont));	

	if(strcmp(fontname,"smallfont.pvr") == 0)
	{
		fontptr->scale = 1;
		fontptr->size = 21;
		fontptr->uoffset = 21.0/BITMAP_XD;
		fontptr->voffset = 21.0/BITMAP_YD;	
		widths = smallfontwidths;
		fontptr->height = 16;
	}
	else
	{
		fontptr->scale = 1.0;
		fontptr->size = 32;
		fontptr->uoffset = 32.0/BITMAP_XD;
		fontptr->voffset = 32.0/BITMAP_YD;	
		widths = fontwidths;
		fontptr->height = 32;
	}

	// load font and setup font texture
	if(loadPVRFileIntoSurface(fontname, "fonts", &fontptr->surface, TRUE) == 0)
		return NULL;
		
	fontptr->numchars = 154;

	memset(fontptr->charlookup, -1, 256);
	for(loop = 0; loop < fontptr->numchars; loop++)
	{
		fontptr->dataoffset[loop] = 0;
		fontptr->width[loop] = (float)widths[loop] * fontptr->scale;
		fontptr->pixelwidth[loop] = (float)widths[loop] * fontptr->scale;
		fontptr->charlookup[alphabet[loop]] = loop;
	
		fontptr->uvOffsets[loop].v[0] = (float)((float)((loop % 8) * 32.0)-1.0) / BITMAP_XD;
		fontptr->uvOffsets[loop].v[1] = (float)((float)(loop/8) * 32.0) / BITMAP_YD;
	}
	fontptr->alpha = 255;

	if(strcmp(fontname,"smallfont.pvr") == 0)
	{
		// initialise font strip context and head
		memset(&smallFontStripContext,0,sizeof(smallFontStripContext));
		memset(&smallFontStripHead,0,sizeof(smallFontStripHead));
		smallFontStripContext.nSize = sizeof(KMSTRIPCONTEXT);
   		kmInitStripContext(KM_STRIPCONTEXT_SYS_GOURAUD | KM_TRANS_POLYGON, &smallFontStripContext);
//		smallFontStripContext.StripControl.nListType		 						= KM_TRANS_POLYGON;
		smallFontStripContext.StripControl.nUserClipMode	 						= KM_USERCLIP_DISABLE;
		smallFontStripContext.StripControl.nShadowMode			 				= KM_NORMAL_POLYGON;
		smallFontStripContext.StripControl.bGouraud		 						= KM_TRUE;
		smallFontStripContext.ObjectControl.nDepthCompare			 			= KM_ALWAYS;
		smallFontStripContext.ObjectControl.nCullingMode			 				= KM_NOCULLING;
		smallFontStripContext.ObjectControl.bZWriteDisable						= KM_FALSE;
		smallFontStripContext.ImageControl[KM_IMAGE_PARAM1].nSRCBlendingMode		= KM_SRCALPHA;
		smallFontStripContext.ImageControl[KM_IMAGE_PARAM1].nDSTBlendingMode		= KM_INVSRCALPHA;
		smallFontStripContext.ImageControl[KM_IMAGE_PARAM1].bSRCSelect			= KM_FALSE;
		smallFontStripContext.ImageControl[KM_IMAGE_PARAM1].bDSTSelect			= KM_FALSE;
		smallFontStripContext.ImageControl[KM_IMAGE_PARAM1].nFogMode				= KM_NOFOG;
		smallFontStripContext.ImageControl[KM_IMAGE_PARAM1].bColorClamp			= KM_FALSE;	
		smallFontStripContext.ImageControl[KM_IMAGE_PARAM1].bUseAlpha			= KM_TRUE;
		smallFontStripContext.ImageControl[KM_IMAGE_PARAM1].bIgnoreTextureAlpha	= KM_FALSE;
		smallFontStripContext.ImageControl[KM_IMAGE_PARAM1].nFilterMode			= KM_POINT_SAMPLE;
   		smallFontStripContext.ImageControl[KM_IMAGE_PARAM1].pTextureSurfaceDesc = &fontptr->surface;
		kmGenerateStripHead03(&smallFontStripHead,&smallFontStripContext);
	}
	else
	{
		// initialise font strip context and head
		memset(&fontStripContext,0,sizeof(fontStripContext));
		memset(&fontStripHead,0,sizeof(fontStripHead));
		fontStripContext.nSize = sizeof(KMSTRIPCONTEXT);
    	kmInitStripContext(KM_STRIPCONTEXT_SYS_GOURAUD | KM_TRANS_POLYGON, &fontStripContext);
//		fontStripContext.StripControl.nListType		 						= KM_TRANS_POLYGON;
		fontStripContext.StripControl.nUserClipMode	 						= KM_USERCLIP_DISABLE;	
		fontStripContext.StripControl.nShadowMode			 				= KM_NORMAL_POLYGON;
		fontStripContext.StripControl.bGouraud		 						= KM_TRUE;
		fontStripContext.ObjectControl.nDepthCompare			 			= KM_ALWAYS;
		fontStripContext.ObjectControl.nCullingMode			 				= KM_NOCULLING;	
		fontStripContext.ObjectControl.bZWriteDisable						= KM_FALSE;
		fontStripContext.ImageControl[KM_IMAGE_PARAM1].nSRCBlendingMode		= KM_SRCALPHA;
		fontStripContext.ImageControl[KM_IMAGE_PARAM1].nDSTBlendingMode		= KM_INVSRCALPHA;
		fontStripContext.ImageControl[KM_IMAGE_PARAM1].bSRCSelect			= KM_FALSE;
		fontStripContext.ImageControl[KM_IMAGE_PARAM1].bDSTSelect			= KM_FALSE;	
		fontStripContext.ImageControl[KM_IMAGE_PARAM1].nFogMode				= KM_NOFOG;
		fontStripContext.ImageControl[KM_IMAGE_PARAM1].bColorClamp			= KM_FALSE;		
		fontStripContext.ImageControl[KM_IMAGE_PARAM1].bUseAlpha			= KM_TRUE;
		fontStripContext.ImageControl[KM_IMAGE_PARAM1].bIgnoreTextureAlpha	= KM_FALSE;
		fontStripContext.ImageControl[KM_IMAGE_PARAM1].nFilterMode			= KM_POINT_SAMPLE;
	   	fontStripContext.ImageControl[KM_IMAGE_PARAM1].pTextureSurfaceDesc = &fontptr->surface;
		kmGenerateStripHead03(&fontStripHead,&fontStripContext);
	}

	// initialise button strip context and head
	memset(&buttonStripContext,0,sizeof(buttonStripContext));
	memset(&buttonStripHead,0,sizeof(buttonStripHead));
	buttonStripContext.nSize = sizeof(KMSTRIPCONTEXT);
    kmInitStripContext(KM_STRIPCONTEXT_SYS_GOURAUD | KM_TRANS_POLYGON, &buttonStripContext);
	buttonStripContext.StripControl.nListType		 						= KM_TRANS_POLYGON;
	buttonStripContext.StripControl.nUserClipMode	 						= KM_USERCLIP_DISABLE;
	buttonStripContext.StripControl.nShadowMode			 				= KM_NORMAL_POLYGON;
	buttonStripContext.StripControl.bGouraud		 						= KM_TRUE;
	buttonStripContext.ObjectControl.nDepthCompare			 			= KM_ALWAYS;
	buttonStripContext.ObjectControl.nCullingMode			 				= KM_NOCULLING;
	buttonStripContext.ObjectControl.bZWriteDisable						= KM_FALSE;
	buttonStripContext.ImageControl[KM_IMAGE_PARAM1].nSRCBlendingMode		= KM_SRCALPHA;
	buttonStripContext.ImageControl[KM_IMAGE_PARAM1].nDSTBlendingMode		= KM_INVSRCALPHA;
	buttonStripContext.ImageControl[KM_IMAGE_PARAM1].bSRCSelect			= KM_FALSE;
	buttonStripContext.ImageControl[KM_IMAGE_PARAM1].bDSTSelect			= KM_FALSE;
	buttonStripContext.ImageControl[KM_IMAGE_PARAM1].nFogMode				= KM_NOFOG;
	buttonStripContext.ImageControl[KM_IMAGE_PARAM1].bColorClamp			= KM_FALSE;	
	buttonStripContext.ImageControl[KM_IMAGE_PARAM1].bUseAlpha			= KM_TRUE;
	buttonStripContext.ImageControl[KM_IMAGE_PARAM1].bIgnoreTextureAlpha	= KM_FALSE;
	buttonStripContext.ImageControl[KM_IMAGE_PARAM1].nFilterMode			= KM_POINT_SAMPLE;
	kmGenerateStripHead03(&buttonStripHead,&buttonStripContext);

	return fontptr;
}

extern psFont *fontSmall;
void fontPrintScaled(psFont *font, short x,short y, char *text, unsigned char r, unsigned char g, unsigned char b,int scale)
{
	unsigned char	*strPtr, c;
	int				cx,cy, loop,yAdd;
	TextureType		*letter;
	POLY_FT4 	*ft4; 

	if(font == fontSmall)
		fontStripHeadPtr = &smallFontStripHead;
	else
		fontStripHeadPtr = &fontStripHead;

	strPtr = (unsigned char *)text;
	cx = x;
	cy = y;

	if(font == fontSmall)
		yAdd = -3;
	else
		yAdd = 0;

	while(*strPtr)
	{
		c = *strPtr;
		switch(c)
		{
		case '\n':
			y += font->height;
			cx = x;
			cy = y;
			break;
		case ' ':
			x += font->height/2;
			break;
		case '@':
			strPtr++;
			switch(*strPtr)
			{
			case 'X':
			    fontDispSprite(buttonSprites[2], x,y + yAdd,r,g,b,font->alpha,font->size);
				x += buttonSprites[2]->w+2;
				break;
			case 'T':
			case 'C':
			   	fontDispSprite(buttonSprites[1], x,y + yAdd,r,g,b,font->alpha,font->size);
				x += buttonSprites[1]->w+2;
				break;
			case 'S':
			   	fontDispSprite(buttonSprites[3], x,y + yAdd,r,g,b,font->alpha,font->size);
				x += buttonSprites[3]->w+2;
				break;
//			case 'T':
//			   	fontDispSprite(buttonSprites[0], x,y + yAdd,r,g,b,font->alpha,font->size);
//				x += buttonSprites[0]->w+2;
//				break;
			case 'L':
//ma			   	fontDispSprite(buttonSprites[4], x,y + yAdd,r,g,b,font->alpha);
//ma				x += buttonSprites[4]->w+2;
				break;
			case 'R':
//ma			   	fontDispSprite(buttonSprites[5], x,y + yAdd,r,g,b,font->alpha);
//ma				x += buttonSprites[5]->w+2;
				break;
			}

/*			for(loop = 0; loop < numOtherSprites; loop ++)
			{
				if((*(strPtr+1)) == otherChars[loop])
				{
					fontDispSprite(otherSprites[loop], x+3,y-3,r,g,b,font->alpha);
					strPtr++;
					x += otherSprites[loop]->w+2;
					break;
				}
			}
*/
			break;

		default:
			loop = font->charlookup[c];
			if (loop<font->numchars)
			{
//				letter = &font->txPtr[loop];
//				if ((x>-350)&&(x<320))
				{
/*					BEGINPRIM(ft4, POLY_FT4);
					setPolyFT4(ft4);
					setXYWH(ft4, x,y-((c>127)?(1):(0)), FMul(letter->w,scale),FMul(letter->h,scale)-1);
					setRGB0(ft4, r,g,b);
					setUVWH(ft4, letter->x,letter->y, letter->w-1, letter->h-1);
					ft4->tpage = letter->tpage;
					setSemiTrans(ft4, (font->alpha > 0) ? 1 : 0);
					if(font->alpha)
						SETSEMIPRIM(ft4, font->alpha);
					ft4->clut = letter->clut;
					ENDPRIM(ft4, 4, POLY_FT4);
*/
//					fontDispChar(letter, x,y-((c>127)?(1):(0)), r,g,b, font->alpha,scale);
					
					fontDispChar(font,font->uvOffsets[loop], x,y/*-((c>127)?(1):(0))*/, r,g,b, font->alpha, font->size, scale);
					
				}
				x += FMul(font->pixelwidth[loop]+2,scale);
			}
		}
		strPtr++;
	}
}


/**************************************************************************
	FUNCTION:	fontExtentW()
	PURPOSE:	Get width of string using font
	PARAMETERS:	font ptr, string
	RETURNS:	Width in pixels
**************************************************************************/

int fontExtentWScaled(psFont *font, char *text,int scale)
{
	unsigned char	*strPtr, c;
	int		loop, x, maxW = 0;

	// trap null text pointer
	if (text == NULL)
		return 0;

	strPtr = (unsigned char *)text;
	x = 0;
	while(*strPtr)
	{
		c = *strPtr;
		switch(c)
		{
		case '\n':
			if (x>maxW)
				maxW = x;
			x = 0;
			break;
		case ' ':
			x += font->height/2;
			break;
		case '@':
			switch(*(strPtr+1))
			{
			case 'X':
				strPtr++;
				x += buttonSprites[2]->w+2;
				break;
			case 'T':
			case 'C':
				strPtr++;
				x += buttonSprites[1]->w+2;
				break;
			case 'S':
				strPtr++;
				x += buttonSprites[3]->w+2;
				break;
//			case 'T':
//				strPtr++;
//				x += buttonSprites[0]->w+2;
//				break;
//			case 'L':
//				strPtr++;
//				x += buttonSprites[4]->w+2;
//				break;
//			case 'R':
//				strPtr++;
//				x += buttonSprites[5]->w+2;
//				break;
			}

/*ma
			for(loop = 0; loop < numOtherSprites; loop ++)
			{
				if((*(strPtr+1)) == otherChars[loop])
				{
					strPtr++;
					x += otherSprites[loop]->w+2;
					break;
				}
			}

			break;
*/			
		default:
			loop = font->charlookup[c];
			if (loop<font->numchars)
				x += font->pixelwidth[loop]+2;
		}
		strPtr++;
	}
	if (x>maxW)
		maxW = x;
	return FMul(maxW,scale);
}

/**************************************************************************
	FUNCTION:	fontUnload()
	PURPOSE:	Unload font from VRAM and free info
	PARAMETERS:	Font ptr
	RETURNS:	
**************************************************************************/

void fontUnload(psFont *font)
{
	kmFreeTexture(&font->surface);
	FREE(font);
}




// *ASL* 13/08/2000
/* ---------------------------------------------------------
   Function : fontDispChar
   Purpose : display font char
   Parameters : font pointer, uv offsets, x,y, r,g,b,a, font size, PSX scale
   Returns : 
   Info : 
*/

void fontDispChar(psFont *font, DCKFLOAT2DVECTOR offset, short x, short y, unsigned char r, unsigned char g, unsigned char b, unsigned char alpha, int size, int psxScale)
{
	float	scaleSize;
	float	uo, vo;

	scaleSize = (float)font->size * ((float)psxScale / 4096.0f);

	if (font == fontSmall)
	{
		uo = U_OFFSET_SMALL;
		vo = V_OFFSET_SMALL;
	}
	else
	{
		uo = U_OFFSET;
		vo = V_OFFSET;
	}

	// initialise char strip
	fontVertices[0].fX = x;
	fontVertices[0].fY = y + scaleSize;
	fontVertices[0].u.fZ = 1.0;
	fontVertices[0].fU = offset.v[0];
	fontVertices[0].fV = offset.v[1] + vo;
	fontVertices[0].uBaseRGB.dwPacked = RGBA(r,g,b,alpha);

	fontVertices[1].fX = x;
	fontVertices[1].fY = y;
	fontVertices[1].u.fZ = 1.0;
	fontVertices[1].fU = offset.v[0];
	fontVertices[1].fV = offset.v[1];
	fontVertices[1].uBaseRGB.dwPacked = RGBA(r,g,b,alpha);

	fontVertices[2].fX = x + scaleSize;
	fontVertices[2].fY = y + scaleSize;
	fontVertices[2].u.fZ = 1.0;
	fontVertices[2].fU = offset.v[0] + uo;
	fontVertices[2].fV = offset.v[1] + vo;
	fontVertices[2].uBaseRGB.dwPacked = RGBA(r,g,b,alpha);
	
	fontVertices[3].fX = x + scaleSize;
	fontVertices[3].fY = y;
	fontVertices[3].u.fZ = 1.0;
	fontVertices[3].fU = offset.v[0] + uo;
	fontVertices[3].fV = offset.v[1];
	fontVertices[3].uBaseRGB.dwPacked = RGBA(r,g,b,alpha);

	// print strip
	kmStartStrip(&vertexBufferDesc, fontStripHeadPtr);	
	kmSetVertex(&vertexBufferDesc, &fontVertices[0], KM_VERTEXTYPE_03, sizeof(KMVERTEX_03));
	kmSetVertex(&vertexBufferDesc, &fontVertices[1], KM_VERTEXTYPE_03, sizeof(KMVERTEX_03));
	kmSetVertex(&vertexBufferDesc, &fontVertices[2], KM_VERTEXTYPE_03, sizeof(KMVERTEX_03));	
	kmSetVertex(&vertexBufferDesc, &fontVertices[3], KM_VERTEXTYPE_03, sizeof(KMVERTEX_03));	
	kmEndStrip(&vertexBufferDesc);
}


/**************************************************************************
	FUNCTION:	fontPrint()
	PURPOSE:	Display string in font at pos in colour
	PARAMETERS:	font ptr, x,y, string, r,g,b
	RETURNS:	
**************************************************************************/

void fontPrint(psFont *font, short x,short y, char *text, unsigned char r, unsigned char g, unsigned char b)
{
	unsigned char	*strPtr, c;
	int				cx,cy, loop;
	TextureType		*letter;
	
	if(font == fontSmall)
		fontStripHeadPtr = &smallFontStripHead;
	else
		fontStripHeadPtr = &fontStripHead;
		
	x += 256;
	y += 128;
	x = ((float)x * (640.0/512.0));
	y = ((float)y * (480.0/256.0));

	strPtr = (unsigned char *)text;
	cx = x;
	cy = y;
	while(*strPtr)
	{
		c = *strPtr;
		switch(c)
		{
			case '\n':
				y += font->height;
				cx = x;
				cy = y;
				break;
				
			case ' ':
				x += (font->height/2)*font->scale;
				break;
				
			case '@':
				switch(*(strPtr+1))
				{
					case 'X':
//					    fontDispSprite(buttonSprites[2], x+3,y-3, font->alpha, font->size);
						strPtr++;
						x += (buttonSprites[2]->w*font->scale)+6;
						break;
					case 'T':
					case 'C':
//					   	fontDispSprite(buttonSprites[1], x+3,y-3, font->alpha, font->size);
						strPtr++;
						x += (buttonSprites[1]->w*font->scale)+6;
						break;
					case 'S':
//					   	fontDispSprite(buttonSprites[3], x+3,y-3, font->alpha, font->size);
						strPtr++;
						x += (buttonSprites[3]->w*font->scale)+6;
						break;
//ma					case 'T':
//ma					   	fontDispSprite(buttonSprites[0], x+3,y-3, font->alpha, font->size);
//ma						strPtr++;
//ma						x += buttonSprites[0]->w+6;
//ma						break;
				}
				break;

			default:
				loop = font->charlookup[c];
				if(loop<font->numchars)
				{
					letter = &font->txPtr[loop];
//					if((x>-350)&&(x<640))
						fontDispChar(font,font->uvOffsets[loop], x,y-((c>127)?(1):(0)), r,g,b, font->alpha, font->size, 4096);
					x += font->pixelwidth[loop];
				}
		}
		strPtr++;
	}
}


/**************************************************************************
	FUNCTION:	fontExtentW()
	PURPOSE:	Get width of string using font
	PARAMETERS:	font ptr, string
	RETURNS:	Width in pixels
**************************************************************************/

int fontExtentW(psFont *font, char *text)
{
	unsigned char	*strPtr, c;
	int		loop, x, maxW = 0;

	strPtr = (unsigned char *)text;
	x = 0;
	while(*strPtr)
	{
		c = *strPtr;
		switch(c)
		{
			case '\n':
				if (x>maxW)
					maxW = x;
				x = 0;
				break;
				
			case ' ':
				x += (font->height/2)*font->scale;
				break;
				
			case '@':
				switch(*(strPtr+1))
				{
					case 'X':
						strPtr++;
						x += (buttonSprites[2]->w*font->scale)+6;
						break;
					case 'C':
						strPtr++;
						x += (buttonSprites[1]->w*font->scale)+6;
						break;
					case 'S':
						strPtr++;
						x += (buttonSprites[3]->w*font->scale)+6;
						break;
					case 'T':
						strPtr++;
						x += (buttonSprites[0]->w*font->scale)+6;
						break;
				}
				break;
				
			default:
				loop = font->charlookup[c];
				if(loop<font->numchars)
					x += font->pixelwidth[loop];
		}
		strPtr++;
	}
	if(x>maxW)
		maxW = x;

	if(maxW > 0)
		maxW = ((float)maxW * (512.0/640.0));
	
	return maxW;
}


/**************************************************************************
	FUNCTION:	fontFitToWidth()
	PURPOSE:	Wrap string to width into buffer
	PARAMETERS:	font ptr, width, string, buffer
	RETURNS:	Number of lines
**************************************************************************/

int fontFitToWidth(psFont *font, int width, char *text, char *buffer)
{
	char	buf[256], *bufPtr, *outPtr = buffer;
	int		pixLen, lines = 0;

	memset(buf, 0, sizeof(buf));
	bufPtr = buf;
	while(1)
	{
//		if(*text>=' ')
			*bufPtr++ = *text;
		pixLen = fontExtentW(font, buf);
		if((pixLen>width)||(*text=='\0'))
		{
			if(pixLen>width)
			{
				while(*bufPtr!=' ')
				{
					bufPtr--;
					text--;
				}
				text++;
			}
			*bufPtr = '\0';
			strcpy(outPtr, buf);
			outPtr += strlen(buf)+1;
			lines++;
			bufPtr = buf;
			memset(buf, 0, sizeof(buf));
			if(*text=='\0')
				break;
		}
		text++;
	}
	
	outPtr = buffer;
	for(pixLen=0; pixLen<lines; pixLen++)
	{
		outPtr += strlen(outPtr)+1;
	}
	
	return lines;
}

/**************************************************************************
	FUNCTION:	fontFitToWidth()
	PURPOSE:	Wrap string to width into buffer
	PARAMETERS:	font ptr, width, string, buffer
	RETURNS:	Number of lines
**************************************************************************/

int fontFitToWidthScaled(psFont *font, int width, char *text, char *buffer)
{
	char	buf[256], *bufPtr, *outPtr = buffer;
	int		pixLen, lines = 0;

	memset(buf, 0, sizeof(buf));
	bufPtr = buf;
	while(1)
	{
//		if(*text>=' ')
		*bufPtr++ = *text;

		pixLen = fontExtentW(font, buf);
		if((pixLen>width)||(*text=='\0')||(*text=='\\'))
		{
			if(*text=='\\')
			{
				text++;
				text++;
				bufPtr--;
			}
			else if(pixLen>width)
			{
				while(*bufPtr != ' ')
				{
					bufPtr--;
					text--;
				}
				text++;
			}
			*bufPtr = '\0';
			strcpy(outPtr, buf);
			outPtr += strlen(buf)+1;
			lines++;
			bufPtr = buf;
			memset(buf, 0, sizeof(buf));
			if(*text=='\0')
				break;
		}
		text++;
	}
	
	outPtr = buffer;
	for(pixLen=0; pixLen<lines; pixLen++)
	{
		outPtr += strlen(outPtr)+1;
	}
	
	return lines;
}

/**************************************************************************
	FUNCTION:	fontPrintN()
	PURPOSE:	Display (partial) string in font at pos in colour
	PARAMETERS:	font ptr, x,y, string, r,g,b, max chars
	RETURNS:	
**************************************************************************/

void fontPrintN(psFont *font, short x,short y, char *text, unsigned char r, unsigned char g, unsigned char b, int n)
{
	unsigned char	*strPtr, c;
	int				cx,cy, loop;
	TextureType		*letter;

	x += 256;
	y += 128;
	x = ((float)x * (640.0/512.0));
	y = ((float)y * (480.0/256.0));

	strPtr = (unsigned char *)text;
	cx = x;
	cy = y;
	while ((*strPtr) && (n>0))
	{
		c = *strPtr;
		switch(c)
		{
			case '\n':
				y += font->height;
				cx = x;
				cy = y;
				break;
				
			case ' ':
				x += font->height/2;
				break;
				
			case '@':
				switch(*(strPtr+1))
				{
					case 'X':
//					    fontDispSprite(buttonSprites[2], x+3,y-3, font->alpha, font->size);
						strPtr++;
						x += buttonSprites[2]->w+6;
						break;
					case 'C':
//					   	fontDispSprite(buttonSprites[1], x+3,y-3, font->alpha, font->size);
						strPtr++;
						x += buttonSprites[1]->w+6;
						break;
					case 'S':
//					   	fontDispSprite(buttonSprites[3], x+3,y-3, font->alpha, font->size);
						strPtr++;
						x += buttonSprites[3]->w+6;
						break;
					case 'T':
//					   	fontDispSprite(buttonSprites[0], x+3,y-3, font->alpha, font->size);
						strPtr++;
						x += buttonSprites[0]->w+6;
						break;
				}
				break;
				
			default:
				loop = font->charlookup[c];
				if(loop<font->numchars)
				{
//					letter = &font->txPtr[loop];
					if((x>-350)&&(x<640))
						fontDispChar(font,font->uvOffsets[loop], x,y-((c>127)?(1):(0)), r,g,b,font->alpha, font->size, 4096);
					x += font->pixelwidth[loop];
				}
		}
		strPtr++;
		n--;
	}	
}

void fontDispSprite(TextureType *tex, short x,short y, uchar r, uchar g, uchar b, uchar alpha, int size)
{
	USHORT		w,h;
//	float		size = 32;

	buttonVertices[0].fX = x+5;
	buttonVertices[0].fY = y + (size*1.0)+5;
	buttonVertices[0].u.fZ = 1.0;
	buttonVertices[0].fU = 0;
	buttonVertices[0].fV = 1;
	buttonVertices[0].uBaseRGB.dwPacked = RGBA(r,g,b,alpha);

	buttonVertices[1].fX = x+5;
	buttonVertices[1].fY = y+5;
	buttonVertices[1].u.fZ = 1.0;
	buttonVertices[1].fU = 0;
	buttonVertices[1].fV = 0;
	buttonVertices[1].uBaseRGB.dwPacked = RGBA(r,g,b,alpha);

	buttonVertices[2].fX = x + (size*1.0)+5;
	buttonVertices[2].fY = y + (size*1.0)+5;
	buttonVertices[2].u.fZ = 1.0;
	buttonVertices[2].fU = 1;
	buttonVertices[2].fV = 1;
	buttonVertices[2].uBaseRGB.dwPacked = RGBA(r,g,b,alpha);
	
	buttonVertices[3].fX = x + (size*1.0)+5;
	buttonVertices[3].fY = y+5;
	buttonVertices[3].u.fZ = 1.0;
	buttonVertices[3].fU = 1;
	buttonVertices[3].fV = 0;
	buttonVertices[3].uBaseRGB.dwPacked = RGBA(r,g,b,alpha);

	kmChangeStripTextureSurface(&buttonStripHead,KM_IMAGE_PARAM1,&tex->surface);
	
	kmStartStrip(&vertexBufferDesc, &buttonStripHead);	
	kmSetVertex(&vertexBufferDesc, &buttonVertices[0], KM_VERTEXTYPE_03, sizeof(KMVERTEX_03));
	kmSetVertex(&vertexBufferDesc, &buttonVertices[1], KM_VERTEXTYPE_03, sizeof(KMVERTEX_03));
	kmSetVertex(&vertexBufferDesc, &buttonVertices[2], KM_VERTEXTYPE_03, sizeof(KMVERTEX_03));
	kmSetVertex(&vertexBufferDesc, &buttonVertices[3], KM_VERTEXTYPE_03, sizeof(KMVERTEX_03));
	kmEndStrip(&vertexBufferDesc);
}

void fontInitButtonSprites()
{
	buttonSprites[0] = FindTexture("BUT_TRIA");
	buttonSprites[1] = FindTexture("BUT_CIRC");
	buttonSprites[2] = FindTexture("BUT_CROS");
	buttonSprites[3] = FindTexture("BUT_SQUA");

/*
	buttonSprites[0] = triangle;
	buttonSprites[1] = circle;
	buttonSprites[2] = cross;
	buttonSprites[3] = square;
*/
}

void fontRegisterButtonSprites(TextureType *triangle, TextureType *circle, TextureType *cross, TextureType *square)
{
	buttonSprites[0] = triangle;
	buttonSprites[1] = circle;
	buttonSprites[2] = cross;
	buttonSprites[3] = square;
}