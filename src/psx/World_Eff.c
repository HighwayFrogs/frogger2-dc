#include "include.h"
#include "main.h"
#include "ptexture.h"
#include "gte.h"
#include "types.h"
#include "main.h"
#include "map_draw.h"
#include "actor2.h"
#include "maths.h"
#include "layout.h"
#include "frontend.h"
#include "sprite.h"

#include "World_Eff.h"

#define gte_stotz_cpu(r)	r = otz;

// JH: Let's define a name so that we can use sprite's in the scenic drawing routines.
#define NEW_SPRITE_MAPS

#define ADDITIVE		(1<<0)
#define SUBTRACTIVE		(1<<1)
#define USLIDING		(1<<4)
#define VSLIDING		(1<<5)
#define PLUSSLIDING		(1<<6)
#define MINUSSLIDING	(1<<7)
#define JIGGLE			(1<<8)
#define MODGY			(1<<9)
#define SLIDESPEED		(1<<10)
#define SLIDESPEED2		(1<<11)
#define SLOWSLIDE		(1<<12)

#define VRAM_STARTX			512
#define VRAM_PAGECOLS		8
#define VRAM_PAGEROWS		2
#define VRAM_PAGES			16
#define VRAM_PAGEW			32
#define VRAM_PAGEH			32
#define VRAM_SETX(X)		(X)
#define VRAM_SETY(Y)		((Y)*VRAM_PAGEW)
#define VRAM_SETXY(X,Y)		((X)+((Y)*VRAM_PAGEW))
#define VRAM_SETPAGE(P)		((P)<<16)
#define VRAM_GETX(HND)		((HND) & (VRAM_PAGEW-1))
#define VRAM_GETY(HND)		(((HND)/VRAM_PAGEW) & (VRAM_PAGEW-1))
#define VRAM_GETXY(HND)		((HND) & 0xffff)
#define VRAM_GETPAGE(HND)	((HND)>>16)
#define VRAM_CALCVRAMX(HND)	(512+((VRAM_GETPAGE(HND)%(VRAM_PAGECOLS))*64)+(VRAM_GETX(HND)*2))
#define VRAM_CALCVRAMY(HND)	(((VRAM_GETPAGE(HND)/(VRAM_PAGECOLS))*256)+(VRAM_GETY(HND)*8))

long slideSpeeds[] = {0, 1, 2, 4};

SVECTOR jiggledVerts[250];

typedef struct
{
	short x,y;
}SHORTXY;

extern	int	polyCount;

//#define WATERANIM_1A (u+((rcos(frame<<6)+4096)>>11))|((v+((rsin(frame<<6)+4096)>>11))<<8)
//#define WATERANIM_2B (u+((rsin(frame<<6)+4096)>>11))|((v+((rcos(frame<<6)+4096)>>11))<<8)

#define WATERANIM_1A (u+((rcos(frame<<10)+4096)>>5))|((v+((rsin(frame<<10)+4096)>>5))<<4)
#define WATERANIM_2B (u+((rsin(frame<<10)+4096)>>5))|((v+((rcos(frame<<10)+4096)>>5))<<8)

#define WATERANIM_1  ( ( u )  ) | ( ( v + ( ( 4096 ) >> 11 ) ) << 8 )
#define WATERANIM_2  ( ( u )  ) | ( ( v + ( ( 4096 ) >> 11 ) ) << 8 )

#define WATER_TRANS_CODE (((ULONG)2)<<24)
#define WATER_TRANS_CLUT (0x80000000)

#define MIN_MAP_DEPTH (10)
	
#define MAX_MAP_DEPTH (1000)

SCENICOBJLIST scenicObjList;

