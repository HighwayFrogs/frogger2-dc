/*
/--------------------------------------------------------------------
|
|      $Id: FilterGetAlpha.h,v 1.3 1999/12/08 15:39:46 Ulrich von Zadow Exp $
|
|      Copyright (c) 1996-1998 Ulrich von Zadow
|
\--------------------------------------------------------------------
*/

#if !defined(INCL_FILTERGETALPHA)
#define INCL_FILTERGETALPHA

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "Filter.h"

class CBmp;

//! Returns an 8 bpp grayscale bitmap containing only the alpha channel
//! of the source bitmap.
class CFilterGetAlpha : public CFilter
{
public:
  //!
	CFilterGetAlpha();
  //!
	virtual ~CFilterGetAlpha();

  //! 
  virtual void Apply(CBmp * pBmpSource, CBmp * pBmpDest);

private:
};

#endif

/*
/--------------------------------------------------------------------
|
|      $Log: FilterGetAlpha.h,v $
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
