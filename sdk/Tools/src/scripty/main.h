#pragma once
#define nullptr 0

// Platform
#ifdef WIN32
#define PLATFORM_WINDOWS
#endif
#ifdef WIN64
#define PLATFORM_WINDOWS
#endif

#ifdef PLATFORM_WINDOWS
#define strcasecmp _stricmp
#endif

#ifdef _DEBUG
#define debug_printf(format, ...) printf("[DEBUG] "format, __VA_ARGS__)
#else
#define debug_printf(format, ...)
#endif

// Enables memory leak profiling.
#ifdef _DEBUG
#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>
#endif


#define nameof(identifier) #identifier