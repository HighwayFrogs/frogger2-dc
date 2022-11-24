/*
/--------------------------------------------------------------------
|
|      $Id: tiffdecex.h,v 1.3 2000/01/16 20:43:15 anonymous Exp $
|
|      Copyright (c) 1996-1998 Ulrich von Zadow
|
\--------------------------------------------------------------------
*/

#ifndef INCL_TIFFDECEX
#define INCL_TIFFDECEX

#include "tiffdec.h"

//! Modest interface to get access to tagged fields
//! This should move to CTiffDecoder but I dare not re-introduce 
//! data members in the reworked thread-safe base decoder
class CTIFFDecoderEx : public CTIFFDecoder
{

public:
  CTIFFDecoderEx();
  ~CTIFFDecoderEx();
  BOOL Associate( CDataSource * pDataSrc );
  void Dissociate();
  int GetField( int tag_id, ... );
  //! this is this first argument to most libtiff's routines
  //! use at your own risk; you've been warned!
  TIFF* GetTIFFHandle()	{ return m_TiffToken; }
 	
protected:
  virtual void DoDecode
    ( CBmp * pBmp,
      RGBAPIXEL ** ppPal,
      int * pDestBPP,
      CDataSource * pDataSrc
    );

private:
  TIFF* m_TiffToken;
};

#endif
/*
/--------------------------------------------------------------------
|
|      $Log: tiffdecex.h,v $
|      Revision 1.3  2000/01/16 20:43:15  anonymous
|      Removed MFC dependencies
|
|      Revision 1.2  2000/01/09 22:24:10  Ulrich von Zadow
|      Corrected tiff callback bug.
|
|      Revision 1.1  1999/10/19 21:30:42  Ulrich von Zadow
|      B. Delmee - Initial revision
|
|
--------------------------------------------------------------------
*/
