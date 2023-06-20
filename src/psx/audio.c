/*

	This file is part of Frogger2, (c) 1999 Interactive Studios Ltd.

	File		: audio.c
	Programmer	: Random Healey
	Date		: 28/06/99 10:37:45

----------------------------------------------------------------------------------------------- */

// #include <ultra64.h>
// #include "..\resource.h"
// #include "incs.h"
#include <islmem.h>

#include <audio.h>

#include "main.h"

#include "memload.h"
#include "types.h"
#include "frogger.h"
#include "layout.h"

#include "am_audio.h"
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
#include "menus.h"
#include "game.h"
#include "cam.h"
#include "options.h"
#include "islxa.h"
#include "QueueSfx.h"

// needed for sfxPrintf
//#if 0
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
//#include <malloc.h>
#include <stdarg.h>
//#include <libsn.h>
#include <sn_fcntl.h>   /* LibCross file types. */
#include <usrsnasm.h>   /* LibCross I/O routines. */
#include "fixed.h"
//#endif

extern XAFileType	*curXA;

int lastSound = -1;

char *musicNames[] = {	"CDA.XA",
						"CDB.XA",
					 };

int musicList[] = { 0,//
					1,//
					2,//
					6,//
					3,//
					4,//
					5,//
					6,//
					7,
					0,
					1,
					2,
};//

#define SFX_BASE		"SFX\\"

 
//#define MAX_AMBIENT_SFX		50
#define DEFAULT_SFX_DIST	700*SCALE

//This value means the pitch can go up to 100000Hz like the PC. This might be a 
//problem for the PSX but I don't think it goes much above 20000Hz, which seems alright. May
//cause problems in the future though, so check!!
#define PITCH_STEP		190 //(DSBFREQUENCY_MAX/256)   //MM NEEDS CHANGING!!!!!

#define MAX_AMBIENT_SAMPLES	8
#define AMBIENT_WAIT		1
 
//#define VOLUME_MIN		-5000
//#define VOLUME_PERCENT (VOLUME_MIN/100)
//#define PITCH_STEP		(DSBFREQUENCY_MAX/256)

SAMPLE voiceArray[4][MAX_VOICES];
 
SAMPLE *genSfx[NUM_GENERIC_SFX];
// SAMPLE **sfx_anim_map = NULL;

//XAFileType	*xaFileData[NUM_CD_TRACKS];
 
// UINT mciDevice = 0;
 
SOUNDLIST soundList;					// Actual Sound Samples List
AMBIENT_SOUND_LIST	ambientSoundList;

int GetSoundVols(SVECTOR *pos,int *vl,int *vr,long radius,unsigned long vol);

int byteToDecibelLUT[256] = 
{
	//0-15
      -10000,-7992,-6992,-6407,-5992,-5670,-5407,-5185,-4992,-4823,-4671,-4533,-4408,-4292,-4185,-4086,
	//16-31
	-3993,-3905,-3823,-3745,-3671,-3601,-3533,-3470,-3408,-3350,-3293,-3239,-3186,-3135,-3086,-3039,
	//32-47
	-2993,-2949,-2906,-2864,-2823,-2784,-2745,-2708,-2671,-2636,-2601,-2567,-2534,-2501,-2470,-2439,
	//48-63
	-2408,-2378,-2349,-2321,-2293,-2265,-2238,-2212,-2186,-2160,-2135,-2111,-2086,-2063,-2039,-2016,
	//64-79
	-1993,-1971,-1949,-1927,-1906,-1885,-1864,-1844,-1823,-1804,-1785,-1765,-1745,-1727,-1708,-1690,
	//80-95
	-1671,-1654,-1636,-1618,-1601,-1584,-1567,-1550,-1534,-1518,-1502,-1486,-1470,-1454,-1439,-1424,
	//96-111
	-1408,-1394,-1379,-1364,-1350,-1335,-1321,-1307,-1293,-1279,-1266,-1252,-1239,-1225,-1212,-1199,
	//112-127
	-1186,-1173,-1161,-1148,-1136,-1123,-1111,-1099,-1087,-1075,-1063,-1051,-1039,-1028,-1016,-1005,
	//128-143
	-994,-982,-971,-960,-950,-938,-928,-917,-906,-896,-885,-875,-864,-854,-844,-834,
	//144-159
	-824,-814,-804,-794,-784,-774,-765,-755,-746,-736,-727,-718,-708,-699,-690,-681,
	//160-175
	-672,-663,-654,-645,-636,-627,-619,-610,-601,-593,-585,-576,-568,-560,-551,-543,
	//176-191
	-535,-527,-518,-510,-502,-494,-486,-478,-471,-463,-455,-447,-440,-432,-424,-417,
	//192-207
	-409,-402,-394,-387,-380,-372,-365,-358,-350,-343,-336,-329,-322,-315,-308,-301,
	//208-223
	-294,-287,-280,-273,-266,-260,-253,-246,-240,-233,-226,-220,-213,-206,-200,-193,
	//224-239
	-187,-181,-174,-168,-161,-155,-149,-142,-136,-130,-124,-118,-112,-106,100,-93,
	//240-255
	-87,-81,-75,-70,-64,-58,-52,-46,-40,-34,-28,-22,-17,-11,-5,0
};

//#if 0
int sfxPrintf(char* fmt, ...)
{	
    va_list	arglist;
    long	r1, r2, r3, r4, r5, r6;
    int		len;
	static	char buffer[260];

	return 0;

	va_start(arglist, fmt);
    r1 = va_arg(arglist, long);
    r2 = va_arg(arglist, long);
    r3 = va_arg(arglist, long);
    r4 = va_arg(arglist, long);
    r5 = va_arg(arglist, long);
    r6 = va_arg(arglist, long);
	va_end(arglist);
    len = sprintf(buffer, fmt, r1, r2, r3, r4, r5, r6);

	debug_printf(buffer);
    return len;
}
//#endif

extern AM_ERROR *amErr;
int sfxPrintf(char* fmt, ...);

void audioError()
{
	if(amErr->number != AM_NO_ERROR && amErr->number != 401)
	{
		sfxPrintf("AM Error #: %u\n", amErr->number);
		sfxPrintf("Message: %s\n", amErr->message);
	}
}

