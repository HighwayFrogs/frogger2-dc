#include "include.h"
#include	<cri_adxf.h>
#include	<cri_adxt.h>

extern texurestring[256];
extern texurestring2[256];

char	*genericStak = NULL;
int		genericStakSize = 0;


// *ASL* 12/08/2000 - Allow quit added
/* ---------------------------------------------------------
   Function : StartVideoPlayback
   Purpose : playback a video stream
   Parameters : stream number, can the user quit the stream
   Returns : 1 if user quit the stream, else 0
   Info : 
*/

static char* fileLoad2(char *filename,int *bytesRead);
extern unsigned int globalAbortFlag;
extern void resetToBootROM();

char *fileLoad(char *filename, int *bytesRead)
{
	char	*ret;

	ret = fileLoad2(filename, bytesRead);
	if (ret == NULL)
	{
		// *ASL* 13/08/2000 - Reset to BootROM on lid being opened by the user
		if (globalAbortFlag == 1)
			resetToBootROM();
	}
	return ret;
}
static char* fileLoad2(char *filename,int *bytesRead)
{
    PKMDWORD    filePtr;
    GDFS        gdfs = NULL;
    long        FileBlocks;
    int			i,flag = TRUE;
    char		buffer[256],*fptr;
	Sint32		status,size;
	int			retry = 0;
	Sint32		gflag;
	Uint32		length;
	long		crc = 0;
	char		*crcPtr;
	ADXF		adxf;

//	syCacheICI();
	
	gdFsChangeDir("\\");

	// change to the appropriate directory
	i = 0;
	fptr = filename;
	while(*fptr != 0)
	{
		switch(*fptr)
		{
			case '\\':
				buffer[i] = 0;
				gdFsChangeDir(buffer);
				fptr++;	
				i=0;			
				break;
				
			default:
				buffer[i++] = *fptr++;				
				break;		
		}
	}
	buffer[i] = 0;

//	if((strcmp(buffer,"GENERIC.STK") == 0)&&(genericStak))
//	{
//		filePtr = Align32Malloc(genericStakSize);
//		memcpy(genericStak,filePtr)
//	}
	
    // Open input file.
    while((!gdfs) && (retry < 50))
    {
	    gdfs = gdFsOpen(buffer, NULL);
	    if(gdfs == NULL)
	    	retry++;
	}
	if(gdfs == NULL)
    	return NULL;

    // Get file size (in blocks/sectors).
	FileBlocks = 0;
    if(bytesRead)
		gdFsGetFileSize(gdfs, (Sint32 *)bytesRead);
    gdFsGetFileSctSize(gdfs, (Sint32 *)&FileBlocks);

	// trap zero length
	if (FileBlocks == 0)
		return NULL;

    // Allocate memory to nearest block size (2048 bytes).    
	if((strcmp(buffer,"Sqrtable.bin") == 0)||(strcmp(buffer,"acostab.bin") == 0))
	    filePtr = syMalloc(FileBlocks * 2048);
	else	
   		filePtr = Align32Malloc(FileBlocks * 2048);
	if(filePtr == NULL)
		utilPrintf("Error\n");

    // Read file to memory region (Destination address must be 32-byte aligned).
    gdFsReqRd32(gdfs, FileBlocks, filePtr);

    // Wait for file access to finish.
    while(gdFsGetStat(gdfs) != GDD_STAT_COMPLETE)
    {
		// *ASL* 13/08/2000 - Reset to BootROM on lid being opened by the user
		if (globalAbortFlag == 1)
			resetToBootROM();

	    status = gdFsGetStat(gdfs);
    	if(status == GDD_STAT_ERR)
    	{
    		utilPrintf("Error\n");
  	 		Align32Free(filePtr);
		    gdFsClose(gdfs);
  	 		return NULL;
    	}
    }

    // Close file.
    gdFsClose(gdfs);

/*	{
		// Open input file.......
		if (!(adxf = ADXF_Open(buffer, NULL)))
		{
			utilPrintf("********file open error********\n");
			return NULL;
		} // end-if

		// Get the file size (blocks/sectors)
		FileBlocks = ADXF_GetFsizeSct(adxf);

		// Alloc the memory needed for loading the file
		filePtr = Align32Malloc(FileBlocks * 2048); //this is a bit big for most files, but we have the room
		if(filePtr == NULL)							 //and several levels won't load otherwise (it is freed v.soon)
			utilPrintf("Error\n");

		// Read the file into memory
		ADXF_ReadNw32(adxf, FileBlocks, filePtr);	

		// Wait until we're done reading the file or an error occurs somewhere....
		while(ADXF_GetStat(adxf) != ADXF_STAT_READEND)
		{
			status = ADXF_GetStat(adxf);
			if (status == ADXF_STAT_ERROR)
				utilPrintf("Error\n");
		}

		// Close the file
		ADXF_Close(adxf);
	}
*/
    return (char*)filePtr;
}
