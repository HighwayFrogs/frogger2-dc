
#include "sonylibs.h"
#include "shell.h"

#include "islpsi.h"
#include "psitypes.h"

#include "enemies.h"
//#include <islutil.h>
#include "bbtimer.h"

BBTIMER poly;

extern long *transformedVertices;
extern long *transformedDepths;
extern float *transformedDepths2;
extern VERT *transformedNormals;
extern PSIMODELCTRL	PSImodelctrl;

extern unsigned long *sortedIndex;
extern int sortCount;

#define WATER_TRANS_CODE (((ULONG)2)<<24)
#define WATER_TRANS_CLUT (0x80000000)

//int polyCount = 0;

// SL: This is the pre-made up all black palette.
const USHORT EXPLORE_black_ref_palette[16] =
//								{	0x4210,0x4210,0x4210,0x4210,	// rgb = 127	// this version does no trans!
								{	0x8000,0x4210,0x4210,0x4210,	// rgb = 127
									0x4210,0x4210,0x4210,0x4210,
									0x4210,0x4210,0x4210,0x4210,
									0x4210,0x4210,0x4210,0x4210
								};

USHORT EXPLORE_black_CLUT;

/**************************************************************************
	FUNCTION:	customDrawPrimitives2
	PURPOSE:	add list of polys into ot using sortlists
	PARAMETERS:	ot (at depth)
	RETURNS:	
**************************************************************************/

void customDrawPrimitives2(int depth)
{
/*	register PACKET		*packet;
	register long		*tfv = transformedVertices;
	VERT 				*tfn = transformedNormals;
	register TMD_P_GT4I	*opcd;
	long				clipflag;
	PSIMODELCTRL		*modctrl = &PSImodelctrl;
	VERT				*vp = modctrl->VertTop;
	int					prims,primsleft,lightmode;
	USHORT				*sorts;
	long				*tfd = transformedDepths;
	
	//utilPrintf("Bugger\n");

	depth=depth>>2;

	prims = (int)modctrl->PrimTop;
	sorts = modctrl->SortOffs;
	primsleft = modctrl->PrimLeft;
	lightmode = modctrl->lighting;

	while(primsleft)
	{	
		opcd = (TMD_P_GT4I*)( prims + (*sorts++<<3) );
		switch (opcd->cd & (0xff-2))
		{
#define si ((POLY_FT3*)packet)
#define op ((TMD_P_FT3I*)opcd)
			case GPU_COM_TF3:

//				BEGINPRIM(si, POLY_FT3);

				gte_ldsxy3(tfv[op->v0], tfv[op->v1], tfv[op->v2]);		// Load 1st three vertices
			
				*(u_long *)  (&si->u0) = *(u_long *) (&op->tu0);		// Texture coords
			
				gte_nclip_b();	// takes 8 cycles

				*(u_long *)  (&si->u2) = *(u_long *) (&op->tu2);

	 			gte_stopz(&clipflag);
			
				if ( !(op->dummy & psiDOUBLESIDED) && (clipflag >= 0) )
					break;									// Back face culling

				*(u_long *)  (&si->u1) = *(u_long *) (&op->tu1);
				gte_stsxy3_ft3(si);
			
				switch (lightmode)
				{
					case DIRECTIONAL:
						gte_ldrgb(&op->r0);
						gte_ldv0(&tfn[op->v0]);
						gte_nccs();			
						gte_strgb(&si->r0);
						break;
					case DIRECTIONONLY:
						gte_ldv0(&tfn[op->v0]);
						gte_ncs();			
						gte_strgb(&si->r0);
						break;
					case COLOURIZE:
						*(u_long *)  (&si->r0) = *(u_long *) (&modctrl->col);
						break;
					default:
						*(u_long *) (&si->r0) = *(u_long *) (&op->r0);		// 9 cycles here
		 		}
				setPolyFT3(si);
				si->code = op->cd | modctrl->semitrans;

// 				ENDPRIM(si, depth & 1023, POLY_FT3);
				modctrl->polysdrawn++;
				break;
#undef si
#undef op
#define si ((POLY_FT4*)packet)
#define op ((TMD_P_FT4I*)opcd)
			case GPU_COM_TF4:

//				BEGINPRIM(si, POLY_FT4);
   			
				gte_ldsxy3(tfv[op->v0], tfv[op->v1], tfv[op->v2]);		// Load 1st three vertices
			
				*(u_long *)  (&si->u0) = *(u_long *) (&op->tu0);		// Texture coords

				gte_nclip_b();	// takes 8 cycles
		
				*(u_long *)  (&si->u1) = *(u_long *) (&op->tu1);

				gte_stopz(&clipflag);

				if ( !(op->dummy & psiDOUBLESIDED) && (clipflag >= 0) )
					break;	// Back face culling
				
				gte_stsxy3_ft4(si);

				*(u_long *)  (&si->x3) = *(u_long *) (&tfv[op->v3]);

				switch (lightmode)
				{
					case DIRECTIONAL:
						gte_ldrgb(&op->r0);
						gte_ldv0(&tfn[op->v0]);
						gte_nccs();			
						gte_strgb(&si->r0);
						break;
					case DIRECTIONONLY:
						gte_ldv0(&tfn[op->v0]);
						gte_ncs();			
						gte_strgb(&si->r0);
						break;
					case COLOURIZE:
						*(u_long *)  (&si->r0) = *(u_long *) (&modctrl->col);
						break;
					default:
						*(u_long *) (&si->r0) = *(u_long *) (&op->r0);		// 9 cycles here
				}
				*(u_long *)  (&si->u2) = *(u_long *) (&op->tu2);
				*(u_long *)  (&si->u3) = *(u_long *) (&op->tu3);

				setPolyFT4(si);
				si->code = op->cd | modctrl->semitrans;
				
				modctrl->polysdrawn++;
 			
// 				ENDPRIM(si, depth & 1023, POLY_FT4);
				break;
#undef si
#undef op
#define si ((POLY_GT3*)packet)
#define op ((TMD_P_GT3I*)opcd)
			case GPU_COM_TG3:
			
//				BEGINPRIM(si, POLY_GT3);

				gte_ldsxy3(tfv[op->v0], tfv[op->v1], tfv[op->v2]);		// Load 1st three vertices
				
				*(u_long *)  (&si->u0) = *(u_long *) (&op->tu0);		// Texture coords

				gte_nclip_b();	// takes 8 cycles
				
				*(u_long *)  (&si->u2) = *(u_long *) (&op->tu2);
					
				gte_stopz(&clipflag);
				
				if (!(op->dummy & psiDOUBLESIDED) && (clipflag >= 0) )
					break;								// Back face culling

				gte_stsxy3_gt3(si);
			

				*(u_long *)  (&si->u1) = *(u_long *) (&op->tu1);

				switch (lightmode)
				{
					case DIRECTIONAL:
						gte_ldrgb3(&op->r0, &op->r1, &op->r2);
						gte_ldv3(&tfn[op->v0], &tfn[op->v1], &tfn[op->v2]);
						gte_ncct();
						gte_strgb3(&si->r0, &si->r1, &si->r2);
						break;
					case DIRECTIONONLY:
						gte_ldv3(&tfn[op->v0], &tfn[op->v1], &tfn[op->v2]);
						gte_nct();
						gte_strgb3(&si->r0, &si->r1, &si->r2);
						break;
					case COLOURIZE:
						*(u_long *)  (&si->r0) = *(u_long *) (&modctrl->col);
						*(u_long *)  (&si->r1) = *(u_long *) (&modctrl->col);
						*(u_long *)  (&si->r2) = *(u_long *) (&modctrl->col);
						break;
					default:
						*(u_long *)  (&si->r0) = *(u_long *) (&op->r0);
						*(u_long *)  (&si->r1) = *(u_long *) (&op->r1);
						*(u_long *)  (&si->r2) = *(u_long *) (&op->r2);
				}
				setPolyGT3(si);
				si->code = op->cd | modctrl->semitrans;
			
				modctrl->polysdrawn++;

//				ENDPRIM(si, depth & 1023, POLY_GT3);
				break;
 		
#undef si
#undef op
#define si ((POLY_GT4*)packet)
#define op opcd
			case GPU_COM_TG4:
		
//				BEGINPRIM(si, POLY_GT4);

				gte_ldsxy3(tfv[op->v0], tfv[op->v1], tfv[op->v2]);		// Load 1st three vertices
				
				*(u_long *)  (&si->u0) = *(u_long *) (&op->tu0);		// Texture coords
			
				gte_nclip_b();	// takes 8 cycles
		
				*(u_long *)  (&si->u1) = *(u_long *) (&op->tu1);
					
				gte_stopz(&clipflag);
				
				if (clipflag >= 0) break;								// Back face culling

				gte_stsxy3_gt4(si);
				
						
				*(u_long *)  (&si->u2) = *(u_long *) (&op->tu2);
				*(u_long *)  (&si->u3) = *(u_long *) (&op->tu3);


				*(u_long *)  (&si->x3) = *(u_long *) (&tfv[op->v3]);
		
		
				switch (lightmode)
				{
					case DIRECTIONAL:
						gte_ldrgb3(&op->r0, &op->r1, &op->r2);
						gte_ldv3(&tfn[op->v0], &tfn[op->v1], &tfn[op->v2]);
						gte_ncct();
						gte_strgb3(&si->r0, &si->r1, &si->r2);
						
						gte_ldrgb(&op->r3);
						gte_ldv0(&tfn[op->v3]);
						gte_nccs();			// NormalColorCol
						gte_strgb(&si->r3);
						break;
					case DIRECTIONONLY:
						gte_ldv3(&tfn[op->v0], &tfn[op->v1], &tfn[op->v2]);
						gte_nct();
						gte_strgb3(&si->r0, &si->r1, &si->r2);

						gte_ldv0(&tfn[op->v3]);
						gte_ncs();			
						gte_strgb(&si->r3);
						break;
					case COLOURIZE:
						*(u_long *)  (&si->r0) = *(u_long *) (&modctrl->col);
						*(u_long *)  (&si->r1) = *(u_long *) (&modctrl->col);
						*(u_long *)  (&si->r2) = *(u_long *) (&modctrl->col);
						*(u_long *)  (&si->r3) = *(u_long *) (&modctrl->col);
						break;
					default:
						*(u_long *)  (&si->r0) = *(u_long *) (&op->r0);
						*(u_long *)  (&si->r1) = *(u_long *) (&op->r1);
						*(u_long *)  (&si->r2) = *(u_long *) (&op->r2);
						*(u_long *)  (&si->r3) = *(u_long *) (&op->r3);
				}
		
				modctrl->polysdrawn++;
			
				setPolyGT4(si);
				si->code = op->cd | modctrl->semitrans;
//				ENDPRIM(si, depth & 1023, POLY_GT4);
				break;
#undef si
#undef op
#define si ((POLY_FT4*)packet)
#define op ((TMD_P_FT4I*)opcd)
			case GPU_COM_TF4SPR :
		   	{
				SHORT		spritez;
				int		width, height;
				TMD_P_FT4I *testpol;
				POLY_FT4 *testsi;

				si = (POLY_FT4*)packet;
				op = (TMD_P_FT4I*)(opcd);

//				BEGINPRIM(si, POLY_FT4);

				testpol = op;
				testsi = si;

	// scaling-and-transform-in-one-go code from the Action Man people...
				width = op->v1;
				gte_SetLDDQB(0);			// clear offset control reg (C2_DQB)
				gte_ldv0(&vp[op->v0]);		// Load centre point
				gte_SetLDDQA(width);		// shove sprite width into control reg (C2_DQA)
				gte_rtps();					// do the rtps
				gte_stsxy(&si->x0);			// get screen x and y
				gte_stsz(&spritez);		// get order table z
	// end of scaling-and-transform


	// tbd - make this ditch according to on-screen SIZE
	// limit to "max poly depth", and we can ditch the "MAXDEPTH" "and" below...


				if (spritez < 20)
					break;

				gte_stopz(&width);		// get scaled width of sprite
				width >>= 17;

			

 				*(u_long *) & si->r0 = *(u_long *) & op->r0;			// Texture coords / colors
				*(u_long *) & si->u0 = *(u_long *) & op->tu0;
				*(u_long *) & si->u1 = *(u_long *) & op->tu1;
				*(u_long *) & si->u2 = *(u_long *) & op->tu2;
				*(u_long *) & si->u3 = *(u_long *) & op->tu3;

				si->x1 = si->x3=si->x0+width;
				si->x0 = si->x2=si->x0-width;
			
 		 		height = width>>1;//(LONG)(width*(3))/spritez;
			
				si->y2 = si->y3=si->y0+height;
				si->y0 = si->y1=si->y0-height;

				setPolyFT4(si);
				si->code = GPU_COM_TF4 | modctrl->semitrans;

//				ENDPRIM(si, depth & 1023, POLY_FT4);
				break;
			}	
#undef si
#undef op
#define si ((POLY_G4*)packet)
#define op ((TMD_P_FG4I*)opcd)
			case GPU_COM_G4:
			case GPU_COM_F4:
				
//				BEGINPRIM(si, POLY_G4);
   			
				gte_ldsxy3(tfv[op->v0], tfv[op->v1], tfv[op->v2]);		// Load 1st three vertices

				*(u_long *)  (&si->x3) = *(u_long *) (&tfv[op->v3]);
 
 				gte_nclip_b();	// takes 8 cycles
		

				gte_stopz(&clipflag);

				// Back face culling
				if ( !(op->dummy & psiDOUBLESIDED) && (clipflag >= 0) ) break;									// Back face culling
				
				gte_stsxy3_g4(si);


				switch (lightmode)
				{
					case DIRECTIONAL:
						gte_ldrgb3(&op->r0, &op->r1, &op->r2);
						gte_ldv3(&tfn[op->v0], &tfn[op->v1], &tfn[op->v2]);
						gte_ncct();
						gte_strgb3(&si->r0, &si->r1, &si->r2);
						
						gte_ldrgb(&op->r3);
						gte_ldv0(&tfn[op->v3]);
						gte_nccs();			// NormalColorCol
						gte_strgb(&si->r3);
						break;
					case DIRECTIONONLY:
						gte_ldv3(&tfn[op->v0], &tfn[op->v1], &tfn[op->v2]);
						gte_nct();
						gte_strgb3(&si->r0, &si->r1, &si->r2);

						gte_ldv0(&tfn[op->v3]);
						gte_ncs();			
						gte_strgb(&si->r3);
						break;
					case COLOURIZE:
						*(u_long *)  (&si->r0) = *(u_long *) (&modctrl->col);
						*(u_long *)  (&si->r1) = *(u_long *) (&modctrl->col);
						*(u_long *)  (&si->r2) = *(u_long *) (&modctrl->col);
						*(u_long *)  (&si->r3) = *(u_long *) (&modctrl->col);
						break;
					default:
						*(u_long *)  (&si->r0) = *(u_long *) (&op->r0);
						*(u_long *)  (&si->r1) = *(u_long *) (&op->r1);
						*(u_long *)  (&si->r2) = *(u_long *) (&op->r2);
						*(u_long *)  (&si->r3) = *(u_long *) (&op->r3);
				}

				setPolyG4(si);
				si->code = op->cd | modctrl->semitrans;

				modctrl->polysdrawn++;

//				ENDPRIM(si, depth & 1023, POLY_G4);
				break;
#undef si
#undef op
#define si ((POLY_G3*)packet)
#define op ((TMD_P_FG3I*)opcd)
			
			case GPU_COM_G3:
			case GPU_COM_F3:

//				BEGINPRIM(si, POLY_G3);
   			
				gte_ldsxy3(tfv[op->v0], tfv[op->v1], tfv[op->v2]);		// Load 1st three vertices

 
				*(u_long *)  (&si->r0) = *(u_long *) (&op->r0);
 				gte_nclip_b();	// takes 8 cycles
		

				gte_stopz(&clipflag);

	//			if (clipflag >= 0) break; 								// Back face culling
				if ( !(op->dummy & psiDOUBLESIDED) && (clipflag >= 0) ) break;									// Back face culling
				
				gte_stsxy3_g3(si);


				switch (lightmode)
				{
					case DIRECTIONAL:
						gte_ldrgb3(&op->r0, &op->r1, &op->r2);
						gte_ldv3(&tfn[op->v0], &tfn[op->v1], &tfn[op->v2]);
						gte_ncct();
						gte_strgb3(&si->r0, &si->r1, &si->r2);
						break;
					case DIRECTIONONLY:
						gte_ldv3(&tfn[op->v0], &tfn[op->v1], &tfn[op->v2]);
						gte_nct();
						gte_strgb3(&si->r0, &si->r1, &si->r2);
						break;
					case COLOURIZE:
						*(u_long *)  (&si->r0) = *(u_long *) (&modctrl->col);
						*(u_long *)  (&si->r1) = *(u_long *) (&modctrl->col);
						*(u_long *)  (&si->r2) = *(u_long *) (&modctrl->col);
						break;
					default:
						*(u_long *)  (&si->r1) = *(u_long *) (&op->r1);
						*(u_long *)  (&si->r2) = *(u_long *) (&op->r2);
				}


				modctrl->polysdrawn++;

				setPolyG3(si);
				si->code = op->cd | modctrl->semitrans;

//				ENDPRIM(si, depth & 1023, POLY_G3);
				break;
#undef si
#undef op
			default:
				break;
		}
		primsleft--;
	}
*/	
}


