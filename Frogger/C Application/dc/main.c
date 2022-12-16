/*
	This file is part of Frogger II, Copyright 2000 Interactive Studios Ltd

	File        : shell.c
	Description : Main core routine and system initialisation
	Date        : 09/03/2000
	Version     : 0.01
*/

#include "include.h"		// so we can use mw_pr() which outputs to the debug window
#include "frogger.h"
#include "sg_Chain.h"
#include "generic.h"
#include <shinobi.h>
#include <kamui2.h>
#include <sg_sd.h>
#include <sg_mw.h>
#include "dc_timer.h"
#include "lang.h"
#include "psxsprite.h"
#include "tongue.h"
#include "menus.h"
#include "audio.h"

#include <ac.h>
#include <a64thunk.h>
#include <am.h>

#include "am_audio.h"
#include "audio.h"
#include "bpamsetup.h"
#include "bpamstream.h"
#include "bpbuttons.h"
#include "bpprintf.h"
#include "bpamsfx.h"
#include "strsfx.h"
#include "islvideo.h"
#include "options.h"
#include "CR_lang.h"
#include "story.h"
#include "memcard.h"
#include "fadeout.h"
#include "savegame.h"
#include "DCK_system.h"
#include "islxa.h"
#include "as.h"
#include "QueueSfx.h"
#include "frontend.h"

// -------
// Globals

extern CurrentData	current[MAX_CHANNELS];


extern XAFileType	*curXA;

KMPACKEDARGB 	borderColour;
KMDWORD 		FBarea[24576 + 19456];
PDS_PERIPHERAL 	*per;

KMSTRIPCONTEXT 	stripContext00;
KMSTRIPHEAD 	stripHead00;
KMSTRIPCONTEXT 	StripContext;
KMSTRIPHEAD 	StripHead;
KMVERTEX_00		vertices[8];

DCKVERTEX 		*globalVerts;

KMSURFACEDESC	TexList[256];

int				matrixSize = 0;
DCKMATRIX		matrixStack[256];
DCKMATRIX		viewMatrix;

KMPACKEDARGB	colourClampMin,colourClampMax;

displayPageType displayPage[2];
displayPageType *currentDisplayPage  = 0;

psFont 			*font;
psFont 			*fontSmall;

int 			drawLandscape = 1;
long 			textEntry = 0;	
char 			textString[255] = "";

long 			drawGame = 1;

INT				textPosX = 10;
INT				textPosY = 10;

GsRVIEW2		camera;

int				polyCount;
int				mapCount;

TEXTOVERLAY 	*startText;
PSIOBJECT 		*world;
ACTOR2 			*me = NULL;
ACTOR2 			*me2 = NULL;
char			textbuffer[256];
float			myframe = 0;
DCKVECTOR		camPos = {0.0f,0.0f,-50.0f};
DCKVECTOR		camTarg = {0.0f,0.0f,0.0f};
float			viewAngle = 3.34;
float			viewDist = 1000;

extern int		frogFacing[4];
extern int		camFacing;
extern struct 	gameStateStruct gameState;

unsigned long 	*sqrtable;
short			*acostable;

int 			vsyncCounter = 0;
int 			lastframe = 0;
int 			myVsyncCounter = 0;

long 			turbo = 4096;

unsigned short 	globalClut;
short			actorShiftDepth;
short			timerBars = OFF;

AC_ERROR_PTR	acErr;
AM_ERROR 		*amErr;
	
KMDWORD			FogDensity;

#ifdef FINAL_MASTER
int useMemCard = 1;
#else
int useMemCard = 1;
#endif

SAVE_INFO saveInfo;

int showQFXError = 0;

// chained vblank handler
SYCHAIN vblChain;

// used to abort the game
unsigned int globalAbortFlag = 0;

// *ASL* 14/08/2000 - sprite counts
int spriteRotNum;
int spriteNum;

int	startAudio = FALSE;

// ----------
// Prototypes

// vblank callback
static void vblankCallback(void *arg);

// gsFs error function callback
static void gdFsErrorCallback(void *obj, Sint32 err);

// show all legal screens and FMV
void showLegalFMV(int allowQuit);

// these should go into islxa.h
extern void XAinit();
extern void XAShutdown();

int	skipSpecFX = FALSE;

/* ---------------------------------------------------------
   Function : Kamui_Init
   Purpose : initialise kamui
   Parameters : 
   Returns : 
   Info : 
*/

void Kamui_Init()
{
	int	i;
	
	// create the frame buffers
	pFB[0] = &primarySurfaceDesc;
	pFB[1] = &renderSurfaceDesc;

    // vertex buffer pointer
    dwDataPtr = (PKMDWORD)(((KMDWORD)syMalloc(0x200000) & 0x0FFFFFFFUL) | 0xA0000000);

	// set the system configuration
	kmSystemConfig.dwSize						= sizeof(KMSYSTEMCONFIGSTRUCT);
	kmSystemConfig.flags						= KM_CONFIGFLAG_ENABLE_CLEAR_FRAMEBUFFER;
	// for frame buffer
	kmSystemConfig.ppSurfaceDescArray			= pFB;
	kmSystemConfig.fb.nNumOfFrameBuffer			= 2;
	// for texture memory
    kmSystemConfig.nTextureMemorySize       	= 0x200000 * 2;		// 4 MB for textures
    kmSystemConfig.nNumOfTextureStruct        	= 4096;
    kmSystemConfig.nNumOfSmallVQStruct        	= 0;//1024;
    kmSystemConfig.pTextureWork               	= &FBarea[0];
    // for Vertex buffer
    kmSystemConfig.pBufferDesc                	= &vertexBufferDesc;
    kmSystemConfig.nNumOfVertexBank           	= 2;
    kmSystemConfig.pVertexBuffer              	= dwDataPtr;		// pointer to vertex buffer
    kmSystemConfig.nVertexBufferSize          	= 0x100000 * 2;		// 2MB for vertex buffer
    kmSystemConfig.nPassDepth                 	= 1;
    kmSystemConfig.Pass[0].dwRegionArrayFlag  	= KM_PASSINFO_AUTOSORT;
    kmSystemConfig.Pass[0].fBufferSize[0]     	= 50.0f;
    kmSystemConfig.Pass[0].fBufferSize[1]     	= 0.0f;
    kmSystemConfig.Pass[0].fBufferSize[2]     	= 50.0f;
    kmSystemConfig.Pass[0].fBufferSize[3]     	= 0.0f;
    kmSystemConfig.Pass[0].fBufferSize[4]     	= 0.0f;

	kmSetSystemConfiguration(&kmSystemConfig);

    // initialise border colour
    borderColour.dwPacked = RGBA(0,0,0,255);	// black border
    kmSetBorderColor(borderColour);
}

