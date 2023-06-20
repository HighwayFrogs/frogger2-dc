#include <am.h>
#include "include.h"

extern AM_STREAM *gStream;
extern KTU32 *gPlayBuffer;
extern KTU32 gPlayBufferSize;

KTBOOL StreamSetup(char *filename, int loop);
void ReplayStream(void);

/*
#define MY_FILE_MAX_HANDLES				32
#define MAX_SFX_INSTANCES				64
#define MY_SFX_REVERB_CHANNEL			0
#define MY_SFX_QSOUND_CHANNEL			12
#define MY_SFX_NUMBER_OF_Q_CHANNELS		4

#define PLAY_BUFFER_SIZE			8192	// a reasonable default play buffer size
#define TRANSFER_BUFFER_SIZE		(PLAY_BUFFER_SIZE * 2)
#define STREAM_MAX_INSTANCES		2

extern GDFS	fileHandleArray[MY_FILE_MAX_HANDLES];

extern volatile KTU32 		gLastSH4ReadIndex;
extern AC_MESSAGE_MODE		gAcMessageMode;
extern volatile KTU32		gAcPollingRate;
extern volatile KTU32		gAcPollingRateCounter;
extern AM_SOUND 			gMySfxInstances[MAX_SFX_INSTANCES];

extern AM_ERROR_PTR			gMyAmError;
extern AC_ERROR_PTR			gMyAcError;
extern GDFS					fileHandleArray[MY_FILE_MAX_HANDLES];
extern AM_SOUND 			gMySfxInstances[MAX_SFX_INSTANCES];

extern KTU32 				gTransferBufferSize;
extern KTU32 				gPlayBufferSize;
extern volatile KTU32 		*gPlayBuffer;
extern volatile KTU32 		*gTransferBuffer;
extern AM_STREAM_PTR		gStream;
extern KTU32				gStreamVolume;

extern KTU32				gMyPlayBufferSize;
extern volatile KTBOOL		gStreamCallbackFlag;
extern volatile KTU32 		*gMyTransferBufferBaseAddress;
extern AM_STREAM			gMyStreamInstances[STREAM_MAX_INSTANCES];
extern AM_STREAM_PTR		gMyActiveStreams[STREAM_MAX_INSTANCES];
extern volatile KTBOOL		gMyStreamFlags[STREAM_MAX_INSTANCES];
extern AM_STREAM_ISR		gMyStreamIsrs[STREAM_MAX_INSTANCES];

KTBOOL 	_myStreamInstanceToIndex	(AM_STREAM_PTR theStream,volatile KTU32 *index);
KTBOOL 	_myStreamAddInstance		(AM_STREAM_PTR theStream);	// add an instance to the server queue
KTBOOL 	_myStreamRemoveInstance		(AM_STREAM_PTR theStream);	// remove an instance from the server queue
KTBOOL 	_myStreamFreeInstance		(AM_STREAM_PTR theStream);	// free a instance memory block
KTBOOL 	_myStreamAllocateInstance	(AM_STREAM_PTR *theStream); // allocate an instance from the pool

extern char music_names[][32];


KTBOOL MySh4Alloc(volatile KTU32 ** base,volatile KTU32 ** aligned,KTU32 size,KTU32 alignment);

void MySh4Free(volatile KTU32 * block);

KTBOOL	_MyFileAllocateHandle(ACFILE *fd);

void _MyFileFreeHandle(ACFILE fd);

void MyFileInit(void);

KTBOOL MyFileIoProc(KTSTRING fileName,ACFILE * fd,KTU8 * buffer,KTU32 * size,AM_FILE_OPERATION_MODE mode);

KTBOOL MyStreamIoProc(KTSTRING fileName,ACFILE * sd,KTU8 * buffer,KTU32 * size,AM_FILE_OPERATION_MODE mode);

KTBOOL MyFileLoad(KTSTRING fileName,volatile KTU32 *buffer);
 
KTBOOL MyFileRewind(ACFILE fd);

KTBOOL MyFileRead(ACFILE fd,KTU8 * buffer,KTU32 size);

KTBOOL MyFileOpen(KTSTRING fileName,ACFILE *fd);

KTBOOL MyFileClose(ACFILE fd);

KTBOOL MyFileGetSize(KTSTRING fileName, KTU32 * size);

KTBOOL MyAmSetup(KTBOOL usePolling);

void MyAmShutdown(void);

KTBOOL MyAllocateSfxInstance(AM_SOUND_PTR *theSound);

KTBOOL MySoundEffectIsPlaying(AM_SOUND_PTR theSound);
 
AM_BANK_PTR MyLoadBank(char *bankFileName);

KTBOOL MyInstallEffectsPatch(AM_BANK_PTR theBank, KTU32 effectsProgramBankIndex,KTU32 effectsOutputBankIndex);

KTBOOL MyStopSoundEffect(AM_SOUND_PTR theSound);

AM_SOUND_PTR MyPlaySoundEffect(AM_BANK_PTR theBank,KTU32 soundNumber,KTU32 volume,KTU32 pan);

AM_SOUND_PTR MyPlaySoundEffectFx(AM_BANK_PTR theBank,KTU32 soundNumber,KTU32 volume,KTU32 pan,KTU32 effectSourceMix,KTBOOL qSoundOnOrOff);

AM_STREAM_PTR MyStreamPrepareFile(char *streamFileName,KTU32 volume,KTU32 pan,volatile KTU32 *transferBuffer,volatile KTU32 *playBuffer,AM_USER_CALLBACK userCallback,AM_STREAM_TRANSFER_METHOD transferMethod);

// Streaming Rountines

KTBOOL _myStreamInstanceToIndex(AM_STREAM_PTR theStream,volatile KTU32 *index);

KTBOOL _myStreamAllocateInstance(AM_STREAM_PTR *theStream);

KTBOOL _myStreamFreeInstance(AM_STREAM_PTR theStream);

KTBOOL _myStreamAddInstance(AM_STREAM_PTR theStream);

KTBOOL _myStreamRemoveInstance(AM_STREAM_PTR theStream);

void MyStreamServer(void);

KTBOOL MyStreamDestroy(AM_STREAM_PTR theStream);

KTBOOL MyStreamDestroyBuffer(AM_STREAM_PTR theStream);

void MyStreamSetPlayBufferSize(KTU32 playBufferSize);

KTBOOL MyStreamGetBufferSpecs(char *streamFileName,KTU32 *transferBufferSize,KTU32 *playBufferSize,AM_STREAM_TRANSFER_METHOD transferMethod);

KTBOOL StreamSetup(char *filename);

KTBOOL MyStreamStop(AM_STREAM_PTR theStream);

KTBOOL MyStreamIsPlaying(AM_STREAM_PTR theStream);

KTBOOL MyStreamStart(AM_STREAM_PTR theStream);

KTBOOL MyStreamSetVolume(AM_STREAM_PTR theStream,KTU32 volume);

KTBOOL MyStreamRestart(AM_STREAM_PTR theStream,AM_USER_CALLBACK userCallback);

KTBOOL MyStreamSetMix(AM_STREAM_PTR theStream,AM_STREAM_MIX_PTR theMix);

AM_STREAM_PTR MyStreamPrepareBuffer(	volatile KTU32 *buffer,	
										KTU32 size,
										volatile KTU32 *playBuffer,
										KTU32 volume,
										KTU32 pan,
										KTU32 sampleRate,
										KTU32 bitDepth,
										AM_TRACK_TYPE monoOrStereo,
										AM_USER_CALLBACK userCallback);
*/