/************************************************************************************
	ISL DC LIBRARY	(c) 2000 Blitz Games Ltd.

	islsfx2.c:		Sound fx handling 2.0 (Dreamcast conversion)
																			tgaulton
************************************************************************************/

#include "newsfx.h"
#include "islutil.h"
#include "sg_gd.h"
#include <ac.h>
#include <a64thunk.h>
#include <am.h>
#include "DCK_Texture.h"

#define MAX_SAMPLE_BANKS	6

extern AC_ERROR_PTR		acErr;
extern AM_ERROR *			amErr;

SfxBankType			sampleBanks[MAX_SAMPLE_BANKS];
AM_BANK_PTR			audio64Banks[MAX_SAMPLE_BANKS];
int					removeOnUnload[MAX_SAMPLE_BANKS];

typedef struct {
	AM_SOUND			sound;
	SfxSampleType		*sample;
	int					volume;
} CurrentData;

CurrentData			current[24];

int					sfxGlobalVolume,sfxOutputOn;


extern AM_ERROR *bpAmError;
extern AC_ERROR_PTR bpAcError;

//-----------------------------------------------------------------------------
//	Setup the AC system
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//	AM_BANK_PTR bpAmLoadBank(KTSTRING bankFileName)
//-----------------------------------------------------------------------------
 
AM_BANK_PTR bpAmLoadBank2(KTSTRING bankFileName, unsigned char **sampleData)
{
	KTU32 bankSize;
	KTU32 bankImageSize;
	KTU32 *aicaBuffer;

	gdFsChangeDir("\\");
	gdFsChangeDir("SFX");
	
	//	get the size of the bank file
	if (!amBankGetSize(bankFileName, &bankSize))
	{
		bpDebugf("amBankGetSize(bankFileName, ...) failed\n");
		return (KTNULL);
	}
	//	align for GD-ROM reading
	bankImageSize = SECTOR_ALIGN(bankSize);

	//	get a buffer in SH4 memory (using aligned size)
	*sampleData = (unsigned char *)syMalloc(bankImageSize);
	if (!*sampleData)
	{
		bpDebugf("syMalloc(bankSize) failed\n");
		return (KTNULL);
	}

	//	load the bank
	if (!amBankLoad(bankFileName, (AM_BANK_PTR)*sampleData, bankImageSize))
	{
		bpDebugf("amBankLoad(bankFileName, ...) failed\n");
		return (KTNULL);
	}

	//	get a buffer in sound memory (using actual size)
	if (!amHeapAlloc(&aicaBuffer, bankSize, 32, AM_PURGABLE_MEMORY,KTNULL))
	{
		bpDebugf("amHeapAlloc(&aicaBuffer, ...) failed\n");
		syFree(*sampleData);
		return (KTNULL);
	}

	//	transfer the bank from SH4 memory to AICA memory
	if (!amBankTransfer(aicaBuffer, (AM_BANK_PTR)*sampleData, bankSize))
	{
		bpDebugf("amBankTransfer(aicaBuffer, ...) failed\n");
		syFree(*sampleData);
		return (KTNULL);
	}



	return (aicaBuffer);
}

AM_SOUND *bpAmPlaySoundEffect2(AM_BANK_PTR theBank, AM_SOUND *theSound, KTU32 soundNumber, KTU32 volume, KTU32 pan)
{
	//	fetch a pointer to the data 
	if (!amSoundFetchSample((KTU32*)theBank, soundNumber, theSound))
	{
		return (KTNULL);
	}

	//	get a voice channel
	if (!amSoundAllocateVoiceChannel(theSound))
	{
		return (KTNULL);
	}
	//	set the initial volume
	if (!amSoundSetVolume(theSound, volume))
	{
		amSoundReleaseVoiceChannel(theSound);
		return (KTNULL);
	}
	//	set the initial pan
	if (!amSoundSetPan(theSound, pan))
	{
		amSoundReleaseVoiceChannel(theSound);
		return (KTNULL);
	}
	//	make it play
	if (!amSoundPlay(theSound))
	{
		return (KTNULL);
	}

	return (theSound);
}









