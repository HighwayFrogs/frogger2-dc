/* Save bitstream or raw */

#include "main.h"

#ifndef DOSVER
#include "stdafx.h"
#else
#include "windows.h"
#include "ctype.h"
#endif

#include <malloc.h>
#include <stdio.h>
#include "debug.h"
#include "gelf.h"


#include "mytypes.h"
#include "error.h"
#include "crc32.h"
#include "stream.h"

#ifndef DOSVER
	#ifdef _DEBUG
		#define new DEBUG_NEW
		#undef THIS_FILE
		static char THIS_FILE[] = __FILE__;
	#endif
#endif

int nBMPs=0;
UBYTE pLPal[768];
void *pPal=NULL;
UBYTE *pImg;
FILE *hFi;
int x,y,bpp;
UBYTE trans[3]={0xff,0,0xff};

char sStrLkUp[126];

int gQuiet=1;
int gUncompressed=0;
int gCrop=0;
int gDecomp=0;
int gRemap=0;
int gSplit=0;
int gRefCentre=0;

int count;
int bOut;

BMPLIST *pRoot;

char *ToUpper(char *sIn)
{
   char *p;
   static char sOut[256];
   
   strcpy(sOut, sIn);

   for(p=sOut; p<sOut+strlen(sOut); p++)
   {
      if(islower(*p))
		  *p=toupper(*p);
   }

   return sOut;
}

char *GetFileNameWithExt(char *sIn)
{
	int i;
	//static char sOut[256];

	for(i=strlen(sIn); i>=0; --i)
	{
		if(sIn[i]=='\\')
			return sIn+i+1;
	}
	return sIn;
}

char *GetFileName(char *sIn)
{
	int i;
	char *sInt=GetFileNameWithExt(sIn);
	static char sOut[256];

	strcpy(sOut, sInt);

	for(i=strlen(sOut); i>=0; --i)
	{
		if(sOut[i]=='.')
		{
			sOut[i]=0;
			break;
		}
	}

	return(sOut);
}

UBYTE Str2Hex(char *sStr, int nChar)
{
	static int fCalledB4=0;
	int result=0;
	int i;

	if(!fCalledB4)
	{
		int lc, c,v;

		for(c=0; c<126; c++)
			sStrLkUp[c]=0;

		for(c=65, lc=97, v=10; c<=70; c++, lc++, v++)
		{
			sStrLkUp[c]=v;
			sStrLkUp[lc]=v;
		}

		for(c=48, v=0; c<=57; c++, v++)
			sStrLkUp[c]=v;
	}
	
	for(i=0; i<nChar; i++)
	{
		result=result<<4;
		result|=sStrLkUp[sStr[i]];
	}

	return(result);
}

void Align(FILE *hFi)
{
	int fp;

	fp=ftell(hFi); // fp|!fp  That is the question
	fp&=3;
	if(!fp)
		return;
	fseek(hFi, 4-fp, SEEK_CUR);
}

/*BMPLIST *STR_AddStartNode(void)
{
	BMPLIST *pThis;

	pThis=(BMPLIST *)malloc(sizeof(BMPLIST));

	return pThis;
}*/

BMPLIST *STR_AddNode(char *sFN)
{
	char s1[32];
	char s2[32];

	BMPLIST *pF, *pBMP;
	int i=0;

	strcpy(s2, ToUpper(sFN));

	for(pBMP=pRoot->pNext; pBMP!=pRoot; pBMP=pBMP->pNext)
	{
		i++;

		strcpy(s1, ToUpper(pBMP->bmp.sFileName));
		if(strcmp(s1, s2)>0)
			break;
	}

	pF=(BMPLIST *)malloc(sizeof(BMPLIST));

	if(!i)
		pBMP=pRoot;

	pF->pLast=pBMP->pLast;
	pF->pNext=pBMP;
	pBMP->pLast->pNext=pF;
	pBMP->pLast=pF;

#ifndef DOSVER
	pF->hWnd=NULL;
#endif

	return pF;
}

int STR_DeleteNode(BMPLIST *pB)
{
	pB->pNext->pLast=pB->pLast;
	pB->pLast->pNext=pB->pNext;

	free(pB);

	return 0;
}

int compareRGB(ULONG rgb1, ULONG rgb2)
{
	int r1,r2, g1,g2, b1,b2;

	r1=rgb1&0xff;
	g1=(rgb1&0xff00)>>8;
	b1=(rgb1&0xff0000)>>16;

	r2=rgb2&0xff;
	g2=(rgb2&0xff00)>>8;
	b2=(rgb2&0xff0000)>>16;

	return (int)sqrt((double)(((r1-r2)*(r1-r2))+((g1-g2)*(g1-g2))+((b1-b2)*(b1-b2))));
}

ULONG averageRGB(ULONG rgb1, ULONG rgb2)
{
	int r1,r2, g1,g2, b1,b2;

	r1=rgb1&0xff;
	g1=(rgb1&0xff00)>>8;
	b1=(rgb1&0xff0000)>>16;

	r2=rgb2&0xff;
	g2=(rgb2&0xff00)>>8;
	b2=(rgb2&0xff0000)>>16;

	return ((r1+r2)>>2)|(((g1+g2)>>2)<<8)|(((b1+b2)>>2)<<16);
}

