/*-----------------------------------------------------------------------------------------------

	File        : DCK_Texture.c
	Programmer	: Martyn Ash
	Description : Texture handler rountines
	Date        : 04/11/97
	Version     : 0.01
	
-----------------------------------------------------------------------------------------------*/

// --------
// Includes

#include <shinobi.h>
#include <kamui2.h>
#include <sg_syhw.h>
#include <sg_sycbl.h>
#include <machine.h>

#include "DCK_Types.h"
#include "DCK_Maths.h"
#include "DCK_Texture.h"
#include "isltex.h"
#include "gte.h"
#include "main.h"

// -------
// Globals

int				DCKnumTextures = 0;
TextureType		*DCKtextureList;
char			textureDir[256] = "textures";

long			numMallocs = 0;

MALLOC_LIST_TYPE	mallocList;

// -------
// Defines

void *(*MallocPtr)(unsigned long) = syMalloc;   // Default allocation is syMalloc().
void (*FreePtr)(void *) = syFree;               // Default de-allocation is syFree().


// *ASL* 13/08/2000 - Reset to BootROM on lid being opened by the user
extern unsigned int globalAbortFlag;
extern void resetToBootROM();



/*	--------------------------------------------------------------------------------
	Function 	: FindTexture
	Purpose 	: 
	Parameters 	: 
	Returns 	: 
	Info 		:
*/

TextureType* FindTexture(char *name)
{
	int			x,y,i;
	TextureType	*tex;
	char		*temp;
	int			texID;
	
	texID = UpdateCRC(name);

	tex	= DCKtextureList;
	for(i=0;i<DCKnumTextures;i++)
	{
		if(tex->imageCRC == texID)
		{
			strcpy(tex->ident,name);
			return tex;
		}
		tex++;
	}
//	return FALSE;
	return DCKtextureList;	
}

/*	--------------------------------------------------------------------------------
	Function 	: FindTexture2
	Purpose 	: 
	Parameters 	: 
	Returns 	: 
	Info 		:
*/

TextureType* FindTexture2(int texID)
{
	int			x,y,i;
	TextureType	*tex;
	char		*temp;
	
	tex	= DCKtextureList;
	for(i=0;i<DCKnumTextures;i++)
	{
		if(tex->imageCRC == texID)
			return tex;
		tex++;
	}
	return FALSE;
	
}
/*	--------------------------------------------------------------------------------
	Function 	: FindTextureN
	Purpose 	: 
	Parameters 	: 
	Returns 	: 
	Info 		:
*/

int FindTextureN(char *name)
{
	int				x,y,i;
	TextureType		*tex;
	char			*temp;
	int				texID;
	
	texID = UpdateCRC(name);

	tex	= DCKtextureList;
	for(i=0;i<DCKnumTextures;i++)
	{
		if(tex->imageCRC == texID)
		{
			strcpy(tex->ident,name);
			return i;
		}
		tex++;
	}
	return FALSE;
}

/*	--------------------------------------------------------------------------------
	Function 	: FindTexture
	Purpose 	: 
	Parameters 	: 
	Returns 	: 
	Info 		:
*/

int FindTextureCRC(int texID)
{
	int				x,y,i;
	TextureType		*tex;
	char			*temp;
	
	tex	= DCKtextureList;
	for(i=0;i<DCKnumTextures;i++)
	{
		if(tex->imageCRC == texID)
		{
			return i;
		}
		tex++;
	}
	return FALSE;
}

/*	--------------------------------------------------------------------------------
	Function 	: Init32Malloc
	Purpose 	: Check the built-in malloc() / free() routines for 32-byte alignment
	Parameters 	: 
	Returns 	: 
	Info 		:
*/

void Init32Malloc (void)
{
	int			i;
    MALLOC_TYPE *ptr;

    MallocPtr = _Align32Malloc;
    FreePtr = _Align32Free;

	mallocList.numEntries = 0;
	mallocList.list = syMalloc(MAX_MALLOC_LIST*sizeof(MALLOC_TYPE));
	
	ptr = mallocList.list;
	for(i=0;i<MAX_MALLOC_LIST;i++)
	{
		ptr->address = NULL;
		ptr->size = -1;
		ptr++;
	}	
}

/*	--------------------------------------------------------------------------------
	Function 	: Align32Malloc
	Purpose 	: Allocate memmory aligned to a 32 byte boundary
	Parameters 	: size of malloc required
	Returns 	: pointer to memmory
	Info 		:
*/

