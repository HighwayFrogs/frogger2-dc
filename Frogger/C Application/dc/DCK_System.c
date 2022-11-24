/*
	(c) 2000 Interactive Studios Ltd.

	File		: DCK_System.c
	Programmer	: Andy Eder
	Created		: 19/01/00

	Updated		: 19/01/00	Andy Eder
				: < revision dates here >

----------------------------------------------------------------------------------------------- */

#include "include.h"
#include "main.h"

extern DCKVERTEX 		*globalVerts;
extern KMSTRIPCONTEXT 	StripContext;
extern int				matrixSize;
extern DCKMATRIX		matrixStack[];
extern DCKMATRIX		viewMatrix;
extern KMSTRIPHEAD 		StripHead;
extern DCKVECTOR		camPos;
extern DCKVECTOR		camTarg;

//----- [ GLOBALS ] -----------------------------------------------------------------------------

KMSTATUS 				kmStatus;			// status of KAMUI2 API
KMVERSIONINFO			kmVersionInfo;		// KAMUI2 version information
KMSYSTEMCONFIGSTRUCT	kmSystemConfig;		// KAMUI2 system configuration

KMSURFACEDESC 	primarySurfaceDesc;			// primary surface
KMSURFACEDESC 	renderSurfaceDesc;			// render surface (back buffer)
KMSURFACEDESC 	textureSurfaceDesc;			// texture surface

PKMSURFACEDESC 	pFB[2] = { NULL, NULL };	// ptrs to our framebuffers

KMVERTEXBUFFDESC vertexBufferDesc;			// global vertex buffer

PKMDWORD 		dwDataPtr;

Sint32			displayMode, frameBufferFormat;

enum
{
	PADSTATUS_NOTCONNECTED,
	PADSTATUS_CONNECTED,
	PADSTATUS_REMOVED,
	PADSTATUS_INSERTED,
};

int				firstPad = 0;
int				numPads = 0;
int				padStatus[4] = {0,0,0,0};


// *ASL* 12/08/2000 - Soft reset debounce flag
int softResetDeBounce = 0;


//----- [ FUNCTION IMPLEMENTATION ] --------------------------------------------------------------

/*	--------------------------------------------------------------------------------
	Function 	: my_memcopy
	Purpose 	: 
	Parameters 	: 
	Returns 	: 
	Info 		:
*/

void my_memcopy(char *source, char *dest, int size)
{
	register int i;
	
	for(i=0;i<size;i++)
		*dest++ = *source++;
}

/*	--------------------------------------------------------------------------------
	Function 	: getShort
	Purpose 	: 
	Parameters 	: 
	Returns 	: 
	Info 		:
*/

short getShort(char *ptr)
{
	short	testShort;

	testShort  = (*ptr++)       & 0xff;
	testShort |= (*ptr++ <<  8) & 0xff00;
		
	return testShort;
}

/*	--------------------------------------------------------------------------------
	Function 	: getInt
	Purpose 	: 
	Parameters 	: 
	Returns 	: 
	Info 		:
*/

int getInt(char *ptr)
{
	int	testInt;

	testInt  = ((*ptr++)       & 0xff);
	testInt |= ((*ptr++ <<  8) & 0xff00);
	testInt |= ((*ptr++ << 16) & 0xff0000);
	testInt |= ((*ptr++ << 24) & 0xff000000);
		
	return testInt;
}

/*	--------------------------------------------------------------------------------
	Function 	: getFloat
	Purpose 	: 
	Parameters 	: 
	Returns 	: 
	Info 		:
*/

float getFloat(char *ptr)
{
	register int 	i;
	float			testFloat;
	char			*dest;
	
	dest = (char *)&testFloat;
	for(i=sizeof(float);i>0;i--)
		*dest++ = *ptr++;

	return testFloat;
}

int checkForControllerRemovedSingle()
{
	int state;

	// test to see if pad still connected
	switch(firstPad)
	{
		case 0:	
			per = pdGetPeripheral(PDD_PORT_A0);
			break;
		case 1:	
			per = pdGetPeripheral(PDD_PORT_B0);
			break;
		case 2:	
			per = pdGetPeripheral(PDD_PORT_C0);
			break;
		case 3:	
			per = pdGetPeripheral(PDD_PORT_D0);
			break;
	}
	state = TRUE;
	if(strstr(per->info->product_name,"(none)"))
	{
		state = FALSE;				
	}
	return !state;
}

