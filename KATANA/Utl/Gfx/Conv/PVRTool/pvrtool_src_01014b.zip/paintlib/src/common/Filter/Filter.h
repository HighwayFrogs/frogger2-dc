/*
/--------------------------------------------------------------------
|
|      $Id: Filter.h,v 1.3 2000/01/16 20:43:15 anonymous Exp $
|
|      Copyright (c) 1996-1998 Ulrich von Zadow
|
\--------------------------------------------------------------------
*/

#if !defined(AFX_FILTER_H__3CED06E1_1B40_11D3_B142_444553540000__INCLUDED_)
#define AFX_FILTER_H__3CED06E1_1B40_11D3_B142_444553540000__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

class CBmp;

//! Base class for filters that operate on bitmaps. Derived classes need
//! to override either the ApplyInPlace or the Apply function.
class CFilter : public CPLObject  
{
public:
	//!
  CFilter();
  //!
	virtual ~CFilter();

  //! In-Place Apply. Applies the filter to pBmp. The base-class
  //! version copies the bitmap after calling Apply (pBmp, pTempBmp).
  virtual void ApplyInPlace(CBmp * pBmp);  

  //! Applies the Filter to pBmpSource and stores the result in
  //! pBmpDest. The base-class version copies the bitmap before calling
  //! ApplyInPlace (pBmpDest).
  virtual void Apply(CBmp * pBmpSource, CBmp * pBmpDest);
};

#endif // !defined(AFX_FILTER_H__3CED06E1_1B40_11D3_B142_444553540000__INCLUDED_)

/*
/--------------------------------------------------------------------
|
|      $Log: Filter.h,v $
|      Revision 1.3  2000/01/16 20:43:15  anonymous
|      Removed MFC dependencies
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
