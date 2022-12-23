
#include "main.h"

#ifndef DOSVER
	#include "stdafx.h"
#else
	#include <stdarg.h>
	#include <stdio.h>
#endif

#include "debug.h"

#ifndef DOSVER
	#ifdef _DEBUG
		#define new DEBUG_NEW
		#undef THIS_FILE
		static char THIS_FILE[] = __FILE__;
	#endif
#endif

void dprintf( char *format, ... )
{
	char		text[MAX_DEBUG_STRING_LEN];
	va_list		argp;

	va_start( argp, format );
	vsprintf( text, format, argp );
	va_end( argp );

#ifdef DOSVER
	fprintf(stderr, text);
#else
	#ifdef _DEBUG			
		_CrtDbgReport( _CRT_WARN, NULL, NULL, "WGS", "CHOP95: %s ", text );
	#endif
#endif
}
