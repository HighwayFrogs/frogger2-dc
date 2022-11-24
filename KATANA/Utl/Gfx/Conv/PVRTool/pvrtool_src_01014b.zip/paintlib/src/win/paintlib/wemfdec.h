/*
/--------------------------------------------------------------------
|
|      $Id: wemfdec.h,v 1.4 2000/01/16 20:43:18 anonymous Exp $
|
|      Copyright (c) 1996-1998 Ulrich von Zadow
|
--------------------------------------------------------------------
*/

#ifndef INCL_WEMFDEC_H
#define INCL_WEMFDEC_H

#ifndef INCL_PICDEC
#include "picdec.h"
#endif

// This only makes sense for Windows
#ifdef _WINDOWS

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


//! Decoder for 16-Bit Windows Metafiles (*.wmf) and 16-Bit Adobe 
//! Placeable Metafiles (*,wmf) and 32-Bit Enhanced Windows 
//! Metafiles (*.emf) for Windows 95, Windows 98 and Windows NT 
//! >= 3.1.
//!
//! Comments by the author, Mario Westphal:<BR>
//!   <I>It can handle at least the 500 tested images I've got
//!   from various free and commercial clipart sources. If
//!   you find a WMF/EMF file it cannot handle, attach it to
//!   an email and send it to mw@mwlabs.de. I'll see what I
//!   can do. But, please, test it with another program
//!   before you send itin to see if it is really a valid 
//!   metafile.</I>
class CWEMFDecoder : public CPicDecoder  
{
public:
  //!
	CWEMFDecoder();

  //!
	virtual ~CWEMFDecoder();

protected:
	//!
  virtual void DoDecode(CBmp * pBmp, RGBAPIXEL** ppPal, int* pDestBPP, CDataSource* pDataSrc);

#ifdef _DEBUG
  virtual void AssertValid() const;
#endif
};

#endif // _WINDOWS

#endif // INCL_WEMFDEC_H
/*
/--------------------------------------------------------------------
|
|      $Log: wemfdec.h,v $
|      Revision 1.4  2000/01/16 20:43:18  anonymous
|      Removed MFC dependencies
|
|      Revision 1.3  1999/12/02 17:07:35  Ulrich von Zadow
|      Changes by bdelmee.
|
|
\--------------------------------------------------------------------
*/
