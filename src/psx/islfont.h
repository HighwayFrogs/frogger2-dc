/************************************************************************************
	ISL PSX LIBRARY	(c) 1999 Interactive Studios Ltd.

	islfont.h:		Font support

************************************************************************************/

#include "include.h"

#ifndef __ISLFONT_H__
#define __ISLFONT_H__

typedef struct _psFont
{
	int					numchars;
	int					height;
	int					dataoffset[256];
	int					width[256];
	int					pixelwidth[256];
	TextureType			*txPtr;
	unsigned short		clut;
	unsigned char		charlookup[256];
	unsigned char		alpha;	
	
	DCKFLOAT2DVECTOR	uvOffsets[256];	
	float				uoffset,voffset;
	KMSURFACEDESC		surface;
	float				scale;
	int				size;
} psFont;

extern TextureType	*buttonSprites[6];

/**************************************************************************
	FUNCTION:	fontLoad()
	PURPOSE:	Load font into VRAM ready for use
	PARAMETERS:	Filename
	RETURNS:	Ptr to font info
**************************************************************************/

psFont *fontLoad(char *fontname);

void fontPrintScaled(psFont *font, short x,short y, char *text, unsigned char r, unsigned char g, unsigned char b,int scale);

/**************************************************************************
	FUNCTION:	fontUnload()
	PURPOSE:	Unload font from VRAM and free info
	PARAMETERS:	Font ptr
	RETURNS:	
**************************************************************************/

void fontUnload(psFont *font);


/**************************************************************************
	FUNCTION:	fontPrint()
	PURPOSE:	Display string in font at pos in colour
	PARAMETERS:	font ptr, x,y, string, r,g,b
	RETURNS:	
**************************************************************************/

void fontPrint(psFont *font, short x,short y, char *text, unsigned char r, unsigned char g, unsigned char b);


/**************************************************************************
	FUNCTION:	fontExtentW()
	PURPOSE:	Get width of string using font
	PARAMETERS:	font ptr, string
	RETURNS:	Width in pixels
**************************************************************************/

int fontExtentW(psFont *font, char *text);


/**************************************************************************
	FUNCTION:	fontFitToWidth()
	PURPOSE:	Wrap string to width into buffer
	PARAMETERS:	font ptr, width, string, buffer
	RETURNS:	Number of lines
**************************************************************************/

int fontFitToWidth(psFont *font, int width, char *text, char *buffer);

// *ASL* 13/08/2000 - Added PSX scale parameter
void fontDispChar(psFont *font, DCKFLOAT2DVECTOR offset, short x,short y, unsigned char r, unsigned char g, unsigned char b, unsigned char alpha, int size, int psxScale);

/**************************************************************************
	FUNCTION:	fontPrintN()
	PURPOSE:	Display (partial) string in font at pos in colour
	PARAMETERS:	font ptr, x,y, string, r,g,b, max chars
	RETURNS:	
**************************************************************************/

void fontPrintN(psFont *font, short x,short y, char *text, unsigned char r, unsigned char g, unsigned char b, int n);

/**************************************************************************
	FUNCTION:	fontRegisterButtonSprites()
	PURPOSE:	Set sprites to be used for button display
	PARAMETERS:	pointers to sprites
	RETURNS:	
**************************************************************************/

void fontRegisterButtonSprites(TextureType *triangle, TextureType *circle, TextureType *cross, TextureType *square);

void fontDispSprite(TextureType *tex, short x,short y, uchar r, uchar g, uchar b, uchar alpha, int size);


#endif //__ISLFONT_H__