void *_Align32Malloc (unsigned long Size)
{
	int			i,j=0;
    void 		*ptr,*AlignedPtr;
    char	 	*cPtr;
	MALLOC_TYPE	*mptr;

    // Adjust the requested size to a multiple of 32 bytes. 
    Size = (Size + 0x1F) & 0xFFFFFFE0;

    // Allocate requested size plus 32 bytes padding. 
    ptr = syMalloc (Size + 32);

    // Align to 32-bytes (add 32 bytes if already aligned - the padding is used below). */
    AlignedPtr = (void *) ((((long) ptr) + 0x20) & 0xFFFFFFE0);

    // Place cookie one byte earlier for _Align32Free(). 
    *((char *) AlignedPtr - 1) = (char) ((long) AlignedPtr - (long) ptr);

	ptr = AlignedPtr;
	
//	ptr = syMalloc(Size);
    if(ptr == NULL)
	{
		j=8;
		return NULL;
	}

	mptr = mallocList.list;
	for(i=0;i<MAX_MALLOC_LIST;i++)
	{
		if(mptr->address == NULL)
			break;
		mptr++;
	}
	
	if(i>=MAX_MALLOC_LIST)
	{
		j=8;
		return;
	}

//	mptr = &mallocList.list[mallocList.numEntries];
//	mptr->address = ptr;
//	mptr->size = Size;
//	mallocList.numEntries++;
//	mallocList.totalMalloc += Size;
		
	mptr->address = ptr;
	mptr->size = Size;
	mallocList.numEntries++;
	mallocList.totalMalloc += Size;
	
	// clear memmory
    cPtr = (char*)ptr;
    for(i=0;i<Size;i++)
    	*cPtr++ = 0;    	
	
    return (ptr);
}

/*	--------------------------------------------------------------------------------
	Function 	: Align32Free
	Purpose 	: Free memmory aligned to a 32 byte boundary
	Parameters 	: Pointer to memmory
	Returns 	: none
	Info 		:
*/

void _Align32Free(void *Ptr)
{
	int			i;
	MALLOC_TYPE	*mptr,*newptr;
   	char 		Diff;

	if(Ptr == NULL)
		return;

/*	// check to see it is actually a malloc
	mptr = mallocList.list;
	for(i=0;i<mallocList.numEntries;i++)
	{
		if(mptr->address == Ptr)
			break;
		mptr++;
	}

	if(i >= mallocList.numEntries)
		return;
		
	syFree(Ptr);

	// clear malloc store	
	mallocList.totalMalloc -= mptr->size;
	if(i < (mallocList.numEntries-1))
	{
		newptr = &mallocList.list[mallocList.numEntries-1];
		mptr->address = newptr->address;
		mptr->size = newptr->size;
		newptr->address = NULL;
		newptr->size = -1;
	}
	else
	{
		mptr->address = NULL;
		mptr->size = -1;	
	}
	mallocList.numEntries--;	
*/		
  
	// check to see it is actually a malloc
	mptr = mallocList.list;
	for(i=0;i<MAX_MALLOC_LIST;i++)
	{
		if(mptr->address == Ptr)
			break;
		mptr++;
	}
	
	// malloc not found
	if(i >= MAX_MALLOC_LIST)
		return;

    // Read cookie and adjust pointer back to original unaligned address before freeing.
    Diff = *((char *) Ptr - 1);
    Ptr = (void *) ((long) Ptr - Diff);

	syFree(Ptr);		
	mallocList.numEntries--;
	mallocList.totalMalloc -= mptr->size;
	mptr->address = NULL;
	mptr->size = -1;
}

/*	--------------------------------------------------------------------------------
	Function 	: memoryReset
	Purpose 	: Reset all mallocs
	Parameters 	: 
	Returns 	: 
	Info 		:
*/

void memoryReset()
{
	int			i;
	MALLOC_TYPE	*mptr;
	void 		*Ptr;
   	char 		Diff;
	
	mptr = mallocList.list;
	for(i=0;i<MAX_MALLOC_LIST;i++)
	{
		if(mptr->address)
		{
			Ptr = mptr->address;
		    Diff = *((char *) Ptr - 1);
		    Ptr = (void *) ((long) Ptr - Diff);
			syFree(Ptr);				
			mptr->address = NULL;
			mallocList.totalMalloc -= mptr->size;
			mptr->size = -1;
			mallocList.numEntries--;
		}
		mptr++;
	}
}

/*	--------------------------------------------------------------------------------
	Function 	: Rectangle2Twiddled
	Purpose 	: 
	Parameters 	: 
	Returns 	: 
	Info 		:
*/

void Rectangle2Twiddled(char *Rectangle, char *Twiddled, int Dimension, int BitsPerPixel)
{
    int 	x, y;
    char 	*In = Rectangle, *Out;

    // Convert BitsPerPixel into a shift value.. 8-bit = shift by 0, 16-bit = shift by 1.
    BitsPerPixel = BitsPerPixel >> 4;

    for(y=0;y<Dimension;y++)
    {
        for(x=0;x<Dimension;x++)
        {
            // TwiddledOffset macro converts coordinate into twiddled texture offset.
            Out = Twiddled + ((TwiddledOffset(x, y)) << BitsPerPixel);

            // Transfer pixel from Rectangle texture to Twiddled.
            *Out = *In;
            if(BitsPerPixel > 0)           // Account for 16-bit data.
                *(Out + 1) = *(In + 1);
            In += BitsPerPixel + 1;
        }
    }
}

// *ASL* 13/08/2000
/* --------------------------------------------------------------------------------
   Function : LoadTextureFile
   Purpose  : load a texture from the gdrom
   Parameters : texture filename
   Returns : texture map pointer
   Info :
*/

