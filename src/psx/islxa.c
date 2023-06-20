/************************************************************************************
	ISL PSX LIBRARY	(c) 1999 Interactive Studios Ltd.

	islxa.h:		XA audio streaming functions

************************************************************************************/

#include "include.h"
#include "islxa.h"
#include "main.h"
#include "menus.h"
#include "options.h"

//#include "sound.h"

//#define XAROOT	"\\"

// Moved to "sound.h"



XADataType	XAData;

// are we enabled or not
static int	XAenable;

#define ADX_WORKSIZE (ADXT_CALC_WORK((1), (ADXT_PLY_STM), (3), (44100)))
static char xaWorkArea[ADX_WORKSIZE];

XAFileType	*curXA = NULL;

// XA buffer
static XAFileType	xaBuffer;

// XA playing flag
static unsigned int	xaPlaying = 0;


// *ASL* 13/08/2000
/* ---------------------------------------------------------
   Function : XAinit
   Purpose : initialise streaming XA
   Parameters :
   Returns : 
   Info : 
*/

void XAinit()
{
	xaPlaying = 0;
	xaBuffer.adxt = NULL;
	curXA = NULL;
}


/* ---------------------------------------------------------
   Function : XAShutdown
   Purpose : shutdown streaming XA
   Parameters :
   Returns : 
   Info : 
*/

void XAShutdown()
{
	XAstop();
}


/* ---------------------------------------------------------
   Function : XAgetFileInfo
   Purpose : get an ADX file handle for our streaming filename
   Parameters : stream filename
   Returns : pointer to the XA buffer
   Info : 
*/

XAFileType *XAgetFileInfo(char *fileName)
{
	// release any currently playing stream
	XAstop();

	// initialise our XA stream buffer
	sprintf(xaBuffer.fileInfo, fileName);
	xaBuffer.startPos = 0;
	xaBuffer.endPos	= 0;
	xaBuffer.status = 0;
	xaBuffer.adxt = ADXT_Create(2, xaWorkArea, ADX_WORKSIZE);

	// problem creating the work buffer
	if (xaBuffer.adxt == NULL)
		return NULL;

	// flag active
	xaPlaying = 1;
	// and return the buffer
	return &xaBuffer;
}


/* ---------------------------------------------------------
   Function : XAstop
   Purpose : stop currently playing XA stream
   Parameters : 
   Returns : 
   Info : 
*/

void XAstop()
{
	if (xaPlaying == 1)
	{
		ADXT_Destroy(xaBuffer.adxt);
		xaBuffer.adxt = NULL;
	}
	xaPlaying = 0;
	curXA = NULL;
}








/**************************************************************************
	FUNCTION:	XAsetStatus()
	PURPOSE:	Enable/disable xa playing
	PARAMETERS:	0 - disable / 1 - enable
	RETURNS:	
**************************************************************************/

void XAsetStatus(int enable)
{
	if(enable)
		XAenable = 1;
	else
		XAenable = 0;
}


/**************************************************************************
	FUNCTION:	XAgetStatus()
	PURPOSE:	Get current status of XA playing
	PARAMETERS:	
	RETURNS:	0 - disabled / 1 - enabled
**************************************************************************/

/*int XAgetStatus()
{
	return XAenable;
}*/


/**************************************************************************
	FUNCTION:	XAcallback()
	PURPOSE:	Callback for XA audio stream
	PARAMETERS:	
	RETURNS:	
**************************************************************************/

static void XAcallback(int intr, u_char *result)
{
	// JIM : Ummm........not sure if we can have callbacks with the middleware stuff, 
	// but we'll find out in a little while
}


#if 0
/**************************************************************************
	FUNCTION:	XAgetFileInfo()

	PURPOSE:	For the PSX, this get's position information for XA file on
				the CD, but we just need it's filename and an ADX file handle 
				thingy.

	PARAMETERS:	Filename

	RETURNS:	
**************************************************************************/

XAFileType *XAgetFileInfo(char *fileName)
{
	XAFileType	*xaf;
	char		fName[40];

	xaf = (XAFileType *)MALLOC0(sizeof(XAFileType));				//
	memset(xaf, 0, sizeof(XAFileType));								//

	sprintf(xaf->fileInfo,fileName);
	xaf->startPos		= 0;									//
	xaf->endPos			= 0;									//
	xaf->status			= 0;									//

	xaf->adxt			= ADXT_Create(2, xaWorkArea, ADX_WORKSIZE);	//
	return xaf;
}
#endif


