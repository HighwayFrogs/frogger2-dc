/*

	This file is part of Frogger2, (c) 1999 Interactive Studios Ltd.

	File		: audio.h
	Programmer	: Andy Eder
	Date		: 28/06/99 10:37:57

----------------------------------------------------------------------------------------------- */

#ifndef AUDIO_H_INCLUDED
#define AUDIO_H_INCLUDED
 
#define	MAX_CHANNELS	48

#include "newsfx.h" 
#include "types.h"
#include "actor2.h"
#include "am.h"

// //***********************************
// // Defines
 
// Hmm - three flags. Worth it?
#define SFXFLAGS_GENERIC		(1<<0)
#define SFXFLAGS_3D_SAMPLE		(1<<1)
#define SFXFLAGS_LOOP			(1<<2)

 
#define AMBIENT_VOLUME	255
#define SAMPLE_VOLUME	255

#define AUDIOTRK_GAMEOVER			100
#define AUDIOTRK_LEVELCOMPLETE		101
#define AUDIOTRK_LEVELCOMPLETELOOP	102

// All generic sfx
enum
{
	GEN_FROG_HOP,
	GEN_SUPER_HOP,
	GEN_DOUBLE_HOP,
	GEN_COLLECT_BABY,
	GEN_COLLECT_COIN,
	GEN_BABYHAPPY,
	GEN_BABYSAD,
	GEN_BABYCRY,
	GEN_BABYREPLY,
	GEN_TELEPORT,
	GEN_POWERUP,
	GEN_CLOCKTICK,
	GEN_POWERTICK,

	GEN_DEATHDROWN,
	GEN_DEATHCRUSH,
	GEN_DEATHEXPLODE,
	GEN_DEATHFALL,
	GEN_DEATHGIB,
	GEN_DEATHMOWED,
	GEN_DEATHCHOP,
	GEN_DEATHELECTRIC,
	GEN_DEATHFIRE,

	NUM_GENERIC_SFX,
}; 

// //***********************************
// // Type Defines
 
// typedef struct _SAMPLE
// {
// 	struct _SAMPLE			*next, *prev;
// 
// 	char					*idName;
// 	unsigned long			flags;
// 	unsigned long			uid;
// 
// 	LPWAVEFORMATEX			lpWavFmt;
// 	BYTE					*data;
// 	DWORD					len;
// 
// 	LPDIRECTSOUNDBUFFER		lpdsBuffer;	
//     LPDIRECTSOUND3DBUFFER   lpds3DBuffer;	// DirectSound 3D buffer interface
// 
// } SAMPLE;

typedef struct _SAMPLE
{
	unsigned long			flags;
	
	char					idName[32];
	unsigned long			uid;

	SfxSampleType			*snd;
	
	AM_BANK_PTR				bankPtr;
	unsigned long			sampleNumber;
	
	int						handle;
} SAMPLE;

#define MAX_SAMPLES 100
#define MAX_VOICES 12 
 
typedef struct _SOUNDLIST
{
	SfxBankType		*genericBank;
	SfxBankType		*levelBank;
	SfxBankType		*loopBank;
	SfxBankType		*voiceBank[4];
	
	short count;
	SAMPLE			array[MAX_SAMPLES];
	
} SOUNDLIST;

typedef struct TAG_AMBIENT_SOUND
{
	struct TAG_AMBIENT_SOUND *next,*prev;

	SVECTOR		pos;
	ACTOR		*follow;

	struct _SAMPLE *sample;

	short		volume;
	short		pitch;
	long		radius;

	long		freq;
	long		randFreq;
	long		counter;
	int			handle;
}AMBIENT_SOUND;
 
 
typedef struct
{
	int array;
	AMBIENT_SOUND head;
	int numEntries;

}AMBIENT_SOUND_LIST; 

typedef struct {
	AM_SOUND			sound;
	SfxSampleType		*sample;
	int					volume;
	int					pan;
} CurrentData;
 
extern SAMPLE *genSfx[];

extern CurrentData   current[MAX_CHANNELS];

extern AMBIENT_SOUND_LIST	ambientSoundList;

//***********************************
// Function Prototypes
 
// extern void LoadSfxMapping( int world, int level );
// extern SAMPLE **FindSfxMapping( unsigned long uid, ACTOR *actor );
 
// extern void LoadSfx( unsigned long worldID );
 
// extern void InitSampleList( );
// extern void FreeSampleList( );
 
// extern void InitAmbientSoundList( );
// extern void FreeAmbientSoundList( );
 
// extern void FreeBufSampleList( );
 
// extern SAMPLE *FindSample( unsigned long uid );
 
// extern void CleanBufferSamples ( void );
// extern void SetSampleFormat ( SAMPLE *sample );
 
 
// extern int PlaySample(SAMPLE *sample,VECTOR *pos,long radius,short volume,short pitch);
// void PlaySfxMappedSample( ACTOR *act, long radius, short volume, short pitch );
// extern AMBIENT_SOUND *AddAmbientSound(SAMPLE *sample,VECTOR *pos,long radius,short volume,short pitch,float freq,float rFreq,ACTOR *follow);
// extern void UpdateAmbientSounds();
 
 
// extern void PrepareSongForLevel( short worldID, short levelID );


// #define UpdateAmbientSounds()
// //#define PTTextureLoad()
// #define FreeAmbientSoundList()
// #define InitAmbientSoundList()

#define PlayVoice(pl, sound) PlaySample(FindVoice(utilStr2CRC(sound),pl), NULL, 0, SAMPLE_VOLUME, -1)

void audioError();

void StartSound();
void StopSound();
void InitSampleList( );
int LoadSfxSet(char *path, SfxBankType **sfxBank,int flags,SAMPLE *array,short *count);
int LoadSfx( long worldID );
SAMPLE *CreateAndAddSample(SfxSampleType *snd,int flags,SAMPLE *array,short *count);
SAMPLE *FindSample( unsigned long uid );
void FreeSampleList( );
int PlaySample( SAMPLE *sample, SVECTOR *pos, long radius, short volume, short pitch );
SAMPLE *FindVoice( unsigned long uid, int pl );
AMBIENT_SOUND *AddAmbientSound(SAMPLE *sample, SVECTOR *pos, long radius, short vol, short pitch, float freq, float randFreq, ACTOR *follow );
void UpdateAmbientSounds();
void InitAmbientSoundList();
void SubAmbientSound(AMBIENT_SOUND *ambientSound);
void FreeAmbientSoundList( );
void PrepareSong(short worldID, int loop);
void StopSong( );
void PauseAudio( );
void UnPauseAudio( );

int IsSongPlaying();

int sfxPlaySample(SfxSampleType *sample, int volL, int volR, int pitch);

void sfxStopChannel(int channel);
void sfxSetChannelPitch(int channel, int pitch);
void sfxSetChannelVolume(int channel, int volL, int volR);
void sfxStartSound();
void StopSample(SAMPLE *sample);
void sfxStopSound();

void UpdateAmbientSoundsVolume();
void UpdateAmbientSoundsPlay();

void StartAmbientSoundsPlay();

int PlaySampleNoSoundSet( SAMPLE *sample, SVECTOR *pos, long radius, short volume, short pitch );


#endif
