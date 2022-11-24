/*
/--------------------------------------------------------------------
|
|      $id$
|
|      Copyright (c) 1996-1998 Ulrich von Zadow
|
--------------------------------------------------------------------
*/

#ifndef INCL_PICTDEC
#define INCL_PICTDEC

#ifndef INCL_CONFIG
#include "config.h"
#endif

#ifndef INCL_PICDEC
#include "picdec.h"
#endif

#include "qdraw.h"

class CJPEGDecoder;

//! This class decodes macintosh PICT files with 1,2,4,8,16 and 32
//! bits per pixel as well as PICT/JPEG. If an alpha channel is
//! present in a 32-bit-PICT, it is decoded as well.
//! The PICT format is a general picture file format and can
//! contain a lot of other elements besides bitmaps. These elements
//! are ignored.
//!
//! There are several opcodes for which I did not find examples.
//! I have marked the appropriate code as "untested". It'll
//! probably work anyway.
class CPictDecoder : public CPicDecoder
{


public:
  //! Creates a decoder. A JPEG decoder is needed to decode PICT
  //! files containing JPEGs.
  CPictDecoder
    ( CJPEGDecoder * pJPEGDecoder
    );

  //! Destroys a decoder.
  virtual ~CPictDecoder
    ();

protected:
  //! Does the actual decoding after a data source has been set up.
  virtual void DoDecode
    ( CBmp * pBmp,
      RGBAPIXEL ** ppPal,
      int * pDestBPP,
      CDataSource * pDataSrc
    );

private:
  // Decodes header and version information.
  // Sets m_Version.
  // Performs checks to make sure the data is really a pict file.
  void readHeader
    ( CDataSource * pDataSrc,
      int& Version
    );

  // This is the main decoder loop. The functions reads opcodes,
  // skips some of them, and hands the rest to opcode-specific
  // functions. It stops decoding after the first opcode containing
  // bitmapped data.
  void interpretOpcodes
    ( CBmp * pBmp,
      RGBAPIXEL ** ppPal,
      int * pDestBPP,
      CDataSource * pDataSrc,
      int& Version
    );

  // Moves to an even byte position in the file and returns the
  // opcode found there.
  WORD readOpcode
    ( int Version,
      CDataSource * pDataSrc
    );

  // Opcode functions

  // Skips clipping rectangle or region opcode.
  void clip
    ( CDataSource * pDataSrc
    );

  // Skips pattern definition opcode.
  // Untested!!
  void pixPat
    ( CDataSource * pDataSrc
    );

  // Skips polygon/region opcodes.
  // Untested!!
  void skipPolyOrRegion
    ( CDataSource * pDataSrc
    );

  // Opcode: Bitmap/pixmap data clipped by a rectangle.
  void bitsRect
    ( CDataSource * pDataSrc,
      RGBAPIXEL **ppPal,
      CBmp * pBmp,
      int * pDestBPP
    );

  // Opcode: Bitmap/pixmap data clipped by a region.
  // Untested...
  void bitsRegion
    ( CDataSource * pDataSrc,
      RGBAPIXEL **ppPal,
      CBmp * pBmp,
      int * pDestBPP
    );

  // DirectBitsRect opcode.
  void opcode9a
    ( CDataSource * pDataSrc,
      int * pDestBPP,
      CBmp * pBmp,
      RGBAPIXEL ** ppPal
    );

  // Long comment opcode. Skipped.
  void longComment
    ( CDataSource * pDataSrc
    );

  // JPEG opcode. Invoke the JPEG decoder for this PICT.
  void jpegOp
    ( CDataSource * pDataSrc,
      CBmp * pBmp,
      int * pDestBPP
    );

  // Bitmap & Pixmap functions.

  // Allocates the output bitmap.
  void createOutputBmp
    ( MacpixMap PixMap,
      int DestBPP,
      CBmp * pBmp
    );

  // Decode version 1 bitmap: 1 bpp.
  void doBitmap
    ( int rowBytes,
      BOOL bIsRegion,
      RGBAPIXEL ** ppPal,
      CBmp * pBmp,
      int * pDestBPP,
      CDataSource * pDataSrc
    );

  // Decode version 2 pixmap.
  void doPixmap
    ( int rowBytes,
      BOOL bIsRegion,
      RGBAPIXEL ** ppPal,
      CBmp * pBmp,
      int * pDestBPP,
      CDataSource * pDataSrc
    );

  // This routine decompresses BitsRects with a packType of 4 (and
  // 32 bits per pixel). In this format, each line is separated
  // into 8-bit-bitplanes and then compressed via RLE. To decode,
  // the routine decompresses each line & then juggles the bytes
  // around to get pixel-oriented data.
  void unpack32bits
    ( MacRect* pBounds,
      WORD rowBytes,
      int NumBitPlanes,    // 3 if RGB, 4 if RGBA
      CBmp * pBmp,
      CDataSource * pDataSrc
    );

  // Decompression routine for 8 bpp. rowBytes is the number of
  // bytes each source row would take if it were uncompressed.
  // This _isn't_ equal to the number of pixels in the row - apple
  // pads the data to a word boundary and then compresses it. Of
  // course, we have to decompress the excess data and then throw it
  // away.
  void unpack8bits
    ( MacRect* pBounds,
      WORD rowBytes,
      CBmp * pBmp,
      CDataSource * pDataSrc,
      RGBAPIXEL ** ppPal,
      int DestBPP
    );

  // Decompression routine for everything but 32 bpp. This routine
  // is slower than the two routines above since it has to deal
  // with a lot of special cases.
  void unpackbits
    ( MacRect* pBounds,
      WORD rowBytes,
      int pixelSize,         // Source bits per pixel.
      CBmp * pBmp,
      CDataSource * pDataSrc,
      RGBAPIXEL ** ppPal,
      int DestBPP
    );

  // Skips unneeded packbits.
  void skipBits
    ( MacRect* pBounds,
      WORD rowBytes,
      int pixelSize,         // Source bits per pixel.
      CDataSource * pDataSrc
    );

  // Expands one RLE unit to 32-bit pixel data.
  void expandBuf
    ( BYTE * pDestBuf,
      BYTE * pSrcBuf,
      int Width,       // Width in bytes for 8 bpp or less.
                       // Width in pixels for 16 bpp.
      int bpp,         // bits per pixel
      RGBAPIXEL * pPal
    );

  // Expands Width units to 8-bit pixel data.
  // Max. 8 bpp source format.
  void expandBuf8
    ( BYTE * pDestBuf,
      BYTE * pSrcBuf,
      int Width,       // Width in bytes.
      int bpp          // bits per pixel.
    );

  void readPixmap
    ( MacpixMap * pPixMap,
      CDataSource * pDataSrc
    );

  // Reads a mac colour table into a windows palette.
  RGBAPIXEL * readColourTable
    ( WORD * pNumColors,
      CDataSource * pDataSrc
    );

  void readRect
    ( MacRect * pr,
      CDataSource * pDataSrc
    );

  void dumpRect
    ( char * psz,
      MacRect * pr
    );

  void tracePixMapHeader
    ( int Level,
      MacpixMap * pPixMap
    );


  CJPEGDecoder * m_pJPEGDecoder; // Used if a pict file contains a JPEG.
};

#endif
/*
/--------------------------------------------------------------------
|
|      $log$
|
--------------------------------------------------------------------
*/
