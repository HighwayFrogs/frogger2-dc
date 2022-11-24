

// *ASL* 10/08/2000 - Use the latest Kamui macros
#define _KM_USE_VERTEX_MACRO_
#define _KM_USE_VERTEX_MACRO_L5_
//#define _KM_USE_CHANGESTRIP_MACRO_


#include <shinobi.h>
#include <kamui2.h>
#include <kamuix.h>

#include "include.h"
#include "main.h"
#include "gte.h"
#include "frogger.h"



//#define WATERANIM_1 (u+((rcos(frame<<6))>>11))|((v+((rsin(frame<<6))>>11))<<8)
//#define WATERANIM_2 (u+((rsin(frame<<6))>>11))|((v+((rcos(frame<<6))>>11))<<8)

//#define WATERANIM_1 (u+((rcos(frame<<6)+4096)>>11))|((v+((rsin(frame<<6)+4096)>>11))<<8)
//#define WATERANIM_2 (u+((rsin(frame<<6)+4096)>>11))|((v+((rcos(frame<<6)+4096)>>11))<<8)

extern EXPLORE_black_CLUT;
extern unsigned long frame;
extern int vsyncCounter;
extern int myVsyncCounter;

#define WATERANIM_1  ( ( u )  ) | ( ( v + ( ( 4096 ) >> 11 ) ) << 8 )
#define WATERANIM_2  ( ( u )  ) | ( ( v + ( ( 4096 ) >> 11 ) ) << 8 )

//#define MAP_SCALE_DEPTH_DOWN

extern int polyCount;
extern int mapCount;

//#define WATERANIM_1	(u|(v<<8))
//#define WATERANIM_2	WATERANIM_1

// Scale-Depth-Down deals with shifting large depth-coordinates in a scaled-up map,
// shifting down to a smaller range of OT positions.

// Scale-Adjust allows a scaled-up map to be integrated into an otherwise NON-scaled-up
// coordinate system (probably a temporary measure), by increasing the object-camera
// distance for the duration of the map-plotting calls.

//#define MAP_SCALE_DEPTH_DOWN 3
//#define MAP_SCALE_ADJUST

// A couple of the optimisations in here are enablable by "#define". Once this is sourcesafed, it
//may be worth tidying them out to make the source neater.

// By my reckoning, doing block memory copies via 2 registers instead of via 1 gives us a 2% speed increase in the map plotter
// (But this define lets you go back to the old stuff if needs be)
//#define TEMP_STORE

// Similarly, here's an optimisation that does the backface cull into a CPU register rather than a
// stacked one.
//#define CLIP_REGISTER

/*

  Routines to RENDER frogger2 maps.

  Essentially, an optimised model printer that only deals with
  
	Textured Tris & Quads
	Single-Sided only.
	Non-Transparent.

*/


#define WATER_TRANS_CODE (((ULONG)2)<<24)
#define WATER_TRANS_CLUT (0x80000000)

// PSI.C variables rqd for the draw prims.
// They're defined in the latest islpsi header, so that's ok.
//long *transformedVertices;
//extern VERT *transformedNormals;
//long *transformedDepths;

//displayPageType *currentDisplayPage;


// I dunno what part of the ISL lib these are in. Part of the psi lib, I think,
// but it's not in the headers yet


typedef struct{
	SHORT x,y,z,w;
}SHORTQUAT;

void ShortquaternionGetMatrix(SHORTQUAT *squat, MATRIX *dmatrix);
typedef struct
{
	short x,y;
}SHORTXY;




#ifndef ADD2POINTER
	#define ADD2POINTER(a,b) ( (void *) ( (int)(a) + (b))  )
#endif



// "addPrimLen" is faster than doing "setPolyGT3/4","addPrim",
// becuase a fair few of the numbers are already known / filled in

// In fact, IF we can guarantee that we're linking into an OT address (as we can, here),
// and NOT the tail end of another primitive, then the "length" part of the P_TAG will be zero,
// so we can just read/write the OT address as a "u_long" instead of using "getaddr/setaddr"

// The "<<24" is optimised to a constant by the compiler if "len" is constant.

// this version also assumes we've got a spare register for use as "tempvar" to get rid of a NOP

#define addPrimLen(ot,p,len,tempvar) \
{\
	tempvar = *(unsigned long *)(ot); \
    (*(unsigned long *)(ot) = (unsigned long)(p)); \
	tempvar |= (len<<24); \
	*(unsigned long *)(p) = tempvar; \
}


// (Previous, less-optimised, versions
/*
#define addPrimLen(ot,p,len)\
{\
	(*(unsigned long *)p = *(u_long *)(ot) | (len<<24));\
    (*(u_long *)(ot) = (u_long)p);\
}
*/
//#define addPrimLen(ot,p,len)	(*(unsigned long *)p = getaddr(ot) | (len<<24)), setaddr(ot,p)
//#define addPrimLen(ot,p,len)	{setlen(p,len); addPrim(ot,p);}




// Extra bit of direct gte-access that doesn't then store the register into a stacked variable
// (ie, this gives a nice little speed improvement)

#define gte_stotz_cpu(r)	r = otz;

/* ma
#define gte_stotz_cpu(r)\
asm(\
  "mfc2 %0, $7;"\
  :\
  : "r" (r)\
)
*/

#ifdef CLIP_REGISTER

// Fred, checking whether the same speed improvement works with the backface cull.
// (Answer, yes it does. Hope I've got this bit of asm right!)
#define gte_stopz_cpu(r)	r = otz;
/*
#define gte_stopz_cpu(r)\
asm(\
	"mfc2 %0, $24;"\
	:\
	: "r"(r)\
)
*/
#endif

// Extra Map Depth is now part of the mesh header. The idea is that it's set to about 10, to ensure that
// the frog appears in front of the landscape. "Underwater" objects should have their extra depth set higher,
// so they don't sort badly with the water plane itself
//#define XTRA_MAP_DEPTH 10


// The MIN map depth is basically in here because when doing the perspective transform
// close to / overlapping the camera plane, the PSX switches to an isometric transform rather than a perspective one,
// which looks horrible!
//#define MIN_MAP_DEPTH 140
//#define MIN_MAP_DEPTH 100
#define MIN_MAP_DEPTH (1)

// Must be under 1024 to work with the library's OT
// Use lower values still to define the far cut-off distance.
#define MAX_MAP_DEPTH (1000)



// *ASL* 20/07/2000 - Dreamcast specific headers

/* --------------------------------------------------------------------------------
   Structure : TDCWorldMesh
   Purpose : world segment mesh structure for holding pre-compiled polygon lists
   Info : 
*/
typedef struct _TDCWorldMesh
{
	int				noofQuadPolys;					// number of quad polygons
	int				*quadIndices;					// quad indices
	KMDWORD			*quadVerts;						// quad vertices pointer
	int				noofTriPolys;					// number of tri polygons
	int				*triIndices;					// quad indices
	KMDWORD			*triVerts;						// triangle vertices pointer
	int				noofVerts;						// number of map vertices
	TDCVector4		*vertices;						// map vertices pointer
} TDCWorldMesh;


// world meshes for current world
TDCWorldMesh	*dcWorldMeshes = NULL;

// number of meshes in the current world
static int dcNoofWorldMeshes = 0;


/* ---------------------------------------------------------
   Function : MapDraw_Dreamcast_DrawMesh
   Purpose : draw dreamcast pre-compiled world segment mesh
   Parameters : FMA_MESH_HEADER structure pointer, pre-compiled dreamcast mesh poiinter
   Returns : 
   Info : 
*/

static void MapDraw_Dreamcast_DrawMesh(FMA_MESH_HEADER *mesh, TDCWorldMesh *dcMesh);



// ===================================================
// New, mega-optimised landscape plotter
// This is the routine to draw a single mesh shape.
// Assumes that the matrix is already set up,
// and that any visibility tests have already been done


// Note that this routine has been very heavily optimised.
// If you ever want to add functionality to the map-draw (eg - double-sided polys, transparancy, subdivision)
// then many of these will actually fail & chug, due to lack of register space.
// The routine is fast because its simple.

