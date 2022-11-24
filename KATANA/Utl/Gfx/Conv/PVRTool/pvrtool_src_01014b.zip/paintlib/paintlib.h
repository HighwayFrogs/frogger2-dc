#ifndef _PAINTLIB_H
#define _PAINTLIB_H

#ifdef _WINDOWS
    #include "src/common/stdpch.h"
    #include "src/common/anydec.h"
    #include "src/common/anybmp.h"
#else
    #define _WINDOWS
    #include "src/common/stdpch.h"
    #include "src/common/anydec.h"
    #include "src/common/anybmp.h"
    #undef _WINDOWS
#endif


#endif //_PAINTLIB_H
