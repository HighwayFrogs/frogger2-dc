/*
/--------------------------------------------------------------------
|
|      $id$
|
|      Copyright (c) 1996-1998 Ulrich von Zadow
|
--------------------------------------------------------------------
*/

#ifndef INCL_DECINFO
#define INCL_DECINFO

class CDataSrc;

class CDecodeInfo
{
public:
  CDataSource * m_pDataSrc;

  CBmp        * m_pBmp;         // Pointer to bitmap being created.
  RGBAPIXEL   * m_pPal;         // Pointer to palette used in file.
  int           m_DestBPP;
}
/*
/--------------------------------------------------------------------
|
|      $log$
|
--------------------------------------------------------------------
*/
