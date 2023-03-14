/*
/--------------------------------------------------------------------
|
|      $Id: tif_msrc.h,v 1.4 2000/01/10 23:52:59 Ulrich von Zadow Exp $
|
|      Copyright (c) 1996-1998 Ulrich von Zadow
|
\--------------------------------------------------------------------
*/

//! Custom data source for libtiff. Assumes all data is in memory at
//! start.

extern TIFF* TIFFOpenMem (unsigned char* pData,
                            int FileSize,
                            int* pCurPos);


typedef struct
{
  unsigned char* pData;
  int    CurPos;
  int    MaxFileSize;
  // used when open for reading
  int    rFileSize;
  // not NULL when open for writing
  int*   pFileSize;
  char   mode[2];
}
MemSrcTIFFInfo;
/*
/--------------------------------------------------------------------
|
|      $Log: tif_msrc.h,v $
|      Revision 1.4  2000/01/10 23:52:59  Ulrich von Zadow
|      Changed formatting & removed tabs.
|
|      Revision 1.3  1999/10/03 18:50:52  Ulrich von Zadow
|      Added automatic logging of changes.
|
|
--------------------------------------------------------------------
*/