PKMDWORD LoadTextureFile2(char *Filename);

PKMDWORD LoadTextureFile(char *filename)
{
	PKMDWORD	pt;

	pt = LoadTextureFile2(filename);
	if (pt == NULL)
	{
		if (globalAbortFlag == 1)
			resetToBootROM();
	}
	return pt;
}
PKMDWORD LoadTextureFile2(char *Filename)
{
    PKMDWORD    TexturePtr;
    GDFS        gdfs;
    long        FileBlocks;

    // Open input file.
    if(!(gdfs = gdFsOpen(Filename, NULL)))
        return NULL;

    // Get file size (in blocks/sectors).
	FileBlocks = 0;
    gdFsGetFileSctSize(gdfs, &FileBlocks);

	// trap zero length
	if (FileBlocks == 0)
		return NULL;

    // Allocate memory to nearest block size (2048 bytes).
    TexturePtr = Align32Malloc(FileBlocks * 2048);

    // Read file to memory region (Destination address must be 32-byte aligned).
    gdFsReqRd32(gdfs, FileBlocks, TexturePtr);

    // Wait for file access to finish.
    while(gdFsGetStat(gdfs) != GDD_STAT_COMPLETE)
	{
		// *ASL* 13/08/2000 - Reset to BootROM on lid being opened by the user
		if (globalAbortFlag == 1)
			resetToBootROM();
	}

    // Close file.
    gdFsClose(gdfs);

    return TexturePtr;
}

/*
#define KM_TEXTURE_TWIDDLED	            (0x0100)*
#define KM_TEXTURE_TWIDDLED_MM	        (0x0200)
#define KM_TEXTURE_VQ	                (0x0300)-
#define KM_TEXTURE_VQ_MM	            (0x0400)
#define KM_TEXTURE_PALETTIZE4	        (0x0500)
#define KM_TEXTURE_PALETTIZE4_MM	    (0x0600)
#define KM_TEXTURE_PALETTIZE8	        (0x0700)
#define KM_TEXTURE_PALETTIZE8_MM	    (0x0800)
#define KM_TEXTURE_RECTANGLE	        (0x0900)*
#define KM_TEXTURE_RECTANGLE_MM	        (0x0A00)	// Reserved : Can't use
#define KM_TEXTURE_STRIDE	            (0x0B00)
#define KM_TEXTURE_STRIDE_MM	        (0x0C00)	// Reserved : Can't use
#define KM_TEXTURE_TWIDDLED_RECTANGLE   (0x0D00)
#define KM_TEXTURE_BMP                  (0x0E00)	// Converted to Twiddled
#define KM_TEXTURE_BMP_MM               (0x0F00)	// Converted to Twiddled MM
#define KM_TEXTURE_SMALLVQ              (0x1000)
#define KM_TEXTURE_SMALLVQ_MM           (0x1100)

#define KM_TEXTURE_ARGB1555             (0x00)
#define KM_TEXTURE_RGB565               (0x01)
#define KM_TEXTURE_ARGB4444             (0x02)
#define KM_TEXTURE_YUV422               (0x03)
#define KM_TEXTURE_BUMP                 (0x04)
#define KM_TEXTURE_RGB555               (0x05) 	// for PCX compatible only.	
#define KM_TEXTURE_YUV420               (0x06)

*/