long sampleWaitCounter = 0;

int UpdateLoopingSampleOld(AMBIENT_SOUND *sample)
{
	int vl,vr,vs;
	short pitch;

	if (!sample)
	{
		utilPrintf("Sample Not Valid!!!!!!\n");
		return 0;
	}

	if(GetSoundVols(&sample->pos,&vl,&vr,sample->radius ,sample->volume) == -1)
	{
		if(sample->handle >= 0)
		{
			sfxStopChannel(sample->handle);
			sample->handle = -1;
		}
		return TRUE;
	}
	
	if(sample->pitch == -1)
	{
		pitch = 0;
	}
	else
	{
		pitch = sample->pitch * PITCH_STEP;
	}

	if((sample->handle == -1) && (sample->sample) && (sample->sample->snd))
	{
 		lastSound = sample->handle = sfxPlaySample(sample->sample->snd, vl,vr, pitch);
		return TRUE;
	}

	// invalid samples..
	if (sample->handle == -1)
		return 0;
	if (sample->sample == NULL)
		return 0;
	if (sample->sample->snd == NULL)
		return 0;

//	if(pitch)
//		sfxSetChannelPitch(sample->handle,pitch);
	if(vr == 0)
		vr = 1;
	if(vl == 0)
		vl = 1;
	sfxSetChannelVolume(sample->handle, vl, vr);

	return 0;
}

int UpdateLoopingSample(AMBIENT_SOUND *sample)
{
	int vl,vr,vs;
	short pitch;

	if (!sample)
	{
		utilPrintf("Sample Not Valid!!!!!!\n");
		return 0;
	}

	GetSoundVols(&sample->pos,&vl,&vr,sample->radius ,sample->volume);
	
/*	if(GetSoundVols(&sample->pos,&vl,&vr,sample->radius ,sample->volume) == -1)
	{
		if(sample->handle >= 0)
		{
			sfxStopChannel(sample->handle);
			sample->handle = -1;
		}
		return TRUE;
	}
*/	
	if(sample->pitch == -1)
	{
		pitch = 0;
	}
	else
	{
		pitch = sample->pitch * PITCH_STEP;
	}

/*	if((sample->handle == -1) && (sample->sample) && (sample->sample->snd))
	{
 		lastSound = sample->handle = sfxPlaySample(sample->sample->snd, vl,vr, pitch);
		return TRUE;
	}
*/
	// invalid samples..
	if (sample->handle == -1)
		return 0;
	if (sample->sample == NULL)
		return 0;
	if (sample->sample->snd == NULL)
		return 0;

	if(pitch)
		sfxSetChannelPitch(sample->handle,pitch);
	if(vr == 0)
		vr = 1;
	if(vl == 0)
		vl = 1;

	sfxSetChannelVolume(sample->handle, vl, vr);

	return 0;
}

/*	--------------------------------------------------------------------------------
	Function		:StartSound
	Purpose		    :  Initialises the PSX sound chip and the soundlibraries
	Parameters		: none
	Returns			: 
	Info			: Should only be called once at the start of the game loop. 
*/
void StartSound()
{
	sfxInitialise(0);
 	sfxStartSound();
 	sfxOn();
 	sfxSetGlobalVolume(255);// set to max volume 
}

/*	--------------------------------------------------------------------------------
	Function		:StopSound
	Purpose		    :  Turns off the PSX sound chip and the soundlibraries
	Parameters		: none
	Returns			: 
	Info			: Should only be called once at the end of the game loop. 
*/
void StopSound()
{
	FreeSampleList();
	
	sfxOff();
	sfxStopSound();
	sfxDestroy();
}

/*	--------------------------------------------------------------------------------
	Function		: InitSampleList
	Purpose			: Initialises the sample list
	Parameters		: 
	Returns			: 
	Info			: 
*/
void InitSampleList( )
{
	//Init the sample list for the real samples.

	int i;
/*
	if(soundList.genericBank)
	{
		sfxUnloadSampleBank(soundList.genericBank);
		sfxRemoveSampleBank(soundList.genericBank,1);
		soundList.genericBank = NULL;
	}
	if(soundList.levelBank)
	{
		sfxUnloadSampleBank(soundList.levelBank);
		sfxRemoveSampleBank(soundList.levelBank,1);
		soundList.levelBank = NULL;
	}
	if(soundList.loopBank)
	{
		sfxUnloadSampleBank(soundList.loopBank);
		sfxRemoveSampleBank(soundList.loopBank,1);
		soundList.loopBank = NULL;
	}
	for(i = 0;i < 4;i++)
	{
		if(soundList.voiceBank[i])
		{
			sfxUnloadSampleBank(soundList.voiceBank[i]);
			sfxRemoveSampleBank(soundList.voiceBank[i],1);
			soundList.voiceBank[i] = NULL;
		}
	}
*/
	soundList.count = 0;
}

/*	--------------------------------------------------------------------------------
	Function		: LoadSfxSet
	Purpose			: Loads the soundbank and adds samples to list
	Parameters		: path to be loaded, adn whether the bank is the generic bank
	Returns			: Success
	Info			: 
*/

#define MAX_SAMPLE_BANKS	6

SfxBankType			sampleBanks[MAX_SAMPLE_BANKS];
AM_BANK_PTR			audio64Banks[MAX_SAMPLE_BANKS];

int	numSoundBanks = 0;

CurrentData			current[MAX_CHANNELS];

int					sfxGlobalVolume=255,sfxOutputOn;
int					oldSfxGlobalVolume;

