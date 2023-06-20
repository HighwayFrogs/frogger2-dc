/*

	This file is part of Frogger2, (c) 1999 Interactive Studios Ltd.

	File		: sprite.c
	Programmer	: Jim Hubbard
	Date		: 25/02/00

----------------------------------------------------------------------------------------------- */

#define F3DEX_GBI_2

#include "layout.h"
#include "Main.h"
#include "frogger.h"
#include "define.h"
#include "game.h"
#include "frontend.h"
#include "story.h"

//ma#include <stdio.h>

#ifdef PC_VERSION
#include <gelf.h>
#include <mdxddraw.h>
#endif

#define IMAGETIME_SECONDS 10

FMV_DATA fmv[NUM_FMV_SEQUENCES] = 
{
	"",0,
	"HLOGO",250,		//	FMV_ATARI_LOGO,            
	"BLITZS",147,		//	FMV_BLITZ_LOGO,            
	"INTRO",147,		//	FMV_INTRO,
	"02S",1085,			//	FMV_FROGGER_LILLIE_BABIES, 
	"03S",677,			//	FMV_SWAMPY_PLAN,           
	"04S",525,			//	FMV_SWAMPY_ESCAPES,        
	"05S",386,			//	FMV_LILLIE_TO_GATE,        
	"06S",518,			//	FMV_BOTH_AT_GATE,          
	"07S",527,			//	FMV_LILLIE_COMPUTER,       
	"08S",943,			//	FMV_SWAMPY_COMMERCIAL,     
	"09S",968,			//	FMV_TELEPORT_TO_EARTH,     
	"10S",462,			//	FMV_ENTER_HAUNTED_HOUSE,   
	"11S",948,			//	FMV_LILLIE_IN_CAGE,        
	"12S",737,			//	FMV_VICTORY,               
};

short storySequenceLevelToChapter[] = 
{
	0,0,1,1,2,2,3,3,4,4,5,5,6,6,7,8,9
};

GAME_LEVEL storySequence[] = 
{
	{WORLDID_FRONTEND,3,1,FROG_FROGGER,0,""},
	{WORLDID_GARDEN,0,0,FROG_FROGGER,FMV_FROGGER_LILLIE_BABIES,"ICONRETROP"},

	{WORLDID_ANCIENT,0,1,FROG_LILLIE,FMV_SWAMPY_PLAN,"ICONRETROP"},
	{WORLDID_SUBTERRANEAN,0,0,FROG_FROGGER,0,"ICONRETROP"},

	{WORLDID_ANCIENT,2,1,FROG_LILLIE,FMV_SWAMPY_ESCAPES,"ICONRETROP"},
	{WORLDID_ANCIENT,1,0,FROG_LILLIE,0,"ICONRETROP"},

	{WORLDID_SUBTERRANEAN,1,1,FROG_FROGGER,FMV_LILLIE_TO_GATE,"ICONRETROP"},
	{WORLDID_SUBTERRANEAN,2,0,FROG_FROGGER,0,"ICONRETROP"},

	{WORLDID_LABORATORY,0,1,FROG_LILLIE,FMV_BOTH_AT_GATE,"ICONRETROP"},
	{WORLDID_LABORATORY,1,0,FROG_LILLIE,0,"ICONRETROP"},

	{WORLDID_SPACE,1,1,FROG_FROGGER,FMV_LILLIE_COMPUTER,"ICONMULTOP"},
	{WORLDID_SPACE,0,0,FROG_FROGGER,0,"ICONMULTOP"},

	{WORLDID_LABORATORY,2,1,FROG_LILLIE,FMV_SWAMPY_COMMERCIAL,"ICONMULTOP"},
	{WORLDID_SPACE,2,0,FROG_FROGGER,0,"ICONMULTOP"},

	{WORLDID_HALLOWEEN,0,0,FROG_FROGGER,FMV_TELEPORT_TO_EARTH,"ICONCHEATOP"},
	
	{WORLDID_HALLOWEEN,1,0,FROG_FROGGER,FMV_ENTER_HAUNTED_HOUSE,"ICONCHEATOP"},
	
	{WORLDID_HALLOWEEN,2,0,FROG_BABYFROG,FMV_LILLIE_IN_CAGE,"ICONCHEATOP"},
};

	
/*	--------------------------------------------------------------------------------
    Function		: StoryStartLevel
	Parameters		: void
	Returns			: 1 if we went into a new mode, 0 if we want to do a default 'next level' thing
*/
void StoryStartLevel(void)
{
	player[0].character = storySequence[gameState.storySequenceLevel].character;
}

