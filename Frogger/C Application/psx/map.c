#include "include.h"

int numBabies = 0;
int tileCount = 0;
int scenicCount = 0;

void *collisionFile = NULL;
void *entityFile = NULL;
void *scenicFile = NULL;

GAMETILE	*gTStart[4];
GAMETILE	*bTStart[5];

GAMETILE	*firstTile;
SCENIC		*firstScenic;


void LoadScenics ( int collBank )
{
	int 	c;
	int 	fileLength;
	int 	*ptrAddr;
	char 	fileName[256];
	SCENIC 	*cur;
	SCENIC 	*next;

	gdFsChangeDir("\\");
	gdFsChangeDir("COLLISION");

	sprintf(fileName,"GARDEN1.SCE");

/*	switch ( collBank )
	{
		case GARDENLEV1_COL:
				sprintf ( fileName, "GARDEN1.SCE" );
			break;

		case GARDENLEV2_COL:
				sprintf ( fileName, "GARDEN2.SCE" );
			break;

		case ANCIENTLEV1_COL:
			sprintf ( fileName, "ANCIENTS1.SCE" );
			break;

		case ANCIENTLEV2_COL:
			sprintf ( fileName, "ANCIENTS2.SCE" );
			break;

		case ANCIENTLEV3_COL:
			sprintf ( fileName, "ANCIENTS3.SCE" );
			break;

		case ANCIENTMASTER_COL:
			sprintf ( fileName, "ANCIENTSBOSS.SCE" );
			break;
		
		case SPACELEV1_COL:
			sprintf ( fileName, "SPACE1.SCE" );
			break;

		case SPACELEV2_COL:
			sprintf ( fileName, "SPACE3.SCE" );
			break;

		case SPACELEV3_COL:
			sprintf ( fileName, "SPACEBOSS.SCE" );
			break;

		case CITYLEV1_COL:
			sprintf ( fileName, "CITY1.SCE" );
			break;

		case CITYLEV2_COL:
			sprintf ( fileName, "CITY2.SCE" );
			break;

		case CITYLEV3_COL:
			sprintf ( fileName, "CITY3.SCE" );
			break;

		case SUBTERRANEANLEV1_COL:
			sprintf ( fileName, "SUB1.SCE" );
			break;

		case SUBTERRANEANLEV2_COL:
			sprintf ( fileName, "SUB2.SCE" );
			break;

		case SUBTERRANEANLEV3_COL:
			sprintf ( fileName, "SUB3.SCE" );
			break;

		case SUBTERRANEANMASTER_COL:
			sprintf ( fileName, "SUBBOSS.SCE" );
			break;

		case LABORATORYLEV1_COL:
			sprintf ( fileName, "LAB1.SCE" );
			break;

		case LABORATORYLEV2_COL:
			sprintf ( fileName, "LAB2.SCE" );
			break;

		case LABORATORYLEV3_COL:
			sprintf ( fileName, "LAB3.SCE" );
			break;


		case HALLOWEENLEV1_COL:
			sprintf ( fileName, "HALLOWEEN1.SCE" );
			break;

		case HALLOWEENLEV2_COL:
			sprintf ( fileName, "HALLO2.SCE" );
			break;

		case HALLOWEENLEV3_COL:
			sprintf ( fileName, "HALLO3.SCE" );
			break;


		case FRONTEND1_COL:
			sprintf ( fileName, "START.SCE" );
			break;

	}
	// ENDSWITCH
*/
	//bb
	if(scenicFile)
		Align32Free(scenicFile);

	// change to the models directory
	gdFsChangeDir("\\");
	gdFsChangeDir("scenics");

	scenicFile = (void*)fileLoad(fileName, &fileLength);
	firstScenic = (SCENIC*)scenicFile;

	// Get a pointer to the addr and get scenic count from it...
	ptrAddr			= (int*)firstScenic;
	scenicCount = *ptrAddr;

	// Move on 4 bytes so that we can get the number of babies.
	firstScenic = (SCENIC*) ((char*)firstScenic + 4);

	c = 0;
	cur = firstScenic;

	utilPrintf("%d\n\n\n\n", sizeof(SCENIC));
	for ( c = 0; c < scenicCount; c++, cur++ )
	{
		if ( ( int ) cur->next != -1 )
			cur->next = ( SCENIC* ) ( ( int ) cur->next + ( int ) firstScenic );
		else
				// If not a valid pointer then set the pointer to NULL
			cur->next = 0;
		// ENDELSEIF
//		utilPrintf("%s, %d, %d, %d, %d\n", cur->name, cur->pos.vx*SCALE, cur->pos.vy, cur->pos.vz, scenicCount);

		cur->pos.vx *= SCALE;
		cur->pos.vy *= SCALE;
		cur->pos.vz *= SCALE;

		utilPrintf("%s, %d, %d, %d, %d\n", cur->name, cur->pos.vx*SCALE, cur->pos.vy, cur->pos.vz, scenicCount);
	}
	// ENDFOR

}

