/*
/--------------------------------------------------------------------
|
|      $Id: winbmp.h,v 1.8 2000/01/17 23:37:12 Ulrich von Zadow Exp $
|
|      Copyright (c) 1996-1998 Ulrich von Zadow
|
\--------------------------------------------------------------------
*/

#ifndef INCL_WINBMP
#define INCL_WINBMP

#ifndef INCL_BITMAP
#include "bitmap.h"
#endif

//! This is the windows version of CBmp. The internal storage format
//! is a windows DIB. It supports all color depths allowed by
//! windows: 1, 4, 8, 16, 24, and 32 bpp.
//!
//! The subset of the windows DIB format supported is as follows: The
//! DIB is stored so that header, palette, and bits are in one
//! buffer. The bottom line is stored first (biHeight must be > 0)
//! and the data is uncompressed (BI_RGB). Color tables for 16, 24,
//! and 32 bpp are not supported. biClrUsed is always 0. The palette
//! mode is DIB_RGB_COLORS. DIB_PAL_COLORS is not supported.
//!
//! Note that almost all real-life DIBs conform to this subset
//! anyway, so there shouldn't be any problems.
//!
//! <i>In the current version, some functions (notably CreateCopy) only
//! support 1, 8 and 32 bpp. Sorry!</i>
class CWinBmp : public CBmp
{

public:
  // Creates an empty bitmap.
  CWinBmp ();

  //! Destroys the bitmap.
  virtual ~CWinBmp ();

  //! Copy constructor
  CWinBmp (const CBmp &Orig);

  //! Assignment operator.
  CWinBmp &operator= (CBmp const &Orig);

  //! Assignment operator.
  CWinBmp &operator= (CWinBmp const &Orig);

#ifdef _DEBUG
  virtual void AssertValid () const;    // Tests internal object state
#endif

  // CWinBmp manipulation

  // Do a bitblt using the alpha channel of pSrcBmp. Restricted to
  // 32 bpp.
  // Legacy routine. Use the Blt classes instead.
  void AlphaBlt (CWinBmp * pSrcBmp, int x, int y);

  // CWinBmp information

  //! Returns the amount of memory used by the object.
  virtual long GetMemUsed ();

  //! Returns number of bytes used per line.
  long GetBytesPerLine ();

  // Windows-specific interface

  //! Loads a bitmap from a windows resource (.rc or .res linked to
  //! the exe). Fails if the bitmap is compressed.
  virtual void CreateRes (HINSTANCE lh_ResInst, int ID);

  //! Takes a HBITMAP and converts it to a CWinBmp.
  void CreateFromHBitmap (HBITMAP hBitMap);

  //! Returns the size of the bitmap in pixels
  SIZE GetSize ();

  //! Access the windows bitmap structure. Using this structure, all
  //! standard DIB manipulations can be performed.
  BITMAPINFOHEADER * GetBMI ();

  //! Saves the DIB in windows bmp format.
  void SaveAsBmp (const char * pszFName);

  // CWinBmp output

  //! Draws the bitmap on the given device context using
  //! StretchDIBits.
  virtual void Draw (HDC hDC, int x, int y, DWORD rop = SRCCOPY);

  //! Draws the bitmap on the given device context using
  //! StretchDIBits. Scales the bitmap by Factor.
  virtual void StretchDraw (HDC hDC, int x, int y, double Factor, DWORD rop = SRCCOPY);

  //! Draws the bitmap on the given device context using
  //! StretchDIBits. Scales the bitmap so w is the width and
  //! h the height.
  virtual void StretchDraw (HDC hDC, int x, int y, int w, int h, DWORD rop = SRCCOPY);

  //! Draws a portion of the bitmap on the given device context
  virtual BOOL DrawExtract (HDC hDC, POINT pntDest, RECT rcSrc);

  //! Puts a copy of the bitmap in the clipboard
  void ToClipboard ();

  //! Reads the clipboard into the bitmap
  bool FromClipboard ();

  //! Gets a pointer to the bitmap bits. (Usually, using GetLineArray()
  //! is much easier!)
  BYTE * GetBits ();

  // Static functions

  //! Returns memory needed by a bitmap with the specified attributes.
  static long GetMemNeeded (LONG width, LONG height, WORD BitsPerPixel);

  //! Returns memory needed by bitmap bits.
  static long GetBitsMemNeeded (LONG width, LONG height, WORD BitsPerPixel);

protected:

  // Protected callbacks

  //! Create a new empty DIB. Bits are uninitialized.
  //! Assumes that no memory is allocated before the call.
  virtual void internalCreate (LONG Width, LONG Height, WORD BitsPerPixel, BOOL bAlphaChannel);

  //! Creates a CWinBmp from an existing bitmap pointer.
  //! Assumes that no memory is allocated before the call.
  virtual void internalCreate (BITMAPINFOHEADER* pBMI);

  //! Deletes memory allocated by member variables.
  virtual void freeMembers ();

  //! Initializes internal table of line addresses.
  virtual void initLineArray ();

  // Creates a copy of the current bitmap in a global memory block
  // and returns a handle to this block.
  virtual HANDLE createCopyHandle ();

  // Set color table pointer & pointer to bits based on m_pBMI.
  virtual void initPointers ();

  // Member variables.

  BITMAPINFOHEADER * m_pBMI;  // Pointer to picture format information.

private:
};

inline CWinBmp::CWinBmp (const CBmp &Orig)
    : CBmp (Orig)
{}


inline CWinBmp & CWinBmp::operator= ( CBmp const &Orig)
{
  CBmp::operator=(Orig);
  return *this;
}

inline CWinBmp & CWinBmp::operator= ( CWinBmp const &Orig)
{
  CBmp::operator=(Orig);
  return *this;
}


#endif
/*
/--------------------------------------------------------------------
|
|      $Log: winbmp.h,v $
|      Revision 1.8  2000/01/17 23:37:12  Ulrich von Zadow
|      Corrected bug in assignment operator.
|
|      Revision 1.7  2000/01/16 20:43:18  anonymous
|      Removed MFC dependencies
|
|      Revision 1.6  2000/01/10 23:53:01  Ulrich von Zadow
|      Changed formatting & removed tabs.
|
|
\--------------------------------------------------------------------
*/
