/*
/--------------------------------------------------------------------
|
|      $id$
|
|      Copyright (c) 1996-1998 Ulrich von Zadow
|
--------------------------------------------------------------------
*/

#ifndef INCL_DATASRC
#define INCL_DATASRC

#include "except.h"

class IProgressNotification;

//! This is a base class for a source of picture data.
//! It defines methods to open, close, and read from data sources.
//! Does byte-order-conversions in the ReadByte, ReadWord, and
//! ReadLong routines.
class CDataSource : public CPLObject
{

public:

  //!
  CDataSource
    ( IProgressNotification * pNotification = NULL
    );

  //!
  virtual ~CDataSource
    ();

  //!
  virtual void Open
    ( const char * pszName,
      int    FileSize
    );

  //!
  virtual void Close
    ();

  //!
  char * GetName
    ();

  //! Read but don't advance file pointer.
  virtual BYTE * GetBufferPtr
    ( int MinBytesInBuffer
    ) = 0;

  //! This needs to be overridden in derived classes.
  virtual BYTE * ReadNBytes
    ( int n
    );

  //!
  int GetFileSize
    ();

  //! This is a legacy routine that interferes with progress notifications.
  //! Don't call it!
  virtual BYTE * ReadEverything
    () = 0;

  //!
  BYTE * Read1Byte
    ();

  //!
  BYTE * Read2Bytes
    ();

  //!
  BYTE * Read4Bytes
    ();

  // JH 15.4.99:
  //! handles progress notification from other libs
  void OProgressNotification
    ( double part
    );

  //!
  void AlignToWord
    ();

  //!
  void Skip
    ( int n
    );

  //! Test to see if we didn't go past the end of the file
  void CheckEOF
    ();

protected:

private:
  char * m_pszName;        // Name of the data source for diagnostic
                           // purposes.
  int    m_FileSize;
  int    m_BytesRead;
  BOOL   m_bSrcLSBFirst;   // Source byte order: TRUE for intel order,
                           // FALSE for Motorola et al. (MSB first).
  IProgressNotification * m_pNotification;
};


inline int CDataSource::GetFileSize
    ()
{
  return m_FileSize;
}

inline BYTE * CDataSource::Read1Byte
    ()
{
  return ReadNBytes (1);
}


inline BYTE * CDataSource::Read2Bytes
    ()
{
  return ReadNBytes (2);
}


inline BYTE * CDataSource::Read4Bytes
    ()
{
  return ReadNBytes (4);
}

inline void CDataSource::AlignToWord
    ()
{
  if (m_BytesRead & 1)
    Read1Byte();
}


inline void CDataSource::Skip
    ( int n
    )
{
  ReadNBytes (n);
}

inline void CDataSource::CheckEOF
    ()
{

  if (m_FileSize < m_BytesRead)
  {
    throw CTextException (ERR_END_OF_FILE,
          "End of file reached while decoding.\n");
  }
}

#endif
/*
/--------------------------------------------------------------------
|
|      $log$
|
--------------------------------------------------------------------
*/