void DrawWater ( FMA_MESH_HEADER *mesh, int flags )
{
	register char *opcd;
	long 	*tfv;
	long 	*tfd;
	float	u0,u1,u2,u3;
	unsigned long 	t1;
	unsigned long 	t2;	
	int		x0,x1,x2,x3;
	int		y0,y1,y2,y3;
	int		z0,z1,z2,z3;

	#define GETX(n)( ((SHORTXY *)( (int)(tfv) +(n) ))->x )
	#define GETY(n)( ((SHORTXY *)( (int)(tfv) +(n) ))->y )
	#define GETV(n)(  *(unsigned long *)( (int)(tfv) +(n) ) )
	#define GETD(n)(  *(unsigned long *)( (int)(tfd) +(n) ) )

	register long depth;

	int i,texID;

	int min_depth = MIN_MAP_DEPTH + mesh->extra_depth;
	int max_depth = MAX_MAP_DEPTH + mesh->extra_depth;

	tfv = (long*)transformedVertices;
	tfd = (long*)transformedDepths;

	if ( max_depth > 1024 - mesh->extra_depth )
		max_depth = 1024 - mesh->extra_depth;

	if( mesh->flags & JIGGLE )
	{
		for ( i = 0; i < mesh->n_verts; i++ )
		{
			jiggledVerts[i].vx = mesh->verts[i].vx + ( rsin ( ( frame << 5 ) + ( mesh->verts[i].vx & ( mesh->verts[i].vz ) ) ) >> 6 );
			jiggledVerts[i].vy = mesh->verts[i].vy + ( rcos ( ( frame << 6 ) + ( mesh->verts[i].vx | ( mesh->verts[i].vz ) ) ) >> 8 );
			jiggledVerts[i].vz = mesh->verts[i].vz + ( rsin ( ( frame << 5 ) + ( mesh->verts[i].vx ^ ( mesh->verts[i].vz ) ) ) >> 7 );
		}
		// ENDFOR
		
		transformVertexListA ((VERT *)jiggledVerts, mesh->n_verts, tfv, tfd );
	}
	else
	{
		transformVertexListA((VERT *)mesh->verts, mesh->n_verts, tfv, tfd);
	}
	// ENDELSEIF
		
//	for ( i = 0; i < mesh->n_verts; i++ )
//	{
//		tfd [ i ] = tfd [ i ] >> 2;
//	}

#define si ((POLY_GT4*)packet)
#define op ((FMA_GT4 *)opcd)

//	polyCount += mesh->n_gt4s;
	
//	//ma temp find texture
	texID = FindTextureN("00WATE04");
	kmChangeStripTextureSurface(&StripHead_GT4_FMA_Trans,KM_IMAGE_PARAM1,&DCKtextureList[texID].surface);
	stripGT4FMATransTextureID = texID;
	kmChangeStripTextureSurface(&StripHead_GT3_FMA_Trans,KM_IMAGE_PARAM1,&DCKtextureList[texID].surface);
//	stripGT3FMATransTextureID = texID;

	op = mesh->gt4s;
	for ( i = mesh->n_gt4s; i != 0; i--, op++ )
	{
		char u,v;

		x0 = GETX(op->vert0); y0 = GETY(op->vert0); z0 = GETD(op->vert0);
		x1 = GETX(op->vert1); y1 = GETY(op->vert1); z1 = GETD(op->vert1);
		x2 = GETX(op->vert2); y2 = GETY(op->vert2); z2 = GETD(op->vert2);
		x3 = GETX(op->vert3); y3 = GETY(op->vert3); z3 = GETD(op->vert3);

		if((x0 > 640)&&(x1 > 640)&&(x2 > 640)&&(x3 > 640))
			continue;
		if((y0 > 480)&&(y1 > 480)&&(y2 > 480)&&(y3 > 480))
			continue;

		if((x0 < 0)&&(x1 < 0)&&(x2 < 0)&&(x3 < 0))
			continue;
		if((y0 < 0)&&(y1 < 0)&&(y2 < 0)&&(y3 < 0))
			continue;		

		gte_ldsz4(z0,z1,z2,z3);
	   	gte_avsz4();
		gte_stotz_cpu ( depth );
		
		if(depth > min_depth && depth < max_depth)
		{
/*			if ( mesh->flags & USLIDDING )
			{
				if ( mesh->flags & PLUSSLIDDING )
					u = op->u0 + ( ( frame / 2 ) % 32 );
					
				if ( mesh->flags & MINUSSLIDDING )
					u = op->u0 - ( ( frame / 2 ) % 32 );

				v = op->v0;
			}
			else if ( mesh->flags & VSLIDDING )
			{
				if ( mesh->flags & PLUSSLIDDING )
					v = op->v0 + ( ( frame / 2 ) % 32 );

				if ( mesh->flags & MINUSSLIDDING )
					v = op->v0 - ( ( frame / 2 ) % 32 );

				u = op->u0;
			}
			else
			{
				u = op->u0;
				v = op->v0;
			}

			if ( mesh->flags & MODGY )
				t1 = WATERANIM_1A;
			else
				t1 = WATERANIM_1;


//			if ( mesh->flags & ADDATIVE )
//				t1 |= ( op->clut << 16 ) | WATER_TRANS_CLUT;
//			else
//				t1 |= ( op->clut << 16 ) | 0;

			if ( mesh->flags & USLIDDING )
			{
				if ( mesh->flags & PLUSSLIDDING )
					u = op->u1 + ( ( frame / 2 ) % 32 );

				if ( mesh->flags & MINUSSLIDDING )
					u = op->u1 - ( ( frame / 2 ) % 32 );
					
				v = op->v1;
			}
			else if ( mesh->flags & VSLIDDING )
			{
				if ( mesh->flags & PLUSSLIDDING )
					v = op->v1 + ( ( frame / 2 ) % 32 );

				if ( mesh->flags & MINUSSLIDDING )
					v = op->v1 - ( ( frame / 2 ) % 32 );
					
				u = op->u1;
			}
			else
			{
				u = op->u1;
				v = op->v1;
			}

			if ( mesh->flags & MODGY )
			{
				t2 = WATERANIM_2B;// | ( op->tpage << 16 );
			}
			else
			{
				t2 = WATERANIM_1;// | ( op->tpage << 16 );
			}
			// ENDELSEIF

			u0 = t1;
			u1 = t2;

//			gte_stsxy3_gt4 ( si );	// The first 3 x's & y's are already in the gte, so we may as well use 'em

			if ( mesh->flags & USLIDDING )
			{
				if ( mesh->flags & PLUSSLIDDING )
					u = op->u2 + ( ( frame / 2 ) % 32 );

				if ( mesh->flags & MINUSSLIDDING )
					u = op->u2 - ( ( frame / 2 ) % 32 );
					
				v = op->v2;
			}
			else if ( mesh->flags & VSLIDDING )
			{
				if ( mesh->flags & PLUSSLIDDING )
					v = op->v2 + ( ( frame / 2 ) % 32 );

				if ( mesh->flags & MINUSSLIDDING )
					v = op->v2 - ( ( frame / 2 ) % 32 );
					
				u = op->u2;
			}
			else
			{
				u = op->u2;
				v = op->v2;
			}

			if ( mesh->flags & MODGY )
				t1 = WATERANIM_1A;
			else
				t1 = WATERANIM_2;

			if ( mesh->flags & USLIDDING )
			{
				if ( mesh->flags & PLUSSLIDDING )
					u = op->u3 + ( ( frame / 2 ) % 32 );

				if ( mesh->flags & MINUSSLIDDING )
					u = op->u3 - ( ( frame / 2 ) % 32 );
					
				v = op->v3;
			}
			else if ( mesh->flags & VSLIDDING )
			{
				if ( mesh->flags & PLUSSLIDDING )
					v = op->v3 + ( ( frame / 2 ) % 32 );

				if ( mesh->flags & MINUSSLIDDING )
					v = op->v3 - ( ( frame / 2 ) % 32 );
					
				u = op->u3;
			}
			else
			{
				u = op->u3;
				v = op->v3;
			}

			if ( mesh->flags & MODGY )
			{
				t2 = WATERANIM_2B;
			}
			else
			{
				t2 = WATERANIM_2;
			}
			
			u2 = t1;
			u3 = t2;

//			*(u_long *)  (&si->x3) = *(u_long *) ( &GETV ( op->vert3 ) );

//			if ( mesh->flags & ADDATIVE )
//				t1 = ( *( u_long *) ( &op->r0 ) ) | WATER_TRANS_CODE;
//			else
//				t1 = ( *( u_long *) ( &op->r0 ) ) | 0;
			// ENDIF

//			t2 = *(u_long *) (&op->r1);

//			*(u_long *)  (&si->r0) = t1;
//			*(u_long *)  (&si->r1) = t2;

//			t1 = *(u_long *) (&op->r2);
//			t2 = *(u_long *) (&op->r3);

//			*(u_long *)  (&si->r2) = t1;
//			*(u_long *)  (&si->r3) = t2;
*/

//			op->tpage = texID;
//			my_setPolyFMA_GT4_Trans(si, op, 128);

			vertices_GT4_FMA_Trans[0].fX = x0;
			vertices_GT4_FMA_Trans[0].fY = y0;
			vertices_GT4_FMA_Trans[0].u.fZ = 1.0 / ((float)z0);
//			vertices_GT4_FMA_Trans[0].fU = u0 / 63.0;		
//			vertices_GT4_FMA_Trans[0].fV = op->v0 / 127.0;
			vertices_GT4_FMA_Trans[0].fU = 1;
			vertices_GT4_FMA_Trans[0].fV = 0;
			vertices_GT4_FMA_Trans[0].uBaseRGB.dwPacked = RGBA(op->r0,op->g0,op->b0,128);
//			vertices_GT4_FMA_Trans[0].uBaseRGB.dwPacked = RGBA(255,255,255,255);

			vertices_GT4_FMA_Trans[1].fX = x1;
			vertices_GT4_FMA_Trans[1].fY = y1;
			vertices_GT4_FMA_Trans[1].u.fZ = 1.0 / ((float)z1);
//			vertices_GT4_FMA_Trans[1].fU = u1 / 63.0;
//			vertices_GT4_FMA_Trans[1].fV = op->v1 / 127.0;
			vertices_GT4_FMA_Trans[1].fU = 0;
			vertices_GT4_FMA_Trans[1].fV = 0;
			vertices_GT4_FMA_Trans[1].uBaseRGB.dwPacked = RGBA(op->r1,op->g1,op->b1,128);
//			vertices_GT4_FMA_Trans[1].uBaseRGB.dwPacked = RGBA(255,255,255,255);

			vertices_GT4_FMA_Trans[2].fX = x2;
			vertices_GT4_FMA_Trans[2].fY = y2;
			vertices_GT4_FMA_Trans[2].u.fZ = 1.0 / ((float)z2);
//			vertices_GT4_FMA_Trans[2].fU = u2 / 63.0;
//			vertices_GT4_FMA_Trans[2].fV = op->v2 / 127.0;
			vertices_GT4_FMA_Trans[2].fU = 1;
			vertices_GT4_FMA_Trans[2].fV = 1;
			vertices_GT4_FMA_Trans[2].uBaseRGB.dwPacked = RGBA(op->r2,op->g2,op->b2,128);
//			vertices_GT4_FMA_Trans[2].uBaseRGB.dwPacked = RGBA(255,255,255,255);

			vertices_GT4_FMA_Trans[3].fX = x3;
			vertices_GT4_FMA_Trans[3].fY = y3;
			vertices_GT4_FMA_Trans[3].u.fZ = 1.0 / ((float)z3);
//			vertices_GT4_FMA_Trans[3].fU = u3 / 63.0;
//			vertices_GT4_FMA_Trans[3].fV = op->v3 / 127.0;
			vertices_GT4_FMA_Trans[3].fU = 0;
			vertices_GT4_FMA_Trans[3].fV = 1;
			vertices_GT4_FMA_Trans[3].uBaseRGB.dwPacked = RGBA(op->r3,op->g3,op->b3,128);
//			vertices_GT4_FMA_Trans[3].uBaseRGB.dwPacked = RGBA(255,255,255,255);

			kmStartStrip(&vertexBufferDesc, &StripHead_GT4_FMA_Trans);
			kmSetVertex(&vertexBufferDesc, &vertices_GT4_FMA_Trans[0], KM_VERTEXTYPE_03, sizeof(KMVERTEX_03));
			kmSetVertex(&vertexBufferDesc, &vertices_GT4_FMA_Trans[1], KM_VERTEXTYPE_03, sizeof(KMVERTEX_03));
			kmSetVertex(&vertexBufferDesc, &vertices_GT4_FMA_Trans[2], KM_VERTEXTYPE_03, sizeof(KMVERTEX_03));	
			kmSetVertex(&vertexBufferDesc, &vertices_GT4_FMA_Trans[3], KM_VERTEXTYPE_03, sizeof(KMVERTEX_03));	
			kmEndStrip(&vertexBufferDesc);			
		}
	}

#undef si
#undef op


#define si ((POLY_GT3*)packet)
#define op ((FMA_GT3 *)opcd)

	op = mesh->gt3s;
	for ( i = mesh->n_gt3s; i != 0; i--, op++ )
	{
		char u,v;

		x0 = GETX(op->vert0); y0 = GETY(op->vert0); z0 = GETD(op->vert0);
		x1 = GETX(op->vert1); y1 = GETY(op->vert1); z1 = GETD(op->vert1);
		x2 = GETX(op->vert2); y2 = GETY(op->vert2); z2 = GETD(op->vert2);

		if((x0 > 640)&&(x1 > 640)&&(x2 > 640))
			continue;
		if((y0 > 480)&&(y1 > 480)&&(y2 > 480))
			continue;

		if((x0 < 0)&&(x1 < 0)&&(x2 < 0))
			continue;
		if((y0 < 0)&&(y1 < 0)&&(y2 < 0))
			continue;		

		gte_ldsz3(z0,z1,z2);
	   	gte_avsz3();
		gte_stotz_cpu ( depth );
		
		if(depth > min_depth && depth < max_depth)
		{

			vertices_GT3_FMA_Trans[0].fX = x0;
			vertices_GT3_FMA_Trans[0].fY = y0;
			vertices_GT3_FMA_Trans[0].u.fZ = 1.0 / ((float)z0);
//			vertices_GT3_FMA_Trans[0].fU = op->u0 / 127.0;		
//			vertices_GT3_FMA_Trans[0].fV = op->v0 / 127.0;
			vertices_GT3_FMA_Trans[0].fU = 1;
			vertices_GT3_FMA_Trans[0].fV = 0;
			vertices_GT3_FMA_Trans[0].uBaseRGB.dwPacked = RGBA(op->r0,op->g0,op->b0,128);
//			vertices_GT3_FMA_Trans[0].uBaseRGB.dwPacked = RGBA(255,255,255,255);

			vertices_GT3_FMA_Trans[1].fX = x1;
			vertices_GT3_FMA_Trans[1].fY = y1;
			vertices_GT3_FMA_Trans[1].u.fZ = 1.0 / ((float)z1);
//			vertices_GT3_FMA_Trans[1].fU = op->u1 / 127.0;
//			vertices_GT3_FMA_Trans[1].fV = op->v1 / 127.0;
			vertices_GT3_FMA_Trans[1].fU = 0;
			vertices_GT3_FMA_Trans[1].fV = 0;
			vertices_GT3_FMA_Trans[1].uBaseRGB.dwPacked = RGBA(op->r1,op->g1,op->b1,128);
//			vertices_GT3_FMA_Trans[1].uBaseRGB.dwPacked = RGBA(255,255,255,255);

			vertices_GT3_FMA_Trans[2].fX = x2;
			vertices_GT3_FMA_Trans[2].fY = y2;
			vertices_GT3_FMA_Trans[2].u.fZ = 1.0 / ((float)z2);
//			vertices_GT3_FMA_Trans[2].fU = op->u2 / 127.0;
//			vertices_GT3_FMA_Trans[2].fV = op->v2 / 127.0;
			vertices_GT3_FMA_Trans[2].fU = 1;
			vertices_GT3_FMA_Trans[2].fV = 1;
			vertices_GT3_FMA_Trans[2].uBaseRGB.dwPacked = RGBA(op->r2,op->g2,op->b2,128);
//			vertices_GT3_FMA_Trans[2].uBaseRGB.dwPacked = RGBA(255,255,255,255);

			kmStartStrip(&vertexBufferDesc, &StripHead_GT3_FMA_Trans);
			kmSetVertex(&vertexBufferDesc, &vertices_GT3_FMA_Trans[0], KM_VERTEXTYPE_03, sizeof(KMVERTEX_03));
			kmSetVertex(&vertexBufferDesc, &vertices_GT3_FMA_Trans[1], KM_VERTEXTYPE_03, sizeof(KMVERTEX_03));
			kmSetVertex(&vertexBufferDesc, &vertices_GT3_FMA_Trans[2], KM_VERTEXTYPE_03, sizeof(KMVERTEX_03));	
			kmEndStrip(&vertexBufferDesc);						
		}
		
	}

#undef si
#undef op

}


