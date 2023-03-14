/*
/--------------------------------------------------------------------
|
|      $Id: FilterResize.h,v 1.2 1999/11/27 18:45:49 Ulrich von Zadow Exp $
|
|      Copyright (c) 1996-1998 Ulrich von Zadow
|
\--------------------------------------------------------------------
*/

#ifndef FILTERResize_H
#define FILTERResize_H

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "Filter.h"

//! Base class for filters that resize the image.
class CFilterResize : public CFilter  
{
public:
  //! 
	CFilterResize(int NewXSize, int NewYSize);
  //! 
	virtual ~CFilterResize();
  //! 
  void SetNewSize(int NewXSize, int NewYSize);

protected:
  int m_NewXSize;
  int m_NewYSize;
};

#endif 

/*
/--------------------------------------------------------------------
|
|      $Log: FilterResize.h,v $
|      Revision 1.2  1999/11/27 18:45:49  Ulrich von Zadow
|      Added/Updated doc comments.
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