void SetCam(VECTOR src, VECTOR tar)
{
	camera.vpx = src.vx;
	camera.vpy = src.vy;
	camera.vpz = src.vz;

	camera.vrx = tar.vx;
	camera.vry = tar.vy;
	camera.vrz = tar.vz;

	camera.rz  =  0;

	GsSetRefView2(&camera);

	gte_SetRotMatrix(&GsWSMATRIX);
	gte_SetTransMatrix(&GsWSMATRIX);
}

void InitCam(void)
{
	// Set up an ambient light
	GsSetAmbient(128*16, 128*16, 128*16);
 	GsSetLightMode(2);

	// Set up a simple camera
	GsSetProjection(475);			// default distance of projection; alters perspective
	SetGeomOffset(640/2,480/2);		// camera focused on middle of screen
//	SetGeomOffset(0,0);				// camera focused on middle of screen

 	bbsrc.vx = 500;
	bbsrc.vy = -350;
 	bbsrc.vz = 11000;
   
	bbtar.vx = bbsrc.vx + (sinf(viewAngle) * viewDist);
	bbtar.vy = bbsrc.vy;// + 200;
	bbtar.vz = bbsrc.vz + (cosf(viewAngle) * viewDist);	

/* 	bbsrc.vx = 0;
 	bbsrc.vy = 00;
 	bbsrc.vz = -300;
  
 	bbtar.vx = 0;
 	bbtar.vy = 0;
 	bbtar.vz = 1300;
*/
	SetCam(bbsrc,bbtar);
}

static void EORCallBack(void *arg)
{
	if ( loadingDisplay )
	{
		loadingDisplayFrame();
	}
}

/*	--------------------------------------------------------------------------------
	Function 	: 
	Purpose 	: 
	Parameters 	: 
	Returns 	: 
	Info 		:
*/

void InitLookupTables()
{
	int			fileLength;

	sqrtable = (unsigned long *)fileLoad("incbin\\Sqrtable.bin",&fileLength);
	acostable = (short *)fileLoad("incbin\\acostab.bin",&fileLength);
}

/*	--------------------------------------------------------------------------------
	Function 	: main
	Purpose 	: main code loop
	Parameters 	: none
	Returns 	: none
	Info 		:
*/

void BuildFogTable (void)
{
	int				i;
	float			depth;
	KMPACKEDARGB	fogcol;
	KMFLOAT			FogTable[128];
	KMSTATUS		status;
	float			min,max,density;

	// Fog Color
	fogcol.byte.bBlue	= 0;//0.5 * 255;
	fogcol.byte.bGreen	= 0;//0.5 * 255;
	fogcol.byte.bRed	= 0.5 * 255;
	status = kmSetFogTableColor(fogcol);

/*ma
	fog.min = (1100)<<2;
	fog.min = (1100+500)<<2;

	min = 1.0/(float)fog.min;
	max = 1.0/(float)fog.max;
	kmGenerateFogTable(FogTable,min,max,0.1,&density,KM_FOGTYPE_LINEAR);

	kmConvertFogDensity(density,&FogDensity);
	kmSetFogDensity(FogDensity);
	kmSetFogTable(&FogTable[0]);
*/
	// Fog Density
	FogDensity = 0xFF0B;		// FF09 - FF0D
	status = kmSetFogDensity(FogDensity);

	// Fog Table
	for(i = 0; i < 128; i++)
	{
#define	FOG_MIN		(-0.8f)
		FogTable[i] = 1.0f - (float)i * ((1.0f - FOG_MIN) / 128.0f);
		if(FogTable[i] < 0.0f)
			FogTable[i] = 0.0f;
	}
	status = kmSetFogTable(&FogTable[0]);
}

extern int frameCount;
extern int AllowPsiDestroy;
extern int psiActorCount;
extern int fmaActorCount;
extern int numTextureBanks;

char texurestring[256];
char texurestring2[256];


extern KTU32 *gTransferBuffer;
extern KTU32 *gPlayBuffer;
extern KTU32 *gHeaderBuffer;
extern KTU32 gTransferBufferSize;

int	texViewer = 0;

short videoKeyHandler()
{
	return 0;
}

extern StrDataType 	vStream;
extern CurrentData	current[MAX_CHANNELS];

int checkForValidControllers();

