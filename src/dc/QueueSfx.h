#ifndef _QUEUESFX_H_INCLUDED
#define _QUEUESFX_H_INCLUDED

// maximum number of available entries
#define MAX_ENTRIES		2048

// maximum number of commands to pump to the sound chip
#define MAX_PUMP_COMMANDS	16

// all queue commands
enum
{
	QFXCMD_NULL,
	QFXCMD_PLAYSAMPLE,
	QFXCMD_STOPSAMPLE,
	QFXCMD_SETVOLUME,
	QFXCMD_SETPAN,
	QFXCMD_SETPITCH,
};

// sound effect queue entry
typedef struct tagTqfxEntry
{
	int	command;				// command
	int channel;				// audio channel
	int volume;					// sample volume (0-127)
	int pan;					// sample pan (0-127)
	int pitch;					// sample playback frequency
	int sampleNum;				// sample number
	int sampleBank;				// sample bank number
	int	retries;
} TqfxEntry;


void qfxInit();

// shutdown the sound effect queue
void qfxShutdown();

// add entry to the queue
int qfxAddEntry(TqfxEntry *addEntry);

// delele entry from the queue
int qfxDelEntry();

// process queue entry
int qfxProcessCommand(TqfxEntry *entry);

// query the state of the queue.. need to check this..!!
int qfxQueryState();

// pump commands to the sound chip
int qfxPumpCommands();

int qfxFlush();

#endif