/************************************************************************************
	ISL PSX LIBRARY	(c) 1999 Interactive Studios Ltd.

	islxa.h:		XA audio streaming functions

************************************************************************************/


#ifndef __ISLXA_H__
#define	__ISLXA_H__

//#include <adx_mw.h>
//#include <cri_adxt.h>
#include	<cri_adxf.h>
#include	<cri_adxt.h>

typedef struct _XAFileType {
	char 			fileInfo[32];			// This will actually point to the filename of the ADX file
	int				startPos;			//
	int				endPos;				//
	unsigned char	status;				// Playing, paused or stopped
	unsigned char	loop;				// Loop this track ?
	int				vol;				// Volume
	ADXT			adxt;				// Ummm......adxt handle thingy.....
} XAFileType;

typedef struct _XADataType {
	long		oldCallback;
	char		buffer[2340];
	int			currChannel;
	int			activeChannel;
	int			currSector;
//	int			currSectorReal;
	XAFileType	*currXA;
	XAFileType	*prevXA;
	int			prevChannel;
	int			prevSector;
} XADataType;

#define BFF_SBH_ID (('S'<<0) | ('B'<<8) | ('H'<<16) | (0<<24))
#define SOUND_XA_NOLOOP 128

extern XADataType	XAData;
extern XAFileType	*curXA;

/**************************************************************************
	FUNCTION:	XAsetStatus()
	PURPOSE:	Enable/disable xa playing
	PARAMETERS:	0 - disable / 1 - enable
	RETURNS:	
**************************************************************************/

void XAsetStatus(int enable);


/**************************************************************************
	FUNCTION:	XAsetStatus()
	PURPOSE:	Enable/disable xa playing
	PARAMETERS:	0 - disable / 1 - enable
	RETURNS:	
**************************************************************************/

//void XAgetStatus();

/**************************************************************************
	FUNCTION:	XAgetFileInfo()
	PURPOSE:	Get position information for XA file
	PARAMETERS:	Filename
	RETURNS:	
**************************************************************************/

XAFileType *XAgetFileInfo(char *fileName);


/**************************************************************************
	FUNCTION:	XAstart()
	PURPOSE:	Get CD ready to play XA audio stream
	PARAMETERS:	Speed 0 = single, 1 = double
	RETURNS:	
**************************************************************************/

void XAstart();


/**************************************************************************
	FUNCTION:	XAstop()
	PURPOSE:	CD back to data - finished playing XA audio streams
	PARAMETERS:	
	RETURNS:	
**************************************************************************/

void XAstop();


/**************************************************************************
	FUNCTION:	XApause()
	PURPOSE:	Stop the XA audio stream
	PARAMETERS:	
	RETURNS:	
**************************************************************************/

void XApause();


/**************************************************************************
	FUNCTION:	XAplayChannel()
	PURPOSE:	Start playing channel from given XA audio stream
	PARAMETERS:	XA file, channel number, loop, vol
	RETURNS:	
**************************************************************************/

#define XAplayChannel(xaF, channel, loop, vol) XAplayChannelOffset(xaF, 0, channel, loop, vol);


/**************************************************************************
	FUNCTION:	XAplayChannelOffset()
	PURPOSE:	Start playing channel from given XA audio stream
	PARAMETERS:	XA file, offset (sectors), channel number, loop, vol
	RETURNS:	
**************************************************************************/

void XAplayChannelOffset(XAFileType *xaF, int offset, int channel, int loop, int vol);


/**************************************************************************
	FUNCTION:	XAcheckPlay()
	PURPOSE:	Test if XA has begun playback
	PARAMETERS:	
	RETURNS:	0 = Not playing yet, 1 = Is playing now
**************************************************************************/

int XAcheckPlay();


/**************************************************************************
	FUNCTION:	XAstorePrevious()
	PURPOSE:	Store current XA playback track/channel
	PARAMETERS:	
	RETURNS:	
**************************************************************************/

void XAstorePrevious();


/**************************************************************************
	FUNCTION:	XArestartPrevious()
	PURPOSE:	Restart previously stored XA playback track/channel
	PARAMETERS:	
	RETURNS:	
**************************************************************************/

void XArestartPrevious();


#endif	//__ISLXA_H__
