/*
/--------------------------------------------------------------------
|
|      $Id: bitmap.h,v 1.11 2000/01/16 20:43:12 anonymous Exp $
|
|      Copyright (c) 1996-1998 Ulrich von Zadow
|
\--------------------------------------------------------------------
*/

#ifndef INCL_BITMAP
#define INCL_BITMAP

#include "plobject.h"
#include "debug.h"

// Define the pixel format for RGB-bitmaps. Change this if you change
// the order of the color components. At the moment, PIXEL_SIZE must
// be 4. The decoders depend on it.
#define PIXEL_SIZE  4
typedef LONG RGBAPIXEL;

#define RGBA_BLUE   0
#define RGBA_GREEN  1
#define RGBA_RED    2
#define RGBA_ALPHA  3

// Note that the preceeding #defines aren't static const ints because
// this breaks preprocessor tests in several places.

//! Device- and OS-independent bitmap class. Manipulates uncompressed
//! bitmaps of all color depths.
//!
//! This class is an abstract base class. It exists to define a
//! format-independent interface for bitmap manipulation and to
//! provide common routines. Derived classes must support at least
//! the color depths 1, 8 and 32 bpp. CBmp defines a public interface
//! for general use and a protected interface for use by derived
//! classes.
//!
//! For 32 bpp, alpha channel information is stored in one byte
//! (RGBA_ALPHA) of each 4-byte pixel. To allow for optimizations
//! when no alpha channel is present, a flag is set whenever the
//! alpha information is valid. The complete alpha channel of a
//! bitmap can be replaced by a different one by calling
//! SetAlphaChannel(). A 0 in an alpha channel entry is completely
//! transparent; a 255 is completely opaque.
class CBmp : public CPLObject
{

public:
  //! Empty constructor. Constructors in derived classes create a
  //! small empty bitmap to ensure that the object is always in a
  //! sane state.
  CBmp
    ();

  //! Empty destructor.
  virtual ~CBmp
    ();

  //! Copy constructor
  CBmp
    ( const CBmp &Orig
    );

  //! Assignment operator. Note that assignment between different derived
  //! classes is possible and results in a format conversion.
  CBmp &operator=
    ( CBmp const &Orig
    );

#ifdef _DEBUG
  virtual void AssertValid
    () const;    // Tests internal object state
#endif

  // CBmp creation

  //! Creates a new empty bitmap. Memory for the bits is allocated
  //! but not initialized. Previous contents of the bitmap object are
  //! discarded. If bAlphaChannel is true, the bitmap is assumed to
  //! contain a valid alpha channel.
  virtual void Create
    ( LONG Width,
      LONG Height,
      WORD BitsPerPixel,
      BOOL bAlphaChannel
    );

  //! Creates a copy of rSrcBmp, converting color depth if nessesary.
  //! Supports 1, 8 and 32 BPP. Alpha channel information is preserved.
  void CreateCopy
    ( const CBmp & rSrcBmp,
      int BPPWanted = 0
    );

  //! Creates a copy of rSrcBmp, converting color information to grayscale
  //! in the process. The resulting bitmap is an 8-bit bitmap.
  void CreateGrayscaleCopy
    ( CBmp & rSrcBmp
    );

  //!
  void CreateRotatedCopy
    ( CBmp & rSrcBmp,
      double angle,
      RGBAPIXEL color
    );

  //! Creates a bitmap containing only the alpha channel of rSrcBmp.
  void CreateFromAlphaChannel
    ( CBmp & rSrcBmp
    );

  //!
  void CreateResizedBilinear
    ( CBmp& rSrcBmp,
      int NewXSize,
      int NewYSize
    );

  //!
  void CreateResizedBox
    ( CBmp& rSrcBmp,
      int NewXSize,
      int NewYSize,
      double NewRadius
    );

  //!
  void CreateResizedGaussian
    ( CBmp& rSrcBmp,
      int NewXSize,
      int NewYSize,
      double NewRadius
    );

  //!
  void CreateResizedHamming
    ( CBmp& rSrcBmp,
      int NewXSize,
      int NewYSize,
      double NewRadius
    );

  //!
  void CreateCropped
    ( CBmp& rSrcBmp,
      int XMin,
      int XMax,
      int YMin,
      int YMax
    );

