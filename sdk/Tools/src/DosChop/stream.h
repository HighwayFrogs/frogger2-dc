#define CRC
#define MAX_BMPS 32

//typedef struct tag

typedef struct tagNSPRITE{
	UBYTE *image; //
	USHORT *pal;	  // remember pointers are always 4 bytes
	UBYTE w; 
	UBYTE h;   
	SSHORT u;    
	SSHORT v;     
	USHORT flags;
#ifdef CRC
	ULONG crc;
#endif
}NSPRITE;

#define LASTSPR		1 
#define EIGHTBIT	2
#define SPLIT		4
#define BITSTREAM	8
#define ALPHA		16
#define COLOURKEY	32
// Because ISL's new libs ditch the u & v information (tsk!) we need to keep track of splits horizonally and
// vertically in the flags, rather than using the u & v entries...
#define VSPLIT		64

#define ERR_OK 0
#define ERR_BADBITS 1

typedef struct tagBMPINFO{
	char sFileName[256];
	void *pPal;
	void *pImg;
	ULONG offPal;
	ULONG offImg;
	int nX, nY;
	int minX, minY;
	int maxX, maxY;
	int nPix;
	int nBPP;
	int fRaw;
	int fSplit;
	int fvSplit;
	int fColourKey;
	int nTransPix;
	USHORT flags;
}BMPINFO;

typedef struct tagBMPLIST
{
	struct tagBMPLIST *pNext;
	struct tagBMPLIST *pLast;
	// main data for list
	BMPINFO bmp;
#ifndef DOSVER
	void *pChildWin;
	HWND hWnd;
#endif
}BMPLIST;

typedef struct tagSAVEDATA
{
	char sFileName[256];
	BMPLIST *pBMPList;
	int gSaveTxt;
	int gCropTrans;
	int gSaveRaw;
	int gFTPink;
	int gDontQuit;
	int gReduceTo;
	int nXBytes;
	int nYBytes;
	int nMipMaps;
	char sAlpha[8];
	UBYTE pTrans[4]; // last one is pad
}SAVEDATA;

extern int gQuiet;
extern int gUncompressed;
extern int gCrop;
extern int gDecomp;
extern int gRemap;
extern int gSplit;
extern int gRefCentre;

extern int nBMPs;
extern BMPLIST *pRoot;
extern UBYTE pLPal[];
extern UBYTE trans[];

char *GetFileNameWithExt(char *);
void Align(FILE *);
UBYTE Str2Hex(char *, int);
void STR_LoadPalette(char *);
void STR_MapToPalette(BMPINFO *, UBYTE *);
BMPLIST *STR_AddNode(char *sFN);
int STR_DeleteNode(BMPLIST *p);
int STR_SaveSPT(SAVEDATA *);
void STR_SavePalette(FILE *, int, UBYTE *);
void STR_SaveStream(FILE *, BMPINFO *, UBYTE *, int);
void STR_GetBitmapLimits(BMPINFO *, int, UBYTE *);
int STR_LoadNewBMP(BMPINFO *);
int STR_SetSPRPointers(NSPRITE *);
void STR_GetBMPInfo(BMPINFO *bmpInf, NSPRITE *pHeader, int n);
UBYTE *STR_GetBitmap(NSPRITE *);
UBYTE *STR_GetPalette(NSPRITE *);
int STR_Reduce24Bit(UBYTE **ppImg, UBYTE **ppPal, int x, int y);