int LoadSfxSet(char *path, SfxBankType **sfxBank,int flags,SAMPLE *array,short *count)
{
	int 			i;
	SAMPLE 			*sfx;
	SfxBankType		*bank;
	AM_BANK_PTR		am_bank;
	SAMPLE 			*newItem;
	int				numSamples,loop,bytesRead;
	AM_SOUND		soundInfo;
	char			filename[256],*fPtr;
	char			*sampleList,*sListPtr;
	SfxSampleType 	*snd;
	AM_BANK_PTR		smpPtr;
	long			sizeInBytes;
	
	sprintf(filename,"%s.kat",path);
	if((am_bank = bpAmLoadBank(filename))==KTNULL)
		return 0;

	bank = &sampleBanks[numSoundBanks];
	audio64Banks[numSoundBanks] = am_bank;
	
	//Allocate memory for sample data and grab data from .kat file
	amBankGetNumberOfAssets(am_bank,(KTU32 *)&numSamples);

	//malloc the bank all in one then fill it in
	bank->numSamples = numSamples;
	bank->sample = (SfxSampleType *) syMalloc(sizeof(SfxSampleType)*numSamples);
	bank->baseAddr = (unsigned long) am_bank;

	smpPtr = am_bank;
	for(loop=0; loop<bank->numSamples; loop++)
	{
		amSoundFetchSample(audio64Banks[numSoundBanks],loop,&soundInfo);
//		amSoundFetchSample(audio64Banks[numSoundBanks],0,&soundInfo);
		bank->sample[loop].sampleData		= smpPtr;
		bank->sample[loop].sampleSize		= soundInfo.sizeInBytes;
		bank->sample[loop].aicaSampleType	= soundInfo.aicaSampleType;
		bank->sample[loop].aicaLoopFlag		= soundInfo.aicaLoopFlag;
		bank->sample[loop].sampleRate		= soundInfo.sampleRate;
		bank->sample[loop].bankNumber		= numSoundBanks;
		bank->sample[loop].sampleNumber		= loop;
		bank->sample[loop].inSPURam			= 1;//Always loaded on DC
		bank->sample[loop].pad				= 0;

		// add the size in bytes and align 32 it
		sizeInBytes = (((long) soundInfo.sizeInBytes) + 0x1F) & 0xFFFFFFE0;
		smpPtr += sizeInBytes;
	}

	//Need name CRC's unfortunately so grab these from a seperate file
	sprintf(filename,"sfx\\%s.txt",path);
	
	sampleList = (char*)fileLoad(filename,&bytesRead);
	sListPtr = sampleList;

	for(loop=0; loop<bank->numSamples; loop++)
	{
		for(fPtr = filename; ((*sListPtr)!='\0') && ((*sListPtr)!='\n'); fPtr++)
		{
			*fPtr = *(sListPtr++);
		}
		fPtr -= 5;			// Take off the .wav extension and the carriage return which seems to have squeezed in
		*fPtr = '\0';
		while(*sListPtr =='\n') sListPtr++;

		bank->sample[loop].nameCRC = utilStr2CRC(filename);
		strcpy(bank->sample[loop].name,filename);
	}

	Align32Free(sampleList);

	snd = (SfxSampleType *) bank->sample;

	for(i=0;i<bank->numSamples;i++)
	{
		if(!snd)
		{
			utilPrintf("oops...can't find sample %i in bank %s\n",i,path);
			continue;
		}
		newItem = array + *count;
		newItem->uid = snd->nameCRC;
		newItem->snd = snd;	
		newItem->flags = flags;
		newItem->bankPtr = am_bank;
		newItem->sampleNumber = *count;
		strcpy(newItem->idName,snd->name);
		
		(*count)++;
			
		*snd++;
	}

	numSoundBanks++;

	return 1;
}

/*	--------------------------------------------------------------------------------
	Function		: LoadSfx
	Purpose			: Loads a sound bank, and puts all the samples in the sample list
	Parameters		: number of world sound bank to load
	Returns			: Success?
	Info			: 
*/
short voiceCount[4];
int LoadSfx(long worldID )
{
	char path[256];
	int j;	

	path[0] = 0;
	if(worldID != -1)
	{
		for(j = 0;j < NUM_FROGS;j++)
		{
			voiceCount[j] = 0;
			LoadSfxSet(frogPool[player[j].character].fileName, &soundList.voiceBank[j],0,&voiceArray[j][0],&voiceCount[j]);
		}
	}

	if ( worldID == 0xffff )
	{
		return 0;
	}
	else if(gameState.multi == SINGLEPLAYER)
	{
		switch( worldID )
		{
			case WORLDID_GARDEN: sprintf( path, "GARDEN" ); break;
			case WORLDID_ANCIENT: sprintf( path, "ANCIENTS" ); break;
			case WORLDID_SPACE: sprintf( path, "SPACE" ); break;
			case WORLDID_CITY: sprintf( path, "CITY" ); break;
			case WORLDID_SUBTERRANEAN: sprintf( path, "SUB" ); break;
			case WORLDID_LABORATORY: sprintf( path, "LAB" ); break;
			case WORLDID_HALLOWEEN: sprintf( path, "HALLOWEEN" ); break;
			case WORLDID_SUPERRETRO: sprintf( path, "SUPERRETRO" ); break;
			case WORLDID_FRONTEND: sprintf( path, "FRONTEND" ); break;
		}
	}
	else
		sprintf( path, "MULTI" );

	LoadSfxSet(path, &soundList.levelBank,0,&soundList.array[0],&soundList.count);

	genSfx[GEN_FROG_HOP] = FindSample(utilStr2CRC("hopongrass"));
	genSfx[GEN_SUPER_HOP] = FindSample(utilStr2CRC("hop2"));
	genSfx[GEN_DOUBLE_HOP] = FindSample(utilStr2CRC("doublehop"));
	genSfx[GEN_COLLECT_BABY] = FindSample(utilStr2CRC("getbabyfrog"));
	genSfx[GEN_COLLECT_COIN] = FindSample(utilStr2CRC("pickupcoin"));
	genSfx[GEN_BABYHAPPY] = FindSample(utilStr2CRC("babyhappy"));
	genSfx[GEN_BABYSAD] = FindSample(utilStr2CRC("babysad"));
	genSfx[GEN_BABYCRY] = FindSample(utilStr2CRC("babycry"));
	genSfx[GEN_BABYREPLY] = FindSample(utilStr2CRC("babyreply"));
	genSfx[GEN_TELEPORT] = FindSample(utilStr2CRC("teleport"));

	genSfx[GEN_POWERUP] = FindSample(utilStr2CRC("powerup"));

	genSfx[GEN_CLOCKTICK] = FindSample(utilStr2CRC("clocktick"));
	genSfx[GEN_POWERTICK] = FindSample(utilStr2CRC("puptick"));

	genSfx[GEN_DEATHDROWN] = FindSample(utilStr2CRC("frogdrown1"));
	genSfx[GEN_DEATHCRUSH] = FindSample(utilStr2CRC("frogcrush"));
	genSfx[GEN_DEATHEXPLODE] = FindSample(utilStr2CRC("frogexplode"));
	genSfx[GEN_DEATHFALL] = FindSample(utilStr2CRC("frogfall"));
	genSfx[GEN_DEATHMOWED] = FindSample(utilStr2CRC("frogmowed"));
	genSfx[GEN_DEATHGIB] = FindSample(utilStr2CRC("frog_gib"));
	genSfx[GEN_DEATHCHOP] = FindSample(utilStr2CRC("frogchop"));
	genSfx[GEN_DEATHELECTRIC] = FindSample(utilStr2CRC("electrocute"));
	genSfx[GEN_DEATHFIRE] = FindSample(utilStr2CRC("burnbum"));

	return 1;
}



