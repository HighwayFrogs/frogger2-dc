/************************************************************************************
	PSX/DC LIBRARY	(c) 2000 BLitz Games Ltd.

	islcard.h:		memory card functions

************************************************************************************/

#ifndef __ISLCARD_H__
#define __ISLCARD_H__

// error codes

#define	McErrNone			(0)
#define	McErrCardNotExist	(1)
#define	McErrCardInvalid	(2)
#define	McErrNewCard		(3)
#define	McErrNotFormat		(4)
#define	McErrFileNotExist	(5)
#define	McErrAlreadyExist	(6)
#define	McErrBlockFull		(7)

#define CARDREAD_OK					0
#define CARDREAD_NOCARD				1
#define CARDREAD_BADCARD			2
#define CARDREAD_NOTFORMATTED		4
#define CARDREAD_NOTFOUND			-1
#define CARDREAD_CORRUPT			-2
#define CARDREAD_NOTFOUNDANDFULL	-3

#define CARDWRITE_OK				0
#define CARDWRITE_NOCARD			1
#define CARDWRITE_BADCARD			2
#define CARDWRITE_NOTFORMATTED		4
#define CARDWRITE_FULL				7

extern Uint32 portNos[];
extern Sint8 *portlit[];

extern short portIndex;
extern unsigned long vmuPortToUse, vmuDriveToUse;
extern unsigned long vmuBeepStopTimer;

/**************************************************************************
	FUNCTION:	cardInitialise()
	PURPOSE:	Initialise memory card routines
	PARAMETERS:	
	RETURNS:	
**************************************************************************/

void cardInitialise();


/**************************************************************************
	FUNCTION:	cardDestroy()
	PURPOSE:	Clear up memory card routines
	PARAMETERS:	
	RETURNS:	
**************************************************************************/

void cardDestroy();


/**************************************************************************
	FUNCTION:	cardRead()
	PURPOSE:	Read game save from memory card
	PARAMETERS:	save name, pointer to data, size of data
	RETURNS:	see list above (+ve Sony errors, -ve loading errors)
**************************************************************************/

int cardRead(char *memCardName, void *saveData, int saveDataSize);


/**************************************************************************
	FUNCTION:	cardWrite()
	PURPOSE:	Write game save to memory card
	PARAMETERS:	save name, pointer to data, size of data
	RETURNS:	see list above (+ve Sony errors, -ve saving errors)
**************************************************************************/

int cardWrite(char *memCardName, void *saveData, int saveDataSize);


/**************************************************************************
	FUNCTION:	cardDisplay()
	PURPOSE:	Display bitmap on card LCD if available
	PARAMETERS:	pointer to bitmap data
	RETURNS:	
**************************************************************************/

void cardDisplay(Uint8 *bitmap);


/**************************************************************************
	FUNCTION:	cardBeep()
	PURPOSE:	Audible beep from VMU
	PARAMETERS:	Should be some controlling duration & pitch, but not yet...
	RETURNS:	
**************************************************************************/

int cardBeep( Uint32 time, int good );

#endif //__ISLCARD_H__
