/*
/--------------------------------------------------------------------
|
|      $Id: pcxdec.h,v 1.7 2000/01/16 20:43:13 anonymous Exp $
|
|      Copyright (c) 1996-1998 Ulrich von Zadow
|
\--------------------------------------------------------------------
*/

#ifndef INCL_PCXDEC
#define INCL_PCXDEC

#ifndef INCL_PICDEC
#include "picdec.h"
#endif

#ifndef INCL_PCX
#include "pcx.h"
#endif

//! PCX file decoder. Decodes 8, 15, 16, 24 and 32 bpp
//! PCX files (compressed and uncompressed) and returns an 8 or 32
//! bpp CBitmap. Preserves the alpha channel.
class CPCXDecoder : public CPicDecoder
{

public:
  //! Creates a decoder
  CPCXDecoder();

  //! Destroys a decoder
  virtual ~CPCXDecoder ();

protected:
  //! Main decoder routine. Reads the header, Creates the bitmap,
  //! reads the palette, and reads the image data.
  virtual void DoDecode (CBmp * pBmp, RGBAPIXEL ** ppPal, int * pDestBPP, CDataSource * pDataSrc);

private:
  void PCX_PlanesToPixels(BYTE *pixels, BYTE *bitplanes,
                          short bytesperline, short planes, short bitsperpixel);
  void PCX_UnpackPixels(BYTE *pixels, BYTE *bitplanes,
                        short bytesperline, short planes, short bitsperpixel);
};

#endif

/*
/--------------------------------------------------------------------
|
|      $Log: pcxdec.h,v $
|      Revision 1.7  2000/01/16 20:43:13  anonymous
|      Removed MFC dependencies
|
|      Revision 1.6  2000/01/10 23:52:59  Ulrich von Zadow
|      Changed formatting & removed tabs.
|
|      Revision 1.5  1999/12/15 21:16:30  Ulrich von Zadow
|      Removed references to gif in pcx decoder.
|
|      Revision 1.4  1999/12/10 01:27:26  Ulrich von Zadow
|      Added assignment operator and copy constructor to
|      bitmap classes.
|
|      Revision 1.3  1999/12/08 15:39:45  Ulrich von Zadow
|      Unix compatibility changes
|
|      Revision 1.2  1999/11/27 18:45:48  Ulrich von Zadow
|      Added/Updated doc comments.
|
\--------------------------------------------------------------------
*/
