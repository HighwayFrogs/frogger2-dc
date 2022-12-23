#include <windows.h>
#include <stdio.h>
#include <string.h>

#include "crc32.h"
#include "gelf.h"
#include "debug.h"
#include "mytypes.h"
#include "stream.h"
#include "error.h"

char sTextFile[64];

WIN32_FIND_DATA FileData;
HANDLE hSearch;

SAVEDATA sD;

/*
void Decompress(char *s)
{
	ULONG nBytes;
	FILE *hFi, *hFT;
	NSPRITE *pData;
	UBYTE *pPal;
	UBYTE *pImg;
	char sBMPName[8];
	int c;

	// got to decompress it all back into BMPs :(

	if(hFi=fopen(s, "rb"), !hFi)
		ERR_TellThemOff(ERR_FATAL, "Couldn't open file");

	fseek(hFi, 0, SEEK_END);
	nBytes=ftell(hFi);
	fseek(hFi, 0, SEEK_SET);

	printf("File is %d bytes in size\n", nBytes);

	if(pData=(NSPRITE *)malloc(nBytes), !pData)
		ERR_TellThemOff(ERR_FATAL, "Out of memory!");

	fread(pData, nBytes, 1, hFi);
	
	if(*sTextFile)
		hFT=fopen(sTextFile, "rt");

	STR_SetSPRPointers(pData);
	c=0;
	while(1)
	{
		BMPINFO bmpInf;

		STR_GetBMPInfo(&bmpInf, pData, c);
		pPal=STR_GetPalette(pData+c);
		pImg=STR_GetBitmap(pData+c);

		if(*sTextFile)
		{
			char ch;
			int cc=0;
			*sBMPName=0;

			while(1)
			{
				ch=fgetc(hFT);
				if(ch>31)
					sBMPName[cc++]=ch;
				else
				{
					sBMPName[cc]=0;
					break;
				}
			}
		}

		if(*sTextFile)
			strcat(sBMPName, ".xxx");
		else
			sprintf(sBMPName, "s%d.bmp\0", c);

		printf("Saving %d\n", c);

		printf("w=%d, h=%d, pPal=%p, pImg=%p\n", pData[c].w, pData[c].h, pPal, pImg);
		if(bmpInf.nBPP==8)
			SaveBMP8bpp(sBMPName, pData[c].w?pData[c].w:256, pData[c].h?pData[c].h:256, pPal, pImg);
		else
			SaveBMP4bpp(sBMPName, pData[c].w?pData[c].w:256, pData[c].h?pData[c].h:256, pPal, pImg);

		if(pData[c].flags&LASTSPR)
			break;

		c++;
	}

	if(*sTextFile)
		fclose(hFT);

	free(pData);
	fclose(hFi);
}
*/