void CreateAndAddScenicObject(SCENIC *sc)
{
	SCENICOBJ *newItem;

	newItem = ( SCENICOBJ* ) MALLOC0 ( sizeof ( SCENICOBJ ) );

	AddScenicObj ( newItem );

	newItem->flags = 0;

	utilPrintf("SCENIC %s, ROT %d %d %d %d\n", sc->name, sc->rot.x, sc->rot.y, sc->rot.z, sc->rot.w);

	QuatToPSXMatrix(&sc->rot, &newItem->matrix);

	newItem->matrix.t[0] =  sc->pos.vx;
	newItem->matrix.t[1] =  sc->pos.vy;
	newItem->matrix.t[2] =  sc->pos.vz;
	
	utilPrintf("Creating Scenic Object : %s\n", sc->name);

	newItem->fmaObj = ( void * ) BFF_FindObject ( BFF_FMA_WORLD_ID, utilStr2CRC ( sc->name ) );

	if ( newItem->fmaObj )
		SetUpWaterMesh ( newItem->fmaObj );
	else
		utilPrintf("Could Not Create Scenic Object.\n");

	if( sc->name[0] == 'S' && sc->name[1] == 'P')
	{
		switch( sc->name[2] )
		{
			case 'L': newItem->flags |= SLIDESPEED; break;
			case 'M': newItem->flags |= SLIDESPEED2; break;
			case 'F': newItem->flags |= (SLIDESPEED | SLIDESPEED2); break;
		}
	}
}


void DrawScenicObjList ( void )
{
	int i;

	SCENICOBJ *cur;

	FMA_MESH_HEADER **mesh;


	if ( scenicObjList.numEntries == 0 )
		return;

	for ( cur = scenicObjList.head.next; cur != &scenicObjList.head; cur = cur->next)
	{

		if ( cur->fmaObj )
		{
			mesh = ADD2POINTER(cur->fmaObj,sizeof(FMA_WORLD));

			for ( i = cur->fmaObj->n_meshes; i != 0; i--, mesh++ )
			{
				MapDraw_SetScenicMatrix(*mesh, cur);
				
				if ( MapDraw_ClipCheck ( *mesh ) )
					DrawScenicObj ( *mesh, cur->flags );
			}
		}
	}
}


void InitScenicObjLinkedList ( void )
{
	scenicObjList.numEntries = 0;
	scenicObjList.head.next = scenicObjList.head.prev = &scenicObjList.head;
}


void AddScenicObj ( SCENICOBJ *scenicObj )
{
	if ( scenicObj->next == NULL )
	{
		scenicObjList.numEntries++;
		scenicObj->prev								= &scenicObjList.head;
		scenicObj->next								= scenicObjList.head.next;
		scenicObjList.head.next->prev	= scenicObj;
		scenicObjList.head.next				=	scenicObj;
	}
}

void FreeScenicObjectList ( void )
{
	SCENICOBJ *cur,*next;

	if( !scenicObjList.numEntries )
		return;

	for ( cur = scenicObjList.head.next; cur != &scenicObjList.head; cur = next )
	{
		next = cur->next;
		SubScenicObject(cur);
	}
//	FREE(specFXList.head);
}


void SubScenicObject ( SCENICOBJ *scenicObj )
{
	if(scenicObj->next == NULL)
		return;

	scenicObj->prev->next = scenicObj->next;
	scenicObj->next->prev = scenicObj->prev;
	scenicObj->next = NULL;
	scenicObjList.numEntries--;

	FREE(scenicObj);
}