  // CBmp manipulation

  //! Fills the color table with a grayscale palette. This function
  //! is only useable for bitmaps containing a color table. Index 0
  //! contains black (0) and the last index contains white (255). The
  //! alpha channel is set to opaque (255) for every palette entry.
  void SetGrayPalette
    ();

  //! Sets the color table to pPal. The contents or pPal are copied.
  void SetPalette
    ( RGBAPIXEL * pPal
    );

  //! Sets one entry in the color table. The function may only be
  //! called if there is a color table stored with the bitmap. The
  //! color table entry is set to the red, green, blue, and alpha
  //! values specified.
  void SetPaletteEntry
    ( BYTE Entry,
      BYTE r,
      BYTE g,
      BYTE b,
      BYTE a
    );

  //! Replaces the alpha channel of the bitmap with a new one. This
  //! only works for bitmaps with 32 bpp. pAlphaBmp must point to an
  //! 8 bpp bitmap with the same dimensions as the object. The alpha
  //! channel information is physically copied into the bitmap.
  void SetAlphaChannel
    ( CBmp * pAlphaBmp
    );

  //!
  void ResizeBilinear
    ( int NewXSize,
      int NewYSize
    );

  //!
  void ResizeBox
    ( int NewXSize,
      int NewYSize,
      double NewRadius
    );

  //!
  void ResizeGaussian
    ( int NewXSize,
      int NewYSize,
      double NewRadius
    );

  //!
  void ResizeHamming
    ( int NewXSize,
      int NewYSize,
      double NewRadius
    );

  //!
  void Crop
    ( int XMin,
      int XMax,
      int YMin,
      int YMax
    );

  //!
  void MakeGrayscale
    ();

  //!
  void Rotate
    ( double angle,
      RGBAPIXEL color
    );

  //! Slow but simple function to set a single pixel
  void SetPixel
    ( int x,
      int y,
      RGBAPIXEL pixel
    );

  //! Slow but simple function to get a single pixel
  RGBAPIXEL GetPixel
    ( int x,
      int y
    );

  //! Find the nearest color to cr in the palette used by this bitmap
  //! The function returns the palette index of the color if the
  //! bitmap has a palette. For true-color bitmaps, it returns
  //! the color itself.
  int FindNearestColor
    ( RGBAPIXEL cr
    );

  // CBmp information.

  //!
  int GetWidth
    () const;

  //!
  int GetHeight
    () const;

  //!
  virtual long GetMemUsed
    () = 0;

  //!
  int GetNumColors
    ();

  //!
  int GetBitsPerPixel
    () const;

  //!
  BOOL HasAlpha
    () const;

  //!
  void SetHasAlpha
    (BOOL b
    );

  //! Returns number of bytes used per line.
  virtual long GetBytesPerLine
    () = 0;

  // CBmp direct manipulation

  //! Returns the address of the color table of the bitmap or NULL if
  //! no color table exists. The color table is stored as an array of
  //! consecutive RGBAPIXELs.
  RGBAPIXEL * GetPalette
    () const;

  //! Returns pointer to an array containing the starting addresses of
  //! the bitmap lines. This array should be used whenever the bitmap
  //! bits need to be manipulated directly.
  BYTE ** GetLineArray
    () const;

protected:

  //! Create a new bitmap with uninitialized bits. (Assume no memory
  //! is allocated yet.)
  virtual void internalCreate
    ( LONG Width,
      LONG Height,
      WORD BitsPerPixel,
      BOOL bAlphaChannel
    ) = 0;

  //! Delete memory allocated by member variables.
  virtual void freeMembers
    () = 0;

  //! Initialize internal table of line addresses.
  virtual void initLineArray
    () = 0;

  //! Can be called from internalCreate() to initialize object state.
  void initLocals
    ( LONG Width,
      LONG Height,
      WORD BitsPerPixel,
      BOOL bAlphaChannel
    );

  void create32BPPCopy
    ( const CBmp & rSrcBmp
    );

  void create8BPPCopy
    ( const CBmp & rSrcBmp
    );

  void create1BPPCopy
    ( const CBmp & rSrcBmp
    );

  // Member variables

  int m_Width;
  int m_Height;
  WORD m_bpp;

