/* Error.c */

#include "main.h"
//#ifndef DOSCHOP
#ifndef DOSVER
#include "stdafx.h"
#endif

#include <stdio.h>
#include <stdlib.h>

#include "debug.h"
#include "mytypes.h"
#include "error.h"

#ifndef DOSCHOP
	#ifdef _DEBUG
		#define new DEBUG_NEW
		#undef THIS_FILE
		static char THIS_FILE[] = __FILE__;
	#endif
#endif

void ERR_TellThemOff(int bFatal, char *sErr)
{
	char sTmp[256];

	if(bFatal)
	{
		sprintf(sTmp, "FATAL ERROR: %s\n", sErr);
#ifndef DOSVER
		AfxMessageBox(sTmp, MB_OK, 0);
#else
		fprintf(stderr, sTmp);
#endif
		exit(1);
	}
	else
	{
		sprintf(sTmp, "WARNING: %s\n", sErr);
#ifndef DOSVER
		AfxMessageBox(sTmp, MB_OK, 0);
#else
		fprintf(stderr, sTmp);
#endif
	}
}