void Compress(char *s)
{
	int x,y,bpp;
	void *pPal, *pImg;
	BMPLIST *pBMPList;

	nBMPs=0;

	pRoot=new BMPLIST;
	pRoot->pLast=pRoot;
	pRoot->pNext=pRoot;

	InitCRCTable();

	// aahh. This must be a filename
	if(hSearch=FindFirstFile(s, &FileData), hSearch==INVALID_HANDLE_VALUE)
		ERR_TellThemOff(ERR_FATAL, "No Files!");

	while(1)
	{
	// Do stuff with BMPs here
		//printf("Getting info on '%s'\n", FileData.cFileName);
		if(!gelfInfo_BMP(FileData.cFileName, &x, &y, &bpp))
		{
			printf("Trying to load %s\n", FileData.cFileName);
			ERR_TellThemOff(ERR_FATAL, "Couldn't load BMP");
		}
		else
		{
			//printf("It's %d x %d x %d\n", x,y,bpp);

			pBMPList=STR_AddNode(FileData.cFileName);
			strcpy(pBMPList->bmp.sFileName, FileData.cFileName);
			pBMPList->bmp.nX=x;
			pBMPList->bmp.nY=y;

			//printf("Attempting to load '%s'  ", FileData.cFileName);
			//printf("It's %d x %d x %d\n", x,y,bpp);

			if(bpp>8)
			{			
				printf("loading %d bit BMP\n", bpp);
	
				pPal=(void *)(-1);
				pImg=(UBYTE *)gelfLoad_BMP(FileData.cFileName, NULL, &pPal, NULL, NULL, NULL,
											GELF_IFORMAT_24BPPBGR, GELF_IMAGEDATA_TOPDOWN);
				
				printf("*pImg=%x\n", *((ULONG *)pImg));

				STR_Reduce24Bit((unsigned char **)&pImg, (unsigned char **)&pPal, x, y);
				bpp=8;

			}
			else
			{	
				pPal=NULL;
				pImg=(UBYTE *)gelfLoad_BMP(FileData.cFileName, NULL, &pPal, NULL, NULL, NULL,
											GELF_IFORMAT_8BPP, GELF_IMAGEDATA_TOPDOWN);
			}

			pBMPList->bmp.nBPP=bpp;
			pBMPList->bmp.pImg=pImg;
			pBMPList->bmp.pPal=pPal;
		}
		
		if(!FindNextFile(hSearch, &FileData))
			break;
	}

	sD.pBMPList=pRoot;

	STR_SaveSPT(&sD);
}

void CompressFromFile(char *s)
{
	int x,y,bpp;
	void *pPal, *pImg;
	BMPLIST *pBMPList;

	int nBMPsFailed=0;


	FILE *hTextFile;
	char sBMPName[32];

	// use text file as list of BMPs

	nBMPs=0;

	pRoot=new BMPLIST;
	pRoot->pLast=pRoot;
	pRoot->pNext=pRoot;

	InitCRCTable();


	hTextFile=fopen(s, "rt");
	if(!hTextFile)
	{
		ERR_TellThemOff(1, "Can't find input text file");
	}

	while(!feof(hTextFile))
	{
		fscanf(hTextFile, "%s", sBMPName);

		if(!gelfInfo_BMP(sBMPName, &x, &y, &bpp))
		{
			char err[128];

			sprintf(err, "Couldn't load '%s'", sBMPName);
			nBMPsFailed++;

			if(sD.gDontQuit)
			{
				ERR_TellThemOff(0, err);
			}
			else
			{
				ERR_TellThemOff(ERR_FATAL, err);
			}
		}
		else
		{
			//printf("It's %d x %d x %d\n", x,y,bpp);

			pBMPList=STR_AddNode(sBMPName);
			strcpy(pBMPList->bmp.sFileName, sBMPName);
			pBMPList->bmp.nX=x;
			pBMPList->bmp.nY=y;
			pBMPList->bmp.nBPP=bpp;

			//printf("Attempting to load '%s'  ", FileData.cFileName);
			//printf("It's %d x %d x %d\n", x,y,bpp);

			if(bpp>8)
				ERR_TellThemOff(ERR_FATAL, "Can't load this BMP (bpp>8)");

			pPal=NULL;
			pImg=(UBYTE *)gelfLoad_BMP(sBMPName, NULL, &pPal, NULL, NULL, NULL,
										GELF_IFORMAT_8BPP, GELF_IMAGEDATA_TOPDOWN);
			pBMPList->bmp.pImg=pImg;
			pBMPList->bmp.pPal=pPal;
		}
	}

	fclose(hTextFile);
	printf("WARNING: You specified %d file(s) which couldn't be loaded\n", nBMPsFailed);

	sD.pBMPList=pRoot;
	STR_SaveSPT(&sD);
}



