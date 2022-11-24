/*
/--------------------------------------------------------------------
|
|      $Id: dibsect.h,v 1.7 2000/01/17 23:37:12 Ulrich von Zadow Exp $
|
|      Copyright (c) 1996-1998 Ulrich von Zadow
|
\--------------------------------------------------------------------
*/

#ifndef INCL_DIBSECT
#define INCL_DIBSECT

#ifndef INCL_WINBMP
#include "winbmp.h"
#endif

//! This is a windows DIBSection wrapped in a CBmp-derived class.
//! It can be used just like a CWinBmp can be used. In addition,
//! CDIBSection can give access to the bitmap as a GDI bitmap handle.
//! This bitmap handle can be selected into a device context. All
//! normal GDI drawing functions can be used to write on the bitmap
//! in this way.
//! 
//! Internally, CDIBSections are stored with header and bits in two 
//! separate buffers.
class CDIBSection : public CWinBmp
{

public:
  //! Creates an empty bitmap.
  CDIBSection
    ();

  //! Destroys the bitmap.
  virtual ~CDIBSection
    ();

  //! Copy constructor
  CDIBSection
    ( const CBmp &Orig
    );

  //! Assignment operator.
  CDIBSection &operator=
    ( CBmp const &Orig
    );

  //! Assignment operator.
  CDIBSection &operator=
    ( CDIBSection const &Orig
    );

#ifdef _DEBUG
  virtual void AssertValid
    () const;    // Tests internal object state
#endif

  //! Calling this function causes the windows DIBSection to be detached
  //! from the CDIBSection object. This means that the bitmap handle and
  //! the bitmap memory (bits and BMI) must be deleted by some other object. 
  //! The CDIBSection object is in the same state as after a constructor 
  //! call after this function is called.
  virtual void Detach
    ();

  // CDIBSection output

  //! Draws the bitmap on the given device context using
  //! BitBlt.
  virtual void Draw
    ( HDC hDC,
      int x,
      int y,
      DWORD rop = SRCCOPY
    );

  //! Draws a portion of the bitmap on the given device context
  virtual BOOL DrawExtract
    ( HDC hDC,
      POINT pntDest,
      RECT rcSrc
    );

  // CDIBSection member access

  //! Returns a GDI handle to the bitmap. This handle can be selected
  //! into a DC and used in normal GDI operations. 
  //! Under Windows NT, GDI operations can be queued. This means that
  //! a program running under NT must call GdiFlush() before the 
  //! DIBSection can be used again after GetHandle() has been called. 
  //! See the documentation for GdiFlush() for details.
  HANDLE GetHandle
    ();


protected:

  // Protected callbacks

  //! Create a new empty DIB. Bits are uninitialized.
  //! Assumes that no memory is allocated before the call.
  virtual void internalCreate
    ( LONG Width,
      LONG Height,
      WORD BitsPerPixel,
      BOOL bAlphaChannel
    );

  // Creates a CDIBSection from an existing bitmap pointer.
  // Assumes that no memory is allocated before the call.
  virtual void internalCreate
    ( BITMAPINFOHEADER* pBMI
    );

  //! Deletes memory allocated by member variables.
  virtual void freeMembers
    ();

  //! Creates a copy of the current bitmap in a global memory block
  //! and returns a handle to this block.
  virtual HANDLE createCopyHandle
    ();

  //! Set color table pointer & pointer to bits based on m_pBMI.
  virtual void initPointers
    ();


private:
  // Local functions

  // Member variables.
  HANDLE   m_hBitmap;

  BOOL     m_bOwnsBitmap;
};

inline CDIBSection::CDIBSection
    ( const CBmp &Orig
    )
  : CWinBmp (Orig)
{
}

inline CDIBSection & CDIBSection::operator=
    ( CBmp const &Orig
    )
{
  CBmp::operator=(Orig);
  return *this;
}

inline CDIBSection & CDIBSection::operator=
    ( CDIBSection const &Orig
    )
{
  CBmp::operator=(Orig);
  return *this;
}


#endif
/*
/--------------------------------------------------------------------
|
|      $Log: dibsect.h,v $
|      Revision 1.7  2000/01/17 23:37:12  Ulrich von Zadow
|      Corrected bug in assignment operator.
|
|      Revision 1.6  2000/01/16 20:43:17  anonymous
|      Removed MFC dependencies
|
|
\--------------------------------------------------------------------
*/
