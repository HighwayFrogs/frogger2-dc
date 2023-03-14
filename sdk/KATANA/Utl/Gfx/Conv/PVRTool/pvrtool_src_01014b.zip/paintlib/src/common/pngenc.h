/*
/--------------------------------------------------------------------
|
|      $Id: pngenc.h,v 1.3 2000/01/16 20:43:14 anonymous Exp $
|
|      Copyright (c) 1996-1998 Ulrich von Zadow
|
\--------------------------------------------------------------------
*/

#ifndef INCL_PNGENC
#define INCL_PNGENC

#ifndef INCL_PICENC
#include "picenc.h"
#endif

//! PNG file encoder. Uses LIBPNG to do the actual conversion.
class CPNGEncoder : public CPicEncoder
{

public:
  //! Creates an encoder
  CPNGEncoder
    ();

  //! Destroys a encoder
  virtual ~CPNGEncoder
    ();

  CDataSink * GetDataSink
    ();

protected:

  //! Sets up LIBPNG environment and calls LIBPNG to encode an image.
  virtual void DoEncode
    ( CBmp* pBmp,
      CDataSink* pDataSnk
    );

private:
	CBmp * m_pBmp;
	CDataSink* m_pDataSnk;

};


#endif  // INCL_PNGENC
/*
/--------------------------------------------------------------------
|
|      $Log: pngenc.h,v $
|      Revision 1.3  2000/01/16 20:43:14  anonymous
|      Removed MFC dependencies
|
|      Revision 1.2  2000/01/08 15:53:12  Ulrich von Zadow
|      Moved several functions to the cpp file so applications don't
|      need the png directory in the include path.
|
|      Revision 1.1  2000/01/04 22:06:17  Ulrich von Zadow
|      Initial version by Neville Richards.
|
|
\--------------------------------------------------------------------
*/
