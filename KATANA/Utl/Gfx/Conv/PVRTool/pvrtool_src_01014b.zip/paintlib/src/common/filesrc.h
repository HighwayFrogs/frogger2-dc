/*
/--------------------------------------------------------------------
|
|      $id$
|
|      Copyright (c) 1996-1998 Ulrich von Zadow
|
--------------------------------------------------------------------
*/

#ifndef INCL_FILESRC
#define INCL_FILESRC

#ifndef INCL_DATASRC
#include "datasrc.h"
#endif

#ifdef _WINDOWS
#define FILE_MAPPING
#endif

class IProgressNotification;

//! This is a class which takes a file as a source of picture data.
class CFileSource : public CDataSource
{

public:
  //!
  CFileSource
    ( IProgressNotification * pNotification = NULL
    );

  //!
  virtual ~CFileSource
    ();

  //!
  virtual int Open
    ( const char * pszFName
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

  virtual BYTE * ReadEverything
    ();

private:
#ifdef FILE_MAPPING
  HANDLE m_hf;    // File handle.
  HANDLE m_hm;    // Handle to file-mapping object.

#else
  BOOL bytesAvailable
    ( int n
    );

  void fillBuffer
    ();

  FILE * m_pFile;
  BYTE * m_pBuffer;

  BYTE * m_pReadPos;
  int m_BytesReadFromFile;
#endif
  BYTE * m_pStartData;
  BYTE * m_pCurPos;
};

#endif
/*
/--------------------------------------------------------------------
|
|      $log$
|
--------------------------------------------------------------------
*/
