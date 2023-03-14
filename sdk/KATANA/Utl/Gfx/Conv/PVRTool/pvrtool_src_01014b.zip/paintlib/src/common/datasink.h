/*
/--------------------------------------------------------------------
|
|      $Id: datasink.h,v 1.5 2000/01/16 20:43:13 anonymous Exp $
|
|      Copyright (c) 1996-1998 Ulrich von Zadow
|
\--------------------------------------------------------------------
*/
// not quite ready for prime-time; bdelmee; 2/99

#ifndef INCL_DATASNK
#define INCL_DATASNK

#include "except.h"

//! This is a base class for a destination of picture data.
//! It defines methods to open, write to, close data sinks.
//!
//! Restriction: actually writes to a memory buffer that needs
//! to be foreseen large enough by user (descendant class)
class CDataSink : public CPLObject
{
  // I'd like to get rid of this, but now it is needed so the encoders
  // can maintain the actually useful length of the buffer
  friend class CTIFFEncoder;
  friend class CTIFFEncoderEx;

public:
  //!
  void Open
    ( const char* pszName,
      BYTE*   pData,
      size_t  MaxDataSize
    );

  //!
  virtual void Close
    ();

  //!
  char* GetName
    ();

  //!
  BYTE* GetBufferPtr
    ();

  //!
  size_t GetDataSize
    ();

  //!
  size_t GetMaxDataSize
    ();

  //!
  size_t WriteNBytes
    ( size_t   n,
	  BYTE* pData
    );

  //!
  void Skip
    ( size_t n );

  //! Test to see if we didn't go past the "end of the file"
  void CheckEOF
    ();

protected:
  BYTE* m_pStartData;
  int   m_nCurPos;   // this SHOULD be an unsigned int (size_t), but TIFFLib wants an int

  //!
  CDataSink
    ();

  //!
  virtual ~CDataSink
   ();

private:
  char*  m_pszName;    // Name of the data source for diagnostic purposes
  size_t m_nMaxFileSize;
};


inline BYTE * CDataSink::GetBufferPtr
    ()
{
	return m_pStartData + m_nCurPos;
}

inline size_t CDataSink::GetMaxDataSize
    ()
{
  return  m_nMaxFileSize;
}

inline size_t CDataSink::GetDataSize
    ()
{
  // return currently used buffer size
  return m_nCurPos;
}

inline void CDataSink::CheckEOF
    ()
{
  //if (GetFileSize() > GetMaxFileSize())
  if (m_nCurPos > (int)m_nMaxFileSize)
  {
    throw CTextException (ERR_END_OF_FILE,
          "Buffer overflow while encoding.\n");
  }
}

inline void CDataSink::Skip
    ( size_t n )
{
	// maybe we should fill the gap with nulls
  m_nCurPos += n;
  CheckEOF ();
}

#endif	// INCL_DATASNK
/*
/--------------------------------------------------------------------
|
|      $Log: datasink.h,v $
|      Revision 1.5  2000/01/16 20:43:13  anonymous
|      Removed MFC dependencies
|
|      Revision 1.4  2000/01/08 15:56:12  Ulrich von Zadow
|      Made sure change logging works in every file.
|
|
\--------------------------------------------------------------------
*/