void LoadLevelEntities ( int worldID, int levelID )
{
	int fileLength;
	char fileName[256];

	gdFsChangeDir("\\");
	gdFsChangeDir("MAPS");
	
	sprintf(fileName, "ENTITY_%d_%d.dat", worldID, levelID);

	//bb
	if(entityFile)
		MALLOC01(entityFile);

	gdFsChangeDir("\\");
	gdFsChangeDir("maps");

	entityFile = (void *)fileLoad(fileName,&fileLength);

	if(!entityFile)
		return;
	// ENDIF

	if(!MemLoadEntities(entityFile))
	{
		utilPrintf("MemLoadEntities failed\n");
	}
	// ENDIF

	//bb
	if(entityFile)
		Align32Free(entityFile);
}

void LoadMapBank ( int collBank )
{
//	DBPRINTF(("Loading Map Banks, Collision And Scenics.\n"));

	LoadCollision(collBank);
	LoadScenics(collBank);
}


void LoadCollision(int collBank)
{
	char 		colFileName[256];
	int 		c, i;
	int			start;
	int 		fileLength;
	int 		*ptrAddr;
	int 		babStart[5];	// Temp holders for the baby game tiles....
	GAMETILE 	*cur;
	GAMETILE 	*next;

	switch(collBank)
	{
		case GARDENLEV1_COL:
			sprintf(colFileName, "GARDEN1.COL");
			break;

		case ANCIENTLEV1_COL:
			sprintf ( colFileName, "ANCIENTS1.COL" );
			break;

		case ANCIENTLEV2_COL:
			sprintf ( colFileName, "ANCIENTS2.COL" );
			break;

		case ANCIENTLEV3_COL:
			sprintf ( colFileName, "ANCIENTS3.COL" );
			break;

		case ANCIENTMASTER_COL:
			sprintf ( colFileName, "ANCIENTSBOSS.COL" );
			break;

		case SPACELEV1_COL:
			sprintf ( colFileName, "SPACE1.COL" );
			break;

		case SPACELEV2_COL:
			sprintf ( colFileName, "SPACE3.COL" );
			break;

		case SPACELEV3_COL:
			sprintf ( colFileName, "SPACEBOSS.COL" );
			break;

		case CITYLEV1_COL:
			sprintf ( colFileName, "CITY1.COL" );
			break;

		case CITYLEV2_COL:
			sprintf ( colFileName, "CITY2.COL" );
			break;

		case CITYLEV3_COL:
			sprintf ( colFileName, "CITY3.COL" );
			break;

		case SUBTERRANEANLEV1_COL:
			sprintf ( colFileName, "SUB1.COL" );
			break;

		case SUBTERRANEANLEV2_COL:
			sprintf ( colFileName, "SUB2.COL" );
			break;

		case SUBTERRANEANLEV3_COL:
			sprintf ( colFileName, "SUB3.COL" );
			break;

		case SUBTERRANEANMASTER_COL:
			sprintf ( colFileName, "SUBBOSS.COL" );
			break;

		case LABORATORYLEV1_COL:
			sprintf ( colFileName, "LAB1.COL" );
			break;

		case LABORATORYLEV2_COL:
			sprintf ( colFileName, "LAB2.COL" );
			break;

		case LABORATORYLEV3_COL:
			sprintf ( colFileName, "LAB3.COL" );
			break;

		case HALLOWEENLEV1_COL:
			sprintf ( colFileName, "HALLOWEEN1.COL" );
			break;

		case HALLOWEENLEV2_COL:
			sprintf ( colFileName, "HALLO2.COL" );
			break;

		case HALLOWEENLEV3_COL:
			sprintf ( colFileName, "HALLO3.COL" );
			break;


		case FRONTEND1_COL:
			sprintf ( colFileName, "START.COL" );
			break;
	}
	// ENDSWITCH

//	strcat ( fileName, ".col" );

//bb
	if(collisionFile)
		Align32Free(collisionFile);

	gdFsChangeDir("\\");
	gdFsChangeDir("collision");

	collisionFile = (void*) fileLoad ( colFileName, &fileLength );
	firstTile = (GAMETILE*) collisionFile;

	// Get a pointer to the addr and get tile count from it...
	ptrAddr		= (int*) firstTile;
	tileCount = (int) *ptrAddr;

	// Move on 4 bytes so that we can get the start tile.
	firstTile = (SCENIC*)((char*)firstTile + 4);

	// Get a pointer to the addr and get a temp pointer to the Frogger startTile.....
	ptrAddr = (int*) firstTile;
	start		= (int)*ptrAddr;

	// Move on 4 bytes so that we can get the number of babies.
	firstTile = (SCENIC*)((char*)firstTile + 4);

	// Get a pointer to the addr and get the number of babies.....
	ptrAddr		= (int*)firstTile;
	numBabies	= (int)*ptrAddr;

	// Move on 4 bytes so that we can get the first baby game tile....
	firstTile = (SCENIC*)((char*)firstTile + 4);

	for ( c = 0; c < numBabies; c++ )
	{
		// Get a pointer to the addr and get the game tile.....
		ptrAddr					= ( int* ) firstTile;
		babStart [ c ]	= ( int ) *ptrAddr;

		// Move on 4 bytes so that we can get the next baby game tile....
		firstTile = (SCENIC*)((char*)firstTile + 4);

	}
	// ENDFOR - for ( c = 0; c < numBabies; c++ )
	

	// These need to be done last, so that we get the correct firstTile offset in the file.....

	//  Set the offsets for the memory pointers in memory for the baby game tiles....
	for ( c = 0; c < numBabies; c++ )
	{
		bTStart [c] = ( GAMETILE* ) ( ( int ) babStart [ c ] + ( int ) firstTile );
	}
	// ENDFOR - for ( c = 0; c < numBabies; c++ )

	//  And lets do the same for the frogger start tile....

	gTStart [0] = ( GAMETILE* ) ( ( int ) start + ( int ) firstTile );


	c = 0;
	cur = firstTile;

	utilPrintf("%d", sizeof(GAMETILE));

	for ( c = 0; c < tileCount; c++, cur++ )
	{
		// If the memory pointer is not -1 then set them up so that they offset into the correct place in the tile list...

		if ( ( int ) cur->tilePtrs[0] != -1 )
			cur->tilePtrs[0] = ( GAMETILE* ) ( ( int ) cur->tilePtrs[0] + ( int ) firstTile );
		else
				// If not a valid pointer then set the pointer to NULL
			cur->tilePtrs[0] = 0;
		// ENDELSEIF

		if ( ( int ) cur->tilePtrs[1] != -1 )
			cur->tilePtrs[1] = ( GAMETILE* ) ( ( int ) cur->tilePtrs[1] + ( int ) firstTile );
		else
				// If not a valid pointer then set the pointer to NULL
			cur->tilePtrs[1] = 0;
		// ENDELSEIF

		if ( ( int ) cur->tilePtrs[2] != -1 )
			cur->tilePtrs[2] = ( GAMETILE* ) ( ( int ) cur->tilePtrs[2] + ( int ) firstTile );
		else
				// If not a valid pointer then set the pointer to NULL
			cur->tilePtrs[2] = 0;
		// ENDELSEIF

		if ( ( int ) cur->tilePtrs[3] != -1 )
			cur->tilePtrs[3] = ( GAMETILE* ) ( ( int ) cur->tilePtrs[3] + ( int ) firstTile );
		else
				// If not a valid pointer then set the pointer to NULL
			cur->tilePtrs[3] = 0;
		// ENDELSEIF

		//ma cast to an int !
		if((int)cur->next != -1)
			cur->next = ( GAMETILE* ) ( ( int ) cur->next + ( int ) firstTile );
		else
				// If not a valid pointer then set the pointer to NULL
			cur->next = 0;
		// ENDELSEIF


		cur->centre.vx *= SCALE;
		cur->centre.vy *= SCALE;
		cur->centre.vz *= SCALE;

// 		for ( i = 0; i < 4; i++ )
// 		{
// 			cur->dirVector[i].vz *= -1;
// 		}
// 		// ENDFOR
// 
// 		cur->normal.vz *= -1;

	}
	// ENDFOR


}



void FreeEntityBank ( void )
{
	utilPrintf("Freeing Entity Bank : MAP\n");

	Align32Free ( ( void* ) entityFile );
}


void FreeMap ( void )
{
	utilPrintf("Freeing Map Bank : MAP\n");

	Align32Free ( ( void* ) collisionFile );
}

void FreeScenic ( void )
{
	utilPrintf("Freeing Scenic Bank : MAP\n");

	Align32Free ( ( void* ) scenicFile );
}
