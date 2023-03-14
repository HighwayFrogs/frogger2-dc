/*
/--------------------------------------------------------------------
|
|      $Id: plpoint.h,v 1.2 2000/01/10 23:52:59 Ulrich von Zadow Exp $
|
|      Copyright (c) 1996-1998 Ulrich von Zadow
|
\--------------------------------------------------------------------
*/

#ifndef INCL_PLPOINT
#define INCL_PLPOINT

#ifdef _MFC_VER
#define CPLPoint CPoint
#else

//! Simple point class for 2d coordinate manipulations. This class
//! contains only inline functions and nothing virtual. Its member variables
//! are public.<br>
//! In effect, this class emulates most of MFC's point class for non-MFC
//! compiles. For MFC compiles, we have a #define CPLPoint CPoint.
class CPLPoint
{
public:
  int x;
  int y;

  //!
  CPLPoint
  ();

  //!
  CPLPoint
  ( int X,
    int Y
  );

  //!
  BOOL operator ==
  ( CPLPoint pt
  ) const;

  //!
  BOOL operator !=
  ( CPLPoint pt
  ) const;

  //!
  void operator +=
  ( CPLPoint pt
  );

  //!
  void operator -=
  ( CPLPoint pt
  );

  //!
  CPLPoint operator -
  () const;

  //!
  CPLPoint operator +
  ( CPLPoint pt
  ) const;
};

inline CPLPoint::CPLPoint
()
{}


inline CPLPoint::CPLPoint
( int X,
  int Y
)
{
  x = X;
  y = Y;
}

inline BOOL CPLPoint::operator ==
( CPLPoint pt
) const
{
  return (x == pt.x && y == pt.y);
}

inline BOOL CPLPoint::operator !=
( CPLPoint pt
) const
{
  return (x != pt.x || y != pt.y);
}

inline void CPLPoint::operator +=
( CPLPoint pt
)
{
  x += pt.x;
  y += pt.y;
}

inline void CPLPoint::operator -=
( CPLPoint pt
)
{
  x -= pt.x;
  y -= pt.y;
}

inline CPLPoint CPLPoint::operator -
() const
{
  return CPLPoint(-x, -y);
}

inline CPLPoint CPLPoint::operator +
( CPLPoint pt
) const
{
  return CPLPoint(x + pt.x, y + pt.y);
}

#endif  // _MFC_VER

#endif

/*
/--------------------------------------------------------------------
|
|      $Log: plpoint.h,v $
|      Revision 1.2  2000/01/10 23:52:59  Ulrich von Zadow
|      Changed formatting & removed tabs.
|
|      Revision 1.1  1999/12/09 16:35:58  Ulrich von Zadow
|      Added CPLPoint.
|
|
\--------------------------------------------------------------------
*/