/*	--------------------------------------------------------------------------------
	Function		:CreateAndAddSamples
	Purpose			: Adds a sample to the sample list
	Parameters		: The sound effect to be added to the sample structure
	Returns			: 
	Info			: 
*/
SAMPLE *CreateAndAddSample(SfxSampleType *snd,int flags,SAMPLE *array,short *count)
{
	SAMPLE *newItem;
	
	if(( soundList.count >= MAX_SAMPLES ) || ( !( newItem = array + *count)))
		return NULL;

	//sfx = (SAMPLE *)MALLOC0(sizeof(SAMPLE));

	newItem->uid = snd->nameCRC;
	newItem->snd = snd;
	newItem->flags = flags;

	(*count)++;

	return newItem;
}

/*	--------------------------------------------------------------------------------
	Function		: FindSample
	Purpose			: Search the list for the required sound
	Parameters		: ID
	Returns			: Sample or NULL
	Info			: 
*/

SAMPLE *FindSample( unsigned long uid )
{
	int i;

	if (!uid)
		return NULL;

	for(i = 0;i < soundList.count;i++)
	{
		if(soundList.array[i].uid == uid)
			return &soundList.array[i];
	}

	return NULL;	
}

void FreeSampleBlock( )
{
	soundList.count = 0;
}

/*	--------------------------------------------------------------------------------
	Function		:FreeSampleList
	Purpose			:Clears the whole sample list and the PSX sound memory
	Parameters		: 
	Returns			: 
	Info			: 
*/

void FreeSampleList( )
{
	int		i;
	SAMPLE 	*cur,*next;
	KTBOOL	ktflag;
	KTU32	memfreeBefore,memfreeAfter;

	amHeapGetFree(&memfreeBefore);
	
	FreeSampleBlock( );

	for(i=(numSoundBanks-1);i>=0;i--)
	{
		ktflag = amHeapFree((unsigned long*)audio64Banks[i]);
		syFree(sampleBanks[i].sample);
	}
	numSoundBanks = 0;

	amHeapGetFree(&memfreeAfter);

	// initialise list for future use
	InitSampleList();					
}

/*	--------------------------------------------------------------------------------
	Function		: PlaySample
	Purpose			: plays a sample
	Parameters		: ID, position, radius, volume, pitch
	Returns			: success?
	Info			: Pass in a valid vector to get attenuation, and a radius to override the default
*/

int sfxwaittime = 5;
int PlaySample( SAMPLE *sample, SVECTOR *pos, long radius, short volume, short pitch )
{
	int vl,vr;
	int vs;

//	return;

	// my vsync counter to only play new sfx after a frame
	if (!sample)
	{
		utilPrintf("Sample Not Valid!!!!!!\n");
		return 0;
	}


//Stuff to ensure calls to PlaySample can be the same for PC and PSX
	if (pitch ==-1)
	{
		pitch = 0;
	}
	else
	{
		pitch *= PITCH_STEP;
	}
//end of stuff

	if( pos )
	{
		if(GetSoundVols(pos,&vl,&vr,radius,volume) == -1)
			return -1;
	}
	else
	{
		vr = vl = (volume*globalSoundVol)/MAX_SOUND_VOL;
	}

	if ( !sample->snd )
		return 0;

//	vs = VSync(1);
//	while((lastSound>=0) && (SpuGetKeyStatus(1<<lastSound)==SPU_ON_ENV_OFF) && (VSync(1)<vs+sfxwaittime));

//	if(sample->flags)
//		utilPrintf("playing looping sample the wrong way!!!!!!!!\n");
	
	sample->handle = lastSound = sfxPlaySample( sample->snd, vl,vr, pitch);
	if(lastSound<0)
		utilPrintf("SOUND NOT WORKED (%i RETURNED)\n",lastSound);
		
	return lastSound;	
}

/*	--------------------------------------------------------------------------------
	Function		: PlaySampleNoSoundSet
	Purpose			: plays a sample
	Parameters		: ID, position, radius, volume, pitch
	Returns			: success?
	Info			: Pass in a valid vector to get attenuation, and a radius to override the default
*/

