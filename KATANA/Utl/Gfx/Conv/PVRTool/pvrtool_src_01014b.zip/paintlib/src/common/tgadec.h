/*
/--------------------------------------------------------------------
|
|      $id$
|
|      Copyright (c) 1996-1998 Ulrich von Zadow
|
--------------------------------------------------------------------
*/

#ifndef INCL_TGADEC
#define INCL_TGADEC

#ifndef INCL_PICDEC
#include "picdec.h"
#endif

#ifndef INCL_TGA
#include "tga.h"
#endif

//! Targa file decoder. Decodes 8, 15, 16, 24 and 32 bpp
//! targa files (compressed and uncompressed) and returns an 8 or 32
//! bpp CBitmap. Preserves the alpha channel.
class CTGADecoder : public CPicDecoder
{


public:
  //! Creates a decoder
  CTGADecoder
    ();

  //! Destroys a decoder
  virtual ~CTGADecoder
    ();

protected:
  //! Main decoder routine. Reads the header, Creates the bitmap,
  //! reads the palette, and reads the image data.
  virtual void DoDecode
    ( CBmp * pBmp,
      RGBAPIXEL ** ppPal,
      int * pDestBPP,
      CDataSource * pDataSrc
    );

private:

  // Fills TGAHEADER structure with the information in the file.
  void readTgaHeader
    ( TGAHEADER * pTgaHead,       // Pointer to TGA header structure
      CDataSource * pDataSrc
    );

  // Reads the TGA palette and creates a windows palette.
  void readPalette
    ( int StartIndex,           // Index of first palette entry.
      int Length,               // Number of palette entries stored.
      int EntrySize,            // Size of palette entries in bits.
      CBmp * pBmp,
      RGBAPIXEL ** ppPal,
      int DestBPP,
      CDataSource * pDataSrc
    );


  // Creates a standalone RGBA grayscale palette.
  void setGrayPalette
    ( RGBAPIXEL ** ppPal
    );

  // Determines compression type and calls readData.
  void readImage
    ( TGAHEADER * pTgaHead,       // Pointer to TGA header structure
      CBmp * pBmp,
      RGBAPIXEL ** ppPal,
      int DestBPP,
      CDataSource * pDataSrc
    );

  // Reads image data line-by-line.
  void readData
    ( TGAHEADER * pTgaHead,       // Pointer to TGA header structure
      BOOL bCompressed,
      CBmp * pBmp,
      RGBAPIXEL * pPal,
      int DestBPP,
      CDataSource * pDataSrc
    );

  // Decodes one line of uncompressed image data.
  void expandUncompressedLine
    ( BYTE * pDest,
      int Width,
      BOOL bReversed,
      int bpp,
      CDataSource * pDataSrc,
      RGBAPIXEL * pPal,
      int DestBPP
    );

  // Decodes one line of compressed image data.
  void expandCompressedLine
    ( BYTE * pDest,
      int Width,
      BOOL bReversed,
      int bpp,
      CDataSource * pDataSrc,
      RGBAPIXEL * pPal,
      int DestBPP
    );

  // Reads one image pixel and returns it in RGBA format.
  RGBAPIXEL readPixel32
    ( int bpp,
      CDataSource * pDataSrc,
      RGBAPIXEL * pPal
    );

  // Reads one image pixel and returns it in 8-bit format.
  BYTE readPixel8
    ( int bpp,
      CDataSource * pDataSrc
    );
};

#endif
/*
/--------------------------------------------------------------------
|
|      $log$
|
--------------------------------------------------------------------
*/
