/*
/--------------------------------------------------------------------
|
|      $Id: 2PSFilters.h,v 1.2 1999/12/02 17:07:34 Ulrich von Zadow Exp $
|
|      Copyright (c) 1996-1998 Ulrich von Zadow
|
\--------------------------------------------------------------------
*/

#ifndef _FILTERS_H_
#define _FILTERS_H_

class CGenericFilter
{
public:
    
    CGenericFilter (double dWidth) : m_dWidth (dWidth) {}
    virtual ~CGenericFilter() {}

    double GetWidth()                   { return m_dWidth; }
    void   SetWidth (double dWidth)     { m_dWidth = dWidth; }

    virtual double Filter (double dVal) = 0;

protected:

    #define FILTER_PI  double (3.1415926535897932384626433832795)
    #define FILTER_2PI double (2.0 * 3.1415926535897932384626433832795)
    #define FILTER_4PI double (4.0 * 3.1415926535897932384626433832795)

    double  m_dWidth;
};

class CBoxFilter : public CGenericFilter
{
public:

    CBoxFilter (double dWidth = 0.5) : CGenericFilter(dWidth) {}
    virtual ~CBoxFilter() {}

    virtual double Filter (double dVal) { return (fabs(dVal) <= m_dWidth ? 1.0 : 0.0); }
};

class CBilinearFilter : public CGenericFilter
{
public:

    CBilinearFilter (double dWidth = 1.0) : CGenericFilter(dWidth) {}
    virtual ~CBilinearFilter() {}

    virtual double Filter (double dVal) 
        {
            dVal = fabs(dVal); 
            return (dVal < m_dWidth ? m_dWidth - dVal : 0.0); 
        }
};

class CGaussianFilter : public CGenericFilter
{
public:

    CGaussianFilter (double dWidth = 3.0) : CGenericFilter(dWidth) {}
    virtual ~CGaussianFilter() {}

    virtual double Filter (double dVal) 
        {
            if (fabs (dVal) > m_dWidth) 
            {
                return 0.0;
            }
            return exp (-dVal * dVal / m_dWidth-1) / sqrt (FILTER_2PI); 
        }
};

class CHammingFilter : public CGenericFilter
{
public:

    CHammingFilter (double dWidth = 0.5) : CGenericFilter(dWidth) {}
    virtual ~CHammingFilter() {}

    virtual double Filter (double dVal) 
        {
            if (fabs (dVal) > m_dWidth) 
            {
                return 0.0; 
            }
            double dWindow = 0.54 + 0.46 * cos (FILTER_2PI * dVal); 
            double dSinc = (dVal == 0) ? 1.0 : sin (FILTER_PI * dVal) / (FILTER_PI * dVal); 
            return dWindow * dSinc;
        }
};

class CBlackmanFilter : public CGenericFilter
{
public:

    CBlackmanFilter (double dWidth = 0.5) : CGenericFilter(dWidth) {}
    virtual ~CBlackmanFilter() {}

    virtual double Filter (double dVal) 
        {
            if (fabs (dVal) > m_dWidth) 
            {
                return 0.0; 
            }
            double dN = 2.0 * m_dWidth + 1.0; 
            return 0.42 + 0.5 * cos (FILTER_2PI * dVal / ( dN - 1.0 )) + 
                   0.08 * cos (FILTER_4PI * dVal / ( dN - 1.0 )); 
        }
};
 
 
#endif  // _FILTERS_H_

/*
/--------------------------------------------------------------------
|
|      $Log: 2PSFilters.h,v $
|      Revision 1.2  1999/12/02 17:07:34  Ulrich von Zadow
|      Changes by bdelmee.
|
|      Revision 1.1  1999/10/21 16:05:17  Ulrich von Zadow
|      Moved filters to separate directory. Added Crop, Grayscale and
|      GetAlpha filters.
|
|      Revision 1.1  1999/10/19 21:29:55  Ulrich von Zadow
|      Added filters.
|
|
\--------------------------------------------------------------------
*/
