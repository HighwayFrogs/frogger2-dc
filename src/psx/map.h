#ifndef MAP_H
#define MAP_H

#include "define.h"

extern int numBabies;
extern int tileCount;

extern void *file;

extern SCENIC		*firstScenic;
extern GAMETILE *firstTile;
extern GAMETILE *gTStart[4];
extern GAMETILE *bTStart[5];

extern void *collisionFile;
extern void *scenicFile;

//void TeleportActorToTile(ACTOR2 *act,GAMETILE *tile,long pl);
//#define TeleportActorToTile(a,b,c) 0

void LoadLevelEntities ( int worldID, int levelID );

void LoadMapBank				( int collBank );
void LoadCollision			( int collBank );
void LoadScenics				( int collBank );
void FreeMap						( void );
void FreeEntityBank			( void );
void FreeScenic					( void );

#endif