void updatePads()
{
	int	i,padCounter;

	padCounter = 0;
	for(i=firstPad;i<(firstPad+numPads);i++)
	{
		switch(i)
		{
			case 0:	
				per = pdGetPeripheral(PDD_PORT_A0);
				break;
			case 1:	
				per = pdGetPeripheral(PDD_PORT_B0);
				break;
			case 2:	
				per = pdGetPeripheral(PDD_PORT_C0);
				break;
			case 3:	
				per = pdGetPeripheral(PDD_PORT_D0);
				break;
		}

		// temp pad emulation
		padData.debounce[padCounter] = 0;
		if(per->press & PDD_DGT_KU)
			padData.debounce[padCounter] |= PAD_UP;
		if(per->press & PDD_DGT_KD)
			padData.debounce[padCounter] |= PAD_DOWN;
		if(per->press & PDD_DGT_KR)	
			padData.debounce[padCounter] |= PAD_RIGHT;
		if(per->press & PDD_DGT_KL)
			padData.debounce[padCounter] |= PAD_LEFT;

		if(per->press & PDD_DGT_TB)
			padData.debounce[padCounter] |= PAD_TRIANGLE;
		if(per->press & PDD_DGT_TA)
			padData.debounce[padCounter] |= PAD_CROSS;
		if(per->press & PDD_DGT_TX)
			padData.debounce[padCounter] |= PAD_SQUARE;
		if(per->press & PDD_DGT_TY)
			padData.debounce[padCounter] |= PAD_CIRCLE;
		if(per->press & PDD_DGT_ST)
			padData.debounce[padCounter] |= PAD_START;

		padCounter++;
	}

	pdExecPeripheralServer();
}



/* ---------------------------------------------------------
   Function : startButtonPressed
   Purpose : has user pressed start button?
   Parameters : 
   Returns : 1 for start button pressed, else 0
   Info : 
*/

int startButtonPressed()
{
	int	i, ret;

	ret = 0;
	for (i=0; i<4; i++)
	{
		switch (i)
		{
			case 0:	
				per = pdGetPeripheral(PDD_PORT_A0);
				break;
			case 1:	
				per = pdGetPeripheral(PDD_PORT_B0);
				break;
			case 2:	
				per = pdGetPeripheral(PDD_PORT_C0);
				break;
			case 3:	
				per = pdGetPeripheral(PDD_PORT_D0);
				break;
		}
		if (per->on & PDD_DGT_ST)
		{
			ret = 1;
			break;
		}
	}
	pdExecPeripheralServer();
	return ret;
}

int numSfxPlaying()
{
	int channel,num = 0;

	for(channel=0; channel<MAX_CHANNELS; channel++)
	{
		if(amSoundIsPlaying(&current[channel].sound))
			num++;

//		if(current[channel].sound.isPlaying)
//			num++;
	}

	return num;
}

void resetToBootROM();
int quitNow = FALSE;

int		hzSelect = 0;
int		hzQuit = 0;
int		hzMode = 0;
UCHAR	hzR,hzG,hzB;
char	hzMessage[32];

KTU32 digiNum,midiNum;

#define	MW_MAX_CH		(8)
#define	MW_SNDRAM_SIZE	(0x4040*MW_MAX_CH)

