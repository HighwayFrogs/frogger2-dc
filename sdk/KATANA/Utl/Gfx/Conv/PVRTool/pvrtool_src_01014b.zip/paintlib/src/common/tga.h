/*
/--------------------------------------------------------------------
|
|      $Id: tga.h,v 1.3 1999/10/03 18:50:52 Ulrich von Zadow Exp $
|
|      Copyright (c) 1996-1998 Ulrich von Zadow
|
\--------------------------------------------------------------------
*/

//! Defines structures and constants present in TGA files.

#ifndef INCL_TGA
#define INCL_TGA

typedef struct _TgaHeader
{
    BYTE   IdLength;            // Image ID Field Length
    BYTE   CmapType;            // Color Map Type
    BYTE   ImageType;           // Image Type

    WORD   CmapIndex;           // First Entry Index
    WORD   CmapLength;          // Color Map Length
    BYTE   CmapEntrySize;       // Color Map Entry Size

    WORD   X_Origin;            // X-origin of Image
    WORD   Y_Origin;            // Y-origin of Image
    WORD   ImageWidth;          // Image Width
    WORD   ImageHeight;         // Image Height
    BYTE   PixelDepth;          // Pixel Depth
    BYTE   ImagDesc;            // Image Descriptor
} TGAHEADER;

// Definitions for image types.
#define TGA_Null 0
#define TGA_Map 1
#define TGA_RGB 2
#define TGA_Mono 3
#define TGA_RLEMap 9
#define TGA_RLERGB 10
#define TGA_RLEMono 11
#define TGA_CompMap 32
#define TGA_CompMap4 33


#endif
/*
/--------------------------------------------------------------------
|
|      $Log: tga.h,v $
|      Revision 1.3  1999/10/03 18:50:52  Ulrich von Zadow
|      Added automatic logging of changes.
|
|
--------------------------------------------------------------------
*/