void ReduceTo(ULONG nTo, ULONG nFrom, ULONG nPix, ULONG *pPal, ULONG *pImg)
{
	int minDiff;
	int diff;
	int p1,p2;
	int c1,c2;
	int i;

	int toGo=nFrom-nTo;

	ULONG *pNewPal;
	UBYTE *pNewImg;

/*	for(p1=0; p1<nFrom; p1++)
	{
		pTemp[p1]=~0;
	}
*/
	while(toGo)
	{
		minDiff=0x7fffffff;

		for(p1=0; p1<nFrom; p1++)
		{
			if(pPal[p1]==~0)
				continue;

			for(p2=0; p2<nFrom; p2++)
			{
				if(pPal[p2]==~0)
					continue;

				if(p1!=p2)
				{
					diff=compareRGB(pPal[p1], pPal[p2]);
					if(diff<minDiff)
					{
						minDiff=diff;
						c1=p1;
						c2=p2;
					}
				}
			}
		}

		printf("colours %d and %d were closest, (%d left)\n", c1, c2, toGo);

		if(c2<c1)
		{// swap c2 & c1
			int t=c1;
			c1=c2;
			c2=t;
		}

		// find average colour and set c1 to be that. (c1 is the one we're going to keep)
		pPal[c1]=averageRGB(pPal[c1], pPal[c2]);

		pPal[c2]=~0;

//		for(p1=c2+1; p1<nFrom; p1++)
//		{
//			pPal[p1]=pPal[p1-1];	// move each palette entry bigger than c2 down one.
//		}
		
		for(p1=0; p1<nPix; p1++)	// set any pixels that are c2 to be c1
		{
			if(pImg[p1]==c2)
				pImg[p1]=c1;
		}

		toGo--;
	}

	pNewPal=(ULONG *)malloc(256*sizeof(ULONG));
	pNewImg=(UBYTE *)malloc(nPix);

	p1=0;

	for(p2=0; p2<nFrom; p2++)
	{

		if(pPal[p2]!=~0)
		{
			printf("doing colour %d (%x)\n", p1, pPal[p2]);

			pNewPal[p1]=pPal[p2];

			for(i=0; i<nPix; i++)
			{	// copy over pixels
				if(pImg[i]==p2)
					pNewImg[i]=p1;
			}

			p1++;
		}
	}

	// copy back to original ones
	for(i=0; i<256; i++)
	{
		pPal[i]=pNewPal[i];
	}

	for(i=0; i<nFrom; i++)
	{
		pImg[i]=pNewImg[i];
	}

	free(pNewImg);
	free(pNewPal);

}

int STR_Reduce24Bit(UBYTE **ppImg, UBYTE **ppPal, int x, int y)
{
	ULONG nBPP;
	ULONG i,c,j;
	ULONG nCols=0;
	ULONG nBigPal=0;
	ULONG nPix=x*y;
	ULONG *pBigPal;
	ULONG *pNewImg;
	UBYTE *pFinalImg;
	ULONG palEntry;
	int gMatch;

	pBigPal=(ULONG *)malloc(sizeof(ULONG)*nPix); // max size, probably smaller than this
	pNewImg=(ULONG *)malloc(sizeof(ULONG)*nPix);

	printf("pImg=%p\n", *ppImg);
	printf("x = %d, y = %d\n", x,y);

	for(i=0,c=0; i<nPix; i++)
	{// go through each pixel and see if RGB value matches any in our BIG palette
		palEntry=(*ppImg)[c++];
		palEntry|=((*ppImg)[c++])<<8;
		palEntry|=((*ppImg)[c++])<<16;
		
		//printf("palEntry=%x, c=%d\n", palEntry, c);

//		palEntry=(*ppImg)[i];

		gMatch=FALSE;
		for(j=0; j<nBigPal; j++)
		{
			if(palEntry==pBigPal[j])
			{
				gMatch=TRUE;
				break;
			}
		}

		pNewImg[i]=j; // pixel value into palette

		if(!gMatch)
			pBigPal[nBigPal++]=palEntry; // new palette entry
	}

	printf("nBigPal = %d\n", nBigPal);

	nBPP=8;
	// now we've got a BIG palette. If it has 256 or less colours, cool.
	if(nBigPal>256)
	{ // reduce it by throwing colours away
		ReduceTo(256, nBigPal, nPix, pBigPal, pNewImg);
	}
	else
	{
		if(nBigPal<=16)
			nBPP=4;
	}

	// malloc new palette
	*ppPal=(UBYTE *)malloc((1<<nBPP)*3);

	for(i=0,c=0; i<((ULONG)1<<nBPP); i++)
	{
		palEntry=pBigPal[i];

		(*ppPal)[c++]=(UBYTE)palEntry&0xff;
		palEntry=palEntry>>8;
		(*ppPal)[c++]=(UBYTE)palEntry&0xff;
		palEntry=palEntry>>8;
		(*ppPal)[c++]=(UBYTE)palEntry&0xff;
	}

	pFinalImg=(UBYTE *)malloc(nPix);

	for(i=0; i<nPix; i++)
		pFinalImg[i]=(UBYTE)pNewImg[i];

	*ppImg=pFinalImg;

	return nBPP;
}