extern long globalFadeVal;

// void LSCAPE_DrawSortedPrimitives(int depth)
// {
// 	register PACKET			*packet, *packetNext;
// 	register long			*tfv = transformedVertices;
// 	register long			*tfd = transformedDepths;
// 	VERT 							*tfn = transformedNormals;
// 	register TMD_P_GT4I		*opcd;
// 	PSIMODELCTRL			*modctrl = &PSImodelctrl;
// 	int						primsleft,lightmode;
// 	ULONG					*sorts = sortedIndex;
// //	ULONG					*sorts = modctrl->SortOffs;
// 	ULONG					sortBucket = 0;
// 	VERT					*vp = modctrl->VertTop;
// 	LONG					fogdepth;
// 	unsigned short			oldtpage;
// 	long					fogFade;
// 	DR_MODE					*dr_mode;
// 	u_long t1;
// 
// 	int						gteH;
// 
// 
// 	gte_ReadGeomScreen(&gteH);
// 
// 	depth=depth>>2;
// 
// 	// SL: get the fogdepth, and modge it accordingly.
// 	fogdepth = 10000;//(depth << (2+1/*LSCAPE_Data.depthshift*/));
// 
// //	if(fogdepth > LSCAPE_Data.fogendz)
// //		return;
// 
// 	// SL: work out the fog fade. in advance. It's the same for the whole model, so this is fine here.
// 	fogFade=globalFadeVal;//255-((255*(fogdepth-3000/*-LSCAPE_Data.fogstartz*/))/(3500-3000/*LSCAPE_Data.fogendz-LSCAPE_Data.fogstartz*/));
// 	// SL: shift everythang into the same rez as the landscape, etc...
// //	depth = (depth >> LSCAPE_Data.depthshift) & MAXDEPTH;
// 
// 	
// 	primsleft = sortCount;
// 	if (!primsleft)
// 		return;
// 
// 	polyCount += primsleft;	
// 
// 	lightmode = modctrl->lighting;
// 	opcd = 0;
// //	TimerStart(&poly);
// 	//utilPrintf("Polys To Draw : %d\n", primsleft);
// 	while(primsleft)
// 	{
// 		if (opcd==0)
// 		{
// 			(int)opcd = sorts[sortBucket++];
// 			continue;
// 		}
// 
// 		primsleft--;
// 
// 		switch (opcd->cd & (0xff-2))
// 		{
// /*-----------------------------------------------------------------------------------------------------------------*/
// #define si ((POLY_FT3*)packet)
// #define siNext ((POLY_FT3*)packetNext)
// #define op ((TMD_P_FT3I*)opcd)
// 
// 			case GPU_COM_TF3:
// 
// 				BEGINPRIM(si, POLY_FT3);
// 
// 				gte_ldsxy3(tfv[op->v0], tfv[op->v1], tfv[op->v2]);		// Load 1st three vertices
// 			
// 				*(u_long *)  (&si->u0) = *(u_long *) (&op->tu0);		// Texture coords
// 				*(u_long *)  (&si->u1) = *(u_long *) (&op->tu1);
// 				*(u_long *)  (&si->u2) = *(u_long *) (&op->tu2);
// 
// 				gte_stsxy3_ft3(si);
// 			
// 				switch (lightmode)
// 				{
// 					case DIRECTIONAL:
// 						gte_ldrgb(&op->r0);
// 						gte_ldv0(&tfn[op->v0]);
// 						gte_nccs();			
// 						gte_strgb(&si->r0);
// 						break;
// 					case DIRECTIONONLY:
// 						gte_ldv0(&tfn[op->v0]);
// 						gte_ncs();
// 						gte_strgb(&si->r0);
// 						break;
// 					case COLOURIZE:
// 						*(u_long *)  (&si->r0) = *(u_long *) (&modctrl->col);
// 						break;
// 					default:
// 						*(u_long *) (&si->r0) = *(u_long *) (&op->r0);
// 		 		}
// 
// 
// 
// // FOG CODE: --------------------------------------------------------
// 
// 				// SL: if it's near enough to not need fogging, or it's already additive or subtractive...
// 				if ( (fogFade >= 255 ))
// 				{
// 					// SL: put in the additive poly...
// 					setPolyFT3(si);
// 					si->code = op->cd | modctrl->semitrans;
// 					ENDPRIM(si, depth, POLY_FT3);
// 				}
// 				else
// 				{
// 	/*				// SL: modge the RGBs accordingly
// 					si->r0 = ((fogFade*(short)si->r0)/255);
// 					si->g0 = ((fogFade*(short)si->g0)/255);
// 					si->b0 = ((fogFade*(short)si->b0)/255);
// 
// /*					t1 = (op->clut<<16)|WATER_TRANS_CLUT;
// 					*(u_long *)  (&si->u0) = t1;
// 
// 					t1 = (*(u_long *) (&op->r0))|WATER_TRANS_CODE;
// 
// 					*(u_long *)  (&si->r0) = t1;*/
// 
// 					// SL: put in the additive poly...
// /*					setPolyFT3(si);
// 					// SL: store the pre-modification tpage...
// 					oldtpage = si->tpage;
// 					// make it additive...
// 					si->code |= 2;
// 					si->tpage |= 32;
// 					// SL: put it in t' table
// 					ENDPRIM(si, depth, POLY_FT3);
// 
// 					// SL: hokay, setup the next poly...
// 
// 					BEGINPRIM(siNext, POLY_FT3);
// 
// 					// SL: copy in all the values using long copies...
// 					*(u_long *)  (&siNext->x0) = *(u_long *) (&si->x0);
// 					*(u_long *)  (&siNext->x1) = *(u_long *) (&si->x1);
// 					*(u_long *)  (&siNext->x2) = *(u_long *) (&si->x2);
// 					*(u_long *)  (&siNext->u0) = *(u_long *) (&si->u0);		// Texture coords
// 					*(u_long *)  (&siNext->u1) = *(u_long *) (&si->u1);
// 					*(u_long *)  (&siNext->u2) = *(u_long *) (&si->u2);
// 					*(u_long *)  (&siNext->r0) = *(u_long *) (&si->r0);			// SL: RGBs
// 
// 					// SL: set it as the right type
// 					setPolyFT3(siNext);
// 					// SL: make it subtractive
// 					siNext->clut = EXPLORE_black_CLUT;
// 					siNext->code |= 2;
// 					siNext->tpage = oldtpage | 64;
// 
// 					// SL: put it in t' table
// 					ENDPRIM(siNext, depth, POLY_FT3);*/
// 				}
// 
// 
// 				// SL: move the prim pointer on one...
// 				op = op->next;
// 				break;
// #undef si
// #undef siNext
// #undef op
// /*-----------------------------------------------------------------------------------------------------------------*/
// #define si ((POLY_FT4*)packet)
// #define siNext ((POLY_FT4*)packetNext)
// #define op ((TMD_P_FT4I*)opcd)
// 				
// 				case GPU_COM_TF4:
// 
// 				BEGINPRIM(si, POLY_FT4);
//    			
// 				gte_ldsxy3(tfv[op->v0], tfv[op->v1], tfv[op->v2]);		// Load 1st three vertices
// 			
// 				*(u_long *)  (&si->u0) = *(u_long *) (&op->tu0);		// Texture coords
// 				*(u_long *)  (&si->u1) = *(u_long *) (&op->tu1);
// 
// 				gte_stsxy3_ft4(si);
// 
// 				*(u_long *)  (&si->x3) = *(u_long *) (&tfv[op->v3]);
// 
// 				switch (lightmode)
// 				{
// 					case DIRECTIONAL:
// 						gte_ldrgb(&op->r0);
// 						gte_ldv0(&tfn[op->v0]);
// 						gte_nccs();			
// 						gte_strgb(&si->r0);
// 						break;
// 					case DIRECTIONONLY:
// 						gte_ldv0(&tfn[op->v0]);
// 						gte_ncs();			
// 						gte_strgb(&si->r0);
// 						break;
// 					case COLOURIZE:
// 						*(u_long *)  (&si->r0) = *(u_long *) (&modctrl->col);
// 						break;
// 					default:
// 						*(u_long *) (&si->r0) = *(u_long *) (&op->r0);		// 9 cycles here
// 				}
// 				*(u_long *)  (&si->u2) = *(u_long *) (&op->tu2);
// 				*(u_long *)  (&si->u3) = *(u_long *) (&op->tu3);
// 
// // FOG CODE: --------------------------------------------------------
// 
// 				// SL: if it's near enough to not need fogging, or it's already additive or subtractive...
// 				if ( (fogFade >= 255))
// 				{
// 					// SL: put in the additive poly...
// 					setPolyFT4(si);
// 					si->code = op->cd | modctrl->semitrans;
// 					ENDPRIM(si, depth, POLY_FT4);
// 				}
// 				else
// 				{
// 					// SL: modge the RGBs accordingly
// /*					si->r0 = ((fogFade*(short)si->r0)/255);
// 					si->g0 = ((fogFade*(short)si->g0)/255);
// 					si->b0 = ((fogFade*(short)si->b0)/255);
// 
// 					// SL: put in the additive poly...
// 					setPolyFT4(si);
// 					// SL: store the pre-modification tpage...
// 					oldtpage = si->tpage;
// 					// make it additive...
// 					si->code |= 2;
// 					si->tpage |= 32;
// 					// SL: put it in t' table
// 					ENDPRIM(si, depth, POLY_FT4);
// 
// 					// SL: hokay, setup the next poly...
// 					BEGINPRIM(siNext, POLY_FT4);
// 
// 					// SL: copy in all the values using long copies...
// 					*(u_long *)  (&siNext->x0) = *(u_long *) (&si->x0);
// 					*(u_long *)  (&siNext->x1) = *(u_long *) (&si->x1);
// 					*(u_long *)  (&siNext->x2) = *(u_long *) (&si->x2);
// 					*(u_long *)  (&siNext->x3) = *(u_long *) (&si->x3);
// 					*(u_long *)  (&siNext->u0) = *(u_long *) (&si->u0);		// Texture coords
// 					*(u_long *)  (&siNext->u1) = *(u_long *) (&si->u1);
// 					*(u_long *)  (&siNext->u2) = *(u_long *) (&si->u2);
// 					*(u_long *)  (&siNext->u3) = *(u_long *) (&si->u3);
// 					*(u_long *)  (&siNext->r0) = *(u_long *) (&si->r0);			// SL: RGBs
// 
// 					// SL: set it as the right type
// 					setPolyFT4(siNext);
// 					// SL: make it subtractive
// 					siNext->clut = EXPLORE_black_CLUT;
// 					siNext->code |= 2;
// 					siNext->tpage = oldtpage | 64;
// 
// 					// SL: put it in t' table
// 					ENDPRIM(siNext, depth, POLY_FT4);*/
// 				}
// 
// // END FOG CODE: --------------------------------------------------------
// 
// 				op = op->next;
// 				break;
// #undef si
// #undef siNext
// #undef op
// /*-----------------------------------------------------------------------------------------------------------------*/
// #define si ((POLY_GT3*)packet)
// #define siNext ((POLY_GT3*)packetNext)
// #define op ((TMD_P_GT3I*)opcd)
// 
// 			case GPU_COM_TG3:
// 
// 				BEGINPRIM(si, POLY_GT3);
// 
// 				gte_ldsxy3(tfv[op->v0], tfv[op->v1], tfv[op->v2]);		// Load 1st three vertices
// 				
// 				*(u_long *)  (&si->u0) = *(u_long *) (&op->tu0);		// Texture coords
// 				*(u_long *)  (&si->u1) = *(u_long *) (&op->tu1);
// 
// 				gte_stsxy3_gt3(si);
// 
// 				*(u_long *)  (&si->u2) = *(u_long *) (&op->tu2);
// 
// 				switch (lightmode)
// 				{
// 					case DIRECTIONAL:
// 						gte_ldrgb3(&op->r0, &op->r1, &op->r2);
// 						gte_ldv3(&tfn[op->v0], &tfn[op->v1], &tfn[op->v2]);
// 						gte_ncct();
// 						gte_strgb3(&si->r0, &si->r1, &si->r2);
// 						break;
// 					case DIRECTIONONLY:
// 						gte_ldv3(&tfn[op->v0], &tfn[op->v1], &tfn[op->v2]);
// 						gte_nct();			
// 						gte_strgb3(&si->r0, &si->r1, &si->r2);
// 						break;
// 					case COLOURIZE:
// 						*(u_long *)  (&si->r0) = *(u_long *) (&modctrl->col);
// 						*(u_long *)  (&si->r1) = *(u_long *) (&modctrl->col);
// 						*(u_long *)  (&si->r2) = *(u_long *) (&modctrl->col);
// 						break;
// 					default:
// 						*(u_long *)  (&si->r0) = *(u_long *) (&op->r0);
// 						*(u_long *)  (&si->r1) = *(u_long *) (&op->r1);
// 						*(u_long *)  (&si->r2) = *(u_long *) (&op->r2);
// 				}
// 
// // FOG CODE: --------------------------------------------------------
// 
// 				// SL: if it's near enough to not need fogging, or it's already additive or subtractive...
// 				if ( (fogFade >= 255))
// 				{
// 					// SL: put in the additive poly...
// 					setPolyGT3(si);
// 					si->code = op->cd | modctrl->semitrans;
// 					ENDPRIM(si, depth, POLY_GT3);
// 				}
// 				else
// 				{
// 					// SL: modge the RGBs accordingly
// /*					si->r0 = ((fogFade*(short)si->r0)/255);
// 					si->g0 = ((fogFade*(short)si->g0)/255);
// 					si->b0 = ((fogFade*(short)si->b0)/255);
// 					si->r1 = ((fogFade*(short)si->r1)/255);
// 					si->g1 = ((fogFade*(short)si->g1)/255);
// 					si->b1 = ((fogFade*(short)si->b1)/255);
// 					si->r2 = ((fogFade*(short)si->r2)/255);
// 					si->g2 = ((fogFade*(short)si->g2)/255);
// 					si->b2 = ((fogFade*(short)si->b2)/255);
// 
// 					// SL: put in the additive poly...
// 					setPolyGT3(si);
// 					// SL: store the pre-modification tpage...
// 					oldtpage = si->tpage;
// 					// make it additive...
// 					si->code |= 2;
// 					si->tpage |= 32;
// 					// SL: put it in t' table
// 					ENDPRIM(si, depth, POLY_GT3);
// 
// 					// SL: hokay, setup the next poly...
// 					BEGINPRIM(siNext, POLY_GT3);
// 
// 					// SL: copy in all the values using long copies...
// 					*(u_long *)  (&siNext->x0) = *(u_long *) (&si->x0);
// 					*(u_long *)  (&siNext->x1) = *(u_long *) (&si->x1);
// 					*(u_long *)  (&siNext->x2) = *(u_long *) (&si->x2);
// 					*(u_long *)  (&siNext->u0) = *(u_long *) (&si->u0);		// Texture coords
// 					*(u_long *)  (&siNext->u1) = *(u_long *) (&si->u1);
// 					*(u_long *)  (&siNext->u2) = *(u_long *) (&si->u2);
// 					*(u_long *)  (&siNext->r0) = *(u_long *) (&si->r0);			// SL: RGBs
// 					*(u_long *)  (&siNext->r1) = *(u_long *) (&si->r1);			// SL: RGBs
// 					*(u_long *)  (&siNext->r2) = *(u_long *) (&si->r2);			// SL: RGBs
// 
// 					// SL: set it as the right type
// 					setPolyGT3(siNext);
// 					// SL: make it subtractive
// 					siNext->clut = EXPLORE_black_CLUT;
// 					siNext->code |= 2;
// 					siNext->tpage = oldtpage | 64;
// 
// 					// SL: put it in t' table
// 					ENDPRIM(siNext, depth, POLY_GT3);*/
// 				}
// 
// // END FOG CODE: --------------------------------------------------------
// 
// 				op = op->next;
// 				break;
// #undef si
// #undef siNext
// #undef op
// /*-----------------------------------------------------------------------------------------------------------------*/
// #define si ((POLY_GT4*)packet)
// #define siNext ((POLY_GT4*)packetNext)
// #define op opcd
// 
// 			case GPU_COM_TG4:
// 				BEGINPRIM(si, POLY_GT4);
// 
// 				gte_ldsxy3(tfv[op->v0], tfv[op->v1], tfv[op->v2]);		// Load 1st three vertices
// 				
// 				*(u_long *)  (&si->u0) = *(u_long *) (&op->tu0);		// Texture coords
// 				*(u_long *)  (&si->u1) = *(u_long *) (&op->tu1);
// 					
// 				gte_stsxy3_gt4(si);
// 				
// 						
// 				*(u_long *)  (&si->u2) = *(u_long *) (&op->tu2);
// 				*(u_long *)  (&si->u3) = *(u_long *) (&op->tu3);
// 
// 				*(u_long *)  (&si->x3) = *(u_long *) (&tfv[op->v3]);
// 		
// 				switch (lightmode)
// 				{
// 					case DIRECTIONAL:
// 						gte_ldrgb3(&op->r0, &op->r1, &op->r2);
// 						gte_ldv3(&tfn[op->v0], &tfn[op->v1], &tfn[op->v2]);
// 						gte_ncct();
// 						gte_strgb3(&si->r0, &si->r1, &si->r2);
// 
// 						gte_ldrgb(&op->r3);
// 						gte_ldv0(&tfn[op->v3]);
// 						gte_nccs();			// NormalColorCol
// 						gte_strgb(&si->r3);
// 						break;
// 					case DIRECTIONONLY:
// 						gte_ldv3(&tfn[op->v0], &tfn[op->v1], &tfn[op->v2]);
// 						gte_nct();			
// 						gte_strgb3(&si->r0, &si->r1, &si->r2);
// 
// 						gte_ldv0(&tfn[op->v3]);
// 						gte_ncs();			// NormalColorCol
// 						gte_strgb(&si->r3);
// 						break;
// 					case COLOURIZE:
// 						*(u_long *)  (&si->r0) = *(u_long *) (&modctrl->col);
// 						*(u_long *)  (&si->r1) = *(u_long *) (&modctrl->col);
// 						*(u_long *)  (&si->r2) = *(u_long *) (&modctrl->col);
// 						*(u_long *)  (&si->r3) = *(u_long *) (&modctrl->col);
// 						break;
// 					default:
// 						*(u_long *)  (&si->r0) = *(u_long *) (&op->r0);
// 						*(u_long *)  (&si->r1) = *(u_long *) (&op->r1);
// 						*(u_long *)  (&si->r2) = *(u_long *) (&op->r2);
// 						*(u_long *)  (&si->r3) = *(u_long *) (&op->r3);
// 				}
// 		
// // FOG CODE: --------------------------------------------------------
// 				
// 				// SL: if it's near enough to not need fogging, or it's already additive or subtractive...
// 				if ( (fogFade >= 255))
// 				{
// 					// SL: put in the additive poly...
// 					setPolyGT4(si);
// 					si->code = op->cd | modctrl->semitrans;
// 					ENDPRIM(si, depth, POLY_GT4);
// 				}
// 				else
// 				{
// 					// SL: modge the RGBs accordingly
// /*					si->r0 = ((fogFade*(short)si->r0)/255);
// 					si->g0 = ((fogFade*(short)si->g0)/255);
// 					si->b0 = ((fogFade*(short)si->b0)/255);
// 					si->r1 = ((fogFade*(short)si->r1)/255);
// 					si->g1 = ((fogFade*(short)si->g1)/255);
// 					si->b1 = ((fogFade*(short)si->b1)/255);
// 					si->r2 = ((fogFade*(short)si->r2)/255);
// 					si->g2 = ((fogFade*(short)si->g2)/255);
// 					si->b2 = ((fogFade*(short)si->b2)/255);
// 					si->r3 = ((fogFade*(short)si->r3)/255);
// 					si->g3 = ((fogFade*(short)si->g3)/255);
// 					si->b3 = ((fogFade*(short)si->b3)/255);
// 
// 					// SL: put in the additive poly...
// 					setPolyGT4(si);
// 					// SL: store the pre-modification tpage...
// 					oldtpage = si->tpage;
// 					// make it additive...
// 					si->code |= 2;
// 					si->tpage |= 32;
// 					// SL: put it in t' table
// 					ENDPRIM(si, depth, POLY_GT4);
// 
// 					// SL: hokay, setup the next poly...
// 					BEGINPRIM(siNext, POLY_GT4);
// 
// 					// SL: copy in all the values using long copies...
// 					*(u_long *)  (&siNext->x0) = *(u_long *) (&si->x0);
// 					*(u_long *)  (&siNext->x1) = *(u_long *) (&si->x1);
// 					*(u_long *)  (&siNext->x2) = *(u_long *) (&si->x2);
// 					*(u_long *)  (&siNext->x3) = *(u_long *) (&si->x3);
// 					*(u_long *)  (&siNext->u0) = *(u_long *) (&si->u0);		// Texture coords
// 					*(u_long *)  (&siNext->u1) = *(u_long *) (&si->u1);
// 					*(u_long *)  (&siNext->u2) = *(u_long *) (&si->u2);
// 					*(u_long *)  (&siNext->u3) = *(u_long *) (&si->u3);
// 					*(u_long *)  (&siNext->r0) = *(u_long *) (&si->r0);			// SL: RGBs
// 					*(u_long *)  (&siNext->r1) = *(u_long *) (&si->r1);			// SL: RGBs
// 					*(u_long *)  (&siNext->r2) = *(u_long *) (&si->r2);			// SL: RGBs
// 					*(u_long *)  (&siNext->r3) = *(u_long *) (&si->r3);			// SL: RGBs
// 
// 					// SL: set it as the right type
// 					setPolyGT4(siNext);
// 					// SL: make it subtractive
// 					siNext->clut = EXPLORE_black_CLUT;
// 					siNext->code |= 2;
// 					siNext->tpage = oldtpage | 64;
// 
// 					// SL: put it in t' table
// 					ENDPRIM(siNext, depth, POLY_GT4);*/
// 				}
// 
// // END FOG CODE: --------------------------------------------------------
// 
// 				(int)op = op->next;
// 				break;
// 
// #undef si
// #undef siNext
// #undef op
// /*-----------------------------------------------------------------------------------------------------------------*/
// 
// #define si ((POLY_FT4*)packet)
// #define op ((TMD_P_FT4I*)opcd)
// 		case GPU_COM_TF4SPR :
// 			if((tfd[op->v0] > modctrl->nearclip) && (tfd[op->v0] < modctrl->farclip))
// 			{
// 				int			width, height;
// 
// 				BEGINPRIM(si, POLY_FT4);
// 
// /*
// 	We can't use the "quick" scaling method in psi objects, since we don't know which
// 	bone the sprite is attached to, hence we can't rtps the vertex and get the scaled width/height,
// 	so we have to do the scaling based on the distance ourselves.
// */
// 
// 				width = ((op->v2 * gteH) / tfd[op->v0]) / 2;
// 				height = ((op->v3 * gteH) / tfd[op->v0]) / 4;
// 
//  				*(u_long *)&si->r0 = *(u_long *)&op->r0;			// Texture coords / colors
// 				*(u_long *)&si->u0 = *(u_long *)&op->tu0;
// 				*(u_long *)&si->u1 = *(u_long *)&op->tu1;
// 				*(u_long *)&si->u2 = *(u_long *)&op->tu2;
// 				*(u_long *)&si->u3 = *(u_long *)&op->tu3;
// 
// 				si->x1 = si->x3 = ((DVECTOR *)tfv)[op->v0].vx + width;
// 				si->x0 = si->x2 = ((DVECTOR *)tfv)[op->v0].vx - width;
// 		 	
// 				si->y2 = si->y3 = ((DVECTOR *)tfv)[op->v0].vy + height;
// 				si->y0 = si->y1 = ((DVECTOR *)tfv)[op->v0].vy - height;
// 				setPolyFT4(si);
// 
// 				si->code |= modctrl->semitrans;
// 		
//  				ENDPRIM(si, depth & 1023, POLY_FT4);
// 				op = op->next;
// 			}
// 			break;
// #undef si
// #undef op
// 		/*		TMD_P_FT4I *testpol;
// 				POLY_FT4 *testsi;
// 
// 				SHORT		spritez;
// 				int		width, height;
// 
// 				si = (POLY_FT4*)packet;
// 				op = (TMD_P_FT4I*)(opcd);
// 
// 				BEGINPRIM(si, POLY_FT4);
// 
// 				testpol = op;
// 				testsi = si;
// 
// 	// scaling-and-transform-in-one-go code from the Action Man people...
// 				width = op->v1;
// 				gte_SetLDDQB(0);			// clear offset control reg (C2_DQB)
// 				gte_ldv0(&vp[op->v0]);		// Load centre point
// 				gte_SetLDDQA(width);		// shove sprite width into control reg (C2_DQA)
// 				gte_rtps();					// do the rtps
// 				gte_stsxy(&si->x0);			// get screen x and y
// 				gte_stsz(&spritez);		// get order table z
// 	// end of scaling-and-transform
// 
// 
// 				if (spritez < 20) break;
// 
// 				gte_stopz(&width);		// get scaled width of sprite
// 				width >>= 17;
// 
//  				*(u_long *) & si->r0 = *(u_long *) & op->r0;			// Texture coords / colors
// 				*(u_long *) & si->u0 = *(u_long *) & op->tu0;
// 				*(u_long *) & si->u1 = *(u_long *) & op->tu1;
// 				*(u_long *) & si->u2 = *(u_long *) & op->tu2;
// 				*(u_long *) & si->u3 = *(u_long *) & op->tu3;
// 
// 				si->x1 = si->x3=si->x0+width;
// 				si->x0 = si->x2=si->x0-width;
// 			
//  		 		height = width>>1;//(LONG)(width*(3))/spritez;
// 			
// 				si->y2 = si->y3=si->y0+height;
// 				si->y0 = si->y1=si->y0-height;
// 				setPolyFT4(si);
// 
// 				si->code = op->cd | modctrl->semitrans;
// 		
//  				ENDPRIM(si, depth, POLY_FT4);
// 				op = op->next;*/
// /*-----------------------------------------------------------------------------------------------------------------*/
// /*#define si ((POLY_G4*)packet)
// #define siNext ((POLY_G4*)packetNext)
// #define op ((TMD_P_FG4I*)opcd)
// 			case GPU_COM_G4:
// 			case GPU_COM_F4:
// 				
// 				BEGINPRIM(si, POLY_G4);
//    			
// 				gte_ldsxy3(tfv[op->v0], tfv[op->v1], tfv[op->v2]);		// Load 1st three vertices
// 
// 				*(u_long *)  (&si->x3) = *(u_long *) (&tfv[op->v3]);
//  
// 				
// 				gte_stsxy3_g4(si);
// 
// 
// 				switch (lightmode)
// 				{
// 					case DIRECTIONAL:
// 						gte_ldrgb3(&op->r0, &op->r1, &op->r2);
// 						gte_ldv3(&tfn[op->v0], &tfn[op->v1], &tfn[op->v2]);
// 						gte_ncct();
// 						gte_strgb3(&si->r0, &si->r1, &si->r2);
// 						
// 						gte_ldrgb(&op->r3);
// 						gte_ldv0(&tfn[op->v3]);
// 						gte_nccs();			// NormalColorCol
// 						gte_strgb(&si->r3);
// 						break;
// 					case DIRECTIONONLY:
// 						gte_ldv3(&tfn[op->v0], &tfn[op->v1], &tfn[op->v2]);
// 						gte_nct();			
// 						gte_strgb3(&si->r0, &si->r1, &si->r2);
// 
// 						gte_ldv0(&tfn[op->v3]);
// 						gte_ncs();			// NormalColorCol
// 						gte_strgb(&si->r3);
// 						break;
// 
// 					case COLOURIZE:
// 						*(u_long *)  (&si->r0) = *(u_long *) (&modctrl->col);
// 						*(u_long *)  (&si->r1) = *(u_long *) (&modctrl->col);
// 						*(u_long *)  (&si->r2) = *(u_long *) (&modctrl->col);
// 						*(u_long *)  (&si->r3) = *(u_long *) (&modctrl->col);
// 						break;
// 					default:
// 						*(u_long *)  (&si->r0) = *(u_long *) (&op->r0);
// 						*(u_long *)  (&si->r1) = *(u_long *) (&op->r1);
// 						*(u_long *)  (&si->r2) = *(u_long *) (&op->r2);
// 						*(u_long *)  (&si->r3) = *(u_long *) (&op->r3);
// 				}
// 
// // FOG CODE: --------------------------------------------------------
// 				
// 				// SL: if it's near enough to not need fogging, or it's already additive or subtractive...
// 				if ( (fogFade >= 255))
// 				{
// 					// SL: put in the additive poly...
// 					setPolyG4(si);
// 					si->code = op->cd | modctrl->semitrans;
// 					ENDPRIM(si, depth, POLY_G4);
// 				}
// 				else
// 				{
// 					// SL: modge the RGBs accordingly
// 					si->r0 = ((fogFade*(short)si->r0)/255);
// 					si->g0 = ((fogFade*(short)si->g0)/255);
// 					si->b0 = ((fogFade*(short)si->b0)/255);
// 					si->r1 = ((fogFade*(short)si->r1)/255);
// 					si->g1 = ((fogFade*(short)si->g1)/255);
// 					si->b1 = ((fogFade*(short)si->b1)/255);
// 					si->r2 = ((fogFade*(short)si->r2)/255);
// 					si->g2 = ((fogFade*(short)si->g2)/255);
// 					si->b2 = ((fogFade*(short)si->b2)/255);
// 					si->r3 = ((fogFade*(short)si->r3)/255);
// 					si->g3 = ((fogFade*(short)si->g3)/255);
// 					si->b3 = ((fogFade*(short)si->b3)/255);
// 
// //					t1 = (op->clut<<16)|WATER_TRANS_CLUT;
// //					*(u_long *)  (&si->u0) = t1;
// 
// //					t1 = (*(u_long *) (&op->r0))|WATER_TRANS_CODE;
// 
// //					*(u_long *)  (&si->r0) = t1;
// 					// SL: put in the additive poly...
// 					setPolyG4(si);
// 					// make it additive...
// 					si->code |= 2;
// 					// SL: put it in t' table
// 					ENDPRIM(si, depth, POLY_G4);
// 					// SL: put in the "make it additive" packet...
// 					BEGINPRIM(dr_mode, DR_MODE);
// 					SetDrawMode(dr_mode, 0,1, 32,0);
// 					ENDPRIM(dr_mode, depth, DR_MODE);
// 
// 					// SL: hokay, setup the next poly...
// 					BEGINPRIM(siNext, POLY_G4);
// 
// 					// SL: copy in all the values using long copies...
// 					*(u_long *)  (&siNext->x0) = *(u_long *) (&si->x0);
// 					*(u_long *)  (&siNext->x1) = *(u_long *) (&si->x1);
// 					*(u_long *)  (&siNext->x2) = *(u_long *) (&si->x2);
// 					*(u_long *)  (&siNext->x3) = *(u_long *) (&si->x3);
// 					*(u_long *)  (&siNext->r0) = *(u_long *) (&si->r0);			// SL: RGBs
// 					*(u_long *)  (&siNext->r1) = *(u_long *) (&si->r1);			// SL: RGBs
// 					*(u_long *)  (&siNext->r2) = *(u_long *) (&si->r2);			// SL: RGBs
// 					*(u_long *)  (&siNext->r3) = *(u_long *) (&si->r3);			// SL: RGBs
// 
// 					// SL: set it as the right type
// 					setPolyG4(siNext);
// 					// SL: make it subtractive
// 					siNext->code |= 2;
// 
// 					// SL: put it in t' table
// 					ENDPRIM(siNext, depth, POLY_G4);
// 					// SL: put in the "make it subtractive" packet...
// 					BEGINPRIM(dr_mode, DR_MODE);
// 					SetDrawMode(dr_mode, 0,1, 64,0);
// 					ENDPRIM(dr_mode, depth, DR_MODE);
// 				}
// 
// 				op = op->next;
// 				break;
// #undef si
// #undef siNext
// #undef op
// /*-----------------------------------------------------------------------------------------------------------------*/
// /*#define si ((POLY_G3*)packet)
// #define siNext ((POLY_G3*)packetNext)
// #define op ((TMD_P_FG3I*)opcd)
// 			
// 			case GPU_COM_G3:
// 			case GPU_COM_F3:
// 			
// 				BEGINPRIM(si, POLY_G3);
//    			
// 				gte_ldsxy3(tfv[op->v0], tfv[op->v1], tfv[op->v2]);		// Load 1st three vertices
// 
// 				*(u_long *)  (&si->r0) = *(u_long *) (&op->r0);
// 				
// 				gte_stsxy3_g3(si);
// 
// 
// 				switch (lightmode)
// 				{
// 					case DIRECTIONAL:
// 						gte_ldrgb3(&op->r0, &op->r1, &op->r2);
// 						gte_ldv3(&tfn[op->v0], &tfn[op->v1], &tfn[op->v2]);
// 						gte_ncct();
// 						gte_strgb3(&si->r0, &si->r1, &si->r2);
// 						break;
// 					case DIRECTIONONLY:
// 						gte_ldv3(&tfn[op->v0], &tfn[op->v1], &tfn[op->v2]);
// 						gte_nct();			
// 						gte_strgb3(&si->r0, &si->r1, &si->r2);
// 						break;
// 					case COLOURIZE:
// 						*(u_long *)  (&si->r0) = *(u_long *) (&modctrl->col);
// 						*(u_long *)  (&si->r1) = *(u_long *) (&modctrl->col);
// 						*(u_long *)  (&si->r2) = *(u_long *) (&modctrl->col);
// 						break;
// 					default:
// 						*(u_long *)  (&si->r1) = *(u_long *) (&op->r1);
// 						*(u_long *)  (&si->r2) = *(u_long *) (&op->r2);
// 				}
// 
// // FOG CODE: --------------------------------------------------------
// 
// 				// SL: if it's near enough to not need fogging, or it's already additive or subtractive...
// 				if ( (fogFade >= 255))
// 				{
// 					// SL: put in the additive poly...
// 					setPolyG3(si);
// 					si->code = op->cd | modctrl->semitrans;
// 					ENDPRIM(si, depth, POLY_G3);
// 				}
// 				else
// 				{
// 					// SL: modge the RGBs accordingly
// 					si->r0 = ((fogFade*(short)si->r0)/255);
// 					si->g0 = ((fogFade*(short)si->g0)/255);
// 					si->b0 = ((fogFade*(short)si->b0)/255);
// 					si->r1 = ((fogFade*(short)si->r1)/255);
// 					si->g1 = ((fogFade*(short)si->g1)/255);
// 					si->b1 = ((fogFade*(short)si->b1)/255);
// 					si->r2 = ((fogFade*(short)si->r2)/255);
// 					si->g2 = ((fogFade*(short)si->g2)/255);
// 					si->b2 = ((fogFade*(short)si->b2)/255);
// 
// //					t1 = (op->clut<<16)|WATER_TRANS_CLUT;
// //					*(u_long *)  (&si->u0) = t1;
// 
// //					t1 = (*(u_long *) (&op->r0))|WATER_TRANS_CODE;
// 
// //					*(u_long *)  (&si->r0) = t1;
// 					// SL: put in the additive poly...
// 					setPolyG3(si);
// 					// make it additive...
// 					si->code |= 2;
// 					// SL: put it in t' table
// 					ENDPRIM(si, depth, POLY_G3);
// 					// SL: put in the "make it additive" packet...
// 					BEGINPRIM(dr_mode, DR_MODE);
// 					SetDrawMode(dr_mode, 0,1, 32,0);
// 					ENDPRIM(dr_mode, depth, DR_MODE);
// 
// 					// SL: hokay, setup the next poly...
// 					BEGINPRIM(siNext, POLY_G3);
// 
// 					// SL: copy in all the values using long copies...
// 					*(u_long *)  (&siNext->x0) = *(u_long *) (&si->x0);
// 					*(u_long *)  (&siNext->x1) = *(u_long *) (&si->x1);
// 					*(u_long *)  (&siNext->x2) = *(u_long *) (&si->x2);
// 					*(u_long *)  (&siNext->r0) = *(u_long *) (&si->r0);			// SL: RGBs
// 					*(u_long *)  (&siNext->r1) = *(u_long *) (&si->r1);			// SL: RGBs
// 					*(u_long *)  (&siNext->r2) = *(u_long *) (&si->r2);			// SL: RGBs
// 
// 					// SL: set it as the right type
// 					setPolyG3(siNext);
// 					// SL: make it subtractive
// 					siNext->code |= 2;
// 
// 					// SL: put it in t' table
// 					ENDPRIM(siNext, depth, POLY_G3);
// 					// SL: put in the "make it subtractive" packet...
// 					BEGINPRIM(dr_mode, DR_MODE);
// 					SetDrawMode(dr_mode, 0,1, 64,0);
// 					ENDPRIM(dr_mode, depth, DR_MODE);
// 				}
// 
// // END FOG CODE: --------------------------------------------------------
// 
// 				op = op->next;
// 				break;
// #undef si
// #undef siNext
// #undef op
// /*-----------------------------------------------------------------------------------------------------------------*/
// 		default:
// 			break;
// 		}
// 	}
// //	TimerStop(&poly);
// //	utilPrintf("Timer Taken To Draw Polys : %d\n", poly.total);
// }


