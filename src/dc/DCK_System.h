/*
	(c) 2000 Interactive Studios Ltd.

	File		: DCK_System.h
	Programmer	: Andy Eder
	Created		: 19/01/00

	Updated		: 19/01/00	Andy Eder
				: < revision dates here >

----------------------------------------------------------------------------------------------- */

#ifndef DCK_SYSTEM_H_INCLUDED
#define DCK_SYSTEM_H_INCLUDED

#include "include.h"

//----- [ GLOBALS ] -----------------------------------------------------------------------------

extern KMSTATUS 			kmStatus;					// status of KAMUI2 API
extern KMVERSIONINFO		kmVersionInfo;				// KAMUI2 version information
extern KMSYSTEMCONFIGSTRUCT	kmSystemConfig;				// KAMUI2 system configuration

extern KMSURFACEDESC 		primarySurfaceDesc;			// primary surface
extern KMSURFACEDESC 		renderSurfaceDesc;			// render surface (back buffer)
extern KMSURFACEDESC 		textureSurfaceDesc;			// texture surface

extern PKMSURFACEDESC 		pFB[2];						// ptrs to our framebuffers

extern KMVERTEXBUFFDESC 	vertexBufferDesc;			// global vertex buffer

extern PKMDWORD				dwDataPtr;

extern Sint32				displayMode, frameBufferFormat;

extern int					firstPad;
extern int					numPads;
extern int					padStatus[4];

//----- [ FUNCTION PROTOTYPES ] -----------------------------------------------------------------

/*	--------------------------------------------------------------------------------
	Function 	: my_memcopy
	Purpose 	: 
	Parameters 	: 
	Returns 	: 
	Info 		:
*/

void my_memcopy(char *source, char *dest, int size);

/*	--------------------------------------------------------------------------------
	Function 	: getShort
	Purpose 	: 
	Parameters 	: 
	Returns 	: 
	Info 		:
*/

short getShort(char *ptr);

/*	--------------------------------------------------------------------------------
	Function 	: getInt
	Purpose 	: 
	Parameters 	: 
	Returns 	: 
	Info 		:
*/

int	getInt(char *ptr);

/*	--------------------------------------------------------------------------------
	Function 	: getFloat
	Purpose 	: 
	Parameters 	: 
	Returns 	: 
	Info 		:
*/

float getFloat(char *ptr);

int checkForControllerRemovedSingle();
int checkForControllerInsertedSingle();
int checkForControllerRemovedMulti();
int checkForControllerInsertedMulti();
int checkForSoftReset();
int checkForValidControllers();


// *ASL* 12/08/2000
/* --------------------------------------------------------------------------------
   Function : initCheckForSoftReset
   Purpose : initialise the soft reset check (!)
   Parameters : 
   Returns : 
   Info :
*/

void initCheckForSoftReset();

#endif