unsigned long oldperiod=0, oldfr=0;
void DrawScenicObj ( FMA_MESH_HEADER *mesh, int flags )
{
//	char 			u,v;
	unsigned long 	t1;
	unsigned long 	t2;
	char 			*opcd;
	long 			*tfv;
	long 			*tfd;
	PACKET 			*packet;
	float			x0,y0,x1,y1,x2,y2,x3,y3;
	unsigned long	u0,v0,u1,v1,u2,v2,u3,v3;
	long 			depth;
	long			z0,z1,z2,z3;
	TextureType		*tex;
	unsigned char	alpha;

	#define GETX(n)( ((SHORTXY *)( (int)(tfv) +(n) ))->x )
	#define GETY(n)( ((SHORTXY *)( (int)(tfv) +(n) ))->y )
	#define GETV(n)(  *(unsigned long *)( (int)(tfv) +(n) ) )
	#define GETD(n)(  *(unsigned long *)( (int)(tfd) +(n) ) )

	int i, j;

	int min_depth = MIN_MAP_DEPTH;// + mesh->extra_depth;
	int max_depth = fog.max>>2;//MAX_MAP_DEPTH;// + mesh->extra_depth;

	// Store UV modge vals for a poly
	short mVs[8];

	// Is this an sp(m/f/l) type slidy thing? If so, what speed?
	unsigned short pcStyleSlide;
	int sldSpd;
	unsigned long period1, period2, fr;

	if((gameState.mode != PAUSE_MODE ) && (gameState.mode != GAMEOVER_MODE))
	{
		pcStyleSlide = ((flags>>10)&3);

		period1 = frameCount<<5; period2 = frameCount<<7;
		oldperiod = period1;

		fr = frame<<5;

		// Multiply speed if we have a multiple
		if( pcStyleSlide )
			sldSpd = ((frame>>1)*slideSpeeds[pcStyleSlide])%32;
		else
			sldSpd = (frame>>1)%32;

		// Reverse if told to
		if( mesh->flags & MINUSSLIDING )
			sldSpd = -sldSpd;
	}
	else
	{
		sldSpd = 0;
		period1 = oldperiod;
		period2 = oldperiod<<2;
		fr = oldfr;
	}

	tfv = (long*)transformedVertices;
	tfd = (long*)transformedDepths;

	if ( max_depth > 1024 - mesh->extra_depth )
		max_depth = 1024 - mesh->extra_depth;
	
	stripGT3FMAtextureID   = -1;
	stripGT3FMAtextureID_A = -1;
	stripGT4FMAtextureID   = -1;
	stripGT4FMAtextureID_A = -1;
	stripGT4FMAtextureID_Add = -1;
	
	if( mesh->flags & JIGGLE )

	{
		// Interesting. The framecount shift controls the periiod of oscillation,
		// and the downshift at the end decreases the amplitude from huge.
		for ( i = 0; i < mesh->n_verts; i++ )
		{
			jiggledVerts[i].vx = mesh->verts[i].vx + (rcos(period1 + (mesh->verts[i].vx & mesh->verts[i].vz))>>8);
			jiggledVerts[i].vy = mesh->verts[i].vy + (rsin(period2 + (mesh->verts[i].vx ^ mesh->verts[i].vz))>>7);
			jiggledVerts[i].vz = mesh->verts[i].vz + (rcos(period1 + (mesh->verts[i].vx | mesh->verts[i].vz))>>8);
		}


		transformVertexListA ((VERT*)jiggledVerts, mesh->n_verts, tfv, tfd );
	}
	else
	{
		transformVertexListA((VERT*)mesh->verts, mesh->n_verts, tfv, tfd);
	}

	// determine alpha value for the mesh
	alpha = 255;
//	if((mesh->flags & ADDITIVE) || (mesh->flags & SUBTRACTIVE))
//		alpha = 128;		

#define si ((POLY_GT4*)packet)
#define op ((FMA_GT4 *)opcd)

	op = mesh->gt4s;
	
//	packet = (PACKET *)currentDisplayPage->primPtr;

	for ( i = mesh->n_gt4s; i != 0; i--, op++ )
	{
		x0 = GETX(op->vert0);
		x1 = GETX(op->vert1);
		x2 = GETX(op->vert2);
		x3 = GETX(op->vert3);

		if((x0 > 640)&&(x1 > 640)&&(x2 > 640)&&(x3 > 640))
			continue;
		if((x0 < 0)&&(x1 < 0)&&(x2 < 0)&&(x3 < 0))
			continue;

		y0 = GETY(op->vert0);
		y1 = GETY(op->vert1);
		y2 = GETY(op->vert2);
		y3 = GETY(op->vert3);
			
		if((y0 > 480)&&(y1 > 480)&&(y2 > 480)&&(y3 > 480))
			continue;
		if((y0 < 0)&&(y1 < 0)&&(y2 < 0)&&(y3 < 0))
			continue;
	
		// Get average Z of the polygon & use as a depth pointer
		z0 = GETD(op->vert0);
		z1 = GETD(op->vert1);
		z2 = GETD(op->vert2);
		z3 = GETD(op->vert3);
		depth = ((z0 + z1 + z2 + z3)*0.0625);

		if(depth > min_depth && depth < max_depth)
		{
			tex = &DCKtextureList[op->tpage];
						
			// Precalc modge values for this quad
			if ( mesh->flags & MODGY )
			{
				int vv;
				for( j=3; j>=0; j-- )
				{
					vv = fr + (mesh->verts[*(&op->vert0+j)].vx * mesh->verts[*(&op->vert0+j)].vz);
					mVs[j<<1]		= rsin(vv)>>10;
					mVs[(j<<1)+1]	= rcos(vv)>>10;
				}
			}

			u0 = op->u0;		// Texture coords
			v0 = op->v0;
			u1 = op->u1;		// Texture coords
			v1 = op->v1;

			if( mesh->flags & USLIDING )
			{
				u0 += sldSpd*2;
				u1 += sldSpd*2;
			}
			if( pcStyleSlide || (mesh->flags & VSLIDING) )
			{
				v0 += sldSpd*2;
				v1 += sldSpd*2;
			}

			if ( mesh->flags & MODGY )
			{
				u0 += mVs[0];
				v0 += mVs[1];
				u1 += mVs[2];
				v1 += mVs[3];
			}
			
			u2 = op->u2;		// Texture coords
			v2 = op->v2;
			u3 = op->u3;		// Texture coords
			v3 = op->v3;
								
			if ( mesh->flags & USLIDING )
			{
				u2 += sldSpd*2;
				u3 += sldSpd*2;
			}
			if( pcStyleSlide || (mesh->flags & VSLIDING) )
			{
				v2 += sldSpd*2;
				v3 += sldSpd*2;
			}

			if ( mesh->flags & MODGY )
			{
				u2 += mVs[4];
				v2 += mVs[5];
				u3 += mVs[6];
				v3 += mVs[7];
			}

			vertices_GT4_FMA[0].fX = x0;
			vertices_GT4_FMA[0].fY = y0;
			vertices_GT4_FMA[0].u.fZ = 1.0 / ((float)z0);
			vertices_GT4_FMA[0].fU = (float)u0 / 127.0;
			vertices_GT4_FMA[0].fV = (float)v0 / 127.0;
			vertices_GT4_FMA[0].uBaseRGB.dwPacked = RGBA(op->r0,op->g0,op->b0,alpha);

			vertices_GT4_FMA[1].fX = x1;
			vertices_GT4_FMA[1].fY = y1;
			vertices_GT4_FMA[1].u.fZ = 1.0 / ((float)z1);
			vertices_GT4_FMA[1].fU = (float)u1 / 127.0;
			vertices_GT4_FMA[1].fV = (float)v1 / 127.0;
			vertices_GT4_FMA[1].uBaseRGB.dwPacked = RGBA(op->r1,op->g1,op->b1,alpha);

			vertices_GT4_FMA[2].fX = x2;
			vertices_GT4_FMA[2].fY = y2;
			vertices_GT4_FMA[2].u.fZ = 1.0 / ((float)z2);
			vertices_GT4_FMA[2].fU = (float)u2 / 127.0;
			vertices_GT4_FMA[2].fV = (float)v2 / 127.0;
			vertices_GT4_FMA[2].uBaseRGB.dwPacked = RGBA(op->r2,op->g2,op->b2,alpha);

			vertices_GT4_FMA[3].fX = x3;
			vertices_GT4_FMA[3].fY = y3;
			vertices_GT4_FMA[3].u.fZ = 1.0 / ((float)z3);
			vertices_GT4_FMA[3].fU = (float)u3 / 127.0;
			vertices_GT4_FMA[3].fV = (float)v3 / 127.0;		
			vertices_GT4_FMA[3].uBaseRGB.dwPacked = RGBA(op->r3,op->g3,op->b3,alpha);

			if((tex->animated)/*||(alpha!=255)*/||(op->pad2 & 32)||(op->pad2 & 64)||(mesh->flags & ADDITIVE))
			{
				// check to see if alpha channel is to be used
				if((tex->colourKey)/*||(alpha!=255)*/)
				{
					// change strip if required
					if(op->tpage != stripGT4FMAtextureID_A)
					{
						kmChangeStripTextureSurface(&StripHead_GT4_FMA_Alpha,KM_IMAGE_PARAM1,tex->surfacePtr);
						stripGT4FMAtextureID_A = op->tpage;
					}
					kmStartStrip(&vertexBufferDesc, &StripHead_GT4_FMA_Alpha);					
				}	
				else
				{
					if((op->pad2 & 32)||(op->pad2 & 64))
					{
						// change strip if required
						if(op->tpage != stripGT4FMAtextureID_Add)
						{

							kmChangeStripTextureSurface(&StripHead_GT4_FMA_Add,KM_IMAGE_PARAM1,tex->surfacePtr);		
							stripGT4FMAtextureID_Add = op->tpage;
						}
						kmStartStrip(&vertexBufferDesc, &StripHead_GT4_FMA_Add);	
					}
					else
					{
						// change strip if required
						if(op->tpage != stripGT4FMAtextureID)
						{
							kmChangeStripTextureSurface(&StripHead_GT4_FMA,KM_IMAGE_PARAM1,tex->surfacePtr);		
							stripGT4FMAtextureID = op->tpage;
						}
						kmStartStrip(&vertexBufferDesc, &StripHead_GT4_FMA);	
					}
				}	
			}
			else
			{
				kmStartStrip(&vertexBufferDesc, &tex->stripHead);	
			}
			
			kmSetVertex(&vertexBufferDesc, &vertices_GT4_FMA[0], KM_VERTEXTYPE_03, sizeof(KMVERTEX_03));
			kmSetVertex(&vertexBufferDesc, &vertices_GT4_FMA[1], KM_VERTEXTYPE_03, sizeof(KMVERTEX_03));
			kmSetVertex(&vertexBufferDesc, &vertices_GT4_FMA[2], KM_VERTEXTYPE_03, sizeof(KMVERTEX_03));	
			kmSetVertex(&vertexBufferDesc, &vertices_GT4_FMA[3], KM_VERTEXTYPE_03, sizeof(KMVERTEX_03));	
			kmEndStrip(&vertexBufferDesc);			

		}
	}
#undef op
#undef si

#define si ((POLY_GT3*)packet)
#define op ((FMA_GT3 *)opcd)

	op = mesh->gt3s;
	for ( i = mesh->n_gt3s; i != 0; i--, op++ )
	{
		x0 = GETX(op->vert0);
		x1 = GETX(op->vert1);
		x2 = GETX(op->vert2);

		if((x0 > 640)&&(x1 > 640)&&(x2 > 640))
			continue;
		if((x0 < 0)&&(x1 < 0)&&(x2 < 0))
			continue;

		y0 = GETY(op->vert0);
		y1 = GETY(op->vert1);
		y2 = GETY(op->vert2);
			
		if((y0 > 480)&&(y1 > 480)&&(y2 > 480))
			continue;
		if((y0 < 0)&&(y1 < 0)&&(y2 < 0))
			continue;

		// Get average Z of the polygon & use as a depth pointer
		z0 = GETD(op->vert0);
		z1 = GETD(op->vert1);
		z2 = GETD(op->vert2);
		depth = ((z0 + z1 + z2)*0.0833);
		
		if(depth > min_depth && depth < max_depth)
		{
			tex = &DCKtextureList[op->tpage];
						
			// Precalc modge values for this quad
			if ( mesh->flags & MODGY )
			{
				int vv;
				for( j=2; j>=0; j-- )
				{
					vv = fr+(mesh->verts[*(&op->vert0+j)].vx * mesh->verts[*(&op->vert0+j)].vz);
					mVs[j<<1]		= rsin(vv)>>10;
					mVs[(j<<1)+1]	= rcos(vv)>>10;
				}
			}

			u0 = op->u0;		// Texture coords
			v0 = op->v0;
			u1 = op->u1;		// Texture coords
			v1 = op->v1;

			if( mesh->flags & USLIDING )
			{
				u0 += sldSpd*2;
				u1 += sldSpd*2;
			}
			if( pcStyleSlide || (mesh->flags & VSLIDING) )
			{
				v0 += sldSpd*2;
				v1 += sldSpd*2;
			}

			if ( mesh->flags & MODGY )
			{
				u0 += mVs[0];
				v0 += mVs[1];
				u1 += mVs[2];
				v1 += mVs[3];
			}
			
			u2 = op->u2;		// Texture coords
			v2 = op->v2;
								
			if ( mesh->flags & USLIDING )
				u2 += sldSpd*2;
			if( pcStyleSlide || (mesh->flags & VSLIDING) )
				v2 += sldSpd*2;

			if ( mesh->flags & MODGY )
			{
				u2 += mVs[4];
				v2 += mVs[5];
			}

			vertices_GT3_FMA[0].fX = x0;
			vertices_GT3_FMA[0].fY = y0;
			vertices_GT3_FMA[0].u.fZ = 1.0 / ((float)z0);
			vertices_GT3_FMA[0].fU = u0 / 127.0;
			vertices_GT3_FMA[0].fV = v0 / 127.0;
			vertices_GT3_FMA[0].uBaseRGB.dwPacked = RGBA(op->r0,op->g0,op->b0,alpha);

			vertices_GT3_FMA[1].fX = x1;
			vertices_GT3_FMA[1].fY = y1;
			vertices_GT3_FMA[1].u.fZ = 1.0 / ((float)z1);
			vertices_GT3_FMA[1].fU = u1 / 127.0;
			vertices_GT3_FMA[1].fV = v1 / 127.0;
			vertices_GT3_FMA[1].uBaseRGB.dwPacked = RGBA(op->r1,op->g1,op->b1,alpha);

			vertices_GT3_FMA[2].fX = x2;
			vertices_GT3_FMA[2].fY = y2;
			vertices_GT3_FMA[2].u.fZ = 1.0 / ((float)z2);
			vertices_GT3_FMA[2].fU = u2 / 127.0;
			vertices_GT3_FMA[2].fV = v2 / 127.0;
			vertices_GT3_FMA[2].uBaseRGB.dwPacked = RGBA(op->r2,op->g2,op->b2,alpha);

			if((tex->animated)/*||(alpha!=255)*/||(op->pad2 & 32)||(op->pad2 & 64))
			{
				// check to see if alpha channel is to be used
				if((tex->colourKey)/*||(alpha!=255)*/)
				{
					// change strip if required		
					if(op->tpage != stripGT3FMAtextureID_A)
					{
						kmChangeStripTextureSurface(&StripHead_GT3_FMA_Alpha,KM_IMAGE_PARAM1,tex->surfacePtr);
						stripGT3FMAtextureID_A = op->tpage;
					}
					kmStartStrip(&vertexBufferDesc, &StripHead_GT3_FMA_Alpha);
				}		
				else
				{
					if((op->pad2 & 32)||(op->pad2 & 64))
					{
						// change strip if required
						if(op->tpage != stripGT4FMAtextureID_Add)
						{

							kmChangeStripTextureSurface(&StripHead_GT4_FMA_Add,KM_IMAGE_PARAM1,tex->surfacePtr);		
							stripGT4FMAtextureID_Add = op->tpage;
						}
						kmStartStrip(&vertexBufferDesc, &StripHead_GT4_FMA_Add);	
					}
					else
					{				
						// change strip if required					
						if(op->tpage != stripGT3FMAtextureID)
						{
							kmChangeStripTextureSurface(&StripHead_GT3_FMA,KM_IMAGE_PARAM1,tex->surfacePtr);
							stripGT3FMAtextureID = op->tpage;
						}
						kmStartStrip(&vertexBufferDesc, &StripHead_GT3_FMA);	
					}
				}	
			}
			else
			{
				kmStartStrip(&vertexBufferDesc, &tex->stripHead);	
			}
	
			kmSetVertex(&vertexBufferDesc, &vertices_GT3_FMA[0], KM_VERTEXTYPE_03, sizeof(KMVERTEX_03));
			kmSetVertex(&vertexBufferDesc, &vertices_GT3_FMA[1], KM_VERTEXTYPE_03, sizeof(KMVERTEX_03));	
			kmSetVertex(&vertexBufferDesc, &vertices_GT3_FMA[2], KM_VERTEXTYPE_03, sizeof(KMVERTEX_03));	
			kmEndStrip(&vertexBufferDesc);

		}
	}
#undef op
#undef si

//	return;
	
#define si ((POLY_FT4*)packet)
#define op ((FMA_SPR *)opcd)

	op = mesh->sprs;

	for(i = mesh->n_sprs; i != 0; i--,op++)
	{
		LONG spritez, width, height;
		SVECTOR tempVect;

//		BEGINPRIM(si, POLY_FT4);

//		setPolyFT4(si);

		tempVect.vx = -op->x;
		tempVect.vy = -op->y;
		tempVect.vz = -op->z;

		width = 32;

		gte_SetLDDQB(0);		// clear offset control reg (C2_DQB)
		gte_ldv0(&tempVect);
		gte_SetLDDQA(width);	// shove sprite width into control reg (C2_DQA)
		gte_rtps();				// do the rtps
//		gte_stsxy(&si->x0);		// get screen x and y
		x0 = screenxy[2].vx;
		y0 = screenxy[2].vy;
		spritez = screenxy[2].vz;
//ma	gte_stsz(&spritez);		// get screen z

		if ( spritez <= 0 || spritez >= fog.max ) 
			continue;

		width = ((op->w * SCALEX * fmaActorScale.vx)>>12) / spritez;
		if(width < 2 || width > 256)
			continue;

 		x1 = x3 = x0 + width;
 		x0 = x2 = x0 - width;
   		if(x1 <= 0)
   			continue;
   		if(x0 >= 640)
   			continue;

		height = ((op->h * SCALEY * fmaActorScale.vx)>>12) / spritez;

		y2 = y3 = y0 + height;
		y0 = y1 = y0 - height;
		
   		if(y2 <= 0)
   			continue;
   		if(y0 >= 480)
   			continue;

		spritez >>= 2;
		vertices_GT4_FMA[0].fX = x0;
		vertices_GT4_FMA[0].fY = y0;
		vertices_GT4_FMA[0].u.fZ = 1.0 / spritez;
		vertices_GT4_FMA[0].fU = 0;
		vertices_GT4_FMA[0].fV = 0;
		vertices_GT4_FMA[0].uBaseRGB.dwPacked = RGBA(op->r0,op->g0,op->b0,255);

		vertices_GT4_FMA[1].fX = x1;
		vertices_GT4_FMA[1].fY = y1;
		vertices_GT4_FMA[1].u.fZ = 1.0 / spritez;
		vertices_GT4_FMA[1].fU = 1;
		vertices_GT4_FMA[1].fV = 0;
		vertices_GT4_FMA[1].uBaseRGB.dwPacked = RGBA(op->r0,op->g0,op->b0,255);

		vertices_GT4_FMA[2].fX = x2;
		vertices_GT4_FMA[2].fY = y2;
		vertices_GT4_FMA[2].u.fZ = 1.0 / spritez;
		vertices_GT4_FMA[2].fU = 0;
		vertices_GT4_FMA[2].fV = 1;
		vertices_GT4_FMA[2].uBaseRGB.dwPacked = RGBA(op->r0,op->g0,op->b0,255);

		vertices_GT4_FMA[3].fX = x3;
		vertices_GT4_FMA[3].fY = y3;
		vertices_GT4_FMA[3].u.fZ = 1.0 / spritez;
		vertices_GT4_FMA[3].fU = 1;
		vertices_GT4_FMA[3].fV = 1;		
		vertices_GT4_FMA[3].uBaseRGB.dwPacked = RGBA(op->r0,op->g0,op->b0,255);

//ma	kmStartStrip(&vertexBufferDesc, &DCKtextureList[op->tpage].stripHead);	

		kmChangeStripTextureSurface(&StripHead_Sprites,KM_IMAGE_PARAM1,DCKtextureList[op->tpage].surfacePtr);
		kmStartStrip(&vertexBufferDesc, &StripHead_Sprites);	

		kmSetVertex(&vertexBufferDesc, &vertices_GT4_FMA[0], KM_VERTEXTYPE_03, sizeof(KMVERTEX_03));
		kmSetVertex(&vertexBufferDesc, &vertices_GT4_FMA[1], KM_VERTEXTYPE_03, sizeof(KMVERTEX_03));
		kmSetVertex(&vertexBufferDesc, &vertices_GT4_FMA[2], KM_VERTEXTYPE_03, sizeof(KMVERTEX_03));	
		kmSetVertex(&vertexBufferDesc, &vertices_GT4_FMA[3], KM_VERTEXTYPE_03, sizeof(KMVERTEX_03));	
		kmEndStrip(&vertexBufferDesc);				
		
	}

#undef si
#undef op

//	currentDisplayPage->primPtr = (char *)packet;
}