void MapDraw_DrawFMA_Mesh2(FMA_MESH_HEADER *mesh)
{
	FMA_GT4			*opgt4;
	FMA_GT3			*opgt3;
	FMA_SPR 		*opspr;
	long 			*map_tfv;
	long 			*map_tfd;
	float 			*map_tfd2;
	long 			depth;
	int 			i;
	long	 		clipflag;
	int 			min_depth = (MIN_MAP_DEPTH + mesh->extra_depth);
	int 			max_depth = (MAX_MAP_DEPTH + mesh->extra_depth);
	float			x0,y0,x1,y1,x2,y2,x3,y3;
	float			z0,z1,z2,z3;
	TextureType		*tex;
	int				num_sprs = 0;
	
	mapCount++;
	
	// These macros are for an optimisation that requires all our vertex numbers to be multiplied by four,	
	// which means they're byte offsets into the tfv/tfd table rather than int-array indices
	// Essentially
	// tfv[n] and tfd[n], where tfv/tfd are arrays of ints, have now been replaced by these macros,
	// where "n" is pre-shifted at PC map-compile time. Thus saving a shift instruction per vertex per polygon.
#define GETX(n)( ((SHORTXY *)( (int)(map_tfv) +(n) ))->x )
#define GETY(n)( ((SHORTXY *)( (int)(map_tfv) +(n) ))->y )
#define GETV(n)(  *(unsigned long *)( (int)(map_tfv) +(n) ) )
#define GETD(n)(  *(unsigned long *)( (int)(map_tfd) +(n) ) )
//#define GETD2(n)(  *(float *)( (int)(map_tfd2) +(n) ) )

	map_tfv = transformedVertices;
	map_tfd = transformedDepths;
	map_tfd2 = transformedDepths2;

	if(max_depth > 1024-mesh->extra_depth)
		max_depth = 1024-mesh->extra_depth;

	transformVertexListA((VERT*)mesh->verts, mesh->n_verts, map_tfv, map_tfd);

	// This should really by in the (or an alternative) transformvertexlist function
	// It scales the OTZ's down so that they actually fit into the size of the ordering table.
#ifdef MAP_SCALE_DEPTH_DOWN
//	{
//		for(i = 0; i < mesh->n_verts; i++)
//			tfd[i] = tfd[i] >> MAP_SCALE_DEPTH_DOWN;
//	}
#endif

	// The MapDraw stores a count+list of quads and one of tris,
	// rather than checking a "type" element for every polygon.
	// (coz it only has those two types. No transparent. No double sided. No nothing)

	opgt4 = mesh->gt4s;
	
	// Little note - Down-counters are quicker than up-counters because you can
	// compare with zero rather than with a variable.
	// "!=0" is also marginally faster than ">0" here.
	// n_gt4s CAN be zero. Otherwise,a (do,while) loop would be ever-so-slightly faster.than the (for...) one.

//	kmxxGetCurrentPtr(&vertexBufferDesc);
		
	for(i = mesh->n_gt4s; i != 0; i--,opgt4++)
	{
		x0 = GETX(opgt4->vert0);
		x1 = GETX(opgt4->vert1);
		x2 = GETX(opgt4->vert2);
		x3 = GETX(opgt4->vert3);

		if((x0 >= 640)&&(x1 >= 640)&&(x2 >= 640)&&(x3 >= 640))
			continue;
		if((x0 < 0)&&(x1 < 0)&&(x2 < 0)&&(x3 < 0))
			continue;
			
		y0 = GETY(opgt4->vert0);
		y1 = GETY(opgt4->vert1);
		y2 = GETY(opgt4->vert2);
		y3 = GETY(opgt4->vert3);
			
		if((y0 >= 480)&&(y1 >= 480)&&(y2 >= 480)&&(y3 >= 480))
			continue;
		if((y0 < 0)&&(y1 < 0)&&(y2 < 0)&&(y3 < 0))
			continue;		
					
		// Get average Z of the polygon & use as a depth pointer
		z0 = GETD(opgt4->vert0);
		z1 = GETD(opgt4->vert1);
		z2 = GETD(opgt4->vert2);
		z3 = GETD(opgt4->vert3);		
		depth = ((z0 + z1 + z2 + z3)*0.0625);

		if(depth < 30)
			continue;
			
		if(depth > min_depth && depth < max_depth)
		{
//			if(clipflag<0)
			{			
				tex = &DCKtextureList[opgt4->tpage];
				
				vertices_GT4_FMA[0].fX = x0;
				vertices_GT4_FMA[0].fY = y0;
				vertices_GT4_FMA[0].u.fZ = 1.0 / z0;
				vertices_GT4_FMA[0].fU = opgt4->u0 / 127.0;
				vertices_GT4_FMA[0].fV = opgt4->v0 / 127.0;
				vertices_GT4_FMA[0].uBaseRGB.dwPacked = RGBA(opgt4->r0,opgt4->g0,opgt4->b0,255);

				vertices_GT4_FMA[1].fX = x1;
				vertices_GT4_FMA[1].fY = y1;
				vertices_GT4_FMA[1].u.fZ = 1.0 / z1;
				vertices_GT4_FMA[1].fU = opgt4->u1 / 127.0;
				vertices_GT4_FMA[1].fV = opgt4->v1 / 127.0;
				vertices_GT4_FMA[1].uBaseRGB.dwPacked = RGBA(opgt4->r1,opgt4->g1,opgt4->b1,255);

				vertices_GT4_FMA[2].fX = x2;
				vertices_GT4_FMA[2].fY = y2;
				vertices_GT4_FMA[2].u.fZ = 1.0 / z2;
				vertices_GT4_FMA[2].fU = opgt4->u2 / 127.0;
				vertices_GT4_FMA[2].fV = opgt4->v2 / 127.0;
				vertices_GT4_FMA[2].uBaseRGB.dwPacked = RGBA(opgt4->r2,opgt4->g2,opgt4->b2,255);

				vertices_GT4_FMA[3].fX = x3;
				vertices_GT4_FMA[3].fY = y3;
				vertices_GT4_FMA[3].u.fZ = 1.0 / z3;
				vertices_GT4_FMA[3].fU = opgt4->u3 / 127.0;
				vertices_GT4_FMA[3].fV = opgt4->v3 / 127.0;		
				vertices_GT4_FMA[3].uBaseRGB.dwPacked = RGBA(opgt4->r3,opgt4->g3,opgt4->b3,255);
				
				if(tex->animated)
				{
					// check to see if alpha channel is to be used
					if(tex->colourKey)
					{
						// change strip if required
						if(opgt4->tpage != stripGT4FMAtextureID_A)
						{
							kmChangeStripTextureSurface(&StripHead_GT4_FMA_Alpha,KM_IMAGE_PARAM1,tex->surfacePtr);
							stripGT4FMAtextureID_A = opgt4->tpage;
						}
						kmStartStrip(&vertexBufferDesc, &StripHead_GT4_FMA_Alpha);
					}	
					else
					{
						// change strip if required
						if(opgt4->tpage != stripGT4FMAtextureID)
						{
							kmChangeStripTextureSurface(&StripHead_GT4_FMA,KM_IMAGE_PARAM1,tex->surfacePtr);		
							stripGT4FMAtextureID = opgt4->tpage;
						}
						kmStartStrip(&vertexBufferDesc, &StripHead_GT4_FMA);	
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


//				kmxxSetVertex_3(KM_VERTEXPARAM_NORMAL,    vertices_GT4_FMA[0].fX,vertices_GT4_FMA[0].fY,vertices_GT4_FMA[0].u.fZ,vertices_GT4_FMA[0].fU,vertices_GT4_FMA[0].fV,vertices_GT4_FMA[0].uBaseRGB.dwPacked,vertices_GT4_FMA[0].uBaseRGB.dwPacked);	
//				kmxxSetVertex_3(KM_VERTEXPARAM_NORMAL,    vertices_GT4_FMA[1].fX,vertices_GT4_FMA[1].fY,vertices_GT4_FMA[1].u.fZ,vertices_GT4_FMA[1].fU,vertices_GT4_FMA[1].fV,vertices_GT4_FMA[1].uBaseRGB.dwPacked,vertices_GT4_FMA[1].uBaseRGB.dwPacked);	
//				kmxxSetVertex_3(KM_VERTEXPARAM_NORMAL,    vertices_GT4_FMA[2].fX,vertices_GT4_FMA[2].fY,vertices_GT4_FMA[2].u.fZ,vertices_GT4_FMA[2].fU,vertices_GT4_FMA[2].fV,vertices_GT4_FMA[2].uBaseRGB.dwPacked,vertices_GT4_FMA[2].uBaseRGB.dwPacked);	
//				kmxxSetVertex_3(KM_VERTEXPARAM_ENDOFSTRIP,vertices_GT4_FMA[3].fX,vertices_GT4_FMA[3].fY,vertices_GT4_FMA[3].u.fZ,vertices_GT4_FMA[3].fU,vertices_GT4_FMA[3].fV,vertices_GT4_FMA[3].uBaseRGB.dwPacked,vertices_GT4_FMA[3].uBaseRGB.dwPacked);	
			
				polyCount++;
			}
		}
	}

//	kmxxReleaseCurrentPtr(&vertexBufferDesc);

	// That's the quads done, now let's do the triangles...

	opgt3 = mesh->gt3s;
	for(i = mesh->n_gt3s; i != 0; i--,opgt3++)
	{
		x0 = GETX(opgt3->vert0);
		x1 = GETX(opgt3->vert1);
		x2 = GETX(opgt3->vert2);

		if((x0 >= 640)&&(x1 >= 640)&&(x2 >= 640))
			continue;
		if((x0 < 0)&&(x1 < 0)&&(x2 < 0))
			continue;
			
		y0 = GETY(opgt3->vert0);
		y1 = GETY(opgt3->vert1);
		y2 = GETY(opgt3->vert2);
			
		if((y0 >= 480)&&(y1 >= 480)&&(y2 >= 480))
			continue;
		if((y0 < 0)&&(y1 < 0)&&(y2 < 0))
			continue;		
					
		// Get average Z of the polygon & use as a depth pointer
		z0 = GETD(opgt3->vert0);
		z1 = GETD(opgt3->vert1);
		z2 = GETD(opgt3->vert2);
		depth = ((z0 + z1 + z2)*0.0833);

		if(depth < 30)
			continue;

		if(depth > min_depth && depth < max_depth)
		{
//			if(clipflag < 0)
			{
				tex = &DCKtextureList[opgt3->tpage];
				
				vertices_GT3_FMA[0].fX = x0;
				vertices_GT3_FMA[0].fY = y0;
				vertices_GT3_FMA[0].u.fZ = 1.0 / z0;
				vertices_GT3_FMA[0].fU = opgt3->u0 / 127.0;
				vertices_GT3_FMA[0].fV = opgt3->v0 / 127.0;
				vertices_GT3_FMA[0].uBaseRGB.dwPacked = RGBA(opgt3->r0,opgt3->g0,opgt3->b0,255);

				vertices_GT3_FMA[1].fX = x1;
				vertices_GT3_FMA[1].fY = y1;
				vertices_GT3_FMA[1].u.fZ = 1.0 / z1;
				vertices_GT3_FMA[1].fU = opgt3->u1 / 127.0;
				vertices_GT3_FMA[1].fV = opgt3->v1 / 127.0;
				vertices_GT3_FMA[1].uBaseRGB.dwPacked = RGBA(opgt3->r1,opgt3->g1,opgt3->b1,255);

				vertices_GT3_FMA[2].fX = x2;
				vertices_GT3_FMA[2].fY = y2;
				vertices_GT3_FMA[2].u.fZ = 1.0 / z2;
				vertices_GT3_FMA[2].fU = opgt3->u2 / 127.0;
				vertices_GT3_FMA[2].fV = opgt3->v2 / 127.0;
				vertices_GT3_FMA[2].uBaseRGB.dwPacked = RGBA(opgt3->r2,opgt3->g2,opgt3->b2,255);

				if(tex->animated)
				{
					// check to see if alpha channel is to be used
					if(tex->colourKey)
					{
						// change strip if required		
						if(opgt3->tpage != stripGT3FMAtextureID_A)
						{
							kmChangeStripTextureSurface(&StripHead_GT3_FMA_Alpha,KM_IMAGE_PARAM1,tex->surfacePtr);
							stripGT3FMAtextureID_A = opgt3->tpage;
						}
						kmStartStrip(&vertexBufferDesc, &StripHead_GT3_FMA_Alpha);
					}		
					else
					{
						// change strip if required
						if(opgt3->tpage != stripGT3FMAtextureID)
						{
							kmChangeStripTextureSurface(&StripHead_GT3_FMA,KM_IMAGE_PARAM1,tex->surfacePtr);
							stripGT3FMAtextureID = opgt3->tpage;
						}
						kmStartStrip(&vertexBufferDesc, &StripHead_GT3_FMA);	
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

				polyCount++;
			}
			
		}
	}
	
//	polyCount += mesh->n_gt3s;

//	return;
	
	opspr = mesh->sprs;
	
//	count = 0;

	//utilPrintf ( "Number Of Sprite : %d\n", mesh->n_sprs );

	num_sprs = mesh->n_sprs;
	
	for(i = mesh->n_sprs; i != 0; i--,opspr++)
	{
		LONG spritez, width, height;
		VERT tempVect;

//		BEGINPRIM(si, POLY_FT4);

//		setPolyFT4(si);

		//utilPrintf ( "Sprite Position : %d : %d : %d\n", op->x, op->y, op->z );

		tempVect.vx = opspr->x;
		tempVect.vy = opspr->y;
		tempVect.vz = opspr->z;

		gte_SetLDDQB(0);		// clear offset control reg (C2_DQB)

		width = 32;

		gte_ldv0(&tempVect);
		gte_SetLDDQA(width);	// shove sprite width into control reg (C2_DQA)
		gte_rtps();				// do the rtps
		x0 = screenxy[2].vx;
		y0 = screenxy[2].vy;		
//		gte_stsxy(&si->x0);		// get screen x and y
		gte_stsz(&spritez);		// get screen z

		if ( spritez <= 0 || spritez >= fog.max ) 
			return;

		width = ( 64 * SCALEX ) / spritez;

 		x1 = x3 = x0 + width;
 		x0 = x2 = x0 - width;

		height = ( 64 * SCALEY ) / spritez;

		y2 = y3 = y0 + height;
		y1 = y0 = y0 - height;

		vertices_GT4_FMA[0].fX = x0;
		vertices_GT4_FMA[0].fY = y0;
		vertices_GT4_FMA[0].u.fZ = 1.0 / spritez;
		vertices_GT4_FMA[0].fU = opspr->u0 / 127.0;
		vertices_GT4_FMA[0].fV = opspr->v0 / 127.0;
		vertices_GT4_FMA[0].uBaseRGB.dwPacked = RGBA(opspr->r0,opspr->g0,opspr->b0,255);

		vertices_GT4_FMA[1].fX = x1;
		vertices_GT4_FMA[1].fY = y1;
		vertices_GT4_FMA[1].u.fZ = 1.0 / spritez;
		vertices_GT4_FMA[1].fU = opspr->u1 / 127.0;
		vertices_GT4_FMA[1].fV = opspr->v1 / 127.0;
		vertices_GT4_FMA[1].uBaseRGB.dwPacked = RGBA(opspr->r0,opspr->g0,opspr->b0,255);

		vertices_GT4_FMA[2].fX = x2;
		vertices_GT4_FMA[2].fY = y2;
		vertices_GT4_FMA[2].u.fZ = 1.0 / spritez;
		vertices_GT4_FMA[2].fU = opspr->u2 / 127.0;
		vertices_GT4_FMA[2].fV = opspr->v2 / 127.0;
		vertices_GT4_FMA[2].uBaseRGB.dwPacked = RGBA(opspr->r0,opspr->g0,opspr->b0,255);

		vertices_GT4_FMA[3].fX = x3;
		vertices_GT4_FMA[3].fY = y3;
		vertices_GT4_FMA[3].u.fZ = 1.0 / spritez;
		vertices_GT4_FMA[3].fU = opspr->u3 / 127.0;
		vertices_GT4_FMA[3].fV = opspr->v3 / 127.0;		
		vertices_GT4_FMA[3].uBaseRGB.dwPacked = RGBA(opspr->r0,opspr->g0,opspr->b0,255);

		kmStartStrip(&vertexBufferDesc, &DCKtextureList[opspr->tpage].stripHead);	
		kmSetVertex(&vertexBufferDesc, &vertices_GT4_FMA[0], KM_VERTEXTYPE_03, sizeof(KMVERTEX_03));
		kmSetVertex(&vertexBufferDesc, &vertices_GT4_FMA[1], KM_VERTEXTYPE_03, sizeof(KMVERTEX_03));
		kmSetVertex(&vertexBufferDesc, &vertices_GT4_FMA[2], KM_VERTEXTYPE_03, sizeof(KMVERTEX_03));	
		kmSetVertex(&vertexBufferDesc, &vertices_GT4_FMA[3], KM_VERTEXTYPE_03, sizeof(KMVERTEX_03));	
		kmEndStrip(&vertexBufferDesc);				

	}

}


void SetUpWaterMesh(FMA_WORLD *pFMA)
{
	int i;
	FMA_MESH_HEADER **ppMh=(FMA_MESH_HEADER **)(((ULONG)pFMA)+sizeof(FMA_WORLD));

//	ASSERT(pFMA);

	for(i=0; i<pFMA->n_meshes; i++)
	{
		FMA_MESH_HEADER *pMh=ppMh[i];

//	int n_gt3s;
//	FMA_GT3 *gt3s;
//	int n_gt4s;
//	FMA_GT4 *gt4s;
		
		int p,v;
		int vc[4];
		int vp;

		for(v=0; v<pMh->n_verts; v++)
		{
			pMh->verts[v].pad=0;
		}

		for(p=0; p<pMh->n_gt3s; p++)
		{
			vc[0]=(pMh->gt3s[p].vert0)>>2;
			vc[1]=(pMh->gt3s[p].vert1)>>2;
			vc[2]=(pMh->gt3s[p].vert2)>>2;

			for(vp=0; vp<3; vp++)
			{
//				ASSERT(vc[vp]<pMh->n_verts);
				pMh->verts[vc[vp]].pad++;
			}
		}

		for(p=0; p<pMh->n_gt4s; p++)
		{
			vc[0]=(pMh->gt4s[p].vert0)>>2;
			vc[1]=(pMh->gt4s[p].vert1)>>2;
			vc[2]=(pMh->gt4s[p].vert2)>>2;
			vc[3]=(pMh->gt4s[p].vert3)>>2;

			for(vp=0; vp<4; vp++)
			{
//				ASSERT(vc[vp]<pMh->n_verts);
				pMh->verts[vc[vp]].pad++;
			}
		}

		for(v=0; v<pMh->n_verts; v++)
		{
//			utilPrintf("vert %d is used %d times\n", v, pMh->verts[v].pad);
		}
	}
}

// Set up the PSX's transformation matrix for a landscape segment,
// or possibly for a NON-ROTATING platform.
void MapDraw_SetMatrix(FMA_MESH_HEADER *mesh, short posx, short posy, short posz)
{
	MATRIX tx;

	gte_SetRotMatrix(&GsWSMATRIX);
	gte_SetTransMatrix(&GsWSMATRIX);

	mesh->posx = posx;
	mesh->posy = -posy;
	mesh->posz = posz;

	// Unnecessary maths for landscape segments, where pos is always zero.
	gte_ldlvl( &mesh->posx);
	gte_rtirtr();
	gte_stlvl(&tx.t);

	gte_SetRotMatrix(&GsWSMATRIX);
	gte_SetTransMatrix(&tx);
}

void MapDraw_SetScenicMatrix(FMA_MESH_HEADER* mesh, SCENICOBJ* sc)
{
	MATRIX tx, rY;


	gte_SetRotMatrix(&GsWSMATRIX);
	gte_SetTransMatrix(&GsWSMATRIX);

	mesh->posx = -sc->matrix.t[0];
	mesh->posy = -sc->matrix.t[1];
	mesh->posz = sc->matrix.t[2];

	// Unnecessary maths for landscape segments, where pos is always zero.
	gte_ldlvl( &mesh->posx);
	gte_rtirtr();
	gte_stlvl(&tx.t);

//	gte_SetRotMatrix(&GsWSMATRIX);
//	gte_SetTransMatrix(&tx);

//bbopt
/*	
	gte_MulMatrix0(&GsWSMATRIX, &sc->matrix, &tx);
	rY.m[0][0] = rY.m[1][1] = rY.m[2][2] = 4096;
	rY.m[0][1] = rY.m[0][2] = rY.m[1][0] = rY.m[1][2] = rY.m[2][0] = rY.m[2][1] = 0;

	RotMatrixY(2048, &rY);
	gte_MulMatrix0(&tx, &rY, &tx);
	gte_SetTransMatrix(&tx);
	gte_SetRotMatrix(&tx);
*/

	gte_MulMatrix0(&GsWSMATRIX, &sc->matrix, &tx);
	rY.m[0][1] = rY.m[0][2] = rY.m[1][0] = rY.m[1][2] = rY.m[2][0] = rY.m[2][1] = 0;
	rY.m[0][0] = -0x1000;
	rY.m[1][1] =  0x1000;
	rY.m[2][2] = -0x1000;
	gte_MulMatrix0(&tx, &rY, &tx);
	gte_SetTransMatrix(&tx);
	gte_SetRotMatrix(&tx);

/*
	rY = sc->matrix;
	rY.m[0][0] = -rY.m[0][0];
//	rY.m[1][1] =  rY.m[1][1];
	rY.m[2][2] = -rY.m[2][2];
	gte_MulMatrix0(&GsWSMATRIX, &rY, &tx);
	gte_SetTransMatrix(&tx);
	gte_SetRotMatrix(&tx);
*/


/*
	MATRIX t1;

	gte_SetRotMatrix(&GsWSMATRIX);
	gte_SetTransMatrix(&GsWSMATRIX);
 
	mesh->posx =  sc->matrix.t[0];
	mesh->posy = -sc->matrix.t[1];
	mesh->posz =  sc->matrix.t[2];

	gte_ldlvl(&mesh->posx);
	gte_rtirtr();
	gte_stlvl(&t1.t);

	gte_SetRotMatrix(&GsWSMATRIX);
	gte_SetTransMatrix(&t1);
*/
}

#define OFFLEFT		1
#define OFFRIGHT	2
#define OFFUP		4
#define OFFDOWN		8
#define OFFFRONT	16
#define OFFBACK		32

// Run through a list of meshes, checking if they're visible, & drawing them if so.

void MapDraw_DrawFMA_World(FMA_WORLD *world)
{
#ifdef MAP_SCALE_ADJUST
	MATRIX temp;
#endif
	int i;
//	int removed_models;
	FMA_MESH_HEADER **mesh;

	mesh = ADD2POINTER(world,sizeof(FMA_WORLD));

// If we're rendering the map to a different integer scale to the objects, then we need to
// move the camera away from the increased-scale map for the duration of the map-plotter.
// Get rid of the relevant #define at the top of this file when everything's unified
#ifdef MAP_SCALE_ADJUST
	{
		temp = GsWSMATRIX;
		GsWSMATRIX.t[0] = GsWSMATRIX.t[0] << (MAP_SCALE_DEPTH_DOWN);
		GsWSMATRIX.t[1] = GsWSMATRIX.t[1] << (MAP_SCALE_DEPTH_DOWN);
		GsWSMATRIX.t[2] = GsWSMATRIX.t[2] << (MAP_SCALE_DEPTH_DOWN);
	}
#endif

	//MapDraw_SetMatrix(*mesh, 0, 0, 0);

//ma	count=0;

	for(i = world->n_meshes; i != 0; i--, mesh++)
	{
		if ( (*mesh)->flags & DRAW_SEGMENT )
		{
			// set this meshes transform matrix
			MapDraw_SetMatrix(*mesh, (*mesh)->posx, (*mesh)->posy, (*mesh)->posz);
			// *ASL* 27/07/2000 - Them set mine..
			PSIDC_SH4XD_SetMatrix(&RotMatrix, &TransVector);

			if(MapDraw_ClipCheck(*mesh))
			{
				// *ASL* 20/07/2000 - Draw the dreamcast world segment mesh
				if (dcNoofWorldMeshes > 0 && dcWorldMeshes != NULL)
					MapDraw_Dreamcast_DrawMesh(*mesh, &dcWorldMeshes[world->n_meshes-i]);
				else
					MapDraw_DrawFMA_Mesh2(*mesh);
			// ENDIF
			}
		}
		// ENDIF

	}
	// ENDFOR

#ifdef MAP_SCALE_ADJUST
	{
		GsWSMATRIX = temp;

	}
#endif

}

// Check to see if a frogger map shape is on-screen
// (Assumes the projection matrix has already been set up for this shape)
int MapDraw_ClipCheck(FMA_MESH_HEADER *mesh)
{
	ULONG check,totalchecks;
	SHORTXY			*xy;
	int v;

// "9", coz the transformer only deals in multiples of three,
// (or at least, it did the last time I looked at it)
// and this array is on the stack, so overrunning would be a seriously bad idea.
	VERT pBBox[9];


// Get the 8 corners of the object's bounding box, & transform them.
	for(v=0; v<8; v++)
	{
		pBBox[v].vx=(v&1)?mesh->minx:mesh->maxx;
		pBBox[v].vy=(v&2)?mesh->miny:mesh->maxy;
		pBBox[v].vz=(v&4)?mesh->minz:mesh->maxz;
	}

	//utilPrintf("Map Clip Check!!!\n");
	transformVertexListA(pBBox, 9, transformedVertices, transformedDepths);


// Check to see whether ALL of the vertices are on the outisde of ANY of the 6 planes of the view-frustrum
	xy = (SHORTXY*)transformedVertices;
	totalchecks=0xffff;
	for(v=0; v<8; v++)
	{
		check=0;
		if(xy[v].x<0)
			check|=OFFLEFT;
		if(xy[v].x>640 )
			check|=OFFRIGHT;
		if(xy[v].y<0)
			check|=OFFUP;
		if(xy[v].y>480 )
			check|=OFFDOWN;

#ifdef MAP_SCALE_DEPTH_DOWN
	  	if((transformedDepths[v]) <= (MIN_MAP_DEPTH<<(2+MAP_SCALE_DEPTH_DOWN)))
			check|=OFFFRONT;

//		if((transformedDepths[v]) >= (MAX_MAP_DEPTH<<(2+MAP_SCALE_DEPTH_DOWN)))
//		if((transformedDepths[v]) >= (worldVisualData [ player[0].worldNum ].levelVisualData [ player[0].levelNum ].farClip<<(2+MAP_SCALE_DEPTH_DOWN)))
		if((transformedDepths[v]) >= (fog.max<<(2+MAP_SCALE_DEPTH_DOWN)))
			check |= OFFBACK;
#else
	  	if((transformedDepths[v]) <= (MIN_MAP_DEPTH<<2))
			check|=OFFFRONT;

//		if((transformedDepths[v]) >= (worldVisualData [ player[0].worldNum ].levelVisualData [ player[0].levelNum ].farClip<<2))
//		if((transformedDepths[v]) >= (fog.max))
		if((transformedDepths[v]) >= (fog.max>>2))
			check |= OFFBACK;
#endif

		totalchecks&=check;
	}

	//returns 1 if ok to draw.
	return(totalchecks==0);
}

//this is just MapDraw_ClipCheck, with out the z check.
//Fma actors are z checked before this func is called
int FmaActor_ClipCheck(FMA_MESH_HEADER *mesh)
{
	ULONG check,totalchecks;
	SHORTXY			*xy;
	int v;

// "9", coz the transformer only deals in multiples of three,
// (or at least, it did the last time I looked at it)
// and this array is on the stack, so overrunning would be a seriously bad idea.
	VERT pBBox[9];


// Get the 8 corners of the object's bounding box, & transform them.
	for(v=0; v<8; v++)
	{
		pBBox[v].vx=(v&1)?mesh->minx:mesh->maxx;
		pBBox[v].vy=(v&2)?mesh->miny:mesh->maxy;
		pBBox[v].vz=(v&4)?mesh->minz:mesh->maxz;
	}

	//utilPrintf("Map Clip Check!!!\n");
	transformVertexListA(pBBox, 9, transformedVertices, transformedDepths);


// Check to see whether ALL of the vertices are on the outisde of ANY of the 6 planes of the view-frustrum
	xy = (SHORTXY*)transformedVertices;
	totalchecks=0xffff;
	for(v=0; v<8; v++)
	{
		check=0;
		if(xy[v].x<0)
			check|=OFFLEFT;
		if(xy[v].x>640 )
			check|=OFFRIGHT;
		if(xy[v].y<0)
			check|=OFFUP;
		if(xy[v].y>480 )
			check|=OFFDOWN;

//bbopt - near/far clipping done in FmaActor2ClipCheck()
//using the actor's pos, before full on clipping
/*
#ifdef MAP_SCALE_DEPTH_DOWN
	  	if((transformedDepths[v]) <= (MIN_MAP_DEPTH<<(2+MAP_SCALE_DEPTH_DOWN)))
			check|=OFFFRONT;

//		if((transformedDepths[v]) >= (MAX_MAP_DEPTH<<(2+MAP_SCALE_DEPTH_DOWN)))
//		if((transformedDepths[v]) >= (worldVisualData [ player[0].worldNum ].levelVisualData [ player[0].levelNum ].farClip<<(2+MAP_SCALE_DEPTH_DOWN)))
		if((transformedDepths[v]) >= (fog.max<<(2+MAP_SCALE_DEPTH_DOWN)))
			check |= OFFBACK;
#else
	  	if((transformedDepths[v]) <= (MIN_MAP_DEPTH<<2))
			check|=OFFFRONT;

//		if((transformedDepths[v]) >= (worldVisualData [ player[0].worldNum ].levelVisualData [ player[0].levelNum ].farClip<<2))
		if((transformedDepths[v]) >= (fog.max))
			check |= OFFBACK;
#endif
*/
		totalchecks&=check;
	}

	//returns 1 if ok to draw.
	return(totalchecks==0);
}


//debug spin-off of FmaActor_ClipCheck which
//gives you the screen coords of the box
void FmaActor_GetSBox(FMA_MESH_HEADER *mesh, SHORTXY *sBox)
{
	SHORTXY			*xy;
	VERT pBBox[9];
	int v;

	//Get the 8 corners of the object's bounding box, & transform them.
	for(v=0; v<8; v++)
	{
		pBBox[v].vx=(v&1)?mesh->minx:mesh->maxx;
		pBBox[v].vy=(v&2)?mesh->miny:mesh->maxy;
		pBBox[v].vz=(v&4)?mesh->minz:mesh->maxz;

		//this is the only addition to this function,
		//from when it was MapDraw_ClipCheck().
// 		pBBox[v].vx+=mesh->posx;
//		pBBox[v].vy+=mesh->posy;
//		pBBox[v].vz+=mesh->posz;
	}

	//utilPrintf("Map Clip Check!!!\n");
	transformVertexListA(pBBox, 9, transformedVertices, transformedDepths);

	//copy to dest
	xy = (SHORTXY*)transformedVertices;
	for(v=0; v<8; v++)
	{
		sBox[v].x = xy[v].x;
		sBox[v].y = xy[v].y;
	}
}

// ===================================================
//              Platform-related code
// Note that this is ALL kept after the map code, in
// order that the map code scrunches itself into the
// instruction cache.
// ===================================================

// Set up the PSX's transformation matrix for a platform segment,
// Calculating the rot-matrix is, of course, only neccesary if the thing's rotated
// In fact, it might be nicer to cache this thing somewhere rather than recalculating it all the time
void MapDraw_SetPlatformMatrix(FMA_MESH_HEADER *mesh)
{
	MATRIX tx;
	MATRIX tx1;

	// If we're using quaternions, get the matrix from (xyzw) this way
	// ShortquaternionGetMatrix((SHORTQUAT *)(&mesh->rotx), &tx1);

	// If we're using xyz angles, get the matrix from (xyz) this way
	RotMatrixYXZ_gte((SVECTOR *)&mesh->rotx,&tx1);

	gte_MulMatrix0(&GsWSMATRIX,&tx1,&tx);

	gte_SetRotMatrix(&GsWSMATRIX);
	gte_SetTransMatrix(&GsWSMATRIX);

	gte_ldlvl(&mesh->posx);
	gte_rtirtr();
	gte_stlvl(&tx.t);

	gte_SetRotMatrix(&tx);
	gte_SetTransMatrix(&tx);
}

void MapDraw_SetWaterMatrix ( FMA_MESH_HEADER* mesh, WATER* sc )
{
	MATRIX tx, rY;

	gte_SetRotMatrix(&GsWSMATRIX);
	gte_SetTransMatrix(&GsWSMATRIX);

	mesh->posx = -sc->matrix.t[0];
	mesh->posy = -sc->matrix.t[1];
	mesh->posz = sc->matrix.t[2];

	// Unnecessary maths for landscape segments, where pos is always zero.
	gte_ldlvl( &mesh->posx);
	gte_rtirtr();
	gte_stlvl(&tx.t);

	
	gte_MulMatrix0(&GsWSMATRIX, &sc->matrix, &tx);
	rY.m[0][0] = rY.m[1][1] = rY.m[2][2] = 4096;
	rY.m[0][1] = rY.m[0][2] = rY.m[1][0] = rY.m[1][2] = rY.m[2][0] = rY.m[2][1] = 0;
	RotMatrixY(2048, &rY);
	gte_MulMatrix0(&tx, &rY, &tx);
	gte_SetRotMatrix(&tx);
	gte_SetTransMatrix(&tx);
}

void MapDraw_DrawFMA_Water ( WATER *cur )
{

	FMA_WORLD *waterObj = cur->fma_water;

	int i;

	FMA_MESH_HEADER **mesh;

	mesh = ADD2POINTER(waterObj,sizeof(FMA_WORLD));

	for ( i = waterObj->n_meshes; i != 0; i--, mesh++ )
	{
		//MapDraw_SetMatrix ( *mesh, -cur->position.vx, cur->position.vy, cur->position.vz );

		MapDraw_SetWaterMatrix(*mesh, cur);

		if ( MapDraw_ClipCheck ( *mesh ) )
		{
			DrawWater ( *mesh, cur->flags );
		}
		// ENDIF
	}
	// ENDFOR

}



// *ASL* 20/07/2000

/* ---------------------------------------------------------
   Function : MapDraw_Dreamcast_InitWorld
   Purpose : pre-compile FMA_WORLD dreamcast polygons
   Parameters : FMA_WORLD structure pointer
   Returns : 
   Info : 
*/

void MapDraw_Dreamcast_InitWorld(FMA_WORLD *fma_world)
{
	FMA_MESH_HEADER		**mesh;
	FMA_GT4				*psxQuads;
	FMA_GT3				*psxTris;
	FMA_SPR				*psxSprites;
	SVECTOR				*psxVerts;
	KMDWORD				*vp;
	TDCVector4			*verts;
	TextureType			*map;
	int					i, j, *ip;

	union
	{
		KMFLOAT f[2];
		KMWORD  f16[4];
	} f;


	// allocate our current world mesh array
	if ((dcNoofWorldMeshes = fma_world->n_meshes) == 0)
		return;
	dcWorldMeshes = (TDCWorldMesh *)MALLOC0(fma_world->n_meshes * sizeof(TDCWorldMesh));

	// loop around all this world segment meshes..
	mesh = ADD2POINTER(fma_world, sizeof(FMA_WORLD));
	for (i=0; i<fma_world->n_meshes; i++)
	{
		// ** Convert all SVECTOR vertices

		// allocate aligned memory for mesh vertices - (allow a possible extra 2 entries for the interleaved transform)
		dcWorldMeshes[i].noofVerts = (*mesh)->n_verts;
		dcWorldMeshes[i].vertices = (TDCVector4 *)MALLOC0(((((*mesh)->n_verts +1) &-2) +1) * sizeof(TDCVector4));

		// convert vertices to TDCVectors
		verts = dcWorldMeshes[i].vertices;
		for (j=0, psxVerts = (*mesh)->verts; j<(*mesh)->n_verts; j++, psxVerts++)
		{
			verts->x = (float)psxVerts->vx;
			verts->y = (float)psxVerts->vy;
			verts->z = (float)psxVerts->vz;
			verts->w = 1.0f;

			verts++;
		}


		// ** Pre-compile all Quad Polygon Vertices

		// allocate quad vertices
		dcWorldMeshes[i].noofQuadPolys = (*mesh)->n_gt4s;
		dcWorldMeshes[i].quadVerts = (KMDWORD *)MALLOC0(((*mesh)->n_gt4s * 4) * 3 * sizeof(KMDWORD));
		// allocate quad indices and strip header
		dcWorldMeshes[i].quadIndices = (int *)MALLOC0(((*mesh)->n_gt4s * 8) * sizeof(int));

		// compile vertices - 4verts in 1 32byte cache line
		vp = dcWorldMeshes[i].quadVerts;
		ip = dcWorldMeshes[i].quadIndices;
		for (j=0, psxQuads = (*mesh)->gt4s; j<(*mesh)->n_gt4s; j++, psxQuads++)
		{
			// vertex0
			*vp++ = (KMDWORD)RGBA(psxQuads->r0, psxQuads->g0, psxQuads->b0, 255);
			*((PKMFLOAT)vp) = (KMFLOAT)psxQuads->v0 / 127.0f; vp++;
			*((PKMFLOAT)vp) = (KMFLOAT)psxQuads->u0 / 127.0f; vp++;
			// vertex1
			*vp++ = (KMDWORD)RGBA(psxQuads->r1, psxQuads->g1, psxQuads->b1, 255);
			*((PKMFLOAT)vp) = (KMFLOAT)psxQuads->v1 / 127.0f; vp++;
			*((PKMFLOAT)vp) = (KMFLOAT)psxQuads->u1 / 127.0f; vp++;
			// vertex2
			*vp++ = (KMDWORD)RGBA(psxQuads->r2, psxQuads->g2, psxQuads->b2, 255);
			*((PKMFLOAT)vp) = (KMFLOAT)psxQuads->v2 / 127.0f; vp++;
			*((PKMFLOAT)vp) = (KMFLOAT)psxQuads->u2 / 127.0f; vp++;
			// vertex3
			*vp++ = (KMDWORD)RGBA(psxQuads->r3, psxQuads->g3, psxQuads->b3, 255);
			*((PKMFLOAT)vp) = (KMFLOAT)psxQuads->v3 / 127.0f; vp++;
			*((PKMFLOAT)vp) = (KMFLOAT)psxQuads->u3 / 127.0f; vp++;

			// polygon indices, strip header and texture map.. 1 32byte cache line
			map = &DCKtextureList[psxQuads->tpage];
			ip[3] = (int)map;

			// ** These indices directly need to reference directly into our transform vertices list whoose
			// ** entries are 16bytes in size (xf,yf,zf,wf). Since they are already pre-multiplied by 4 for
			// ** the PSX, we need to multiply them again by 4.

			ip[4] = psxQuads->vert3 << 2;
			ip[5] = psxQuads->vert2 << 2;
			ip[6] = psxQuads->vert1 << 2;
			ip[7] = psxQuads->vert0 << 2;
			ip += 8;
		}


		// ** Pre-compile all Triangle Polygon Vertices

		// allocate tri vertices
		dcWorldMeshes[i].noofTriPolys = (*mesh)->n_gt3s;
		dcWorldMeshes[i].triVerts = (KMDWORD *)MALLOC0(((*mesh)->n_gt3s * 3) * 3 * sizeof(KMDWORD));
		// allocate tri indices and strip header
		dcWorldMeshes[i].triIndices = (int *)MALLOC0(((*mesh)->n_gt3s * 8) * sizeof(int));

		// compile vertices
		vp = dcWorldMeshes[i].triVerts;
		ip = dcWorldMeshes[i].triIndices;
		for (j=0, psxTris = (*mesh)->gt3s; j<(*mesh)->n_gt3s; j++, psxTris++)
		{
			// vertex0
			*vp++ = (KMDWORD)RGBA(psxTris->r0, psxTris->g0, psxTris->b0, 255);
			*((PKMFLOAT)vp) = (KMFLOAT)psxTris->v0 / 127.0f; vp++;
			*((PKMFLOAT)vp) = (KMFLOAT)psxTris->u0 / 127.0f; vp++;
			// vertex1
			*vp++ = (KMDWORD)RGBA(psxTris->r1, psxTris->g1, psxTris->b1, 255);
			*((PKMFLOAT)vp) = (KMFLOAT)psxTris->v1 / 127.0f; vp++;
			*((PKMFLOAT)vp) = (KMFLOAT)psxTris->u1 / 127.0f; vp++;
			// vertex2
			*vp++ = (KMDWORD)RGBA(psxTris->r2, psxTris->g2, psxTris->b2, 255);
			*((PKMFLOAT)vp) = (KMFLOAT)psxTris->v2 / 127.0f; vp++;
			*((PKMFLOAT)vp) = (KMFLOAT)psxTris->u2 / 127.0f; vp++;

			// polygon indices, strip header and texture map.. 1 32byte cache line
			map = &DCKtextureList[psxTris->tpage];
			ip[4] = (int)map;

			// indices
			ip[5] = psxTris->vert2 << 2;
			ip[6] = psxTris->vert1 << 2;
			ip[7] = psxTris->vert0 << 2;
			ip += 8;
		}

		mesh++;
	}
}


/* ---------------------------------------------------------
   Function : MapDraw_Dreamcast_ReleaseWorld
   Purpose : release the pre-compiled dreamcast world
   Parameters :
   Returns : 
   Info : 
*/

void MapDraw_Dreamcast_ReleaseWorld()
{
	if (dcWorldMeshes != NULL)
	{
		while (dcNoofWorldMeshes--)
		{
			FREE(dcWorldMeshes[dcNoofWorldMeshes].vertices);
			FREE(dcWorldMeshes[dcNoofWorldMeshes].quadIndices);
			FREE(dcWorldMeshes[dcNoofWorldMeshes].quadVerts);
			FREE(dcWorldMeshes[dcNoofWorldMeshes].triIndices);
			FREE(dcWorldMeshes[dcNoofWorldMeshes].triVerts);
		}
		FREE(dcWorldMeshes);
	}
	dcNoofWorldMeshes = 0;
	dcWorldMeshes = NULL;
}


/* ---------------------------------------------------------
   Function : MapDraw_Dreamcast_DrawMesh
   Purpose : draw dreamcast pre-compiled world segment mesh
   Parameters : FMA_MESH_HEADER structure pointer, pre-compiled dreamcast mesh poiinter
   Returns : 
   Info : 
*/

static void MapDraw_Dreamcast_DrawMesh(FMA_MESH_HEADER *mesh, TDCWorldMesh *dcMesh)
{
	float	mapmin, mapmax;
	float	x0, x1, x2, x3;
	float	y0, y1, y2, y3;
	float	z0, z1, z2, z3;

	mapCount++;

	// calculate polygon maximum and minimum z depths
	mapmin = MIN_MAP_DEPTH + ((float)mesh->extra_depth);
	mapmax = MAX_MAP_DEPTH + ((float)mesh->extra_depth);
	mapmax = min(mapmax, (1024.0f - ((float)mesh->extra_depth)));

	// transform our map vertices to 
	PSIDC_SH4XD_TransformVertices(dcMesh->vertices, alignedTransformedVertices, dcMesh->noofVerts);

	// push all map world polygons to KAMUI..
	kmxxGetCurrentPtr(&vertexBufferDesc);
	{
		// integer registers
		register int		tvs = (int)alignedTransformedVertices;
		register int		*vis;
		register int		r0;
		register float		*v0, *v1, *v2, *v3;
		register int		*kmvs;
		register int		*visEnd;
		register int		shLast;

		// float registers
		register float		zmin, zmax, fr0, fr1;

		// force first strip set
		shLast = 0;


		// ** Push all quads to KAMUI

		// prefetch our first polygon..
		if (dcMesh->quadIndices)
			prefetch(dcMesh->quadIndices);

		// set quad minimum and maximum z clip.. 1/(4*4)
		zmin = mapmin * 16.0f;
		zmax = mapmax * 16.0f;

		// push all quads..
		kmvs = (int *)dcMesh->quadVerts;
		visEnd = dcMesh->quadIndices + (dcMesh->noofQuadPolys * 8);
		for (vis = dcMesh->quadIndices + 8; vis <= visEnd; vis += 13)
		{
			// prefetch the next polygon
			prefetch(vis);

			// load all this polygons z's
			r0 = *--vis;										// z0
			v0 = (float *)((int)tvs + r0);
			fr0 = *v0;

			r0 = *--vis;										// z1
			v1 = (float *)((int)tvs + r0);
			fr1 = *v1;
			fr0 += fr1;

			r0 = *--vis;										// z2
			v2 = (float *)((int)tvs + r0);
			fr1 = *v2;
			fr0 += fr1;

			r0 = *--vis;										// z3
			v3 = (float *)((int)tvs + r0);
			fr1 = *v3;
			fr0 += fr1;											// z0+z1+z2+z3

			// load ahead this polygons texture map
			r0 = *--vis;

			// z clip check
			if (fr0 < zmin || fr0 > zmax)
			{
				kmvs += 12;
				continue;
			}

			// inc all vertices
			v0++;
			v1++;
			v2++;
			v3++;

			// load all 2d x's
			x0 = *v0++;
			x1 = *v1++;
			x2 = *v2++;
			x3 = *v3++;

			// completely off at the LHS or RHS?
			if ((x0 > 640.0f && x1 > 640.0f && x2 > 640.0f && x3 > 640.0f) ||
				(x0 <   0.0f && x1 <   0.0f && x2 <   0.0f && x3 <   0.0f))
			{
				kmvs += 12;
				continue;
			}

			// load all 2d y's
			y0 = *v0++;
			y1 = *v1++;
			y2 = *v2++;
			y3 = *v3++;

			// completely off at the TOP or BOTTOM?
			if ((y0 > 480.0f && y1 > 480.0f && y2 > 480.0f && y3 > 480.0f) ||
				(y0 <   0.0f && y1 <   0.0f && y2 <   0.0f && y3 <   0.0f))
			{
				kmvs += 12;
				continue;
			}

			// ** Print the polygon

			// load all 1/z's
			z0 = *v0;
			z1 = *v1;
			z2 = *v2;
			z3 = *v3;

			// are we an animated texture map?
			if (!(((TextureType *)r0)->animated))
			{
				// need to change the texture strip header?
				r0 = (int)&((TextureType *)r0)->stripHead;
				if (r0 != shLast)
				{
					// change render state
					kmxxStartStrip(&vertexBufferDesc, r0);
					shLast = r0;
				}
			}
			else
			{
				// check to see if alpha channel is to be used
				if (((TextureType *)r0)->colourKey)
				{
					kmChangeStripTextureSurface(&StripHead_GT4_FMA_Alpha,KM_IMAGE_PARAM1, ((TextureType *)r0)->surfacePtr);
					kmxxStartStrip(&vertexBufferDesc, &StripHead_GT4_FMA_Alpha);
				}
				else
				{
					kmChangeStripTextureSurface(&StripHead_GT4_FMA,KM_IMAGE_PARAM1, ((TextureType *)r0)->surfacePtr);
					kmxxStartStrip(&vertexBufferDesc, &StripHead_GT4_FMA);
				}
				shLast = 0;
			}

			// vertex 0
			r0 = *kmvs++;										// ->rgb
			pkmCurrentPtr += 8;
			*--pkmCurrentPtr = r0;
			*--pkmCurrentPtr = r0;
			fr0 = *((float *)kmvs); kmvs++;						// ->v
			fr1 = *((float *)kmvs); kmvs++;						// ->u
			*(PKMFLOAT)--pkmCurrentPtr = fr0;
			*(PKMFLOAT)--pkmCurrentPtr = fr1;
			*(PKMFLOAT)--pkmCurrentPtr = z0;
			*(PKMFLOAT)--pkmCurrentPtr = y0;
			*(PKMFLOAT)--pkmCurrentPtr = x0;
			*--pkmCurrentPtr = KM_VERTEXPARAM_NORMAL;
			prefetch((void *)pkmCurrentPtr);
			pkmCurrentPtr += 8+8;

			// vertex 1
			r0 = *kmvs++;										// ->rgb
			*--pkmCurrentPtr = r0;
			*--pkmCurrentPtr = r0;
			fr0 = *((float *)kmvs); kmvs++;						// ->v
			fr1 = *((float *)kmvs); kmvs++;						// ->u
			*(PKMFLOAT)--pkmCurrentPtr = fr0;
			*(PKMFLOAT)--pkmCurrentPtr = fr1;
			*(PKMFLOAT)--pkmCurrentPtr = z1;
			*(PKMFLOAT)--pkmCurrentPtr = y1;
			*(PKMFLOAT)--pkmCurrentPtr = x1;
			*--pkmCurrentPtr = KM_VERTEXPARAM_NORMAL;
			prefetch((void *)pkmCurrentPtr);
			pkmCurrentPtr += 8+8;

			// vertex 2
			r0 = *kmvs++;										// ->rgb
			*--pkmCurrentPtr = r0;
			*--pkmCurrentPtr = r0;
			fr0 = *((float *)kmvs); kmvs++;						// ->v
			fr1 = *((float *)kmvs); kmvs++;						// ->u
			*(PKMFLOAT)--pkmCurrentPtr = fr0;
			*(PKMFLOAT)--pkmCurrentPtr = fr1;
			*(PKMFLOAT)--pkmCurrentPtr = z2;
			*(PKMFLOAT)--pkmCurrentPtr = y2;
			*(PKMFLOAT)--pkmCurrentPtr = x2;
			*--pkmCurrentPtr = KM_VERTEXPARAM_NORMAL;
			prefetch((void *)pkmCurrentPtr);
			pkmCurrentPtr += 8+8;

			// vertex 3
			r0 = *kmvs++;										// ->rgb
			*--pkmCurrentPtr = r0;
			*--pkmCurrentPtr = r0;
			fr0 = *((float *)kmvs); kmvs++;						// ->v
			fr1 = *((float *)kmvs); kmvs++;						// ->u
			*(PKMFLOAT)--pkmCurrentPtr = fr0;
			*(PKMFLOAT)--pkmCurrentPtr = fr1;
			*(PKMFLOAT)--pkmCurrentPtr = z3;
			*(PKMFLOAT)--pkmCurrentPtr = y3;
			*(PKMFLOAT)--pkmCurrentPtr = x3;
			*--pkmCurrentPtr = KM_VERTEXPARAM_ENDOFSTRIP;
			prefetch((void *)pkmCurrentPtr);
			pkmCurrentPtr += 8;
		}


		// ** Push all triangles to KAMUI

		// prefetch our first polygon..
		if (dcMesh->triIndices)
			prefetch(dcMesh->triIndices);

		// set triangle minimum and maximum z clip.. 1/(3*4)
		zmin = mapmin * 12.0f;
		zmax = mapmax * 12.0f;

		// push all triangles..
		kmvs = (int *)dcMesh->triVerts;
		visEnd = dcMesh->triIndices + (dcMesh->noofTriPolys * 8);
		for (vis = dcMesh->triIndices + 8; vis <= visEnd; vis += 12)
		{
			// prefetch the next polygon
			prefetch(vis);

			// load all this polygons z's
			r0 = *--vis;										// z0
			v0 = (float *)((int)tvs + r0);
			fr0 = *v0;

			r0 = *--vis;										// z1
			v1 = (float *)((int)tvs + r0);
			fr1 = *v1;
			fr0 += fr1;

			r0 = *--vis;										// z2
			v2 = (float *)((int)tvs + r0);
			fr1 = *v2;
			fr0 += fr1;											// z0+z1+z2

			// load ahead this polygons texture map
			r0 = *--vis;

			// z clip check
			if (fr0 < zmin || fr0 > zmax)
			{
				kmvs += 9;
				continue;
			}

			// inc all vertices
			v0++;
			v1++;
			v2++;

			// load all 2d x's
			x0 = *v0++;
			x1 = *v1++;
			x2 = *v2++;

			// completely off at the LHS or RHS?
			if ((x0 > 640.0f && x1 > 640.0f && x2 > 640.0f) ||
				(x0 <   0.0f && x1 <   0.0f && x2 <   0.0f))
			{
				kmvs += 9;
				continue;
			}

			// load all 2d y's
			y0 = *v0++;
			y1 = *v1++;
			y2 = *v2++;

			// completely off at the TOP or BOTTOM?
			if ((y0 > 480.0f && y1 > 480.0f && y2 > 480.0f) ||
				(y0 <   0.0f && y1 <   0.0f && y2 <   0.0f))
			{
				kmvs += 9;
				continue;
			}

			// ** Print the polygon

			// load all 1/z's
			z0 = *v0;
			z1 = *v1;
			z2 = *v2;

			// are we an animated texture map?
			if (!(((TextureType *)r0)->animated))
			{
				// need to change the texture strip header?
				r0 = (int)&((TextureType *)r0)->stripHead;
				if (r0 != shLast)
				{
					// change render state
					kmxxStartStrip(&vertexBufferDesc, r0);
					shLast = r0;
				}
			}
			else
			{
				// check to see if alpha channel is to be used
				if (((TextureType *)r0)->colourKey)
				{
					kmChangeStripTextureSurface(&StripHead_GT4_FMA_Alpha,KM_IMAGE_PARAM1, ((TextureType *)r0)->surfacePtr);
					kmxxStartStrip(&vertexBufferDesc, &StripHead_GT4_FMA_Alpha);
				}
				else
				{
					kmChangeStripTextureSurface(&StripHead_GT4_FMA,KM_IMAGE_PARAM1, ((TextureType *)r0)->surfacePtr);
					kmxxStartStrip(&vertexBufferDesc, &StripHead_GT4_FMA);
				}
				shLast = 0;
			}

			// vertex 0
			r0 = *kmvs++;										// ->rgb
			pkmCurrentPtr += 8;
			*--pkmCurrentPtr = r0;
			*--pkmCurrentPtr = r0;
			fr0 = *((float *)kmvs); kmvs++;						// ->v
			fr1 = *((float *)kmvs); kmvs++;						// ->u
			*(PKMFLOAT)--pkmCurrentPtr = fr0;
			*(PKMFLOAT)--pkmCurrentPtr = fr1;
			*(PKMFLOAT)--pkmCurrentPtr = z0;
			*(PKMFLOAT)--pkmCurrentPtr = y0;
			*(PKMFLOAT)--pkmCurrentPtr = x0;
			*--pkmCurrentPtr = KM_VERTEXPARAM_NORMAL;
			prefetch((void *)pkmCurrentPtr);
			pkmCurrentPtr += 8+8;

			// vertex 1
			r0 = *kmvs++;										// ->rgb
			*--pkmCurrentPtr = r0;
			*--pkmCurrentPtr = r0;
			fr0 = *((float *)kmvs); kmvs++;						// ->v
			fr1 = *((float *)kmvs); kmvs++;						// ->u
			*(PKMFLOAT)--pkmCurrentPtr = fr0;
			*(PKMFLOAT)--pkmCurrentPtr = fr1;
			*(PKMFLOAT)--pkmCurrentPtr = z1;
			*(PKMFLOAT)--pkmCurrentPtr = y1;
			*(PKMFLOAT)--pkmCurrentPtr = x1;
			*--pkmCurrentPtr = KM_VERTEXPARAM_NORMAL;
			prefetch((void *)pkmCurrentPtr);
			pkmCurrentPtr += 8+8;

			// vertex 2
			r0 = *kmvs++;										// ->rgb
			*--pkmCurrentPtr = r0;
			*--pkmCurrentPtr = r0;
			fr0 = *((float *)kmvs); kmvs++;						// ->v
			fr1 = *((float *)kmvs); kmvs++;						// ->u
			*(PKMFLOAT)--pkmCurrentPtr = fr0;
			*(PKMFLOAT)--pkmCurrentPtr = fr1;
			*(PKMFLOAT)--pkmCurrentPtr = z2;
			*(PKMFLOAT)--pkmCurrentPtr = y2;
			*(PKMFLOAT)--pkmCurrentPtr = x2;
			*--pkmCurrentPtr = KM_VERTEXPARAM_ENDOFSTRIP;
			prefetch((void *)pkmCurrentPtr);
			pkmCurrentPtr += 8;
		}
	}
	kmxxReleaseCurrentPtr(&vertexBufferDesc);
}
