/*
	(c) 2000 Interactive Studios Ltd.

	File		: DCK_Texture.h
	Programmer	: Andy Eder
	Created		: 19/01/00

	Updated		: 19/01/00	Andy Eder
				: < revision dates here >

----------------------------------------------------------------------------------------------- */
#ifndef DCK_TEXTURE_H_INCLUDED
#define DCK_TEXTURE_H_INCLUDED

#include "isltex.h"
#include "..\prefix_dc.h"

//----- [ DEFINES ] -----------------------------------------------------------------------------

extern void *(*MallocPtr)(unsigned long);   // Default allocation is syMalloc().
extern void (*FreePtr)(void *);               // Default de-allocation is syFree().

#define _TWIDDLEADR(COORD)      ((COORD & 1)             | ((COORD >> 1) & 1) << 2   |  \
                                ((COORD >> 2) & 1) << 4  | ((COORD >> 3) & 1) << 6   |  \
                                ((COORD >> 4) & 1) << 8  | ((COORD >> 5) & 1) << 10  |  \
                                ((COORD >> 6) & 1) << 12 | ((COORD >> 7) & 1) << 14  |  \
                                ((COORD >> 8) & 1) << 16 | ((COORD >> 9) & 1) << 18)

#define TwiddledOffset(X, Y)	((_TWIDDLEADR(X) << 1) | _TWIDDLEADR(Y))

// Set up 32-byte aligned malloc() / free() memory management.
#define Align32Malloc			(*MallocPtr)
#define Align32Free				(*FreePtr)

// Align a pointer to the nearest 32-byte aligned memory address. 
#define Align32Byte(ADR)		(((long) ADR) + 0x1F) & 0xFFFFFFE0)

#define	MAX_TEXTURE_LIST		500

#ifdef NTSC_VERSION
#define	MAX_MALLOC_LIST			2000
#else
#define	MAX_MALLOC_LIST			3000
#endif

//----- [ TYPEDEFS ] -----------------------------------------------------------------------------

typedef struct TAG_MALLOC_TYPE
{
	Uint32	*address;
	int		size;
} MALLOC_TYPE;

typedef struct TAG_MALLOC_LIST_TYPE
{
	int			numEntries;
	int			totalMalloc;
	MALLOC_TYPE	*list;
} MALLOC_LIST_TYPE;

extern MALLOC_LIST_TYPE	mallocList;

//----- [ GLOBALS ] -----------------------------------------------------------------------------

extern int			DCKnumTextures;
extern TextureType	*DCKtextureList;
extern char			textureDir[];

//----- [ FUNCTION PROTOTYPES ] -----------------------------------------------------------------

TextureType* FindTexture(char *name);

TextureType* FindTexture2(int texID);

int FindTextureN(char *name);

int FindTextureCRC(int texID);

void Init32Malloc(void);

void *_Align32Malloc(unsigned long Size);

void _Align32Free (void *Ptr);

void memoryReset();

void Rectangle2Twiddled(char *Rectangle, char *Twiddled, int Dimension, int BitsPerPixel);

PKMDWORD LoadTextureFile(char *Filename);

int loadPVRFile(char *Filename, int flags);

int loadPVRFileIntoSurface(char *Filename, char *directory, KMSURFACEDESC *surface, int flags);

#endif
