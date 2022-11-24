#include "include.h"

#ifndef _WORLDEFF_H_INCLUDE
#define _WORLDEFF_H_INCLUDE

void DrawWater ( FMA_MESH_HEADER *mesh, int flags );

typedef struct _SCENICOBJ
{
	struct _SCENICOBJ *next,*prev;			// ptr to next / prev platform

	int						flags;
	FMA_WORLD*		fmaObj;
//	SVECTOR				position;

	//bb
	MATRIX matrix;

} SCENICOBJ;

typedef struct _SCENICOBJLIST
{
	int				numEntries;
	SCENICOBJ		head;
} SCENICOBJLIST;

extern SCENICOBJLIST scenicObjList;

void CreateAndAddScenicObject(SCENIC *sc);

void InitScenicObjLinkedList ( void );

void AddScenicObj ( SCENICOBJ *scenicObj );

void FreeScenicObjectList ( void );

void SubScenicObject ( SCENICOBJ *scenicObj );

void DrawScenicObj ( FMA_MESH_HEADER *mesh, int flags );

void DrawCube ( FMA_MESH_HEADER *mesh, int flags );

#endif