int main(int argc, char **argv)
{
	int i;
	char *s;

	gelfInit();

	if(argc<2)
	{
		printf("Sprite Chopper - by Chris Wilson - (C)Interactive Studios Ltd.(15/2/98)\n");
		printf("doschop  -[options] filename[.bmp]\n");
		printf("options: -c = crop\n");
		printf("         -f = set output filename\n");
		printf("         -x = use text file to name BMPs\n");
		printf("         -u = don't compress\n");
		printf("         -q = don't bail out if loading a BMP fails\n");
		printf("         -r = reduce colours     i.e r4 = reduce to 4 bits (16 colour)\n");
		printf("         -m = mipmap levels      i.e m4 = mipmap to 4 levels\n");
		printf("         -w = set slice width in bytes (default = 64)\n");
		printf("         -h = set slice height pixels (default = 256)\n");
		printf("         -t = transparent colour i.e tFF0000 = true red\n");
		printf("              (defaults to magenta FF00FF)\n");
		printf("\nIf the filename is preceded with an @ symbol, the file will be interpreted as a\n");
		printf("text file containing the names of the BMP files to be used.\n");
		printf("\nThis software is subject to continuous development. Please email any bug\n");
		printf("reports or suggestions you may have to: cwilson@intstudios.co.uk\n");
		printf("\n");
	}

	strcpy(sD.sFileName, "out.spt");
	sD.gSaveTxt=FALSE;
	sD.gCropTrans=FALSE;
	sD.gSaveRaw=FALSE;
	sD.gFTPink=FALSE;
	sD.gDontQuit=FALSE;
	sD.gReduceTo=0;
	sD.nMipMaps=0;
	sD.nXBytes=64;
	sD.nYBytes=256;
	strcpy(sD.sAlpha, "ai_");
	sD.pTrans[0]=0xff;
	sD.pTrans[1]=0x00;
	sD.pTrans[2]=0xff;

	for(i=1; i<argc; i++)
	{
		s=argv[i];
		if(*s=='-') // Ahhh. A switch
		{
			int c=1;

			while(1)
			{
				if(!s[c])
					break;

				s[c]|=32;
				switch(s[c])
				{
				case 'f':
					strcpy(sD.sFileName, s+c+1);
					c+=strlen(sD.sFileName);
					break;
				case 'x':
					sD.gSaveTxt=TRUE;
					break;
				case 'c':
					sD.gCropTrans=TRUE;
					break;
				case 'u':
					sD.gSaveRaw=TRUE;
					break;
				case 'q':
					sD.gDontQuit=TRUE;
					break;
				case 't':
					sD.pTrans[0]=Str2Hex(s+c+1,2);
					sD.pTrans[1]=Str2Hex(s+c+3,2);
					sD.pTrans[2]=Str2Hex(s+c+5,2);
					c+=6;
					break;
				case 'r':
					sD.gReduceTo=Str2Hex(s+c+1,1);
					c++;
					break;
				case 'm':
					sD.nMipMaps=Str2Hex(s+c+1,1);
					c++;
					break;
				case 'w':
					{
						char sW[8];
						int i;
						char b;

						for(i=0; ; i++)
						{
							b = s[c+1];
							if((b>='0') && (b<='9'))
							{
								sW[i] = b;
								c++;
							}
							else
								break;
						}

						sD.nXBytes = atoi(sW);
//						printf("width = %d\n", sD.nXBytes);
					}
					break;
				case 'h':
					{
						char sW[8];
						int i;
						char b;

						for(i=0; ; i++)
						{
							b = s[c+1];
							if((b>='0') && (b<='9'))
							{
								sW[i] = b;
								c++;
							}
							else
								break;
						}
						sD.nYBytes = atoi(sW);
//						printf("height = %d\n", sD.nYBytes);
					}
					break;
				default:
					ERR_TellThemOff(0, "Bad option");
				}

				c++;
			}
		}
		else
		{
			if(*s=='@')
			{
				CompressFromFile(s+1);
			}
			else
			{
				Compress(s);
			}
		}

	}

	gelfShutdown();

	return(0);
}