int STR_LoadNewBMP(BMPINFO *pBMP)
{
	void *pPal;
	void *pImg;
	int x,y,bpp;

	if(!gelfInfo_BMP(pBMP->sFileName, &x, &y, &bpp))
		dprintf("Couldn't load BMP");
	else
	{
		printf("loading %s, bpp = %d\n", pBMP->sFileName, bpp);

		if(bpp>8)
		{
			pPal=(void *)-1;
			// we'll generate our own palette
			pImg=gelfLoad_BMP(pBMP->sFileName, NULL, &pPal, NULL, NULL, NULL,
								GELF_IFORMAT_24BPPRGB, GELF_IMAGEDATA_TOPDOWN);

			pBMP->nBPP=STR_Reduce24Bit((UBYTE **)&pImg, (UBYTE **)&pPal, x, y);
		}
		else
		{

			pBMP->nBPP=bpp;
			pPal=NULL;

			pImg=gelfLoad_BMP(pBMP->sFileName, NULL, &pPal, NULL, NULL, NULL,
								GELF_IFORMAT_8BPP, GELF_IMAGEDATA_TOPDOWN);
		}

		pBMP->pImg=(UBYTE *)pImg;
		pBMP->pPal=(UBYTE *)pPal;
		pBMP->nX=x;
		pBMP->nY=y;
	}
	
	return (int)pImg;
}

//#define REALLOC(P,S) ((P)?realloc((P), (S)+_msize(P)):malloc(S))

int STR_MakeMipMap(BMPINFO *pBMP, int mipLevel)
{
	char *pNewImg;
	short *pNewPal;
	long *pTempImage;
	char *pImg=(char *)pBMP->pImg;
	char *pPal=(char *)pBMP->pPal;
	int  nX=pBMP->nX;
	int  nY=pBMP->nY;
	int  newX, newY;
	int  ctr,cX,cY;

	printf("?");

	pNewPal=(short *)malloc(16*2);
	newX=nX>>mipLevel;
	newY=nY>>mipLevel;

	pTempImage=(long *)malloc(sizeof(long)*(newX*newY));
	pNewImg=(char *)malloc(sizeof(char)*(newX*newY));

	//printf("Image %d by %d, reduces to %d by %d\n", nX,nY, newX, newY);

	ctr=0;
	for(cY=0; cY<newY; cY++)
	{
		for(cX=0; cX<newX; cX++)
		{
			char r,g,b;
			char pix=pImg[(cX<<mipLevel)+((cY<<mipLevel)*cX)];
			short p=pPal[pix];

			pNewImg[ctr++]=pix;
		}
	}
	
	pBMP->pImg=pNewImg;
	pBMP->nX=newX;
	pBMP->nY=newY;

	printf("!\n");

	return 0;
}

//#define maxHeight 64

//#define PP if(tpal) printf("[%d] %d\n", __LINE__, tpal[2]);	else printf("pointer buggered\n");

