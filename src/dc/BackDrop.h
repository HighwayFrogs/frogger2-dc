/*
	This file is part of Frogger II, Copyright 2000 Interactive Studios Ltd

	File        : backdrop.h
	Description : Backdrop routines and structures
	Date        : 09/03/2000
	Version     : 0.01
*/

#include "include.h"

#ifndef _BACKDROP_H_INCLUDE
#define _BACKDROP_H_INCLUDE

// ------------------
// Constants & Macros

#define BACKDROP_WIDTH	639
#define BACKDROP_HEIGHT	479

// --------------------
// Structures & Classes

typedef struct _BACKDROP
{
	RECT	 		rect;
	char	 		old_red;
	char 			init;
	char 			draw;
	INT				imageXD,imageYD;
	KMSURFACEDESC	surface;
} BACKDROP;

// -------
// Globals

extern BACKDROP 		backDrop;
extern KMSTRIPCONTEXT	backStripContext;
extern KMSTRIPHEAD		backStripHead;
extern KMVERTEX_03		backdropVertices[];

extern BACKDROP 		artBackDrop;
extern KMSTRIPCONTEXT	artBackStripContext;
extern KMSTRIPHEAD		artBackStripHead;
extern KMVERTEX_03		artBackdropVertices[];

// ----------
// Prototypes

/*	--------------------------------------------------------------------------------
	Function 	: InitBackdrop
	Purpose 	: Load and setup a background imageg
	Parameters 	: filename
	Returns 	: none
	Info 		:
*/

void InitBackdrop(char * const filename);

/*	--------------------------------------------------------------------------------
	Function 	: DrawBackDrop
	Purpose 	: Draw the current background image to the screen
	Parameters 	: none
	Returns 	: none
	Info 		:
*/

void DrawBackDrop(int num, int i);

/*	--------------------------------------------------------------------------------
	Function 	: FreeBackdrop
	Purpose 	: Free the current background image
	Parameters 	: none
	Returns 	: none
	Info 		:
*/

void FreeBackdrop(void);


// *ASL* 14/08/2000
/* --------------------------------------------------------------------------------
   Function : DrawLegalBackDrop
   Purpose : draw the currently loaded background image to the screen
   Parameters : 
   Returns : 
   Info :
*/

void DrawLegalBackDrop();


/* --------------------------------------------------------------------------------
   Function : FreeLegalBackdrop
   Purpose : free the currently loaded background image
   Parameters : 
   Returns : 
   Info :
*/

void FreeLegalBackdrop();

#endif