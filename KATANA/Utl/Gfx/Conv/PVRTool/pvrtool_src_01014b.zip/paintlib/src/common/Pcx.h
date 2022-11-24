/*
/--------------------------------------------------------------------
|
|      $Id: pcx.h,v 1.4 2000/01/10 23:52:59 Ulrich von Zadow Exp $
|
|      Defines structures and constants present in PCX files.
|
|      Copyright (c) 1996-1998 Ulrich von Zadow
|
\--------------------------------------------------------------------
*/

#ifndef INCL_PCX
#define INCL_PCX

// PCX Image File
typedef struct tagPCXHEADER
{
  char Manufacturer;  // always 0X0A
  char Version;  // version number
  char Encoding;  // always 1
  char BitsPerPixel;  // color bits
  WORD Xmin, Ymin;  // image origin
  WORD Xmax, Ymax;  // image dimensions
  WORD Hres, Vres;  // resolution values
  BYTE ColorMap[16][3];  // color palette
  char Reserved;
  char ColorPlanes;  // color planes
  WORD BytesPerLine;  // line buffer size
  WORD PaletteType;  // grey or color palette
  char Filter[58];
}
*LPPCXHEADER;

#define PCX_MAGIC 0X0A  // PCX magic number
#define PCX_256_COLORS 0X0C  // magic number for 256 colors
#define PCX_HDR_SIZE 128  // size of PCX header
#define PCX_MAXCOLORS 256
#define PCX_MAXPLANES 4
#define PCX_MAXVAL 255

#endif

/*
/--------------------------------------------------------------------
|
|      $Log: pcx.h,v $
|      Revision 1.4  2000/01/10 23:52:59  Ulrich von Zadow
|      Changed formatting & removed tabs.
|
|      Revision 1.3  1999/12/15 21:16:30  Ulrich von Zadow
|      Removed references to gif in pcx decoder.
|
|
\--------------------------------------------------------------------
*/