int STR_SaveSPT(SAVEDATA *pSD)
{
	// copy linked list into our array (simpler this way)

	BMPINFO pBMPs[1024];
//	int nBMPInf;
	int j;
	int startX, startY, tWidth, tHeight, nChops, nChopsY, nLeft, nLeftY;
	int baseX;
	int maxWidth;
	int maxHeight;
	int rX,rY;
	FILE *hFi, *hFt;
	BMPLIST *pBMP;
	BMPLIST *pRoot=pSD->pBMPList;
	BMPINFO bmpTemp;
	int nBMPs;

	hFi=fopen(pSD->sFileName, "wb");
	if(pSD->gSaveTxt)
		hFt=fopen("out.txt","wt");

	/*for(nBMPInf=0, pBMP=pRoot->pNext; pBMP!=pRoot; pBMP=pBMP->pNext)
		nBMPInf++;
	if(pSD->gFTPink)
		nBMPInf++;

	pBMPs=(BMPINFO *)malloc(sizeof(BMPINFO)*nBMPInf);
	*/

	nBMPs=0;

	// create an extra BMP here, if gFTPink==TRUE
	if(pSD->gFTPink)
	{
		UBYTE *pImg;
		UBYTE *pPal;
		int i;

		//pBMPs=(BMPINFO *)REALLOC(pBMPs, sizeof(BMPINFO));

		#define XS 16
		#define YS 16

		pImg=(UBYTE *)malloc(XS*YS);
		pPal=(UBYTE *)malloc(3*16); // 16 colour texture

		for(i=0; i<16; i++)
		{
			pPal[(i*3)]=0xff;
			pPal[(i*3)+1]=0xff;
			pPal[(i*3)+2]=0xff;
		}

		for(i=0; i<(XS*YS); i++)
			pImg[i]=0; // colour 0 

		pBMPs[nBMPs].sFileName[0]=0;
		pBMPs[nBMPs].nX=XS;
		pBMPs[nBMPs].nY=YS;
		pBMPs[nBMPs].nBPP=4;
		pBMPs[nBMPs].pImg=(void *)pImg;
		pBMPs[nBMPs].pPal=(void *)pPal;
		pBMPs[nBMPs].minX=0;
		pBMPs[nBMPs].minY=0;
		pBMPs[nBMPs].maxX=XS;
		pBMPs[nBMPs].maxY=YS;

		nBMPs++;
	}

	for(pBMP=pRoot->pNext; pBMP!=pRoot; pBMP=pBMP->pNext)
	{
		int i;
		int transCol;

		UBYTE *pImg=(UBYTE *)pBMP->bmp.pImg;
		UBYTE *pPal=(UBYTE *)pBMP->bmp.pPal;
		int palSize=(pBMP->bmp.nBPP==8)?256:16;

		if((pBMP->bmp.nBPP!=4) && (pBMP->bmp.nBPP!=8))
		{
			fprintf(stderr, "Image must be 4 or 8 bit\n");
			exit(1);
		}

		// is there any transparency ?

		//printf("\nProcessing '%s' ...\n",  pBMP->bmp.sFileName);
		//printf("  Sprite is %d colours\n", palSize);
		transCol=-1;

		for(i=0; i<palSize; i++)
		{
			if((pPal[(i*3)+0]==pSD->pTrans[0])
				&& (pPal[(i*3)+1]==pSD->pTrans[1])
				&& (pPal[(i*3)+2]==pSD->pTrans[2]))
			{
				if(transCol!=-1)
				{
					printf("WARNING: '%s' has more than one transparent colour. Expect problems!\n", pBMP->bmp.sFileName);
				}

				transCol=i;
			}
		}


		//printf("  Transparent palette entry is %d\n", transCol);

		pBMP->bmp.nTransPix=0;

		if(transCol!=-1)
		{
			// right. Swap palette entry 0 with transparent,
			// but only if it is not 0

			for(i=0; i<(pBMP->bmp.nX*pBMP->bmp.nY); i++)
			{
				if(pImg[i]==transCol)
				{
					pBMP->bmp.nTransPix++;
				}
			}

			if(transCol>0)
			{
				pPal[(transCol*3)+0]=pPal[0];
				pPal[(transCol*3)+1]=pPal[1];
				pPal[(transCol*3)+2]=pPal[2];

				pPal[0]=pSD->pTrans[0];
				pPal[1]=pSD->pTrans[1];
				pPal[2]=pSD->pTrans[2];

				for(i=0; i<(pBMP->bmp.nX*pBMP->bmp.nY); i++)
				{
					if(pImg[i]==transCol)
					{
						pImg[i]=0;
					}
					else
					{
						if(pImg[i]==0)
							pImg[i]=transCol;
					}
				}
			}

			//printf("  Image has %d transparent pixels\n", pBMP->bmp.nTransPix);
		}


		i=0;
		//for(i=0; i<(pSD->nMipMaps+1); i++)
		{
			//printf("Saving BMP ...\n");
			memcpy(&bmpTemp, &(pBMP->bmp), sizeof(BMPINFO));
			//if(i>0)
			//{
			//	// right. We must do mippy-mappy stuff
			//	STR_MakeMipMap(&bmpTemp, i); // where i = level
			//}

			if(pSD->gSaveTxt)
			{
				fprintf(hFt, "%s ", ToUpper(GetFileName(bmpTemp.sFileName)));
				fprintf(hFt, "%8x\n", UpdateCRC(ToUpper(GetFileName(bmpTemp.sFileName))));
			}

			// get limits of sprite
			if(pSD->gCropTrans)
				STR_GetBitmapLimits(&bmpTemp, 0, pSD->pTrans);
			else
			{
				bmpTemp.minX=0;
				bmpTemp.minY=0;
				bmpTemp.maxX=bmpTemp.nX;
				bmpTemp.maxY=bmpTemp.nY;
			}

			// work out maximum width
			if(bmpTemp.nBPP==4)
				maxWidth=(pSD->nXBytes)*2;
			else
				maxWidth=pSD->nXBytes;

			maxHeight = pSD->nYBytes;

			tWidth=	bmpTemp.maxX-bmpTemp.minX;
			tHeight=bmpTemp.maxY-bmpTemp.minY;
			startX=	bmpTemp.minX;
			startY=	bmpTemp.minY;


//		printf("Splitting %d %d ... %d %d\n",bmpTemp.minX,bmpTemp.minY,bmpTemp.maxX,bmpTemp.maxY);

// okeydokey, vsplit first?

			baseX = startX;
			for(;startY <bmpTemp.maxY;)
			{
				startX = baseX;
				for(;startX <bmpTemp.maxX;)
				{
					//printf("    Splitting ... ");
					memcpy(pBMPs+nBMPs, &bmpTemp, sizeof(BMPINFO)); // copy bmp data (not much changes)

					pBMPs[nBMPs].fSplit=TRUE;

					if(startX == bmpTemp.minX)	// only the first sprite in each column can ever have a v-split marker
						pBMPs[nBMPs].fvSplit=TRUE;
					else
						pBMPs[nBMPs].fvSplit=FALSE;


					pBMPs[nBMPs].minX=startX;
					startX+=maxWidth;
					pBMPs[nBMPs].maxX=startX;

					pBMPs[nBMPs].minY=startY;
					pBMPs[nBMPs].maxY=startY + maxHeight;


					nBMPs++;

					if(pBMPs[nBMPs-1].maxY>=bmpTemp.maxY)
					{
						pBMPs[nBMPs-1].maxY=bmpTemp.maxY;
						pBMPs[nBMPs-1].fvSplit=FALSE;
					}

					if(pBMPs[nBMPs-1].maxX>=bmpTemp.maxX)
					{
						pBMPs[nBMPs-1].fSplit=FALSE;
						pBMPs[nBMPs-1].maxX=bmpTemp.maxX;
						//printf("BMP goes %d to %d\n", pBMPs[nBMPs-1].minX, pBMPs[nBMPs-1].maxX);
//						break;
					}
//					printf("Intermediate %d %d ... %d %d\n",pBMPs[nBMPs-1].minX,pBMPs[nBMPs-1].minY,pBMPs[nBMPs-1].maxX,pBMPs[nBMPs-1].maxY);
				}

				startY += maxHeight;
			}
		}
	}

	// write dummy header (we go back and write these at the end)
	fseek(hFi, sizeof(NSPRITE)*nBMPs, SEEK_CUR);	

	for(j=0; j<nBMPs; j++)
	{
		// save palette
		Align(hFi);
		pBMPs[j].offPal=ftell(hFi);

/*			if(tp!=((UBYTE *)pBMPs[j].pPal)[2])
			{
				fprintf(stderr, "palette corrupted!\n");
				exit(1);
			}
*/

		//((UBYTE *)pBMPs[j].pPal)[0]=255;
		//((UBYTE *)pBMPs[j].pPal)[1]=0;
		//((UBYTE *)pBMPs[j].pPal)[2]=255;

		STR_SavePalette(hFi, pBMPs[j].nBPP, (UBYTE *)pBMPs[j].pPal);
	}

	for(j=0; j<nBMPs; j++)
	{
		// save image in a bitstream
		Align(hFi);
		pBMPs[j].offImg=ftell(hFi);
		STR_SaveStream(hFi, pBMPs+j, pSD->pTrans, pSD->gSaveRaw);
	}

	// go back to the beginning and write the header(s)
	fseek(hFi, 0, SEEK_SET);
	for(j=0; j<nBMPs; j++)
	{
		NSPRITE Head;

		rX=pBMPs[j].nX>>1;
		rY=pBMPs[j].nY>>1;

		//printf("Saving header for sprite %d\n", j);
		//printf("Info: offImg=%x, offPal=%x\n", pBMPs[j].offImg, pBMPs[j].offPal);
		//printf("      w=%d, h=%d\n", pBMPs[j].maxX-pBMPs[j].minX, pBMPs[j].maxY-pBMPs[j].minY);

		Head.image=	(UBYTE *)pBMPs[j].offImg;
		Head.pal=	(USHORT *)pBMPs[j].offPal;
		Head.w=		pBMPs[j].maxX-pBMPs[j].minX;
		Head.h=		pBMPs[j].maxY-pBMPs[j].minY;
		Head.u=		pBMPs[j].minX-rX;
		Head.v=		pBMPs[j].minY-rY;
#ifdef CRC
		Head.crc=	UpdateCRC(ToUpper(GetFileName(pBMPs[j].sFileName)));
#endif
		//dprintf("u=%d, v=%d\n", Head.u, Head.v);

		Head.flags=	0;

		if(j==(nBMPs-1))
			Head.flags=LASTSPR;

		if(pBMPs[j].nBPP==8)
		{
			printf("8 bit!\n");
			Head.flags|=EIGHTBIT;
		}

		if(pBMPs[j].fRaw==0)
			Head.flags|=BITSTREAM;

		if(pBMPs[j].offPal==0)
			Head.flags|=ALPHA;
		
		if(pBMPs[j].fSplit)
			Head.flags|=SPLIT;

		if(pBMPs[j].fvSplit)
			Head.flags|=VSPLIT;

		//if(pBMPs[j].fColourKey)
		//	Head.flags|=COLOURKEY;

		if(pBMPs[j].nTransPix)
		{
			//printf("Texture has %d transparent pixels\n", pBMPs[j].nTransPix);
			Head.flags|=COLOURKEY;
		}

		fwrite(&Head, sizeof(NSPRITE), 1, hFi);
	}

	if(pSD->gFTPink)
	{
		free(pBMPs[0].pImg);
		free(pBMPs[0].pPal);
	}

	if(pSD->gSaveTxt)
	{
		fprintf(hFt, "END");
		fclose(hFt);
	}

	fclose(hFi);
//	free(pBMPs);

	return 0;
}

