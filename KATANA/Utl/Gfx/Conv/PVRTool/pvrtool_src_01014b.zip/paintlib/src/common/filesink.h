/*
/--------------------------------------------------------------------
|
|      $Id: filesink.h,v 1.4 2000/01/16 20:43:13 anonymous Exp $
|
|      Copyright (c) 1996-1998 Ulrich von Zadow
|
\--------------------------------------------------------------------
*/
// not quite ready for prime-time; bdelmee; 2/99

#ifndef INCL_FILESNK
#define INCL_FILESNK

#ifndef INCL_DATASNK
#include "datasink.h"
#endif

//! This is a class which takes a file as a destination of picture data.
//!
//! Restriction: needs to allocate a buffer large enough to hold a complete
//! image file. But we don't know its size before it's actually encoded, so
//! we have to be generous or "accidents will happen"
//! A better implementation would rely on memory-mapped files or implement
//! dynamic buffering.
class CFileSink : public CDataSink
{

public:
  //!
  CFileSink
    ();

  //!
  virtual ~CFileSink
    ();

  //!
  virtual int Open
    ( const char * pszFName,
      int MaxFileSize
    );

  //!
  virtual void Close
    ();

private:
  FILE * m_pFile;
  BYTE * m_pDataBuf;
};

#endif

/*
/--------------------------------------------------------------------
|
|      $Log: filesink.h,v $
|      Revision 1.4  2000/01/16 20:43:13  anonymous
|      Removed MFC dependencies
|
|      Revision 1.3  2000/01/08 15:56:12  Ulrich von Zadow
|      Made sure change logging works in every file.
|
|
\--------------------------------------------------------------------
*/