int PlaySampleNoSoundSet( SAMPLE *sample, SVECTOR *pos, long radius, short volume, short pitch )
{
	int vl,vr;
	int vs;

	// my vsync counter to only play new sfx after a frame
	if (!sample)
	{
		utilPrintf("Sample Not Valid!!!!!!\n");
		return 0;
	}

	//Stuff to ensure calls to PlaySample can be the same for PC and PSX
	if (pitch ==-1)
	{
		pitch = 0;
	}
	else
	{
		pitch *= PITCH_STEP;
	}
	//end of stuff
	
	if( pos )
	{
		GetSoundVols(pos,&vl,&vr,radius,volume);
	}
	else
	{
		vr = vl = (volume*globalSoundVol)/MAX_SOUND_VOL;
	}

	if ( !sample->snd )
		return 0;

	sample->handle = lastSound = sfxPlaySample( sample->snd, vl,vr, pitch);
	if(lastSound<0)
	{
		utilPrintf("SOUND NOT WORKED (%i RETURNED)\n",lastSound);
//		sfxPrintf("SOUND NOT WORKED (%i RETURNED)", lastSound);
	}
		
	return lastSound;	
}

 /*	--------------------------------------------------------------------------------
 	Function 	: AddAmbientSound
 	Purpose 	: Create an ambient sound
 	Parameters 	: 
 	Returns 	: 
 	Info 		:
 */

 AMBIENT_SOUND *AddAmbientSound(SAMPLE *sample, SVECTOR *pos, long radius, short vol, short pitch, float freq, float randFreq, ACTOR *follow )
 {
 	AMBIENT_SOUND *ptr;
 	AMBIENT_SOUND *ambientSound;

//	if(strcmp("lp_water",sample->idName) != 0)
//		return;

//	if(ambientSoundList.numEntries >= 4)
//		return;

 	if( !sample )
 		return NULL;
 
 	ambientSound = (AMBIENT_SOUND *)MALLOC0(sizeof(AMBIENT_SOUND));
 	ptr = &ambientSoundList.head;
 
 	if( pos ) SetVectorSS( &ambientSound->pos, pos );
 	if( follow ) ambientSound->follow = follow;
 
 	ambientSound->volume = vol;
 	ambientSound->sample = sample;
 	ambientSound->pitch = pitch;

	// ** We need to limit the radius of the ambient sound effects for the following
	// ** levels to prevent the game from crashing! We believe the crashes could be
	// ** happening when >15 ambient channels are used AND we simply haven't the time
	// ** to fix it.


/*	// limit the radius on offending levels
	if ((player[0].worldNum == WORLDID_SUBTERRANEAN && player[0].levelNum == LEVELID_SUBTERRANEAN1))
	 	ambientSound->radius = (long)(((float)radius) * 0.65f);
	else if ((player[0].worldNum == WORLDID_SUBTERRANEAN && player[0].levelNum == LEVELID_SUBTERRANEAN3)||		 
		(player[0].worldNum == WORLDID_HALLOWEEN && player[0].levelNum == LEVELID_HALLOWEEN3)||
		(player[0].worldNum == WORLDID_LABORATORY && player[0].levelNum == LEVELID_LABORATORY3))
	 	ambientSound->radius = (long)(((float)radius) * 0.5f);
	else
*/	 	ambientSound->radius = radius;
 
 //	ambientSound->radius = (long)(((float)radius) * 0.5f);
 
 	ambientSound->freq = freq*60;
 	ambientSound->randFreq = randFreq*60;
 
 	ambientSound->prev = ptr;
 	ambientSound->next = ptr->next;
 	ptr->next->prev = ambientSound;
 	ptr->next = ambientSound;
 	ambientSoundList.numEntries++;

	ambientSound->handle = -1;

 	return ambientSound;
 }
 
 
 /*	--------------------------------------------------------------------------------
 	Function 	: UpdateAmbientSounds
 	Purpose 	: Start sound effect if close enough, if timeout, etc
 	Parameters 	: 
 	Returns 	: 
 	Info 		:
 */
 void UpdateAmbientSounds()
 {
 	AMBIENT_SOUND *amb,*amb2;
 	SVECTOR *pos;

	sampleWaitCounter++;

 	// Update each ambient in turn
 	for( amb = ambientSoundList.head.next; amb != &ambientSoundList.head; amb = amb2 )
 	{
 		amb2 = amb->next;
 
 		// Timeout, so play a new sound
 		if( actFrameCount < amb->counter )
 			continue;
 
 		// If it is attached to a platform, make ambient follow that platform
 		if( amb->follow )
 			SetVectorSS( &amb->pos, &amb->follow->position );
 
 		// If sound doesn't have a source
 //		if( MagnitudeSquared(&amb->pos) )	//PUT BACK IN?!?!
 //			pos = NULL;	//PUT BACK IN?!?!
 //		else	//PUT BACK IN?!?!
 			pos = &amb->pos;
 
		if(!(amb->sample->snd->pad & 1))
		{	
			if(PlaySample(amb->sample, &amb->pos, amb->radius, amb->volume, amb->pitch ) != -1)
			{
				amb->sample->snd->pad = 1;
				amb->handle = amb->sample->handle;
			}
		}
		else
		{
			UpdateLoopingSampleOld(amb);
		}
		 
 		// Freq and randFreq are cunningly pre-multiplied by 60
 		amb->counter = actFrameCount + amb->freq + ((amb->randFreq)?Random(amb->randFreq):0);
 	} 	
 }
 

  /*	--------------------------------------------------------------------------------
 	Function 	: UpdateAmbientSoundsVolume
 	Purpose 	: Start sound effect if close enough, if timeout, etc
 	Parameters 	: 
 	Returns 	: 
 	Info 		:
 */

 void UpdateAmbientSoundsVolume()
 {
	 int	vl,vr,pitch;	
	 AMBIENT_SOUND *amb,*amb2;
 	 SVECTOR *pos;

 	// Update each ambient in turn
 	for( amb = ambientSoundList.head.next; amb != &ambientSoundList.head; amb = amb2 )
 	{
 		amb2 = amb->next;

 		// If it is attached to a platform, make ambient follow that platform
 		if( amb->follow )
 			SetVectorSS( &amb->pos, &amb->follow->position );

 		// Timeout, so play a new sound
//		if( actFrameCount < amb->counter )
// 			continue;
 
		UpdateLoopingSample(amb);
		
/*		if(amb->sample->snd->pad & 1)
		{	
			if(UpdateLoopingSample(amb))
			{
		 		amb->counter = actFrameCount + amb->freq + ((amb->randFreq)?Random(amb->randFreq):0);	
			}
		}
*/		 
 		// Freq and randFreq are cunningly pre-multiplied by 60
 		amb->counter = actFrameCount + amb->freq + ((amb->randFreq)?Random(amb->randFreq):0);
 	} 	
 }

 /*	--------------------------------------------------------------------------------
 	Function 	: UpdateAmbientSoundsPlay
 	Purpose 	: Start sound effect if close enough, if timeout, etc
 	Parameters 	: 
 	Returns 	: 
 	Info 		:
 */
 void UpdateAmbientSoundsPlay()
 {
 	AMBIENT_SOUND *amb,*amb2;
 	SVECTOR *pos;

 	// Update each ambient in turn
 	for( amb = ambientSoundList.head.next; amb != &ambientSoundList.head; amb = amb2 )
 	{
 		amb2 = amb->next;
 
 		// Timeout, so play a new sound
 		if( actFrameCount < amb->counter )
 			continue;
 
 		// If it is attached to a platform, make ambient follow that platform
 		if( amb->follow )
 			SetVectorSS( &amb->pos, &amb->follow->position );
 
 		// If sound doesn't have a source
		pos = &amb->pos;
 
		if(!(amb->sample->snd->pad & 1))
		{	
			if(PlaySample(amb->sample, &amb->pos, amb->radius, amb->volume, amb->pitch ) != -1)
			{
				amb->sample->snd->pad = 1;
				amb->handle = amb->sample->handle;
				return;
			}
		}
 	} 	
 }
 
 /*	--------------------------------------------------------------------------------
 	Function 	: StartAmbientSoundsPlay
 	Purpose 	: Start sound effect if close enough, if timeout, etc
 	Parameters 	: 
 	Returns 	: 
 	Info 		:
 */

 void StartAmbientSoundsPlay()
 {
	int i;
 	AMBIENT_SOUND *amb,*amb2;
 	SVECTOR *pos;

 	// Update each ambient in turn
 	for( amb = ambientSoundList.head.next; amb != &ambientSoundList.head; amb = amb2 )
 	{
 		amb2 = amb->next;
 
 		// If it is attached to a platform, make ambient follow that platform
 		if( amb->follow )
 			SetVectorSS( &amb->pos, &amb->follow->position );
 
 		// If sound doesn't have a source
		pos = &amb->pos;

		acSystemDelay(500000);
 		
		if(PlaySampleNoSoundSet(amb->sample, &amb->pos, amb->radius, 1, amb->pitch ) != -1)
		{
			amb->sample->snd->pad = 1;
			amb->handle = amb->sample->handle;

		}
//		sfxPrintf("ambient sample num %d, name %s\n", amb->handle, amb->sample->idName);
 	} 	
	acSystemDelay(500000);

 }
 
 
 void InitAmbientSoundList()
 {
 	ambientSoundList.head.next = ambientSoundList.head.prev = &ambientSoundList.head;
 	ambientSoundList.numEntries = 0;
 }
 
 
 
 
 
 void SubAmbientSound(AMBIENT_SOUND *ambientSound)
 {
 	if(ambientSound->handle != -1)
		sfxStopChannel(ambientSound->handle);
 
 	ambientSound->prev->next = ambientSound->next;
 	ambientSound->next->prev = ambientSound->prev;
 	ambientSoundList.numEntries--;
 
 	FREE( ambientSound );
 }
 
 
 void FreeAmbientSoundList( )
 {
 	AMBIENT_SOUND *cur,*next;

	utilPrintf("Freeing Schmambient sound list\n");
 
 	// check if any elements in list
 	if( !ambientSoundList.numEntries )
 		return;
 
 	// traverse enemy list and free elements

 	for( cur = ambientSoundList.head.next; cur != &ambientSoundList.head; cur = next )
 	{
 		next = cur->next;
 
 		SubAmbientSound( cur );
//	 	if(cur->handle != -1)
//			sfxStopChannel(cur->handle);
 	}
 

 	// initialise list for future use
 	InitAmbientSoundList();
}

