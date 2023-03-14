/*
/--------------------------------------------------------------------
|
|      $Id: qdraw.h,v 1.4 2000/01/10 23:52:59 Ulrich von Zadow Exp $
|
|      Copyright (c) 1996-1998 Ulrich von Zadow
|
\--------------------------------------------------------------------
*/

//! This file defines the data structures used in pict files. They
//! correspond to the appropriate MAC QuickDraw structs. See
//! QuickDraw docs for an explanation of these structs.

#ifndef INCL_QDRAW
#define INCL_QDRAW

struct MacRect
{
  WORD top;
  WORD left;
  WORD bottom;
  WORD right;
};

struct MacpixMap
{
  // Ptr baseAddr              // Not used in file.
  // short rowBytes            // read in seperatly.
  struct MacRect Bounds;
  WORD version;
  WORD packType;
  LONG packSize;
  LONG hRes;
  LONG vRes;
  WORD pixelType;
  WORD pixelSize;
  WORD cmpCount;
  WORD cmpSize;
  LONG planeBytes;
  LONG pmTable;
  LONG pmReserved;
};

struct MacRGBColour
{
  WORD red;
  WORD green;
  WORD blue;
};

struct MacPoint
{
  WORD x;
  WORD y;
};

struct MacPattern // Klaube
{
  BYTE pix[64];
};

#endif
/*
/--------------------------------------------------------------------
|
|      $Log: qdraw.h,v $
|      Revision 1.4  2000/01/10 23:52:59  Ulrich von Zadow
|      Changed formatting & removed tabs.
|
|      Revision 1.3  1999/10/03 18:50:51  Ulrich von Zadow
|      Added automatic logging of changes.
|
|
--------------------------------------------------------------------
*/
