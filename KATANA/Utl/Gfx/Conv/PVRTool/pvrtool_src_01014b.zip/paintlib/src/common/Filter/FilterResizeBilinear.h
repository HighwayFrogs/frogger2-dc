/*
/--------------------------------------------------------------------
|
|      $Id: FilterResizeBilinear.h,v 1.2 1999/11/27 18:45:49 Ulrich von Zadow Exp $
|
|      Copyright (c) 1996-1998 Ulrich von Zadow
|
\--------------------------------------------------------------------
*/

#ifndef FILTERResizeBILINEAR_H
#define FILTERResizeBILINEAR_H

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "FilterResize.h"

//! Resizes a bitmap using bilinear interpolation.
class CFilterResizeBilinear : public CFilterResize
{
public:
  //! 
  CFilterResizeBilinear (int NewXSize, int NewYSize);
  //! 
  virtual void Apply(CBmp * pBmpSource, CBmp * pBmpDest);
};

#endif 

/*
/--------------------------------------------------------------------
|
|      $Log: FilterResizeBilinear.h,v $
|      Revision 1.2  1999/11/27 18:45:49  Ulrich von Zadow
|      Added/Updated doc comments.
|
|      Revision 1.1  1999/10/21 16:05:18  Ulrich von Zadow
|      Moved filters to separate directory. Added Crop, Grayscale and
|      GetAlpha filters.
|
|      Revision 1.1  1999/10/19 21:29:45  Ulrich von Zadow
|      Added filters.
|
|
\--------------------------------------------------------------------
*/