/*	--------------------------------------------------------------------------------
	Function		: PrepareSongForLevel
	Purpose			: loads and starts playback of song for specified level
	Parameters		: short,short
	Returns			: void
	Info			: 
*/

XAFileType	*adxtestCur;

void PrepareSong(short worldID,int loop)
{
	KTU32	memfreeBefore,memfreeAfter;
	int 	chan,vol;
	int 	xaNum = 0;
	char	buffer[32];
	float	newVolume;

//	return;

	if(!startAudio)
		return;

	XAstop();

	switch ( worldID )
	{
		case AUDIOTRK_GAMEOVER:
			chan = 10;
			break;
		case AUDIOTRK_LEVELCOMPLETE:
			chan = 9;
			break;
		case AUDIOTRK_LEVELCOMPLETELOOP:
			chan = 11;
			break;
		// use re-order lookup for correct track number
		default:
			chan = musicList[ worldID ] + 1;
			break;
	}

	if(chan < 10)
		sprintf(buffer,"track0%d.adx",chan);
	else
		sprintf(buffer,"track%d.adx",chan);

	adxtestCur = XAgetFileInfo(buffer);
	if (adxtestCur == NULL)
	{
		curXA = NULL;
		return;
	}

	XAplayChannel(adxtestCur, 1, loop, 64);

	// set global volume
	newVolume = (float)globalMusicVol * 2.55;
	vol = (int)newVolume;
	vol = byteToDecibelLUT[vol] / 10;
	ADXT_SetOutVol(curXA->adxt,vol);

	return;
}

void StopSong( )
{ 
	XAstop();

	return;
}

long PAN_MAX = 4096*10;
int GetSoundVols(SVECTOR *pos,int *vl,int *vr,long radius,unsigned long vol)
{
	fixed att, dist;
	SVECTOR diff;
	long pan = 0;
	SVECTOR m;
	SVECTOR tempSvect;
	
	vol = (vol * globalSoundVol)/MAX_SOUND_VOL;

	if(vol == 0)
	{
		*vl = *vr = 0;
		return -1;
	}

	att = (radius)?radius/*/SCALE*/:DEFAULT_SFX_DIST;
	att = att<<12;

	SubVectorSSF(&diff, pos, &currCamSource);
//	SubVectorSSF(&diff, pos, &currCamTarget);
//	SubVectorSSS( &diff, pos, &frog[0]->actor->position );
	// Volume attenuation - check also for radius != 0 and use instead of default
	dist = MagnitudeS(&diff);
	if( dist > (att) )
	{
		*vl = *vr = 0;
		return -1;
	}

	vol = (FDiv(att-dist,att)*vol)>>12;

	tempSvect.vx = -pos->vx;
	tempSvect.vy = -pos->vy;
	tempSvect.vz = pos->vz;

	gte_ldv0(&tempSvect);
	gte_rtps();
	gte_stsxy((long *)&m.vx);
//	gte_stsz(&m.vz);	//screen z/4 as otz
	m.vz = (screenxy[2].vz >> 2);
//	m.vz >>= 2;

	dist = MagnitudeS(&m);
	if(dist)
	{
		pan = (m.vx*PAN_MAX)/dist;
		pan *= 4096;
	}
	else
		pan = 0;

	if(pan >= 0)
	{
		*vl = ((PAN_MAX - pan)*vol)/PAN_MAX;
		*vr = vol;
	}
	else
	{
		*vl = vol;
		*vr = ((PAN_MAX + pan)*vol)/PAN_MAX;
	}
	
	return TRUE;
}