void main()
{
	int					i,j,counter,padCounter;
	MWS_PLY_INIT_SFD	iprm;
	Uint32 				*memfree,*memsize;
	unsigned long		hyp,test;
	int					numUsed,numPadsDetected = 0;
	Sint32				langNum,soundMode;
	void				*pbuf;
	KTBOOL				ret;
	AM_SOUND			amsnd[8];
	KTU32				port[8];
	volatile KTU32		*sndram = KTNULL;
	Sint32				dstat;
	char				*cptr;
	
	#ifdef __GNUC__
	shinobi_workaround();
	#endif

	//	Initialize system for middleware(call before sbInitSystem)
	mwPlyPreInitSofdec();
		
	// initialise video mode
	switch(syCblCheck())
	{
		case SYE_CBL_VGA:
			sbInitSystem((int)KM_DSPMODE_VGA,(int)KM_DSPBPP_RGB565,1);
			displayMode = KM_DSPMODE_VGA;		
			break;
			
		case SYE_CBL_PAL:
			sbInitSystem((int)KM_DSPMODE_PALNI640x480EXT,(int)KM_DSPBPP_RGB565,1);
			displayMode = KM_DSPMODE_PALNI640x480EXT;		
			break;
			
		case SYE_CBL_NTSC:
		default:
			sbInitSystem((int)KM_DSPMODE_NTSCNI640x480,(int)KM_DSPBPP_RGB565,1);
			displayMode = KM_DSPMODE_NTSCNI640x480;		
			break;
	}
	frameBufferFormat = KM_DSPBPP_RGB888;

#ifndef NTSC_VERSION
	// set our gdFs error callback
	gdFsEntryErrFuncAll(gdFsErrorCallback, (void *)0);
#endif
	
	// needs to be called before calling ADXT_Init
	XAinit();
	ADXT_Init();

    // check malloc alignment
    Init32Malloc();

	Kamui_Init();

    // initialise our CRC table
	InitCRCTable();	

	// Open the AM system	
	if(!bpAmSetup(AC_DRIVER_DA, KTFALSE, KTNULL))
	{
		acASEBRK(KTTRUE);
	}

	// As above in the AC layer, the Middleware library requires a block of sound RAM and
	// ports.  However, some AM level commands are used to accomplish this.
	for (i=0; i<MW_MAX_CH; i++)
	{
		ret = amSoundAllocateVoiceChannel(&amsnd[i]);
		if (ret == KTFALSE)
		{
			for (;;);
		}
		port[i] = amsnd[i].voiceChannel;
	}
	ret = amHeapAlloc(&sndram, MW_SNDRAM_SIZE, 32, AM_FIXED_MEMORY, NULL);
	if (ret == KTFALSE)
	{
		for (;;);
	}
	mwSetupAudio64(sndram, MW_SNDRAM_SIZE, port, MW_MAX_CH);

	(void)acSetTransferMode(AC_TRANSFER_DMA);
		
	acSystemDelay(500000);

	acErr = acErrorGetLast();
	amErr = amErrorGetLast();
		
	// Setup the file system
	a64FileInit();

	InitLookupTables();
	utilInitCRC();
	utilSeedRandomInt(398623);
	InitCam();
	actorInitialise();

	// adjust best times
	for(i = 0;i < NUM_WORLDS;i++)
	{
		for(j = 0;j < origWorldVisualData[i].numLevels;j++)
		{
			origWorldVisualData[i].levelVisualData[j].parTime += (int)((float)origWorldVisualData[i].levelVisualData[j].parTime*0.10);
		}
	}

	// load the two fonts
	font = fontLoad("font.pvr");
	fontSmall = fontLoad("smallfont.pvr");

	InitProgressBar();
	InitTimerbar();
	
	initialisePsxStrips();

	numTextureBanks = 0;

	// get boot rom settings
	pbuf = (void*)syMalloc(16*1024);
	syCfgInit(pbuf);

	// get language
	syCfgGetLanguage(&langNum);
	switch(langNum)
	{
		case SYD_CFG_JAPANESE:
			gameTextLang = LANG_UK;
			break;
		case SYD_CFG_ENGLISH:
			gameTextLang = LANG_UK;
			break;
		case SYD_CFG_GERMAN:
			gameTextLang = LANG_D;
			break;
		case SYD_CFG_FRENCH:
			gameTextLang = LANG_F;
			break;
		case SYD_CFG_SPANISH:
			gameTextLang = LANG_UK;
			break;
		case SYD_CFG_ITALIAN:
			gameTextLang = LANG_IT;
			break;
		default:
			gameTextLang = LANG_UK;
			break;
	}

#ifdef	NTSC_VERSION
	// ma - temp fix for us version
	gameTextLang = LANG_US;
#endif

	// get sound mode
	syCfgGetSoundMode(&soundMode);
	if(soundMode == SYD_CFG_STEREO)
	{
		options.stereo = TRUE;
		acSystemDelay(500000);
		acSystemSetStereoOrMono(0);
		acSystemDelay(500000);
	}
	else
	{
		options.stereo = FALSE;
		acSystemDelay(500000);
		acSystemSetStereoOrMono(1);
		acSystemDelay(500000);
	}

	syCfgExit();
	syFree(pbuf);

	qfxInit();

	// *ASL* 09/09/2000

	// ** This flag needs to be cleared before either the vblank or the
	// ** GD error callbacks are set
	globalAbortFlag = 0;

	// setup our vblank callback
	vblChain = syChainAddHandler(SYD_CHAIN_EVENT_IML6_VBLANK, vblankCallback, 0x60, NULL);
	
#ifdef NTSC_VERSION
	// set our gdFs error callback
	gdFsEntryErrFuncAll(gdFsErrorCallback, (void *)0);
#endif

	// *ASL* 12/08/2000 - Init soft reset
	initCheckForSoftReset();

	// *ASL* 14/08/2000 - *Make sure these are cleared before initial call to showLegalFMV*
	backDrop.init = FALSE;
	backDrop.draw = FALSE;

	// show all legal screens and FMV
	showLegalFMV(0);

	// default world data.. needs to be called before CommonInit
	memcpy((char*)worldVisualData,(char*)origWorldVisualData,sizeof(worldVisualData));

	CommonInit();

	// don't continue until at least 1 valid pad detected
	numPadsDetected = checkForValidControllers();
	if(numPadsDetected == 0)
	{
		InitTiledBackdrop("LOGO");
		ScreenFade(0,255,20);

		while(!numPadsDetected)
		{
			kmBeginScene(&kmSystemConfig);
			kmBeginPass(&vertexBufferDesc);

#ifdef NTSC_VERSION
			FontInSpace((320-fontExtentWScaled(fontSmall, GAMESTRING(STR_DC_START_PAD_NOPAD),4096)/2),240, GAMESTRING(STR_DC_START_PAD_NOPAD), 380,2, 255,255,255);
			//FontInSpace(0,240, GAMESTRING(STR_DC_START_PAD_NOPAD), 380,2, 255,255,255, 4096, fontSmall, TRUE);
#else
			FontInSpaceScaled(0,240, GAMESTRING(STR_DC_START_PAD_NOPAD), 380,2, 255,255,255, 4096, fontSmall, TRUE);
#endif

			utilPrintf("header: %d\n",sizeof(SAVEGAME_HEADER));
			utilPrintf("level: %d\n",sizeof(SAVEGAME_LEVELINFO));
			utilPrintf("level * numLevels: %d\n",sizeof(SAVEGAME_LEVELINFO)*SAVEGAME_LEVELS);
			utilPrintf("total: %d\n",(SAVEGAME_HEADERSIZE+sizeof(SAVEGAME_LEVELINFO)*SAVEGAME_LEVELS));
			utilPrintf("psxheader: %d\n",PSXCARDHEADER_SIZE);
			utilPrintf("\n");

			DrawTiledBackdrop(NO);
			DrawScreenTransition();
			actFrameCount++;

			kmEndPass(&vertexBufferDesc);
					
			kmRender(KM_RENDER_FLIP);
			kmEndScene(&kmSystemConfig);

#ifndef NTSC_VERSION
			// check the drive status
			dstat = gdFsGetDrvStat();
			if(dstat == GDD_DRVSTAT_OPEN)
			{
				resetToBootROM();
			}
			gdFsReqDrvStat();
#endif

			if(globalAbortFlag == 1)
				resetToBootROM();

			numPadsDetected = checkForValidControllers();
		}
#ifndef NTSC_VERSION
		acSystemDelay(5000000);
#endif
	}

#ifndef	NTSC_VERSION
	if(displayMode == KM_DSPMODE_PALNI640x480EXT)
	{
		// determine whether to use 50/60 Hz for the PAL version
		InitArtBackdrop("ARTWORK13",TRUE);
		InitTiledBackdrop("LOGO");
		ScreenFade(0,255,20);

		while((!hzQuit) || fadingOut)
		{
			updatePads();

			kmBeginScene(&kmSystemConfig);
			kmBeginPass(&vertexBufferDesc);

			switch(hzMode)
			{
				case 0:
					FontInSpaceScaled((320-fontExtentWScaled(font, GAMESTRING(STR_DC_TV_SETTING),4096)/2),40, GAMESTRING(STR_DC_TV_SETTING), 380,2, 255,255,255, 4096, font, FALSE);

					hzR = 127+((rsin(actFrameCount*4000)+4096)*64)/4096;
					hzG = 127+((rcos(actFrameCount*4000)+4096)*64)/4096;
					hzB = 255;

					if(hzSelect == 0)
						FontInSpaceScaled((160-fontExtentWScaled(font, GAMESTRING(STR_DC_TV_50HZ_SELECT),4096)/2),140, GAMESTRING(STR_DC_TV_50HZ_SELECT), 380,2, hzR,hzG,255, 4096, font, FALSE);
					else
						FontInSpaceScaled((160-fontExtentWScaled(fontSmall, GAMESTRING(STR_DC_TV_50HZ_SELECT),4096)/2),140, GAMESTRING(STR_DC_TV_50HZ_SELECT), 380,2, 255,255,255, 4096, fontSmall, FALSE);

					if(hzSelect == 1)
						FontInSpaceScaled((320-fontExtentWScaled(font, GAMESTRING(STR_DC_TV_60HZ_SELECT),4096)/2),140, GAMESTRING(STR_DC_TV_60HZ_SELECT), 380,2, hzR,hzG,255, 4096, font, FALSE);
					else
						FontInSpaceScaled((320-fontExtentWScaled(fontSmall, GAMESTRING(STR_DC_TV_60HZ_SELECT),4096)/2),140, GAMESTRING(STR_DC_TV_60HZ_SELECT), 380,2, 255,255,255, 4096, fontSmall, FALSE);

					if(hzSelect == 2)
						FontInSpaceScaled((480-fontExtentWScaled(font, GAMESTRING(STR_DC_TV_TEST_SELECT),4096)/2),140, GAMESTRING(STR_DC_TV_TEST_SELECT), 380,2, hzR,hzG,255, 4096, font, FALSE);
					else
						FontInSpaceScaled((480-fontExtentWScaled(fontSmall, GAMESTRING(STR_DC_TV_TEST_SELECT),4096)/2),140, GAMESTRING(STR_DC_TV_TEST_SELECT), 380,2, 255,255,255, 4096, fontSmall, FALSE);

					switch(hzSelect)
					{
						case 0:
							FontInSpaceScaled(0,240, GAMESTRING(STR_DC_TV_50HZ), 380,8, 255,255,255, 4096, fontSmall, TRUE);
							break;
						case 1:
							FontInSpaceScaled(0,240, GAMESTRING(STR_DC_TV_60HZ), 380,8, 255,255,255, 4096, fontSmall, TRUE);
							break;
						case 2:
							FontInSpaceScaled(0,240, GAMESTRING(STR_DC_TV_TEST), 380,8, 255,255,255, 4096, fontSmall, TRUE);
							break;
					}

					FontInSpaceScaled((320-fontExtentWScaled(fontSmall, GAMESTRING(STR_X_SELECT_OPTION),4096)/2),380, GAMESTRING(STR_X_SELECT_OPTION), 380,8, 255,255,255, 4096, fontSmall, FALSE);

					if(padData.debounce[firstPad] & PAD_RIGHT)
					{
						hzSelect++;
						if(hzSelect > 2)
							hzSelect = 2;
					}
					if(padData.debounce[firstPad] & PAD_LEFT)
					{
						hzSelect--;
						if(hzSelect < 0)
							hzSelect = 0;
					}

					if(padData.debounce[firstPad] & PAD_CROSS)
					{
						switch(hzSelect)
						{
							// 50 Hz mode
							case 0:
								ScreenFade(255,0,20);
								hzQuit = TRUE;
								break;

							// 60 Hz mode
							case 1:
								ScreenFade(255,0,20);
								hzQuit = TRUE;
								kmSetDisplayMode (KM_DSPMODE_NTSCNI640x480, KM_DSPBPP_RGB565, TRUE, FALSE);
								break;

							// Test 60 Hz mode
							case 2:
								hzMode = 1;
								actFrameCount = 0;
								kmSetDisplayMode (KM_DSPMODE_NTSCNI640x480, KM_DSPBPP_RGB565, TRUE, FALSE);
								break;
						}
					}
					DrawTiledBackdrop(NO);
					break;

				case 1:
					DrawArtBackDrop(0, 0);
					sprintf(hzMessage,"%d",5-(actFrameCount/60));
					FontInSpaceScaled((320-fontExtentWScaled(font, hzMessage,4096)/2),80,hzMessage , 380,8, 255,255,255, 4096, font, FALSE);

					if(actFrameCount > (60*5))
					{
						hzMode = 0;
						kmSetDisplayMode (KM_DSPMODE_PALNI640x480EXT, KM_DSPBPP_RGB565, TRUE, FALSE);
						displayMode = KM_DSPMODE_PALNI640x480EXT;		

						acSystemDelay(500000);
					}
					break;
			}

			DrawScreenTransition();
			actFrameCount++;

			kmEndPass(&vertexBufferDesc);

			kmRender(KM_RENDER_FLIP);
			kmEndScene(&kmSystemConfig);

			// check for soft reset
			if(checkForSoftReset())
			{
				resetToBootROM();
			}

			// check the drive status
			gdFsReqDrvStat();
			dstat = gdFsGetDrvStat();
			if(dstat == GDD_DRVSTAT_OPEN)
			{
				resetToBootROM();
			}

			if(globalAbortFlag == 1)
				resetToBootROM();
		}
		FreeArtBackdrop();
		acSystemDelay(50000000);
	}
#endif

	// initialise memory card
	cardInitialise();

	// ????
	LoadGame();

	// load game from memory card
	actFrameCount = 0;
	if(saveInfo.saveFrame)
	{
		InitTiledBackdrop("LOGO");
		ScreenFade(0,255,20);
		keepFade = NO;

		while(((saveInfo.saveFrame) || (fadingOut)) && (!quitNow))
		{
			updatePads();

			kmBeginScene(&kmSystemConfig);
			kmBeginPass(&vertexBufferDesc);

			DrawTiledBackdrop(NO);
			DrawScreenTransition();
			actFrameCount++;

			if(actFrameCount > 20)
				ChooseLoadSave();
				
			if((saveInfo.saveFrame == 0) && (keepFade == 0))
			{
				ScreenFade(255,0,20);
				keepFade = YES;
			}
#ifdef NTSC_VERSION
			// TODO: Maybe move the quit now flag checker here..
#endif

			kmEndPass(&vertexBufferDesc);
					
			kmRender(KM_RENDER_FLIP);
			kmEndScene(&kmSystemConfig);

			if(saveInfo.saveStage != SAVEMENU_SAVE)
			{
#ifndef NTSC_VERSION
				// check for soft reset
				if(checkForSoftReset())
				{
					resetToBootROM();
				}

				// check the drive status
				dstat = gdFsGetDrvStat();
				if(dstat == GDD_DRVSTAT_OPEN)
				{
					resetToBootROM();
				}
				gdFsReqDrvStat();
#endif
				if(globalAbortFlag == 1)
					quitNow = TRUE;
				
			}
		}
		afterSaveFlag = 0;
		FreeTiledBackdrop();
		ShowLCDLogo();
	}
#ifndef NTSC_VERSION
	acSystemDelay(50000000);
#endif

	if(quitNow)
		resetToBootROM();

#ifdef _DEBUG
	utilPrintf("DEBUG ACTIVE!!!!!\n");
#endif

	// ** Main loop

	startAudio = TRUE;
	PrepareSong((short)WORLDID_FRONTEND, 1);		// loop track

	// *ASL* 08/08/2000 - Until user quits or opens the lid
	while (globalAbortFlag == 0)
	{
		DCTIMER_START(0);

		polyCount = 0;
		actorCount = 0;		
		mapCount = 0;
		psiActorCount = 0;
		fmaActorCount = 0;
		resetToFrontend = FALSE;

		// *ASL* 14/08/2000 - sprite counts
		spriteRotNum = 0;
		spriteNum = 0;

		padCounter = 0;
		i=0;
		while((padCounter < numPads)&&(i < 4))
		{
			switch(i)
			{
				case 0:	
					per = pdGetPeripheral(PDD_PORT_A0);
					break;
				case 1:	
					per = pdGetPeripheral(PDD_PORT_B0);
					break;
				case 2:	
					per = pdGetPeripheral(PDD_PORT_C0);
					break;
				case 3:	
					per = pdGetPeripheral(PDD_PORT_D0);
					break;
			}

			if(strstr(per->info->product_name,"(none)"))
			{
				i++;
				continue;
			}

			// temp pad emulation
			padData.debounce[padCounter] = 0;
			if(per->press & PDD_DGT_KU)
				padData.debounce[padCounter] |= PAD_UP;
			if(per->press & PDD_DGT_KD)
				padData.debounce[padCounter] |= PAD_DOWN;
			if(per->press & PDD_DGT_KR)	
				padData.debounce[padCounter] |= PAD_RIGHT;
			if(per->press & PDD_DGT_KL)
				padData.debounce[padCounter] |= PAD_LEFT;

			if(per->press & PDD_DGT_TB)
				padData.debounce[padCounter] |= PAD_TRIANGLE;
			if(per->press & PDD_DGT_TA)
				padData.debounce[padCounter] |= PAD_CROSS;
			if(per->press & PDD_DGT_TX)
				padData.debounce[padCounter] |= PAD_SQUARE;
			if(per->press & PDD_DGT_TY)
				padData.debounce[padCounter] |= PAD_CIRCLE;
			if(per->press & PDD_DGT_ST)
				padData.debounce[padCounter] |= PAD_START;

			if(per->press & PDD_DGT_TR)
				padData.debounce[padCounter] |= PAD_R1;
			if(per->press & PDD_DGT_TL)
				padData.debounce[padCounter] |= PAD_L1;
		
			// toggle timer bars on/off
			if((per->press & PDD_DGT_TY)&&((per->r <= 128)&&(per->r > 0))&&((per->l <= 128)&&(per->l > 0)))
				timerBars = !timerBars;

			// temp pad emulation
			padData.digital[padCounter] = 0;
			if(per->on & PDD_DGT_KU)
				padData.digital[padCounter] |= PAD_UP;
			if(per->on & PDD_DGT_KD)
				padData.digital[padCounter] |= PAD_DOWN;
			if(per->on & PDD_DGT_KR)
				padData.digital[padCounter] |= PAD_RIGHT;
			if(per->on & PDD_DGT_KL)
				padData.digital[padCounter] |= PAD_LEFT;

			if(per->on & PDD_DGT_TB)
				padData.digital[padCounter] |= PAD_TRIANGLE;
			if(per->on & PDD_DGT_TA)
				padData.digital[padCounter] |= PAD_CROSS;
			if(per->on & PDD_DGT_TX)
				padData.digital[padCounter] |= PAD_SQUARE;
			if(per->on & PDD_DGT_TY)
				padData.digital[padCounter] |= PAD_CIRCLE;
			if(per->on & PDD_DGT_ST)
				padData.digital[padCounter] |= PAD_START;

			padCounter++;
			i++;
		}
		
		if((gameState.mode != PAUSE_MODE ) && (gameState.mode != GAMEOVER_MODE))
		{
			UpdateTextureAnimations();
		}

		// Jim: I'm sorry...
//		if( saveInfo.saveFrame )
		{
			DCTIMER_START(1);
			kmBeginScene(&kmSystemConfig);
			kmBeginPass(&vertexBufferDesc);
			DCTIMER_STOP(1);
		}

		// pass through game loop
		GameLoop();

		// loop the music
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

		// *ASL* 10/08/2000 - Immediately abort the loop on user quit
//		if (globalAbortFlag == 1)
//		{
//			break;
//		}

		// *ASL* 20/08/2000 - Reset all strip cache vars
		stripFT3textureID = -1;
		stripFT3textureID_A = -1;
		stripFT3dsided = FALSE;
		stripFT3dsided_A = FALSE;

		stripFT4textureID = -1;
		stripFT4textureID_A = -1;
		stripFT4dsided = FALSE;
		stripFT4dsided_A = FALSE;

		stripGT3textureID = -1;
		stripGT3textureID_A = -1;
		stripGT3dsided = FALSE;
		stripGT3dsided_A = FALSE;

		stripGT4textureID = -1;
		stripGT4textureID_A = -1;
		stripGT4dsided = FALSE;
		stripGT4dsided_A = FALSE;

		stripGT3FMAtextureID = -1;
		stripGT3FMAtextureID_A = -1;

		stripGT4FMAtextureID = -1;
		stripGT4FMAtextureID_A = -1;
		stripGT4FMAtextureID_Add = -1;

		stripTF4SPRtextureID = -1;
		stripTF4SPRtextureID_A = -1;

		stripGT3FMATransTextureID = -1;
		stripGT4FMATransTextureID = -1;

		strip3DBackdropTextureID = -1;


		// Jim: I'm sorry...
		if( !saveInfo.saveFrame )
		{
			DCTIMER_START(1);
			kmBeginScene(&kmSystemConfig);
			kmBeginPass(&vertexBufferDesc);
			DCTIMER_STOP(1);
		}

		// JH:  Main Draw Function That Runs All The Draw Functions.
		if(gameState.mode == ARTVIEWER_MODE)
		{
			DrawArtBackDrop(0, 0);
			DrawScreenTransition();
		}
		else if(gameState.mode == THEEND_MODE)
		{
			PrintTextOverlays();
			DrawScreenTransition();
		}
		else
		{
			DCTIMER_START(2);		
			if(!skipSpecFX)
				DrawSpecialFX();
			PrintSprites();
			DCTIMER_STOP(2);

			DCTIMER_START(3);		
			if(drawLandscape && drawGame)
				DrawWorld();		
			DCTIMER_STOP(3);		

			DCTIMER_START(4);
			if(drawLandscape && drawGame)
				DrawScenicObjList();
			if(drawLandscape && drawGame)
				DrawWaterList();
			DCTIMER_STOP(4);

			DCTIMER_START(5);		
			if(drawGame)
				DrawActorList();
			DCTIMER_STOP(5);		

			if( gameState.multi == SINGLEPLAYER )
			{
				UpdateFrogTongue(0);
				UpdateFrogCroak(0);
			}

			if(tileTexture[0])
				DrawTiledBackdrop(saveInfo.saveFrame ? NO : YES);

			DCTIMER_START(6);				
			if( !saveInfo.saveFrame && !afterSaveFlag )
			{
				PrintSpriteOverlays(1);
				PrintSpriteOverlays(0);
				PrintTextOverlays();
			}
			DrawScreenTransition();
			DCTIMER_STOP(6);		
		}

		// update the samples and set them to play
		if((gameState.mode == INGAME_MODE)||(gameState.mode == FRONTEND_MODE))
		{
			UpdateAmbientSoundsVolume();
		}

		gte_SetRotMatrix(&GsWSMATRIX);
		gte_SetTransMatrix(&GsWSMATRIX);

		DCTIMER_START(7);		
		DCTIMER_STOP(7);		
		
//		acDigiPortsAvailable(&digiNum,&midiNum);

//		sprintf(textbuffer,"SFX: %d (%d) (%d)",numSfxPlaying(), ambientSoundList.numEntries,digiNum);
//		fontPrint(font, textPosX-200,textPosY-140, textbuffer, 255,255,255);		

//		sprintf(textbuffer,"QFX: %d", qfxQueryState());
//		fontPrint(font, textPosX-200,textPosY-140+16, textbuffer, 255,255,255);		

//		if (showQFXError)
//		{
//			showQFXError--;
//			sprintf(textbuffer,"QFX: !!Error!!");
//			fontPrint(font, textPosX-200,textPosY-140+32, textbuffer, 255,255,255);
//		}


/*		{
		 	AMBIENT_SOUND *amb,*amb2;
		 	SVECTOR *pos;

			int	yp = textPosY-140+16;

 			for( amb = ambientSoundList.head.next; amb != &ambientSoundList.head; amb = amb2 )
 			{
 				amb2 = amb->next;
 
 				// If it is attached to a platform, make ambient follow that platform
 				if( amb->follow )
 					SetVectorSS( &amb->pos, &amb->follow->position );
 
 				// If sound doesn't have a source
				pos = &amb->pos;

				acSystemDelay(500000);

				if (amb->handle == -1 || amb->sample == NULL)
					sprintf(textbuffer,"amb err");
				else
					sprintf(textbuffer,"%s, %d", amb->sample->idName, current[amb->handle].volume);
				fontPrint(font, textPosX-200, yp, textbuffer, 255,255,255);

				yp += 16;
 			}
		}
*/

/*		if(timerBars)
		{
			DCTIMER_PRINTS();
			
		 	numUsed = 0;
			for(i=0; i<24; i++)
			{
				if(current[i].sound.isPlaying)
					numUsed++;
			}

//			// *ASL* 14/08/2000 - sprite numbers
//			sprintf(textbuffer, "%d, %d  %d", spriteNum, spriteRotNum, spriteNum+spriteRotNum);
//			fontPrint(font, textPosX,textPosY+16, textbuffer, 255,255,255);
		}
*/	

		kmEndPass(&vertexBufferDesc);
				
		DCTIMER_START(8);		
		kmRender(KM_RENDER_FLIP);
		kmEndScene(&kmSystemConfig);

/*		if((gameState.mode != PAUSE_MODE) || (quittingLevel))
		{
			//bb
			lastActFrameCount = actFrameCount;

			gameSpeed = vsyncCounter<<12;

			if(gameSpeed > (5<<12))
				gameSpeed = (5<<12);
					
			actFrameCount += gameSpeed>>12;
			vsyncCounter = 0;
		}
*/		
		if((gameState.mode!=PAUSE_MODE) || ((quittingLevel)&&(!pauseConfirmMode)))
		{
			lastActFrameCount = actFrameCount;

			gameSpeed = vsyncCounter<<12;
	
			if(gameSpeed > (5<<12))
				gameSpeed = (5<<12);

 			actFrameCount += gameSpeed>>12;

#ifndef NTSC_VERSION
			// test to see if 50Hz PAL
			if(hzSelect == 0)
			{
				gameSpeed *= 6;
				gameSpeed /= 5;
			}
#endif	
			vsyncCounter = 0;
		}
		else
		{
			pauseGameSpeed = vsyncCounter<<12;
			vsyncCounter = 0;
		}

		// filter the sound commands to the sound chip
		if (qfxPumpCommands() == 0)
		{
			// acknowledge our error
			showQFXError = 50;
		}

		pdExecPeripheralServer();
		DCTIMER_STOP(8);		
		DCTIMER_STOP(0);
	}


	// *ASL* 10/08/2000
	// ** We got to here so user must have opened the lid or quit from the frontend. Either
	// ** way we should shutdown all and return to the BootROM.

	// shutdown the sound effect queue
	qfxShutdown();

	// shutdown
	FreeBackdrop();
	bpAmShutdown();
	syChainDeleteHandler(vblChain);

	// exit the system
	sbExitSystem();

	// forcibly displays the main menu screen of the BootROM
	syBtExit();
}


