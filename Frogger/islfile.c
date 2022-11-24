#include "include.h"
#include "islxa.h"
#include "sound.h"
#include "cri_adxf.h"

// FlatPacker magic number
#define FLA_MAGIC			0x32414c46			// Magic number 'FLA2' INTEL

#define	HISTORY_SIZE		4096				// flatpacker stuff
#define MASK_HISTORY		(HISTORY_SIZE-1)
#define MASK_UPPER			(0xF0)
#define MASK_LOWER			(0x0F)
#define SHIFT_UPPER			16
#define LSR_UPPER			4
#define MAX_COMP_LEN		17

static unsigned char	*outputBufPtr;			// flatpacker temp storage
static int				outputBufLen;
static unsigned char	LZhistory[HISTORY_SIZE];
static unsigned short	LZhistoryOff;


/**************************************************************************
	FUNCTION:	DecompressOutputByte()
	PURPOSE:	decompresses a byte of data
	PARAMETERS:	the byte of data
	RETURNS:
**************************************************************************/

static void DecompressOutputByte(unsigned char data)
{
	*outputBufPtr++ = data;
	outputBufLen++;
	LZhistory[LZhistoryOff] = data;
	LZhistoryOff = (LZhistoryOff+1) & MASK_HISTORY;
}


/**************************************************************************
	FUNCTION:	utilDecompressBuffer()
	PURPOSE:	Decompress data from input buffer into output buffer
	PARAMETERS:	Input buffer start, Output buffer start
	RETURNS:	Length of uncompressed data
**************************************************************************/

int utilDecompressBuffer(unsigned char *inBuf, unsigned char *outBuf)
{
	unsigned short	tag, count, offset, loop;

	outputBufPtr = outBuf;								// Initialise output
	outputBufLen = 0;
	
	LZhistoryOff = 0;									// Clear history
	memset(LZhistory, 0, HISTORY_SIZE);

	while(1)
	{
		tag = *inBuf++;
		for(loop=0; loop!=8; loop++)
		{
			if (tag & 0x80)
			{
				if ((count=*inBuf++) == 0)
				{
					return outputBufLen;				// Finished now
				}
				else
				{										// Copy from history
					offset = HISTORY_SIZE-(((MASK_UPPER & count)*SHIFT_UPPER)+(*inBuf++));
					count &= MASK_LOWER;
					count += 2;
					while (count!=0)
					{
						DecompressOutputByte(LZhistory[(LZhistoryOff+offset) & MASK_HISTORY]);
						count--;
					}
				}
			}
			else
			{
				DecompressOutputByte(*inBuf++);			// Copy data byte
			}
			tag += tag;
		}
	}
	return outputBufLen;
}

char* fileLoadDC(char *filename,int *bytesRead)
{
    PKMDWORD    filePtr;
    GDFS        gdfs;
    long        FileBlocks;
    int			i,flag = TRUE;
    char		buffer[256],*fptr, *tmp;
	Sint32		status,size;
	ADXF		adxf;

//	syCacheICI();

	utilUpperStr(filename);
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

	if (strcmp(&filename[strlen(filename)-3], "TXT")==0)
	{

	
	
		// Open input file.
		if(!(gdfs = gdFsOpen(buffer, NULL)))
		{
			debug_printf("********file open error********\n");
			return NULL;
		}
		// Get file size (in blocks/sectors).
	//    if(bytesRead)
		gdFsGetFileSize(gdfs, bytesRead);

		gdFsGetFileSctSize(gdfs, &FileBlocks);
		// Allocate memory to nearest block size (2048 bytes).
	   //	debug_printf("fileblocks=%d\n",FileBlocks);
	   //	debug_printf("trying to get %d of memory\n",FileBlocks*2048);
		filePtr = Align32Malloc(FileBlocks * 2048); //this is a bit big for most files, but we have the room
		if(filePtr == NULL)										  //and several levels won't load otherwise (it is freed v.soon)
			debug_printf("Error\n");

		// Read file to memory region (Destination address must be 32-byte aligned).
		gdFsReqRd32(gdfs, FileBlocks, filePtr);
		// Wait for file access to finish.
		while(gdFsGetStat(gdfs) != GDD_STAT_COMPLETE)
		{
  			status = gdFsGetStat(gdfs);
    		if(status == GDD_STAT_ERR)
    			utilPrintf("Error\n");
		}		  
		// Close file.
		gdFsClose(gdfs); 
	} else {


		// JIM : Alternate file loading using the middleware ADX stuff. Just make sure that you call
		// ADXT_Init() before you do any fileloads, else it may do strange things....

		// Open input file.......
		if (!(adxf = ADXF_Open(buffer, NULL)))
		{
			debug_printf("********file open error********\n");
			return NULL;
		} // end-if

		// Get the file size (blocks/sectors)
		FileBlocks = ADXF_GetFsizeSct(adxf);

		// Alloc the memory needed for loading the file
		filePtr = Align32Malloc(FileBlocks * 2048); //this is a bit big for most files, but we have the room
		if(filePtr == NULL)							 //and several levels won't load otherwise (it is freed v.soon)
			debug_printf("Error\n");

		// Read the file into memory
		ADXF_ReadNw32(adxf, FileBlocks, filePtr);	

		// Wait until we're done reading the file or an error occurs somewhere....
		while(ADXF_GetStat(adxf) != ADXF_STAT_READEND)
		{
			status = ADXF_GetStat(adxf);
			if (status == ADXF_STAT_ERROR)
				debug_printf("Error\n");
		}

		// Close the file
		ADXF_Close(adxf);
	}

    return filePtr;
}


