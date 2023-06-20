/*

	This file is part of Frogger2, (c) 1999 Interactive Studios Ltd.

	File			: E3_Demo.c
	Programmer		: David Swift
	Date			: 14/04/2000
	Comments		: E3_Demo.c is part of the demo for E3'00

----------------------------------------------------------------------------------------------- */

#include <islpad.h>
#include <islutil.h>
#include <islmem.h>

#include "frogger.h"
#include "frontend.h"
#include "backdrop.h"
#include "textover.h"
#include "main.h"
#include "define.h"
#include "game.h"
#include "overlays.h"
#include "sprite.h"
#include "e3_demo.h"
#include "layout.h"
#include "multi.h"

#ifdef PC_VERSION
#include <banks.h>
#endif

#define NUMSPARKLES 30

#define ICON_WIDTH ((64*4096)/640)
#define ICON_HEIGHT ((64*4096)/480)

#define SPARK_WIDTH		128
#define SPARK_HEIGHT	192

const int circpos[4][2] = { { 1728, 1346 }, { 1728, 2070 }, { 1728, 2740 }, { 1732, 3390 } };
const int textpos[4][2] = { { 200, 1231 }, { 200, 1931 }, { 200, 2599 }, { 200, 3195 } };

const char* levelseltext[4] = { "Haunted House", "Boulder Canyon", "Space Chase", "2 Player" };
const char* levelicons[4] = { "HALL2", "ANCIENT1", "SPACE2", "ANCIENTMULTI" };
typedef struct _E3LEVELSELSTUFF
{
	SPRITEOVERLAY *sparkles[NUMSPARKLES];
	SPRITEOVERLAY *icons[4];
	SPRITEOVERLAY *levels[4];
	TEXTOVERLAY *leveltxt;
	TEXTOVERLAY *t[4];
	int sel, s;
	unsigned long prevSparkleTime;
	TIMER demoMode;
} E3LEVELSELSTUFF;

E3LEVELSELSTUFF *e3l;

void StartE3LevelSelect(void)
{
	char fileName[64];

	int i;

	FreeAllLists();

	utilPrintf("Starting E3 level select mode ----------------------\n");
	LoadTextureBank(INGAMEGENERIC_TEX_BANK);
	LoadTextureBank(FRONTEND_TEX_BANK);

#ifdef PSX_VERSION
	sprintf(fileName, "LEVELSEL.RAW" );
	InitBackdrop ( fileName );
#else
	InitBackdrop ("LEVELSEL");
#endif

	gameState.mode = E3_LEVELSELECT_MODE;

	e3l = (E3LEVELSELSTUFF*)MALLOC0(sizeof(E3LEVELSELSTUFF));
	e3l->sel = Random(3);
	NUM_FROGS = 0;
	GTInit(&e3l->demoMode, 15);

	for (i=0; i<4; i++)
	{
		e3l->icons[i] = CreateAndAddSpriteOverlay(
			circpos[i][0]-ICON_WIDTH/2, circpos[i][1]-ICON_HEIGHT/2, "LSEL_ICO", ICON_WIDTH, ICON_HEIGHT, 255, 0);
		e3l->icons[i]->num = 1;

		e3l->t[i] = CreateAndAddTextOverlay(
			textpos[i][0], textpos[i][1], (char*)levelseltext[i], 0, 255, fontSmall, 0, 0);

		e3l->t[i]->r = e3l->t[i]->b = 128, e3l->t[i]->g = 255;

		e3l->levels[i] = CreateAndAddSpriteOverlay(2500, 1500, (char*)levelicons[i], 819, 1092, 255, 0);
		e3l->levels[i]->draw = 0;

	}

	for (i=0; i<NUMSPARKLES; i++)
	{
		e3l->sparkles[i] = CreateAndAddSpriteOverlay(
			circpos[0][0], circpos[0][0], "FLASH", 128, 128, 0, SPRITE_ADDITIVE);
	}
	e3l->s = 0;

	//e3l->leveltxt = CreateAndAddTextOverlay(2200, 2700, (char*)levelseltext[0], 0, 255, font, 0, 0);
	CreateAndAddTextOverlay(2200, 2900, "Press Enter", 0, 192, font, 0, 0);
}

void RunE3LevelSelect ( void )
{
	SPRITEOVERLAY *sparkle;
	int i;

	if (padData.debounce[0])
		GTInit(&e3l->demoMode, 30);
	
	GTUpdate(&e3l->demoMode, -1);
	if (!e3l->demoMode.time)
	{
		NUM_FROGS = 1;
		FREE(e3l);
		FreeAllLists();
		FreeBackdrop();
		InitDemoMode();
		return;
	}

	for (i=0; i<NUMSPARKLES; i++)
	{
		sparkle = e3l->sparkles[i];

		if (sparkle->a > (gameSpeed>>10))
			sparkle->a -= (gameSpeed>>10);
		
		sparkle->width = Random(SPARK_WIDTH)+SPARK_WIDTH;
		sparkle->height = Random(SPARK_WIDTH)+SPARK_WIDTH;
	}

	for (i=0; i<4; i++)
	{
		if (i==e3l->sel)
		{
			e3l->icons[i]->draw = 1;
			e3l->levels[i]->draw = 1;
			e3l->t[i]->r = e3l->t[i]->b = e3l->t[i]->g = 255;
		}
		else
		{
			e3l->icons[i]->draw = 0;
			e3l->levels[i]->draw = 0;
			e3l->t[i]->r = e3l->t[i]->b = 128, e3l->t[i]->g = 192;
		}
	}

//	e3l->leveltxt->text = levelseltext[e3l->sel];

	if ((actFrameCount-e3l->prevSparkleTime)>1)
	{
		if ((++e3l->s) == NUMSPARKLES) e3l->s = 0;
		sparkle = e3l->sparkles[e3l->s];
		sparkle->xPos = (175*rsin(actFrameCount*70) >> 12) + circpos[e3l->sel][0] - 96;
		sparkle->yPos = (230*rcos(actFrameCount*70) >> 12) + circpos[e3l->sel][1] - 128;
		sparkle->a = 255;
		e3l->prevSparkleTime = actFrameCount;
	}

	if (padData.debounce[0] & PAD_UP)
	{
		if (e3l->sel > 0) e3l->sel--;
	}
	else if (padData.debounce[0] & PAD_DOWN)
	{
		if (e3l->sel < 3) e3l->sel++;
	}
	else if (padData.debounce[0] & (PAD_CROSS | PAD_START))
	{
		int sel = e3l->sel;

		FREE(e3l);
		FreeBackdrop();
		FreeAllLists();

		gameState.mode = INGAME_MODE;
		gameState.multi = SINGLEPLAYER;
		NUM_FROGS = 1;

		switch (sel)
		{
		case 0:
			InitLevel(WORLDID_HALLOWEEN, LEVELID_HALLOWEEN2);
			return;

		case 1:
			InitLevel(WORLDID_ANCIENT, LEVELID_ANCIENT1);
			return;

		case 2:
			InitLevel(WORLDID_SPACE, LEVELID_SPACE2);
			return;

		case 3:
			gameState.multi = MULTILOCAL;
			multiplayerMode = MULTIMODE_RACE;

			NUM_FROGS = 2;
			player[0].character = FROG_FROGGER;
			player[1].character = FROG_LILLIE;
			InitLevel(WORLDID_ANCIENT, LEVELID_ANCIENT4);
			return;
		}
	}
}