int loadPVRFile(char *Filename, int flags)
{
	int			i,x,y;
	char		buf[256],*headerPtr,*ptr;
    PKMDWORD    TexturePtr,TwiddledPtr;
  	USHORT		width,height,*pixelPtr,pixel,newPixel;
  	int			headerSize,colFormat,texFormat,texAttrib,attrib;
  	UCHAR		r,g,b,a;

	// change directory  
	gdFsChangeDir("\\");
	gdFsChangeDir(textureDir);

	// load texture file
    TexturePtr = LoadTextureFile(Filename);
    if(TexturePtr == NULL)
    	return FALSE;

  	// copy name and create crc
	sprintf(buf,"%s",Filename);
	ptr = buf;
	for(i=0;i<strlen(Filename);i++)
	{
		if(*ptr == '.')
		{
			*ptr = 0;
			break;
		}
		ptr++;
	}
	utilUpperStr(buf);		
	DCKtextureList[DCKnumTextures].imageCRC = UpdateCRC(buf);
	sprintf(DCKtextureList[DCKnumTextures].ident,"%s",buf);
    
	// determine type of pvr file
	headerPtr = (char*)TexturePtr;
	if((*headerPtr == 'G')&&(*(headerPtr+1) == 'B')&&(*(headerPtr+2) == 'I')&&(*(headerPtr+3) == 'X'))
	{
		headerSize = 32;
		headerPtr += (6*4);
	}
	else if((*headerPtr == 'P')&&(*(headerPtr+1) == 'V')&&(*(headerPtr+2) == 'R')&&(*(headerPtr+3) == 'T'))
	{
		headerSize = 16;	
		headerPtr += (2*4);
	}

    // get the format and dimensions of the texture
	texAttrib = getInt(headerPtr);
	colFormat = (texAttrib & 0xf);
	texFormat = (texAttrib & 0xf00);
	headerPtr += 4;	
	width = getShort(headerPtr);
	headerPtr += 2;
	height = getShort(headerPtr);		
	
	// process the texture
	switch(texFormat)
	{
		case KM_TEXTURE_VQ:
		case KM_TEXTURE_TWIDDLED:
			headerPtr = ((char*)TexturePtr) + headerSize;
			kmCreateTextureSurface(&DCKtextureList[DCKnumTextures].surface, width, height,texAttrib);
			kmLoadTexture(&DCKtextureList[DCKnumTextures].surface, (PKMDWORD)headerPtr);
			break;
			
		case KM_TEXTURE_RECTANGLE:
			if(flags)
			{
				texAttrib = ((texAttrib & 0xfffff0ff) | KM_TEXTURE_TWIDDLED);
				kmCreateTextureSurface(&DCKtextureList[DCKnumTextures].surface, width, height,KM_TEXTURE_ARGB1555 | KM_TEXTURE_TWIDDLED);
				TwiddledPtr = (PKMDWORD) Align32Malloc(width * height * 2);
				pixelPtr = (USHORT*) ((long) TexturePtr + headerSize);
				for(i=0;i<(width*height);i++)
				{
					pixel = *pixelPtr;
					b = (pixel & 0xf800)>>11;
					g = (pixel & 0x07e0)>>5;
					r = (pixel & 0x001f);

					if((r == 31)&&(g == 0)&&(b == 31))
						newPixel = RGBA1555(r,g,b,0);
					else
						newPixel = RGBA1555(r,g,b,1);
						
					*pixelPtr++ = newPixel;
				}
				Rectangle2Twiddled((char *) ((long) TexturePtr + headerSize), (char *) TwiddledPtr, width, 16);
				kmLoadTexture(&DCKtextureList[DCKnumTextures].surface, TwiddledPtr);	
				Align32Free(TwiddledPtr);		
			}
			else
			{
				texAttrib = ((texAttrib & 0xfffff0ff) | KM_TEXTURE_TWIDDLED);
				kmCreateTextureSurface(&DCKtextureList[DCKnumTextures].surface, width, height,texAttrib);
				TwiddledPtr = (PKMDWORD) Align32Malloc(width * height * 2);
				Rectangle2Twiddled((char *) ((long) TexturePtr + headerSize), (char *) TwiddledPtr, width, 16);
				kmLoadTexture(&DCKtextureList[DCKnumTextures].surface, TwiddledPtr);	
				Align32Free(TwiddledPtr);
			}
			break;
			
		default:
			Align32Free(TexturePtr);
			return FALSE;	
			break;
	}
	Align32Free(TexturePtr);

    DCKnumTextures++;
    
    return TRUE;
}

