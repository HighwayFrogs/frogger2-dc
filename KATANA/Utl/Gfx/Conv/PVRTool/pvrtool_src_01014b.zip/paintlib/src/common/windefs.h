/*
/--------------------------------------------------------------------
|
|      $Id: windefs.h,v 1.3 1999/10/03 18:50:52 Ulrich von Zadow Exp $
|
|      Copyright (c) 1996-1998 Ulrich von Zadow
|
\--------------------------------------------------------------------
*/

//! Windows Bitmap Struct Definitions
//!
//! Contains the definitions for the data structures used in
//! windows bitmap files. For windows apps, they are already
//! defined - but not with these names.
//!
//! The WINRGBQUAD, WINBITMAPFILEHEADER, WINBITMAPINFOHEADER,
//! and WINBITMAPINFO structs are defined as RGBQUAD etc. in
//! the windows header files. See the windows documentation
//! for details.

//  Copyright (c) 1996-1998 Ulrich von Zadow

#ifndef INCL_WINDEFS
#define INCL_WINDEFS

typedef struct tagWINRGBQUAD
{
  BYTE    rgbBlue;
  BYTE    rgbGreen;
  BYTE    rgbRed;
  BYTE    rgbReserved;
} WINRGBQUAD;

typedef struct tagWINBITMAPFILEHEADER
{
  WORD    bfType;
  LONG    bfSize;
  WORD    bfReserved1;
  WORD    bfReserved2;
  LONG    bfOffBits;
} WINBITMAPFILEHEADER;

typedef struct tagWINBITMAPINFOHEADER
{
  LONG   biSize;
  LONG   biWidth;
  LONG   biHeight;
  WORD   biPlanes;
  WORD   biBitCount;
  LONG   biCompression;
  LONG   biSizeImage;
  LONG   biXPelsPerMeter;
  LONG   biYPelsPerMeter;
  LONG   biClrUsed;
  LONG   biClrImportant;
} WINBITMAPINFOHEADER;

typedef struct tagWINBITMAPINFO
{
  WINBITMAPINFOHEADER bmiHeader;
  WINRGBQUAD          bmiColors[1];
} WINBITMAPINFO;

/* constants for the biCompression field */
#define BI_RGB        0L
#define BI_RLE8       1L
#define BI_RLE4       2L
#define BI_BITFIELDS  3L

#endif
/*
/--------------------------------------------------------------------
|
|      $Log: windefs.h,v $
|      Revision 1.3  1999/10/03 18:50:52  Ulrich von Zadow
|      Added automatic logging of changes.
|
|
--------------------------------------------------------------------
*/