void PTTextureLoad ( void )
{
/*	CreateProceduralTexture ( "PRC_FIRE1" );		

	CreateProceduralTexture ( "PRC_WATRD1" );		

	CreateProceduralTexture ( "PRC_WATRT1" );		
//	CreateProceduralTexture ( "00WATE04" );*/

}

void CreateProceduralTexture ( char *name )
{
	unsigned long i;
//	unsigned long rVand,gVand,bVand,rVshr,gVshr,bVshr;
	unsigned short newCol,nR,nG,nB;//,nA;
	unsigned short *tempPalette;

//	unsigned char *tempBuf;

//	int counter;

	RECT texRect;


	PROCTEXTURE *pt = (PROCTEXTURE *)MALLOC0( sizeof(PROCTEXTURE) );

	// Set proc texture members
	if ( !(pt->tex = textureFindCRCInAllBanks ( utilStr2CRC ( name ) ) ) )
	{
		utilPrintf("Could Not Find Texture : %s\n", name);
		FREE ( pt );
		return;
	}
	// ENDIF

	// Add to procedural text list
	pt->next = prcTexList;
	prcTexList = pt;

	pt->buf1 = (unsigned char *)MALLOC0( 2048 );
	pt->buf2 = (unsigned char *)MALLOC0( 2048 );
	pt->palette = (unsigned short *)MALLOC0( 512 );

	tempPalette = (unsigned short *)MALLOC0( 512 );

	pt->active = 1;

	texRect.x = VRAM_CALCVRAMX(pt->tex->handle);
	texRect.y = VRAM_CALCVRAMY(pt->tex->handle);
	texRect.w = 16;
	texRect.h = 32;

	DrawSync(0);
//MA	StoreImage ( &texRect, (unsigned long*)pt->buf1 );
	DrawSync(0);

	
//MA	StoreImage ( &texRect, (unsigned long*)pt->buf2 );
	DrawSync(0);

	texRect.x = (pt->tex->clut & 0x3f) << 4;
	texRect.y = pt->tex->clut >> 6;
	texRect.w = 256;
	texRect.h = 1;

	DrawSync(0);
//MA	StoreImage ( &texRect, (unsigned long*)pt->palette );
	DrawSync(0);

	for( i=0; i<256; i++ )
	{
		nB = ( pt->palette [ pt->buf1 [ i ] ] >> 10 ) & 31;
		nG = ( pt->palette [ pt->buf1 [ i ] ] >> 5  ) & 31;
		nR =   pt->palette [ pt->buf1 [ i ] ] & 31;

		newCol = ( ( ( nB ) << 10 ) | ( ( nG ) << 5 ) | ( nR ) );
		
		((unsigned short *)tempPalette)[i] = newCol;
	}
			
//MA	LoadClut ( (unsigned long*)tempPalette, (pt->tex->clut & 0x3f) << 4, pt->tex->clut >> 6 );
	
	// Set update function and type depending on filename
	if( name[4]=='F' && name[5]=='I' && name[6]=='R' && name[7]=='E' )
		pt->Update = ProcessPTFire;
	else if( name[4]=='f' && name[5]=='f' && name[6]=='l' && name[7]=='d' )
		pt->Update = ProcessPTForcefield;
	else if( name[4]=='w' && name[5]=='a' && name[6]=='t' && name[7]=='r' )
	{
		if( name[8]=='r' )
			pt->Update = ProcessPTWaterRipples;
		else if( name[8]=='d' )
			pt->Update = ProcessPTWaterDrops;
		else if( name[8]=='b' )
			pt->Update = ProcessPTWaterBubbler;
		else if( name[8]=='t' )
			pt->Update = ProcessPTWaterTrail;
		else if( name[8]=='w' )
			pt->Update = ProcessPTWaterWaves;
	}
	else if( name[4]=='s' && name[5]=='t' && name[6]=='e' && name[7]=='a' )
	{
		pt->Update = ProcessPTSteam;
//		pt->active = 0;
//		steamTex = pt;
	}
	else if( name[4]=='c' && name[5]=='n' && name[6]=='d' && name[7]=='l' )
		pt->Update = ProcessPTCandle;

	FREE ( tempPalette );
}