int loadPVRFileIntoSurface(char *Filename, char *directory, KMSURFACEDESC *surface, int flags)
{
	int			i,x,y;
	char		buf[256],*headerPtr,*ptr;
    PKMDWORD    TexturePtr,TwiddledPtr;
  	USHORT		width,height,*pixelPtr,pixel,newPixel;
  	int			headerSize,colFormat,texFormat,texAttrib,attrib;
  	UCHAR		r,g,b,a;

	// change directory
	if(directory)  
	{
		gdFsChangeDir("\\");
		gdFsChangeDir(directory);
	}

	// load texture file
    TexturePtr = LoadTextureFile(Filename);
    if(TexturePtr == NULL)
    	return FALSE;
   
	// determine type of pvr file
	headerPtr = (char*)TexturePtr;
	if((*headerPtr == 'G')&&(*(headerPtr+1) == 'B')&&(*(headerPtr+2) == 'I')&&(*(headerPtr+3) == 'X'))
	{
		headerSize = 32;
		headerPtr += (6*4);
	}
	else if((*headerPtr == 'P')&&(*(headerPtr+1) == 'V')&&(*(headerPtr+2) == 'R')&&(*(headerPtr+3) == 'T'))
	{
		headerSize = 16;	
		headerPtr += (2*4);
	}

    // get the format and dimensions of the texture
	texAttrib = getInt(headerPtr);
	colFormat = (texAttrib & 0xf);
	texFormat = (texAttrib & 0xf00);
	headerPtr += 4;	
	width = getShort(headerPtr);
	headerPtr += 2;
	height = getShort(headerPtr);		
	
	// process the texture
	switch(texFormat)
	{
		case KM_TEXTURE_VQ:
		case KM_TEXTURE_TWIDDLED:
		case KM_TEXTURE_TWIDDLED_RECTANGLE:
			headerPtr = ((char*)TexturePtr) + headerSize;
			
			if(flags)
			{
				pixelPtr = (USHORT*) ((long) TexturePtr + headerSize);
				for(i=0;i<(width*height);i++)
				{
					pixel = *pixelPtr;
					r = (pixel & 0x7C00)>>10;
					g = (pixel & 0x03e0)>>5;
					b = (pixel & 0x001f);

					if((r == 31)&&(g == 0)&&(b == 31))
						newPixel = RGBA1555(r,g,b,0);
					else
						newPixel = RGBA1555(r,g,b,1);
						
					*pixelPtr++ = newPixel;
				}
			}
						
			kmCreateTextureSurface(surface, width, height,texAttrib);
			kmLoadTexture(surface, (PKMDWORD)headerPtr);
			break;
			
		case KM_TEXTURE_RECTANGLE:
			if(flags)
			{
				texAttrib = ((texAttrib & 0xfffff0ff) | KM_TEXTURE_TWIDDLED);
				kmCreateTextureSurface(surface, width, height,KM_TEXTURE_ARGB1555 | KM_TEXTURE_TWIDDLED);
				TwiddledPtr = (PKMDWORD) Align32Malloc(width * height * 2);
				pixelPtr = (USHORT*) ((long) TexturePtr + headerSize);
				for(i=0;i<(width*height);i++)
				{
					pixel = *pixelPtr;
					r = (pixel & 0xf800)>>11;
					g = (pixel & 0x07e0)>>5;
					b = (pixel & 0x001f);

					if((r == 31)&&(g == 0)&&(b == 31))
						newPixel = RGBA1555(r,g,b,0);
					else
						newPixel = RGBA1555(r,g,b,1);
						
					*pixelPtr++ = newPixel;
				}
				Rectangle2Twiddled((char *) ((long) TexturePtr + headerSize), (char *) TwiddledPtr, width, 16);
				kmLoadTexture(surface, TwiddledPtr);	
				Align32Free(TwiddledPtr);		
			}
			else
			{
				texAttrib = ((texAttrib & 0xfffff0ff) | KM_TEXTURE_TWIDDLED);
				kmCreateTextureSurface(surface, width, height,texAttrib);
				TwiddledPtr = (PKMDWORD) Align32Malloc(width * height * 2);
				Rectangle2Twiddled((char *) ((long) TexturePtr + headerSize), (char *) TwiddledPtr, width, 16);
				kmLoadTexture(surface, TwiddledPtr);	
				Align32Free(TwiddledPtr);
			}
			break;
			
		default:
			Align32Free(TexturePtr);
			return FALSE;	
			break;
	}
	Align32Free(TexturePtr);

    return TRUE;
}

int loadBMPFileIntoSurface(char *Filename, char *directory, KMSURFACEDESC *surface, int flags)
{
	int				i,j,size;
	char			buf[256],*headerPtr,*dataPtr,*palPtr,*pptr,index;
    PKMDWORD   		TexturePtr,TwiddledPtr;
  	USHORT			width,height,bpp,*pixelPtr,*newPixelPtr;
  	int				headerSize,colFormat,texFormat,texAttrib,attrib;
  	UCHAR			r,g,b,a;
  	BMPHeaderType	*bmpHeader;
	USHORT			*tempBitmap;

	// change directory
	if(directory)  
	{
		gdFsChangeDir("\\");
		gdFsChangeDir(directory);
	}

	// load texture file
    TexturePtr = LoadTextureFile(Filename);
    if(TexturePtr == NULL)
    	return FALSE;

	dataPtr = (char*)TexturePtr;
	if((*dataPtr != 'B')||(*(dataPtr+1) != 'M'))
    	return FALSE;
	
	bmpHeader = (BMPHeaderType*)(dataPtr+2);
	dataPtr += getInt((char*)&bmpHeader->headersize);

	width = getShort((char*)&bmpHeader->width);
	height = getShort((char*)&bmpHeader->depth);		
	bpp = getShort((char*)&bmpHeader->bits);		

	tempBitmap = Align32Malloc(width*height*sizeof(USHORT));
	
	switch(bpp)
	{
		case 4:
			pixelPtr = (tempBitmap + ((width*(height-1))) );
			palPtr = (char*)bmpHeader+(sizeof(BMPHeaderType));			
			for(i=0;i<height;i++)
			{
				for(j=0;j<(width/2);j++)
				{
					index = *dataPtr++;
					
					pptr = palPtr+((index>>4)*4);
					b = (*pptr++) >> 3;
					g = (*pptr++) >> 2;
					r = (*pptr++) >> 3;
					*pixelPtr++ = RGB565(r,g,b);		
					
					pptr = palPtr+((index&0x0F)*4);
					b = (*pptr++) >> 3;
					g = (*pptr++) >> 2;
					r = (*pptr++) >> 3;
					*pixelPtr++ = RGB565(r,g,b);		
				}
				pixelPtr -= (width*2);
			}
			break;
			
		case 8:
			pixelPtr = (tempBitmap + ((width*(height-1))) );
			palPtr = (char*)bmpHeader+(sizeof(BMPHeaderType));			
			for(i=0;i<height;i++)
			{
				for(j=0;j<width;j++)
				{
					index = *dataPtr++;
					pptr = palPtr+(index*4);
					b = (*pptr++) >> 3;
					g = (*pptr++) >> 2;
					r = (*pptr++) >> 3;
					*pixelPtr++ = RGB565(r,g,b);		
				}
				pixelPtr -= (width*2);
			}
			break;
			
		case 24:
			pixelPtr = (tempBitmap + ((width*(height-1))) );
			for(i=0;i<height;i++)
			{
				for(j=0;j<width;j++)
				{
					b = (*dataPtr++) >> 3;
					g = (*dataPtr++) >> 2;
					r = (*dataPtr++) >> 3;
					*pixelPtr++ = RGB565(r,g,b);		
				}
				pixelPtr -= (width*2);
			}
			break;
	}
	
	kmCreateTextureSurface(surface, width, height,KM_TEXTURE_RGB565|KM_TEXTURE_STRIDE);
	kmLoadTexture(surface, (PKMDWORD)tempBitmap);

	syFree(tempBitmap);

	Align32Free(TexturePtr);

    return TRUE;
}