/**************************************************************************
	FUNCTION:	XAstart()
	PURPOSE:	Get CD ready to play XA audio stream
	PARAMETERS:	CD speed 0=single 1=double
	RETURNS:	
**************************************************************************/

void XAstart(int speed)
{
	// JIM - TODO : Dummy function !! This is all handled by the middleware stuff
}


#if 0
/**************************************************************************
	FUNCTION:	XAstop()
	PURPOSE:	CD back to data - finished playing XA audio streams
	PARAMETERS:	
	RETURNS:	
**************************************************************************/

void XAstop()
{
	if (curXA)						// Is there an adx currently playing ?
	{								// yup, so........
		if(curXA->adxt)
		{
			ADXT_Destroy(curXA->adxt);		// Tell middleware to quit playing it......
			curXA = NULL;
		}
	} // end-if
}
#endif


/**************************************************************************
	FUNCTION:	XApause()
	PURPOSE:	CD back to data - pause playing XA audio streams
	PARAMETERS:	
	RETURNS:	
**************************************************************************/

void XApause()
{
	// JIM - TODO : Pause playing the current ADX stuff......


}


/**************************************************************************
	FUNCTION:	XAplayChannelOffset()
	PURPOSE:	Start playing channel from given XA audio stream
	PARAMETERS:	XA file, offset (sectors), channel number, loop
	RETURNS:	
**************************************************************************/

void XAplayChannelOffset(XAFileType *xaF, int offset, int channel, int loop, int vol)
{
	gdFsChangeDir("\\");						// Change to the audio directory on the GD
	gdFsChangeDir("MUSIC");						// 

	curXA		= xaF;							// Umm...for convenience let's have a global 
												// var letting us know the current ADX that's
												// playing.

	xaF->loop	= loop;							// loop flag
	xaF->vol	= vol;							// volume to play the ADX at 

	ADXT_StartFname(xaF->adxt, xaF->fileInfo);	// Start playing the XDA using the middlware

	if(options.stereo)
	{
		// set streaming audio to stereo
		ADXT_SetOutPan(xaF->adxt,0,ADXT_PAN_LEFT);
		ADXT_SetOutPan(xaF->adxt,1,ADXT_PAN_RIGHT);

		// set sfx to stereo
		acSystemDelay(500000);
		acSystemSetStereoOrMono(0);
		acSystemDelay(500000);
	}
	else
	{
		// set streaming audio to mono
		ADXT_SetOutPan(xaF->adxt,0,ADXT_PAN_CENTER);
		ADXT_SetOutPan(xaF->adxt,1,ADXT_PAN_CENTER);

		// set sfx to mono
		acSystemDelay(500000);
		acSystemSetStereoOrMono(1);
		acSystemDelay(500000);
	}
}


/**************************************************************************
	FUNCTION:	XAcheckPlay()
	PURPOSE:	Test if XA has begun playback
	PARAMETERS:	
	RETURNS:	0 = Not playing yet, 1 = Is playing now
**************************************************************************/

int XAcheckPlay()
{
unsigned char result[8];

	if (!XAenable)
		return 1;
	return 0;
}


/**************************************************************************
	FUNCTION:	XAstorePrevious()
	PURPOSE:	Store current XA playback track/channel
	PARAMETERS:	
	RETURNS:	
**************************************************************************/

void XAstorePrevious()
{
	XAData.prevXA = XAData.currXA;
	XAData.prevChannel = XAData.activeChannel;
	XAData.prevSector = XAData.currSector;
}


/**************************************************************************
	FUNCTION:	XArestartPrevious()
	PURPOSE:	Restart previously stored XA playback track/channel
	PARAMETERS:	
	RETURNS:	
**************************************************************************/

void XArestartPrevious()
{

	if (!XAenable)
	{
		printf("XArestartPrevious: disabled\n");
		XAData.prevXA->status = 0;
		return;
	}
	if (XAData.prevXA==NULL)
	{
		return;
	}

	
	XAData.activeChannel = XAData.prevChannel;
		
	XAData.currXA = XAData.prevXA;
	XAData.currSector = XAData.prevSector;
	XAData.prevXA->status = 1;

	printf("XArestartPrevious: %d [%d->%d]\n", XAData.prevSector,XAData.prevXA->startPos,XAData.prevXA->endPos);	
}

