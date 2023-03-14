/*
/--------------------------------------------------------------------
|
|      $Id: tiffenc.h,v 1.6 2000/01/16 20:43:15 anonymous Exp $
|
|      Copyright (c) 1996-1998 Ulrich von Zadow
|
\--------------------------------------------------------------------
*/
// not quite ready for prime-time; bdelmee; 2/99

#ifndef INCL_TIFFENC
#define INCL_TIFFENC

#ifndef INCL_PICENC
#include "picenc.h"
#endif

typedef	struct tiff TIFF;

//! TIFF file encoder. Uses LIBTIFF to do the actual conversion.
class CTIFFEncoder : public CPicEncoder
{

public:
  //! Creates an encoder
  CTIFFEncoder
    ();

  //! Destroys a encoder
  virtual ~CTIFFEncoder
    ();

protected:
  //! Sets up LIBTIFF environment and calls LIBTIFF to encode an image.
  virtual void DoEncode
    ( CBmp* pBmp,
      CDataSink* pDataSnk
    );

  //! this one mostly useful for the "extended" decoder
  virtual void DoTiffEncode
    ( CBmp* pBmp,
      TIFF* tif
    );

  //! According to the characteristics of the given bitmap, 
  //! set the baseline tags 
  int SetBaseTags( TIFF*, CBmp* );

  //! Set tag value in directory; also, "pseudo-tags" are interpreted by 
  //! LIBTIFF as modifiers to certain algorithms (compression....). 
  //! We do _not_ support multiple images by file, nor tiling nor stripping.
  int SetField( TIFF*, int tag_id, ... );
};


#endif  // INCL_TIFFENC
/*
/--------------------------------------------------------------------
|
|      $Log: tiffenc.h,v $
|      Revision 1.6  2000/01/16 20:43:15  anonymous
|      Removed MFC dependencies
|
|      Revision 1.5  1999/12/02 17:07:34  Ulrich von Zadow
|      Changes by bdelmee.
|
|
\--------------------------------------------------------------------
*/
