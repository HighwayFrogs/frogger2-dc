/*
/--------------------------------------------------------------------
|
|      $Id: tiffdec.h,v 1.6 2000/01/16 20:43:15 anonymous Exp $
|
|      Copyright (c) 1996-1998 Ulrich von Zadow
|
\--------------------------------------------------------------------
*/

#ifndef INCL_TIFFDEC
#define INCL_TIFFDEC

#ifndef INCL_PICDEC
#include "picdec.h"
#endif

extern "C"
{
#include "tiff.h"
#include "tiffio.h"
}

//! TIFF file decoder. Uses LIBTIFF to do the actual conversion.
//! Supports 1, 8 and 24 bits per pixel as output formats.
class CTIFFDecoder : public CPicDecoder
{

friend class _tiff_helper;
friend class CTIFFEncoder;

public:
  //! Creates a decoder
  CTIFFDecoder
    ();

  //! Destroys a decoder
  virtual ~CTIFFDecoder
    ();
/*
  CBmp * MakeBmpFromFile
    ( char * pszFName
    );
*/
protected:
  //! Sets up LIBTIFF environment and calls LIBTIFF to decode an
  //! image.
  virtual void DoDecode
    ( CBmp * pBmp,
      RGBAPIXEL ** ppPal,
      int * pDestBPP,
      CDataSource * pDataSrc
    );

  virtual void DoTiffDecode
    ( CBmp * pBmp,
      RGBAPIXEL ** ppPal,
      int * pDestBPP,
      CDataSource * pDataSrc,
      TIFF* tiff
    );

private:
	// these 2 helpers dedicated to hi/low bit-depths respectively
	void doHiColor
    ( TIFF * tif,
      CBmp * pBmp,
      int * pDestBPP
    );
	void doLoColor
    ( TIFF * tif,
      CBmp * pBmp,
      int * pDestBPP,
      RGBAPIXEL ** ppPal
    );

  // Callback from the TIFF library.
  // Should be accessed only by the TIFF decoder and libtiff.
  static void Win32WarningHandler
    ( const char* module,
      const char* fmt,
      va_list ap
    );

  // Callback from the TIFF library.
  // Should be accessed only by the TIFF decoder and libtiff.
  static void Win32ErrorHandler
    ( const char* module,
      const char* fmt,
      va_list ap
    );

private:
  // Temporary storage for an error message. 
  // Used in the callbacks, so it's static.
  static char m_szLastErr[256];
};


#endif
/*
/--------------------------------------------------------------------
|
|      $Log: tiffdec.h,v $
|      Revision 1.6  2000/01/16 20:43:15  anonymous
|      Removed MFC dependencies
|
|      Revision 1.5  2000/01/08 15:56:12  Ulrich von Zadow
|      Made sure change logging works in every file.
|
|      Revision 1.4  1999/10/03 18:50:52  Ulrich von Zadow
|      Added automatic logging of changes.
|
|
--------------------------------------------------------------------
*/
