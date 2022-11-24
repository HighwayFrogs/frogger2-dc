/*
/--------------------------------------------------------------------
|
|      $Id: debug.h,v 1.1 2000/01/17 23:45:07 Ulrich von Zadow Exp $
|
|      Plattform-independent support for PLASSERT_VALID, PLTRACE and
|      PLASSERT.
|
|      Copyright (c) 1996-1998 Ulrich von Zadow
|
\--------------------------------------------------------------------
*/

#ifndef INCL_DEBUG
#define INCL_DEBUG

//------------- PLASSERT_VALID
#ifdef _DEBUG
  #define PLASSERT_VALID(pOb) (pOb)->AssertValid()
#else
  #define PLASSERT_VALID(pOb) do{} while (0)
#endif

//------------- TRACE
#ifdef _DEBUG
  void PLTrace(const char * pszFormat, ...);
  #define PLTRACE ::PLTrace
#else
  // This will be optimized away in release mode and still allow TRACE
  // to take a variable amount of arguments :-).
  inline void PLTrace (const char *, ...) { }
  #define PLTRACE  1 ? (void)0 : ::PLTrace
#endif

//------------- ASSERT

#ifdef _DEBUG
  #define PLASSERT(f)            \
    if (!(f))                \
      {                      \
        PLTRACE ("Assertion failed at %s, %i", __FILE__, __LINE__); \
        abort();             \
      }
#else
  #define PLASSERT(f) do{}while (0)
#endif

#endif // INCL_DEBUG

/*
/--------------------------------------------------------------------
|
|      $Log: debug.h,v $
|      Revision 1.1  2000/01/17 23:45:07  Ulrich von Zadow
|      MFC-Free version.
|
|
\--------------------------------------------------------------------
*/