SAMPLE *FindVoice( unsigned long uid, int pl )
{
	int j;

	for(j = 0;j < voiceCount[pl];j++)
		if(voiceArray[pl][j].uid == uid)
			return &voiceArray[pl][j];

	return NULL;
}

void StopSample(SAMPLE *sample)
{
	if((sample) && (sample->handle != -1))
	{
		sfxStopChannel(sample->handle);
		sample->handle = -1;
	}
}
int	oldVolumes[MAX_CHANNELS];

void PauseAudio( )
{
	int	channel;

	if (curXA)						// Is there an adx currently playing ?
	{								// yup, so........
		if(curXA->adxt)
		{
			ADXT_Pause(curXA->adxt,1);
		}
	}

	acSystemDelay(500000);
	acSystemSetMasterVolume(0);
	acSystemDelay(500000);

/*	for(channel=0; channel<MAX_CHANNELS; channel++)
	{
		if(current[channel].sound.isPlaying)
			oldVolumes[channel] = current[channel].sound.volume;
		else
			oldVolumes[channel] = 0;
	}
*/
	sfxOff();
}

void UnPauseAudio( )
{
	int	channel;

	sfxOutputOn = 1;

	// set the master volume
	acSystemDelay(500000);
	acSystemSetMasterVolume(15);
	acSystemDelay(500000);

	// make sure the streaming audio is unpaused
	if (curXA != NULL)
	{
		if (curXA->adxt != NULL)
		{
			ADXT_Pause(curXA->adxt,0);
		}
	}

	// force all of the channels to reset the their volumes
	for(channel=0; channel<MAX_CHANNELS; channel++)
	{
		if(current[channel].sound.isPlaying)
		{
			current[channel].volume = 0;
//ma			current[channel].volume = oldVolumes[channel];
//ma			amSoundSetVolume(&current[channel].sound,oldVolumes[channel]);
		}
	}

}

void SpuSetCommonCDVolume(int volume, int volume2)
{
	int		vol;
	float	newVolume;
	
	if(!curXA)
		return;

	if(!curXA->adxt)
		return;

	newVolume = (float)globalMusicVol * 2.55;
	vol = (int)newVolume;
	vol = byteToDecibelLUT[vol] / 10;
	ADXT_SetOutVol(curXA->adxt,vol);
}

int IsSongPlaying()
{
	if(curXA)
	{
		if(curXA->adxt)
		{
			if(ADXT_GetStat(curXA->adxt) == ADXT_STAT_PLAYEND)
			{
				if(curXA->loop)
					ADXT_StartFname(curXA->adxt, curXA->fileInfo);	// Start playing the XDA using the middlware
			}
		}
	}

//	if(bpAmStreamDone(gStream))
//		return TRUE;
		
	return	FALSE;
}

/**************************************************************************
	FUNCTION:	sfxPlaySample()
	PURPOSE:	Plays the requested sample
	PARAMETERS:	pointer to sample, left & right volume (0 - 255), pitch (Hz, 0 for default)
	RETURNS:	channel number, or -1 if failure
**************************************************************************/

int sfxPlaySample(SfxSampleType *sample, int volL, int volR, int pitch)
{
	AM_SOUND	*sound;
	int			volAverage,volPan,channel;

	for(channel=0; channel<MAX_CHANNELS; channel++)
	{
		if(!current[channel].sound.isPlaying)
			break;
	}
	if(channel==MAX_CHANNELS)
		return -1;

	if(volL||volR)
	{
		if(options.stereo)
		{
			volPan = (127 * volL)/(volL+volR);
			volAverage = volL > volR ? volL : volR;
		}
		else
		{
			volPan = 64;
			volAverage = volL > volR ? volL : volR;
		}
	}
	else
	{
		volPan = 64;
		volAverage = 0;
	}

	current[channel].sample = sample;
//	current[channel].volume = (volAverage*sfxGlobalVolume)/512;
	current[channel].volume = volAverage;

//	volAverage = sfxOutputOn ? current[channel].volume : 0;
//	volAverage = current[channel].volume;

	sound = bpAmPlaySoundEffect(sample,audio64Banks[sample->bankNumber],&current[channel].sound,sample->sampleNumber,volAverage,volPan);
	if(sound == KTNULL)
	{
		return -1;
	}

//	sound = bpAmPlaySoundEffect(audio64Banks[sample->bankNumber],&current[channel].sound,sample->sampleNumber,current[channel].volume,volPan);

	if((pitch>0) && sound) 
	{
		amSoundSetCurrentPlaybackRate(sound,pitch);
		audioError();
	}

	return channel;
}

/**************************************************************************
	FUNCTION:	sfxInitialise()
	PURPOSE:	Initialise the sound library
	PARAMETERS:	reverb mode (not supported in this version)
	RETURNS:	none
**************************************************************************/

void sfxInitialise(int reverbMode)
{
	int loop;

	utilInitCRC();//TG: Temp fix

//	if(!bpAmSetup(AC_DRIVER_DA, KTFALSE, KTNULL))
//		for(;;);

//	acSetTransferMode(AC_TRANSFER_DMA);
//	acSetTransferMode(AC_TRANSFER_CPU);
		
//	acSystemDelay(500000);

//	acErr = acErrorGetLast();
//	amErr = amErrorGetLast();

	//Initialise all sample banks to null so we know they aren't in use
	for(loop=0; loop<MAX_SAMPLE_BANKS; loop++)
	{
		sampleBanks[loop].used = 0;
		audio64Banks[loop] = NULL;
	}

	for(loop=0; loop<MAX_CHANNELS; loop++)
	{
		current[loop].sound.isPlaying = 0;
		current[loop].sample = NULL;
	}

	amSystemSetVolumeMode(USELINEAR);

	sfxGlobalVolume = 255;
	sfxOutputOn = 1;
		
//	a64FileInit();
}


