/*
/--------------------------------------------------------------------
|
|      $Id: FilterCrop.h,v 1.3 1999/12/08 15:39:46 Ulrich von Zadow Exp $
|
|      Copyright (c) 1996-1998 Ulrich von Zadow
|
\--------------------------------------------------------------------
*/

#if !defined(INCL_FILTERCROP)
#define INCL_FILTERCROP

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "Filter.h"

class CBmp;

//! Filter that cuts part of the image off.
class CFilterCrop : public CFilter
{
public:

  //!
	CFilterCrop(int XMin, int XMax, int YMin, int YMax);

  //!
	virtual ~CFilterCrop();

  //! 
  virtual void Apply(CBmp * pBmpSource, CBmp * pBmpDest);

private:
  int m_XMin;
  int m_XMax;
  int m_YMin;
  int m_YMax;

};

#endif

/*
/--------------------------------------------------------------------
|
|      $Log: FilterCrop.h,v $
|      Revision 1.3  1999/12/08 15:39:46  Ulrich von Zadow
|      Unix compatibility changes
|
|      Revision 1.2  1999/11/27 18:45:48  Ulrich von Zadow
|      Added/Updated doc comments.
|
|      Revision 1.1  1999/10/21 16:05:17  Ulrich von Zadow
|      Moved filters to separate directory. Added Crop, Grayscale and
|      GetAlpha filters.
|
|      Revision 1.1  1999/10/19 21:29:44  Ulrich von Zadow
|      Added filters.
|
|
\--------------------------------------------------------------------
*/