void PTSurfaceBlit( TextureType *tex, unsigned long *buf, unsigned short *pal )
{
	RECT texRect;

	int			atbdx,atbdy;

	register PACKET*		packet;

	static TextureType		*tPtr;

	texRect.x = VRAM_CALCVRAMX(tex->handle);
	texRect.y = VRAM_CALCVRAMY(tex->handle);
	texRect.w = tex->w;
	texRect.h = tex->h;
	
//MA	LoadImage ( &texRect, (unsigned long*)buf );



#define si ((POLY_FT4*)packet)

	atbdx = 32;
	atbdy = 32;

	tPtr = tex;

/*ma			BEGINPRIM(si, POLY_FT4);
	
			si->x0 = atbdx;
			si->y0 = atbdy;

			si->x1 = atbdx + tex->w;
			si->y1 = atbdy;

			si->x2 = atbdx;
			si->y2 = atbdy + tex->h;
			//si->y2 = atbdy + (spr->height);

			si->x3 = atbdx + tex->w;
			si->y3 = atbdy + tex->h;

	
			si->r0 = 255;
			si->g0 = 255;
			si->b0 = 255;
	
			si->u0 = tPtr->u0;
			si->v0 = tPtr->v0;
			si->u1 = tPtr->u1;
			si->v1 = tPtr->v1;
			si->u2 = tPtr->u2;
			si->v2 = tPtr->v2;
			si->u3 = tPtr->u3;
			si->v3 = tPtr->v3;
		
			si->code = GPU_COM_TF4;
			si->tpage = tPtr->tpage;
			si->clut = tPtr->clut;
		
			setPolyFT4(si);
			ENDPRIM(si, 1, POLY_FT4);
*/

#undef si

}


unsigned short *realWaterData;
unsigned short *waterData;
unsigned short *reflectionData;

unsigned short *waterPalette;
unsigned short *reflectionPalette;

TextureType *realWater;
TextureType *water;
TextureType *reflection;

void InitWater ( void )
{
/*	int counter;
	RECT rect;

	// JH : Get a pointer to texture that we will over right in VRAM.
	if ( !( realWater = textureFindCRCInAllBanks ( utilStr2CRC ( "00WATE04" ) ) ) )
	{
		// JH : Output debug message if not found.
		utilPrintf("Could Not Find Texture : 00WATE04\n");
		return;
	}
	// ENDIF

	// JH : atempt to find the required texture.
	if ( !( water = textureFindCRCInAllBanks ( utilStr2CRC ( "WATER" ) ) ) )
	{
		// JH : Output debug message if not found.
		utilPrintf("Could Not Find Texture : WATER\n");
		return;
	}
	// ENDIF

	// JH : atempt to find the required texture.
	if ( !( reflection = textureFindCRCInAllBanks ( utilStr2CRC ( "REFLECT" ) ) ) )
	{
		// JH : Output debug message if not found.
		utilPrintf("Could Not Find Texture : REFLECT\n");
		return;
	}
	// ENDIF

	// JH : Make sure we can get both textures before we continue. Malloc the space for each texture,
	// JH : so that we can acces the texture data;
	realWaterData				= (unsigned short *)MALLOC0( 8192 );
	waterData				= (unsigned short *)MALLOC0( 8192 );
	reflectionData	= (unsigned short *)MALLOC0( 8192 );

	waterPalette			= (unsigned char *)MALLOC0( 512 );
	reflectionPalette	= (unsigned char *)MALLOC0( 512 );

	// JH : Make the rectangle of the texture in VRAM.
	rect.x = VRAM_CALCVRAMX ( realWater->handle );
	rect.y = VRAM_CALCVRAMY ( realWater->handle );
	rect.w = 64;
	rect.h = 64;

	// JH : Copy the texture data into a buffer.
	StoreImage ( &rect, (unsigned long*)realWaterData );

	// JH : Make the rectangle of the texture in VRAM.
	rect.x = VRAM_CALCVRAMX ( water->handle );
	rect.y = VRAM_CALCVRAMY ( water->handle );
	rect.w = 64;
	rect.h = 64;

	// JH : Copy the texture data into a buffer.
	StoreImage ( &rect, (unsigned long*)waterData );

	// JH : Make the rectangle of the texture in VRAM.
	rect.x = VRAM_CALCVRAMX ( reflection->handle );
	rect.y = VRAM_CALCVRAMY ( reflection->handle );
	rect.w = 64;
	rect.h = 64;

	// JH : Copy the texture data into a buffer.
	StoreImage ( &rect, (unsigned long*)reflectionData );

	/*//*/ JH : Make the rectangle of the palette in VRAM.
	rect.x = ( water->clut & 0x3f ) << 4;
	rect.y = water->clut >> 6;
	rect.w = 256;
	rect.h = 1;

	// JH : Copy the palette data into a buffer.
//MA	StoreImage ( &rect, (unsigned long*)waterPalette );

//MA	LoadClut ( (unsigned long*)waterPalette, (realWater->clut & 0x3f) << 4, realWater->clut >> 6 );

*/
	// JH : Make the rectangle of the palette in VRAM.
	/*rect.x = ( water->clut & 0x3f ) << 4;
	rect.y = water->clut >> 6;
	rect.w = 256;
	rect.h = 1;

	// JH : Copy the palette data into a buffer.
	StoreImage ( &rect, (unsigned long*)waterPalette );

//	for ( counter = 0; counter < 256; counter++ )
//		reflectionPalette[counter] = ((( reflectionPalette[counter] >> 10 ) & 0x1f) * 12) / 0x1f;

	// JH : Make the rectangle of the palette in VRAM.
	rect.x = ( realWater->clut & 0x3f ) << 4;
	rect.y = realWater->clut >> 6;
	rect.w = 256;
	rect.h = 1;

	// JH : Copy the palette data into a buffer.
	LoadImage ( &rect, (unsigned long*)waterPalette );*/
}