// *ASL* 10/08/2000
/* ---------------------------------------------------------
   Function : vblankCallback
   Purpose : vblanking callback
   Parameters : arguments pointer
   Returns : 
   Info : 
*/

static void vblankCallback(void *arg)
{
	Sint32	gdstat;

	if (vsyncCounter > lastframe)
	{
		lastframe = vsyncCounter;
		myVsyncCounter = 0;
	}
	else
	{
		myVsyncCounter++;
	}
	frame++;
	vsyncCounter++;	

	// check if the user opened the tray..
	gdstat = gdFsGetDrvStat();
	if (gdstat == GDD_DRVSTAT_OPEN || gdstat == GDD_DRVSTAT_BUSY)
	{
		gdFsReqDrvStat();
	}
}

/* ---------------------------------------------------------
   Function : gdFsErrorCallback
   Purpose : gsFs error function callback
   Parameters : user object pointer, error code
   Returns : 
   Info : 
*/

static void gdFsErrorCallback(void *obj, Sint32 err)
{
	// check if the user opened the tray..
	if (globalAbortFlag == 0)
	{
		// was the tray opened?
		if (err == GDD_ERR_TRAYOPEND || err == GDD_ERR_UNITATTENT)
		{
			globalAbortFlag = 1;
		}
	}
}


