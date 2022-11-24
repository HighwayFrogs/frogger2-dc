/*
/--------------------------------------------------------------------
|
|      $Id: jpegdec.h,v 1.6 2000/01/16 20:43:13 anonymous Exp $
|
|      Copyright (c) 1996-1998 Ulrich von Zadow
|
\--------------------------------------------------------------------
*/

#ifndef INCL_JPEGDEC
#define INCL_JPEGDEC

#ifndef INCL_PICDEC
#include "picdec.h"
#endif

extern "C"
{
#include "jpeglib.h"
}

//! JPEG file decoder. Uses the independent JPEG group's library
//! to do the actual conversion.
class CJPEGDecoder : public CPicDecoder
{


public:
  //! Creates a decoder
  CJPEGDecoder
    ();

  //! Destroys a decoder
  ~CJPEGDecoder
    ();

/* I used this function to test the IJPEG rouines without a custom
   data source. If something goes wrong, it may still be useful to
   isolate bugs.
  CBmp * MakeBmpFromFile
    ( char * pszFName
    );
*/

  //! TRUE (the default) selects fast but sloppy decoding.
  void SetFast
    ( BOOL bFast
    );

  // Jo Hagelberg 15.4.99
  // this must be static as it's passed to extern C
  //! callback for jpeglib's progress notification
  static void JNotification
    (j_common_ptr cinfo
    );

protected:
  //! Sets up the jpeg library data source and error handler and
  //! calls the jpeg decoder.
  void DoDecode
    ( CBmp * pBmp,
      RGBAPIXEL ** ppPal,
      int * pDestBPP,
      CDataSource * pDataSrc
    );

private:
  // Assumes IJPEG decoder is already set up.
  void decodeGray
    ( CBmp * pBmp,
      int w,
      int h,
      int * pDestBPP
    );

  // Assumes IJPEG decoder is already set up.
  void decodeRGB
    ( CBmp * pBmp,
      int w,
      int h,
      int * pDestBPP
    );

  jpeg_decompress_struct cinfo;  // IJPEG decoder state.
  jpeg_error_mgr         jerr;   // Custom error manager.

  BOOL  m_bFast;  // TRUE selects fast but sloppy decoding.
};

#endif
/*
/--------------------------------------------------------------------
|
|      $Log: jpegdec.h,v $
|      Revision 1.6  2000/01/16 20:43:13  anonymous
|      Removed MFC dependencies
|
|      Revision 1.5  1999/12/08 15:39:45  Ulrich von Zadow
|      Unix compatibility changes
|
|      Revision 1.4  1999/11/27 18:45:48  Ulrich von Zadow
|      Added/Updated doc comments.
|
|
\--------------------------------------------------------------------
*/