void UpdateWater ( void )
{
	/*long wIndex1,wIndex2;
	long wOffset;

	long x,y,y1,p;

	int counterX, counterY;
	RECT rect;

//	for ( counterY = 0; counterY < 64; counterY++ )
//		for ( counterX = 0; counterX < 64; counterX++ )
//			realWaterData [ counterX + counterY * 64 ] = reflectionData [ counterX + counterY * 64 ];
		
wOffset = 0;;

	for (y=0,y1=0; y<64*64; y+= 64,y1+=64)
		for (x=0; x<64; x++)
		{
			wIndex1 = ((x + wOffset) & (64-1)) + y;
			wIndex2 = ((x + *(reflectionData+wIndex1)) & (64-1)) + y;

			*(realWaterData+x+y1) = *(waterData+wIndex2);
		}

//memcpy ( realWaterData, waterData, 4096 );

	rect.x = VRAM_CALCVRAMX ( realWater->handle );
	rect.y = VRAM_CALCVRAMY ( realWater->handle );
	rect.w = realWater->w;
	rect.h = realWater->h;
	
	LoadImage ( &rect, (unsigned long*)realWaterData );
*/
}

// For space - NOT a bitmap!
void PrintStaticBackdrop( FMA_MESH_HEADER *mesh )
{
	float			x0,y0,x1,y1,x2,y2,x3,y3;
	unsigned long	u0,v0,u1,v1,u2,v2,u3,v3;
	long			z0,z1,z2,z3;
	TextureType		*tex;
	unsigned long 	t1;
	unsigned long 	t2;
	PACKET 			*packet;
	char 			*opcd;
	long 			*tfv;
	long 			*tfd;
	long 			depth;
	int 			i, j;

	tfv = transformedVertices;
	tfd = transformedDepths;

//	if( max_depth > 1024 - mesh->extra_depth )
//		max_depth = 1024 - mesh->extra_depth;

	transformVertexListA((VERT*)mesh->verts, mesh->n_verts, tfv, tfd);

#define si ((POLY_GT4*)packet)
#define op ((FMA_GT4 *)opcd)

	op = mesh->gt4s;
	polyCount += mesh->n_gt4s;

	strip3DBackdropTextureID = -1;

	for( i = mesh->n_gt4s; i != 0; i--, op++ )
	{
		x0 = GETX(op->vert0);
		x1 = GETX(op->vert1);
		x2 = GETX(op->vert2);
		x3 = GETX(op->vert3);

		if((x0 > 640)&&(x1 > 640)&&(x2 > 640)&&(x3 > 640))
			continue;
		if((x0 < 0)&&(x1 < 0)&&(x2 < 0)&&(x3 < 0))
			continue;

		y0 = GETY(op->vert0);
		y1 = GETY(op->vert1);
		y2 = GETY(op->vert2);
		y3 = GETY(op->vert3);
			
		if((y0 > 480)&&(y1 > 480)&&(y2 > 480)&&(y3 > 480))
			continue;
		if((y0 < 0)&&(y1 < 0)&&(y2 < 0)&&(y3 < 0))
			continue;
	
		// Get average Z of the polygon & use as a depth pointer
		z0 = GETD(op->vert0) + (PSImodelctrl.farclip*2);
		z1 = GETD(op->vert1) + (PSImodelctrl.farclip*2);
		z2 = GETD(op->vert2) + (PSImodelctrl.farclip*2);
		z3 = GETD(op->vert3) + (PSImodelctrl.farclip*2);
		depth = ((z0 + z1 + z2 + z3)*0.0625);
				
//		if( depth > min_depth )
		{
			tex = &DCKtextureList[op->tpage];

			vertices_GT4_FMA[0].fX = x0;
			vertices_GT4_FMA[0].fY = y0;
			vertices_GT4_FMA[0].u.fZ = 1.0 / ((float)z0);
			vertices_GT4_FMA[0].fU = (float)op->u0 / 127.0;
			vertices_GT4_FMA[0].fV = (float)op->v0 / 127.0;
			vertices_GT4_FMA[0].uBaseRGB.dwPacked = RGBA(op->r0,op->g0,op->b0,255);

			vertices_GT4_FMA[1].fX = x1;
			vertices_GT4_FMA[1].fY = y1;
			vertices_GT4_FMA[1].u.fZ = 1.0 / ((float)z1);
			vertices_GT4_FMA[1].fU = (float)op->u1 / 127.0;
			vertices_GT4_FMA[1].fV = (float)op->v1 / 127.0;
			vertices_GT4_FMA[1].uBaseRGB.dwPacked = RGBA(op->r1,op->g1,op->b1,255);

			vertices_GT4_FMA[2].fX = x2;
			vertices_GT4_FMA[2].fY = y2;
			vertices_GT4_FMA[2].u.fZ = 1.0 / ((float)z2);
			vertices_GT4_FMA[2].fU = (float)op->u2 / 127.0;
			vertices_GT4_FMA[2].fV = (float)op->v2 / 127.0;
			vertices_GT4_FMA[2].uBaseRGB.dwPacked = RGBA(op->r2,op->g2,op->b2,255);

			vertices_GT4_FMA[3].fX = x3;
			vertices_GT4_FMA[3].fY = y3;
			vertices_GT4_FMA[3].u.fZ = 1.0 / ((float)z3);
			vertices_GT4_FMA[3].fU = (float)op->u3 / 127.0;
			vertices_GT4_FMA[3].fV = (float)op->v3 / 127.0;		
			vertices_GT4_FMA[3].uBaseRGB.dwPacked = RGBA(op->r3,op->g3,op->b3,255);

//			if(tex->animated)
//			{
				// check to see if alpha channel is to be used
				if(tex->colourKey)
				{
					// change strip if required
					if(op->tpage != strip3DBackdropTextureID)
					{
						kmChangeStripTextureSurface(&StripHead_3DBackdrop,KM_IMAGE_PARAM1,tex->surfacePtr);
						strip3DBackdropTextureID = op->tpage;
					}
					kmStartStrip(&vertexBufferDesc, &StripHead_3DBackdrop);
				}	
				else
				{
					// change strip if required
					if(op->tpage != strip3DBackdropTextureID)
					{
						kmChangeStripTextureSurface(&StripHead_3DBackdrop,KM_IMAGE_PARAM1,tex->surfacePtr);		
						strip3DBackdropTextureID = op->tpage;
					}
					kmStartStrip(&vertexBufferDesc, &StripHead_3DBackdrop);	
				}	
//			}
//			else
//			{
//				kmStartStrip(&vertexBufferDesc, &tex->stripHead);	
//			}
			
			kmSetVertex(&vertexBufferDesc, &vertices_GT4_FMA[0], KM_VERTEXTYPE_03, sizeof(KMVERTEX_03));
			kmSetVertex(&vertexBufferDesc, &vertices_GT4_FMA[1], KM_VERTEXTYPE_03, sizeof(KMVERTEX_03));
			kmSetVertex(&vertexBufferDesc, &vertices_GT4_FMA[2], KM_VERTEXTYPE_03, sizeof(KMVERTEX_03));	
			kmSetVertex(&vertexBufferDesc, &vertices_GT4_FMA[3], KM_VERTEXTYPE_03, sizeof(KMVERTEX_03));	
			kmEndStrip(&vertexBufferDesc);			
		}
	}
#undef op
#undef si
	
#define si ((POLY_GT3*)packet)
#define op ((FMA_GT3 *)opcd)

	op = mesh->gt3s;

	polyCount += mesh->n_gt3s;

	for ( i = mesh->n_gt3s; i != 0; i--, op++ )
	{
		x0 = GETX(op->vert0);
		x1 = GETX(op->vert1);
		x2 = GETX(op->vert2);

		if((x0 > 640)&&(x1 > 640)&&(x2 > 640))
			continue;
		if((x0 < 0)&&(x1 < 0)&&(x2 < 0))
			continue;

		y0 = GETY(op->vert0);
		y1 = GETY(op->vert1);
		y2 = GETY(op->vert2);
			
		if((y0 > 480)&&(y1 > 480)&&(y2 > 480))
			continue;
		if((y0 < 0)&&(y1 < 0)&&(y2 < 0))
			continue;
	
		// Get average Z of the polygon & use as a depth pointer
		z0 = GETD(op->vert0) + (PSImodelctrl.farclip*2);
		z1 = GETD(op->vert1) + (PSImodelctrl.farclip*2);
		z2 = GETD(op->vert2) + (PSImodelctrl.farclip*2);
		depth = ((z0 + z1 + z2)*0.0833);

//		if ( depth > min_depth )
		{	
			tex = &DCKtextureList[op->tpage];

			vertices_GT3_FMA[0].fX = x0;
			vertices_GT3_FMA[0].fY = y0;
			vertices_GT3_FMA[0].u.fZ = 1.0 / ((float)z0);
			vertices_GT3_FMA[0].fU = (float)op->u0 / 127.0;
			vertices_GT3_FMA[0].fV = (float)op->v0 / 127.0;
			vertices_GT3_FMA[0].uBaseRGB.dwPacked = RGBA(op->r0,op->g0,op->b0,255);

			vertices_GT3_FMA[1].fX = x1;
			vertices_GT3_FMA[1].fY = y1;
			vertices_GT3_FMA[1].u.fZ = 1.0 / ((float)z1);
			vertices_GT3_FMA[1].fU = (float)op->u1 / 127.0;
			vertices_GT3_FMA[1].fV = (float)op->v1 / 127.0;
			vertices_GT3_FMA[1].uBaseRGB.dwPacked = RGBA(op->r1,op->g1,op->b1,255);

			vertices_GT3_FMA[2].fX = x2;
			vertices_GT3_FMA[2].fY = y2;
			vertices_GT3_FMA[2].u.fZ = 1.0 / ((float)z2);
			vertices_GT3_FMA[2].fU = (float)op->u2 / 127.0;
			vertices_GT3_FMA[2].fV = (float)op->v2 / 127.0;
			vertices_GT3_FMA[2].uBaseRGB.dwPacked = RGBA(op->r2,op->g2,op->b2,255);

//			if(tex->animated)
//			{
				// check to see if alpha channel is to be used
				if(tex->colourKey)
				{
					// change strip if required
					if(op->tpage != strip3DBackdropTextureID)
					{
						kmChangeStripTextureSurface(&StripHead_3DBackdrop,KM_IMAGE_PARAM1,tex->surfacePtr);
						strip3DBackdropTextureID = op->tpage;
					}
					kmStartStrip(&vertexBufferDesc, &StripHead_3DBackdrop);
				}	
				else
				{
					// change strip if required
					if(op->tpage != strip3DBackdropTextureID)
					{
						kmChangeStripTextureSurface(&StripHead_3DBackdrop,KM_IMAGE_PARAM1,&DCKtextureList[op->tpage].surface);		
						strip3DBackdropTextureID = op->tpage;
					}
					kmStartStrip(&vertexBufferDesc, &StripHead_3DBackdrop);	
				}	
//			}
//			else
//			{
//				kmStartStrip(&vertexBufferDesc, &tex->stripHead);	
//			}
			
			kmSetVertex(&vertexBufferDesc, &vertices_GT3_FMA[0], KM_VERTEXTYPE_03, sizeof(KMVERTEX_03));
			kmSetVertex(&vertexBufferDesc, &vertices_GT3_FMA[1], KM_VERTEXTYPE_03, sizeof(KMVERTEX_03));
			kmSetVertex(&vertexBufferDesc, &vertices_GT3_FMA[2], KM_VERTEXTYPE_03, sizeof(KMVERTEX_03));	
			kmEndStrip(&vertexBufferDesc);			
		}
	}
#undef op
#undef si
}


