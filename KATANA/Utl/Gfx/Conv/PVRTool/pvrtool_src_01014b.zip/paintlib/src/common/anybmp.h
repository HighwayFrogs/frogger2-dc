/*
/--------------------------------------------------------------------
|
|      $Id: anybmp.h,v 1.7 2000/01/16 20:43:12 anonymous Exp $
|
|      Copyright (c) 1996-1998 Ulrich von Zadow
|
\--------------------------------------------------------------------
*/

#ifndef INCL_ANYBMP
#define INCL_ANYBMP

#ifndef INCL_BITMAP
#include "bitmap.h"
#endif

//! Manipulates uncompressed device- and platform-independent bitmaps.
//! Supported data formats are 8 and 32 bpp. The data is stored
//! sequentially without padding in the bitmap. The class implements exactly
//! the interface defined by CBmp without additions.
class CAnyBmp : public CBmp
{

public:
  //! Creates an empty bitmap.
  CAnyBmp
    ();

  //! Destroys the bitmap.
  virtual ~CAnyBmp
    ();

  //! Copy constructor
  CAnyBmp
    ( const CBmp &Orig
    );

  //! Assignment operator.
  CAnyBmp &operator=
    ( CBmp const &Orig
    );

  // CAnyBmp information

  //! Returns the amount of memory used by the object.
  long GetMemUsed
    ();

  //! Returns number of bytes used per line.
  long GetBytesPerLine
    ();


  // Static functions

  //! Returns memory needed by a bitmap with the specified attributes.
  static long GetMemNeeded
    ( LONG width,
      LONG height,
      WORD BitsPerPixel
    );

  //! Returns memory needed by bitmap bits.
  static long GetBitsMemNeeded
    ( LONG width,
      LONG height,
      WORD BitsPerPixel
    );


protected:
  // Local functions

  // Create a new bitmap with uninitialized bits. Assumes no memory
  // is allocated yet.
  virtual void internalCreate
    ( LONG Width,
      LONG Height,
      WORD BitsPerPixel,
      BOOL bAlphaChannel
    );

  // Deletes memory allocated by member variables.
  virtual void freeMembers
    ();

  // Initializes internal table of line addresses.
  virtual void initLineArray
    ();
};

inline CAnyBmp::CAnyBmp
    ( const CBmp &Orig
    )
  : CBmp (Orig)
{
}

inline CAnyBmp & CAnyBmp::operator=
    ( CBmp const &Orig
    )
{
  CBmp::operator=(Orig);
  return *this;
}


#endif
/*
/--------------------------------------------------------------------
|
|      $Log: anybmp.h,v $
|      Revision 1.7  2000/01/16 20:43:12  anonymous
|      Removed MFC dependencies
|
|      Revision 1.6  1999/12/14 12:30:13  Ulrich von Zadow
|      Corrected copy constructor and assignment operator.
|
|      Revision 1.5  1999/12/10 01:27:26  Ulrich von Zadow
|      Added assignment operator and copy constructor to
|      bitmap classes.
|
|      Revision 1.4  1999/12/02 17:07:34  Ulrich von Zadow
|      Changes by bdelmee.
|
|
--------------------------------------------------------------------
*/
