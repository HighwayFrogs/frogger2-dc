#include "include.h"
#include "am_audio.h"
#include <shinobi.h>
#include <sg_chain.h>
#include <sg_syg2.h>
#include "frogger.h"

#include <ac.h>
#include <a64thunk.h>
#include <am.h>

#include "am_audio.h"
#include "bpamsetup.h"
#include "bpamstream.h"
#include "bpbuttons.h"
#include "bpprintf.h"
#include "bpamsfx.h"
#include "strsfx.h"

#define STREAM_PLAY_BUFFER_MONO_SIZE		16384
#define STREAM_PLAY_BUFFER_STEREO_SIZE		(STREAM_PLAY_BUFFER_MONO_SIZE * 2)
#define STREAM_TRANSFER_BUFFER_SIZE			(STREAM_PLAY_BUFFER_STEREO_SIZE * 2)

#define MAX_ONE_SHOTS						4
	
KTBOOL gStartSound = KTTRUE;

KTU32 gSoundCount = 0;
KTU32 gFailSoundCount = 0;
KTU32 gStreamCount = 0;
KTU32 gFailStreamCount = 0;

KTU32 *gTransferBuffer = KTNULL;
KTU32 gTransferBufferSize = STREAM_TRANSFER_BUFFER_SIZE;
KTU32 *gPlayBuffer = KTNULL;
KTU32 gPlayBufferSize = STREAM_PLAY_BUFFER_STEREO_SIZE;
KTU32 *gHeaderBuffer = KTNULL;

AM_STREAM *gStream = KTNULL;

char	currentStreamFilename[32];

KTBOOL StreamSetup(char *filename, int loop)
{
	// get the necessary buffers lined up for the stream
	//
	// allocate memory for the play buffers.
	//
	// play buffer MUST! be in aica memory and 32 byte aligned.

	gdFsChangeDir("\\");
//	gdFsChangeDir("MUSIC");
	
/*	if (!amHeapAlloc(&gPlayBuffer, gPlayBufferSize, 32,	AM_PURGABLE_MEMORY,KTNULL))
	{
		return (KTFALSE);
	}

	// allocate memory for the transfer buffers.
	//
	// transfer buffer MUST! be in sh4 memory and 32 byte aligned or file
	// reads will fail with the GDFS file system.

	gTransferBuffer = (KTU32*)syMalloc(gTransferBufferSize);
	if (!gTransferBuffer)
	{
		return (KTFALSE);
	}

	// allocate memory for the header buffers.
	//
	// header buffer MUST! be in sh4 memory and 32 byte aligned or file
	// reads will fail with the GDFS file system.

	gHeaderBuffer = (KTU32*)syMalloc(AM_FILE_BLOCK_SIZE);
	if (!gHeaderBuffer)
	{
		return (KTFALSE);
	}

	// prepare the stream
*/
	gStream = bpAmStreamPrepareFile(filename, 64/*127*/,
		AM_PAN_CENTER, gHeaderBuffer, gTransferBuffer, gTransferBufferSize,
		gPlayBuffer, gPlayBufferSize, KTNULL);
	if (!gStream)
	{
		return (KTFALSE);
	}

	sprintf(currentStreamFilename,"%s",filename);

	return (KTTRUE);
}

void ReplayStream(void)
{
//	amStreamStart(gStream);
	
	if (!bpAmStreamDestroy(gStream))
	{
		++gFailStreamCount;
		return;
	}

	gdFsChangeDir("\\");
//	gdFsChangeDir("MUSIC");

	gStream = bpAmStreamPrepareFile(currentStreamFilename, 127,
		AM_PAN_CENTER, gHeaderBuffer, gTransferBuffer, gTransferBufferSize,
		gPlayBuffer, gPlayBufferSize, KTNULL);
	if (!gStream)
	{
		++gFailStreamCount;
		return;
	}
		
	if (!bpAmStreamStart(gStream))
	{
		++gFailStreamCount;
		return;
	}

	++gStreamCount;
}

KTBOOL StreamDestroy()
{
	// free sound mem buffer
/*	if(gPlayBuffer)
	{
		amHeapFree(gPlayBuffer);
	}

	// free transfer buffer
	if(gTransferBuffer)
	{
		syFree(gTransferBuffer);
	}

	// free header buffer
	if(gHeaderBuffer)
	{
		syFree(gHeaderBuffer);
		return (KTFALSE);
	}
*/
	currentStreamFilename[0] = 0;
	
	return TRUE;
}