/*
void STR_Compress(char *s)
{
	int j;
	FILE *hFi;
	int newWidth;
	int newLeft;
	int n;
	int origBMPs;

	// aahh. This must be a filename
	if(hSearch=FindFirstFile(s, &FileData), hSearch==INVALID_HANDLE_VALUE)
		ERR_TellThemOff(ERR_FATAL, "No Files!");

	hFi=fopen("test.spt","wb");

	while(1)
	{
	// Do stuff with BMPs here
		//printf("Getting info on '%s'\n", FileData.cFileName);
		if(!gelfInfo_BMP(FileData.cFileName, &x, &y, &bpp))
			ERR_TellThemOff(ERR_FATAL, "Couldn't load BMP");
		else
		{
			//printf("It's %d x %d x %d\n", x,y,bpp);

			strcpy(pBMPs[nBMPs].sFileName, FileData.cFileName);
			pBMPs[nBMPs].nX=x;
			pBMPs[nBMPs].nY=y;
			pBMPs[nBMPs].nBPP=bpp;

			pPal=NULL;
			pImg=(UBYTE *)gelfLoad_BMP(FileData.cFileName, NULL, &pPal, NULL, NULL, NULL,
										GELF_IFORMAT_8BPP, GELF_IMAGEDATA_TOPDOWN);
			pBMPs[nBMPs].pImg=pImg;
			pBMPs[nBMPs].pPal=pPal;

			//printf("pImg=%p, pPal=%p\n", pImg, pPal);

			// write dummy header (we go back and write these at the end)
			fseek(hFi, sizeof(NSPRITE), SEEK_CUR);

			// get limits of sprite
			STR_GetBitmapLimits(pBMPs, nBMPs, trans);

			newWidth=pBMPs[nBMPs].maxX-pBMPs[nBMPs].minX;
			newLeft=newWidth-256;
			n=newWidth>>8;
			origBMPs=nBMPs;
			nBMPs++;

			if(n)
			{
				pBMPs[nBMPs-1].maxX=pBMPs[nBMPs-1].minX+256;

				while(1)
				{
					if(newLeft<256) // last one
					{
						memcpy(pBMPs+nBMPs, pBMPs+origBMPs, sizeof(BMPINFO));
						pBMPs[nBMPs].minX=pBMPs[nBMPs-1].maxX;
						pBMPs[nBMPs].maxX=pBMPs[nBMPs].minX+newLeft;
						nBMPs++;
						break;
					}
					if(newLeft>=256)
					{
						memcpy(pBMPs+nBMPs, pBMPs+origBMPs, sizeof(BMPINFO));
						pBMPs[nBMPs].minX=pBMPs[nBMPs-1].maxX;
						pBMPs[nBMPs].maxX=pBMPs[nBMPs].minX+256;
						newLeft-=256;
						nBMPs++;
					}
				}
			}
		}

		if(!FindNextFile(hSearch, &FileData))
			break;
	}

	for(j=0; j<nBMPs; j++)
	{
		// save palette
		if(!gQuiet)
			printf("%s\n", pBMPs[j].sFileName);

		Align(hFi);
		pBMPs[j].offPal=ftell(hFi);
		STR_SavePalette(hFi, pBMPs[j].nBPP, (UBYTE *)pBMPs[j].pPal);
	}
	if(!gQuiet)
		printf("end\n");

	for(j=0; j<nBMPs; j++)
	{
		// save image in a bitstream
		Align(hFi);
		pBMPs[j].offImg=ftell(hFi);
		STR_SaveStream(hFi, pBMPs+j, trans, gUncompressed);
	}

	// go back to the beginning and write the header(s)
	fseek(hFi, 0, SEEK_SET);
	for(j=0; j<nBMPs; j++)
	{
		NSPRITE Head;

		Head.image=	(UBYTE *)pBMPs[j].offImg;
		Head.pal=	(USHORT *)pBMPs[j].offPal;
		Head.w=		pBMPs[j].maxX-pBMPs[j].minX;
		Head.h=		pBMPs[j].maxY-pBMPs[j].minY;
		Head.u=		pBMPs[j].minX;
		Head.v=		pBMPs[j].minY;
		Head.flags=	0;

		if(j==(nBMPs-1))
			Head.flags=LASTSPR;

		if(pBMPs[j].nBPP==8)
			Head.flags|=EIGHTBIT;

		if(pBMPs[j].fRaw==0)
			Head.flags|=BITSTREAM;

		fwrite(&Head, sizeof(NSPRITE), 1, hFi);
	}

	fclose(hFi);
	FindClose(hSearch);
}
*/