// *ASL* 12/08/2000
/* ---------------------------------------------------------
   Function : showLegalFMV
   Purpose : show all legal screens and FMV
   Parameters : allow user to skip the legal stuff
   Returns : 
   Info : 
*/

void showLegalFMV(int allowQuit)
{
	int	ret, flag;

	utilPrintf("Playing FMV.....\n");

	ret = 0;

	// make sure the currently loaded backdrop image is free
	FreeLegalBackdrop();

	// play Hasbro FMV
	fmvSoftReset = FMV_SOFTRESET_TOBOOTROM;
	if (StartVideoPlayback(FMV_ATARI_LOGO, allowQuit) == 1)
		return;
	// play our FMV
	fmvSoftReset = FMV_SOFTRESET_TOBOOTROM;
	if (StartVideoPlayback(FMV_BLITZ_LOGO, allowQuit) == 1)
		return;
	// play our intro
	fmvSoftReset = FMV_SOFTRESET_TOBOOTROM;
	if (StartVideoPlayback(FMV_INTRO, 1) == 1 && allowQuit)
		return;

	// show SoftDec screen
	actFrameCount = 0;
	InitBackdrop("SOFDEC");
	ScreenFade(0,255,20);
	flag = 0;
	while (!flag || fadingOut)
	{
		DrawLegalBackDrop();
		DrawScreenTransition();
		actFrameCount++;
		if (!flag && (actFrameCount > (3*60)))
		{
			ScreenFade(255,0,20);
			flag = 1;
		}
		if (!flag && allowQuit && startButtonPressed() == 1)
		{
			ret = 1;
			ScreenFade(255,0,20);
			flag = 1;
		}
	}
	FreeLegalBackdrop();
	if (ret == 1)
		return;

	// show legal screen
	actFrameCount = 0;

	// setup correct legal screen
#ifdef NTSC_VERSION
	InitBackdrop ("FR2LEGAL");
#else
	InitBackdrop ("FR2LEGALEU");
#endif

	ScreenFade(0,255,20);
	flag = 0;
	while (!flag || fadingOut)
	{
		DrawLegalBackDrop();
		DrawScreenTransition();
		actFrameCount++;
		if (!flag && (actFrameCount > (3*60)))
		{
			ScreenFade(255,0,20);
			flag = 1;
		}
		if (!flag && allowQuit && startButtonPressed() == 1)
		{
			ret = 1;
			ScreenFade(255,0,20);
			flag = 1;
		}
	}
	FreeLegalBackdrop();
}


// *ASL* 13/08/2000
/* ---------------------------------------------------------
   Function : resetToBootROM
   Purpose : reset to boot rom
   Parameters :
   Returns : 
   Info : 
*/

void resetToBootROM()
{
	bpAmShutdown();
	syChainDeleteHandler(vblChain);
	sbExitSystem();
	syBtExit();
	for (;;);
}