void DrawCube ( FMA_MESH_HEADER *mesh, int flags )
{
//	char 			u,v;
	unsigned long 	t1;
	unsigned long 	t2;
	char 			*opcd;
	long 			*tfv;
	long 			*tfd;
	PACKET 			*packet;
	float			x0,y0,x1,y1,x2,y2,x3,y3;
	unsigned long	u0,v0,u1,v1,u2,v2,u3,v3;
	long 			depth;
	long			z0,z1,z2,z3;
	TextureType		*tex;
	unsigned char	alpha;

	#define GETX(n)( ((SHORTXY *)( (int)(tfv) +(n) ))->x )
	#define GETY(n)( ((SHORTXY *)( (int)(tfv) +(n) ))->y )
	#define GETV(n)(  *(unsigned long *)( (int)(tfv) +(n) ) )
	#define GETD(n)(  *(unsigned long *)( (int)(tfd) +(n) ) )

	int i, j;

	int min_depth = MIN_MAP_DEPTH;// + mesh->extra_depth;
	int max_depth = fog.max>>2;//MAX_MAP_DEPTH;// + mesh->extra_depth;

	tfv = (long*)transformedVertices;
	tfd = (long*)transformedDepths;

	if ( max_depth > 1024 - mesh->extra_depth )
		max_depth = 1024 - mesh->extra_depth;
	
	stripGT3FMAtextureID   = -1;
	stripGT3FMAtextureID_A = -1;
	stripGT4FMAtextureID   = -1;
	stripGT4FMAtextureID_A = -1;
	stripGT4FMAtextureID_Add = -1;
	
	transformVertexListA((VERT*)mesh->verts, mesh->n_verts, tfv, tfd);

#define si ((POLY_GT4*)packet)
#define op ((FMA_GT4 *)opcd)

	op = mesh->gt4s;

	for ( i = mesh->n_gt4s; i != 0; i--, op++ )
	{
		x0 = GETX(op->vert0);
		x1 = GETX(op->vert1);
		x2 = GETX(op->vert2);
		x3 = GETX(op->vert3);

		if((x0 > 640)&&(x1 > 640)&&(x2 > 640)&&(x3 > 640))
			continue;
		if((x0 < 0)&&(x1 < 0)&&(x2 < 0)&&(x3 < 0))
			continue;

		y0 = GETY(op->vert0);
		y1 = GETY(op->vert1);
		y2 = GETY(op->vert2);
		y3 = GETY(op->vert3);
			
		if((y0 > 480)&&(y1 > 480)&&(y2 > 480)&&(y3 > 480))
			continue;
		if((y0 < 0)&&(y1 < 0)&&(y2 < 0)&&(y3 < 0))
			continue;
	
		// Get average Z of the polygon & use as a depth pointer
		z0 = GETD(op->vert0);
		z1 = GETD(op->vert1);
		z2 = GETD(op->vert2);
		z3 = GETD(op->vert3);
		z0 += 50;
		z1 += 50;
		z2 += 50;
		z3 += 50;
		depth = ((z0 + z1 + z2 + z3)*0.0625);

		if(depth > min_depth && depth < max_depth)
		{
			tex = &DCKtextureList[globalClut];
			u0 = op->u0;		// Texture coords
			v0 = op->v0;
			u1 = op->u1;		// Texture coords
			v1 = op->v1;
			u2 = op->u2;		// Texture coords
			v2 = op->v2;
			u3 = op->u3;		// Texture coords
			v3 = op->v3;

			vertices_GT4_FMA[0].fX = x0;
			vertices_GT4_FMA[0].fY = y0;
			vertices_GT4_FMA[0].u.fZ = 1.0 / ((float)z0);
			vertices_GT4_FMA[0].fU = (float)u0 / 127.0;
			vertices_GT4_FMA[0].fV = (float)v0 / 127.0;
			vertices_GT4_FMA[0].uBaseRGB.dwPacked = RGBA(op->r0,op->g0,op->b0,alpha);

			vertices_GT4_FMA[1].fX = x1;
			vertices_GT4_FMA[1].fY = y1;
			vertices_GT4_FMA[1].u.fZ = 1.0 / ((float)z1);
			vertices_GT4_FMA[1].fU = (float)u1 / 127.0;
			vertices_GT4_FMA[1].fV = (float)v1 / 127.0;
			vertices_GT4_FMA[1].uBaseRGB.dwPacked = RGBA(op->r1,op->g1,op->b1,alpha);

			vertices_GT4_FMA[2].fX = x2;
			vertices_GT4_FMA[2].fY = y2;
			vertices_GT4_FMA[2].u.fZ = 1.0 / ((float)z2);
			vertices_GT4_FMA[2].fU = (float)u2 / 127.0;
			vertices_GT4_FMA[2].fV = (float)v2 / 127.0;
			vertices_GT4_FMA[2].uBaseRGB.dwPacked = RGBA(op->r2,op->g2,op->b2,alpha);

			vertices_GT4_FMA[3].fX = x3;
			vertices_GT4_FMA[3].fY = y3;
			vertices_GT4_FMA[3].u.fZ = 1.0 / ((float)z3);
			vertices_GT4_FMA[3].fU = (float)u3 / 127.0;
			vertices_GT4_FMA[3].fV = (float)v3 / 127.0;		
			vertices_GT4_FMA[3].uBaseRGB.dwPacked = RGBA(op->r3,op->g3,op->b3,alpha);

			kmStartStrip(&vertexBufferDesc, &tex->stripHead);	
			
			kmSetVertex(&vertexBufferDesc, &vertices_GT4_FMA[0], KM_VERTEXTYPE_03, sizeof(KMVERTEX_03));
			kmSetVertex(&vertexBufferDesc, &vertices_GT4_FMA[1], KM_VERTEXTYPE_03, sizeof(KMVERTEX_03));
			kmSetVertex(&vertexBufferDesc, &vertices_GT4_FMA[2], KM_VERTEXTYPE_03, sizeof(KMVERTEX_03));	
			kmSetVertex(&vertexBufferDesc, &vertices_GT4_FMA[3], KM_VERTEXTYPE_03, sizeof(KMVERTEX_03));	
			kmEndStrip(&vertexBufferDesc);			

		}
	}
#undef op
#undef si
}
