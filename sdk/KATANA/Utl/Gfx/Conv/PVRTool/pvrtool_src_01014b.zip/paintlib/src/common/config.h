/*
/--------------------------------------------------------------------
|
|      $Id: config.h,v 1.7 2000/01/16 20:43:13 anonymous Exp $
|
|      Copyright (c) 1996-1998 Ulrich von Zadow
|
--------------------------------------------------------------------
*/

//! Contains configuration information.

#ifndef INCL_CONFIG
#define INCL_CONFIG

#define SUPPORT_TGA
#define SUPPORT_BMP
#define SUPPORT_TIFF
#define SUPPORT_PICT
#define SUPPORT_JPEG
#define SUPPORT_PNG
#ifdef _WINDOWS
  #define SUPPORT_WEMF
#endif
#define SUPPORT_PCX

// The following lines can be used to define maximum memory useage
// for a single BITMAP. If we need more, we assume something went wrong.
// If MAX_BITMAP_SIZE isn't defined, the code doesn't check for huge
// bitmaps and just tries to allocate the memory. This can take a
// long time.
// #ifndef MAX_BITMAP_SIZE
// #define MAX_BITMAP_SIZE 8L*1024*1024
// #endif

#endif
/*
/--------------------------------------------------------------------
|
|      $Log: config.h,v $
|      Revision 1.7  2000/01/16 20:43:13  anonymous
|      Removed MFC dependencies
|
|      Revision 1.6  1999/12/08 15:39:45  Ulrich von Zadow
|      Unix compatibility changes
|
|      Revision 1.5  1999/10/21 17:43:08  Ulrich von Zadow
|      Added pcx support by Meng Bo.
|
|      Revision 1.4  1999/10/03 18:50:51  Ulrich von Zadow
|      Added automatic logging of changes.
|
|
--------------------------------------------------------------------
*/