/*
typedef struct tagNSPRITE {
	unsigned char	*image;		// image data
	unsigned short	*pal;		// palette data
	unsigned char	w;			// width
	unsigned char	h;			// height
	short			u;    
	short			v;     
	unsigned short	flags;
	unsigned long	crc;
} NSPRITE

#define NLASTSPR	1 
#define NEIGHTBIT	2
#define NSPLIT		4
#define NBITSTREAM	8
#define NALPHA		16
#define NCOLOURKEY	32

*/

int loadNSPRITEIntoSurface(NSPRITE *sprite)
{
	int				i,j,size;
	unsigned char	buf[256],*headerPtr,*dataPtr,*pptr;
    PKMDWORD   		TexturePtr,TwiddledPtr;
  	USHORT			width,height,bpp,alpha,*pixelPtr,*newPixelPtr,palCol,*palPtr,index,*dPtr,pixel;
  	int				headerSize,colFormat,texFormat,texAttrib,attrib;
  	UCHAR			r,g,b,a;
  	BMPHeaderType	*bmpHeader;
	int				newWidth,newHeight;
	int				twiddled = TRUE;
	KMSTATUS 		tflag;
	TextureType		*texPtr;
	
	dataPtr = (sprite->image)+4;
	palPtr = sprite->pal;
	
	width = sprite->w;
	height = sprite->h;
	
	bpp = 4;
	if(sprite->flags & NEIGHTBIT)
		bpp = 8;

	alpha = FALSE;
	if(sprite->flags & NALPHA)
		alpha = TRUE;
	
	// check to see if the texture is a valid size
	newWidth = width;
	newHeight = height;
	if((width != 8)&&(width != 16)&&(width != 32)&&(width != 64)&&(width != 128)&&(width != 256)&&(width != 512)&&(width != 1024))
	{
		if(width < 8)
			newWidth = 8;
		else if(width < 16)
			newWidth = 16;
		else if(width < 32)
			newWidth = 32;
		else if(width < 64)
			newWidth = 64;
		else if(width < 128)
			newWidth = 128;
		else if(width < 256)
			newWidth = 256;
		else if(width < 512)
			newWidth = 512;
		else if(width < 1024)
			newWidth = 1024;
	}
	if((height != 8)&&(height != 16)&&(height != 32)&&(height != 64)&&(height != 128)&&(height != 256)&&(height != 512)&&(height != 1024))
	{
		if(height < 8)
			newHeight = 8;
		else if(height < 16)
			newHeight = 16;
		else if(height < 32)
			newHeight = 32;
		else if(height < 64)
			newHeight = 64;
		else if(height < 128)
			newHeight = 128; 
		else if(height < 256)
			newHeight = 256;
		else if(height < 512)
			newHeight = 512;
		else if(height < 1024)
			newHeight = 1024;
	}
	if(newWidth < newHeight)
	{
		newWidth = newHeight;
		twiddled = false;
	}
	else if(newHeight < newWidth)
	{
		newHeight = newWidth;
		twiddled = false;
	}


	texPtr = &DCKtextureList[DCKnumTextures];				
	if(sprite->flags & NALPHA)
	{
		switch(bpp)
		{
			case 4:
				pixelPtr = bitmapPtr;
				for(i=0;i<height;i++)
				{
					for(j=0;j<(width/2);j++)
					{
						index = *dataPtr++;

						palCol = *(palPtr+(index>>4));					
						r = ((palCol) & 0x1F)>>1;
						*pixelPtr++ = RGBA4444(15,15,15,r);

						palCol = *(palPtr+(index&0x0F));
						r = ((palCol) & 0x1F)>>1;
						*pixelPtr++ = RGBA4444(15,15,15,r);
					}
				}
				break;
			
			case 8:
				pixelPtr = bitmapPtr;
				for(i=0;i<height;i++)
				{
					for(j=0;j<width;j++)
					{
						index = *dataPtr++;
						
						palCol = *(palPtr+index);					
						a = ((palCol) & 0x1F)>>1;
						b = (palCol >> 11);
						g = (palCol >> 6) & 0x0F;
						r = (palCol >> 1) & 0x0F;
						*pixelPtr++ = RGBA4444(r,g,b,a);
					}
				}
				break;		
		}
		texPtr->colourKey = TRUE;			
		Rectangle2Twiddled((char*)bitmapPtr,(char*)twiddlePtr, width, 16);
		tflag = kmCreateTextureSurface(&texPtr->surface, newWidth, newHeight,KM_TEXTURE_ARGB4444|KM_TEXTURE_TWIDDLED);
		if(tflag != KMSTATUS_SUCCESS)
			return FALSE;
		kmLoadTexture(&texPtr->surface, (PKMDWORD)twiddlePtr);	
	}
	else if(sprite->flags & NCOLOURKEY)
	{
		texPtr->colourKey = TRUE;			
		tflag = kmCreateTextureSurface(&texPtr->surface, newWidth, newHeight,KM_TEXTURE_ARGB1555|KM_TEXTURE_TWIDDLED);
		if(tflag != KMSTATUS_SUCCESS)
			return FALSE;
		memcpy((char*)bitmapPtr,(char*)dataPtr,(width*height*2));
		kmLoadTexture(&texPtr->surface, (PKMDWORD)bitmapPtr);
//		kmLoadTexture(&texPtr->surface, (PKMDWORD)twiddlePtr);
	}
	else
	{
		texPtr->colourKey = FALSE;
		tflag = kmCreateTextureSurface(&texPtr->surface, newWidth, newHeight,KM_TEXTURE_RGB565|KM_TEXTURE_TWIDDLED);
		if(tflag != KMSTATUS_SUCCESS)
			return FALSE;
		memcpy((char*)bitmapPtr,(char*)dataPtr,(width*height*2));
		kmLoadTexture(&texPtr->surface, (PKMDWORD)bitmapPtr);
//		kmLoadTexture(&texPtr->surface, (PKMDWORD)twiddlePtr);
	}	

	texPtr->surfacePtr = &texPtr->surface;
	texPtr->imageCRC = sprite->crc;
	texPtr->bpp = bpp;	
	texPtr->w = newWidth;	
	texPtr->h = newHeight;	
	texPtr->animated = FALSE;

	// pre generate strip
	if(texPtr->colourKey)
	{
		memcpy(&texPtr->stripHead,&StripHead_GT4_FMA_Alpha, sizeof(StripHead_GT4_FMA_Alpha));
		kmChangeStripTextureSurface(&texPtr->stripHead, KM_IMAGE_PARAM1, &texPtr->surface);
	}
	else
	{
		memcpy(&texPtr->stripHead,&StripHead_GT4_FMA,sizeof(StripHead_GT4_FMA));
		kmChangeStripTextureSurface(&texPtr->stripHead, KM_IMAGE_PARAM1, &texPtr->surface);
	}

	DCKnumTextures++;
	
    return TRUE;
}

