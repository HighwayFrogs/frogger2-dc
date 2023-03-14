/*
/--------------------------------------------------------------------
|
|      $Id: FilterResizeBox.h,v 1.2 1999/11/27 18:45:49 Ulrich von Zadow Exp $
|
|      Copyright (c) 1996-1998 Ulrich von Zadow
|
\--------------------------------------------------------------------
*/

#ifndef FILTERResizeBOX_H
#define FILTERResizeBOX_H

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "FilterResize.h"

//! Resizes a bitmap and applies a box filter to it.
class CFilterResizeBox : public CFilterResize
{
public:
  //! 
  CFilterResizeBox (int NewXSize, int NewYSize, double NewRadius);
  //! 
  virtual void Apply(CBmp * pBmpSource, CBmp * pBmpDest);

private: 
  double m_NewRadius;
};

#endif 

/*
/--------------------------------------------------------------------
|
|      $Log: FilterResizeBox.h,v $
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
