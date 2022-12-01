/*

	This file is part of Frogger2, (c) 1999 Interactive Studios Ltd.


	File		: levelplaying.c
	Programmer	: David Swift
	Date		: 17/03/99

----------------------------------------------------------------------------------------------- */

#include <islpad.h>
#include <islfile.h>
#include <islmem.h>
#include <islutil.h>
//ma#include <stdio.h>

#include "frontend.h"
#include "define.h"
#include "layout.h"
#include "game.h"
#include "textover.h"
#include "levplay.h"
#include "overlays.h"
#include "hud.h"
#include "fadeout.h"
#include "lang.h"

#ifdef DREAMCAST_VERSION
#include "main.h"
#endif

#define DEMO_TIMEOUT	25
#define NUM_DEMOS		3


// *ASL* 12/08/2000 - Show intro every n passes
#define	SHOW_INTRO_NOOF_PASSES	1


// -------
// Globals

unsigned long playKeyCount;
unsigned long curPlayKey;
unsigned long *playKeyList;

extern int oldDiffMode;

TIMER demoTimeout;

int demoLevels[NUM_DEMOS][2] = {
	{ WORLDID_GARDEN,	LEVELID_GARDEN1 },
	{ WORLDID_ANCIENT,	LEVELID_ANCIENT1 },
	{ WORLDID_HALLOWEEN,LEVELID_HALLOWEEN2 }
//	{ WORLDID_ANCIENT,	LEVELID_ANCIENT1 }
};

int nextDemo = 0;


// *ASL* 12/08/2000
// user quit flag
int userQuit = 0;
// show intro pass count
int showIntroPass = SHOW_INTRO_NOOF_PASSES;

// demo screen vars
char		demoCBuf[64];
TEXTOVERLAY	*demoRestartText = NULL;


// -------------------
// Function Prototypes

// *ASL* 12/08/2000 - main,c - show all legal screens and FMV
extern void showLegalFMV(int allowQuit);


/*	--------------------------------------------------------------------------------
	Function		: LoadDemoFile
	Purpose			: load demo file
	Parameters		: 
	Returns			: 
*/
void LoadDemoFile(int world, int level)
{
	char filename[128]; int count;
	sprintf(filename, "DEMOS\\RECORD_%lu_%lu.KEY", world, level);
	
	playKeyList = (unsigned long*)fileLoad(filename, &count);
	
	if (playKeyList)
		playKeyCount = count / (3*4);
}

/*	--------------------------------------------------------------------------------
	Function		: InitDemoMode
	Purpose			: Initialise demo mode
	Parameters		: 
	Returns			: 
*/
void InitDemoMode()
{
	int world, level;

	utilPrintf ( "Next Demo : %d\n", nextDemo );
	world = demoLevels[nextDemo][0];
	level = demoLevels[nextDemo][1];

	gameState.multi = SINGLEPLAYER;
	gameState.single = ARCADE_MODE;
	gameState.mode = DEMO_MODE;

	playKeyList = NULL;
	InitLevel(world, level);

	if (!playKeyList)
	{
		utilPrintf("DEMO MODE: Failed loading %s, returning\n"); return;
	}
	curPlayKey = 0;

#ifdef PSX_VERSION
	CreateAndAddTextOverlay(2048, 200+390+390+390, GAMESTRING(STR_DEMO_MODE), YES, 128, NULL, 0);
	displayPage[0].drawenv.isbg = displayPage[1].drawenv.isbg = 1;
#else
	CreateAndAddTextOverlay(2048, 128, GAMESTRING(STR_DEMO_MODE), YES, 128, NULL, 0);
#endif

	CreateAndAddTextOverlay(2048, 3500, GAMESTRING(STR_PRESSSTART), YES, 128, NULL, 0);
//	CreateAndAddSpriteOverlay( 3000, 2700, "BLITZGAMES", 800, 1000, 190, 0 );
//	CreateAndAddSpriteOverlay( 250, 3200, "FROGGER2", 600, 800, 255, 0 );

	// *ASL* 18/08/2000 - Show Best Time for the current playing demo level
	{
		TEXTOVERLAY	*bTxt;

		// print level name
		bTxt = CreateAndAddTextOverlay(2048, 200, GAMESTRING(worldVisualData[player[0].worldNum].levelVisualData[player[0].levelNum].description_str), YES, (char)127, NULL, TEXTOVERLAY_SHADOW);
		bTxt->r = 0;
		bTxt->g = 255;
		bTxt->b = 255;
		bTxt->xPosTo = 2048;

		// print level best time
		sprintf(demoCBuf, GAMESTRING(STR_RECORD),worldVisualData[player[0].worldNum].levelVisualData[player[0].levelNum].parName,((int)worldVisualData[player[0].worldNum].levelVisualData[player[0].levelNum].parTime/600)%600,((int)worldVisualData[player[0].worldNum].levelVisualData[player[0].levelNum].parTime/10)%60,((int)worldVisualData[player[0].worldNum].levelVisualData[player[0].levelNum].parTime)%10);
		demoRestartText = CreateAndAddTextOverlay(2048, 200+390, demoCBuf, YES, 128, NULL, TEXTOVERLAY_SHADOW);
		demoRestartText->r = 255;
		demoRestartText->g = 255;
		demoRestartText->b = 255;
		demoRestartText->xPosTo = 2048;
	}

	GTInit(&demoTimeout, DEMO_TIMEOUT);

	DisableHUD();

	// *ASL* 12/08/2000 - Clear user quit flag
	userQuit = 0;
}