int loadBigNSPRITEIntoSurface(TextureBankType *bankPtr, int swidth, int sheight, int snumber, int num)
{
	int				i,j,size;
	char			buf[256],*headerPtr,*dataPtr,*pptr;
    PKMDWORD   		TexturePtr,TwiddledPtr;
  	USHORT			width,height,bpp,*pixelPtr,*newPixelPtr,palCol,*palPtr,index;
  	int				headerSize,colFormat,texFormat,texAttrib,attrib;
  	UCHAR			r,g,b,a;
  	BMPHeaderType	*bmpHeader;
	USHORT			*tempBitmap,*tempTwiddle;
	int				newWidth,newHeight,loop;
	int				twiddled = TRUE;
	NSPRITE 		*sprite = &bankPtr->pNSprite[num];
	TextureType		*texPtr;

	bpp = 4;
	if(sprite->flags & NEIGHTBIT)
		bpp = 8;
	
	// check to see if the texture is a valid size
	newWidth = swidth;
	newHeight = sheight;
	if((swidth != 8)&&(swidth != 16)&&(swidth != 32)&&(swidth != 64)&&(swidth != 128)&&(swidth != 256)&&(swidth != 512)&&(swidth != 1024))
	{
		if(swidth < 8)
			newWidth = 8;
		else if(swidth < 16)
			newWidth = 16;
		else if(swidth < 32)
			newWidth = 32;
		else if(swidth < 64)
			newWidth = 64;
		else if(swidth < 128)
			newWidth = 128;
		else if(swidth < 256)
			newWidth = 256;
		else if(swidth < 512)
			newWidth = 512;
		else if(swidth < 1024)
			newWidth = 1024;
	}
	if((sheight != 8)&&(sheight != 16)&&(sheight != 32)&&(sheight != 64)&&(sheight != 128)&&(sheight != 256)&&(sheight != 512)&&(sheight != 1024))
	{
		if(sheight < 8)
			newHeight = 8;
		else if(sheight < 16)
			newHeight = 16;
		else if(sheight < 32)
			newHeight = 32;
		else if(sheight < 64)
			newHeight = 64;
		else if(sheight < 128)
			newHeight = 128;
		else if(sheight < 256)
			newHeight = 256;
		else if(sheight < 512)
			newHeight = 512;
		else if(sheight < 1024)
			newHeight = 1024;
	}
	if(newWidth < newHeight)
	{
		newWidth = newHeight;
		twiddled = false;
	}
	else if(newHeight < newWidth)
	{
		newHeight = newWidth;
		twiddled = false;
	}

	tempBitmap = Align32Malloc(newWidth*newHeight*sizeof(USHORT));
	tempTwiddle = Align32Malloc(newWidth*newHeight*sizeof(USHORT));

	texPtr = &DCKtextureList[DCKnumTextures];			

	loop=0;
	for(loop=0;loop<snumber;loop++)	
	{		
		sprite = &bankPtr->pNSprite[num+loop];
		// *ASL* 20/07/2000 Ambig - Cast to char pointer
		dataPtr = (char *)(sprite->image)+4;
		palPtr = sprite->pal;		
		width = sprite->w;
		height = sprite->h;
	
		if(sprite->flags & NCOLOURKEY)
		{
			switch(bpp)
			{
				case 4:
					pixelPtr = tempBitmap + (loop*width);
					for(i=0;i<height;i++)
					{
						for(j=0;j<(width/2);j++)
						{
							index = *dataPtr++;

							palCol = *(palPtr+(index>>4));					
							b = (palCol >> 10);
							g = (palCol >> 4) & 0x3E;
							r = (palCol) & 0x1F;
							if((r == 31)&&(g == 0)&&(b == 31))					
								*pixelPtr++ = RGBA1555(r,g,b,0);
							else
								*pixelPtr++ = RGBA1555(r,g,b,1);	

							palCol = *(palPtr+(index&0x0F));
							b = (palCol >> 10);
							g = (palCol >> 4) & 0x3E;
							r = (palCol) & 0x1F;
							if((r == 31)&&(g == 0)&&(b == 31))					
								*pixelPtr++ = RGBA1555(r,g,b,0);
							else
								*pixelPtr++ = RGBA1555(r,g,b,1);
						}
						pixelPtr += width;
					}
					break;
			
				case 8:
					pixelPtr = tempBitmap + (loop*width);
					for(i=0;i<height;i++)
					{
						for(j=0;j<width;j++)
						{
							index = *dataPtr++;

							palCol = *(palPtr+index);					
							b = (palCol >> 10);
							g = (palCol >> 5) & 0x1F;
							r = (palCol) & 0x1F;
							if((r == 31)&&(g == 0)&&(b == 31))
								*pixelPtr++ = RGBA1555(r,g,b,0);
							else
								*pixelPtr++ = RGBA1555(r,g,b,1);					
						}
						pixelPtr += width;
					}
					break;		
			}
		}
		else
		{
			switch(bpp)
			{
				case 4:
					pixelPtr = tempBitmap + (loop*width);
					for(i=0;i<height;i++)
					{
						for(j=0;j<(width/2);j++)
						{
							index = *dataPtr++;

							palCol = *(palPtr+(index>>4));					
							b = (palCol >> 10);
							g = (palCol >> 4) & 0x3E;
							r = (palCol) & 0x1F;
							*pixelPtr++ = RGB565(r,g,b);

							palCol = *(palPtr+(index&0x0F));
							b = (palCol >> 10);
							g = (palCol >> 4) & 0x3E;
							r = (palCol) & 0x1F;
							*pixelPtr++ = RGB565(r,g,b);
						}
						pixelPtr += width;
					}
					break;
			
				case 8:
					pixelPtr = tempBitmap + (loop*width);
					for(i=0;i<height;i++)
					{
						for(j=0;j<width;j++)
						{
							index = *dataPtr++;
						
							palCol = *(palPtr+index);					
							b = (palCol >> 10);
							g = (palCol >> 4) & 0x3E;
							r = (palCol) & 0x1F;
							*pixelPtr++ = RGB565(r,g,b);
						}
						pixelPtr += width;
					}
					break;		
			}
		}	
	}

	// set sprite details
	sprite = &bankPtr->pNSprite[num];

	Rectangle2Twiddled((char*)tempBitmap,(char*)tempTwiddle, newWidth, 16);
	if(sprite->flags & NCOLOURKEY)
	{
		texPtr->colourKey = TRUE;
		kmCreateTextureSurface(&texPtr->surface, newWidth, newHeight,KM_TEXTURE_ARGB1555|KM_TEXTURE_TWIDDLED);
	}
	else
	{
		texPtr->colourKey = FALSE;
		kmCreateTextureSurface(&texPtr->surface, newWidth, newHeight,KM_TEXTURE_RGB565|KM_TEXTURE_TWIDDLED);
	}
	kmLoadTexture(&texPtr->surface, (PKMDWORD)tempTwiddle);

	texPtr->imageCRC = sprite->crc;
	texPtr->bpp = bpp;	
	texPtr->w = newWidth;	
	texPtr->h = newHeight;	

	Align32Free(tempBitmap);
	Align32Free(tempTwiddle);

	DCKnumTextures++;
	
    return TRUE;
}