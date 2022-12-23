/*
	This file is part of ObjEdit2, Copyright 1997 Interactive Studios Ltd

	File        : crc32.cpp
	Description : CRC generating code
	Author      : R. Hackett
	Date        : 15.02.97
*/
#include "main.h"
#ifndef DOSVER
	#include "stdafx.h"
#else
	#include <string.h>
#endif

#include "crc32.h"

#ifndef DOSVER
	#ifdef _DEBUG
		#define new DEBUG_NEW
		#undef THIS_FILE
		static char THIS_FILE[] = __FILE__;
	#endif
#endif

#define POLYNOMIAL 0x04c11db7L

static unsigned long CRCtable[256];


/*	--------------------------------------------------------------------------------
	Function : InitCRCTable
	Purpose : Initialise the CRC table
	Parameters : 
	Returns : 
	Info : 
*/

void InitCRCTable()
{
	register int i, j;
	register unsigned long CRCaccum;

	for (i=0; i<256; i++)
		{
		CRCaccum = ((unsigned long)i<<24);
		for (j=0; j<8; j++)
			{
			if (CRCaccum & 0x80000000L)
				CRCaccum = (CRCaccum<<1)^POLYNOMIAL;
			else
				CRCaccum = (CRCaccum<<1);
			}
		CRCtable[i] = CRCaccum;
		}
}


/*	--------------------------------------------------------------------------------
	Function : UpdateCRC
	Purpose : update a CRC for specified data
	Parameters : existing CRC, data ptr, size of data in bytes
	Returns : 
	Info : 
*/

unsigned long UpdateCRC(char *ptr)
{
	register int i, j;
	int size = strlen(ptr);
	unsigned long CRCaccum = 0;

	for (j=0; j<size; j++)
		{
		i = ((int)(CRCaccum>>24)^(*ptr++))&0xff;
		CRCaccum = (CRCaccum<<8)^CRCtable[i];
		}
	return CRCaccum;
}