/**************** ISLSFX LIBRARY CONVERSION - MIMICS PLAYSTATION CALLS **************/



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

	if(!bpAmSetup(AC_DRIVER_DA, KTFALSE, KTNULL))
		for(;;);

	acSetTransferMode(AC_TRANSFER_DMA);
//	acSetTransferMode(AC_TRANSFER_CPU);
		
	acSystemDelay(500000);

	acErr = acErrorGetLast();
	amErr = amErrorGetLast();

	//Initialise all sample banks to null so we know they aren't in use
	for(loop=0; loop<MAX_SAMPLE_BANKS; loop++)
	{
//		sampleBanks[loop] = NULL;
		sampleBanks[loop].used = FALSE;
		audio64Banks[loop] = NULL;
	}

	for(loop=0; loop<24; loop++)
	{
		current[loop].sound.isPlaying = 0;
		current[loop].sample = NULL;
	}

	amSystemSetVolumeMode(USELINEAR);

	sfxGlobalVolume = 255;
	sfxOutputOn = 1;
		
	a64FileInit();
}


/**************************************************************************
	FUNCTION:	sfxDestroy()
	PURPOSE:	Shutdown the sound library
	PARAMETERS:	none
	RETURNS:	none
**************************************************************************/

void sfxDestroy()
{
	acSystemShutdown();
	amShutdown();
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
	FUNCTION:	sfxFixupSampleBankHeader()
	PURPOSE:	fix up sample bank header already in ram
	PARAMETERS:	pointer to sample bank header, spu base address
	RETURNS:	pointer to sample bank header
**************************************************************************/

SfxBankType *sfxFixupSampleBankHeader(SfxBankType *bank, unsigned long spuAddr)
{
	int loop;

	//DC SPECIFIC: What we actually get passed as spuAddr is a pointer to a preloaded sample bank
	SfxBankType *prevBank = (SfxBankType *)spuAddr;

	//Same as PS, fix the sample header data pointer
	bank->sample = (SfxSampleType *) ((unsigned long)bank + sizeof(SfxBankType));

	//Basic check to ensure banks match (fingers crossed!)
	if(bank->numSamples!=prevBank->numSamples) for(;;);

	//We now transfer the preloaded bank data into the bank pointer passed in
	bank->sampleData = prevBank->sampleData;
	for(loop=0; loop<bank->numSamples; loop++)
		bank->sample[loop] = prevBank->sample[loop];

	//Find old bank in bank list and change pointer to new bank
//ma	for(loop=0; loop<MAX_SAMPLE_BANKS; loop++)
//ma	{
//ma		if(prevBank == sampleBanks[loop])
//ma		{
//ma			sampleBanks[loop] = bank;
//ma			break;
//ma		}
//ma	}
	//Then free up the old bank
//ma	syFree(prevBank);
}


/**************************************************************************
	FUNCTION:	sfxLoadSampleBankBody()
	PURPOSE:	Load a sample bank into spu ram
	PARAMETERS:	filename of sample bank (minus extension)
	RETURNS:	spu base address
**************************************************************************/

unsigned long sfxLoadSampleBankBody(char *fileName)
{
	int bankNum;

	//DC SPECIFIC: We cheat a bit here and load the entire sample bank using the DC files.. 
	SfxBankType *bank = sfxLoadSampleBank(fileName);
	sfxDestroySampleBank(bank);

	//If banks are loaded using the sfxLoadSampleBankBody method then remove is never called 
	//so do it when unloading instead
//ma	for(bankNum=0; bankNum<MAX_SAMPLE_BANKS; bankNum++)
//ma		if(sampleBanks[bankNum] == bank) break;

	removeOnUnload[bankNum] = 1;

	return (unsigned long)bank;
}


/**************************************************************************
	FUNCTION:	sfxLoadSampleBank()
	PURPOSE:	Load a sample bank into ram
	PARAMETERS:	filename of sample bank (minus extension)
	RETURNS:	pointer to sample bank
**************************************************************************/

SfxBankType *sfxLoadSampleBank(char *fileName)
{
	SfxBankType		*bank;
	char			*sampleList,*sListPtr;
	unsigned long	numSamples;
	int				loop,charLoop,bankNum,bytesRead;
	char			fname[256],*fPtr;
	AM_SOUND		soundInfo;
	unsigned char	*sampleDataPtr;
	char			buffer[256];

	//Find a free bank in the global list
	for(bankNum=0; bankNum<MAX_SAMPLE_BANKS; bankNum++)
		if(!sampleBanks[bankNum].used) break;

	// List is full so cancel load... probably a bug if this happens
	if(loop==MAX_SAMPLE_BANKS) return NULL;

	//If banks are loaded using the sfxLoadSampleBankBody method then remove is never called 
	//so do it when unloading instead
	removeOnUnload[bankNum] = 0;

	//Do the DC specific data load
	//Downloads the bank into audio ram so downloadsample is actually a dummy function on DC
	strcpy(fname,fileName);
	strcat(fname,".kat\0");
	if((audio64Banks[bankNum] = bpAmLoadBank2(fname,&sampleDataPtr))==KTNULL)
	{
		syFree(bank);
		return 0;
	}

	//Allocate memory for sample data and grab data from .kat file
	amBankGetNumberOfAssets(audio64Banks[bankNum],&numSamples);

	//malloc the bank all in one then fill it in
//	sampleBanks[bankNum] = (SfxBankType *) Align32Malloc(sizeof(SfxBankType));
//	bank = &sampleBanks[bankNum];
	sampleBanks[bankNum].numSamples = numSamples;
	sampleBanks[bankNum].sample = (SfxSampleType *) Align32Malloc(sizeof(SfxSampleType)*numSamples);
	sampleBanks[bankNum].sampleData = sampleDataPtr;

	for(loop=0; loop<bank->numSamples; loop++)
	{
		amSoundFetchSample(audio64Banks[bankNum],loop,&soundInfo);
		sampleBanks[bankNum].sample[loop].sampleSize	= soundInfo.sizeInBytes;
		sampleBanks[bankNum].sample[loop].sampleData	= sampleDataPtr;
		sampleBanks[bankNum].sample[loop].sampleRate	= soundInfo.sampleRate;
		sampleBanks[bankNum].sample[loop].bankNumber	= bankNum;
		sampleBanks[bankNum].sample[loop].sampleNumber	= loop;
		sampleBanks[bankNum].sample[loop].inSPURam		= 1;//Always loaded on DC
		sampleDataPtr += soundInfo.sizeInBytes;
	}

//	return;
	
	//Need name CRC's unfortunately so grab these from a seperate file
	strcpy(fname,fileName);
	strcat(fname,".txt\0");
//	strcpy(buffer,"SFX\\");
//	strcat(buffer,fname);
	
	sampleList = (char*)fileLoad(fname,&bytesRead);
	sListPtr = sampleList;

	for(loop=0; loop<sampleBanks[bankNum].numSamples; loop++)
	{
		for(fPtr = fname; ((*sListPtr)!='\0') && ((*sListPtr)!='\n'); fPtr++)
		{
			*fPtr = *(sListPtr++);
		}
		fPtr -= 5;//Take off the .wav extension and the carriage return which seems to have squeezed in
		*fPtr = '\0';
		while(*sListPtr =='\n') sListPtr++;

		sampleBanks[bankNum].sample[loop].nameCRC = utilStr2CRC(fname);
//		strcpy(sampleBanks[bankNum].sample[loop].name,fname);
	}

//	Align32Free(sampleList);

	return bank;
}
	
/**************************************************************************
	FUNCTION:	sfxFindSampleInBank()
	PURPOSE:	Find a sample in a bank
	PARAMETERS:	sample name
	RETURNS:	pointer to sample, or NULL if not found
**************************************************************************/

SfxSampleType *sfxFindSampleInBank(char *sampleName, SfxBankType *bank)
{
	char sname[64];
	strcpy(sname,sampleName);
	utilUpperStr(sname);
	return sfxFindSampleInBankCRC(utilStr2CRC(sname),bank);
}

/**************************************************************************
	FUNCTION:	sfxFindSampleInAllBanks()
	PURPOSE:	Find a sample in all loaded banks
	PARAMETERS:	sample name
	RETURNS:	pointer to sample, or NULL if not found
**************************************************************************/

SfxSampleType *sfxFindSampleInAllBanks(char *sampleName)
{
	char sname[64];
	strcpy(sname,sampleName);
	utilUpperStr(sname);
	return sfxFindSampleInAllBanksCRC(utilStr2CRC(sname));
}

/**************************************************************************
	FUNCTION:	sfxFindSampleInBankCRC()
	PURPOSE:	Find a sample in a bank
	PARAMETERS:	sample name crc, bank to search
	RETURNS:	pointer to sample, or NULL if not found
**************************************************************************/

SfxSampleType *sfxFindSampleInBankCRC(unsigned long CRC, SfxBankType *bank)
{
	int loop;

	for(loop=0; loop<bank->numSamples; loop++)
	{
		if(CRC == bank->sample[loop].nameCRC)
			return &bank->sample[loop];
	}
	return NULL;
}

/**************************************************************************
	FUNCTION:	sfxFindSampleInAllBanksCRC()
	PURPOSE:	Find a sample in all loaded banks
	PARAMETERS:	sample name CRC
	RETURNS:	pointer to sample, or NULL if not found
**************************************************************************/

SfxSampleType *sfxFindSampleInAllBanksCRC(unsigned long CRC)
{
	int loop,bankLoop;

	for(bankLoop=0; bankLoop<MAX_SAMPLE_BANKS; bankLoop++)
	{
		for(loop=0; loop<sampleBanks[bankLoop].numSamples; loop++)
		{
			if(CRC == sampleBanks[bankLoop].sample[loop].nameCRC)
				return &sampleBanks[bankLoop].sample[loop];
		}
	}
	return NULL;
}

/**************************************************************************
	FUNCTION:	sfxDownloadSample
	PURPOSE:	Download sample to SPU ram
	PARAMETERS:	pointer to sample
	RETURNS:	pointer to sample, or NULL if failed
**************************************************************************/

SfxSampleType *sfxDownloadSample(SfxSampleType *sample)
{
	return sample;
}

/**************************************************************************
	FUNCTION:	sfxUnloadSample
	PURPOSE:	Unload sample from SPU ram
	PARAMETERS:	pointer to sample
	RETURNS:	pointer to sample, or NULL if failed
**************************************************************************/

SfxSampleType *sfxUnloadSample(SfxSampleType *sample)
{
	//NULL function (upload/download) is handled per bank
	return sample;
	
}

/**************************************************************************
	FUNCTION:	sfxUnloadSampleBank
	PURPOSE:	Unload sample bank from SPU ram
	PARAMETERS:	pointer to sample bank
	RETURNS:	pointer to sample bank, or NULL if failed
**************************************************************************/

SfxBankType *sfxUnloadSampleBank(SfxBankType *bank)
{
	int loop,result;
	//Find in bank list
	for(loop=0; loop<MAX_SAMPLE_BANKS; loop++)
	{
		if(bank == &sampleBanks[loop])
			break;
	}

	result = amHeapFree(audio64Banks[loop]);

	if(!result)	return NULL;

	audio64Banks[loop] = NULL;

	if(removeOnUnload[loop])
		sfxRemoveSampleBank(bank,0);

	return bank;
}

/**************************************************************************
	FUNCTION:	sfxDestroySampleBank
	PURPOSE:	Destroy ram copy of sample data
	PARAMETERS:	pointer to sample bank
	RETURNS:	pointer to samplebank, or NULL if failed
**************************************************************************/

SfxBankType *sfxDestroySampleBank(SfxBankType *bank)
{
//	if(syFree(bank->sampleData))
//		return bank;
//	else 
		return NULL;
}

/**************************************************************************
	FUNCTION:	sfxRemoveSampleBank
	PURPOSE:	Remove sample bank from list
	PARAMETERS:	pointer to sample bank, whether to free it (1, 0)
	RETURNS:	1 if removed, 0 if not found in list
**************************************************************************/

int sfxRemoveSampleBank(SfxBankType *bank, int free)
{
	int loop;

	//Check against bank list and free if found
	for(loop=0; loop<MAX_SAMPLE_BANKS; loop++)
	{
		if(bank == &sampleBanks[loop])
		{
			if(free) syFree(bank);

			sampleBanks[loop].used = FALSE;

//			amHeapFree(audio64Banks[loop]);
//			audio64Banks[loop] = NULL;
			return 1;
		}
	}
	return 0;
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

	for(channel=0; channel<24; channel++)
	{
		if(current[channel].sound.isPlaying)
			amSoundSetVolume(&current[channel].sound,current[channel].volume);
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

	for(channel=0; channel<24; channel++)
	{
		if(current[channel].sound.isPlaying)
			amSoundSetVolume(&current[channel].sound,0);
	}

	sfxOutputOn = 0;
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

	for(channel=0; channel<24; channel++)
	{
		if(current[channel].sound.isPlaying)
			amSoundSetVolume(&current[channel].sound,0);
	}
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
	FUNCTION:	sfxSetReverb()
	PURPOSE:	Set reverb level
	PARAMETERS:	delay (0 - 127), depth (0 - 255).
	RETURNS:	1 if successful, 0 if failure
	NOTE:		delay is only effective on SFX_REVERB_MODE_ECHO & SFX_REVERB_MODE_DELAY
**************************************************************************/

int sfxSetReverb(int delay, int depth)
{
	//not supported in this version
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
	
	for(channel=0; channel<24; channel++)
	{
		if(!current[channel].sound.isPlaying)
			break;
	}

	if(channel==24) return;

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
	current[channel].volume = (volAverage*sfxGlobalVolume)/512;

	volAverage = sfxOutputOn ? current[channel].volume : 0;

	sound = bpAmPlaySoundEffect2(audio64Banks[sample->bankNumber],&current[channel].sound,
								sample->sampleNumber,volAverage,volPan);

	if((pitch>0) && sound) 
		amSoundSetCurrentPlaybackRate(sound,pitch);

	return channel;
}


/**************************************************************************
	FUNCTION:	sfxStopSample()
	PURPOSE:	Stop the requested sample
	PARAMETERS:	pointer to sample, channel number (0 - 23) or -1 for all
	RETURNS:	1 if ok, 0 if failure
**************************************************************************/

int sfxStopSample(SfxSampleType *sample, int channel)
{
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
		amSoundStop(&current[channel].sound);
}


/**************************************************************************
	FUNCTION:	sfxGetSampleStatus
	PURPOSE:	get the status of a sample
	PARAMETERS:	pointer to sample
	RETURNS:	Bitfield of channels currently playing sample
**************************************************************************/

unsigned long sfxGetSampleStatus(SfxSampleType *sample)
{
	unsigned long	bitField=0;
	int				channel;

	for(channel=0; channel<24; channel++)
	{
		if((sample==current[channel].sample) && (current[channel].sound.isPlaying))
			bitField |= (1<<channel);
	}
	return bitField;
}


/**************************************************************************
	FUNCTION:	sfxGetChannelStatus
	PURPOSE:	get the status of a channel
	PARAMETERS:	channel number (0 - 23)
	RETURNS:	Currently playing sample, or NULL if no sample playing
**************************************************************************/

SfxSampleType *sfxGetChannelStatus(int channel)
{
	if((channel>=0) && (channel<=23) && current[channel].sound.isPlaying)
		return current[channel].sample;

	return NULL;
}


/**************************************************************************
	FUNCTION:	sfxSetChannelReverb
	PURPOSE:	Turn reverb on/off for a channel
	PARAMETERS:	channel number (0 - 23), status (1 = on, 0 = off)
	RETURNS:	1 if OK, 0 if failure
**************************************************************************/

int sfxSetChannelReverb(int channel, int status)
{
	//so yeah
}


/**************************************************************************
	FUNCTION:	sfxGetChannelReverb
	PURPOSE:	Get reverb status for a channel
	PARAMETERS:	channel number (0 - 23)
	RETURNS:	1 if on, 0 if off, -1 if failure
**************************************************************************/

int sfxGetChannelReverb(int channel)
{
	//er... yeah
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
		amSoundSetCurrentPlaybackRate(&current[channel].sound,pitch);
}


/**************************************************************************
	FUNCTION:	sfxSetChannelVolume()
	PURPOSE:	set volume of channel
	PARAMETERS:	channel number, left vol, right vol (0 - 255)
	RETURNS:	none
**************************************************************************/

void sfxSetChannelVolume(int channel, int volL, int volR)
{
	int			volAverage,volPan;

	if(!current[channel].sound.isPlaying) return;
	
	if(volL||volR)
	{
		volPan = (127 * volL)/(volL+volR);
		volAverage = volL > volR ? volL : volR;
	}
	else
	{
		volPan = 0;
		volAverage = 0;
	}

	current[channel].volume = (volAverage*sfxGlobalVolume)/512;

	volAverage = sfxOutputOn ? current[channel].volume : 0;

	amSoundSetPan(&current[channel].sound,volPan);

	amSoundSetVolume(&current[channel].sound,volAverage);
}


/**************************************************************************
	FUNCTION:	9Volume()
	PURPOSE:	set volume of requested sample
	PARAMETERS:	pointer to sample, channel number (0 - 23) or -1 for all, left and right volumes (0 - 255)
	RETURNS:	1 if ok, 0 if failure
**************************************************************************/

int sfxSetSampleVolume(SfxSampleType *sample, int channel, int volL, int volR)
{
	if(channel==-1)
	{
		//Loop through all channels searching for sample
		for(channel=0; channel<24; channel++)
		{
			if((current[channel].sound.isPlaying) && (current[channel].sample==sample))
			{
				sfxSetChannelVolume(channel,volL,volR);
			}
		}
	}
	else if((current[channel].sound.isPlaying) && (current[channel].sample==sample))
	{
		//This channel only
		sfxSetChannelVolume(channel,volL,volR);
	}
}


/**************************************************************************
	FUNCTION:	sfxSetSamplePitch()
	PURPOSE:	set volume of requested sample
	PARAMETERS:	pointer to sample, channel number (0 - 23) or -1 for all, pitch (Hz)
	RETURNS:	1 if ok, 0 if failure
**************************************************************************/

int sfxSetSamplePitch(SfxSampleType *sample, int channel, int pitch)
{
	if(channel==-1)
	{
		//Loop through all channels searching for sample
		for(channel=0; channel<24; channel++)
		{
			if((current[channel].sound.isPlaying) && (current[channel].sample==sample))
			{
				sfxSetChannelPitch(channel,pitch);
			}
		}
	}
	else if((current[channel].sound.isPlaying) && (current[channel].sample==sample))
	{
		//This channel only
		sfxSetChannelPitch(channel,pitch);
	}}


/**************************************************************************
	FUNCTION:	sfxGetFreeSoundMemory
	PURPOSE:	Print the amount of free SPU ram
	PARAMETERS:	
	RETURNS:	free ram in bytes
**************************************************************************/

int sfxGetFreeSoundMemory()
{
	KTU32 freeMem;
	amHeapGetFree(&freeMem);

	return (int)freeMem;
}



