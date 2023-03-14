/*
/--------------------------------------------------------------------
|
|      $Id: FilterRotate.h,v 1.3 1999/12/08 16:31:41 Ulrich von Zadow Exp $
|
|      Copyright (c) 1996-1998 Ulrich von Zadow
|
\--------------------------------------------------------------------
*/

#ifndef FILTERROTATE_H
#define FILTERROTATE_H

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "Filter.h"

//! Rotates a bitmap by angle radians.
class CFilterRotate : public CFilter  
{
public:
  //! 
	CFilterRotate(double angle, RGBAPIXEL crDefault);
  //! 
	virtual ~CFilterRotate();
  //! 
  virtual void Apply(CBmp * pBmpSource, CBmp * pBmpDest);

protected:
  double m_angle;
  RGBAPIXEL m_crDefault;
};

#endif 

/*
/--------------------------------------------------------------------
|
|      $Log: FilterRotate.h,v $
|      Revision 1.3  1999/12/08 16:31:41  Ulrich von Zadow
|      Unix compatibility
|
|      Revision 1.2  1999/11/27 18:45:49  Ulrich von Zadow
|      Added/Updated doc comments.
|
|      Revision 1.1  1999/10/21 18:47:43  Ulrich von Zadow
|      no message
|
|      Revision 1.1  1999/10/21 16:05:18  Ulrich von Zadow
|      Moved filters to separate directory. Added Crop, Grayscale and
|      GetAlpha filters.
|
|      Revision 1.1  1999/10/19 21:29:44  Ulrich von Zadow
|      Added filters.
|
|
\--------------------------------------------------------------------
*/