void STR_LoadPalette(char *sFi)
{
	FILE *hFi;

	hFi=fopen(sFi, "rb");
	fread(pLPal, 768, 1, hFi);
	fclose(hFi);
}

void STR_MapToPalette(BMPINFO *pBMP, UBYTE *pNewPal)
{
	int pp,pnp;
	int i,j;
	int r,g,b;
	int diff;
	int nPix=(pBMP->nX)*(pBMP->nY);
	UBYTE *pImg=(UBYTE *)pBMP->pImg;
	UBYTE *pOldPal=(UBYTE *)pBMP->pPal;
	int nCols=(pBMP->nBPP==4)?16:256;
	int Closest=0;
	int nDiff;

	for(i=0; i<nPix; i++)
	{
		pp=pImg[i]*3;
		r=pOldPal[pp];
		g=pOldPal[pp+1];
		b=pOldPal[pp+2];

		nDiff=0xffff;
		for(j=0; j<nCols; j++)
		{
			pnp=j*3;
			diff=abs(r-pNewPal[pnp]);
			diff+=abs(g-pNewPal[pnp+1]);
			diff+=abs(b-pNewPal[pnp+2]);
			if(diff<nDiff)
			{
				nDiff=diff;
				Closest=j;
			}
		}
		pImg[i]=Closest; // new colour for the pixel (index to new palette) 
	}

	pBMP->pPal=pNewPal; // set pointer to use new palette
}

void STR_SavePalette(FILE *hFi, int depth, UBYTE *pPal)
{
	int i;
	USHORT PalEnt;
	int r,g,b;
	int pp;
	int nCol=1<<depth;

	for(i=0; i<nCol; i++)
	{
		pp=i*3;

		r=pPal[pp];
		g=pPal[pp+1];
		b=pPal[pp+2];

		if(r+g+b)
			PalEnt=0;
		else
			PalEnt=1;

		PalEnt=PalEnt<<5;
		PalEnt|=b>>3;

		PalEnt=PalEnt<<5;

		PalEnt|=g>>3;
		PalEnt=PalEnt<<5;

		PalEnt|=r>>3;

		//if(depth==4) printf("%d. %x ", i, PalEnt);
		fwrite(&PalEnt, 1, 2, hFi);
	}
}

