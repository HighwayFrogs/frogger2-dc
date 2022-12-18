/****************************************************************

	ISL DREAMCAST LIBRARY (c) 2000 Blitz Games Ltd.		
	islvideo.c	Video player
														tgaulton
*****************************************************************/

#ifndef __ISLVIDEO_H__
#define __ISLVIDEO_H__

/* ---------------------------------------------------------------------------
 * - CONSTANTS
 * ---------------------------------------------------------------------------
 */
 
// Streaming modes.
#define STR_MODE24				1
#define STR_MODE16				0


// Border modes.
#define STR_BORDERS_OFF			0
#define STR_BORDERS_ON			1


// PlayStream return values.
#define	PLAYSTR_END				1
#define	PLAYSTR_USER			2
#define	PLAYSTR_ERROR			3

enum
{
	FMV_SOFTRESET_NONE,
	FMV_SOFTRESET_TOBOOTROM,
	FMV_SOFTRESET_TOTITLE,
};

/* ---------------------------------------------------------------------------
 * - DATA TYPE AND STRUCTURE DECLARATIONS
 * ---------------------------------------------------------------------------
 */

typedef struct _StrDataType
{
		char		*strName;					// Stream file name. 
		short		mode;						// 24-Bit or 16-Bit streaming.
		short		drawBorders;				// != 0 if borders on.
		short		scrWidth;					// Screen res width.
		short		x;							// X,Y position. 
		short		y;
		short		width;						// Stream width and height.
		short		height;
		long		endFrame;					// Last frame No.
		long		vlcBufSize;					// Size of each VLC buffer (including header).
		short		volume;						// Left and Right ADPCM volume.
} StrDataType;

extern int quitAllVideo;

/* ---------------------------------------------------------------------------
 * - PUBLIC FUNCTION PROTOTYPES
 * ---------------------------------------------------------------------------
 */

// *ASL* 12/08/2000 - Allow user quit
/* ---------------------------------------------------------
   Function : videoPlayStream
   Purpose : playback a video stream
   Parameters : stream pointer, PAL mode, allow user quit flag
   Returns : 1 if user quit the stream, else 0
   Info : 
*/

int videoPlayStream(StrDataType *str, int palMode, int allowQuit);

void videoSetAudioChannel(int channel);

#ifndef NTSC_VERSION
extern int	fmvSoftReset;
extern int	fmvBookSoftReset;
#endif

#endif // __ISLVIDEO_H__