int checkForControllerInsertedSingle()
{
	int i,pads[4],state;

	switch(firstPad)
	{
		case 0:	
			per = pdGetPeripheral(PDD_PORT_A0);
			break;
		case 1:	
			per = pdGetPeripheral(PDD_PORT_B0);
			break;
		case 2:	
			per = pdGetPeripheral(PDD_PORT_C0);
			break;
		case 3:	
			per = pdGetPeripheral(PDD_PORT_D0);
			break;
	}
	state = TRUE;				
	if(strstr(per->info->product_name,"(none)"))
	{
		state = FALSE;				
	}

	return state;
}

int checkForControllerRemovedMulti()
{
	int i,state,actualPad = 0;

	for(i=firstPad;i<4;i++)
	{		
		// test to see if pad still connected
		switch(i)
		{
			case 0:	
				per = pdGetPeripheral(PDD_PORT_A0);
				break;
			case 1:	
				per = pdGetPeripheral(PDD_PORT_B0);
				break;
			case 2:	
				per = pdGetPeripheral(PDD_PORT_C0);
				break;
			case 3:	
				per = pdGetPeripheral(PDD_PORT_D0);
				break;
		}
		state = TRUE;
		if(strstr(per->info->product_name,"(none)"))
		{
			state = FALSE;				
		}
		else
			actualPad++;

		if((padStatus[i] == TRUE)&&(state == FALSE))
			return actualPad+1;

	}

	return FALSE;
}

int checkForControllerInsertedMulti()
{
	int i,state;

	for(i=firstPad;i<4;i++)
	{		
		// test to see if pad still connected
		switch(i)
		{
			case 0:	
				per = pdGetPeripheral(PDD_PORT_A0);
				break;
			case 1:	
				per = pdGetPeripheral(PDD_PORT_B0);
				break;
			case 2:	
				per = pdGetPeripheral(PDD_PORT_C0);
				break;
			case 3:	
				per = pdGetPeripheral(PDD_PORT_D0);
				break;
		}
		state = TRUE;
		if(strstr(per->info->product_name,"(none)"))
		{
			state = FALSE;				
		}

		if(padStatus[i] != state)
			return FALSE;
	}

	return TRUE;
}


/* --------------------------------------------------------------------------------
   Function : initCheckForSoftReset
   Purpose : initialise the soft reset check (!)
   Parameters : 
   Returns : 
   Info :
*/

void initCheckForSoftReset()
{
	softResetDeBounce = 0;
}


/* --------------------------------------------------------------------------------
   Function : checkForSoftReset
   Purpose : check if user as pressed soft reset
   Parameters : 
   Returns : TRUE soft reset, else FALSE
   Info :
*/

int checkForSoftReset()
{
	int i;

	for(i=0;i<4;i++)
	{
		switch(i)
		{
			case 0:	
				per = pdGetPeripheral(PDD_PORT_A0);
				break;
			case 1:	
				per = pdGetPeripheral(PDD_PORT_B0);
				break;
			case 2:	
				per = pdGetPeripheral(PDD_PORT_C0);
				break;
			case 3:	
				per = pdGetPeripheral(PDD_PORT_D0);
				break;
		}

		if(strstr(per->info->product_name,"(none)") == FALSE)
		{
			// *ASL* 12/08/2000 - Check reset debounce
			if (softResetDeBounce == 1)
			{
				if ((!(per->on & PDD_DGT_TB)) &&
					(!(per->on & PDD_DGT_TA)) &&
					(!(per->on & PDD_DGT_TX)) &&
					(!(per->on & PDD_DGT_TY)) &&
					(!(per->on & PDD_DGT_ST)))
				{
					softResetDeBounce = 0;
				}
			}
			else
			{
				if ((per->on & PDD_DGT_TB)&&
					(per->on & PDD_DGT_TA)&&
					(per->on & PDD_DGT_TX)&&
					(per->on & PDD_DGT_TY)&&
					(per->on & PDD_DGT_ST))
				{
					softResetDeBounce = 1;
					return TRUE;
				}
			}
		}
	}

	return FALSE;
}



int checkForValidControllers()
{
	int i;

	firstPad = 4;
	numPads = 0;
	for(i = 3;i >= 0;i--)
	{
		switch(i)
		{
			case 0:	
				per = pdGetPeripheral(PDD_PORT_A0);
				break;
			case 1:	
				per = pdGetPeripheral(PDD_PORT_B0);
				break;
			case 2:	
				per = pdGetPeripheral(PDD_PORT_C0);
				break;
			case 3:	
				per = pdGetPeripheral(PDD_PORT_D0);
				break;
		}
		padData.present[i] = TRUE;				
		if(strstr(per->info->product_name,"(none)"))
		{
			padData.present[i] = FALSE;				
			padStatus[i] = PADSTATUS_NOTCONNECTED;
		}
		else
		{
			firstPad = i;
			numPads++;
			padStatus[i] = PADSTATUS_CONNECTED;
		}
	}

	return numPads;
}