void STR_GetBitmapLimits(BMPINFO *pInf, int n, UBYTE *pTrans)
{
	int x,y;
	UBYTE *pImg=(UBYTE *)pInf[n].pImg;
	UBYTE *pPal=(UBYTE *)pInf[n].pPal;
	int minX=0xffff,minY=0xffff;
	int maxX=0,maxY=0;
	int off,pp;
	int count=0;

	for(y=0; y<pInf[n].nY; y++)
	{
		for(x=0; x<pInf[n].nX; x++)
		{
			off=(y*pInf[n].nX)+x;
			if((pImg[off]>15) && pInf[n].nBPP==4)
				dprintf("More than 16 colours in 16 colour BMP!!");

			pp=pImg[off]*3;

			if(!((pPal[pp+0]==pTrans[0])&&(pPal[pp+1]==pTrans[1])&&(pPal[pp+2]==pTrans[2])))
			{
				// a solid pixel was found
				if(x<minX) minX=x;
				if(x>maxX) maxX=x;
				if(y<minY) minY=y;
				if(y>maxY) maxY=y;

				count++;
			}
		}
	}

	//printf("This file has %d non-transparent pixels\n", count);

	if(count)
	{
		pInf[n].minX=minX;
		pInf[n].maxX=maxX+1;
		pInf[n].minY=minY;
		pInf[n].maxY=maxY+1;
	}
	else
	{
		pInf[n].minX=0;
		pInf[n].minY=0;
		pInf[n].maxX=pInf[n].nX;
		pInf[n].maxY=pInf[n].nY;
	}
}

//void STR_SaveStream(FILE *hFile,
//					int xS, int yS, int xC, int yC, int xCX, int yCX, 
//					int depth, UBYTE *pData, UBYTE *pPal, UBYTE *pTrans)

void STR_SaveStream(FILE *hFile, BMPINFO *pBMPInf, UBYTE *pTrans, int gUnComp)
{
	int x,y, off;
	int i;
	int pp;
	ULONG nBits;
	int nSolid;
	ULONG lOut;
	int fRaw;
	int trans;
	int nPadX;

	int xS=pBMPInf->nX;
	int yS=pBMPInf->nY;
	int xC=pBMPInf->minX;
	int yC=pBMPInf->minY;
	int xCX=pBMPInf->maxX;
	int yCX=pBMPInf->maxY;
	int xWnd=xCX-xC;	// real size of image
	int xRight=xC+xWnd; // real right of image to be saved out

	UBYTE *pData=(UBYTE *)pBMPInf->pImg;
	UBYTE *pPal=(UBYTE *)pBMPInf->pPal;
	SLONG transCol=-1;

	for(i=0; i<(1<<(pBMPInf->nBPP)); i++)
	{
		pp=i*3;

		if((pPal[pp+0]==pTrans[0])&&(pPal[pp+1]==pTrans[1])&&(pPal[pp+2]==pTrans[2]))
		{
			transCol=i;
			break;
		}
	}

	// we need to find out whether the sprite is 16 bit aligned.
	//if it's not, then insert some pad bits :(

	nPadX=0;
	if(pBMPInf->nBPP==4)
	{// 4 pixels
		if(xWnd&3) // it isn't 16 bit aligned
			nPadX=4-(xWnd&3);
	}
	else // 2 pixels
	{
		if(xWnd&1) // it isn't 16 bit aligned
			nPadX=1;
	}

	count=0;
	lOut=0;

	// write number of bits (pixels) in bitstream, and therefore the entire image
	//nBits=(xCX-xC)*(yCX-yC);
	nBits=xWnd*(yCX-yC);
	fwrite(&nBits, 1, 4, hFile);

	nSolid=0;
	for(y=yC; y<yCX; y++)
	{
		for(x=xC; x<xCX; x++)
		{
			off=y*xS+x;

			if(pData[off]!=transCol)
				nSolid++;
		}
	}

	if(nSolid==(xS*yS))
		pBMPInf->fColourKey=FALSE;
	else
		pBMPInf->fColourKey=TRUE;

	if(gUnComp)
	{
		fRaw=1;
		pBMPInf->fRaw=1;
	}
	else
	{
		fRaw=0;
		pBMPInf->fRaw=0;
	}

	if((!fRaw) && (nBits>(nSolid+(nBits>>3))))
	{


		//printf("Bitstream\n");
		for(y=yC; y<yCX; y++)
		{
			for(x=xC; x<(xCX+nPadX); x++) //xCX
			{
				off=y*xS+x;

				if(x<xCX)	// if it's >= then it'll always be transparent
				{			// (the important thing is we don't read pData[off].
							// This would be VERY BAD)
					if(pData[off]!=transCol)
						lOut|=(1<<count);
				}

				count++;
				count&=31;
				if(!count)
				{
					fwrite(&lOut, 1, 4, hFile);
					lOut=0;
				}
			}
		}

		if(count)
		{
			lOut=lOut<<(31-count);
			fwrite(&lOut, 1, 4, hFile);
		}

		fwrite(&transCol, 1, 4, hFile);
	}

	count=0;
	lOut=0;

	switch(pBMPInf->nBPP)
	{
	case 8:
		for(y=yC; y<yCX; y++)//go round again this time saving out pixels
		{
			for(x=xC; x<(xCX+nPadX); x++)
			{
				if(x<xCX)
				{
					off=y*xS+x;
					trans=pData[off]!=transCol;
				}
				else
					trans=TRUE; // set trans to true if we're on the pad region

				if(fRaw||trans)
				{
					lOut=lOut>>8;
					lOut|=(pData[off]<<24);

					count++;
					if(!(count&3))
					{
						fwrite(&lOut, 1, 4, hFile);
						lOut=0;
					}
				}
			}
		}
		break;
	case 4:
		for(y=yC; y<yCX; y++)//go round again this time saving out pixels
		{
			for(x=xC; x<(xCX+nPadX); x++)
			{
				if(x<xCX)
				{
					off=y*xS+x;
					trans=pData[off]!=transCol;
				}
				else
					trans=TRUE; // set trans to true if we're on the pad region

				if(fRaw||trans)
				{
					lOut=lOut>>4;
					lOut|=pData[off]<<28;

					count++;
					if(!(count&7))
					{
						fwrite(&lOut, 1, 4, hFile);
						lOut=0;
					}
				}
			}
		}

		if(count&7)
		{	// this bit is in case a final word needs writing
			lOut=lOut>>((8-(count&7))*4);
			fwrite(&lOut, 1, 4, hFile);
		}

		break;
	default:
		dprintf("Invalid colour depth");
		//ERR_TellThemOff(ERR_FATAL,"Invalid colour depth");
	}
}