  RGBAPIXEL * m_pClrTab;      // Pointer to the color table.
  BYTE      * m_pBits;        // Pointer to the bits.
  BOOL        m_bAlphaChannel;
  BYTE     ** m_pLineArray;   // Table of the starting addresses of
                              // the lines.

private:
};

inline CBmp::CBmp
    ( const CBmp &Orig
    )
{
  CreateCopy(Orig);
}


inline CBmp & CBmp::operator=
    ( CBmp const &Orig
    )
{
  if (this != &Orig)
    CreateCopy(Orig);
  return *this;
}

inline void CBmp::SetPaletteEntry
    ( BYTE Entry,
      BYTE r,
      BYTE g,
      BYTE b,
      BYTE a
    )
{
  BYTE * pEntry = (BYTE *)(m_pClrTab+Entry);

  pEntry[RGBA_RED] = r;
  pEntry[RGBA_GREEN] = g;
  pEntry[RGBA_BLUE] = b;
  pEntry[RGBA_ALPHA] = a;
}


// CBmp information

inline int CBmp::GetWidth
    () const
{
  PLASSERT_VALID (this);

  return m_Width;

}


inline int CBmp::GetHeight
    () const
{
  PLASSERT_VALID (this);

  return m_Height;
}


inline int CBmp::GetNumColors
    ()
{
  PLASSERT_VALID (this);

  if (m_bpp == 32)
    return 1 << 24;
   else
    return 1 << m_bpp;
}


inline int CBmp::GetBitsPerPixel
    () const
{
  PLASSERT_VALID (this);

  return m_bpp;
}


inline BOOL CBmp::HasAlpha
    () const
{
  PLASSERT_VALID (this);

  return m_bAlphaChannel;
}


inline void CBmp::SetHasAlpha
    (BOOL b
	)
{
  PLASSERT_VALID (this);
  m_bAlphaChannel = b;
}

// CBmp direct manipulation


inline BYTE ** CBmp::GetLineArray
    () const
{
  return m_pLineArray;
}


inline RGBAPIXEL * CBmp::GetPalette
    () const
    // Returns adress of the color table of the bitmap or NULL if no
    // color table exists.
{
  PLASSERT_VALID (this);

  return m_pClrTab;
}

// Global function

inline void SetRGBAPixel
    ( RGBAPIXEL * pPixel,
      BYTE r,
      BYTE g,
      BYTE b,
      BYTE a
    )
{
  ((BYTE *)pPixel)[RGBA_RED  ] = r;
  ((BYTE *)pPixel)[RGBA_GREEN] = g;
  ((BYTE *)pPixel)[RGBA_BLUE ] = b;
  ((BYTE *)pPixel)[RGBA_ALPHA] = a;
}

#ifndef _WINDOWS	// bdelmee; added for *nix
inline BYTE GetRValue(RGBAPIXEL rgba) { return ((BYTE *) &rgba)[RGBA_RED  ]; }
inline BYTE GetGValue(RGBAPIXEL rgba) { return ((BYTE *) &rgba)[RGBA_GREEN]; }
inline BYTE GetBValue(RGBAPIXEL rgba) { return ((BYTE *) &rgba)[RGBA_BLUE ]; }
inline BYTE GetAValue(RGBAPIXEL rgba) { return ((BYTE *) &rgba)[RGBA_ALPHA]; }
#endif

#endif
/*
/--------------------------------------------------------------------
|
|      $Log: bitmap.h,v $
|      Revision 1.11  2000/01/16 20:43:12  anonymous
|      Removed MFC dependencies
|
|      Revision 1.10  1999/12/10 01:27:26  Ulrich von Zadow
|      Added assignment operator and copy constructor to
|      bitmap classes.
|
|      Revision 1.9  1999/12/09 16:35:22  Ulrich von Zadow
|      no message
|
|      Revision 1.8  1999/12/08 15:39:45  Ulrich von Zadow
|      Unix compatibility changes
|
|      Revision 1.7  1999/12/02 17:07:34  Ulrich von Zadow
|      Changes by bdelmee.
|
|      Revision 1.6  1999/10/22 21:25:51  Ulrich von Zadow
|      Removed buggy octree quantization
|
|
\--------------------------------------------------------------------
*/
