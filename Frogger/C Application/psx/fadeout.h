/*

	This file is part of Frogger2, (c) 1999 Interactive Studios Ltd.

	File		: fadeout.h
	Programmer	: David
	Date		: 
	Purpose		: Transition effects 'n shit

----------------------------------------------------------------------------------------------- */

#ifndef _FADEOUT_H
#define _FADEOUT_H

#include "include.h"

extern int (*fadeProc)(void);

extern int fadingOut, keepFade;
extern int fadeText;
extern int flashScreen;

extern KMSURFACEDESC	fadeSurface;
extern KMSTRIPCONTEXT	fadeStripContext;
extern KMSTRIPHEAD		fadeStripHead;
extern KMVERTEX_00		fadeVertices[];

void ScreenFade(int start, int end, long time);
void DrawScreenTransition(void);
void BlankScreen(void);

#endif