UBYTE *STR_GetBitmap(NSPRITE *sprInf)
{
	int nPix;
	ULONG *pBits=(ULONG *)sprInf->image;
	ULONG *pData;
	ULONG *pNewPix, *pNP;
	ULONG transCol;
	int i;
	int fRaw;
	ULONG lIn=0;
	ULONG lOut=0;
	ULONG lBits=0;
	ULONG bit,byte;

	int inCount=0;
	int outCount=0;

	nPix=*pBits++;

	if(sprInf->flags&BITSTREAM)
		fRaw=0;
	else
		return (UBYTE *)pBits;

	if(pNewPix=(ULONG *)malloc(nPix), !pNewPix)
	{
		printf("nPix=%d\n", nPix);
		dprintf("Out of memory (GetBitMap)");
		//ERR_TellThemOff(ERR_FATAL, "Out of memory (GetBitMap)");
	}

	pNP=pNewPix;

	// now, pBits points to start of mask
	pData=(ULONG *)((nPix>>3)+(ULONG)pBits);
	transCol=*pData++;
	// now, pData points to start of image :)

	if(sprInf->flags&EIGHTBIT)
	{
		for(i=0; i<nPix; i++)
		{
			if(!(i&31))
			{// get word
				lBits=*pBits++;
			}
			// now we must get bit 0. If it is 1 we've got to copy a pixel across
			//                        if it is 0 we put the trans colour there instead.
			bit=lBits&1;
			lBits=lBits>>1; // thats lBits finished with now.
		
			if(bit) // solid pixel
			{// get byte from pixel list
				if(!(inCount&3))
				{// get next 4 pixel values
					lIn=*pData++;
				}
				byte=lIn&255;
				lIn=lIn>>8;
				// only increment inCount if solid pixel
				inCount++;
			}
			else
				byte=transCol;

			// now store the byte in lOut
			lOut=lOut>>8;
			lOut|=(byte<<24);

			if((i&3)==3)
				*(pNewPix++)=lOut;
		}
	}
	else
	{
		for(i=0; i<nPix; i++)
		{
			if(!(i&31))
			{// get word
				lBits=*pBits++;
			}
			// now we must get bit 0. If it is 1 we've got to copy a pixel across
			//                        if it is 0 we put the trans colour there instead.
			bit=lBits&1;
			lBits=lBits>>1; // thats lBits finished with now.
		
			if(bit) // solid pixel
			{// get byte from pixel list
				if(!(inCount&7))
				{// get next 8 pixel values
					lIn=*pData++;
				}
				byte=lIn&15;
				lIn=lIn>>4;
				// only increment inCount if solid pixel
				inCount++;
			}
			else
				byte=transCol;

			// now store the byte in lOut
			lOut=lOut>>4;
			lOut|=(byte<<28);

			if((i&7)==7)
				*(pNewPix++)=lOut;
		}
	}

	return (UBYTE *)pNP;
}

UBYTE *STR_GetPalette(NSPRITE *sprInf)
{
	// this is the easy one
	int i;
	USHORT *pPal;
	int palEntry;
	UBYTE *pNewPal;
	int pp;
	int nCol=(sprInf->flags&EIGHTBIT)?256:16;

	if(pNewPal=(UBYTE *)malloc(3*nCol), !pNewPal)
		dprintf("Out of memory (GetPalette)");
		//ERR_TellThemOff(ERR_FATAL, "Out of memory (GetPalette)");

	//printf("Loading %d colours\n", nCol);
	pPal=sprInf->pal;
	for(i=0; i<nCol; i++)
	{
		pp=i*3;
		palEntry=pPal[i];
		pNewPal[pp+2]=((palEntry&31)<<3)|((palEntry&31)>>2);
		palEntry=palEntry>>5;
		pNewPal[pp+1]=((palEntry&31)<<3)|((palEntry&31)>>2);
		palEntry=palEntry>>5;
		pNewPal[pp]  =((palEntry&31)<<3)|((palEntry&31)>>2);
	}

	return pNewPal;
}

int STR_SetSPRPointers(NSPRITE *pHeader)
{
	int n=0;

	while(1)
	{
		pHeader[n].pal=  (USHORT *)((ULONG)pHeader+((ULONG)pHeader[n].pal));
		pHeader[n].image=(UBYTE *)((ULONG)pHeader+((ULONG)pHeader[n].image));

		if(pHeader[n].flags&LASTSPR)
			return(++n);
		n++;
	}
}

void STR_GetBMPInfo(BMPINFO *bmpInf, NSPRITE *pHeader, int n)
{
	bmpInf->pPal=pHeader[n].pal;
	bmpInf->pImg=pHeader[n].image;
	bmpInf->nX=pHeader[n].w;
	bmpInf->nY=pHeader[n].h;
	bmpInf->nBPP=(pHeader[n].flags&EIGHTBIT)?8:4;

	if(!bmpInf->nX)
		bmpInf->nX=256;
	if(!bmpInf->nY)
		bmpInf->nY=256;
}