void LSCAPE_DrawSortedPrimitives(int depth)
{
/*	register PACKET			*packet, *packetNext;
	register long			*tfv = transformedVertices;
	register long			*tfd = transformedDepths;
	VERT 					*tfn = transformedNormals;
	register TMD_P_GT4I		*opcd;
	PSIMODELCTRL			*modctrl = &PSImodelctrl;
	int						primsleft,lightmode;
	ULONG					*sorts = sortedIndex;
//	ULONG					*sorts = modctrl->SortOffs;
	ULONG					sortBucket = 0;
	VERT					*vp = modctrl->VertTop;
	LONG					fogdepth;
	unsigned short			oldtpage;
	long					fogFade;
	DR_MODE					*dr_mode;
	u_long t1;

	int						gteH;


	gte_ReadGeomScreen(&gteH);

	depth=depth>>2;

	// SL: get the fogdepth, and modge it accordingly.
	fogdepth = 10000;//(depth << (2+1));

//	if(fogdepth > LSCAPE_Data.fogendz)
//		return;

	// SL: work out the fog fade. in advance. It's the same for the whole model, so this is fine here.
	fogFade=globalFadeVal;//255-((255*(fogdepth-3000))/(3500-3000));
	// SL: shift everythang into the same rez as the landscape, etc...
//	depth = (depth >> LSCAPE_Data.depthshift) & MAXDEPTH;

	
	primsleft = sortCount;
	if (!primsleft)
		return;

//	polyCount += primsleft;	

	lightmode = modctrl->lighting;
	opcd = 0;
//	TimerStart(&poly);
	//utilPrintf("Polys To Draw : %d\n", primsleft);


	while(primsleft)
	{
		if (opcd==0)
		{
			opcd = (TMD_P_GT4I*)sorts[sortBucket++];
			continue;
		}

		primsleft--;

		switch (opcd->cd & (0xff-2))
		{
#define si ((POLY_FT3*)packet)
#define siNext ((POLY_FT3*)packetNext)
#define op ((TMD_P_FT3I*)opcd)

			case GPU_COM_TF3:
//				BEGINPRIM(si, POLY_FT3);

				gte_ldsxy3(tfv[op->v0], tfv[op->v1], tfv[op->v2]);		// Load 1st three vertices
			
				*(u_long *) (&si->u0) = *(u_long *) (&op->tu0);		// Texture coords
				*(u_long *) (&si->u1) = *(u_long *) (&op->tu1);
				*(u_long *) (&si->u2) = *(u_long *) (&op->tu2);

				gte_stsxy3_ft3(si);
			
				*(u_long *) (&si->r0) = *(u_long *) (&op->r0);

				// SL: put in the additive poly...
				setPolyFT3(si);
				si->code = op->cd | modctrl->semitrans;
//				ENDPRIM(si, depth, POLY_FT3);

				// SL: move the prim pointer on one...
				op = (TMD_P_FT3I*)op->next;
				break;
#undef si
#undef siNext
#undef op
#define si ((POLY_FT4*)packet)
#define siNext ((POLY_FT4*)packetNext)
#define op ((TMD_P_FT4I*)opcd)
				
			case GPU_COM_TF4:

//				BEGINPRIM(si, POLY_FT4);
   			
				gte_ldsxy3(tfv[op->v0], tfv[op->v1], tfv[op->v2]);		// Load 1st three vertices
			
				*(u_long *)  (&si->u0) = *(u_long *) (&op->tu0);		// Texture coords
				*(u_long *)  (&si->u1) = *(u_long *) (&op->tu1);

				gte_stsxy3_ft4(si);

				*(u_long *) (&si->x3) = *(u_long *) (&tfv[op->v3]);

				*(u_long *) (&si->r0) = *(u_long *) (&op->r0);		// 9 cycles here

				*(u_long *) (&si->u2) = *(u_long *) (&op->tu2);
				*(u_long *) (&si->u3) = *(u_long *) (&op->tu3);

				// SL: put in the additive poly...
				setPolyFT4(si);
				si->code = op->cd | modctrl->semitrans;
//				ENDPRIM(si, depth, POLY_FT4);

				op = (TMD_P_FT4I*)op->next;
				break;
#undef si
#undef siNext
#undef op
#define si ((POLY_GT3*)packet)
#define siNext ((POLY_GT3*)packetNext)
#define op ((TMD_P_GT3I*)opcd)

			case GPU_COM_TG3:

//				BEGINPRIM(si, POLY_GT3);

				gte_ldsxy3(tfv[op->v0], tfv[op->v1], tfv[op->v2]);		// Load 1st three vertices
				
				*(u_long *)  (&si->u0) = *(u_long *) (&op->tu0);		// Texture coords
				*(u_long *)  (&si->u1) = *(u_long *) (&op->tu1);

				gte_stsxy3_gt3(si);

				*(u_long *)  (&si->u2) = *(u_long *) (&op->tu2);

				*(u_long *)  (&si->r0) = *(u_long *) (&op->r0);
				*(u_long *)  (&si->r1) = *(u_long *) (&op->r1);
				*(u_long *)  (&si->r2) = *(u_long *) (&op->r2);

				// SL: put in the additive poly...
				setPolyGT3(si);
				si->code = op->cd | modctrl->semitrans;
//				ENDPRIM(si, depth, POLY_GT3);

				op = (TMD_P_GT3I*)op->next;
				break;
#undef si
#undef siNext
#undef op
#define si ((POLY_GT4*)packet)
#define siNext ((POLY_GT4*)packetNext)
#define op opcd

			case GPU_COM_TG4:
//				BEGINPRIM(si, POLY_GT4);

				gte_ldsxy3(tfv[op->v0], tfv[op->v1], tfv[op->v2]);		// Load 1st three vertices
				
				*(u_long *)  (&si->u0) = *(u_long *) (&op->tu0);		// Texture coords
				*(u_long *)  (&si->u1) = *(u_long *) (&op->tu1);
					
				gte_stsxy3_gt4(si);
				
						
				*(u_long *)  (&si->u2) = *(u_long *) (&op->tu2);
				*(u_long *)  (&si->u3) = *(u_long *) (&op->tu3);

				*(u_long *)  (&si->x3) = *(u_long *) (&tfv[op->v3]);
		
				*(u_long *)  (&si->r0) = *(u_long *) (&op->r0);
				*(u_long *)  (&si->r1) = *(u_long *) (&op->r1);
				*(u_long *)  (&si->r2) = *(u_long *) (&op->r2);
				*(u_long *)  (&si->r3) = *(u_long *) (&op->r3);
		
				// SL: put in the additive poly...
				setPolyGT4(si);
				si->code = op->cd | modctrl->semitrans;
//				ENDPRIM(si, depth, POLY_GT4);

				op = (TMD_P_GT4I*)op->next;
				break;

#undef si
#undef siNext
#undef op

#define si ((POLY_FT4*)packet)
#define op ((TMD_P_FT4I*)opcd)

			case GPU_COM_TF4SPR :

				if((tfd[op->v0] > modctrl->nearclip) && (tfd[op->v0] < modctrl->farclip))
				{
					int			width, height;

//					BEGINPRIM(si, POLY_FT4);

	
	//	We can't use the "quick" scaling method in psi objects, since we don't know which
	//	bone the sprite is attached to, hence we can't rtps the vertex and get the scaled width/height,
	//	so we have to do the scaling based on the distance ourselves.
	

					width = ((op->v2 * gteH) / tfd[op->v0]) / 2;
					height = ((op->v3 * gteH) / tfd[op->v0]) / 4;

	 				*(u_long *)&si->r0 = *(u_long *)&op->r0;			// Texture coords / colors
					*(u_long *)&si->u0 = *(u_long *)&op->tu0;
					*(u_long *)&si->u1 = *(u_long *)&op->tu1;
					*(u_long *)&si->u2 = *(u_long *)&op->tu2;
					*(u_long *)&si->u3 = *(u_long *)&op->tu3;

					si->x1 = si->x3 = ((DVECTOR *)tfv)[op->v0].vx + width;
					si->x0 = si->x2 = ((DVECTOR *)tfv)[op->v0].vx - width;
			 	
					si->y2 = si->y3 = ((DVECTOR *)tfv)[op->v0].vy + height;
					si->y0 = si->y1 = ((DVECTOR *)tfv)[op->v0].vy - height;
					setPolyFT4(si);

					si->code |= modctrl->semitrans;
			
//	 				ENDPRIM(si, depth & 1023, POLY_FT4);
					op = (TMD_P_FT4I*)op->next;
				}
				break;
#undef si
#undef op
			default:
				break;
		}
	}
//	TimerStop(&poly);
//	utilPrintf("Timer Taken To Draw Polys : %d\n", poly.total);
*/
}