/**************************************************************************
	FUNCTION:	sfxDestroy()
	PURPOSE:	Shutdown the sound library
	PARAMETERS:	none
	RETURNS:	none
**************************************************************************/

void sfxDestroy()
{
//	acSystemShutdown();
//	amShutdown();
}


/**************************************************************************
	FUNCTION:	sfxUpdate()
	PURPOSE:	update the sound queue
	PARAMETERS:	none
	RETURNS:	none
**************************************************************************/

void sfxUpdate()
{
}

/**************************************************************************
	FUNCTION:	sfxOn()
	PURPOSE:	Turn sound output on (emulated on DC)
	PARAMETERS:	none
	RETURNS:	none
**************************************************************************/

void sfxOn()
{
	int channel;

	sfxOutputOn = 1;

	for(channel=0; channel<MAX_CHANNELS; channel++)
	{
		if(current[channel].sound.isPlaying)
		{
			// queue up our volume
			TqfxEntry fx;
			fx.command = QFXCMD_SETVOLUME;
			fx.volume = current[channel].volume;
			fx.channel = channel;
			qfxAddEntry(&fx);
		}

//ma			amSoundSetVolume(&current[channel].sound,current[channel].volume);
	}
}


/**************************************************************************
	FUNCTION:	sfxOff()
	PURPOSE:	Turn sound output off (emulated on DC)
	PARAMETERS:	none
	RETURNS:	none
**************************************************************************/

void sfxOff()
{
	int channel;

	for(channel=0; channel<MAX_CHANNELS; channel++)
	{
		if(current[channel].sound.isPlaying)
		{
			// queue up our volume
			TqfxEntry fx;
			fx.command = QFXCMD_SETVOLUME;
			fx.volume = 0;
			fx.channel = channel;
			qfxAddEntry(&fx);
		}

//ma			amSoundSetVolume(&current[channel].sound,0);
	}

	sfxOutputOn = 0;
}

/**************************************************************************
	FUNCTION:	sfxSetGlobalVolume()
	PURPOSE:	Set global sample volume
	PARAMETERS:	volume
	RETURNS:	none
**************************************************************************/

void sfxSetGlobalVolume(int vol)
{
	if(vol<0) vol = 0;
	if(vol>255) vol = 255;
	sfxGlobalVolume = vol;
}

/**************************************************************************
	FUNCTION:	sfxStopChannel
	PURPOSE:	Stop a channel from playing
	PARAMETERS:	channel number (0 - 23), or -1 for all
	RETURNS:	none
**************************************************************************/

void sfxStopChannel(int channel)
{
	if(current[channel].sound.isPlaying)
	{
		// queue up our stop channel
		TqfxEntry fx;
		fx.command = QFXCMD_STOPSAMPLE;
		fx.channel = channel;
		qfxAddEntry(&fx);
//ma		amSoundStop(&current[channel].sound);
	}
}

/**************************************************************************
	FUNCTION:	sfxSetChannelPitch()
	PURPOSE:	set pitch of channel
	PARAMETERS:	channel number, pitch (Hz)
	RETURNS:	none
**************************************************************************/

void sfxSetChannelPitch(int channel, int pitch)
{
	if(current[channel].sound.isPlaying)
	{
		// queue up our pitch change
		TqfxEntry fx;
		fx.command = QFXCMD_SETPITCH;
		fx.pitch = pitch;
		fx.channel = channel;
		qfxAddEntry(&fx);
	}
}


/**************************************************************************
	FUNCTION:	sfxSetChannelVolume()
	PURPOSE:	set volume of channel
	PARAMETERS:	channel number, left vol, right vol (0 - 255)
	RETURNS:	none
**************************************************************************/

KTU32	oldVolume = 0;

void sfxSetChannelVolume(int channel, int volL, int volR)
{
	int	volAverage,volPan,newVolume,dif;

	if(!current[channel].sound.isPlaying)
		return;
	
	if(volL||volR)
	{
		if(options.stereo)
		{
			volPan = (127 * volL)/(volL+volR);
			volAverage = volL > volR ? volL : volR;
		}
		else
		{
			volPan = 64;
			volAverage = volL > volR ? volL : volR;
		}
	}
	else
	{
		volPan = 0;
		volAverage = 0;
	}

	// calculate new volume
	newVolume = (volAverage * sfxGlobalVolume) >>8;
	oldVolume = current[channel].volume;

	// only change volume on major volume update
	dif = abs(oldVolume - newVolume);
	if(dif > 8)
	{
		// set volume
		current[channel].volume = newVolume;
		{
			// queue up our volume
			TqfxEntry fx;
			fx.command = QFXCMD_SETVOLUME;
			fx.volume = newVolume;
			fx.channel = channel;
			qfxAddEntry(&fx);
		}
		//amSoundSetVolume(&current[channel].sound,newVolume);

		// only set pan on change
		if (current[channel].pan != volPan)
		{
			// queue up our pan
			TqfxEntry fx;
			fx.command = QFXCMD_SETPAN;
			fx.pan = volPan;
			fx.channel = channel;
			qfxAddEntry(&fx);
		}
		//amSoundSetPan(&current[channel].sound,volPan);
		current[channel].pan = volPan;
	}
}

/**************************************************************************
	FUNCTION:	sfxStartSound()
	PURPOSE:	Start sound DMA processing
	PARAMETERS:	none
	RETURNS:	none
**************************************************************************/

void sfxStartSound()
{
}

/**************************************************************************
	FUNCTION:	sfxStopSound()
	PURPOSE:	Stop sound DMA processing (DC just cuts all samples)
	PARAMETERS:	none
	RETURNS:	none
**************************************************************************/

void sfxStopSound()
{
	int channel;

	for(channel=0; channel<MAX_CHANNELS; channel++)
	{
		if(current[channel].sound.isPlaying)
		{
			// queue up our volume
			TqfxEntry fx;
			fx.command = QFXCMD_SETVOLUME;
			fx.volume = 0;
			fx.channel = channel;
			qfxAddEntry(&fx);
		}

//ma		amSoundSetVolume(&current[channel].sound,0);
	}
}