char* fileLoad(char *filename,int *bytesRead)
{
	char *file, *unpacked;
	long *lPtr;


	file = fileLoadDC(filename, bytesRead);

	lPtr = (long*)file;
	if(lPtr[0]==FLA_MAGIC)
	{
		debug_printf("********packed %d byte file spotted\n",lPtr[1]);

		*bytesRead = lPtr[1];
		unpacked = Align32Malloc(lPtr[1]);
		utilDecompressBuffer(file+8, unpacked);
		Align32Free(file);
		file = unpacked;
	}
	else
	debug_printf("********NOT PACKED - lPtr[0]=%d\n",lPtr[0]) ;



	return file;


//	fout = (char*)Align32Malloc(((((unsigned long)filePtr[1]+1024)>>11)+1)<<11);		// round up to next 2k boundary
	


	 /*
	// Check to see if the data is compressed.
	lPtr = (long*) filePtr;
	if (*lPtr == FLA_MAGIC))
	{
		utilPrintf("File %s is a packed file. Unpacking %l bytes of data.\n", fName, lPtr[1]);
		size= lPtr[1];

		if ((realData = malloc(lPtr[1]))==NULL)
			return 0;

		utilDecompressBuffer(data2+8, realData);
		free(data2);
		data2 = realData;
	}
	   */



	//filePtr is ptr to start of file
/*	if(filePtr[0]==FLA_MAGIC)//we have a flatpacked file
	{
		debug_printf("packed file spotted\n");
		debug_printf("going to be %d bytes\n",filePtr[1]);


		// allocate uncompressed amount
		fout = (char*)Align32Malloc(((((unsigned long)filePtr[1]+1024)>>11)+1)<<11);		// round up to next 2k boundary
		fout_start = fout;
		fin = (char*)filePtr[2];
		debug_printf("decompress start\n");
		utilDecompressBuffer(fin, fout);
		debug_printf("decompress end\n");
			
//			ptr = buffer+(lPtr[1]-size)+1024;
  //			PCread(fHandle, ptr, size);
	//		utilDecompressBuffer(ptr+8, buffer);
	  //		*length = lPtr[1];
		Align32Free(filePtr);//should be finished with by now
		return fout_start;
	}

    return filePtr;*/
}

/*

FileIODataType	fileIO;
static char		sectorBuf[2048];

		}
		lPtr = (unsigned long *)sectorBuf;
		PClseek(fHandle, 0,0);
		// does it start with the magic number?
		if (lPtr[0]==FLA_MAGIC)
		{
			// allocate uncompressed amount
			buffer = MALLOC((((lPtr[1]+1024)>>11)+1)<<11);		// round up to next 2k boundary
			if(buffer)
			{
				ptr = buffer+(lPtr[1]-size)+1024;
				PCread(fHandle, ptr, size);
				utilDecompressBuffer(ptr+8, buffer);
				*length = lPtr[1];
			}
			else
			{
				printf("Error allocating PC file '%s' (FlatPacked)\n", fName);
				PCclose(fHandle);
				continue;
			}
		}
		else
		{
			// file is not compressed, so just allocate file size
			if ((buffer = MALLOC(((size>>11)+1)<<11))==NULL)
			{
				printf("Error allocating PC file '%s'\n", fName);
				PCclose(fHandle);
				continue;
			}
			if (PCread(fHandle, buffer, size)!=size)
			{
				printf("Error reading PC file '%s'\n", fName);
				FREE(buffer);
				PCclose(fHandle);
				continue;
			}
			*length = size;
		}
		PCclose(fHandle);
		break;
	}
  */