void DrawSortedPrimitivesFaded ( int depth )
{
/*	register PACKET			*packet, *packetNext;
	register long			*tfv = transformedVertices;
	register long			*tfd = transformedDepths;
//	VERT 							*tfn = transformedNormals;
	register TMD_P_GT4I		*opcd;
	PSIMODELCTRL			*modctrl = &PSImodelctrl;
	int						primsleft;//,lightmode;
	ULONG					*sorts = sortedIndex;
//	ULONG					*sorts = modctrl->SortOffs;
	ULONG					sortBucket = 0;
//	VERT					*vp = modctrl->VertTop;
//	LONG					fogdepth;
	unsigned short			oldtpage;
//	long					fogFade;
//	DR_MODE					*dr_mode;
//	u_long t1;

	int						gteH;


	gte_ReadGeomScreen(&gteH);

	depth=depth>>2;

	// SL: work out the fog fade. in advance. It's the same for the whole model, so this is fine here.
//	fogFade=globalFadeVal;

	// SL: shift everythang into the same rez as the landscape, etc...
	//	depth = (depth >> LSCAPE_Data.depthshift) & MAXDEPTH;

	
	primsleft = sortCount;
	if (!primsleft)
		return;

//	polyCount += primsleft;	

	opcd = 0;

	while(primsleft)
	{
		if (opcd==0)
		{
			opcd = (TMD_P_GT4I*)sorts[sortBucket++];
			continue;
		}

		primsleft--;

		switch (opcd->cd & (0xff-2))
		{
#define si ((POLY_FT3*)packet)
#define siNext ((POLY_FT3*)packetNext)
#define op ((TMD_P_FT3I*)opcd)

			case GPU_COM_TF3:
//				BEGINPRIM(si, POLY_FT3);

				gte_ldsxy3(tfv[op->v0], tfv[op->v1], tfv[op->v2]);		// Load 1st three vertices
			
				*(u_long *) (&si->u0) = *(u_long *) (&op->tu0);		// Texture coords
				*(u_long *) (&si->u1) = *(u_long *) (&op->tu1);
				*(u_long *) (&si->u2) = *(u_long *) (&op->tu2);

				gte_stsxy3_ft3(si);
			
				*(u_long *) (&si->r0) = *(u_long *) (&op->r0);

 				// SL: modge the RGBs accordingly
 				si->r0 = ((globalFadeVal*(short)si->r0)/255);
 				si->g0 = ((globalFadeVal*(short)si->g0)/255);
 				si->b0 = ((globalFadeVal*(short)si->b0)/255);

 				// SL: put in the additive poly...
 				setPolyFT3(si);
 				// SL: store the pre-modification tpage...
 				oldtpage = si->tpage;
 				// make it additive...
 				si->code |= 2;
 				si->tpage |= 32;
 				// SL: put it in t' table
 //				ENDPRIM(si, depth, POLY_FT3);

 				// SL: hokay, setup the next poly...

 	//			BEGINPRIM(siNext, POLY_FT3);

 				// SL: copy in all the values using long copies...
 				*(u_long *)  (&siNext->x0) = *(u_long *) (&si->x0);
 				*(u_long *)  (&siNext->x1) = *(u_long *) (&si->x1);
 				*(u_long *)  (&siNext->x2) = *(u_long *) (&si->x2);
 				*(u_long *)  (&siNext->u0) = *(u_long *) (&si->u0);		// Texture coords
 				*(u_long *)  (&siNext->u1) = *(u_long *) (&si->u1);
 				*(u_long *)  (&siNext->u2) = *(u_long *) (&si->u2);
 				*(u_long *)  (&siNext->r0) = *(u_long *) (&si->r0);			// SL: RGBs

 				// SL: set it as the right type
 				setPolyFT3(siNext);
 				// SL: make it subtractive
 				siNext->clut = EXPLORE_black_CLUT;
 				siNext->code |= 2;
 				siNext->tpage = oldtpage | 64;

 				// SL: put it in t' table
 //				ENDPRIM(siNext, depth, POLY_FT3);

				op = (TMD_P_FT3I*)op->next;
				break;
#undef si
#undef siNext
#undef op
#define si ((POLY_FT4*)packet)
#define siNext ((POLY_FT4*)packetNext)
#define op ((TMD_P_FT4I*)opcd)
				
			case GPU_COM_TF4:

//				BEGINPRIM(si, POLY_FT4);
   			
				gte_ldsxy3(tfv[op->v0], tfv[op->v1], tfv[op->v2]);		// Load 1st three vertices
			
				*(u_long *)  (&si->u0) = *(u_long *) (&op->tu0);		// Texture coords
				*(u_long *)  (&si->u1) = *(u_long *) (&op->tu1);

				gte_stsxy3_ft4(si);

				*(u_long *) (&si->x3) = *(u_long *) (&tfv[op->v3]);

				*(u_long *) (&si->r0) = *(u_long *) (&op->r0);		// 9 cycles here

				*(u_long *) (&si->u2) = *(u_long *) (&op->tu2);
				*(u_long *) (&si->u3) = *(u_long *) (&op->tu3);

 				// SL: modge the RGBs accordingly
 				si->r0 = ((globalFadeVal*(short)si->r0)/255);
 				si->g0 = ((globalFadeVal*(short)si->g0)/255);
 				si->b0 = ((globalFadeVal*(short)si->b0)/255);

 				// SL: put in the additive poly...
 				setPolyFT4(si);
 				// SL: store the pre-modification tpage...
 				oldtpage = si->tpage;
 				// make it additive...
 				si->code |= 2;
 				si->tpage |= 32;
 				// SL: put it in t' table
// 				ENDPRIM(si, depth, POLY_FT4);

 				// SL: hokay, setup the next poly...
 //				BEGINPRIM(siNext, POLY_FT4);

 				// SL: copy in all the values using long copies...
 				*(u_long *)  (&siNext->x0) = *(u_long *) (&si->x0);
 				*(u_long *)  (&siNext->x1) = *(u_long *) (&si->x1);
 				*(u_long *)  (&siNext->x2) = *(u_long *) (&si->x2);
 				*(u_long *)  (&siNext->x3) = *(u_long *) (&si->x3);
 				*(u_long *)  (&siNext->u0) = *(u_long *) (&si->u0);		// Texture coords
 				*(u_long *)  (&siNext->u1) = *(u_long *) (&si->u1);
 				*(u_long *)  (&siNext->u2) = *(u_long *) (&si->u2);
 				*(u_long *)  (&siNext->u3) = *(u_long *) (&si->u3);
 				*(u_long *)  (&siNext->r0) = *(u_long *) (&si->r0);			// SL: RGBs

 				// SL: set it as the right type
 				setPolyFT4(siNext);
 				// SL: make it subtractive
 				siNext->clut = EXPLORE_black_CLUT;
 				siNext->code |= 2;
 				siNext->tpage = oldtpage | 64;

 				// SL: put it in t' table
 //				ENDPRIM(siNext, depth, POLY_FT4);

    			op = (TMD_P_FT4I*)op->next;
				break;
#undef si
#undef siNext
#undef op
#define si ((POLY_GT3*)packet)
#define siNext ((POLY_GT3*)packetNext)
#define op ((TMD_P_GT3I*)opcd)

			case GPU_COM_TG3:

//				BEGINPRIM(si, POLY_GT3);

				gte_ldsxy3(tfv[op->v0], tfv[op->v1], tfv[op->v2]);		// Load 1st three vertices
				
				*(u_long *)  (&si->u0) = *(u_long *) (&op->tu0);		// Texture coords
				*(u_long *)  (&si->u1) = *(u_long *) (&op->tu1);

				gte_stsxy3_gt3(si);

				*(u_long *)  (&si->u2) = *(u_long *) (&op->tu2);

				*(u_long *)  (&si->r0) = *(u_long *) (&op->r0);
				*(u_long *)  (&si->r1) = *(u_long *) (&op->r1);
				*(u_long *)  (&si->r2) = *(u_long *) (&op->r2);

 				// SL: modge the RGBs accordingly
 				si->r0 = ((globalFadeVal*(short)si->r0)/255);
 				si->g0 = ((globalFadeVal*(short)si->g0)/255);
 				si->b0 = ((globalFadeVal*(short)si->b0)/255);
 				si->r1 = ((globalFadeVal*(short)si->r1)/255);
 				si->g1 = ((globalFadeVal*(short)si->g1)/255);
 				si->b1 = ((globalFadeVal*(short)si->b1)/255);
 				si->r2 = ((globalFadeVal*(short)si->r2)/255);
 				si->g2 = ((globalFadeVal*(short)si->g2)/255);
 				si->b2 = ((globalFadeVal*(short)si->b2)/255);

 				// SL: put in the additive poly...
 				setPolyGT3(si);
 				// SL: store the pre-modification tpage...
 				oldtpage = si->tpage;
 				// make it additive...
 				si->code |= 2;
 				si->tpage |= 32;
 				// SL: put it in t' table
 //				ENDPRIM(si, depth, POLY_GT3);

 				// SL: hokay, setup the next poly...
// 				BEGINPRIM(siNext, POLY_GT3);

 				// SL: copy in all the values using long copies...
 				*(u_long *)  (&siNext->x0) = *(u_long *) (&si->x0);
 				*(u_long *)  (&siNext->x1) = *(u_long *) (&si->x1);
 				*(u_long *)  (&siNext->x2) = *(u_long *) (&si->x2);
 				*(u_long *)  (&siNext->u0) = *(u_long *) (&si->u0);		// Texture coords
 				*(u_long *)  (&siNext->u1) = *(u_long *) (&si->u1);
 				*(u_long *)  (&siNext->u2) = *(u_long *) (&si->u2);
 				*(u_long *)  (&siNext->r0) = *(u_long *) (&si->r0);			// SL: RGBs
 				*(u_long *)  (&siNext->r1) = *(u_long *) (&si->r1);			// SL: RGBs
 				*(u_long *)  (&siNext->r2) = *(u_long *) (&si->r2);			// SL: RGBs

 				// SL: set it as the right type
 				setPolyGT3(siNext);
 				// SL: make it subtractive
 				siNext->clut = EXPLORE_black_CLUT;
 				siNext->code |= 2;
 				siNext->tpage = oldtpage | 64;

 				// SL: put it in t' table
// 				ENDPRIM(siNext, depth, POLY_GT3);
				op = (TMD_P_GT3I*)op->next;
				break;
#undef si
#undef siNext
#undef op
#define si ((POLY_GT4*)packet)
#define siNext ((POLY_GT4*)packetNext)
#define op opcd

			case GPU_COM_TG4:
//				BEGINPRIM(si, POLY_GT4);

				gte_ldsxy3(tfv[op->v0], tfv[op->v1], tfv[op->v2]);		// Load 1st three vertices
				
				*(u_long *)  (&si->u0) = *(u_long *) (&op->tu0);		// Texture coords
				*(u_long *)  (&si->u1) = *(u_long *) (&op->tu1);
					
				gte_stsxy3_gt4(si);
				
						
				*(u_long *)  (&si->u2) = *(u_long *) (&op->tu2);
				*(u_long *)  (&si->u3) = *(u_long *) (&op->tu3);

				*(u_long *)  (&si->x3) = *(u_long *) (&tfv[op->v3]);
		
				*(u_long *)  (&si->r0) = *(u_long *) (&op->r0);
				*(u_long *)  (&si->r1) = *(u_long *) (&op->r1);
				*(u_long *)  (&si->r2) = *(u_long *) (&op->r2);
				*(u_long *)  (&si->r3) = *(u_long *) (&op->r3);
		
 				// SL: modge the RGBs accordingly
 				si->r0 = ((globalFadeVal*(short)si->r0)/255);
 				si->g0 = ((globalFadeVal*(short)si->g0)/255);
 				si->b0 = ((globalFadeVal*(short)si->b0)/255);
 				si->r1 = ((globalFadeVal*(short)si->r1)/255);
 				si->g1 = ((globalFadeVal*(short)si->g1)/255);
 				si->b1 = ((globalFadeVal*(short)si->b1)/255);
 				si->r2 = ((globalFadeVal*(short)si->r2)/255);
 				si->g2 = ((globalFadeVal*(short)si->g2)/255);
 				si->b2 = ((globalFadeVal*(short)si->b2)/255);
 				si->r3 = ((globalFadeVal*(short)si->r3)/255);
 				si->g3 = ((globalFadeVal*(short)si->g3)/255);
 				si->b3 = ((globalFadeVal*(short)si->b3)/255);

 				// SL: put in the additive poly...
 				setPolyGT4(si);
 				// SL: store the pre-modification tpage...
 				oldtpage = si->tpage;
 				// make it additive...
 				si->code |= 2;
 				si->tpage |= 32;
 				// SL: put it in t' table
// 				ENDPRIM(si, depth, POLY_GT4);

 				// SL: hokay, setup the next poly...
// 				BEGINPRIM(siNext, POLY_GT4);

 				// SL: copy in all the values using long copies...
 				*(u_long *)  (&siNext->x0) = *(u_long *) (&si->x0);
 				*(u_long *)  (&siNext->x1) = *(u_long *) (&si->x1);
 				*(u_long *)  (&siNext->x2) = *(u_long *) (&si->x2);
 				*(u_long *)  (&siNext->x3) = *(u_long *) (&si->x3);
 				*(u_long *)  (&siNext->u0) = *(u_long *) (&si->u0);		// Texture coords
 				*(u_long *)  (&siNext->u1) = *(u_long *) (&si->u1);
 				*(u_long *)  (&siNext->u2) = *(u_long *) (&si->u2);
 				*(u_long *)  (&siNext->u3) = *(u_long *) (&si->u3);
 				*(u_long *)  (&siNext->r0) = *(u_long *) (&si->r0);			// SL: RGBs
 				*(u_long *)  (&siNext->r1) = *(u_long *) (&si->r1);			// SL: RGBs
 				*(u_long *)  (&siNext->r2) = *(u_long *) (&si->r2);			// SL: RGBs
 				*(u_long *)  (&siNext->r3) = *(u_long *) (&si->r3);			// SL: RGBs

 				// SL: set it as the right type
 				setPolyGT4(siNext);
 				// SL: make it subtractive
 				siNext->clut = EXPLORE_black_CLUT;
 				siNext->code |= 2;
 				siNext->tpage = oldtpage | 64;

 				// SL: put it in t' table
// 				ENDPRIM(siNext, depth, POLY_GT4);

				op = (TMD_P_GT4I*)op->next;
				break;

#undef si
#undef siNext
#undef op

#define si ((POLY_FT4*)packet)
#define op ((TMD_P_FT4I*)opcd)

			case GPU_COM_TF4SPR :

				if((tfd[op->v0] > modctrl->nearclip) && (tfd[op->v0] < modctrl->farclip))
				{
					int			width, height;

//					BEGINPRIM(si, POLY_FT4);

	
	//	We can't use the "quick" scaling method in psi objects, since we don't know which
	//	bone the sprite is attached to, hence we can't rtps the vertex and get the scaled width/height,
	//	so we have to do the scaling based on the distance ourselves.
	

					width = ((op->v2 * gteH) / tfd[op->v0]) / 2;
					height = ((op->v3 * gteH) / tfd[op->v0]) / 4;

	 				*(u_long *)&si->r0 = *(u_long *)&op->r0;			// Texture coords / colors
					*(u_long *)&si->u0 = *(u_long *)&op->tu0;
					*(u_long *)&si->u1 = *(u_long *)&op->tu1;
					*(u_long *)&si->u2 = *(u_long *)&op->tu2;
					*(u_long *)&si->u3 = *(u_long *)&op->tu3;

					si->x1 = si->x3 = ((DVECTOR *)tfv)[op->v0].vx + width;
					si->x0 = si->x2 = ((DVECTOR *)tfv)[op->v0].vx - width;
			 	
					si->y2 = si->y3 = ((DVECTOR *)tfv)[op->v0].vy + height;
					si->y0 = si->y1 = ((DVECTOR *)tfv)[op->v0].vy - height;
					setPolyFT4(si);

					si->code |= modctrl->semitrans;
			
//	 				ENDPRIM(si, depth & 1023, POLY_FT4);
					op = (TMD_P_FT4I*)op->next;
				}
				break;
#undef si
#undef op
			default:
				break;
		}
	}
//	TimerStop(&poly);
//	utilPrintf("Timer Taken To Draw Polys : %d\n", poly.total);
*/
}
