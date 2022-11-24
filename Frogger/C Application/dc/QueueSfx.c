

#include "main.h"
#include "QueueSfx.h"
#include "audio.h"

extern AC_ERROR_PTR		acErr;
extern AM_ERROR			*amErr;

// our queue
TqfxEntry	*queueEntries = NULL;
// start and free pointers
volatile TqfxEntry	*startPtr = NULL, *freePtr = NULL;

// queue is empty
int isEmptyFlag = 0;
// queue is full
int isFullFlag = 0;

// initialised flag
int initFlag = 0;

// initialise the sound effect queue
void qfxInit()
{
	// allocate our static sound effect queue
	queueEntries = (TqfxEntry *)syMalloc(MAX_ENTRIES * sizeof(TqfxEntry));
	if (queueEntries == 0)
		return;

	// set initial queue state
	startPtr = freePtr = queueEntries;
	isEmptyFlag = 1;		// queue empty
	isFullFlag = 0;			// queue full

	// queue initialised
	initFlag = 1;
}

// shutdown the sound effect queue
void qfxShutdown()
{
	// invalidate queue
	initFlag = 0;

	// free queue
	if (queueEntries != NULL)
		syFree(queueEntries);
	queueEntries = NULL;

	startPtr = freePtr = NULL;
}

// add entry to the queue
int qfxAddEntry(TqfxEntry *addEntry)
{
	// can only add entry if the queue isn't full
	if (initFlag == 0 || isFullFlag == 1 || addEntry == NULL)
		return 0;

	// copy entry into queue
	*freePtr = *addEntry;

	// set number of retries
	freePtr->retries = 0;

	// inc free pointer and check if the queue is full
	freePtr++;
	if (freePtr >= (queueEntries + MAX_ENTRIES))
		freePtr = queueEntries;
	if (freePtr == startPtr)
		isFullFlag = 1;

	isEmptyFlag = 0;
	return 1;
}

// delete entry from the queue
int qfxDelEntry()
{
	// can only remove an entry if the queue isn't empty
	if (initFlag == 0 || isEmptyFlag == 1)
		return 0;


	// process this command
	if (qfxProcessCommand(startPtr) == -1)
	{
		freePtr->retries++;
		if(freePtr->retries < 5)
			return -1;			// abort on error
	}

	// inc start pointer and check if the queue is empty
	startPtr++;
	if (startPtr >= (queueEntries + MAX_ENTRIES))
		startPtr = queueEntries;
	if (startPtr == freePtr)
		isEmptyFlag = 1;

	isFullFlag = 0;
	return 1;
}

// process queue entry
int qfxProcessCommand(TqfxEntry *entry)
{
	if (initFlag == 0 || entry == NULL)
		return 0;

	// process our queue command
	switch (entry->command)
	{
	case QFXCMD_NULL:
		break;

	case QFXCMD_PLAYSAMPLE:
		break;

	// stop the sample
	case QFXCMD_STOPSAMPLE:

		if (amSoundIsPlaying(&current[entry->channel].sound) == KTFALSE)
			return 1;
//ma - err		if (amErr->number == AM_CANT_SEND_DRIVER_COMMAND)
//ma - err			return -1;

		if (amSoundStop(&current[entry->channel].sound) == KTFALSE)
			return -1;
//ma - err		if (amErr->number == AM_CANT_SEND_DRIVER_COMMAND)
//ma - err			return -1;
		break;

	// set volume
	case QFXCMD_SETVOLUME:
		if (amSoundIsPlaying(&current[entry->channel].sound) == KTFALSE)
			return 1;
		if (amSoundSetVolume(&current[entry->channel].sound,entry->volume) == KTFALSE)
			return -1;
//ma - err		if (amErr->number == AM_CANT_SEND_DRIVER_COMMAND)
//ma - err			return -1;
		break;

	// set pan
	case QFXCMD_SETPAN:
		if (amSoundIsPlaying(&current[entry->channel].sound) == KTFALSE)
			return 1;
		if (amSoundSetPan(&current[entry->channel].sound,entry->pan) == KTFALSE)
			return -1;
//ma - err		if (amErr->number == AM_CANT_SEND_DRIVER_COMMAND)
//ma - err			return -1;
		break;

	// set pitch
	case QFXCMD_SETPITCH:
		if (amSoundIsPlaying(&current[entry->channel].sound) == KTFALSE)
			return 1;
		if (amSoundSetCurrentPlaybackRate(&current[entry->channel].sound, entry->pitch) == KTFALSE)
			return -1;
//ma - err		if (amErr->number == AM_CANT_SEND_DRIVER_COMMAND)
//ma - err			return -1;
		break;
	}

	return 1;
}

// query the state of the queue.. need to check this..!!
int qfxQueryState()
{
	if (initFlag == 0)
		return 0;

	// currently returns the number of entries in the queue
	return (abs(freePtr-startPtr));
}

// pump commands to the sound chip
int qfxPumpCommands()
{
	while (isEmptyFlag == 0)
	{
		// remove entry and process commmand
		if (qfxDelEntry() == -1)
			return 0;			// ..abort on error
	}
	return 1;
}

