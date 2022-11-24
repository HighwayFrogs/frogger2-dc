/*
/--------------------------------------------------------------------
|
|      $Id: stdpch.h,v 1.6 2000/01/16 20:43:14 anonymous Exp $
|
|      Copyright (c) 1996-1998 Ulrich von Zadow
|
\--------------------------------------------------------------------
*/

//! Contains most system-specific includes and definitions. On windows
//! systems, it corresponds to stdafx.h. On other systems, the
//! appropriate data types and macros are declared here.
//!
//! This file defines the basic data types (BOOL, BYTE, WORD,...)

#include "config.h"
#include "PLObject.h"

#ifdef _WINDOWS
  #define VC_EXTRALEAN  // Exclude rarely-used stuff from Windows headers
  #include <windows.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "debug.h" 

#ifndef _WINDOWS
  // Basic definitions for non-windows version.
  #include <stdarg.h>

  typedef int                 BOOL;
  typedef unsigned char       BYTE;
  typedef unsigned short      WORD;         // This is 16 bit.
  typedef long                LONG;         // This is 32 bit.
  typedef unsigned long       ULONG;

  #ifndef FALSE
  #define FALSE   0
  #endif

  #ifndef TRUE
  #define TRUE    1
  #endif

  #ifndef NULL
  #define NULL    0
  #endif

  #define HIBYTE(w)   ((BYTE) (((WORD) (w) >> 8) & 0xFF))
#endif

/*
/--------------------------------------------------------------------
|
|      $Log: stdpch.h,v $
|      Revision 1.6  2000/01/16 20:43:14  anonymous
|      Removed MFC dependencies
|
|      Revision 1.5  1999/12/08 15:43:58  Ulrich von Zadow
|      Changed ASSERT and PLASSERT_VALID so that they
|      disappear correctly in release mode.
|
|      Revision 1.4  1999/11/22 14:59:37  Ulrich von Zadow
|      no message
|
|      Revision 1.3  1999/10/03 18:50:52  Ulrich von Zadow
|      Added automatic logging of changes.
|
|
\--------------------------------------------------------------------
*/
