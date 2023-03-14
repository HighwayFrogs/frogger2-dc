/*
/--------------------------------------------------------------------
|
|      $Id: tiffencex.h,v 1.3 2000/01/16 20:43:15 anonymous Exp $
|
|      Copyright (c) 1996-1998 Ulrich von Zadow
|
\--------------------------------------------------------------------
*/
// not quite ready for prime-time; bdelmee; 2/99

#ifndef INCL_TIFFENCEX
#define INCL_TIFFENCEX

#include "tiffenc.h"

//! Basic tiff output is available via "CTIFFEncoder::MakeFileFromBmp()".
//! This does not compress data or allow any informative tag to be set.
//! For those who need greater control, CTIFFDecoderEx is provided.
class CTIFFEncoderEx : public CTIFFEncoder
{

public:
  //! Creates an encoder
  CTIFFEncoderEx();

  //! Destroys an encoder
  virtual ~CTIFFEncoderEx();

  //! Bind DataSink to Encoder (the link is the TIFF* member)
  BOOL Associate( CDataSink* );

  //! Flush the output
  void Dissociate( void );

  //! SetBaseTabs and SetField make their base class equivalent usable,
  //! without requiring the user to know about the libtiff internals (TIFF*)
  int SetBaseTags( CBmp* );

  //! SetBaseTabs and SetField make their base class equivalent usable,
  //! without requiring the user to know about the libtiff internals (TIFF*)
  int SetField( int tag_id, ... );

protected:
  //! Sets up LIBTIFF environment and calls LIBTIFF to encode an image.
  virtual void DoEncode( CBmp*, CDataSink* = 0 );

private:
  TIFF* m_TiffToken;
};

#endif
/*
/--------------------------------------------------------------------
|
|      $Log: tiffencex.h,v $
|      Revision 1.3  2000/01/16 20:43:15  anonymous
|      Removed MFC dependencies
|
|      Revision 1.2  1999/11/27 18:45:48  Ulrich von Zadow
|      Added/Updated doc comments.
|
|      Revision 1.1  1999/10/19 21:30:42  Ulrich von Zadow
|      B. Delmee - Initial revision
|
|
--------------------------------------------------------------------
*/