// remove all entrys from the queue
int qfxFlush()
{
	if (initFlag != 1)
		return 0;

	// reset queue pointers
	startPtr = freePtr = queueEntries;
	isEmptyFlag = 1;		// queue is empty
	isFullFlag = 0;			// queue isn't full

	return 1;
}


#if 0

QFXCMD_PLAYSAMPLE
{
	int				c;
	TqfxEntry		qfx;
	CurrentData		*chan;

	// locate a free sample structure for our sound effect
	for (c=0; c<MAX_CHANNELS; c++)
	{
		if (current[c].sound.used)
			break;
	}
	if (c >= MAX_CHANNELS)			// all channels used
		return 1;					// abort with no error
	chan = &current[c].sound;

	// flag this channel used for the moment
	chan->used = 1;
	chan->pan = entry->pan;
	chan->pitch = entry->pitch;
	chan->volume = entry->volume;

	// try and fetch the sample from the bank
	if (amSoundFetchSample(entry->theBank, entry->soundIndex, &chan->theSound) == KTFALSE)
	{
		chan->used = 0;				// release our channel
		return -1;					// and abort with error
	}

	// queue up the channel allocation
	qfx.command = QFXCMD_PLAYSAMPLE_ALLOCCHAN;
	qfx.channel = c;
	if (qfxAddEntry(&qfx) == 0)
	{
		chan->used = 0;				// release our channel
		return 1;					// and abort with no error
	}

	return 1;
}

QFXCMD_PLAYSAMPLE_ALLOCCHAN;
{
	TqfxEntry		qfx;
	CurrentData		*chan;
	
	chan = &current[entry->channel];

	// allocate a channel for our sample
	if (amSoundAllocateVoiceChannel(chan->theSound) == KTFALSE)
	{
		chan->used = 0;				// release our channel
		return -1;					// and abort with error
	}

	// queue up the initial sample volume
	qfx.command = QFXCMD_PLAYSAMPLE_SETVOLUME;
	qfx.channel = entry->channel;
	if (qfxAddEntry(&qfx) == 0)
	{
		// ** The chances of the above function failing are quite rare, but
		// ** we still should handle failure. Not too happy about the second
		// ** call to amSound though.

		if (amSoundReleaseVoiceChannel(chan->theSound) == KTFALSE)
		{
			// ** I really don't think the release voice channel will issue a command,
			// ** but if so wait for command buffer to empty and try again.

			// wait and try again
			acSystemDelay(500000);
			amSoundReleaseVoiceChannel(chan->theSound);
		}
		chan->used = 0;				// release our channel
		return 1;					// and abort with no error
	}

	return 1;
}

QFXCMD_PLAYSAMPLE_SETVOLUME
{
	TqfxEntry		qfx;
	CurrentData		*chan;
	
	chan = &current[entry->channel];

	// set our samples initial volume
	if (amSoundSetVolume(chan->theSound, chan->volume) == KTFALSE)
	{
		// release our allocated sample channel
		if (amSoundReleaseVoiceChannel(chan->theSound) == KTFALSE)
		{
			// wait and try again
			acSystemDelay(500000);
			amSoundReleaseVoiceChannel(chan->theSound);
		}
		chan->used = 0;				// release our channel
		return -1;					// and abort with error
	}

	// queue up the initial sample pan
	qfx.command = QFXCMD_PLAYSAMPLE_SETPAN;
	qfx.channel = entry->channel;
	if (qfxAddEntry(&qfx) == 0)
	{
		if (amSoundReleaseVoiceChannel(chan->theSound) == KTFALSE)
		{
			// wait and try again
			acSystemDelay(500000);
			amSoundReleaseVoiceChannel(chan->theSound);
		}
		chan->used = 0;				// release our channel
		return -1;
	}
}


QFXCMD_PLAYSAMPLE_SETPAN
{
	TqfxEntry		qfx;
	CurrentData		*chan;
	
	chan = &current[entry->channel];

	// set our samples initial volume
	if (amSoundSetPan(chan->theSound, chan->pan) == KTFALSE)
	{
		// release our allocated sample channel
		if (amSoundReleaseVoiceChannel(chan->theSound) == KTFALSE)
		{
			// wait and try again
			acSystemDelay(500000);
			amSoundReleaseVoiceChannel(chan->theSound);
		}
		chan->used = 0;				// release our channel
		return -1;					// and abort with error
	}

	// queue up the sample to play
	qfx.command = QFXCMD_PLAYSAMPLE_PLAY;
	qfx.channel = entry->channel;
	if (qfxAddEntry(&qfx) == 0)
	{
		if (amSoundReleaseVoiceChannel(chan->theSound) == KTFALSE)
		{
			// wait and try again
			acSystemDelay(500000);
			amSoundReleaseVoiceChannel(chan->theSound);
		}
		chan->used = 0;				// release our channel
		return -1;
	}

	return 1;
}


QFXCMD_PLAYSAMPLE_PLAY
{
	TqfxEntry		qfx;
	CurrentData		*chan;
	
	chan = &current[entry->channel];

	// set our samples initial volume
	if (amSoundPlay(chan->theSound) == KTFALSE)
	{
		chan->used = 0;				// release our channel
		return -1;					// and abort with error
	}

	// flag our sample is playing
	chan->isPlaying = 1;

	return 1;
}


#endif