/*	--------------------------------------------------------------------------------
	Function		: RunDemoLoop
	Purpose			: Runs the demo-mode game loop
	Parameters		: 
	Returns			: 
*/
int quittingDemo = NO;
void RunDemoMode()
{
	int pl;

	GTUpdate(&demoTimeout, -1);

	// *ASL* 18/08/2000 - Update restart text colour
	if (demoRestartText)
	{
		demoRestartText->r = 127+((rsin(actFrameCount*4000)+4096)*64)/4096;
		demoRestartText->g = 127+((rcos(actFrameCount*4000)+4096)*64)/4096;
		demoRestartText->b = 16;
	}

	if((quittingDemo) && (fadingOut == 0))
	{
		// *ASL* 18/08/2000 - Clear out restart text
		demoRestartText = NULL;

		quittingDemo = NO;
		utilPrintf("We've finished with demo, then exit..........................................\n");

		FREE(playKeyList);

		if ((++nextDemo) == NUM_DEMOS)
			nextDemo = 0;

		// just mute all ambient sounds until the InitLevel call below releases all properly
		sfxStopSound();

		// *ASL* 12/08/2000 - Determine whether to show FMV intro
		if (userQuit == 1)
		{
			// on user quit.. show next time
			showIntroPass = 0;
		}
		else
		{
			// need to show this pass?
			if (--showIntroPass <= 0)
			{
				showIntroPass = SHOW_INTRO_NOOF_PASSES;
				showLegalFMV(1);
			}
		}

#ifdef E3_DEMO
		StartE3LevelSelect();
#else
		if( UndoChangeModel( frog[0]->actor ) )
			player[0].idleEnable = 1;

		gameState.difficulty = oldDiffMode;
		oldDiffMode = gameState.difficulty;

		gameState.mode = FRONTEND_MODE;
#ifdef NTSC_VERSION
		player[0].character = FROG_FROGGER;
#else
		player[0].character = FROG_FROGGER;
		player[0].worldNum = WORLDID_FRONTEND;
		player[0].levelNum = LEVELID_FRONTEND1;
#endif
		InitLevel(WORLDID_FRONTEND, LEVELID_FRONTEND1);
		frameCount = 0; // why? who can say? (me! me!)
#endif
		return;
	}

	if(quittingDemo)
		return;

	if ((padData.debounce[0] & PAD_START) || curPlayKey >= playKeyCount || !demoTimeout.time)
	{
		// *ASL* 12/08/2000 - User quit demo?
		if (padData.debounce[0])
			userQuit = 1;

		quittingDemo = YES;
		ScreenFade(255,0,30);
		keepFade = 0;
	}
	for (pl=0; pl<4; pl++)
		padData.digital[0] = padData.debounce[0] = 0;

	while ((curPlayKey < playKeyCount) && (playKeyList[curPlayKey*3]<actFrameCount))
	{
		pl = playKeyList[(curPlayKey*3)+2];

		padData.digital[pl] |= (unsigned short)playKeyList[(curPlayKey*3)+1];
		padData.debounce[pl] |= ~(unsigned short)playKeyList[((curPlayKey-1)*3)+1] & padData.digital[pl];
		curPlayKey++;
	}

	RunGameLoop();
}
