/*
/--------------------------------------------------------------------
|
|      $Id: picenc.h,v 1.7 2000/01/16 20:43:14 anonymous Exp $
|
|      Copyright (c) 1996-1998 Ulrich von Zadow
|
\--------------------------------------------------------------------
*/
// not quite ready for prime-time; bdelmee; 2/99

#ifndef INCL_PICENC
#define INCL_PICENC

#ifndef INCL_BITMAP
#include "bitmap.h"
#endif

#ifndef INCL_DATASNK
#include "datasink.h"
#endif

// the details of the bitmap class are not needed here
class CBmp;

//! CPicEncoder is an abstract base class. It defines common routines
//! for all encoders. Encoders for specific file formats can be
//! derived from this class. Objects of this class interact with a
//! CDataSink to encode bitmaps.
class CPicEncoder : public CPLObject
{


public:
  //! Empty constructor. The actual initialization takes place in a
  //! derived class.
  CPicEncoder
    ();

  //! Destructor. Frees memory allocated.
  virtual ~CPicEncoder
    ();

  //! Encodes a bitmap to a file and stores.
  virtual void MakeFileFromBmp(
    const char *,
    CBmp*
    );

  //! Encodes a picture to a pre-existing data destination (sink).
  virtual void SaveBmp( CBmp*, CDataSink* );

  void SetTraceConfig( int Level, char * pszFName );

  // This function is needed by callbacks outside of any object,
  // so it's public and static. It should not be called from
  // outside of the library. - 3.6.99 MS
  static void raiseError( int Code, char * pszErr );

  void Trace( int TraceLevel, const char * pszMessage );

protected:
  //! Implements the actual encoding process. Uses variables local to the
  //! object to retrieve and store the data. Implemented in derived classes.
  //!
  virtual void DoEncode
    ( CBmp* pBmp,
      CDataSink* pDataSrc
      ) = 0;
};
#endif
/*
/--------------------------------------------------------------------
|
|      $Log: picenc.h,v $
|      Revision 1.7  2000/01/16 20:43:14  anonymous
|      Removed MFC dependencies
|
|      Revision 1.6  2000/01/08 15:51:30  Ulrich von Zadow
|      Misc. modifications to png encoder.
|
|      Revision 1.5  1999/11/27 18:45:48  Ulrich von Zadow
|      Added/Updated doc comments.
|
|      Revision 1.4  1999/10/19 21:28:05  Ulrich von Zadow
|      Added jpeg encoder
|
|      Revision 1.3  1999/10/03 18:50:50  Ulrich von Zadow
|      Added automatic logging of changes.
|
|
\--------------------------------------------------------------------
*/
