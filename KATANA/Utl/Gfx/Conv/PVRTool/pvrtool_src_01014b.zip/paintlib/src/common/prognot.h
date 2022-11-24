/*
/--------------------------------------------------------------------
|
|      $id$
|
|      Copyright (c) 1996-1998 Ulrich von Zadow
|
--------------------------------------------------------------------
*/

#ifndef INCL_PROGNOT
#define INCL_PROGNOT

//! Defines an interface for progress notifications.
class IProgressNotification
{
public:
  //! Called during decoding as progress gets made.
  virtual void OnProgress 
    ( double Part
    ) = 0;
};

#endif
/*
/--------------------------------------------------------------------
|
|      $log$
|
--------------------------------------------------------------------
*/
