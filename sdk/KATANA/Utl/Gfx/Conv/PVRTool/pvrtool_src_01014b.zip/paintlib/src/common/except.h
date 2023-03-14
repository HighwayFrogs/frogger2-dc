/*
/--------------------------------------------------------------------
|
|      $Id: except.h,v 1.4 2000/01/17 23:38:07 Ulrich von Zadow Exp $
|
|      Copyright (c) 1996-1998 Ulrich von Zadow
|
\--------------------------------------------------------------------
*/

#ifndef INCL_EXCEPT
#define INCL_EXCEPT


// Error codes
#define ERR_OK              0            // No error
#define ERR_WRONG_SIGNATURE 1            // Expected file signature
                                         // not found.
#define ERR_FORMAT_UNKNOWN  2            // Sanity check failed.

#define ERR_PATH_NOT_FOUND  3            // <--
#define ERR_FILE_NOT_FOUND  4            // <-- Problems with the file
#define ERR_ACCESS_DENIED   5            // <-- system.

#define ERR_FORMAT_NOT_SUPPORTED 6       // Known but unsupported
                                         // format.
#define ERR_INTERNAL        7            // Internal error of some sort.
#define ERR_UNKNOWN_FILE_TYPE    8       // Couldn't recognize the
                                         // file type.
#define ERR_DIB_TOO_LARGE   9            // Maximum size for 1 bmp was
                                         // exceeded.
#define ERR_NO_MEMORY      10            // Out of memory.
#define ERR_END_OF_FILE    11            // End of file reached before
                                         // end of image.

//! An object of this class is thrown by other classes when an error
//! occurs. It contains an error code and a string describing the
//! error. The error code is meant to be used internally in the
//! program; the descriptive string can be output to the user. Error
//! codes and strings do not correspond 1:1. The strings are more
//! precise.
class CTextException : public CPLObject
{

public:
  //! Creates an exception.
  CTextException
    ( int Code,
      const char * pszErr
    );

  //! Copy constructor.
  CTextException
    ( const CTextException& ex
    );

  //! Destroys an exception
  virtual ~CTextException
    ();

  //! Returns the code of the error that caused the exception. Valid
  //! error codes are:
  //!
  //! ERR_WRONG_SIGNATURE (1): Expected file signature not found.
  //!
  //! ERR_FORMAT_UNKNOWN (2): Unexpected data encountered. This
  //! probably indicates a corrupt file or an unknown file
  //! sub-format.
  //!
  //! ERR_PATH_NOT_FOUND (3), ERR_FILE_NOT_FOUND (4),
  //! ERR_ACCESS_DENIED (5): Problems with the file system.
  //!
  //! ERR_FORMAT_NOT_SUPPORTED (6): Known but unsupported format.
  //!
  //! ERR_INTERNAL (7): Kaputt. Tell me about it.
  //!
  //! ERR_UNKNOWN_FILE_TYPE (8): Couldn't recognize the file type.
  //!
  //! ERR_DIB_TOO_LARGE (9): Maximum size for 1 bmp was exceeded.
  //! (See MAX_BITMAP_SIZE above for an explanation).
  //!
  //! ERR_NO_MEMORY (10): Out of memory.
  //!
  //! ERR_END_OF_FILE (11): End of file reached before end of image.
  virtual int GetCode
    ()
    const;


  //! This operator allows the exception to be treated as a string
  //! whenever needed. The string contains the error message.
  virtual operator const char *
    ()
    const;

private:
  int     m_Code;
  char *  m_pszErr;
};

#endif
/*
/--------------------------------------------------------------------
|
|      $Log: except.h,v $
|      Revision 1.4  2000/01/17 23:38:07  Ulrich von Zadow
|      MFC removal aftermath.
|
|
\--------------------------------------------------------------------
*/
