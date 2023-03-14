/*
/--------------------------------------------------------------------
|
|      $Id: ressrc.h,v 1.5 2000/01/16 20:43:18 anonymous Exp $
|
|      Copyright (c) 1996-1998 Ulrich von Zadow
|
\--------------------------------------------------------------------
*/

#ifndef INCL_RESSRC
#define INCL_RESSRC

#ifndef INCL_DATASRC
#include "datasrc.h"
#endif

//! This is a class which takes a windows resource as a source of
//! picture data.
class CResourceSource : public CDataSource
{
public:
  //!
  CResourceSource
    ();

  //!
  virtual ~CResourceSource
    ();

  //!
  virtual int Open
    ( HINSTANCE lh_ResInst, int ResourceID,
      const char * pResType = NULL
    );

  //!
  virtual void Close
    ();

  virtual BYTE * ReadNBytes
    ( int n
    );

  //! Read but don't advance file pointer.
  virtual BYTE * GetBufferPtr
    ( int MinBytesInBuffer
    );

  //! This is a legacy routine that interferes with progress notifications.
  //! Don't call it!
  virtual BYTE * ReadEverything
    ();

private:
  HRSRC   m_hRsrc;
  HGLOBAL m_hGlobal;
  BYTE * m_pCurPos;
};

#endif
/*
/--------------------------------------------------------------------
|
|      $Log: ressrc.h,v $
|      Revision 1.5  2000/01/16 20:43:18  anonymous
|      Removed MFC dependencies
|
|      Revision 1.4  2000/01/11 22:07:11  Ulrich von Zadow
|      Added instance handle parameter.
|
|      Revision 1.3  1999/11/02 21:20:14  Ulrich von Zadow
|      AfxFindResourceHandle statt AfxGetInstanceHandle
|      verwendet.
|
|
\--------------------------------------------------------------------
*/
