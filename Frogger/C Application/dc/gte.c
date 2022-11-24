/*
	This file is part of Frogger II, Copyright 2000 Interactive Studios Ltd

	File        : gte.c
	Description : Gte playstation emulaution routines and structures
	Date        : 09/03/2000
	Version     : 0.01
*/

#include "include.h"
#include "psitypes.h"
#include "sprite.h"


// 1/4096.0f
#define	INV4096	(0.000244140625f)


// -------
// Globals

MATRIX	GsIDMATRIX = {4096, 0, 0, 0, 4096, 0, 0, 0, 4096, 0, 0, 0};
MATRIX	GsWSMATRIX;

// Vertex registers
SVECTOR vr0;
SVECTOR vr1;
SVECTOR vr2;

// Poly Colour registers
CVECTOR cr;
CVECTOR cr0;
CVECTOR cr1;
CVECTOR cr2;

short cbk[3];

// Light Colour registers
u_char lcr0[3];
u_char lcr1[3];
u_char lcr2[3];

// Screen X-Y coordinates
SVECTOR screenxy[4];

long opz;

// Screen Z coordinates
long otz;

// Depth cue;
long dc;

// Register DQA
int dqa;

// Register DQB
int dqb;

// Variables for rtps and rtpt
int GSh = 950;
int GSx = 0;
int GSy = 0;

// *ASL* 26/07/2000 - Screen geometry floats
float fGSx = 0;
float fGSy = 0;
float fGSh = 950.0f;
float fGShHalf = 950.0f * 0.5f;
float fGShLimit = 950.0f / (950.0f * 0.5f);

// *ASL* 11/08/2000 - Transformation floats
float fTransVector[3];
float fRotMatrix[3][3];

// Rotation matrix
MATRIX RotMatrix;

// Trans matrix
VECTOR TransVector;

// Light Source Vectors
short ls0[3];
short ls1[3];
short ls2[3];

int LMode = 0;


// Far color
VECTOR farc;

// 16 Bit general/universal vector
SVECTOR sv;

// 32 Bit general/universal vector
VECTOR lv;

//ARM: cos & sin look up tables used in RotMatrixYXZ_gte(SVECTOR *r,MATRIX *m)
int		acos_table_init = 0;
short	acos_table[4096];
int		asin_table_init = 0;
short	asin_table[4096];

//ARM: cos & sin look up tables used in rsin(int num) and rcos(int num)
int		rcos_table_init = 0;
int		rcos_table[4096];
int		rsin_table_init = 0;
int		rsin_table[4096];

/**************************************************************************
	FUNCTION:	MATHSPC_ACos
	PURPOSE:	returns cos value from table, creates table on 1st call
	PARAMETERS:	
	RETURNS:	
**************************************************************************/
short MATHSPC_ACos(short angleIn)
{
	int		i;
	int		angle=angleIn;

	if(angle<0)
	{
		angle= 4096-((-angle)&4095);
	}
	else
	{
		angle&=4095;
	}

	if(!acos_table_init)
	{
		for(i=0; i<4096; i++)
		{
			acos_table[i] = (short)(cos((float)i*0.0015347f) * 4096);
		}
		acos_table_init = 1;
	}

	return(acos_table[angle]);

}

/**************************************************************************
	FUNCTION:	MATHSPC_ASin
	PURPOSE:	returns sin value from table, creates table on 1st call
	PARAMETERS:	
	RETURNS:	
**************************************************************************/
short MATHSPC_ASin(short angleIn)
{
	int		i;
	int		angle=angleIn;

	if(angle<0)
	{
		angle= 4096-((-angle)&4095);
	}
	else
	{
		angle&=4095;
	}

	if(!asin_table_init)
	{
		for(i=0; i<4096; i++)
		{
			asin_table[i] = (short)(sin((float)i*0.0015347f) * 4096);
		}
		asin_table_init = 1;
	}

	return(asin_table[angle]);

}
// ************************************************

// Load values to vertex register 0
void gte_ldv0(VOID *v)
{
	vr0.vx = ((SVECTOR *)v)->vx;
	vr0.vy = ((SVECTOR *)v)->vy;
	vr0.vz = ((SVECTOR *)v)->vz;
}

// Load LS 16 bits of vector to vertex register 0
void gte_ldlv0(VOID *v)
{
	vr0.vx = ((VECTOR *)v)->vx & 0xffff;
	vr0.vy = ((VECTOR *)v)->vy & 0xffff;
	vr0.vz = ((VECTOR *)v)->vz & 0xffff;
}

// Load values to vertex register 1
void gte_ldv1(VOID *v)
{
	vr1.vx = ((SVECTOR *)v)->vx;
	vr1.vy = ((SVECTOR *)v)->vy;
	vr1.vz = ((SVECTOR *)v)->vz;
}

// Load values to vertex register 2
void gte_ldv2(VOID *v)
{
	vr2.vx = ((SVECTOR *)v)->vx;
	vr2.vy = ((SVECTOR *)v)->vy;
	vr2.vz = ((SVECTOR *)v)->vz;
}

// Load values to all 3 vertex registers
void gte_ldv3(VOID *v0, VOID *v1, VOID *v2)
{
	vr0.vx = ((SVECTOR *)v0)->vx;
	vr0.vy = ((SVECTOR *)v0)->vy;
	vr0.vz = ((SVECTOR *)v0)->vz;

	vr1.vx = ((SVECTOR *)v1)->vx;
	vr1.vy = ((SVECTOR *)v1)->vy;
	vr1.vz = ((SVECTOR *)v1)->vz;

	vr2.vx = ((SVECTOR *)v2)->vx;
	vr2.vy = ((SVECTOR *)v2)->vy;
	vr2.vz = ((SVECTOR *)v2)->vz;
}

// Load rgb vectors into colour register 0
void gte_ldrgb(u_char *v)
{
	cr.r = *v;
	cr.g = *(v + 1);
	cr.b = *(v + 2);
	cr.cd = *(v + 3);
}

// Load 3 rgb vectors into colour registers
void gte_ldrgb3(u_char *v0, u_char *v1, u_char *v2)
{
	cr0.r = *v0;
	cr0.g = *(v0 + 1);
	cr0.b = *(v0 + 2);
	cr0.cd = *(v0 + 3);

	cr1.r = *v1;
	cr1.g = *(v1 + 1);
	cr1.b = *(v1 + 2);
	cr1.cd = *(v1 + 3);

	cr2.r = *v2;
	cr2.g = *(v2 + 1);
	cr2.b = *(v2 + 2);
	cr2.cd = *(v2 + 3);

	cr.r = cr2.r;
	cr.g = cr2.g;
	cr.b = cr2.b;
	cr.cd = cr2.cd;
}

// Load continuous vertex to vertex register
void gte_ldv3c(VOID *v)
{
	SVECTOR *temp = (SVECTOR *)v;
	vr0.vx = temp->vx;
	vr0.vy = temp->vy;
	vr0.vz = temp->vz;

	temp++;
	vr1.vx = temp->vx;
	vr1.vy = temp->vy;
	vr1.vz = temp->vz;

	temp++;
	vr2.vx = temp->vx;
	vr2.vy = temp->vy;
	vr2.vz = temp->vz;
}

// Load vector 
void gte_ldlvl(VOID *v)
{
	register long *temp = (long *)v;
	sv.vx = (short)*temp;
	*temp++;
	sv.vy = (short)*temp;
	*temp++;
	sv.vz = (short)*temp;
}

// Load values to screen XY coordinates
void gte_ldsxy3(long sxy0, long sxy1, long sxy2)
{
	screenxy[0].vx = (sxy0 & 0xffff);
	screenxy[0].vy = (short)(sxy0 >> 16);

	screenxy[2].vx = (sxy1 & 0xffff);
	screenxy[2].vy = (short)(sxy1 >> 16);
	
	screenxy[1].vx = (sxy2 & 0xffff);
	screenxy[1].vy = (short)(sxy2 >> 16);
}

void gte_ldsxy4(long sxy0, long sxy1, long sxy2, long sxy3)
{
	screenxy[0].vx = (sxy0 & 0xffff);
	screenxy[0].vy = (short)(sxy0 >> 16);

	screenxy[2].vx = (sxy1 & 0xffff);
	screenxy[2].vy = (short)(sxy1 >> 16);
	
	screenxy[1].vx = (sxy2 & 0xffff);
	screenxy[1].vy = (short)(sxy2 >> 16);
	
	screenxy[3].vx = (sxy3 & 0xffff);
	screenxy[3].vy = (short)(sxy2 >> 16);
}

// Load values to 3 screen Z coordinates
void gte_ldsz3(long sz0, long sz1, long sz2)
{
	screenxy[0].vz = (short)sz0;
	screenxy[1].vz = (short)sz1;
	screenxy[2].vz = (short)sz2;
}

// Load values to 4 screen Z coordinates
void gte_ldsz4(long sz0, long sz1, long sz2, long sz3)
{
	screenxy[0].vz = (short)sz0;
	screenxy[1].vz = (short)sz1;
	screenxy[2].vz = (short)sz2;
	screenxy[3].vz = (short)sz3;
}

// Load values to depth cue
void gte_lddp(long p)
{
	dc = p;
}

// Load value to 16-bit vector
void gte_ldsv(SVECTOR *v)
{
	sv.vx = v->vx;
	sv.vy = v->vy;
	sv.vz = v->vz;
}

// Store colour register 0 to RGB vector
void gte_strgb(u_char *v)
{
	*v = cr.r;
	*(v + 1) = cr.g;
	*(v + 2) = cr.b;
	*(v + 3) = cr.cd;
}

// Store all 3 colour registers to RGB vectors
void gte_strgb3(u_char *v0, u_char *v1, u_char *v2)
{
	*v0 = cr0.r;
	*(v0 + 1) = cr0.g;
	*(v0 + 2) = cr0.b;
	*(v0 + 3) = cr0.cd;

	*v1 = cr1.r;
	*(v1 + 1) = cr1.g;
	*(v1 + 2) = cr1.b;
	*(v1 + 3) = cr1.cd;

	*v2 = cr2.r;
	*(v2 + 1) = cr2.g;
	*(v2 + 2) = cr2.b;
	*(v2 + 3) = cr2.cd;
}

// Store one set of screen co-ordinates
void gte_stsxy(VOID *sxy)
{
	((short *)sxy)[0] =screenxy[2].vx;
	((short *)sxy)[1] =screenxy[2].vy;
}

// Store screen XY coordinate 0 to long
void gte_stsxy0(VOID *sxy)
{
	((short *)sxy)[0] =screenxy[0].vx;
	((short *)sxy)[1] =screenxy[0].vy;
}

// Store screen XY coordinate 1 to long
void gte_stsxy1(VOID *sxy)
{
	((short *)sxy)[0] =screenxy[1].vx;
	((short *)sxy)[1] =screenxy[1].vy;
}

// Store screen XY coordinate 2 to long 
void gte_stsxy2(VOID *sxy)
{
	((short *)sxy)[0] =screenxy[2].vx;
	((short *)sxy)[1] =screenxy[2].vy;
}

// Store all 3 screen XY coordinates to longs
void gte_stsxy3(VOID *sxy0, VOID *sxy1, VOID *sxy2)
{
	((short *)sxy0)[0] =screenxy[0].vx;
	((short *)sxy0)[1] =screenxy[0].vy;

	((short *)sxy1)[0] =screenxy[1].vx;
	((short *)sxy1)[1] =screenxy[1].vy;

	((short *)sxy2)[0] =screenxy[2].vx;
	((short *)sxy2)[1] =screenxy[2].vy;
}

// Store one set of co-ordinates
void gte_stsz(VOID *sz)
{
	register long *temp = (long *)sz;
	*((long *)temp) = screenxy[2].vz;
}

// Store all 3 screen Z coordinates to longs
void gte_stsz3(long *sz0, long *sz1, long *sz2)
{
	*sz0 = screenxy[0].vz;
	*sz1 = screenxy[1].vz;
	*sz2 = screenxy[2].vz;
}

// Store all 3 screen Z coordinates to continuous vertices
void gte_stsz3c(VOID *sz)
{
	register long *temp = (long *)sz;

	*temp = screenxy[0].vz;
	temp++;
	*temp = screenxy[1].vz;
	temp++;
	*temp = screenxy[2].vz;
}

// Store all 3 screen XY coordinates to continuous vertices
void gte_stsxy3c(VOID *sxy)
{
	register short *temp0 = (short *)sxy;

	temp0[0] =screenxy[0].vx;
	temp0[1] =screenxy[0].vy;
	temp0++;
	temp0[0] =screenxy[1].vx;
	temp0[1] =screenxy[1].vy;
	temp0++;
	temp0[0] =screenxy[2].vx;
	temp0[1] =screenxy[2].vy;
}

// Store short (16 bit) vector
void gte_stsv(VOID *v)
{
	((SVECTOR *)v)->vx = sv.vx;
	((SVECTOR *)v)->vy = sv.vy;
	((SVECTOR *)v)->vz = sv.vz;
}

// Store screen XY coordinates to POLY_F3 type
void gte_stsxy3_f3(POLY_F3 *packet)
{
	packet->x0 = screenxy[0].vx;
	packet->y0 = screenxy[0].vy;

	packet->x1 = screenxy[1].vx;
	packet->y1 = screenxy[1].vy;
	
	packet->x2 = screenxy[2].vx;
	packet->y2 = screenxy[2].vy;
}

// Store screen XY coordinates to POLY_F4 type
void gte_stsxy3_f4(POLY_F4 *packet)
{
	packet->x0 = screenxy[0].vx;
	packet->y0 = screenxy[0].vy;
	
	packet->x1 = screenxy[1].vx;
	packet->y1 = screenxy[1].vy;
	
	packet->x2 = screenxy[2].vx;
	packet->y2 = screenxy[2].vy;
}

// Store screen XY coordinates to POLY_FT3 type
void gte_stsxy3_ft3(POLY_FT3 *packet)
{
	packet->x0 = screenxy[0].vx;
	packet->y0 = screenxy[0].vy;
	
	packet->x1 = screenxy[1].vx;
	packet->y1 = screenxy[1].vy;
	
	packet->x2 = screenxy[2].vx;
	packet->y2 = screenxy[2].vy;
}

// Store screen XY coordinates to POLY_FT4 type
void gte_stsxy3_ft4(POLY_FT4 *packet)
{
	packet->x0 = screenxy[0].vx;
	packet->y0 = screenxy[0].vy;

	packet->x1 = screenxy[1].vx;
	packet->y1 = screenxy[1].vy;
	
	packet->x2 = screenxy[2].vx;
	packet->y2 = screenxy[2].vy;
}

// Store screen XY coordinates to POLY_G3 type
void gte_stsxy3_g3(POLY_G3 *packet)
{
	packet->x0 = screenxy[0].vx;
	packet->y0 = screenxy[0].vy;
	
	packet->x1 = screenxy[1].vx;
	packet->y1 = screenxy[1].vy;
	
	packet->x2 = screenxy[2].vx;
	packet->y2 = screenxy[2].vy;
}

// Store screen XY coordinates to POLY_G4 type
void gte_stsxy3_g4(POLY_G4 *packet)
{
	packet->x0 = screenxy[0].vx;
	packet->y0 = screenxy[0].vy;
	
	packet->x1 = screenxy[1].vx;
	packet->y1 = screenxy[1].vy;
	
	packet->x2 = screenxy[2].vx;
	packet->y2 = screenxy[2].vy;
}

// Store screen XY coordinates to POLY_GT3 type
void gte_stsxy3_gt3(POLY_GT3 *packet)
{
	packet->x0 = screenxy[0].vx;
	packet->y0 = screenxy[0].vy;
	
	packet->x1 = screenxy[1].vx;
	packet->y1 = screenxy[1].vy;
	
	packet->x2 = screenxy[2].vx;
	packet->y2 = screenxy[2].vy;
}


// Store screen XY coordinates to POLY_GT4 type
void gte_stsxy3_gt4(POLY_GT4 *packet)
{
	packet->x0 = screenxy[0].vx;
	packet->y0 = screenxy[0].vy;
	
	packet->x1 = screenxy[1].vx;
	packet->y1 = screenxy[1].vy;
	
	packet->x2 = screenxy[2].vx;
	packet->y2 = screenxy[2].vy;
}




/* ---------------------------------------------------------
   Function : gte_SetRotMatrix
   Purpose : PSX gte emulation set rotate matrix
   Parameters : MATRIX structure pointer
   Returns : 
   Info : change '#if 1' below to '#if 0' to use original PSX version
*/

#if 1
// ** Float version
void gte_SetRotMatrix(MATRIX *m)
{
	register float div = INV4096;

	RotMatrix.m[0][0] = m->m[0][0];
	RotMatrix.m[0][1] = m->m[0][1];
	RotMatrix.m[0][2] = m->m[0][2];

	RotMatrix.m[1][0] = m->m[1][0];
	RotMatrix.m[1][1] = m->m[1][1];
	RotMatrix.m[1][2] = m->m[1][2];

	RotMatrix.m[2][0] = m->m[2][0];
	RotMatrix.m[2][1] = m->m[2][1];
	RotMatrix.m[2][2] = m->m[2][2];

	// also set in floats
	fRotMatrix[0][0] = ((float)m->m[0][0]) * div;
	fRotMatrix[0][1] = ((float)m->m[0][1]) * div;
	fRotMatrix[0][2] = ((float)m->m[0][2]) * div;

	fRotMatrix[1][0] = ((float)m->m[1][0]) * div;
	fRotMatrix[1][1] = ((float)m->m[1][1]) * div;
	fRotMatrix[1][2] = ((float)m->m[1][2]) * div;

	fRotMatrix[2][0] = ((float)m->m[2][0]) * div;
	fRotMatrix[2][1] = ((float)m->m[2][1]) * div;
	fRotMatrix[2][2] = ((float)m->m[2][2]) * div;
}

#else

// ** Original PSX version
void gte_SetRotMatrix(MATRIX *m)
{
	RotMatrix.m[0][0] = m->m[0][0];
	RotMatrix.m[0][1] = m->m[0][1];
	RotMatrix.m[0][2] = m->m[0][2];

	RotMatrix.m[1][0] = m->m[1][0];
	RotMatrix.m[1][1] = m->m[1][1];
	RotMatrix.m[1][2] = m->m[1][2];

	RotMatrix.m[2][0] = m->m[2][0];
	RotMatrix.m[2][1] = m->m[2][1];
	RotMatrix.m[2][2] = m->m[2][2];
}
#endif


/* ---------------------------------------------------------
   Function : gte_SetTransMatrix
   Purpose : PSX gte emulation set translation matrix
   Parameters : MATRIX structure pointer
   Returns : 
   Info : change '#if 1' below to '#if 0' to use original PSX version
*/

#if 1
// ** Float version
void gte_SetTransMatrix(MATRIX *m)
{
	TransVector.vx = m->t[0];
	TransVector.vy = m->t[1];
	TransVector.vz = m->t[2];

	fTransVector[0] = (float)TransVector.vx;
	fTransVector[1] = (float)TransVector.vy;
	fTransVector[2] = (float)TransVector.vz;
}

#else

// ** Original PSX version
void gte_SetTransMatrix(MATRIX *m)
{
	TransVector.vx = m->t[0];
	TransVector.vy = m->t[1];
	TransVector.vz = m->t[2];
}
#endif


// Set the far colour with RGB values
void gte_SetFarColor(u_char rfc, u_char gfc, u_char bfc)
{
	farc.vx = rfc;
	farc.vy = gfc;
	farc.vz = bfc;
}


// *ASL* 17/08/2000
/* ---------------------------------------------------------
   Function : gte_MulMatrix0
   Purpose : PSX gte emulation mulitply two 3x3 matrices
   Parameters : MATRIX pointers, m0 * m1 = m2
   Returns : m2 
   Info : change '#if 1' below to '#if 0' to use original PSX version
*/

#if 1
// ** Hopefully optimised PSX emulation matrix multiply
void gte_MulMatrix0(MATRIX *m0, MATRIX *m1, MATRIX *m2) 
{
	// are our matrices are non-aliased?
	if (m1 != m2)
	{
		// ** non-aliased matrices

		register short	*mp0, *mp1, *mp2;
		register int	r0,r1,r2, a0,a1,a2;
		register int	shift = 12;

		mp0 = &m0->m[0][0];
		mp2 = &m2->m[0][0];

		r0 = *mp0++;					// m0[0][0]
		r1 = *mp0++;					// m0[0][1]
		r2 = *mp0++;					// m0[0][2]

		mp1 = &m1->m[0][0];
		a0  = ((*mp1++ * r0) >>shift);
		a1  = ((*mp1++ * r0) >>shift);
		a2  = ((*mp1++ * r0) >>shift);
		a0 += ((*mp1++ * r1) >>shift);
		a1 += ((*mp1++ * r1) >>shift);
		a2 += ((*mp1++ * r1) >>shift);
		a0 += ((*mp1++ * r2) >>shift);
		a1 += ((*mp1++ * r2) >>shift);
		a2 += ((*mp1   * r2) >>shift);
		*mp2++ = a0;
		*mp2++ = a1;
		*mp2++ = a2;

		r0 = *mp0++;					// m0[1][0]
		r1 = *mp0++;					// m0[1][1]
		r2 = *mp0++;					// m0[1][2]

		mp1 = &m1->m[0][0];
		a0  = ((*mp1++ * r0) >>shift);
		a1  = ((*mp1++ * r0) >>shift);
		a2  = ((*mp1++ * r0) >>shift);
		a0 += ((*mp1++ * r1) >>shift);
		a1 += ((*mp1++ * r1) >>shift);
		a2 += ((*mp1++ * r1) >>shift);
		a0 += ((*mp1++ * r2) >>shift);
		a1 += ((*mp1++ * r2) >>shift);
		a2 += ((*mp1   * r2) >>shift);
		*mp2++ = a0;
		*mp2++ = a1;
		*mp2++ = a2;

		r0 = *mp0++;					// m0[2][0]
		r1 = *mp0++;					// m0[2][1]
		r2 = *mp0;						// m0[2][2]

		mp1 = &m1->m[0][0];
		a0  = ((*mp1++ * r0) >>shift);
		a1  = ((*mp1++ * r0) >>shift);
		a2  = ((*mp1++ * r0) >>shift);
		a0 += ((*mp1++ * r1) >>shift);
		a1 += ((*mp1++ * r1) >>shift);
		a2 += ((*mp1++ * r1) >>shift);
		a0 += ((*mp1++ * r2) >>shift);
		a1 += ((*mp1++ * r2) >>shift);
		a2 += ((*mp1   * r2) >>shift);
		*mp2++ = a0;
		*mp2++ = a1;
		*mp2   = a2;

	}
	else
	{
		// ** aliased matrices

		short			mat[9];
		register short	*mp0, *mp1, *mp2;
		register int	r0,r1,r2, a0,a1,a2;
		register int	shift = 12;

		mp0 = &m0->m[0][0];
		mp2 = &m1->m[0][0];

		// copy aliased matrix into working mat
		memcpy(mat, &m2->m[0][0], sizeof(short)*3*3);

		r0 = *mp0++;					// m0[0][0]
		r1 = *mp0++;					// m0[0][1]
		r2 = *mp0++;					// m0[0][2]

		mp1 = mat;
		a0  = ((*mp1++ * r0) >>shift);
		a1  = ((*mp1++ * r0) >>shift);
		a2  = ((*mp1++ * r0) >>shift);
		a0 += ((*mp1++ * r1) >>shift);
		a1 += ((*mp1++ * r1) >>shift);
		a2 += ((*mp1++ * r1) >>shift);
		a0 += ((*mp1++ * r2) >>shift);
		a1 += ((*mp1++ * r2) >>shift);
		a2 += ((*mp1   * r2) >>shift);
		*mp2++ = a0;
		*mp2++ = a1;
		*mp2++ = a2;

		r0 = *mp0++;					// m0[1][0]
		r1 = *mp0++;					// m0[1][1]
		r2 = *mp0++;					// m0[1][2]

		mp1 = mat;
		a0  = ((*mp1++ * r0) >>shift);
		a1  = ((*mp1++ * r0) >>shift);
		a2  = ((*mp1++ * r0) >>shift);
		a0 += ((*mp1++ * r1) >>shift);
		a1 += ((*mp1++ * r1) >>shift);
		a2 += ((*mp1++ * r1) >>shift);
		a0 += ((*mp1++ * r2) >>shift);
		a1 += ((*mp1++ * r2) >>shift);
		a2 += ((*mp1   * r2) >>shift);
		*mp2++ = a0;
		*mp2++ = a1;
		*mp2++ = a2;

		r0 = *mp0++;					// m0[2][0]
		r1 = *mp0++;					// m0[2][1]
		r2 = *mp0;						// m0[2][2]

		mp1 = mat;
		a0  = ((*mp1++ * r0) >>shift);
		a1  = ((*mp1++ * r0) >>shift);
		a2  = ((*mp1++ * r0) >>shift);
		a0 += ((*mp1++ * r1) >>shift);
		a1 += ((*mp1++ * r1) >>shift);
		a2 += ((*mp1++ * r1) >>shift);
		a0 += ((*mp1++ * r2) >>shift);
		a1 += ((*mp1++ * r2) >>shift);
		a2 += ((*mp1   * r2) >>shift);
		*mp2++ = a0;
		*mp2++ = a1;
		*mp2   = a2;
	}
}

#else

void gte_MulMatrix0(MATRIX *m0, MATRIX *m1, MATRIX *m2) 
{
	register MATRIX tempmat;
	int		 i,j,k;

	tempmat.t[0] = m2->t[0];
	tempmat.t[1] = m2->t[1];
	tempmat.t[2] = m2->t[2];

	for (i=0; i<3; i++)
		for (j=0; j<3; j++)
			tempmat.m[i][j] = 0;

	// Do necessary multiplication
	for (i=0; i<3; i++)
		for (j=0; j<3; j++)
			for (k=0; k<3; k++)
				tempmat.m[i][j] += ((m1->m[k][j] * m0->m[i][k])>> 12);

	*m2 = tempmat;
}
#endif


// Get Z average of three Z values
void gte_avsz3(void)
{
	otz =  (((short)screenxy[0].vz + (short)screenxy[1].vz + (short)screenxy[2].vz)*0.0833);
//	otz =  (((short)screenxy[0].vz + (short)screenxy[1].vz + (short)screenxy[2].vz)/3)/4;
}

// Get Z average of three Z values (with no nop)
void gte_avsz3_b(void)
{
	otz =  (((short)screenxy[0].vz + (short)screenxy[1].vz + (short)screenxy[2].vz)*0.0833);
//	otz =  (((short)screenxy[0].vz + (short)screenxy[1].vz + (short)screenxy[2].vz)/3)/4;
}

// Get Z average of four Z values
void gte_avsz4(void)
{
	otz = (((short)screenxy[0].vz + (short)screenxy[1].vz + (short)screenxy[2].vz + screenxy[3].vz)*0.0625);
//	otz = (((short)screenxy[0].vz + (short)screenxy[1].vz + (short)screenxy[2].vz + screenxy[3].vz)/4)/4;
}

// Get Z average of four Z values (with no nop)
void gte_avsz4_b(void)
{
	otz = (((short)screenxy[0].vz + (short)screenxy[1].vz + (short)screenxy[2].vz + screenxy[3].vz)*0.0625);
//	otz = (((short)screenxy[0].vz + (short)screenxy[1].vz + (short)screenxy[2].vz + screenxy[3].vz)/4)/4;
}

// Store otz 
void gte_stszotz(long *oz)
{
	*oz = (screenxy[2].vz)*0.0625;
//	*oz = (screenxy[2].vz)/16;
}

// Store otz as long????
void gte_stotz(VOID *oz)
{
	long *temp = (long *)oz;
	*temp = otz;
}

// Store opz as long
void gte_stopz(long *oz)
{
	*oz = opz;
}

// Do nothing
void gte_nop(void)
{
	/* No operation!!! */
}

// Kernel of NormalClip
void gte_nclip(void)
{
	// Dot product of screenxy
	opz = ((screenxy[0].vx * screenxy[2].vy) + 
		(screenxy[2].vx * screenxy[1].vy) + 
		(screenxy[1].vx * screenxy[0].vy) -
		(screenxy[0].vx * screenxy[1].vy) - 
		(screenxy[2].vx * screenxy[0].vy) - 
		(screenxy[1].vx * screenxy[2].vy));
}

// Kernel of NormalClip (with no nop)
void gte_nclip_b(void)
{
	gte_nclip();
}

//  Store 16 bit vector
void gte_stlvl(VOID *v)
{
	register long *temp = (long *)v;
	*temp = sv.vx;
	temp++;
	*temp = sv.vy;
	temp++;
	*temp = sv.vz;
}

// Store 32 bit vector
void gte_stlvnl(VOID *v)
{
	register long *temp = (long *)v;

	*temp = lv.vx;
	temp++;
	*temp = lv.vy;
	temp++;
	*temp = lv.vz;
}


/* ---------------------------------------------------------
   Function : gte_rtv0
   Purpose : PSX emulation rotation matrix * vertex register 0
   Parameters : internals
   Returns : 
   Info : change '#if 1' below to '#if 0' to use original PSX version
*/

#if 1
// ** Hopefully optimised PSX emulation vertex matrix mulitply
void gte_rtv0(void)
{
	register int	vrx = (int)vr0.vx, vry = (int)vr0.vy, vrz = (int)vr0.vz;
	register int	n, shift = 12;
	register short	*mp = &RotMatrix.m[0][0];
	register short	*sp = &sv.vx;

	n  = *mp++ * vrx;
	n += *mp++ * vry;
	n += *mp++ * vrz;
	n >>= shift;
	*sp++ = (short)n;
	n  = *mp++ * vrx;
	n += *mp++ * vry;
	n += *mp++ * vrz;
	n >>= shift;
	*sp++ = (short)n;
	n  = *mp++ * vrx;
	n += *mp++ * vry;
	n += *mp   * vrz;
	n >>= shift;
	*sp   = (short)n;
}

#else

void gte_rtv0(void)
{
	sv.vx = (((RotMatrix.m[0][0] * vr0.vx) + 
		(RotMatrix.m[0][1] * vr0.vy) + (RotMatrix.m[0][2] * vr0.vz)) >> 12);
	sv.vy = (((RotMatrix.m[1][0] * vr0.vx) + 
		(RotMatrix.m[1][1] * vr0.vy) + (RotMatrix.m[1][2] * vr0.vz)) >> 12);
	sv.vz = (((RotMatrix.m[2][0] * vr0.vx) + 
		(RotMatrix.m[2][1] * vr0.vy) + (RotMatrix.m[2][2] * vr0.vz)) >> 12);
}
#endif


// *ASL* 18/08/2000
/* ---------------------------------------------------------
   Function : gte_rtv0tr
   Purpose : PSX emulation rotation matrix * vr0 vertex register 0 + translate
   Parameters : internals
   Returns : 
   Info : change '#if 1' below to '#if 0' to use original PSX version
*/

// (rt * v0) + tr
#if 1
// ** Hopefully optimised PSX emulation vertex matrix mulitply
void gte_rtv0tr(void)
{
	register int	vrx = (int)vr0.vx, vry = (int)vr0.vy, vrz = (int)vr0.vz;
	register int	tx = TransVector.vx, ty = TransVector.vy, tz = TransVector.vz;
	register int	n, shift = 12;
	register short	*mp = &RotMatrix.m[0][0];
	register short	*sp = &sv.vx;

	n  = *mp++ * vrx;
	n += *mp++ * vry;
	n += *mp++ * vrz;
	n >>= shift;
	n += tx;
	*sp++ = (short)n;
	n  = *mp++ * vrx;
	n += *mp++ * vry;
	n += *mp++ * vrz;
	n >>= shift;
	n += ty;
	*sp++ = (short)n;
	n  = *mp++ * vrx;
	n += *mp++ * vry;
	n += *mp   * vrz;
	n >>= shift;
	n += tz;
	*sp   = (short)n;
}

#else

void gte_rtv0tr(void)
{
	sv.vx = ((((RotMatrix.m[0][0] * vr0.vx) + (RotMatrix.m[0][1] * vr0.vy)
		+ (RotMatrix.m[0][2] * vr0.vz)) >> 12) + TransVector.vx);
	sv.vy = ((((RotMatrix.m[1][0] * vr0.vx) + (RotMatrix.m[1][1] * vr0.vy)
		+ (RotMatrix.m[1][2] * vr0.vz)) >> 12) + TransVector.vy);
	sv.vz = ((((RotMatrix.m[2][0] * vr0.vx) + (RotMatrix.m[2][1] * vr0.vy)
		+ (RotMatrix.m[2][2] * vr0.vz)) >> 12) + TransVector.vz);
}
#endif


// Transfer Vector + Rotation Matrix * Vertex Register 0
void gte_rt(void)
{
	sv.vx = ((((RotMatrix.m[0][0] * vr0.vx) + 
		(RotMatrix.m[0][1] * vr0.vy) + (RotMatrix.m[0][2] * vr0.vz)) >> 12)
		+ (short)TransVector.vx);

	sv.vy = ((((RotMatrix.m[1][0] * vr0.vx) + 
		(RotMatrix.m[1][1] * vr0.vy) + (RotMatrix.m[1][2] * vr0.vz)) >> 12)
		+ (short)TransVector.vy);

	sv.vz = ((((RotMatrix.m[2][0] * vr0.vx) + 
		(RotMatrix.m[2][1] * vr0.vy) + (RotMatrix.m[2][2] * vr0.vz)) >> 12)
		+ (short)TransVector.vz);

	lv.vx = (long)sv.vx;
	lv.vy = (long)sv.vy;
	lv.vz = (long)sv.vz;
}


// *ASL* 18/08/2000
/* ---------------------------------------------------------
   Function : gte_rtirtr
   Purpose : PSX emulation rotation matrix * sv vertex register + translate
   Parameters : internals
   Returns : 
   Info : change '#if 1' below to '#if 0' to use original PSX version
*/

// rt * sv + tr
#if 1
// ** Hopefully optimised PSX emulation vertex matrix mulitply
void gte_rtirtr(void)
{
	register int	svx = (int)sv.vx, svy = (int)sv.vy, svz = (int)sv.vz;
	register int	tx = TransVector.vx, ty = TransVector.vy, tz = TransVector.vz;
	register int	n, shift = 12;
	register short	*mp = &RotMatrix.m[0][0];
	register short	*sp = &sv.vx;

	n  = *mp++ * svx;
	n += *mp++ * svy;
	n += *mp++ * svz;
	n >>= shift;
	n += tx;
	*sp++ = (short)n;
	n  = *mp++ * svx;
	n += *mp++ * svy;
	n += *mp++ * svz;
	n >>= shift;
	n += ty;
	*sp++ = (short)n;
	n  = *mp++ * svx;
	n += *mp++ * svy;
	n += *mp   * svz;
	n >>= shift;
	n += tz;
	*sp   = (short)n;
}

#else

void gte_rtirtr(void)
{
	register SVECTOR tempsv;
	tempsv.vx = sv.vx;
	tempsv.vy = sv.vy;
	tempsv.vz = sv.vz;

	sv.vx = ((((RotMatrix.m[0][0] * tempsv.vx) + 
		(RotMatrix.m[0][1] * tempsv.vy) + (RotMatrix.m[0][2] * tempsv.vz)) >> 12)
		 + (short)TransVector.vx);
	sv.vy = ((((RotMatrix.m[1][0] * tempsv.vx) + 
		(RotMatrix.m[1][1] * tempsv.vy) + (RotMatrix.m[1][2] * tempsv.vz)) >> 12)
		 + (short)TransVector.vy);
	sv.vz = ((((RotMatrix.m[2][0] * tempsv.vx) + 
		(RotMatrix.m[2][1] * tempsv.vy) + (RotMatrix.m[2][2] * tempsv.vz)) >> 12)
		 + (short)TransVector.vz);
}
#endif


// Same but with no nop
void gte_rtirtr_b(void)
{
	gte_rtirtr();
}

// rt * sv
void gte_rtir(void)
{
	register SVECTOR tempsv;
	tempsv.vx = sv.vx;
	tempsv.vy = sv.vy;
	tempsv.vz = sv.vz;

	sv.vx = (((RotMatrix.m[0][0] * tempsv.vx) + 
		(RotMatrix.m[0][1] * tempsv.vy) + (RotMatrix.m[0][2] * tempsv.vz)) >> 12);
	sv.vy = (((RotMatrix.m[1][0] * tempsv.vx) + 
		(RotMatrix.m[1][1] * tempsv.vy) + (RotMatrix.m[1][2] * tempsv.vz)) >> 12);
	sv.vz = (((RotMatrix.m[2][0] * tempsv.vx) + 
		(RotMatrix.m[2][1] * tempsv.vy) + (RotMatrix.m[2][2] * tempsv.vz)) >> 12);
}

// Kernel of Square0
void gte_sqr0(void)
{
	lv.vx = ((long)sv.vx * (long)sv.vx);
	lv.vy = ((long)sv.vy * (long)sv.vy);
	lv.vz = ((long)sv.vz * (long)sv.vz);

	sv.vx = lv.vx;
	sv.vy = lv.vy;
	sv.vz = lv.vz;

	//lv->vx = sv->vx;
	//lv->vy = sv->vy;
	//lv->vz = sv->vz;
}

// Kernel of Square0 (with no nop)
void gte_sqr0_b(void)
{
	lv.vx = ((long)sv.vx * (long)sv.vx);
	lv.vy = ((long)sv.vy * (long)sv.vy);
	lv.vz = ((long)sv.vz * (long)sv.vz);

	sv.vx = lv.vx;
	sv.vy = lv.vy;
	sv.vz = lv.vz;
}

// Load a long vector to far colour 
void gte_ldfc(VECTOR *vc)
{
	farc.vx = vc->vx;
	farc.vy = vc->vy;
	farc.vz = vc->vz;
}

// Load vector to far colour for interpolation
void gte_ld_intpol_sv0(VOID *v)
{
	farc.vx = ((SVECTOR *)v)->vx;
	farc.vy = ((SVECTOR *)v)->vy;
	farc.vz = ((SVECTOR *)v)->vz;
}

// Load vector to short vector for interpolation
void gte_ld_intpol_sv1(VOID *v)
{
	sv.vx = ((SVECTOR *)v)->vx;
	sv.vy = ((SVECTOR *)v)->vy;
	sv.vz = ((SVECTOR *)v)->vz;
}


// *ASL* 17/08/2000
/* ---------------------------------------------------------
   Function : gte_rtps
   Purpose : PSX gte emulation transform to screen function
   Parameters : internal vars
   Returns : 
   Info : change '#if 1' below to '#if 0' to use original PSX version
*/

// (rt * v0) + tr
#if 1
#pragma inline(gte_rtps)
// ** Hopefully optimised PSX emulation transform
void gte_rtps(void)
{
	register float	vrx = (float)vr0.vx, vry = (float)vr0.vy, vrz = (float)vr0.vz;
	register float	tx = fTransVector[0], ty = fTransVector[1], tz = fTransVector[2];
	register float	screenvx, screenvy, screenvz;
	register float	ps, psz;
	register short	*scrs = &screenxy[2].vx;
	register float	*mp = &fRotMatrix[0][0];

	// vector transform
	screenvx =  *mp++ * vrx;
	screenvx += *mp++ * vry;
	screenvx += *mp++ * vrz;
	screenvx += tx;

	screenvy =  *mp++ * vrx;
	screenvy += *mp++ * vry;
	screenvy += *mp++ * vrz;
	screenvy += ty;

	screenvz =  *mp++ * vrx;
	screenvz += *mp++ * vry;
	screenvz += *mp   * vrz;
	screenvz += tz;

	// limit perpspective calculation
	if (screenvz < fGShHalf)
	{
		ps = fGShLimit;
		psz = fGSh / screenvz;
	}
	else
	{
		ps = psz = fGSh / screenvz;
	}

	// calculate PSX opz
	opz = (dqb + (dqa * psz) * 4096.0f) * 16.0f;

	// perspective calculation
	screenvx *= ps;
	screenvy *= ps;
	screenvx += fGSx;
	screenvy += fGSy;

	// limit the short output.. !!really need to store floats!!
	if (screenvx < -1024.0f)
		screenvx = -1024.0f;
	else if (screenvx > 1023.0f)
		screenvx = 1023.0f;
	if (screenvy < -1024.0f)
		screenvy = -1024.0f;
	else if (screenvy > 1023.0f)
		screenvy = 1023.0f;

	*scrs++ = (short)screenvx;
	*scrs++ = (short)screenvy;
	*scrs   = (short)screenvz;
}

#else

// ** Original Transform
void gte_rtps(void)
{	
	register float calc1;
	register unsigned short ustempz;
	float	fscreenvx, fscreenvy, fscreenvz;

	// 1st calculation
	fscreenvx = ((((RotMatrix.m[0][0] * vr0.vx) + (RotMatrix.m[0][1] * vr0.vy)
		+ (RotMatrix.m[0][2] * vr0.vz)) >> 12) + TransVector.vx);
	fscreenvy = ((((RotMatrix.m[1][0] * vr0.vx) + (RotMatrix.m[1][1] * vr0.vy)
		+ (RotMatrix.m[1][2] * vr0.vz)) >> 12) + TransVector.vy);
	fscreenvz = ((((RotMatrix.m[2][0] * vr0.vx) + (RotMatrix.m[2][1] * vr0.vy)
		+ (RotMatrix.m[2][2] * vr0.vz)) >> 12) + TransVector.vz);

	// LimA1S, A2S, A3S
	if (fscreenvx < -32768) fscreenvx = -32768;
	if (fscreenvy < -32768) fscreenvy = -32768;
	if (fscreenvz < -32768) fscreenvz = -32768;
	if (fscreenvx > 32767) fscreenvx = 32767;
	if (fscreenvy > 32767) fscreenvy = 32767;
	if (fscreenvz > 32767) fscreenvz = 32767;

	// Convert to unsigned short
	ustempz  = (unsigned short)fscreenvz;

	// limC
	if (ustempz < 0) ustempz = 0;
	if (ustempz > 65535) ustempz = 65535;

	fscreenvz = (short)ustempz;
	if (fscreenvz < 0) fscreenvz = 0;
	if (fscreenvz > 32767) fscreenvz = 32767;

	// PLAYSTATION INACCURACY!!
	if ((float)fscreenvz < (GSh/2))
		calc1 = (GSh/(GSh/2));
	else
		calc1 = (GSh/(float)fscreenvz);

	// 2nd calculation
	fscreenvx = (float)(GSx + (fscreenvx * calc1));
	fscreenvy = (float)(GSy + (fscreenvy * calc1));

	opz = (dqb + (dqa * (GSh/(float)fscreenvz))*4096)*16;

	// limD1 and D2
	if (fscreenvx < -1024) 
		fscreenvx = -1024;
	if (fscreenvx > 1023) 
		fscreenvx = 1023;
	if (fscreenvy < -1024) 
		fscreenvy = -1024;
	if (fscreenvy > 1023) 
		fscreenvy = 1023;
		
	screenxy[2].vx = (short)fscreenvx;
	screenxy[2].vy = (short)fscreenvy;
	screenxy[2].vz = (short)fscreenvz;		
}
#endif


// Same with no nop
void gte_rtps_b(void)
{
	gte_rtps();
}


// *ASL* 18/07/2000
/* ---------------------------------------------------------
   Function : RotTrans
   Purpose : PSX gte emulation rotate matrix vector transform
   Parameters : vector in pointer, vector out pointer, flag?
   Returns : 
   Info : change '#if 1' below to '#if 0' to use original PSX version
*/

// Similar to gte_rtps 
#if 1
// ** Hopefully optimised PSX emulation transform
void RotTrans(SVECTOR *v0, VECTOR *v1, long *flag)
{
	register int	ivx = v0->vx, ivy = v0->vy, ivz = v0->vz;
	register int	n, shift = 12;
	register long	*lp = &v1->vx;
	register short	*mp = &RotMatrix.m[0][0];

	n  = *mp++ * ivx;
	n += *mp++ * ivy;
	n += *mp++ * ivz;
	n >>= shift;
	*lp++ = (long)n;
	n  = *mp++ * ivx;
	n += *mp++ * ivy;
	n += *mp++ * ivz;
	n >>= shift;
	*lp++ = (long)n;
	n  = *mp++ * ivx;
	n += *mp++ * ivy;
	n += *mp   * ivz;
	n >>= shift;
	*lp   = (long)n;
}

#else

void RotTrans(SVECTOR *v0, VECTOR *v1, long *flag)
{
	v1->vx = (((RotMatrix.m[0][0] * v0->vx) + (RotMatrix.m[0][1] * v0->vy)
		+ (RotMatrix.m[0][2] * v0->vz)) >> 12);
	v1->vy = (((RotMatrix.m[1][0] * v0->vx) + (RotMatrix.m[1][1] * v0->vy)
		+ (RotMatrix.m[1][2] * v0->vz)) >> 12);
	v1->vz = (((RotMatrix.m[2][0] * v0->vx) + (RotMatrix.m[2][1] * v0->vy)
		+ (RotMatrix.m[2][2] * v0->vz)) >> 12);
}
#endif


// *ASL* 16/08/2000
/* ---------------------------------------------------------
   Function : gte_rtpt
   Purpose : PSX gte emulation transform to screen function
   Parameters : internal vars
   Returns : 
   Info : change '#if 1' below to '#if 0' to use original PSX version
*/

// (rt * v0) + tr for all 3
#if 1
// ** Hopefully optimised PSX emulation transform
void gte_rtpt(void)
{
	short			*scrs;
	// load all vr regs into floats
	float			vr0x = (float)vr0.vx, vr0y = (float)vr0.vy, vr0z = (float)vr0.vz;
	float			vr1x = (float)vr1.vx, vr1y = (float)vr1.vy, vr1z = (float)vr1.vz;
	float			vr2x = (float)vr2.vx, vr2y = (float)vr2.vy, vr2z = (float)vr2.vz;
	// working world space floats
	float			worlds[3*3], *wp;

	register float	ps, psz;
	register float	screenvx, screenvy, screenvz;
	register float	*mp = &fRotMatrix[0][0];


	// ** vector transform

	wp = worlds;

	// load and calc all xs
	psz = fTransVector[0];
	screenvx = *mp++; screenvy = *mp++; screenvz = *mp++;
	ps =  screenvx * vr0x; ps += screenvy * vr0y; ps += screenvz * vr0z; ps += psz;
	*wp++ = ps;
	ps =  screenvx * vr1x; ps += screenvy * vr1y; ps += screenvz * vr1z; ps += psz;
	*wp++ = ps;
	ps =  screenvx * vr2x; ps += screenvy * vr2y; ps += screenvz * vr2z; ps += psz;
	*wp++ = ps;
	// load and calc all ys
	psz = fTransVector[1];
	screenvx = *mp++; screenvy = *mp++; screenvz = *mp++;
	ps =  screenvx * vr0x; ps += screenvy * vr0y; ps += screenvz * vr0z; ps += psz;
	*wp++ = ps;
	ps =  screenvx * vr1x; ps += screenvy * vr1y; ps += screenvz * vr1z; ps += psz;
	*wp++ = ps;
	ps =  screenvx * vr2x; ps += screenvy * vr2y; ps += screenvz * vr2z; ps += psz;
	*wp++ = ps;
	// load and calc all zs
	psz = fTransVector[2];
	screenvx = *mp++; screenvy = *mp++; screenvz = *mp++;
	ps =  screenvx * vr0x; ps += screenvy * vr0y; ps += screenvz * vr0z; ps += psz;
	*wp++ = ps;
	ps =  screenvx * vr1x; ps += screenvy * vr1y; ps += screenvz * vr1z; ps += psz;
	*wp++ = ps;
	ps =  screenvx * vr2x; ps += screenvy * vr2y; ps += screenvz * vr2z; ps += psz;
	*wp++ = ps;


	// ** Vertex 0

	// limit perpspective calculation
	screenvz = *--wp;
	if (screenvz < fGShHalf)
	{
		ps = fGShHalf;
		psz = fGSh / screenvz;
	}
	else
	{
		ps = psz = fGSh / screenvz;
	}

	// perspective calculation
	screenvx = worlds[0] *= ps;
	screenvy = worlds[3] *= ps;
	screenvx += fGSx;
	screenvy += fGSy;

	if (screenvx < -1024.0f)
		screenvx = -1024.0f;
	else if (screenvx > 1023.0f)
		screenvx = 1023.0f;
	if (screenvy < -1024.0f)
		screenvy = -1024.0f;
	else if (screenvy > 1023.0f)
		screenvy = 1023.0f;

	scrs = &screenxy[0].vx;
	*scrs++ = (short)screenvx;
	*scrs++ = (short)screenvy;
	*scrs   = (short)screenvz;
	

	// ** Vertex 1
	screenvz = *--wp;
	if (screenvz < fGShHalf)
	{
		ps = fGShHalf;
		psz = fGSh / screenvz;
	}
	else
	{
		ps = psz = fGSh / screenvz;
	}

	screenvx = worlds[1] * ps;
	screenvy = worlds[4] * ps;
	screenvx += fGSx;
	screenvy += fGSy;

	if (screenvx < -1024.0f)
		screenvx = -1024.0f;
	else if (screenvx > 1023.0f)
		screenvx = 1023.0f;
	if (screenvy < -1024.0f)
		screenvy = -1024.0f;
	else if (screenvy > 1023.0f)
		screenvy = 1023.0f;

	scrs = &screenxy[1].vx;
	*scrs++ = (short)screenvx;
	*scrs++ = (short)screenvy;
	*scrs   = (short)screenvz;


	// ** Vertex 2
	screenvz = *--wp;
	if (screenvz < fGShHalf)
	{
		ps = fGShHalf;
		psz = fGSh / screenvz;
	}
	else
	{
		ps = psz = fGSh / screenvz;
	}

	// calculate PSX opz
	opz = (dqb + (dqa * psz) * 4096.0f) * 16.0f;

	screenvx = worlds[2] *= ps;
	screenvy = worlds[5] *= ps;
	screenvx += fGSx;
	screenvy += fGSy;

	if (screenvx < -1024.0f)
		screenvx = -1024.0f;
	else if (screenvx > 1023.0f)
		screenvx = 1023.0f;
	if (screenvy < -1024.0f)
		screenvy = -1024.0f;
	else if (screenvy > 1023.0f)
		screenvy = 1023.0f;

	scrs = &screenxy[2].vx;
	*scrs++ = (short)screenvx;
	*scrs++ = (short)screenvy;
	*scrs   = (short)screenvz;
}

#else

void gte_rtpt(void)
{
	register float calc1[3];
	register unsigned short ustempz[3];

	// First calculation
	screenxy[0].vx = ((((RotMatrix.m[0][0] * vr0.vx) + (RotMatrix.m[0][1] * vr0.vy)
		+ (RotMatrix.m[0][2] * vr0.vz)) >> 12) + TransVector.vx);
	screenxy[0].vy = ((((RotMatrix.m[1][0] * vr0.vx) + (RotMatrix.m[1][1] * vr0.vy)
		+ (RotMatrix.m[1][2] * vr0.vz)) >> 12) + TransVector.vy);
	screenxy[0].vz = ((((RotMatrix.m[2][0] * vr0.vx) + (RotMatrix.m[2][1] * vr0.vy)
		+ (RotMatrix.m[2][2] * vr0.vz)) >> 12) + TransVector.vz);

	screenxy[1].vx = ((((RotMatrix.m[0][0] * vr1.vx) + (RotMatrix.m[0][1] * vr1.vy)
		+ (RotMatrix.m[0][2] * vr1.vz)) >> 12) + TransVector.vx);
	screenxy[1].vy = ((((RotMatrix.m[1][0] * vr1.vx) + (RotMatrix.m[1][1] * vr1.vy)
		+ (RotMatrix.m[1][2] * vr1.vz)) >> 12) + TransVector.vy);
	screenxy[1].vz = ((((RotMatrix.m[2][0] * vr1.vx) + (RotMatrix.m[2][1] * vr1.vy)
		+ (RotMatrix.m[2][2] * vr1.vz)) >> 12) + TransVector.vz);

	screenxy[2].vx = ((((RotMatrix.m[0][0] * vr2.vx) + (RotMatrix.m[0][1] * vr2.vy)
		+ (RotMatrix.m[0][2] * vr2.vz)) >> 12) + TransVector.vx);
	screenxy[2].vy = ((((RotMatrix.m[1][0] * vr2.vx) + (RotMatrix.m[1][1] * vr2.vy)
		+ (RotMatrix.m[1][2] * vr2.vz)) >> 12) + TransVector.vy);
	screenxy[2].vz = ((((RotMatrix.m[2][0] * vr2.vx) + (RotMatrix.m[2][1] * vr2.vy)
		+ (RotMatrix.m[2][2] * vr2.vz)) >> 12) + TransVector.vz);

	// limA1S
	if (screenxy[0].vx < -32768) screenxy[0].vx = -32768;
	if (screenxy[1].vx < -32768) screenxy[1].vx = -32768;
	if (screenxy[2].vx < -32768) screenxy[2].vx = -32768;
	if (screenxy[0].vx > 32767) screenxy[0].vx = 32767;
	if (screenxy[1].vx > 32767) screenxy[1].vx = 32767;
	if (screenxy[2].vx > 32767) screenxy[2].vx = 32767;
	
	if (screenxy[0].vy < -32768) screenxy[0].vy = -32768;
	if (screenxy[1].vy < -32768) screenxy[1].vy = -32768;
	if (screenxy[2].vy < -32768) screenxy[2].vy = -32768;
	if (screenxy[0].vy > 32767) screenxy[0].vy = 32767;
	if (screenxy[1].vy > 32767) screenxy[1].vy = 32767;
	if (screenxy[2].vy > 32767) screenxy[2].vy = 32767;

	if (screenxy[0].vz < -32768) screenxy[0].vz = -32768;
	if (screenxy[1].vz < -32768) screenxy[1].vz = -32768;
	if (screenxy[2].vz < -32768) screenxy[2].vz = -32768;
	if (screenxy[0].vz > 32767) screenxy[0].vz = 32767;
	if (screenxy[1].vz > 32767) screenxy[1].vz = 32767;
	if (screenxy[2].vz > 32767) screenxy[2].vz = 32767;

	// Convert to unsigned short
	ustempz[0]  = (unsigned short)screenxy[0].vz;
	ustempz[1]  = (unsigned short)screenxy[1].vz;
	ustempz[2]  = (unsigned short)screenxy[2].vz;

	// limC
	if (ustempz[0] < 0) ustempz[0] = 0;
	if (ustempz[1] < 0) ustempz[1] = 0;
	if (ustempz[2] < 0) ustempz[2] = 0;

	if (ustempz[0] > 65535) ustempz[0] = 65535;
	if (ustempz[1] > 65535) ustempz[1] = 65535;
	if (ustempz[2] > 65535) ustempz[2] = 65535;

	screenxy[0].vz = (short)ustempz[0];
	screenxy[1].vz = (short)ustempz[1];
	screenxy[2].vz = (short)ustempz[2];

	// Extra limiter added (needed)
	if (screenxy[0].vz < 0) screenxy[0].vz = 0;
	if (screenxy[0].vz > 32767) screenxy[0].vz = 32767;
	if (screenxy[1].vz < 0) screenxy[1].vz = 0;
	if (screenxy[1].vz > 32767) screenxy[1].vz = 32767;
	if (screenxy[2].vz < 0) screenxy[2].vz = 0;
	if (screenxy[2].vz > 32767) screenxy[2].vz = 32767;

	// PLAYSTATION INACCURACY!!
	if ((float)screenxy[0].vz < (GSh/2))
		calc1[0] = (GSh/(GSh/2));
	else
		calc1[0] = (GSh/(float)screenxy[0].vz);

	if ((float)screenxy[1].vz < (GSh/2))
		calc1[1] = (GSh/(GSh/2));
	else
		calc1[1] = (GSh/(float)screenxy[1].vz);

	if ((float)screenxy[2].vz < (GSh/2))
		calc1[2] = (GSh/(GSh/2));
	else
		calc1[2] = (GSh/(float)screenxy[2].vz);

	// 2nd calculation
	screenxy[0].vx = (float)(GSx + (screenxy[0].vx * calc1[0]));
	screenxy[0].vy = (float)(GSy + (screenxy[0].vy * calc1[0]));
	screenxy[1].vx = (float)(GSx + (screenxy[1].vx * calc1[1]));
	screenxy[1].vy = (float)(GSy + (screenxy[1].vy * calc1[1]));
	screenxy[2].vx = (float)(GSx + (screenxy[2].vx * calc1[2]));
	screenxy[2].vy = (float)(GSy + (screenxy[2].vy * calc1[2]));

	opz = (dqb + (dqa * (GSh/(float)screenxy[2].vz))*4096)*16;

	// limD1
	if (screenxy[0].vx < -1024) screenxy[0].vx = -1024;
	if (screenxy[1].vx < -1024) screenxy[1].vx = -1024;
	if (screenxy[2].vx < -1024) screenxy[2].vx = -1024;
	if (screenxy[0].vx > 1023) screenxy[0].vx = 1023;
	if (screenxy[1].vx > 1023) screenxy[1].vx = 1023;
	if (screenxy[2].vx > 1023) screenxy[2].vx = 1023;

	// limD2
	if (screenxy[0].vy < -1024) screenxy[0].vy = -1024;
	if (screenxy[1].vy < -1024) screenxy[1].vy = -1024;
	if (screenxy[2].vy < -1024) screenxy[2].vy = -1024;
	if (screenxy[0].vy > 1023) screenxy[0].vy = 1023;
	if (screenxy[1].vy > 1023) screenxy[1].vy = 1023;
	if (screenxy[2].vy > 1023) screenxy[2].vy = 1023;

	// limA1S
	if (screenxy[2].vx < -32768) screenxy[2].vx = -32768;
	if (screenxy[2].vy < -32768) screenxy[2].vy = -32768;
	if (screenxy[2].vz < -32768) screenxy[2].vz = -32768;
	if (screenxy[2].vx > 32767) screenxy[2].vx = 32767;
	if (screenxy[2].vy > 32767) screenxy[2].vy = 32767;
	if (screenxy[2].vz > 32767) screenxy[2].vz = 32767;
}
#endif

	
// Same with no nop
void gte_rtpt_b(void)
{
	gte_rtpt();
}

// Load DQA register with integer???
void gte_SetLDDQA(int i)
{
	dqa = i;
}

// Load DQB register
void gte_SetLDDQB(int i)
{
	dqb = i;
}

// Load DQA register (again)
void gte_lddqa(int i)
{
	dqa = i;
}

// Load DQB register (again)
void gte_lddqb(int i)
{
	dqb = i;
}


// *ASL* 18/07/2000
/* ---------------------------------------------------------
   Function : gte_intpl
   Purpose : PSX gte emulation interpolate
   Parameters : internal
   Returns : 
   Info : change '#if 1' below to '#if 0' to use original PSX version
*/

// Interpolate
#if 1
// ** Hopefully optimised PSX emulation transform
void gte_intpl(void)
{
	register long	ldc = dc, ndc = 4096 - dc;
	register int	n, shift = 12;
	register long	*fp = &farc.vx, *lp = &lv.vx;
	register short	*sp = &sv.vx;

	n  = *sp++ * ndc;
	n += *fp++ * dc;
	n >>= shift;
	*lp++ = n;

	n  = *sp++ * ndc;
	n += *fp++ * dc;
	n >>= shift;
	*lp++ = n;

	n  = *sp * ndc;
	n += *fp * dc;
	n >>= shift;
	*lp = n;
}

#else

void gte_intpl(void)
{
	lv.vx = (((4096 - dc) * sv.vx) + (dc * farc.vx)) >> 12;
	lv.vy = (((4096 - dc) * sv.vy) + (dc * farc.vy)) >> 12;
	lv.vz = (((4096 - dc) * sv.vz) + (dc * farc.vz)) >> 12;
}
#endif


// Kernel of DpqColor
void gte_dpcs(void)
{
	cr.r = (((4096 - dc) * cr.r) + (dc * farc.vx)) >> 12;
	cr.g = (((4096 - dc) * cr.g) + (dc * farc.vy)) >> 12;
	cr.b = (((4096 - dc) * cr.b) + (dc * farc.vz)) >> 12;
}

// Kernel of DpqColor3
void gte_dpct(void)
{
	cr0.r = (((4096 - dc) * cr0.r) + (dc * farc.vx)) >> 12;
	cr0.g = (((4096 - dc) * cr0.g) + (dc * farc.vy)) >> 12;
	cr0.b = (((4096 - dc) * cr0.b) + (dc * farc.vz)) >> 12;

	cr1.r = (((4096 - dc) * cr1.r) + (dc * farc.vx)) >> 12;
	cr1.g = (((4096 - dc) * cr1.g) + (dc * farc.vy)) >> 12;
	cr1.b = (((4096 - dc) * cr1.b) + (dc * farc.vz)) >> 12;

	cr2.r = (((4096 - dc) * cr2.r) + (dc * farc.vx)) >> 12;
	cr2.g = (((4096 - dc) * cr2.g) + (dc * farc.vy)) >> 12;
	cr2.b = (((4096 - dc) * cr2.b) + (dc * farc.vz)) >> 12;
}

// Kernel of NormalColorCol
void gte_nccs(void)
{
	register SVECTOR tempcr;
	register short outcr[3];

	// LightMatrix * Vertex Register 0
	sv.vx = (((ls0[0] * vr0.vx) + (ls0[1] * vr0.vy) 
		+ (ls0[2] * vr0.vz)) >> 12);
	sv.vy = (((ls1[0] * vr0.vx) + (ls1[1] * vr0.vy) 
		+ (ls1[2] * vr0.vz)) >> 12);
	sv.vz = (((ls2[0] * vr0.vx) + (ls2[1] * vr0.vy) 
		+ (ls2[2] * vr0.vz)) >> 12);

	//Limit number
	if (sv.vx < 0)
		sv.vx = 0;
	if (sv.vy < 0)
		sv.vy = 0;
	if (sv.vz < 0)
		sv.vz = 0;

	if (sv.vx > 32767)
		sv.vx = 32767;
	if (sv.vy > 32767)
		sv.vy = 32767;
	if (sv.vz > 32767)
		sv.vz = 32767;

	// (Short Vector * Colour Registers) + Background Colour
	tempcr.vx = ((((sv.vx * lcr0[0]) + (sv.vy * lcr1[0]) + (sv.vz * lcr2[0])) >> 12) + cbk[0]);
	tempcr.vy = ((((sv.vx * lcr0[1]) + (sv.vy * lcr1[1]) + (sv.vz * lcr2[1])) >> 12) + cbk[1]);
	tempcr.vz = ((((sv.vx * lcr0[2]) + (sv.vy * lcr1[2]) + (sv.vz * lcr2[2])) >> 12) + cbk[2]);

	//Limit number
	if (tempcr.vx < 0)
		tempcr.vx = 0;
	if (tempcr.vy < 0)
		tempcr.vy = 0;
	if (tempcr.vz < 0)
		tempcr.vz = 0;

	if (tempcr.vx > 32767)
		tempcr.vx = 32767;
	if (tempcr.vy > 32767)
		tempcr.vy = 32767;
	if (tempcr.vz > 32767)
		tempcr.vz = 32767;

	outcr[0] = (cr.r * tempcr.vx)/255;
	outcr[1] = (cr.g * tempcr.vy)/255;
	outcr[2] = (cr.b * tempcr.vz)/255;

	//Limit number
	if (outcr[0] < 0)
		outcr[0] = 0;
	if (outcr[1] < 0)
		outcr[1] = 0;
	if (outcr[2] < 0)
		outcr[2] = 0;

	if (outcr[0] > 255)
		outcr[0] = 255;
	if (outcr[1] > 255)
		outcr[1] = 255;
	if (outcr[2] > 255)
		outcr[2] = 255;

	cr.r = outcr[0];
	cr.g = outcr[1];
	cr.b = outcr[2];
}

// Kernel of NormalColorCol3
void gte_ncct(void)
{
	SVECTOR sv1;
	SVECTOR sv2;
	SVECTOR sv3;
	SVECTOR tempcr0;
	SVECTOR tempcr1;
	SVECTOR tempcr2;

	short outcr0[3];
	short outcr1[3];
	short outcr2[3];

	// Same as above but for all three colour registers
	sv1.vx = (((ls0[0] * vr0.vx) + (ls0[1] * vr0.vy) 
		+ (ls0[2] * vr0.vz)) >> 12);
	sv1.vy = (((ls1[0] * vr0.vx) + (ls1[1] * vr0.vy) 
		+ (ls1[2] * vr0.vz)) >> 12);
	sv1.vz = (((ls2[0] * vr0.vx) + (ls2[1] * vr0.vy) 
		+ (ls2[2] * vr0.vz)) >> 12);

	//Limit number
	if (sv1.vx < 0)
		sv1.vx = 0;
	if (sv1.vy < 0)
		sv1.vy = 0;
	if (sv1.vz < 0)
		sv1.vz = 0;

	if (sv1.vx > 32767)
		sv1.vx = 32767;
	if (sv1.vy > 32767)
		sv1.vy = 32767;
	if (sv1.vz > 32767)
		sv1.vz = 32767;

	sv2.vx = (((ls0[0] * vr1.vx) + (ls0[1] * vr1.vy) 
		+ (ls0[2] * vr1.vz)) >> 12);
	sv2.vy = (((ls1[0] * vr1.vx) + (ls1[1] * vr1.vy) 
		+ (ls1[2] * vr1.vz)) >> 12);
	sv2.vz = (((ls2[0] * vr1.vx) + (ls2[1] * vr1.vy) 
		+ (ls2[2] * vr1.vz)) >> 12);

	//Limit number
	if (sv2.vx < 0)
		sv2.vx = 0;
	if (sv2.vy < 0)
		sv2.vy = 0;
	if (sv2.vz < 0)
		sv2.vz = 0;

	if (sv2.vx > 32767)
		sv2.vx = 32767;
	if (sv2.vy > 32767)
		sv2.vy = 32767;
	if (sv2.vz > 32767)
		sv2.vz = 32767;

	sv3.vx = (((ls0[0] * vr2.vx) + (ls0[1] * vr2.vy) 
		+ (ls0[2] * vr2.vz)) >> 12);
	sv3.vy = (((ls1[0] * vr2.vx) + (ls1[1] * vr2.vy) 
		+ (ls1[2] * vr2.vz)) >> 12);
	sv3.vz = (((ls2[0] * vr2.vx) + (ls2[1] * vr2.vy) 
		+ (ls2[2] * vr2.vz)) >> 12);

	//Limit number
	if (sv3.vx < 0)
		sv3.vx = 0;
	if (sv3.vy < 0)
		sv3.vy = 0;
	if (sv3.vz < 0)
		sv3.vz = 0;

	if (sv3.vx > 32767)
		sv3.vx = 32767;
	if (sv3.vy > 32767)
		sv3.vy = 32767;
	if (sv3.vz > 32767)
		sv3.vz = 32767;

	tempcr0.vx = ((((sv1.vx * lcr0[0]) + (sv1.vy * lcr1[0]) + (sv1.vz * lcr2[0])) >> 12) + cbk[0]);
	tempcr0.vy = ((((sv1.vx * lcr0[1]) + (sv1.vy * lcr1[1]) + (sv1.vz * lcr2[1])) >> 12) + cbk[1]);
	tempcr0.vz = ((((sv1.vx * lcr0[2]) + (sv1.vy * lcr1[2]) + (sv1.vz * lcr2[2])) >> 12) + cbk[2]);

	//Limit number
	if (tempcr0.vx < 0)
		tempcr0.vx = 0;
	if (tempcr0.vy < 0)
		tempcr0.vy = 0;
	if (tempcr0.vz < 0)
		tempcr0.vz = 0;

	if (tempcr0.vx > 32767)
		tempcr0.vx = 32767;
	if (tempcr0.vy > 32767)
		tempcr0.vy = 32767;
	if (tempcr0.vz > 32767)
		tempcr0.vz = 32767;

	tempcr1.vx = ((((sv2.vx * lcr0[0]) + (sv2.vy * lcr1[0]) + (sv2.vz * lcr2[0])) >> 12) + cbk[0]);
	tempcr1.vy = ((((sv2.vx * lcr0[1]) + (sv2.vy * lcr1[1]) + (sv2.vz * lcr2[1])) >> 12) + cbk[1]);
	tempcr1.vz = ((((sv2.vx * lcr0[2]) + (sv2.vy * lcr1[2]) + (sv2.vz * lcr2[2])) >> 12) + cbk[2]);

	//Limit number
	if (tempcr1.vx < 0)
		tempcr1.vx = 0;
	if (tempcr1.vy < 0)
		tempcr1.vy = 0;
	if (tempcr1.vz < 0)
		tempcr1.vz = 0;

	if (tempcr1.vx > 32767)
		tempcr1.vx = 32767;
	if (tempcr1.vy > 32767)
		tempcr1.vy = 32767;
	if (tempcr1.vz > 32767)
		tempcr1.vz = 32767;

	tempcr2.vx = ((((sv3.vx * lcr0[0]) + (sv3.vy * lcr1[0]) + (sv3.vz * lcr2[0])) >> 12) + cbk[0]);
	tempcr2.vy = ((((sv3.vx * lcr0[1]) + (sv3.vy * lcr1[1]) + (sv3.vz * lcr2[1])) >> 12) + cbk[1]);
	tempcr2.vz = ((((sv3.vx * lcr0[2]) + (sv3.vy * lcr1[2]) + (sv3.vz * lcr2[2])) >> 12) + cbk[2]);

	//Limit number
	if (tempcr2.vx < 0)
		tempcr2.vx = 0;
	if (tempcr2.vy < 0)
		tempcr2.vy = 0;
	if (tempcr2.vz < 0)
		tempcr2.vz = 0;

	if (tempcr2.vx > 32767)
		tempcr2.vx = 32767;
	if (tempcr2.vy > 32767)
		tempcr2.vy = 32767;
	if (tempcr2.vz > 32767)
		tempcr2.vz = 32767;

	outcr0[0] = (cr0.r * tempcr0.vx)/255;
	outcr0[1] = (cr0.g * tempcr0.vy)/255;
	outcr0[2] = (cr0.b * tempcr0.vz)/255;

	//Limit number
	if (outcr0[0] < 0)
		outcr0[0] = 0;
	if (outcr0[1] < 0)
		outcr0[1] = 0;
	if (outcr0[2] < 0)
		outcr0[2] = 0;

	if (outcr0[0] > 255)
		outcr0[0] = 255;
	if (outcr0[1] > 255)
		outcr0[1] = 255;
	if (outcr0[2] > 255)
		outcr0[2] = 255;

	outcr1[0] = (cr1.r * tempcr1.vx)/255;
	outcr1[1] = (cr1.g * tempcr1.vy)/255;
	outcr1[2] = (cr1.b * tempcr1.vz)/255;

	//Limit number
	if (outcr1[0] < 0)
		outcr1[0] = 0;
	if (outcr1[1] < 0)
		outcr1[1] = 0;
	if (outcr1[2] < 0)
		outcr1[2] = 0;

	if (outcr1[0] > 255)
		outcr1[0] = 255;
	if (outcr1[1] > 255)
		outcr1[1] = 255;
	if (outcr1[2] > 255)
		outcr1[2] = 255;

	outcr2[0] = (cr2.r * tempcr2.vx)/255;
	outcr2[1] = (cr2.g * tempcr2.vy)/255;
	outcr2[2] = (cr2.b * tempcr2.vz)/255;

	//Limit number
	if (outcr2[0] < 0)
		outcr2[0] = 0;
	if (outcr2[1] < 0)
		outcr2[1] = 0;
	if (outcr2[2] < 0)
		outcr2[2] = 0;

	if (outcr2[0] > 255)
		outcr2[0] = 255;
	if (outcr2[1] > 255)
		outcr2[1] = 255;
	if (outcr2[2] > 255)
		outcr2[2] = 255;

	cr0.r = outcr0[0];
	cr0.g = outcr0[1];
	cr0.b = outcr0[2];

	cr1.r = outcr1[0];
	cr1.g = outcr1[1];
	cr1.b = outcr1[2];

	cr2.r = outcr2[0];
	cr2.g = outcr2[1];
	cr2.b = outcr2[2];
}

// Kernel of NormalColor
void gte_ncs(void)
{
	register SVECTOR tempcr;

	// LightMatrix * Vertex Register 0
	sv.vx = (((ls0[0] * vr0.vx) + (ls0[1] * vr0.vy) 
		+ (ls0[2] * vr0.vz)) >> 12);
	sv.vy = (((ls1[0] * vr0.vx) + (ls1[1] * vr0.vy) 
		+ (ls1[2] * vr0.vz)) >> 12);
	sv.vz = (((ls2[0] * vr0.vx) + (ls2[1] * vr0.vy) 
		+ (ls2[2] * vr0.vz)) >> 12);

	//Limit number
	if (sv.vx < 0)
		sv.vx = 0;
	if (sv.vy < 0)
		sv.vy = 0;
	if (sv.vz < 0)
		sv.vz = 0;

	if (sv.vx > 32767)
		sv.vx = 32767;
	if (sv.vy > 32767)
		sv.vy = 32767;
	if (sv.vz > 32767)
		sv.vz = 32767;

	// (Short Vector * Colour Registers) + Background Colour
	tempcr.vx = ((((sv.vx * lcr0[0]) + (sv.vy * lcr1[0]) + (sv.vz * lcr2[0])) >> 12) + cbk[0]);
	tempcr.vy = ((((sv.vx * lcr0[1]) + (sv.vy * lcr1[1]) + (sv.vz * lcr2[1])) >> 12) + cbk[1]);
	tempcr.vz = ((((sv.vx * lcr0[2]) + (sv.vy * lcr1[2]) + (sv.vz * lcr2[2])) >> 12) + cbk[2]);

	//Limit number
	if (tempcr.vx < 0)
		tempcr.vx = 0;
	if (tempcr.vy < 0)
		tempcr.vy = 0;
	if (tempcr.vz < 0)
		tempcr.vz = 0;

	if (tempcr.vx > 255)
		tempcr.vx = 255;
	if (tempcr.vy > 255)
		tempcr.vy = 255;
	if (tempcr.vz > 255)
		tempcr.vz = 255;

	cr.r = tempcr.vx;
	cr.g = tempcr.vy;
	cr.b = tempcr.vz;
}


// *ASL* 18/08/2000
/* ---------------------------------------------------------
   Function : gte_nct
   Purpose : PSX gte emulation 
   Parameters : 
   Returns : 
   Info : change '#if 1' below to '#if 0' to use original PSX version
*/

// Kernel of NormalColor3
#if 1
// ** Hopefully optimised PSX emulation matrix multiply
void gte_nct(void)
{
	SVECTOR sv1;
	SVECTOR sv2;
	SVECTOR sv3;
	SVECTOR tempcr0;
	SVECTOR tempcr1;
	SVECTOR tempcr2;

	// Same as above but for all three colour registers
	sv1.vx = (((ls0[0] * vr0.vx) + (ls0[1] * vr0.vy) 
		+ (ls0[2] * vr0.vz)) >> 12);
	sv1.vy = (((ls1[0] * vr0.vx) + (ls1[1] * vr0.vy) 
		+ (ls1[2] * vr0.vz)) >> 12);
	sv1.vz = (((ls2[0] * vr0.vx) + (ls2[1] * vr0.vy) 
		+ (ls2[2] * vr0.vz)) >> 12);

	//Limit number
	if (sv1.vx < 0)
		sv1.vx = 0;
	if (sv1.vy < 0)
		sv1.vy = 0;
	if (sv1.vz < 0)
		sv1.vz = 0;

	if (sv1.vx > 32767)
		sv1.vx = 32767;
	if (sv1.vy > 32767)
		sv1.vy = 32767;
	if (sv1.vz > 32767)
		sv1.vz = 32767;

	sv2.vx = (((ls0[0] * vr1.vx) + (ls0[1] * vr1.vy) 
		+ (ls0[2] * vr1.vz)) >> 12);
	sv2.vy = (((ls1[0] * vr1.vx) + (ls1[1] * vr1.vy) 
		+ (ls1[2] * vr1.vz)) >> 12);
	sv2.vz = (((ls2[0] * vr1.vx) + (ls2[1] * vr1.vy) 
		+ (ls2[2] * vr1.vz)) >> 12);

	//Limit number
	if (sv2.vx < 0)
		sv2.vx = 0;
	if (sv2.vy < 0)
		sv2.vy = 0;
	if (sv2.vz < 0)
		sv2.vz = 0;

	if (sv2.vx > 32767)
		sv2.vx = 32767;
	if (sv2.vy > 32767)
		sv2.vy = 32767;
	if (sv2.vz > 32767)
		sv2.vz = 32767;

	sv3.vx = (((ls0[0] * vr2.vx) + (ls0[1] * vr2.vy) 
		+ (ls0[2] * vr2.vz)) >> 12);
	sv3.vy = (((ls1[0] * vr2.vx) + (ls1[1] * vr2.vy) 
		+ (ls1[2] * vr2.vz)) >> 12);
	sv3.vz = (((ls2[0] * vr2.vx) + (ls2[1] * vr2.vy) 
		+ (ls2[2] * vr2.vz)) >> 12);

	//Limit number
	if (sv3.vx < 0)
		sv3.vx = 0;
	if (sv3.vy < 0)
		sv3.vy = 0;
	if (sv3.vz < 0)
		sv3.vz = 0;

	if (sv3.vx > 32767)
		sv3.vx = 32767;
	if (sv3.vy > 32767)
		sv3.vy = 32767;
	if (sv3.vz > 32767)
		sv3.vz = 32767;

	tempcr0.vx = ((((sv1.vx * lcr0[0]) + (sv1.vy * lcr1[0]) + (sv1.vz * lcr2[0])) >> 12) + cbk[0]);
	tempcr0.vy = ((((sv1.vx * lcr0[1]) + (sv1.vy * lcr1[1]) + (sv1.vz * lcr2[1])) >> 12) + cbk[1]);
	tempcr0.vz = ((((sv1.vx * lcr0[2]) + (sv1.vy * lcr1[2]) + (sv1.vz * lcr2[2])) >> 12) + cbk[2]);

	//Limit number
	if (tempcr0.vx < 0)
		tempcr0.vx = 0;
	if (tempcr0.vy < 0)
		tempcr0.vy = 0;
	if (tempcr0.vz < 0)
		tempcr0.vz = 0;

	if (tempcr0.vx > 255)
		tempcr0.vx = 255;
	if (tempcr0.vy > 255)
		tempcr0.vy = 255;
	if (tempcr0.vz > 255)
		tempcr0.vz = 255;

	tempcr1.vx = ((((sv2.vx * lcr0[0]) + (sv2.vy * lcr1[0]) + (sv2.vz * lcr2[0])) >> 12) + cbk[0]);
	tempcr1.vy = ((((sv2.vx * lcr0[1]) + (sv2.vy * lcr1[1]) + (sv2.vz * lcr2[1])) >> 12) + cbk[1]);
	tempcr1.vz = ((((sv2.vx * lcr0[2]) + (sv2.vy * lcr1[2]) + (sv2.vz * lcr2[2])) >> 12) + cbk[2]);

	//Limit number
	if (tempcr1.vx < 0)
		tempcr1.vx = 0;
	if (tempcr1.vy < 0)
		tempcr1.vy = 0;
	if (tempcr1.vz < 0)
		tempcr1.vz = 0;

	if (tempcr1.vx > 255)
		tempcr1.vx = 255;
	if (tempcr1.vy > 255)
		tempcr1.vy = 255;
	if (tempcr1.vz > 255)
		tempcr1.vz = 255;

	tempcr2.vx = ((((sv3.vx * lcr0[0]) + (sv3.vy * lcr1[0]) + (sv3.vz * lcr2[0])) >> 12) + cbk[0]);
	tempcr2.vy = ((((sv3.vx * lcr0[1]) + (sv3.vy * lcr1[1]) + (sv3.vz * lcr2[1])) >> 12) + cbk[1]);
	tempcr2.vz = ((((sv3.vx * lcr0[2]) + (sv3.vy * lcr1[2]) + (sv3.vz * lcr2[2])) >> 12) + cbk[2]);

	//Limit number
	if (tempcr2.vx < 0)
		tempcr2.vx = 0;
	if (tempcr2.vy < 0)
		tempcr2.vy = 0;
	if (tempcr2.vz < 0)
		tempcr2.vz = 0;

	if (tempcr2.vx > 255)
		tempcr2.vx = 255;
	if (tempcr2.vy > 255)
		tempcr2.vy = 255;
	if (tempcr2.vz > 255)
		tempcr2.vz = 255;

	cr0.r = tempcr0.vx;
	cr0.g = tempcr0.vy;
	cr0.b = tempcr0.vz;

	cr1.r = tempcr1.vx;
	cr1.g = tempcr1.vy;
	cr1.b = tempcr1.vz;

	cr2.r = tempcr2.vx;
	cr2.g = tempcr2.vy;
	cr2.b = tempcr2.vz;
}

#else

void gte_nct(void)
{
	SVECTOR sv1;
	SVECTOR sv2;
	SVECTOR sv3;
	SVECTOR tempcr0;
	SVECTOR tempcr1;
	SVECTOR tempcr2;

	// Same as above but for all three colour registers
	sv1.vx = (((ls0[0] * vr0.vx) + (ls0[1] * vr0.vy) 
		+ (ls0[2] * vr0.vz)) >> 12);
	sv1.vy = (((ls1[0] * vr0.vx) + (ls1[1] * vr0.vy) 
		+ (ls1[2] * vr0.vz)) >> 12);
	sv1.vz = (((ls2[0] * vr0.vx) + (ls2[1] * vr0.vy) 
		+ (ls2[2] * vr0.vz)) >> 12);

	//Limit number
	if (sv1.vx < 0)
		sv1.vx = 0;
	if (sv1.vy < 0)
		sv1.vy = 0;
	if (sv1.vz < 0)
		sv1.vz = 0;

	if (sv1.vx > 32767)
		sv1.vx = 32767;
	if (sv1.vy > 32767)
		sv1.vy = 32767;
	if (sv1.vz > 32767)
		sv1.vz = 32767;

	sv2.vx = (((ls0[0] * vr1.vx) + (ls0[1] * vr1.vy) 
		+ (ls0[2] * vr1.vz)) >> 12);
	sv2.vy = (((ls1[0] * vr1.vx) + (ls1[1] * vr1.vy) 
		+ (ls1[2] * vr1.vz)) >> 12);
	sv2.vz = (((ls2[0] * vr1.vx) + (ls2[1] * vr1.vy) 
		+ (ls2[2] * vr1.vz)) >> 12);

	//Limit number
	if (sv2.vx < 0)
		sv2.vx = 0;
	if (sv2.vy < 0)
		sv2.vy = 0;
	if (sv2.vz < 0)
		sv2.vz = 0;

	if (sv2.vx > 32767)
		sv2.vx = 32767;
	if (sv2.vy > 32767)
		sv2.vy = 32767;
	if (sv2.vz > 32767)
		sv2.vz = 32767;

	sv3.vx = (((ls0[0] * vr2.vx) + (ls0[1] * vr2.vy) 
		+ (ls0[2] * vr2.vz)) >> 12);
	sv3.vy = (((ls1[0] * vr2.vx) + (ls1[1] * vr2.vy) 
		+ (ls1[2] * vr2.vz)) >> 12);
	sv3.vz = (((ls2[0] * vr2.vx) + (ls2[1] * vr2.vy) 
		+ (ls2[2] * vr2.vz)) >> 12);

	//Limit number
	if (sv3.vx < 0)
		sv3.vx = 0;
	if (sv3.vy < 0)
		sv3.vy = 0;
	if (sv3.vz < 0)
		sv3.vz = 0;

	if (sv3.vx > 32767)
		sv3.vx = 32767;
	if (sv3.vy > 32767)
		sv3.vy = 32767;
	if (sv3.vz > 32767)
		sv3.vz = 32767;

	tempcr0.vx = ((((sv1.vx * lcr0[0]) + (sv1.vy * lcr1[0]) + (sv1.vz * lcr2[0])) >> 12) + cbk[0]);
	tempcr0.vy = ((((sv1.vx * lcr0[1]) + (sv1.vy * lcr1[1]) + (sv1.vz * lcr2[1])) >> 12) + cbk[1]);
	tempcr0.vz = ((((sv1.vx * lcr0[2]) + (sv1.vy * lcr1[2]) + (sv1.vz * lcr2[2])) >> 12) + cbk[2]);

	//Limit number
	if (tempcr0.vx < 0)
		tempcr0.vx = 0;
	if (tempcr0.vy < 0)
		tempcr0.vy = 0;
	if (tempcr0.vz < 0)
		tempcr0.vz = 0;

	if (tempcr0.vx > 255)
		tempcr0.vx = 255;
	if (tempcr0.vy > 255)
		tempcr0.vy = 255;
	if (tempcr0.vz > 255)
		tempcr0.vz = 255;

	tempcr1.vx = ((((sv2.vx * lcr0[0]) + (sv2.vy * lcr1[0]) + (sv2.vz * lcr2[0])) >> 12) + cbk[0]);
	tempcr1.vy = ((((sv2.vx * lcr0[1]) + (sv2.vy * lcr1[1]) + (sv2.vz * lcr2[1])) >> 12) + cbk[1]);
	tempcr1.vz = ((((sv2.vx * lcr0[2]) + (sv2.vy * lcr1[2]) + (sv2.vz * lcr2[2])) >> 12) + cbk[2]);

	//Limit number
	if (tempcr1.vx < 0)
		tempcr1.vx = 0;
	if (tempcr1.vy < 0)
		tempcr1.vy = 0;
	if (tempcr1.vz < 0)
		tempcr1.vz = 0;

	if (tempcr1.vx > 255)
		tempcr1.vx = 255;
	if (tempcr1.vy > 255)
		tempcr1.vy = 255;
	if (tempcr1.vz > 255)
		tempcr1.vz = 255;

	tempcr2.vx = ((((sv3.vx * lcr0[0]) + (sv3.vy * lcr1[0]) + (sv3.vz * lcr2[0])) >> 12) + cbk[0]);
	tempcr2.vy = ((((sv3.vx * lcr0[1]) + (sv3.vy * lcr1[1]) + (sv3.vz * lcr2[1])) >> 12) + cbk[1]);
	tempcr2.vz = ((((sv3.vx * lcr0[2]) + (sv3.vy * lcr1[2]) + (sv3.vz * lcr2[2])) >> 12) + cbk[2]);

	//Limit number
	if (tempcr2.vx < 0)
		tempcr2.vx = 0;
	if (tempcr2.vy < 0)
		tempcr2.vy = 0;
	if (tempcr2.vz < 0)
		tempcr2.vz = 0;

	if (tempcr2.vx > 255)
		tempcr2.vx = 255;
	if (tempcr2.vy > 255)
		tempcr2.vy = 255;
	if (tempcr2.vz > 255)
		tempcr2.vz = 255;

	cr0.r = tempcr0.vx;
	cr0.g = tempcr0.vy;
	cr0.b = tempcr0.vz;

	cr1.r = tempcr1.vx;
	cr1.g = tempcr1.vy;
	cr1.b = tempcr1.vz;

	cr2.r = tempcr2.vx;
	cr2.g = tempcr2.vy;
	cr2.b = tempcr2.vz;
}
#endif


void gte_nct_b(void)
{
	gte_nct();
}

void gte_ncs_b(void)
{
	gte_ncs();
}


// Extras
void GsSetProjection(int h)
{
	GSh = h;

	// *ASL* 26/07/2000 - Floating point projection values
	fGSh = (float)GSh;
	fGShHalf = fGSh * 0.5f;
	fGShLimit = fGSh / fGShHalf;
}

void SetGeomOffset(int x, int y)
{
	GSx = x;
	GSy = y;

	// *ASL* 26/07/2000 - Floating point offset vales;
	fGSx = (float)GSx;
	fGSy = (float)GSy;
}


void GsSetAmbient(int r, int g, int b)
{
	// Set background colour
	double tempcbk[3];
	tempcbk[0] = (double)r/11.73;
	tempcbk[1] = (double)g/11.73;
	tempcbk[2] = (double)b/11.73;

	cbk[0] = (int)tempcbk[0];
	cbk[1] = (int)tempcbk[1];
	cbk[2] = (int)tempcbk[2];
}

void GsSetLightMatrix(MATRIX *mp)
{

	MATRIX temp;

	temp.m[0][0] = (((mp->m[0][0] * ls0[0]) + 
		(mp->m[0][1] * ls0[1]) + (mp->m[0][2] * ls0[2])) >> 12);

	temp.m[0][1] = (((mp->m[1][0] * ls0[0]) + 
		(mp->m[1][1] * ls0[1]) + (mp->m[1][2] * ls0[2])) >> 12);

	temp.m[0][2] = (((mp->m[2][0] * ls0[0]) + 
		(mp->m[2][1] * ls0[1]) + (mp->m[2][2] * ls0[2])) >> 12);


	temp.m[1][0] = (((mp->m[0][0] * ls1[0]) + 
		(mp->m[0][1] * ls1[1]) + (mp->m[0][2] * ls1[2])) >> 12);

	temp.m[1][1] = (((mp->m[1][0] * ls1[0]) + 
		(mp->m[1][1] * ls1[1]) + (mp->m[1][2] * ls1[2])) >> 12);

	temp.m[1][2] = (((mp->m[2][0] * ls1[0]) + 
		(mp->m[2][1] * ls1[1]) + (mp->m[2][2] * ls1[2])) >> 12);


	temp.m[2][0] = (((mp->m[0][0] * ls2[0]) + 
		(mp->m[0][1] * ls2[1]) + (mp->m[0][2] * ls2[2])) >> 12);

	temp.m[2][1] = (((mp->m[1][0] * ls2[0]) + 
		(mp->m[1][1] * ls2[1]) + (mp->m[1][2] * ls2[2])) >> 12);

	temp.m[2][2] = (((mp->m[2][0] * ls2[0]) + 
		(mp->m[2][1] * ls2[1]) + (mp->m[2][2] * ls2[2])) >> 12);
	
	ls0[0] = temp.m[0][0];
	ls0[1] = temp.m[0][1];
	ls0[2] = temp.m[0][2];

	ls1[0] = temp.m[1][0];
	ls1[1] = temp.m[1][1];
	ls1[2] = temp.m[1][2];

	ls2[0] = temp.m[2][0];
	ls2[1] = temp.m[2][1];
	ls2[2] = temp.m[2][2];
}

void GsSetLightMatrix2(MATRIX *mp)
{
	GsSetLightMatrix(mp);
}

void GsSetLightMode(int mode)
{
	// Mode = 0, 1 or 2
	LMode = mode;
}

void GsSetFlatLight( int lightNum, GsF_LIGHT *light)
{
	double res;
	double lVec[3];

	// Normalise
	
	lVec[0] = (double)light->vx;
	lVec[1] = (double)light->vy;
	lVec[2] = (double)light->vz;

	res = sqrt((lVec[0] * lVec[0]) + (lVec[1] * lVec[1]) + (lVec[2] * lVec[2]));
	lVec[0] /=res;
	lVec[1] /=res;
	lVec[2] /=res;

	lVec[0] *=-4096;
	lVec[1] *=-4096;
	lVec[2] *=-4096;

	light->vx = (short)lVec[0];
	light->vy = (short)lVec[1];
	light->vz = (short)lVec[2];

	switch(lightNum)
	{
	case 0:
		{
			lcr0[0] = light->r;
			lcr0[1] = light->g;
			lcr0[2] = light->b;
		
			ls0[0] = light->vx;
			ls0[1] = light->vy;
			ls0[2] = light->vz;

		} break;
	case 1:
		{
			lcr1[0] = light->r;
			lcr1[1] = light->g;
			lcr1[2] = light->b;
	
			ls1[0] = light->vx;
			ls1[1] = light->vy;
			ls1[2] = light->vz;

		} break;
	case 2:
		{
			lcr2[0] = light->r;
			lcr2[1] = light->g;
			lcr2[2] = light->b;
		
			ls2[0] = light->vx;
			ls2[1] = light->vy;
			ls2[2] = light->vz;
			
		} break;
	}
}

MATRIX *RotMatrixYXZ_gte(SVECTOR *r,MATRIX *m)
{
	// Rotation Matrix is manipulated
	MATRIX m1;
	MATRIX m2;
	MATRIX m3;
	register long tempT[3];

	register float fr[3];

	// Store translation values before they get overwritten
	tempT[0] = m->t[0];
	tempT[1] = m->t[1];
	tempT[2] = m->t[2];

	fr[0] = (float)r->vx;
	fr[1] = (float)r->vy;
	fr[2] = (float)r->vz;

	// Degrees (360) and radians
	fr[0]*=0.0015347;
	fr[1]*=0.0015347;
	fr[2]*=0.0015347;

	m1.m[0][0] = (short)((float)(cos(fr[1]) * 4096));
	m1.m[0][1] = 0;
	m1.m[0][2] = (short)((float)(sin(fr[1]) * 4096));

	m1.m[1][0] = 0;
	m1.m[1][1] = 4096;
	m1.m[1][2] = 0;

	m1.m[2][0] = (short)((float)(-sin(fr[1]) * 4096));
	m1.m[2][1] = 0;
	m1.m[2][2] = (short)((float)(cos(fr[1]) * 4096));

	m2.m[0][0] = 4096;
	m2.m[0][1] = 0;
	m2.m[0][2] = 0;

	m2.m[1][0] = 0;
	m2.m[1][1] = (short)((float)(cos(fr[0]) * 4096));
	m2.m[1][2] = (short)((float)(-sin(fr[0]) * 4096));

	m2.m[2][0] = 0;
	m2.m[2][1] = (short)((float)(sin(fr[0]) * 4096));
	m2.m[2][2] = (short)((float)(cos(fr[0]) * 4096));

	m3.m[0][0] = (short)((float)(cos(fr[2]) * 4096));
	m3.m[0][1] = (short)((float)(-sin(fr[2]) * 4096));
	m3.m[0][2] = 0;

	m3.m[1][0] = (short)((float)(sin(fr[2]) * 4096));
	m3.m[1][1] = (short)((float)(cos(fr[2]) * 4096));
	m3.m[1][2] = 0;

	m3.m[2][0] = 0;
	m3.m[2][1] = 0;
	m3.m[2][2] = 4096;

	gte_MulMatrix0(&m1, &m2, m);
	gte_MulMatrix0(m, &m3, m);

	// Restore translation values
	m->t[0] = tempT[0];
	m->t[1] = tempT[1];
	m->t[2] = tempT[2];

	return m;
}

MATRIX *RotMatrixY(long y,MATRIX *m)
{
	// Rotation Matrix is manipulated
	MATRIX m1;
	MATRIX m2;
	MATRIX m3;
	float fr[3];

	fr[0] = (float)0;
	fr[1] = (float)y;
	fr[2] = (float)0;

	// Degrees (360) and radians
	fr[0]*=0.0015347;
	fr[1]*=0.0015347;
	fr[2]*=0.0015347;

	m1.m[0][0] = (short)((float)(cos(fr[1]) * 4096));
	m1.m[0][1] = 0;
	m1.m[0][2] = (short)((float)(sin(fr[1]) * 4096));

	m1.m[1][0] = 0;
	m1.m[1][1] = 4096;
	m1.m[1][2] = 0;

	m1.m[2][0] = (short)((float)(-sin(fr[1]) * 4096));
	m1.m[2][1] = 0;
	m1.m[2][2] = (short)((float)(cos(fr[1]) * 4096));

	m2.m[0][0] = 4096;
	m2.m[0][1] = 0;
	m2.m[0][2] = 0;

	m2.m[1][0] = 0;
	m2.m[1][1] = (short)((float)(cos(fr[0]) * 4096));
	m2.m[1][2] = (short)((float)(-sin(fr[0]) * 4096));

	m2.m[2][0] = 0;
	m2.m[2][1] = (short)((float)(sin(fr[0]) * 4096));
	m2.m[2][2] = (short)((float)(cos(fr[0]) * 4096));

	m3.m[0][0] = (short)((float)(cos(fr[2]) * 4096));
	m3.m[0][1] = (short)((float)(-sin(fr[2]) * 4096));
	m3.m[0][2] = 0;

	m3.m[1][0] = (short)((float)(sin(fr[2]) * 4096));
	m3.m[1][1] = (short)((float)(cos(fr[2]) * 4096));
	m3.m[1][2] = 0;
	
	m3.m[2][0] = 0;
	m3.m[2][1] = 0;
	m3.m[2][2] = 4096;

	gte_MulMatrix0(&m1, &m2, m);
	gte_MulMatrix0(m, &m3, m);

	return m;
}


// *ASL* 18/07/2000
/* ---------------------------------------------------------
   Function : ScaleMatrix
   Purpose : PSX gte emulation matrix scale
   Parameters : matrix to scale pointer, scale vector pointer
   Returns : 
   Info : change '#if 1' below to '#if 0' to use original PSX version
*/

#if 1
// ** Hopefully optimised PSX emulation transform
MATRIX *ScaleMatrix(MATRIX *m, VECTOR *v)
{
	if (v == NULL)
	{
		register short	*mp = &m->m[0][0];
		register short	i1 = 4096, i0 = 0;

		*mp++ = i0;
		*mp++ = i1;
		*mp++ = i1;
		*mp++ = i1;
		*mp++ = i0;
		*mp++ = i1;
		*mp++ = i1;
		*mp++ = i1;
		*mp   = i0;
	}
	else
	{
		register long	*vp = &v->vx;
		register short	*mp = &m->m[0][0];
		register long	vn, n, shift = 12;

		// scale matrix x row
		vn = *vp++;				// vx
		n = *mp;
		n *= vn;
		n >>= shift;
		*mp++ = (short)n;
		n = *mp;
		n *= vn;
		n >>= shift;
		*mp++ = (short)n;
		n = *mp;
		n *= vn;
		n >>= shift;
		*mp++ = (short)n;

		// scale matrix y row
		vn = *vp++;				// vy
		n = *mp;
		n *= vn;
		n >>= shift;
		*mp++ = (short)n;
		n = *mp;
		n *= vn;
		n >>= shift;
		*mp++ = (short)n;
		n = *mp;
		n *= vn;
		n >>= shift;
		*mp++ = (short)n;

		// scale matrix z row
		vn = *vp;				// vz
		n = *mp;
		n *= vn;
		n >>= shift;
		*mp++ = (short)n;
		n = *mp;
		n *= vn;
		n >>= shift;
		*mp++ = (short)n;
		n = *mp;
		n *= vn;
		n >>= shift;
		*mp   = (short)n;
	}
	return m;
}

#else

MATRIX *ScaleMatrix(MATRIX *m, VECTOR *v)
{
	if(v == NULL)//PP: if the vector was NULL....
	{
		//PP: return an identity matrix
		m->m[0][0]=4096;
		m->m[0][1]=0;
		m->m[0][2]=0;
		m->m[1][0]=0;
		m->m[1][1]=4096;
		m->m[1][2]=0;
		m->m[2][0]=0;
		m->m[2][1]=0;
		m->m[2][2]=4096;

		return m;
	}
	
	m->m[0][0] = (short)((m->m[0][0] * v->vx) >> 12);
	m->m[0][1] = (short)((m->m[0][1] * v->vy) >> 12);
	m->m[0][2] = (short)((m->m[0][2] * v->vz) >> 12);
	m->m[1][0] = (short)((m->m[1][0] * v->vx) >> 12);
	m->m[1][1] = (short)((m->m[1][1] * v->vy) >> 12);
	m->m[1][2] = (short)((m->m[1][2] * v->vz) >> 12);
	m->m[2][0] = (short)((m->m[2][0] * v->vx) >> 12);
	m->m[2][1] = (short)((m->m[2][1] * v->vy) >> 12);
	m->m[2][2] = (short)((m->m[2][2] * v->vz) >> 12);

	m->t[0] = (short)m->t[0];
	m->t[1] = (short)m->t[1];
	m->t[2] = (short)m->t[2];

	return m;
}
#endif



// *ASL* 18/07/2000
/* ---------------------------------------------------------
   Function : ApplyMatrix
   Purpose : PSX gte emulation matrix vector multiply
   Parameters : matrix pointer, vector in pointer, vector out pointer
   Returns : 
   Info : change '#if 1' below to '#if 0' to use original PSX version
*/

#if 1
// ** Hopefully optimised PSX emulation transform
VECTOR *ApplyMatrix(MATRIX *m, SVECTOR *v0, VECTOR *v1)
{
	register short	*mp = &m->m[0][0];
	register long	*vp = &v1->vx, shift = 12, n;
	register short	vx = v0->vx, vy = v1->vy, vz = v0->vz;

	n  = *mp++ * vx;
	n += *mp++ * vy;
	n += *mp++ * vz;
	n >>= shift;
	*vp++ = n;
	n  = *mp++ * vx;
	n += *mp++ * vy;
	n += *mp++ * vz;
	n >>= shift;
	*vp++ = n;
	n  = *mp++ * vx;
	n += *mp++ * vy;
	n += *mp   * vz;
	n >>= shift;
	*vp   = n;

	return v1;
}

#else

VECTOR *ApplyMatrix(MATRIX *m, SVECTOR *v0, VECTOR *v1)
{
	register short	*mp = &m->m[0][0];
	register long	*vp = &v1->vx, shift = 12, n;
	register short	vx = v0->vx, vy = v1->vy, vz = v0->vz;

	n  = *mp++ * vx;
	n += *mp++ * vy;
	n += *mp++ * vz;
	n >>= shift;
	*vp++ = n;
	n  = *mp++ * vx;
	n += *mp++ * vy;
	n += *mp++ * vz;
	n >>= shift;
	*vp++ = n;
	n  = *mp++ * vx;
	n += *mp++ * vy;
	n += *mp   * vz;
	n >>= shift;
	*vp   = n;

	return v1;
}
#endif


VECTOR *ApplyMatrixLV(MATRIX *m, VECTOR *v0, VECTOR *v1)
{
	VECTOR v;
	v = *v0;
	// Matrix m * Vector v0
	v1->vx = (((m->m[0][0] * v.vx) + (m->m[0][1] * v.vy) 
		+ (m->m[0][2] * v.vz)) >> 12);
	v1->vy = (((m->m[1][0] * v.vx) + (m->m[1][1] * v.vy) 
		+ (m->m[1][2] * v.vz)) >> 12);
	v1->vz = (((m->m[2][0] * v.vx) + (m->m[2][1] * v.vy) 
		+ (m->m[2][2] * v.vz)) >> 12);

	return v1;
}


VECTOR *ApplyRotMatrix(SVECTOR *v0, VECTOR *v1)
{
	// RotMatrix m * SVector v0
	v1->vx = (((RotMatrix.m[0][0] * v0->vx) + (RotMatrix.m[0][1] * v0->vy) 
		+ (RotMatrix.m[0][2] * v0->vz)) >> 12);
	v1->vy = (((RotMatrix.m[1][0] * v0->vx) + (RotMatrix.m[1][1] * v0->vy) 
		+ (RotMatrix.m[1][2] * v0->vz)) >> 12);
	v1->vz = (((RotMatrix.m[2][0] * v0->vx) + (RotMatrix.m[2][1] * v0->vy) 
		+ (RotMatrix.m[2][2] * v0->vz)) >> 12);

	return v1;
}


// CS: I'm afraid I've spent a few minutes knocking this up - it's not fully tested!
//PP: seems to work OK now - I just construct the matrix the other way round, ie [0][0] -> [2][2] etc
//			This matches the way RotMatrixZYX does it.

MATRIX *RotMatrixZ(long r,MATRIX *m)
{
//ARM:	changed to use look up tables for sin/cos
//		this function has speeded up alot, but feel free to improve on it !
	// Rotation Matrix is manipulated

	//top row
	m->m[0][0]=4096;
	m->m[0][1]=0;
	m->m[0][2]=0;

	//middle row
	m->m[1][0]=0;
	m->m[1][1]=MATHSPC_ACos((short)r);
	m->m[1][2]=-MATHSPC_ASin((short)r);

	//bottom row
	m->m[2][0]=0;
	m->m[2][1]=MATHSPC_ASin((short)r);
	m->m[2][2]=MATHSPC_ACos((short)r);

	return m;
}

SVECTOR *ApplyMatrixSV(MATRIX *m, SVECTOR *v0, SVECTOR *v1)
{
	SVECTOR v;
	v = *v0;
	// Matrix m * SVector v0
	v1->vx = (((m->m[0][0] * v.vx) + (m->m[0][1] * v.vy) 
		+ (m->m[0][2] * v.vz)) >> 12);
	v1->vy = (((m->m[1][0] * v.vx) + (m->m[1][1] * v.vy) 
		+ (m->m[1][2] * v.vz)) >> 12);
	v1->vz = (((m->m[2][0] * v.vx) + (m->m[2][1] * v.vy) 
		+ (m->m[2][2] * v.vz)) >> 12);

	return v1;
}

VECTOR *ApplyRotMatrixLV(VECTOR *v0, VECTOR *v1)
{
	VECTOR v;
	v = *v0;
	// RotMatrix m * SVector v0
	v1->vx = (((RotMatrix.m[0][0] * v.vx) + (RotMatrix.m[0][1] * v.vy) 
		+ (RotMatrix.m[0][2] * v.vz)) >> 12);
	v1->vy = (((RotMatrix.m[1][0] * v.vx) + (RotMatrix.m[1][1] * v.vy) 
		+ (RotMatrix.m[1][2] * v.vz)) >> 12);
	v1->vz = (((RotMatrix.m[2][0] * v.vx) + (RotMatrix.m[2][1] * v.vy) 
		+ (RotMatrix.m[2][2] * v.vz)) >> 12);

	return v1;
}

MATRIX	zmatrix;

int GsSetRefView2(GsRVIEW2 *pv)
{
	float 	xAxis[3], yAxis[3], zAxis[3];
	float 	transCo[3];
	double 	res;
	SVECTOR	vec;
	
	// Default world-up position
	yAxis[0] = 0;
	yAxis[1] = 1;
	yAxis[2] = 0;

	// *rsin(pv->rz/4096)
	
	zAxis[0] = pv->vrx - pv->vpx;
	zAxis[1] = pv->vry - pv->vpy;
	zAxis[2] = pv->vrz - pv->vpz;

	// Normalise zAxis
	res = sqrt((zAxis[0] * zAxis[0]) + (zAxis[1] * zAxis[1]) + (zAxis[2] * zAxis[2]));
	zAxis[0] /= res;
	zAxis[1] /= res;
	zAxis[2] /= res;

	// Cross Product of y and z axis for xAxis
	xAxis[0] = yAxis[1] * zAxis[2] - yAxis[2] * zAxis[1];
	xAxis[1] = yAxis[2] * zAxis[0] - yAxis[0] * zAxis[2];
	xAxis[2] = yAxis[0] * zAxis[1] - yAxis[1] * zAxis[0];

	// Normalise xAxis
	res = sqrt((xAxis[0] * xAxis[0]) + (xAxis[1] * xAxis[1]) + (xAxis[2] * xAxis[2]));
	xAxis[0] /= res;
	xAxis[1] /= res;
	xAxis[2] /= res;

	// Cross Product of z and x axis for yAxis
	yAxis[0] = zAxis[1] * xAxis[2] - zAxis[2] * xAxis[1];
	yAxis[1] = zAxis[2] * xAxis[0] - zAxis[0] * xAxis[2];
	yAxis[2] = zAxis[0] * xAxis[1] - zAxis[1] * xAxis[0];

	// Normalise yAxis
	res = sqrt((yAxis[0] * yAxis[0]) + (yAxis[1] * yAxis[1]) + (yAxis[2] * yAxis[2]));
	yAxis[0] /= res;
	yAxis[1] /= res;
	yAxis[2] /= res;

	xAxis[0] *= 4096;
	xAxis[1] *= 4096;
	xAxis[2] *= 4096;

	yAxis[0] *= 4096;
	yAxis[1] *= 4096;
	yAxis[2] *= 4096;

	zAxis[0] *= 4096;
	zAxis[1] *= 4096;
	zAxis[2] *= 4096;
	
	// Put all values into world matrix

	GsWSMATRIX.m[0][0] = (short)xAxis[0];
	GsWSMATRIX.m[0][1] = (short)xAxis[1];
	GsWSMATRIX.m[0][2] = (short)xAxis[2];
	
	GsWSMATRIX.m[1][0] = (short)yAxis[0];
	GsWSMATRIX.m[1][1] = (short)yAxis[1];
	GsWSMATRIX.m[1][2] = (short)yAxis[2];
	
	GsWSMATRIX.m[2][0] = (short)zAxis[0];
	GsWSMATRIX.m[2][1] = (short)zAxis[1];
	GsWSMATRIX.m[2][2] = (short)zAxis[2];

	GsWSMATRIX.t[0] = -pv->vpx;
	GsWSMATRIX.t[1] = -pv->vpy;
	GsWSMATRIX.t[2] = -pv->vpz;
	
	// Multiply matrix by translation vector
	transCo[0] = (((GsWSMATRIX.m[0][0] * GsWSMATRIX.t[0]) + 
		(GsWSMATRIX.m[0][1] * GsWSMATRIX.t[1]) + (GsWSMATRIX.m[0][2] * GsWSMATRIX.t[2])) >> 12);
	transCo[1] = (((GsWSMATRIX.m[1][0] * GsWSMATRIX.t[0]) + 
		(GsWSMATRIX.m[1][1] * GsWSMATRIX.t[1]) + (GsWSMATRIX.m[1][2] * GsWSMATRIX.t[2])) >> 12);
	transCo[2] = (((GsWSMATRIX.m[2][0] * GsWSMATRIX.t[0]) + 
		(GsWSMATRIX.m[2][1] * GsWSMATRIX.t[1]) + (GsWSMATRIX.m[2][2] * GsWSMATRIX.t[2])) >> 12);

	GsWSMATRIX.t[0] = (short)transCo[0];
	GsWSMATRIX.t[1] = (short)transCo[1];
	GsWSMATRIX.t[2] = (short)transCo[2];

	// rotate the view according to rz
	vec.vx = 0;
	vec.vy = 0;
	vec.vz = -(pv->rz / 360);
	RotMatrixYXZ_gte(&vec,&zmatrix);
	gte_MulMatrix0(&zmatrix,&GsWSMATRIX,&GsWSMATRIX);

	gte_SetRotMatrix(&zmatrix);
	gte_ldlvl(&GsWSMATRIX.t);
	gte_rtir();
	gte_stlvl(&GsWSMATRIX.t);

	return 0;
}

int rsin(int angleIn)
{
	//ARM: changed to use table i created
	return (int)((sin((double)angleIn * 0.001533981)) * 4096);			// NC: Reverted back to this...for now
	/*int		i;
	int		angle=angleIn;

	if(angle<0)
	{
		angle= -(-angle&4095);
	}
	else
	{
		angle&=4095;
	}

	if(!rsin_table_init)
	{
		for(i=0; i<4096; i++)
		{
			rsin_table[i] = (short)(sin((float)i*0.001533981f)*4096);
		}
		rsin_table_init = 1;
	}

	return(rsin_table[angle]);*/
}


int rcos(int angleIn)
{
	//ARM: changed to use table i created
	return (int)((cos((double)angleIn * 0.001533981)) * 4096);		// NC: Reverted back to this.....for now
	/*int		i;
	int		angle=angleIn;

	if(angle<0)
	{
		angle= -(-angle&4095);
	}
	else
	{
		angle&=4095;
	}

	if(!rcos_table_init)
	{
		for(i=0; i<4096; i++)
		{
			rcos_table[i] = (short)(cos((float)i*0.001533981f)*4096);
		}
		rcos_table_init = 1;
	}

	return(rcos_table[angle]);*/
}
void gte_OuterProduct0(VECTOR *v0, VECTOR *v1, VECTOR *v2)
{
	v2->vx = (long)((float)v0->vy * v1->vz - v0->vz * v1->vy);
    v2->vy = (long)((float)v0->vz * v1->vx - v0->vx * v1->vz);
    v2->vz = (long)((float)v0->vx * v1->vy - v0->vy * v1->vx);
}

void gte_ReadGeomScreen(int *gH)
{
	*gH = GSh;
}

int VSync(int i)
{
	// Not sure yet
	return 1;
}

int DrawSync(int i)
{
	// Also not sure
	return 1;
}

ULONG* ClearOTagR(ULONG *i,int j)
{
	return i;
}


void DrawOTag(ULONG *i)
{
}

DISPENV *PutDispEnv(DISPENV *env)
{
	return env;
}

DRAWENV *PutDrawEnv(DRAWENV *env)
{
	return env;
}

typedef struct
{
	short x,y;
}SHORTXY;

KMSTRIPCONTEXT	StripContext_G3;
KMSTRIPHEAD		StripHead_G3;
KMVERTEX_00		vertices_G3[] =
{
{ KM_VERTEXPARAM_NORMAL,     0,0, 1.0f, 0, 0, RGBA(255,255,255,255), 0 },
{ KM_VERTEXPARAM_NORMAL,     0,0, 1.0f, 0, 0, RGBA(255,255,255,255), 0 },
{ KM_VERTEXPARAM_ENDOFSTRIP, 0,0, 1.0f, 0, 0, RGBA(255,255,255,255), 0 },
};

KMSTRIPCONTEXT	StripContext_G4;
KMSTRIPHEAD		StripHead_G4;
KMVERTEX_00		vertices_G4[] =
{
{ KM_VERTEXPARAM_NORMAL,     0,0, 1.0f, 0, 0, RGBA(255,255,255,255), 0 },
{ KM_VERTEXPARAM_NORMAL,     0,0, 1.0f, 0, 0, RGBA(255,255,255,255), 0 },
{ KM_VERTEXPARAM_NORMAL,     0,0, 1.0f, 0, 0, RGBA(255,255,255,255), 0 },
{ KM_VERTEXPARAM_ENDOFSTRIP, 0,0, 1.0f, 0, 0, RGBA(255,255,255,255), 0 }
};

KMSTRIPCONTEXT	StripContext_FT3;
KMSTRIPHEAD		StripHead_FT3;
KMSTRIPCONTEXT	StripContext_FT3_Alpha;
KMSTRIPHEAD		StripHead_FT3_Alpha;
KMVERTEX_03		vertices_FT3[] =
{
{ KM_VERTEXPARAM_NORMAL,     0,0, 1.0f, 0.0f, 1.0f, RGBA(255,255,255,255), 0 },
{ KM_VERTEXPARAM_NORMAL,     0,0, 1.0f, 0.0f, 0.0f, RGBA(255,255,255,255), 0 },
{ KM_VERTEXPARAM_ENDOFSTRIP, 0,0, 1.0f, 1.0f, 0.0f, RGBA(255,255,255,255), 0 }
};

KMSTRIPCONTEXT	StripContext_FT4;
KMSTRIPHEAD		StripHead_FT4;
KMSTRIPCONTEXT	StripContext_FT4_Alpha;
KMSTRIPHEAD		StripHead_FT4_Alpha;
KMVERTEX_03		vertices_FT4[] =
{
{ KM_VERTEXPARAM_NORMAL,     0,0, 1.0f, 0.0f, 1.0f, RGBA(255,255,255,255), 0 },
{ KM_VERTEXPARAM_NORMAL,     0,0, 1.0f, 0.0f, 0.0f, RGBA(255,255,255,255), 0 },
{ KM_VERTEXPARAM_NORMAL,     0,0, 1.0f, 1.0f, 1.0f, RGBA(255,255,255,255), 0 },
{ KM_VERTEXPARAM_ENDOFSTRIP, 0,0, 1.0f, 1.0f, 0.0f, RGBA(255,255,255,255), 0 }
};

KMSTRIPCONTEXT	StripContext_GT3;
KMSTRIPHEAD		StripHead_GT3;
KMSTRIPCONTEXT	StripContext_GT3_Alpha;
KMSTRIPHEAD		StripHead_GT3_Alpha;
KMVERTEX_03		vertices_GT3[] =
{
{ KM_VERTEXPARAM_NORMAL,     0,0, 1.0f, 0.0f, 1.0f, RGBA(255,255,255,255), 0 },
{ KM_VERTEXPARAM_NORMAL,     0,0, 1.0f, 0.0f, 0.0f, RGBA(255,255,255,255), 0 },
{ KM_VERTEXPARAM_ENDOFSTRIP, 0,0, 1.0f, 1.0f, 1.0f, RGBA(255,255,255,255), 0 },
};

KMSTRIPCONTEXT	StripContext_GT4;
KMSTRIPHEAD		StripHead_GT4;
KMSTRIPCONTEXT	StripContext_GT4_Alpha;
KMSTRIPHEAD		StripHead_GT4_Alpha;
KMSTRIPCONTEXT	StripContext_GT4_Add;
KMSTRIPHEAD		StripHead_GT4_Add;
KMVERTEX_03		vertices_GT4[] =
{
{ KM_VERTEXPARAM_NORMAL,     0,0, 1.0f, 0.0f, 1.0f, RGBA(255,255,255,255), 0 },
{ KM_VERTEXPARAM_NORMAL,     0,0, 1.0f, 0.0f, 0.0f, RGBA(255,255,255,255), 0 },
{ KM_VERTEXPARAM_NORMAL,     0,0, 1.0f, 1.0f, 1.0f, RGBA(255,255,255,255), 0 },
{ KM_VERTEXPARAM_ENDOFSTRIP, 0,0, 1.0f, 1.0f, 0.0f, RGBA(255,255,255,255), 0 }
};

// *ASL* 07/08/2000 - Vertex4 strips
KMSTRIPHEAD		StripHead_GT4_FMA_Vertex4;
KMSTRIPHEAD		StripHead_GT4_FMA_Alpha_Vertex4;
KMVERTEX_04		vertices_GT4_FMA_Vertex4[] =
{
{ KM_VERTEXPARAM_NORMAL,     0,0, 1.0f, 0, 0, RGBA(255,255,255,255), 0 },
{ KM_VERTEXPARAM_NORMAL,     0,0, 1.0f, 0, 0, RGBA(255,255,255,255), 0 },
{ KM_VERTEXPARAM_NORMAL,     0,0, 1.0f, 1, 0, RGBA(255,255,255,255), 0 },
{ KM_VERTEXPARAM_ENDOFSTRIP, 0,0, 1.0f, 1, 0, RGBA(255,255,255,255), 0 }
};


KMSTRIPCONTEXT	StripContext_TF4SPR;
KMSTRIPHEAD		StripHead_TF4SPR;
KMSTRIPCONTEXT	StripContext_TF4SPR_Alpha;
KMSTRIPHEAD		StripHead_TF4SPR_Alpha;
KMVERTEX_03		vertices_TF4SPR[] =
{
{ KM_VERTEXPARAM_NORMAL,     0,0, 1.0f, 0.0f, 1.0f, RGBA(255,255,255,255), 0 },
{ KM_VERTEXPARAM_NORMAL,     0,0, 1.0f, 0.0f, 0.0f, RGBA(255,255,255,255), 0 },
{ KM_VERTEXPARAM_NORMAL,     0,0, 1.0f, 1.0f, 1.0f, RGBA(255,255,255,255), 0 },
{ KM_VERTEXPARAM_ENDOFSTRIP, 0,0, 1.0f, 1.0f, 0.0f, RGBA(255,255,255,255), 0 }
};

KMSTRIPCONTEXT	StripContext_GT3_FMA;
KMSTRIPHEAD		StripHead_GT3_FMA;
KMSTRIPCONTEXT	StripContext_GT3_FMA_Alpha;
KMSTRIPHEAD		StripHead_GT3_FMA_Alpha;
KMVERTEX_03		vertices_GT3_FMA[] =
{
{ KM_VERTEXPARAM_NORMAL,     0,0, 1.0f, 0.0f, 1.0f, RGBA(255,255,255,255), 0 },
{ KM_VERTEXPARAM_NORMAL,     0,0, 1.0f, 0.0f, 0.0f, RGBA(255,255,255,255), 0 },
{ KM_VERTEXPARAM_ENDOFSTRIP, 0,0, 1.0f, 1.0f, 1.0f, RGBA(255,255,255,255), 0 },
};

KMSTRIPCONTEXT	StripContext_GT3_FMA_Trans;
KMSTRIPHEAD		StripHead_GT3_FMA_Trans;
KMVERTEX_03		vertices_GT3_FMA_Trans[] =
{
{ KM_VERTEXPARAM_NORMAL,     0,0, 1.0f, 0.0f, 0.0f, RGBA(255,255,255,255), 0 },
{ KM_VERTEXPARAM_NORMAL,     0,0, 1.0f, 1.0f, 1.0f, RGBA(255,255,255,255), 0 },
{ KM_VERTEXPARAM_ENDOFSTRIP, 0,0, 1.0f, 1.0f, 0.0f, RGBA(255,255,255,255), 0 }
};

KMSTRIPCONTEXT	StripContext_GT4_FMA;
KMSTRIPHEAD		StripHead_GT4_FMA;
KMSTRIPCONTEXT	StripContext_GT4_FMA_Alpha;
KMSTRIPHEAD		StripHead_GT4_FMA_Alpha;
KMSTRIPCONTEXT	StripContext_GT4_FMA_Add;
KMSTRIPHEAD		StripHead_GT4_FMA_Add;
KMVERTEX_03		vertices_GT4_FMA[] =
{
{ KM_VERTEXPARAM_NORMAL,     0,0, 1.0f, 0.0f, 1.0f, RGBA(255,255,255,255), 0 },
{ KM_VERTEXPARAM_NORMAL,     0,0, 1.0f, 0.0f, 0.0f, RGBA(255,255,255,255), 0 },
{ KM_VERTEXPARAM_NORMAL,     0,0, 1.0f, 1.0f, 1.0f, RGBA(255,255,255,255), 0 },
{ KM_VERTEXPARAM_ENDOFSTRIP, 0,0, 1.0f, 1.0f, 0.0f, RGBA(255,255,255,255), 0 }
};

KMSTRIPCONTEXT	StripContext_GT4_FMA_Trans;
KMSTRIPHEAD		StripHead_GT4_FMA_Trans;
KMVERTEX_03		vertices_GT4_FMA_Trans[] =
{
{ KM_VERTEXPARAM_NORMAL,     0,0, 1.0f, 0.0f, 1.0f, RGBA(255,255,255,255), 0 },
{ KM_VERTEXPARAM_NORMAL,     0,0, 1.0f, 0.0f, 0.0f, RGBA(255,255,255,255), 0 },
{ KM_VERTEXPARAM_NORMAL,     0,0, 1.0f, 1.0f, 1.0f, RGBA(255,255,255,255), 0 },
{ KM_VERTEXPARAM_ENDOFSTRIP, 0,0, 1.0f, 1.0f, 0.0f, RGBA(255,255,255,255), 0 }
};

KMSTRIPCONTEXT	StripContext_Sprites;
KMSTRIPHEAD		StripHead_Sprites;
KMVERTEX_03		vertices_Sprites[] =
{
{ KM_VERTEXPARAM_NORMAL,     0,0, 1.0f, 0.0f, 1.0f, RGBA(255,255,255,255), 0 },
{ KM_VERTEXPARAM_NORMAL,     0,0, 1.0f, 0.0f, 0.0f, RGBA(255,255,255,255), 0 },
{ KM_VERTEXPARAM_NORMAL,     0,0, 1.0f, 1.0f, 1.0f, RGBA(255,255,255,255), 0 },
{ KM_VERTEXPARAM_ENDOFSTRIP, 0,0, 1.0f, 1.0f, 0.0f, RGBA(255,255,255,255), 0 }
};

KMSTRIPCONTEXT	StripContext_Sprites_Add;
KMSTRIPHEAD		StripHead_Sprites_Add;
KMSTRIPCONTEXT	StripContext_Sprites_Sub;
KMSTRIPHEAD		StripHead_Sprites_Sub;
KMVERTEX_03		vertices_Sprites_Add[] =
{
{ KM_VERTEXPARAM_NORMAL,     0,0, 1.0f, 0.0f, 1.0f, RGBA(255,255,255,255), 0 },
{ KM_VERTEXPARAM_NORMAL,     0,0, 1.0f, 0.0f, 0.0f, RGBA(255,255,255,255), 0 },
{ KM_VERTEXPARAM_NORMAL,     0,0, 1.0f, 1.0f, 1.0f, RGBA(255,255,255,255), 0 },
{ KM_VERTEXPARAM_ENDOFSTRIP, 0,0, 1.0f, 1.0f, 0.0f, RGBA(255,255,255,255), 0 }
};

KMSTRIPCONTEXT	StripContext_SpritesNoTex;
KMSTRIPHEAD		StripHead_SpritesNoTex;
KMSTRIPCONTEXT	StripContext_SpritesNoTex_Sub;
KMSTRIPHEAD		StripHead_SpritesNoTex_Sub;
KMVERTEX_00		vertices_SpritesNoTex[] =
{
{ KM_VERTEXPARAM_NORMAL,     0,0, 1.0f, 0, 0, RGBA(255,255,255,255), 0 },
{ KM_VERTEXPARAM_NORMAL,     0,0, 1.0f, 0, 0, RGBA(255,255,255,255), 0 },
{ KM_VERTEXPARAM_NORMAL,     0,0, 1.0f, 0, 0, RGBA(255,255,255,255), 0 },
{ KM_VERTEXPARAM_ENDOFSTRIP, 0,0, 1.0f, 0, 0, RGBA(255,255,255,255), 0 }
};

KMSTRIPCONTEXT	StripContext_3DSprites;
KMSTRIPHEAD		StripHead_3DSprites;
KMVERTEX_03		vertices_3DSprites[] =
{
{ KM_VERTEXPARAM_NORMAL,     0,0, 1.0f, 0.0f, 1.0f, RGBA(255,255,255,255), 0 },
{ KM_VERTEXPARAM_NORMAL,     0,0, 1.0f, 0.0f, 0.0f, RGBA(255,255,255,255), 0 },
{ KM_VERTEXPARAM_NORMAL,     0,0, 1.0f, 1.0f, 1.0f, RGBA(255,255,255,255), 0 },
{ KM_VERTEXPARAM_ENDOFSTRIP, 0,0, 1.0f, 1.0f, 0.0f, RGBA(255,255,255,255), 0 }
};

KMSTRIPCONTEXT	StripContext_3DBackdrop;
KMSTRIPHEAD		StripHead_3DBackdrop;

int	stripFT3textureID = -1;
int	stripFT3textureID_A = -1;
int	stripFT3dsided = FALSE;
int	stripFT3dsided_A = FALSE;

int	stripFT4textureID = -1;
int	stripFT4textureID_A = -1;
int	stripFT4dsided = FALSE;
int	stripFT4dsided_A = FALSE;

int	stripGT3textureID = -1;
int	stripGT3textureID_A = -1;
int	stripGT3dsided = FALSE;
int	stripGT3dsided_A = FALSE;

int	stripGT4textureID = -1;
int	stripGT4textureID_A = -1;
int	stripGT4dsided = FALSE;
int	stripGT4dsided_A = FALSE;
int	stripGT4textureID_Add = -1;

int	stripGT3FMAtextureID = -1;
int	stripGT3FMAtextureID_A = -1;
int	stripGT3textureID_Add = -1;

int	stripGT4FMAtextureID = -1;
int	stripGT4FMAtextureID_A = -1;
int	stripGT4FMAtextureID_Add = -1;

int	stripTF4SPRtextureID = -1;
int	stripTF4SPRtextureID_A = -1;

int	stripGT3FMATransTextureID = -1;
int	stripGT4FMATransTextureID = -1;

int	strip3DBackdropTextureID = -1;

float	uvLookUpTable4bpp[32];
float	uvLookUpTable8bpp[128];

void initialisePsxStrips()
{
	int	i;

//	TextureType		*DCKtextureList[MAX_TEXTURE_LIST];

	DCKtextureList = syMalloc(MAX_TEXTURE_LIST*sizeof(TextureType));
	
	// setup uv look up tables	
	for(i=0;i<32;i++)
		uvLookUpTable4bpp[i] = (float)i / 31.0;

	for(i=0;i<128;i++)
		uvLookUpTable8bpp[i] = (float)i / 127.0;
		
	// G3 strip
    kmInitStripContext(KM_STRIPCONTEXT_SYS_GOURAUD | KM_OPAQUE_POLYGON, &StripContext_G3);
	memset(&StripContext_G3,0,sizeof(StripContext_G3));
	memset(&StripHead_G3,0,sizeof(StripHead_G3));
	StripContext_G3.nSize = sizeof(KMSTRIPCONTEXT);
    kmInitStripContext(KM_STRIPCONTEXT_SYS_GOURAUD | KM_OPAQUE_POLYGON, &StripContext_G3);
	StripContext_G3.StripControl.nListType		 					= KM_OPAQUE_POLYGON;
	StripContext_G3.StripControl.nUserClipMode	 					= KM_USERCLIP_DISABLE;
	StripContext_G3.StripControl.nShadowMode			 			= KM_NORMAL_POLYGON;
	StripContext_G3.StripControl.bGouraud		 					= KM_TRUE;
	StripContext_G3.ObjectControl.nDepthCompare			 			= KM_GREATER;
	StripContext_G3.ObjectControl.nCullingMode			 			= KM_CULLCCW;
//	StripContext_G3.ObjectControl.nCullingMode			 			= KM_NOCULLING;
	StripContext_G3.ObjectControl.bZWriteDisable					= KM_FALSE;
	StripContext_G3.ImageControl[KM_IMAGE_PARAM1].nSRCBlendingMode	= KM_ONE;	
	StripContext_G3.ImageControl[KM_IMAGE_PARAM1].nDSTBlendingMode	= KM_ZERO;
	StripContext_G3.ImageControl[KM_IMAGE_PARAM1].bSRCSelect		= KM_FALSE;
	StripContext_G3.ImageControl[KM_IMAGE_PARAM1].bDSTSelect		= KM_FALSE;
	StripContext_G3.ImageControl[KM_IMAGE_PARAM1].nFogMode			= KM_NOFOG;	
	StripContext_G3.ImageControl[KM_IMAGE_PARAM1].bColorClamp		= KM_FALSE;
	StripContext_G3.ImageControl[KM_IMAGE_PARAM1].bUseAlpha			= KM_FALSE;
	StripContext_G3.ImageControl[KM_IMAGE_PARAM1].nFilterMode		= KM_BILINEAR;
	kmGenerateStripHead00(&StripHead_G3,&StripContext_G3);

	// G4 strip 
    kmInitStripContext(KM_STRIPCONTEXT_SYS_GOURAUD | KM_OPAQUE_POLYGON, &StripContext_G4);
	memset(&StripContext_G4,0,sizeof(StripContext_G4));
	memset(&StripHead_G4,0,sizeof(StripHead_G4));
	StripContext_G4.nSize = sizeof(KMSTRIPCONTEXT);
    kmInitStripContext(KM_STRIPCONTEXT_SYS_GOURAUD | KM_OPAQUE_POLYGON, &StripContext_G4);
	StripContext_G4.StripControl.nListType		 					= KM_OPAQUE_POLYGON;
	StripContext_G4.StripControl.nUserClipMode	 					= KM_USERCLIP_DISABLE;
	StripContext_G4.StripControl.nShadowMode			 			= KM_NORMAL_POLYGON;
	StripContext_G4.StripControl.bGouraud		 					= KM_TRUE;
	StripContext_G4.ObjectControl.nDepthCompare			 			= KM_ALWAYS;//KM_GREATER;
//	StripContext_G4.ObjectControl.nCullingMode			 			= KM_CULLCCW;
	StripContext_G4.ObjectControl.nCullingMode			 			= KM_NOCULLING;
	StripContext_G4.ObjectControl.bZWriteDisable					= KM_FALSE;
	StripContext_G4.ImageControl[KM_IMAGE_PARAM1].nSRCBlendingMode	= KM_ONE;	
	StripContext_G4.ImageControl[KM_IMAGE_PARAM1].nDSTBlendingMode	= KM_ZERO;
	StripContext_G4.ImageControl[KM_IMAGE_PARAM1].bSRCSelect		= KM_FALSE;
	StripContext_G4.ImageControl[KM_IMAGE_PARAM1].bDSTSelect		= KM_FALSE;
	StripContext_G4.ImageControl[KM_IMAGE_PARAM1].nFogMode			= KM_NOFOG;	
	StripContext_G4.ImageControl[KM_IMAGE_PARAM1].bColorClamp		= KM_FALSE;
	StripContext_G4.ImageControl[KM_IMAGE_PARAM1].bUseAlpha			= KM_FALSE;
	StripContext_G4.ImageControl[KM_IMAGE_PARAM1].nFilterMode		= KM_BILINEAR;
	kmGenerateStripHead00(&StripHead_G4,&StripContext_G4);

	// FT3 strip
    kmInitStripContext(KM_STRIPCONTEXT_SYS_GOURAUD | KM_OPAQUE_POLYGON, &StripContext_FT3);
	memset(&StripContext_FT3,0,sizeof(StripContext_FT3));
	memset(&StripHead_FT3,0,sizeof(StripHead_FT3));
	StripContext_FT3.nSize = sizeof(KMSTRIPCONTEXT);
    kmInitStripContext(KM_STRIPCONTEXT_SYS_GOURAUD | KM_OPAQUE_POLYGON, &StripContext_FT3);
    
	StripContext_FT3.StripControl.nListType		 						= KM_OPAQUE_POLYGON;
	StripContext_FT3.StripControl.nUserClipMode	 						= KM_USERCLIP_DISABLE;
	StripContext_FT3.StripControl.nShadowMode			 				= KM_NORMAL_POLYGON;
	StripContext_FT3.StripControl.bGouraud		 						= KM_TRUE;
	StripContext_FT3.ObjectControl.nDepthCompare				 		= KM_GREATER;
	StripContext_FT3.ObjectControl.nCullingMode			 				= KM_CULLCW;
	StripContext_FT3.ObjectControl.bZWriteDisable						= KM_FALSE;
	StripContext_FT3.ImageControl[KM_IMAGE_PARAM1].nSRCBlendingMode		= KM_ONE;	
	StripContext_FT3.ImageControl[KM_IMAGE_PARAM1].nDSTBlendingMode		= KM_ZERO;
	StripContext_FT3.ImageControl[KM_IMAGE_PARAM1].bSRCSelect			= KM_FALSE;
	StripContext_FT3.ImageControl[KM_IMAGE_PARAM1].bDSTSelect			= KM_FALSE;
	StripContext_FT3.ImageControl[KM_IMAGE_PARAM1].nFogMode				= KM_NOFOG;	
	StripContext_FT3.ImageControl[KM_IMAGE_PARAM1].bColorClamp			= KM_FALSE;
	StripContext_FT3.ImageControl[KM_IMAGE_PARAM1].bUseAlpha			= KM_FALSE;
	StripContext_FT3.ImageControl[KM_IMAGE_PARAM1].bIgnoreTextureAlpha	= KM_FALSE;
	StripContext_FT3.ImageControl[KM_IMAGE_PARAM1].nFilterMode			= KM_BILINEAR;
    StripContext_FT3.ImageControl[KM_IMAGE_PARAM1].pTextureSurfaceDesc 	= &DCKtextureList[0].surface;
	kmGenerateStripHead03(&StripHead_FT3,&StripContext_FT3);

	// FT3 Alpha strip
    kmInitStripContext(KM_STRIPCONTEXT_SYS_GOURAUD | KM_TRANS_POLYGON, &StripContext_FT3_Alpha);
	memset(&StripContext_FT3_Alpha,0,sizeof(StripContext_FT3_Alpha));
	memset(&StripHead_FT3_Alpha,0,sizeof(StripHead_FT3_Alpha));
	StripContext_FT3_Alpha.nSize = sizeof(KMSTRIPCONTEXT);
    kmInitStripContext(KM_STRIPCONTEXT_SYS_GOURAUD | KM_TRANS_POLYGON, &StripContext_FT3_Alpha);
    
	StripContext_FT3_Alpha.StripControl.nListType		 						= KM_TRANS_POLYGON;
	StripContext_FT3_Alpha.StripControl.nUserClipMode	 						= KM_USERCLIP_DISABLE;
	StripContext_FT3_Alpha.StripControl.nShadowMode			 					= KM_NORMAL_POLYGON;
	StripContext_FT3_Alpha.StripControl.bGouraud		 						= KM_TRUE;
	StripContext_FT3_Alpha.ObjectControl.nDepthCompare					 		= KM_GREATER;
	StripContext_FT3_Alpha.ObjectControl.nCullingMode			 				= KM_CULLCW;
	StripContext_FT3_Alpha.ObjectControl.bZWriteDisable							= KM_FALSE;
	StripContext_FT3_Alpha.ImageControl[KM_IMAGE_PARAM1].nSRCBlendingMode		= KM_SRCALPHA;
	StripContext_FT3_Alpha.ImageControl[KM_IMAGE_PARAM1].nDSTBlendingMode		= KM_INVSRCALPHA;	
	StripContext_FT3_Alpha.ImageControl[KM_IMAGE_PARAM1].bSRCSelect				= KM_FALSE;
	StripContext_FT3_Alpha.ImageControl[KM_IMAGE_PARAM1].bDSTSelect				= KM_FALSE;
	StripContext_FT3_Alpha.ImageControl[KM_IMAGE_PARAM1].nFogMode				= KM_NOFOG;	
	StripContext_FT3_Alpha.ImageControl[KM_IMAGE_PARAM1].bColorClamp			= KM_FALSE;
	StripContext_FT3_Alpha.ImageControl[KM_IMAGE_PARAM1].bUseAlpha				= KM_TRUE;
	StripContext_FT3_Alpha.ImageControl[KM_IMAGE_PARAM1].bIgnoreTextureAlpha	= KM_FALSE;
	StripContext_FT3_Alpha.ImageControl[KM_IMAGE_PARAM1].nFilterMode			= KM_BILINEAR;
    StripContext_FT3_Alpha.ImageControl[KM_IMAGE_PARAM1].pTextureSurfaceDesc 	= &DCKtextureList[0].surface;
	kmGenerateStripHead03(&StripHead_FT3_Alpha,&StripContext_FT3_Alpha);

	// FT4 strip
    kmInitStripContext(KM_STRIPCONTEXT_SYS_GOURAUD | KM_OPAQUE_POLYGON, &StripContext_FT4);
	memset(&StripContext_FT4,0,sizeof(StripContext_FT4));
	memset(&StripHead_FT4,0,sizeof(StripHead_FT4));
	StripContext_FT4.nSize = sizeof(KMSTRIPCONTEXT);
    kmInitStripContext(KM_STRIPCONTEXT_SYS_GOURAUD | KM_OPAQUE_POLYGON, &StripContext_FT4);
    
	StripContext_FT4.StripControl.nListType		 						= KM_OPAQUE_POLYGON;
	StripContext_FT4.StripControl.nUserClipMode	 						= KM_USERCLIP_DISABLE;
	StripContext_FT4.StripControl.nShadowMode			 				= KM_NORMAL_POLYGON;
	StripContext_FT4.StripControl.bGouraud		 						= KM_TRUE;
	StripContext_FT4.ObjectControl.nDepthCompare				 		= KM_GREATER;
	StripContext_FT4.ObjectControl.nCullingMode			 				= KM_CULLCW;
	StripContext_FT4.ObjectControl.bZWriteDisable						= KM_FALSE;
	StripContext_FT4.ImageControl[KM_IMAGE_PARAM1].nSRCBlendingMode		= KM_ONE;	
	StripContext_FT4.ImageControl[KM_IMAGE_PARAM1].nDSTBlendingMode		= KM_ZERO;
	StripContext_FT4.ImageControl[KM_IMAGE_PARAM1].bSRCSelect			= KM_FALSE;
	StripContext_FT4.ImageControl[KM_IMAGE_PARAM1].bDSTSelect			= KM_FALSE;
	StripContext_FT4.ImageControl[KM_IMAGE_PARAM1].nFogMode				= KM_NOFOG;	
	StripContext_FT4.ImageControl[KM_IMAGE_PARAM1].bColorClamp			= KM_FALSE;
	StripContext_FT4.ImageControl[KM_IMAGE_PARAM1].bUseAlpha			= KM_FALSE;
	StripContext_FT4.ImageControl[KM_IMAGE_PARAM1].bIgnoreTextureAlpha	= KM_FALSE;
	StripContext_FT4.ImageControl[KM_IMAGE_PARAM1].nFilterMode			= KM_BILINEAR;
    StripContext_FT4.ImageControl[KM_IMAGE_PARAM1].pTextureSurfaceDesc 	= &DCKtextureList[0].surface;
	kmGenerateStripHead03(&StripHead_FT4,&StripContext_FT4);

	// FT4 Alpha strip
    kmInitStripContext(KM_STRIPCONTEXT_SYS_GOURAUD | KM_TRANS_POLYGON, &StripContext_FT4_Alpha);
	memset(&StripContext_FT4_Alpha,0,sizeof(StripContext_FT4_Alpha));
	memset(&StripHead_FT4_Alpha,0,sizeof(StripHead_FT4_Alpha));
	StripContext_FT4_Alpha.nSize = sizeof(KMSTRIPCONTEXT);
    kmInitStripContext(KM_STRIPCONTEXT_SYS_GOURAUD | KM_TRANS_POLYGON, &StripContext_FT4_Alpha);
    
	StripContext_FT4_Alpha.StripControl.nListType		 						= KM_TRANS_POLYGON;
	StripContext_FT4_Alpha.StripControl.nUserClipMode	 						= KM_USERCLIP_DISABLE;
	StripContext_FT4_Alpha.StripControl.nShadowMode			 					= KM_NORMAL_POLYGON;
	StripContext_FT4_Alpha.StripControl.bGouraud		 						= KM_TRUE;
	StripContext_FT4_Alpha.ObjectControl.nDepthCompare					 		= KM_GREATER;
	StripContext_FT4_Alpha.ObjectControl.nCullingMode			 				= KM_CULLCW;
	StripContext_FT4_Alpha.ObjectControl.bZWriteDisable							= KM_FALSE;
	StripContext_FT4_Alpha.ImageControl[KM_IMAGE_PARAM1].nSRCBlendingMode		= KM_SRCALPHA;
	StripContext_FT4_Alpha.ImageControl[KM_IMAGE_PARAM1].nDSTBlendingMode		= KM_INVSRCALPHA;	
	StripContext_FT4_Alpha.ImageControl[KM_IMAGE_PARAM1].bSRCSelect				= KM_FALSE;
	StripContext_FT4_Alpha.ImageControl[KM_IMAGE_PARAM1].bDSTSelect				= KM_FALSE;
	StripContext_FT4_Alpha.ImageControl[KM_IMAGE_PARAM1].nFogMode				= KM_NOFOG;	
	StripContext_FT4_Alpha.ImageControl[KM_IMAGE_PARAM1].bColorClamp			= KM_FALSE;
	StripContext_FT4_Alpha.ImageControl[KM_IMAGE_PARAM1].bUseAlpha				= KM_TRUE;
	StripContext_FT4_Alpha.ImageControl[KM_IMAGE_PARAM1].bIgnoreTextureAlpha	= KM_FALSE;
	StripContext_FT4_Alpha.ImageControl[KM_IMAGE_PARAM1].nFilterMode			= KM_BILINEAR;
    StripContext_FT4_Alpha.ImageControl[KM_IMAGE_PARAM1].pTextureSurfaceDesc 	= &DCKtextureList[0].surface;
	kmGenerateStripHead03(&StripHead_FT4_Alpha,&StripContext_FT4_Alpha);

	// GT3 strip
    kmInitStripContext(KM_STRIPCONTEXT_SYS_GOURAUD | KM_OPAQUE_POLYGON, &StripContext_GT3);
	memset(&StripContext_GT3,0,sizeof(StripContext_GT3));
	memset(&StripHead_GT3,0,sizeof(StripHead_GT3));
	StripContext_GT3.nSize = sizeof(KMSTRIPCONTEXT);
    kmInitStripContext(KM_STRIPCONTEXT_SYS_GOURAUD | KM_OPAQUE_POLYGON, &StripContext_GT3);

	StripContext_GT3.StripControl.nListType		 						= KM_OPAQUE_POLYGON;
	StripContext_GT3.StripControl.nUserClipMode	 						= KM_USERCLIP_DISABLE;
	StripContext_GT3.StripControl.nShadowMode			 				= KM_NORMAL_POLYGON;
	StripContext_GT3.StripControl.bGouraud		 						= KM_TRUE;
	StripContext_GT3.ObjectControl.nDepthCompare			 			= KM_GREATER;
	StripContext_GT3.ObjectControl.nCullingMode			 				= KM_CULLCW;
	StripContext_GT3.ObjectControl.bZWriteDisable						= KM_FALSE;
	StripContext_GT3.ImageControl[KM_IMAGE_PARAM1].nSRCBlendingMode		= KM_ONE;	
	StripContext_GT3.ImageControl[KM_IMAGE_PARAM1].nDSTBlendingMode		= KM_ZERO;
	StripContext_GT3.ImageControl[KM_IMAGE_PARAM1].bSRCSelect			= KM_FALSE;
	StripContext_GT3.ImageControl[KM_IMAGE_PARAM1].bDSTSelect			= KM_FALSE;
	StripContext_GT3.ImageControl[KM_IMAGE_PARAM1].nFogMode				= KM_NOFOG;	
	StripContext_GT3.ImageControl[KM_IMAGE_PARAM1].bColorClamp			= KM_FALSE;
	StripContext_GT3.ImageControl[KM_IMAGE_PARAM1].bUseAlpha			= KM_TRUE;
	StripContext_GT3.ImageControl[KM_IMAGE_PARAM1].bIgnoreTextureAlpha	= KM_FALSE;
	StripContext_GT3.ImageControl[KM_IMAGE_PARAM1].nFilterMode			= KM_BILINEAR;
    StripContext_GT3.ImageControl[KM_IMAGE_PARAM1].pTextureSurfaceDesc 	= &DCKtextureList[0].surface;
//	StripContext_GT3.ImageControl[KM_IMAGE_PARAM1].nClampUV				= TRUE;
	kmGenerateStripHead03(&StripHead_GT3,&StripContext_GT3);	
	
	// GT3 Alpha strip
    kmInitStripContext(KM_STRIPCONTEXT_SYS_GOURAUD | KM_TRANS_POLYGON, &StripContext_GT3_Alpha);
	memset(&StripContext_GT3_Alpha,0,sizeof(StripContext_GT3_Alpha));
	memset(&StripHead_GT3_Alpha,0,sizeof(StripHead_GT3_Alpha));
	StripContext_GT3_Alpha.nSize = sizeof(KMSTRIPCONTEXT);
    kmInitStripContext(KM_STRIPCONTEXT_SYS_GOURAUD | KM_TRANS_POLYGON, &StripContext_GT3_Alpha);

	StripContext_GT3_Alpha.StripControl.nListType		 						= KM_TRANS_POLYGON;
	StripContext_GT3_Alpha.StripControl.nUserClipMode	 						= KM_USERCLIP_DISABLE;
	StripContext_GT3_Alpha.StripControl.nShadowMode			 					= KM_NORMAL_POLYGON;
	StripContext_GT3_Alpha.StripControl.bGouraud		 						= KM_TRUE;
	StripContext_GT3_Alpha.ObjectControl.nDepthCompare			 				= KM_GREATER;
	StripContext_GT3_Alpha.ObjectControl.nCullingMode			 				= KM_CULLCW;
	StripContext_GT3_Alpha.ObjectControl.bZWriteDisable							= KM_FALSE;
	StripContext_GT3_Alpha.ImageControl[KM_IMAGE_PARAM1].nSRCBlendingMode		= KM_SRCALPHA;	
	StripContext_GT3_Alpha.ImageControl[KM_IMAGE_PARAM1].nDSTBlendingMode		= KM_INVSRCALPHA;
	StripContext_GT3_Alpha.ImageControl[KM_IMAGE_PARAM1].bSRCSelect				= KM_FALSE;
	StripContext_GT3_Alpha.ImageControl[KM_IMAGE_PARAM1].bDSTSelect				= KM_FALSE;
	StripContext_GT3_Alpha.ImageControl[KM_IMAGE_PARAM1].nFogMode				= KM_NOFOG;	
	StripContext_GT3_Alpha.ImageControl[KM_IMAGE_PARAM1].bColorClamp			= KM_FALSE;
	StripContext_GT3_Alpha.ImageControl[KM_IMAGE_PARAM1].bUseAlpha				= KM_TRUE;
	StripContext_GT3_Alpha.ImageControl[KM_IMAGE_PARAM1].bIgnoreTextureAlpha	= KM_FALSE;
	StripContext_GT3_Alpha.ImageControl[KM_IMAGE_PARAM1].nFilterMode			= KM_BILINEAR;
    StripContext_GT3_Alpha.ImageControl[KM_IMAGE_PARAM1].pTextureSurfaceDesc 	= &DCKtextureList[0].surface;
//	StripContext_GT3_Alpha.ImageControl[KM_IMAGE_PARAM1].nClampUV				= TRUE;
	kmGenerateStripHead03(&StripHead_GT3_Alpha,&StripContext_GT3_Alpha);	

	// GT4 strip
    kmInitStripContext(KM_STRIPCONTEXT_SYS_GOURAUD | KM_OPAQUE_POLYGON, &StripContext_GT4);
	memset(&StripContext_GT4,0,sizeof(StripContext_GT4));
	memset(&StripHead_GT4,0,sizeof(StripHead_GT4));
	StripContext_GT4.nSize = sizeof(KMSTRIPCONTEXT);
    kmInitStripContext(KM_STRIPCONTEXT_SYS_GOURAUD | KM_OPAQUE_POLYGON, &StripContext_GT4);
    
	StripContext_GT4.StripControl.nListType		 						= KM_OPAQUE_POLYGON;
	StripContext_GT4.StripControl.nUserClipMode	 						= KM_USERCLIP_DISABLE;
	StripContext_GT4.StripControl.nShadowMode			 				= KM_NORMAL_POLYGON;
	StripContext_GT4.StripControl.bGouraud		 						= KM_TRUE;
	StripContext_GT4.ObjectControl.nDepthCompare				 		= KM_GREATER;
	StripContext_GT4.ObjectControl.nCullingMode			 				= KM_CULLCW;
	StripContext_GT4.ObjectControl.bZWriteDisable						= KM_FALSE;
	StripContext_GT4.ImageControl[KM_IMAGE_PARAM1].nSRCBlendingMode		= KM_ONE;	
	StripContext_GT4.ImageControl[KM_IMAGE_PARAM1].nDSTBlendingMode		= KM_ZERO;
	StripContext_GT4.ImageControl[KM_IMAGE_PARAM1].bSRCSelect			= KM_FALSE;
	StripContext_GT4.ImageControl[KM_IMAGE_PARAM1].bDSTSelect			= KM_FALSE;
	StripContext_GT4.ImageControl[KM_IMAGE_PARAM1].nFogMode				= KM_NOFOG;	
	StripContext_GT4.ImageControl[KM_IMAGE_PARAM1].bColorClamp			= KM_FALSE;
	StripContext_GT4.ImageControl[KM_IMAGE_PARAM1].bUseAlpha			= KM_FALSE;
	StripContext_GT4.ImageControl[KM_IMAGE_PARAM1].bIgnoreTextureAlpha	= KM_FALSE;
	StripContext_GT4.ImageControl[KM_IMAGE_PARAM1].nFilterMode			= KM_BILINEAR;
    StripContext_GT4.ImageControl[KM_IMAGE_PARAM1].pTextureSurfaceDesc 	= &DCKtextureList[0].surface;
	kmGenerateStripHead03(&StripHead_GT4,&StripContext_GT4);

	// GT4 strip
    kmInitStripContext(KM_STRIPCONTEXT_SYS_GOURAUD | KM_OPAQUE_POLYGON, &StripContext_GT4_Add);
	memset(&StripContext_GT4_Add,0,sizeof(StripContext_GT4_Add));
	memset(&StripHead_GT4_Add,0,sizeof(StripHead_GT4_Add));
	StripContext_GT4_Add.nSize = sizeof(KMSTRIPCONTEXT);
    kmInitStripContext(KM_STRIPCONTEXT_SYS_GOURAUD | KM_OPAQUE_POLYGON, &StripContext_GT4_Add);
    
	StripContext_GT4_Add.StripControl.nListType		 						= KM_OPAQUE_POLYGON;
	StripContext_GT4_Add.StripControl.nUserClipMode	 						= KM_USERCLIP_DISABLE;
	StripContext_GT4_Add.StripControl.nShadowMode			 				= KM_NORMAL_POLYGON;
	StripContext_GT4_Add.StripControl.bGouraud		 						= KM_TRUE;
	StripContext_GT4_Add.ObjectControl.nDepthCompare			 		= KM_GREATER;
	StripContext_GT4_Add.ObjectControl.nCullingMode			 				= KM_CULLCW;
	StripContext_GT4_Add.ObjectControl.bZWriteDisable						= KM_FALSE;
	StripContext_GT4_Add.ImageControl[KM_IMAGE_PARAM1].nSRCBlendingMode		= KM_ONE;	
	StripContext_GT4_Add.ImageControl[KM_IMAGE_PARAM1].nDSTBlendingMode		= KM_ONE;
	StripContext_GT4_Add.ImageControl[KM_IMAGE_PARAM1].bSRCSelect			= KM_FALSE;
	StripContext_GT4_Add.ImageControl[KM_IMAGE_PARAM1].bDSTSelect			= KM_FALSE;
	StripContext_GT4_Add.ImageControl[KM_IMAGE_PARAM1].nFogMode				= KM_NOFOG;	
	StripContext_GT4_Add.ImageControl[KM_IMAGE_PARAM1].bColorClamp			= KM_FALSE;
	StripContext_GT4_Add.ImageControl[KM_IMAGE_PARAM1].bUseAlpha			= KM_FALSE;
	StripContext_GT4_Add.ImageControl[KM_IMAGE_PARAM1].bIgnoreTextureAlpha	= KM_FALSE;
	StripContext_GT4_Add.ImageControl[KM_IMAGE_PARAM1].nFilterMode			= KM_BILINEAR;
    StripContext_GT4_Add.ImageControl[KM_IMAGE_PARAM1].pTextureSurfaceDesc 	= &DCKtextureList[0].surface;
	kmGenerateStripHead03(&StripHead_GT4_Add,&StripContext_GT4_Add);

	// GT4 Alpha strip
    kmInitStripContext(KM_STRIPCONTEXT_SYS_GOURAUD | KM_TRANS_POLYGON, &StripContext_GT4_Alpha);
	memset(&StripContext_GT4_Alpha,0,sizeof(StripContext_GT4_Alpha));
	memset(&StripHead_GT4_Alpha,0,sizeof(StripHead_GT4_Alpha));
	StripContext_GT4_Alpha.nSize = sizeof(KMSTRIPCONTEXT);
    kmInitStripContext(KM_STRIPCONTEXT_SYS_GOURAUD | KM_TRANS_POLYGON, &StripContext_GT4_Alpha);
    
	StripContext_GT4_Alpha.StripControl.nListType		 						= KM_TRANS_POLYGON;
	StripContext_GT4_Alpha.StripControl.nUserClipMode	 						= KM_USERCLIP_DISABLE;
	StripContext_GT4_Alpha.StripControl.nShadowMode			 					= KM_NORMAL_POLYGON;
	StripContext_GT4_Alpha.StripControl.bGouraud		 						= KM_TRUE;
	StripContext_GT4_Alpha.ObjectControl.nDepthCompare					 		= KM_GREATER;
	StripContext_GT4_Alpha.ObjectControl.nCullingMode			 				= KM_CULLCW;
	StripContext_GT4_Alpha.ObjectControl.bZWriteDisable							= KM_FALSE;
	StripContext_GT4_Alpha.ImageControl[KM_IMAGE_PARAM1].nSRCBlendingMode		= KM_SRCALPHA;
	StripContext_GT4_Alpha.ImageControl[KM_IMAGE_PARAM1].nDSTBlendingMode		= KM_INVSRCALPHA;	
	StripContext_GT4_Alpha.ImageControl[KM_IMAGE_PARAM1].bSRCSelect				= KM_FALSE;
	StripContext_GT4_Alpha.ImageControl[KM_IMAGE_PARAM1].bDSTSelect				= KM_FALSE;
	StripContext_GT4_Alpha.ImageControl[KM_IMAGE_PARAM1].nFogMode				= KM_NOFOG;	
	StripContext_GT4_Alpha.ImageControl[KM_IMAGE_PARAM1].bColorClamp			= KM_FALSE;
	StripContext_GT4_Alpha.ImageControl[KM_IMAGE_PARAM1].bUseAlpha				= KM_TRUE;
	StripContext_GT4_Alpha.ImageControl[KM_IMAGE_PARAM1].bIgnoreTextureAlpha	= KM_FALSE;
	StripContext_GT4_Alpha.ImageControl[KM_IMAGE_PARAM1].nFilterMode			= KM_BILINEAR;
    StripContext_GT4_Alpha.ImageControl[KM_IMAGE_PARAM1].pTextureSurfaceDesc 	= &DCKtextureList[0].surface;
	kmGenerateStripHead03(&StripHead_GT4_Alpha,&StripContext_GT4_Alpha);

	
	// SPRITE DRAWING
	// TF4SPR strip
    kmInitStripContext(KM_STRIPCONTEXT_SYS_GOURAUD | KM_OPAQUE_POLYGON, &StripContext_TF4SPR);
	memset(&StripContext_TF4SPR,0,sizeof(StripContext_TF4SPR));
	memset(&StripHead_TF4SPR,0,sizeof(StripHead_TF4SPR));
	StripContext_TF4SPR.nSize = sizeof(KMSTRIPCONTEXT);
    kmInitStripContext(KM_STRIPCONTEXT_SYS_GOURAUD | KM_OPAQUE_POLYGON, &StripContext_TF4SPR);
    
	StripContext_TF4SPR.StripControl.nListType		 						= KM_OPAQUE_POLYGON;
	StripContext_TF4SPR.StripControl.nUserClipMode	 						= KM_USERCLIP_DISABLE;
	StripContext_TF4SPR.StripControl.nShadowMode			 				= KM_NORMAL_POLYGON;
	StripContext_TF4SPR.StripControl.bGouraud		 						= KM_TRUE;
	StripContext_TF4SPR.ObjectControl.nDepthCompare				 			= KM_GREATER;
	StripContext_TF4SPR.ObjectControl.nCullingMode			 				= KM_NOCULLING;
	StripContext_TF4SPR.ObjectControl.bZWriteDisable						= KM_FALSE;
	StripContext_TF4SPR.ImageControl[KM_IMAGE_PARAM1].nSRCBlendingMode		= KM_ONE;	
	StripContext_TF4SPR.ImageControl[KM_IMAGE_PARAM1].nDSTBlendingMode		= KM_ZERO;
	StripContext_TF4SPR.ImageControl[KM_IMAGE_PARAM1].bSRCSelect			= KM_FALSE;
	StripContext_TF4SPR.ImageControl[KM_IMAGE_PARAM1].bDSTSelect			= KM_FALSE;
	StripContext_TF4SPR.ImageControl[KM_IMAGE_PARAM1].nFogMode				= KM_NOFOG;	
	StripContext_TF4SPR.ImageControl[KM_IMAGE_PARAM1].bColorClamp			= KM_FALSE;
	StripContext_TF4SPR.ImageControl[KM_IMAGE_PARAM1].bUseAlpha				= KM_FALSE;
	StripContext_TF4SPR.ImageControl[KM_IMAGE_PARAM1].bIgnoreTextureAlpha	= KM_FALSE;
	StripContext_TF4SPR.ImageControl[KM_IMAGE_PARAM1].nFilterMode			= KM_BILINEAR;
    StripContext_TF4SPR.ImageControl[KM_IMAGE_PARAM1].pTextureSurfaceDesc 	= &DCKtextureList[0].surface;
	kmGenerateStripHead03(&StripHead_TF4SPR,&StripContext_TF4SPR);

	// TF4SPR Alpha strip
    kmInitStripContext(KM_STRIPCONTEXT_SYS_GOURAUD | KM_TRANS_POLYGON, &StripContext_TF4SPR_Alpha);
	memset(&StripContext_TF4SPR_Alpha,0,sizeof(StripContext_TF4SPR_Alpha));
	memset(&StripHead_TF4SPR_Alpha,0,sizeof(StripHead_TF4SPR_Alpha));
	StripContext_TF4SPR_Alpha.nSize = sizeof(KMSTRIPCONTEXT);
    kmInitStripContext(KM_STRIPCONTEXT_SYS_GOURAUD | KM_TRANS_POLYGON, &StripContext_TF4SPR_Alpha);
    
	StripContext_TF4SPR_Alpha.StripControl.nListType		 						= KM_TRANS_POLYGON;
	StripContext_TF4SPR_Alpha.StripControl.nUserClipMode	 						= KM_USERCLIP_DISABLE;
	StripContext_TF4SPR_Alpha.StripControl.nShadowMode			 					= KM_NORMAL_POLYGON;
	StripContext_TF4SPR_Alpha.StripControl.bGouraud		 							= KM_TRUE;
	StripContext_TF4SPR_Alpha.ObjectControl.nDepthCompare					 		= KM_GREATER;
	StripContext_TF4SPR_Alpha.ObjectControl.nCullingMode			 				= KM_NOCULLING;
	StripContext_TF4SPR_Alpha.ObjectControl.bZWriteDisable							= KM_FALSE;
	StripContext_TF4SPR_Alpha.ImageControl[KM_IMAGE_PARAM1].nSRCBlendingMode		= KM_SRCALPHA;
	StripContext_TF4SPR_Alpha.ImageControl[KM_IMAGE_PARAM1].nDSTBlendingMode		= KM_INVSRCALPHA;	
	StripContext_TF4SPR_Alpha.ImageControl[KM_IMAGE_PARAM1].bSRCSelect				= KM_FALSE;
	StripContext_TF4SPR_Alpha.ImageControl[KM_IMAGE_PARAM1].bDSTSelect				= KM_FALSE;
	StripContext_TF4SPR_Alpha.ImageControl[KM_IMAGE_PARAM1].nFogMode				= KM_NOFOG;	
	StripContext_TF4SPR_Alpha.ImageControl[KM_IMAGE_PARAM1].bColorClamp				= KM_FALSE;
	StripContext_TF4SPR_Alpha.ImageControl[KM_IMAGE_PARAM1].bUseAlpha				= KM_TRUE;
	StripContext_TF4SPR_Alpha.ImageControl[KM_IMAGE_PARAM1].bIgnoreTextureAlpha		= KM_FALSE;
	StripContext_TF4SPR_Alpha.ImageControl[KM_IMAGE_PARAM1].nFilterMode				= KM_BILINEAR;
    StripContext_TF4SPR_Alpha.ImageControl[KM_IMAGE_PARAM1].pTextureSurfaceDesc 	= &DCKtextureList[0].surface;
	kmGenerateStripHead03(&StripHead_TF4SPR_Alpha,&StripContext_TF4SPR_Alpha);

	// MAP DRAWING STRIPS

	// GT3 FMA strip
    kmInitStripContext(KM_STRIPCONTEXT_SYS_GOURAUD | KM_OPAQUE_POLYGON, &StripContext_GT3_FMA);
	memset(&StripContext_GT3_FMA,0,sizeof(StripContext_GT3_FMA));
	memset(&StripHead_GT3_FMA,0,sizeof(StripHead_GT3_FMA));
	StripContext_GT3_FMA.nSize = sizeof(KMSTRIPCONTEXT);
    kmInitStripContext(KM_STRIPCONTEXT_SYS_GOURAUD | KM_OPAQUE_POLYGON, &StripContext_GT3_FMA);

	StripContext_GT3_FMA.StripControl.nListType		 						= KM_OPAQUE_POLYGON;
	StripContext_GT3_FMA.StripControl.nUserClipMode	 						= KM_USERCLIP_DISABLE;
	StripContext_GT3_FMA.StripControl.nShadowMode			 				= KM_NORMAL_POLYGON;
	StripContext_GT3_FMA.StripControl.bGouraud		 						= KM_TRUE;
	StripContext_GT3_FMA.ObjectControl.nDepthCompare			 			= KM_GREATER;
	StripContext_GT3_FMA.ObjectControl.nCullingMode			 				= KM_CULLCW;
	StripContext_GT3_FMA.ObjectControl.bZWriteDisable						= KM_FALSE;
	StripContext_GT3_FMA.ImageControl[KM_IMAGE_PARAM1].nSRCBlendingMode		= KM_ONE;	
	StripContext_GT3_FMA.ImageControl[KM_IMAGE_PARAM1].nDSTBlendingMode		= KM_ZERO;
	StripContext_GT3_FMA.ImageControl[KM_IMAGE_PARAM1].bSRCSelect			= KM_FALSE;
	StripContext_GT3_FMA.ImageControl[KM_IMAGE_PARAM1].bDSTSelect			= KM_FALSE;
	StripContext_GT3_FMA.ImageControl[KM_IMAGE_PARAM1].nFogMode				= KM_NOFOG;//KM_FOGTABLE;	
	StripContext_GT3_FMA.ImageControl[KM_IMAGE_PARAM1].bColorClamp			= KM_FALSE;
	StripContext_GT3_FMA.ImageControl[KM_IMAGE_PARAM1].bUseAlpha			= KM_TRUE;
	StripContext_GT3_FMA.ImageControl[KM_IMAGE_PARAM1].bIgnoreTextureAlpha	= KM_FALSE;
	StripContext_GT3_FMA.ImageControl[KM_IMAGE_PARAM1].nFilterMode			= KM_BILINEAR;
    StripContext_GT3_FMA.ImageControl[KM_IMAGE_PARAM1].pTextureSurfaceDesc 	= &DCKtextureList[0].surface;
	StripContext_GT3_FMA.ImageControl[KM_IMAGE_PARAM1].nClampUV				= KM_CLAMP_UV;
	kmGenerateStripHead03(&StripHead_GT3_FMA,&StripContext_GT3_FMA);	
	
	// GT3 FMA Alpha strip
    kmInitStripContext(KM_STRIPCONTEXT_SYS_GOURAUD | KM_TRANS_POLYGON, &StripContext_GT3_FMA_Alpha);
	memset(&StripContext_GT3_FMA_Alpha,0,sizeof(StripContext_GT3_FMA_Alpha));
	memset(&StripHead_GT3_FMA_Alpha,0,sizeof(StripHead_GT3_FMA_Alpha));
	StripContext_GT3_FMA_Alpha.nSize = sizeof(KMSTRIPCONTEXT);
    kmInitStripContext(KM_STRIPCONTEXT_SYS_GOURAUD | KM_TRANS_POLYGON, &StripContext_GT3_FMA_Alpha);

	StripContext_GT3_FMA_Alpha.StripControl.nListType		 						= KM_TRANS_POLYGON;
	StripContext_GT3_FMA_Alpha.StripControl.nUserClipMode	 						= KM_USERCLIP_DISABLE;
	StripContext_GT3_FMA_Alpha.StripControl.nShadowMode			 					= KM_NORMAL_POLYGON;
	StripContext_GT3_FMA_Alpha.StripControl.bGouraud		 						= KM_TRUE;
	StripContext_GT3_FMA_Alpha.ObjectControl.nDepthCompare			 				= KM_LESS;
	StripContext_GT3_FMA_Alpha.ObjectControl.nCullingMode			 				= KM_CULLCW;
	StripContext_GT3_FMA_Alpha.ObjectControl.bZWriteDisable							= KM_FALSE;
	StripContext_GT3_FMA_Alpha.ImageControl[KM_IMAGE_PARAM1].nSRCBlendingMode		= KM_SRCALPHA;	
	StripContext_GT3_FMA_Alpha.ImageControl[KM_IMAGE_PARAM1].nDSTBlendingMode		= KM_INVSRCALPHA;
	StripContext_GT3_FMA_Alpha.ImageControl[KM_IMAGE_PARAM1].bSRCSelect				= KM_FALSE;
	StripContext_GT3_FMA_Alpha.ImageControl[KM_IMAGE_PARAM1].bDSTSelect				= KM_FALSE;
	StripContext_GT3_FMA_Alpha.ImageControl[KM_IMAGE_PARAM1].nFogMode				= KM_NOFOG;//KM_FOGTABLE;	
	StripContext_GT3_FMA_Alpha.ImageControl[KM_IMAGE_PARAM1].bColorClamp			= KM_FALSE;
	StripContext_GT3_FMA_Alpha.ImageControl[KM_IMAGE_PARAM1].bUseAlpha				= KM_TRUE;
	StripContext_GT3_FMA_Alpha.ImageControl[KM_IMAGE_PARAM1].bIgnoreTextureAlpha	= KM_FALSE;
	StripContext_GT3_FMA_Alpha.ImageControl[KM_IMAGE_PARAM1].nFilterMode			= KM_BILINEAR;
    StripContext_GT3_FMA_Alpha.ImageControl[KM_IMAGE_PARAM1].pTextureSurfaceDesc 	= &DCKtextureList[0].surface;
	StripContext_GT3_FMA_Alpha.ImageControl[KM_IMAGE_PARAM1].nClampUV				= KM_CLAMP_UV;
	kmGenerateStripHead03(&StripHead_GT3_FMA_Alpha,&StripContext_GT3_FMA_Alpha);	
	
	// GT4 FMA strip
    kmInitStripContext(KM_STRIPCONTEXT_SYS_GOURAUD | KM_OPAQUE_POLYGON, &StripContext_GT4_FMA);
	memset(&StripContext_GT4_FMA,0,sizeof(StripContext_GT4_FMA));
	memset(&StripHead_GT4_FMA,0,sizeof(StripHead_GT4_FMA));
	StripContext_GT4_FMA.nSize = sizeof(KMSTRIPCONTEXT);
    kmInitStripContext(KM_STRIPCONTEXT_SYS_GOURAUD | KM_OPAQUE_POLYGON, &StripContext_GT4_FMA);
    
	StripContext_GT4_FMA.StripControl.nListType		 						= KM_OPAQUE_POLYGON;
	StripContext_GT4_FMA.StripControl.nUserClipMode	 						= KM_USERCLIP_DISABLE;
	StripContext_GT4_FMA.StripControl.nShadowMode			 				= KM_NORMAL_POLYGON;
	StripContext_GT4_FMA.StripControl.bGouraud		 						= KM_TRUE;
	StripContext_GT4_FMA.ObjectControl.nDepthCompare				 		= KM_GREATER;
	StripContext_GT4_FMA.ObjectControl.nCullingMode			 				= KM_CULLCW;
	StripContext_GT4_FMA.ObjectControl.bZWriteDisable						= KM_FALSE;
	StripContext_GT4_FMA.ImageControl[KM_IMAGE_PARAM1].nSRCBlendingMode		= KM_ONE;	
	StripContext_GT4_FMA.ImageControl[KM_IMAGE_PARAM1].nDSTBlendingMode		= KM_ZERO;
	StripContext_GT4_FMA.ImageControl[KM_IMAGE_PARAM1].bSRCSelect			= KM_FALSE;
	StripContext_GT4_FMA.ImageControl[KM_IMAGE_PARAM1].bDSTSelect			= KM_FALSE;
	StripContext_GT4_FMA.ImageControl[KM_IMAGE_PARAM1].nFogMode				= KM_NOFOG;//KM_FOGTABLE;	
	StripContext_GT4_FMA.ImageControl[KM_IMAGE_PARAM1].bColorClamp			= KM_FALSE;
	StripContext_GT4_FMA.ImageControl[KM_IMAGE_PARAM1].bUseAlpha			= KM_FALSE;
	StripContext_GT4_FMA.ImageControl[KM_IMAGE_PARAM1].bIgnoreTextureAlpha	= KM_TRUE;
	StripContext_GT4_FMA.ImageControl[KM_IMAGE_PARAM1].nFilterMode			= KM_BILINEAR;
    StripContext_GT4_FMA.ImageControl[KM_IMAGE_PARAM1].pTextureSurfaceDesc 	= &DCKtextureList[0].surface;
	StripContext_GT4_FMA.ImageControl[KM_IMAGE_PARAM1].nClampUV				= KM_CLAMP_UV;
	kmGenerateStripHead03(&StripHead_GT4_FMA,&StripContext_GT4_FMA);

	// *ASL* 07/08/2000 - GT4 FMA Vertex4 strip
	memset(&StripHead_GT4_FMA_Vertex4, 0, sizeof(StripHead_GT4_FMA_Vertex4));
	kmGenerateStripHead04(&StripHead_GT4_FMA_Vertex4, &StripContext_GT4_FMA);


	// GT4 FMA Alpha strip
    kmInitStripContext(KM_STRIPCONTEXT_SYS_GOURAUD | KM_TRANS_POLYGON, &StripContext_GT4_FMA_Alpha);
	memset(&StripContext_GT4_FMA_Alpha,0,sizeof(StripContext_GT4_FMA_Alpha));
	memset(&StripHead_GT4_FMA_Alpha,0,sizeof(StripHead_GT4_FMA_Alpha));
	StripContext_GT4_FMA_Alpha.nSize = sizeof(KMSTRIPCONTEXT);
    kmInitStripContext(KM_STRIPCONTEXT_SYS_GOURAUD | KM_TRANS_POLYGON, &StripContext_GT4_FMA_Alpha);
    
	StripContext_GT4_FMA_Alpha.StripControl.nListType		 						= KM_TRANS_POLYGON;
	StripContext_GT4_FMA_Alpha.StripControl.nUserClipMode	 						= KM_USERCLIP_DISABLE;
	StripContext_GT4_FMA_Alpha.StripControl.nShadowMode			 					= KM_NORMAL_POLYGON;
	StripContext_GT4_FMA_Alpha.StripControl.bGouraud		 						= KM_TRUE;
	StripContext_GT4_FMA_Alpha.ObjectControl.nDepthCompare					 		= KM_GREATER;
	StripContext_GT4_FMA_Alpha.ObjectControl.nCullingMode			 				= KM_CULLCW;
	StripContext_GT4_FMA_Alpha.ObjectControl.bZWriteDisable							= KM_FALSE;
	StripContext_GT4_FMA_Alpha.ImageControl[KM_IMAGE_PARAM1].nSRCBlendingMode		= KM_SRCALPHA;
	StripContext_GT4_FMA_Alpha.ImageControl[KM_IMAGE_PARAM1].nDSTBlendingMode		= KM_INVSRCALPHA;	
	StripContext_GT4_FMA_Alpha.ImageControl[KM_IMAGE_PARAM1].bSRCSelect				= KM_FALSE;
	StripContext_GT4_FMA_Alpha.ImageControl[KM_IMAGE_PARAM1].bDSTSelect				= KM_FALSE;
	StripContext_GT4_FMA_Alpha.ImageControl[KM_IMAGE_PARAM1].nFogMode				= KM_NOFOG;//KM_FOGTABLE;	
	StripContext_GT4_FMA_Alpha.ImageControl[KM_IMAGE_PARAM1].bColorClamp			= KM_TRUE;
	StripContext_GT4_FMA_Alpha.ImageControl[KM_IMAGE_PARAM1].bUseAlpha				= KM_TRUE;
	StripContext_GT4_FMA_Alpha.ImageControl[KM_IMAGE_PARAM1].bIgnoreTextureAlpha	= KM_FALSE;
	StripContext_GT4_FMA_Alpha.ImageControl[KM_IMAGE_PARAM1].nFilterMode			= KM_BILINEAR;
    StripContext_GT4_FMA_Alpha.ImageControl[KM_IMAGE_PARAM1].pTextureSurfaceDesc 	= &DCKtextureList[0].surface;
	StripContext_GT4_FMA_Alpha.ImageControl[KM_IMAGE_PARAM1].nClampUV				= KM_CLAMP_UV;
	kmGenerateStripHead03(&StripHead_GT4_FMA_Alpha,&StripContext_GT4_FMA_Alpha);

	// *ASL* 07/08/2000 - GT4 FMA Alpha Vertex4 strip
	memset(&StripHead_GT4_FMA_Alpha_Vertex4, 0, sizeof(StripHead_GT4_FMA_Alpha_Vertex4));
	kmGenerateStripHead04(&StripHead_GT4_FMA_Alpha_Vertex4, &StripContext_GT4_FMA_Alpha);


	// GT4 FMA Add strip
    kmInitStripContext(KM_STRIPCONTEXT_SYS_GOURAUD | KM_TRANS_POLYGON, &StripContext_GT4_FMA_Add);
	memset(&StripContext_GT4_FMA_Add,0,sizeof(StripContext_GT4_FMA_Add));
	memset(&StripHead_GT4_FMA_Add,0,sizeof(StripHead_GT4_FMA_Add));
	StripContext_GT4_FMA_Add.nSize = sizeof(KMSTRIPCONTEXT);
    kmInitStripContext(KM_STRIPCONTEXT_SYS_GOURAUD | KM_TRANS_POLYGON, &StripContext_GT4_FMA_Add);
    
	StripContext_GT4_FMA_Add.StripControl.nListType		 						= KM_TRANS_POLYGON;
	StripContext_GT4_FMA_Add.StripControl.nUserClipMode	 						= KM_USERCLIP_DISABLE;
	StripContext_GT4_FMA_Add.StripControl.nShadowMode			 				= KM_NORMAL_POLYGON;
	StripContext_GT4_FMA_Add.StripControl.bGouraud		 						= KM_TRUE;
	StripContext_GT4_FMA_Add.ObjectControl.nDepthCompare				 		= KM_GREATER;
	StripContext_GT4_FMA_Add.ObjectControl.nCullingMode			 				= KM_CULLCW;
	StripContext_GT4_FMA_Add.ObjectControl.bZWriteDisable						= KM_FALSE;
	StripContext_GT4_FMA_Add.ImageControl[KM_IMAGE_PARAM1].nSRCBlendingMode		= KM_ONE;	
	StripContext_GT4_FMA_Add.ImageControl[KM_IMAGE_PARAM1].nDSTBlendingMode		= KM_ONE;
	StripContext_GT4_FMA_Add.ImageControl[KM_IMAGE_PARAM1].bSRCSelect			= KM_FALSE;
	StripContext_GT4_FMA_Add.ImageControl[KM_IMAGE_PARAM1].bDSTSelect			= KM_FALSE;
	StripContext_GT4_FMA_Add.ImageControl[KM_IMAGE_PARAM1].nFogMode				= KM_NOFOG;//KM_FOGTABLE;	
	StripContext_GT4_FMA_Add.ImageControl[KM_IMAGE_PARAM1].bColorClamp			= KM_FALSE;
	StripContext_GT4_FMA_Add.ImageControl[KM_IMAGE_PARAM1].bUseAlpha			= KM_FALSE;
	StripContext_GT4_FMA_Add.ImageControl[KM_IMAGE_PARAM1].bIgnoreTextureAlpha	= KM_TRUE;
	StripContext_GT4_FMA_Add.ImageControl[KM_IMAGE_PARAM1].nFilterMode			= KM_BILINEAR;
    StripContext_GT4_FMA_Add.ImageControl[KM_IMAGE_PARAM1].pTextureSurfaceDesc 	= &DCKtextureList[0].surface;
//	StripContext_GT4_FMA_Add.ImageControl[KM_IMAGE_PARAM1].nClampUV				= KM_CLAMP_UV;
	kmGenerateStripHead03(&StripHead_GT4_FMA_Add,&StripContext_GT4_FMA_Add);

	// GT3 FMA Trans strip
    kmInitStripContext(KM_STRIPCONTEXT_SYS_GOURAUD | KM_TRANS_POLYGON, &StripContext_GT3_FMA_Trans);
	memset(&StripHead_GT3_FMA_Trans,0,sizeof(StripContext_GT3_FMA_Trans));
	memset(&StripHead_GT3_FMA_Trans,0,sizeof(StripHead_GT3_FMA_Trans));
	StripContext_GT3_FMA_Trans.nSize = sizeof(KMSTRIPCONTEXT);
    kmInitStripContext(KM_STRIPCONTEXT_SYS_GOURAUD | KM_TRANS_POLYGON, &StripContext_GT3_FMA_Trans);
    
	StripContext_GT3_FMA_Trans.StripControl.nListType		 						= KM_TRANS_POLYGON;
	StripContext_GT3_FMA_Trans.StripControl.nUserClipMode	 						= KM_USERCLIP_DISABLE;
	StripContext_GT3_FMA_Trans.StripControl.nShadowMode			 					= KM_NORMAL_POLYGON;
	StripContext_GT3_FMA_Trans.StripControl.bGouraud		 						= KM_TRUE;
	StripContext_GT3_FMA_Trans.ObjectControl.nDepthCompare				 			= KM_GREATER;
	StripContext_GT3_FMA_Trans.ObjectControl.nCullingMode			 				= KM_CULLCW;
	StripContext_GT3_FMA_Trans.ObjectControl.bZWriteDisable							= KM_FALSE;
	StripContext_GT3_FMA_Trans.ImageControl[KM_IMAGE_PARAM1].nSRCBlendingMode		= KM_SRCALPHA;	
	StripContext_GT3_FMA_Trans.ImageControl[KM_IMAGE_PARAM1].nDSTBlendingMode		= KM_INVSRCALPHA;
	StripContext_GT3_FMA_Trans.ImageControl[KM_IMAGE_PARAM1].bSRCSelect				= KM_FALSE;
	StripContext_GT3_FMA_Trans.ImageControl[KM_IMAGE_PARAM1].bDSTSelect				= KM_FALSE;
	StripContext_GT3_FMA_Trans.ImageControl[KM_IMAGE_PARAM1].nFogMode				= KM_NOFOG;	
	StripContext_GT3_FMA_Trans.ImageControl[KM_IMAGE_PARAM1].bColorClamp			= KM_FALSE;
	StripContext_GT3_FMA_Trans.ImageControl[KM_IMAGE_PARAM1].bUseAlpha				= KM_TRUE;
	StripContext_GT3_FMA_Trans.ImageControl[KM_IMAGE_PARAM1].bIgnoreTextureAlpha	= KM_FALSE;
	StripContext_GT3_FMA_Trans.ImageControl[KM_IMAGE_PARAM1].nFilterMode			= KM_BILINEAR;
    StripContext_GT3_FMA_Trans.ImageControl[KM_IMAGE_PARAM1].pTextureSurfaceDesc 	= &DCKtextureList[0].surface;
	kmGenerateStripHead03(&StripHead_GT3_FMA_Trans,&StripContext_GT3_FMA_Trans);
	
	// GT4 FMA Trans strip
    kmInitStripContext(KM_STRIPCONTEXT_SYS_GOURAUD | KM_TRANS_POLYGON, &StripContext_GT4_FMA_Trans);
	memset(&StripHead_GT4_FMA_Trans,0,sizeof(StripContext_GT4_FMA_Trans));
	memset(&StripHead_GT4_FMA_Trans,0,sizeof(StripHead_GT4_FMA_Trans));
	StripContext_GT4_FMA_Trans.nSize = sizeof(KMSTRIPCONTEXT);
    kmInitStripContext(KM_STRIPCONTEXT_SYS_GOURAUD | KM_TRANS_POLYGON, &StripContext_GT4_FMA_Trans);
    
	StripContext_GT4_FMA_Trans.StripControl.nListType		 						= KM_TRANS_POLYGON;
	StripContext_GT4_FMA_Trans.StripControl.nUserClipMode	 						= KM_USERCLIP_DISABLE;
	StripContext_GT4_FMA_Trans.StripControl.nShadowMode			 					= KM_NORMAL_POLYGON;
	StripContext_GT4_FMA_Trans.StripControl.bGouraud		 						= KM_TRUE;
	StripContext_GT4_FMA_Trans.ObjectControl.nDepthCompare				 			= KM_GREATER;
	StripContext_GT4_FMA_Trans.ObjectControl.nCullingMode			 				= KM_CULLCW;
	StripContext_GT4_FMA_Trans.ObjectControl.bZWriteDisable							= KM_FALSE;
	StripContext_GT4_FMA_Trans.ImageControl[KM_IMAGE_PARAM1].nSRCBlendingMode		= KM_SRCALPHA;	
	StripContext_GT4_FMA_Trans.ImageControl[KM_IMAGE_PARAM1].nDSTBlendingMode		= KM_INVSRCALPHA;
	StripContext_GT4_FMA_Trans.ImageControl[KM_IMAGE_PARAM1].bSRCSelect				= KM_FALSE;
	StripContext_GT4_FMA_Trans.ImageControl[KM_IMAGE_PARAM1].bDSTSelect				= KM_FALSE;
	StripContext_GT4_FMA_Trans.ImageControl[KM_IMAGE_PARAM1].nFogMode				= KM_NOFOG;	
	StripContext_GT4_FMA_Trans.ImageControl[KM_IMAGE_PARAM1].bColorClamp			= KM_FALSE;
	StripContext_GT4_FMA_Trans.ImageControl[KM_IMAGE_PARAM1].bUseAlpha				= KM_TRUE;
	StripContext_GT4_FMA_Trans.ImageControl[KM_IMAGE_PARAM1].bIgnoreTextureAlpha	= KM_FALSE;
	StripContext_GT4_FMA_Trans.ImageControl[KM_IMAGE_PARAM1].nFilterMode			= KM_BILINEAR;
    StripContext_GT4_FMA_Trans.ImageControl[KM_IMAGE_PARAM1].pTextureSurfaceDesc 	= &DCKtextureList[0].surface;
	kmGenerateStripHead03(&StripHead_GT4_FMA_Trans,&StripContext_GT4_FMA_Trans);
	
	// initialise sprite strip context and head
	memset(&StripContext_Sprites,0,sizeof(StripContext_Sprites));
	memset(&StripHead_Sprites,0,sizeof(StripHead_Sprites));
	StripContext_Sprites.nSize = sizeof(KMSTRIPCONTEXT);
    kmInitStripContext(KM_STRIPCONTEXT_SYS_GOURAUD | KM_TRANS_POLYGON, &StripContext_Sprites);
	StripContext_Sprites.StripControl.nListType		 						= KM_TRANS_POLYGON;
	StripContext_Sprites.StripControl.nUserClipMode	 						= KM_USERCLIP_DISABLE;
	StripContext_Sprites.StripControl.nShadowMode			 				= KM_NORMAL_POLYGON;
	StripContext_Sprites.StripControl.bGouraud		 						= KM_TRUE;
	StripContext_Sprites.ObjectControl.nDepthCompare			 			= KM_ALWAYS;
	StripContext_Sprites.ObjectControl.nCullingMode			 				= KM_NOCULLING;
	StripContext_Sprites.ObjectControl.bZWriteDisable						= KM_FALSE;
	StripContext_Sprites.ImageControl[KM_IMAGE_PARAM1].nSRCBlendingMode		= KM_SRCALPHA;
	StripContext_Sprites.ImageControl[KM_IMAGE_PARAM1].nDSTBlendingMode		= KM_INVSRCALPHA;
	StripContext_Sprites.ImageControl[KM_IMAGE_PARAM1].bSRCSelect			= KM_FALSE;
	StripContext_Sprites.ImageControl[KM_IMAGE_PARAM1].bDSTSelect			= KM_FALSE;
	StripContext_Sprites.ImageControl[KM_IMAGE_PARAM1].nFogMode				= KM_NOFOG;
	StripContext_Sprites.ImageControl[KM_IMAGE_PARAM1].bColorClamp			= KM_FALSE;	
	StripContext_Sprites.ImageControl[KM_IMAGE_PARAM1].bUseAlpha			= KM_TRUE;
	StripContext_Sprites.ImageControl[KM_IMAGE_PARAM1].bIgnoreTextureAlpha	= KM_FALSE;
	StripContext_Sprites.ImageControl[KM_IMAGE_PARAM1].nFilterMode			= KM_BILINEAR;
    StripContext_Sprites.ImageControl[KM_IMAGE_PARAM1].pTextureSurfaceDesc = &DCKtextureList[0].surface;
	StripContext_Sprites.ImageControl[KM_IMAGE_PARAM1].nClampUV				= KM_CLAMP_UV;
	kmGenerateStripHead03(&StripHead_Sprites,&StripContext_Sprites);

	// initialise additive sprite strip context and head
	memset(&StripContext_Sprites_Add,0,sizeof(StripContext_Sprites_Add));
	memset(&StripHead_Sprites_Add,0,sizeof(StripHead_Sprites_Add));
	StripContext_Sprites_Add.nSize = sizeof(KMSTRIPCONTEXT);
    kmInitStripContext(KM_STRIPCONTEXT_SYS_GOURAUD | KM_TRANS_POLYGON, &StripContext_Sprites_Add);
	StripContext_Sprites_Add.StripControl.nListType		 						= KM_TRANS_POLYGON;
	StripContext_Sprites_Add.StripControl.nUserClipMode	 						= KM_USERCLIP_DISABLE;
	StripContext_Sprites_Add.StripControl.nShadowMode			 				= KM_NORMAL_POLYGON;
	StripContext_Sprites_Add.StripControl.bGouraud		 						= KM_TRUE;
	StripContext_Sprites_Add.ObjectControl.nDepthCompare			 			= KM_GREATER;
	StripContext_Sprites_Add.ObjectControl.nCullingMode			 				= KM_NOCULLING;
	StripContext_Sprites_Add.ObjectControl.bZWriteDisable						= KM_FALSE;
	StripContext_Sprites_Add.ImageControl[KM_IMAGE_PARAM1].nSRCBlendingMode		= KM_ONE;
	StripContext_Sprites_Add.ImageControl[KM_IMAGE_PARAM1].nDSTBlendingMode		= KM_ONE;
	StripContext_Sprites_Add.ImageControl[KM_IMAGE_PARAM1].bSRCSelect			= KM_FALSE;
	StripContext_Sprites_Add.ImageControl[KM_IMAGE_PARAM1].bDSTSelect			= KM_FALSE;
	StripContext_Sprites_Add.ImageControl[KM_IMAGE_PARAM1].nFogMode				= KM_NOFOG;
	StripContext_Sprites_Add.ImageControl[KM_IMAGE_PARAM1].bColorClamp			= KM_FALSE;	
	StripContext_Sprites_Add.ImageControl[KM_IMAGE_PARAM1].bUseAlpha			= KM_TRUE;
	StripContext_Sprites_Add.ImageControl[KM_IMAGE_PARAM1].bIgnoreTextureAlpha	= KM_FALSE;
	StripContext_Sprites_Add.ImageControl[KM_IMAGE_PARAM1].nFilterMode			= KM_BILINEAR;
    StripContext_Sprites_Add.ImageControl[KM_IMAGE_PARAM1].pTextureSurfaceDesc = &DCKtextureList[0].surface;
	StripContext_Sprites_Add.ImageControl[KM_IMAGE_PARAM1].nClampUV				= KM_CLAMP_UV;
	kmGenerateStripHead03(&StripHead_Sprites_Add,&StripContext_Sprites_Add);

	// initialise subtractive sprite strip context and head
	memset(&StripContext_Sprites_Sub,0,sizeof(StripContext_Sprites_Sub));
	memset(&StripHead_Sprites_Sub,0,sizeof(StripHead_Sprites_Sub));
	StripContext_Sprites_Sub.nSize = sizeof(KMSTRIPCONTEXT);
    kmInitStripContext(KM_STRIPCONTEXT_SYS_GOURAUD | KM_TRANS_POLYGON, &StripContext_Sprites_Sub);
	StripContext_Sprites_Sub.StripControl.nListType		 						= KM_TRANS_POLYGON;
	StripContext_Sprites_Sub.StripControl.nUserClipMode	 						= KM_USERCLIP_DISABLE;
	StripContext_Sprites_Sub.StripControl.nShadowMode			 				= KM_NORMAL_POLYGON;
	StripContext_Sprites_Sub.StripControl.bGouraud		 						= KM_TRUE;
	StripContext_Sprites_Sub.ObjectControl.nDepthCompare			 			= KM_GREATER;
	StripContext_Sprites_Sub.ObjectControl.nCullingMode			 				= KM_NOCULLING;
	StripContext_Sprites_Sub.ObjectControl.bZWriteDisable						= KM_FALSE;
	StripContext_Sprites_Sub.ImageControl[KM_IMAGE_PARAM1].nSRCBlendingMode		= KM_ZERO;
	StripContext_Sprites_Sub.ImageControl[KM_IMAGE_PARAM1].nDSTBlendingMode		= KM_INVSRCCOLOR;
	StripContext_Sprites_Sub.ImageControl[KM_IMAGE_PARAM1].bSRCSelect			= KM_FALSE;
	StripContext_Sprites_Sub.ImageControl[KM_IMAGE_PARAM1].bDSTSelect			= KM_FALSE;
	StripContext_Sprites_Sub.ImageControl[KM_IMAGE_PARAM1].nFogMode				= KM_NOFOG;
	StripContext_Sprites_Sub.ImageControl[KM_IMAGE_PARAM1].bColorClamp			= KM_FALSE;	
	StripContext_Sprites_Sub.ImageControl[KM_IMAGE_PARAM1].bUseAlpha			= KM_TRUE;
	StripContext_Sprites_Sub.ImageControl[KM_IMAGE_PARAM1].bIgnoreTextureAlpha	= KM_FALSE;
	StripContext_Sprites_Sub.ImageControl[KM_IMAGE_PARAM1].nFilterMode			= KM_BILINEAR;
    StripContext_Sprites_Sub.ImageControl[KM_IMAGE_PARAM1].pTextureSurfaceDesc = &DCKtextureList[0].surface;
	StripContext_Sprites_Sub.ImageControl[KM_IMAGE_PARAM1].nClampUV				= KM_CLAMP_UV;
	kmGenerateStripHead03(&StripHead_Sprites_Sub,&StripContext_Sprites_Sub);

	// initialise sprite with no texture strip context and head
	memset(&StripContext_SpritesNoTex,0,sizeof(StripContext_SpritesNoTex));
	memset(&StripHead_SpritesNoTex,0,sizeof(StripHead_SpritesNoTex));
	StripContext_SpritesNoTex.nSize = sizeof(KMSTRIPCONTEXT);
    kmInitStripContext(KM_STRIPCONTEXT_SYS_GOURAUD | KM_TRANS_POLYGON, &StripContext_SpritesNoTex);
	StripContext_SpritesNoTex.StripControl.nListType		 					= KM_TRANS_POLYGON;
	StripContext_SpritesNoTex.StripControl.nUserClipMode	 					= KM_USERCLIP_DISABLE;
	StripContext_SpritesNoTex.StripControl.nShadowMode			 				= KM_NORMAL_POLYGON;
	StripContext_SpritesNoTex.StripControl.bGouraud		 						= KM_TRUE;
	StripContext_SpritesNoTex.ObjectControl.nDepthCompare			 			= KM_GREATER;
	StripContext_SpritesNoTex.ObjectControl.nCullingMode			 			= KM_NOCULLING;
	StripContext_SpritesNoTex.ObjectControl.bZWriteDisable						= KM_TRUE;
	StripContext_SpritesNoTex.ImageControl[KM_IMAGE_PARAM1].nSRCBlendingMode	= KM_SRCALPHA;
	StripContext_SpritesNoTex.ImageControl[KM_IMAGE_PARAM1].nDSTBlendingMode	= KM_INVSRCALPHA;	
	StripContext_SpritesNoTex.ImageControl[KM_IMAGE_PARAM1].bSRCSelect			= KM_FALSE;
	StripContext_SpritesNoTex.ImageControl[KM_IMAGE_PARAM1].bDSTSelect			= KM_FALSE;
	StripContext_SpritesNoTex.ImageControl[KM_IMAGE_PARAM1].nFogMode			= KM_NOFOG;
	StripContext_SpritesNoTex.ImageControl[KM_IMAGE_PARAM1].bColorClamp			= KM_FALSE;	
	StripContext_SpritesNoTex.ImageControl[KM_IMAGE_PARAM1].bUseAlpha			= KM_TRUE;
	StripContext_SpritesNoTex.ImageControl[KM_IMAGE_PARAM1].bIgnoreTextureAlpha	= KM_FALSE;
	StripContext_SpritesNoTex.ImageControl[KM_IMAGE_PARAM1].nFilterMode			= KM_BILINEAR;//KM_POINT_SAMPLE;
	StripContext_SpritesNoTex.ImageControl[KM_IMAGE_PARAM1].nClampUV			= KM_CLAMP_UV;
	kmGenerateStripHead00(&StripHead_SpritesNoTex,&StripContext_SpritesNoTex);

	// initialise sprite with no texture strip context and head
	memset(&StripContext_SpritesNoTex_Sub,0,sizeof(StripContext_SpritesNoTex_Sub));
	memset(&StripHead_SpritesNoTex_Sub,0,sizeof(StripHead_SpritesNoTex_Sub));
	StripContext_SpritesNoTex_Sub.nSize = sizeof(KMSTRIPCONTEXT);
    kmInitStripContext(KM_STRIPCONTEXT_SYS_GOURAUD | KM_TRANS_POLYGON, &StripContext_SpritesNoTex_Sub);
	StripContext_SpritesNoTex_Sub.StripControl.nListType		 					= KM_TRANS_POLYGON;
	StripContext_SpritesNoTex_Sub.StripControl.nUserClipMode	 					= KM_USERCLIP_DISABLE;
	StripContext_SpritesNoTex_Sub.StripControl.nShadowMode			 				= KM_NORMAL_POLYGON;
	StripContext_SpritesNoTex_Sub.StripControl.bGouraud		 						= KM_TRUE;
	StripContext_SpritesNoTex_Sub.ObjectControl.nDepthCompare			 			= KM_GREATER;
	StripContext_SpritesNoTex_Sub.ObjectControl.nCullingMode			 			= KM_NOCULLING;
	StripContext_SpritesNoTex_Sub.ObjectControl.bZWriteDisable						= KM_TRUE;
	StripContext_SpritesNoTex_Sub.ImageControl[KM_IMAGE_PARAM1].nSRCBlendingMode	= KM_ZERO;
	StripContext_SpritesNoTex_Sub.ImageControl[KM_IMAGE_PARAM1].nDSTBlendingMode	= KM_INVSRCCOLOR;	
	StripContext_SpritesNoTex_Sub.ImageControl[KM_IMAGE_PARAM1].bSRCSelect			= KM_FALSE;
	StripContext_SpritesNoTex_Sub.ImageControl[KM_IMAGE_PARAM1].bDSTSelect			= KM_FALSE;
	StripContext_SpritesNoTex_Sub.ImageControl[KM_IMAGE_PARAM1].nFogMode			= KM_NOFOG;
	StripContext_SpritesNoTex_Sub.ImageControl[KM_IMAGE_PARAM1].bColorClamp			= KM_FALSE;	
	StripContext_SpritesNoTex_Sub.ImageControl[KM_IMAGE_PARAM1].bUseAlpha			= KM_TRUE;
	StripContext_SpritesNoTex_Sub.ImageControl[KM_IMAGE_PARAM1].bIgnoreTextureAlpha	= KM_FALSE;
	StripContext_SpritesNoTex_Sub.ImageControl[KM_IMAGE_PARAM1].nFilterMode			= KM_BILINEAR;//KM_POINT_SAMPLE;
	StripContext_SpritesNoTex_Sub.ImageControl[KM_IMAGE_PARAM1].nClampUV			= KM_CLAMP_UV;
	kmGenerateStripHead00(&StripHead_SpritesNoTex_Sub,&StripContext_SpritesNoTex_Sub);
	
	// initialise 3d sprite strip context and head
	memset(&StripContext_3DSprites,0,sizeof(StripContext_3DSprites));
	memset(&StripHead_3DSprites,0,sizeof(StripHead_3DSprites));
	StripContext_3DSprites.nSize = sizeof(KMSTRIPCONTEXT);
    kmInitStripContext(KM_STRIPCONTEXT_SYS_GOURAUD | KM_TRANS_POLYGON, &StripContext_3DSprites);
	StripContext_3DSprites.StripControl.nListType		 						= KM_TRANS_POLYGON;
	StripContext_3DSprites.StripControl.nUserClipMode	 						= KM_USERCLIP_DISABLE;
	StripContext_3DSprites.StripControl.nShadowMode			 					= KM_NORMAL_POLYGON;
	StripContext_3DSprites.StripControl.bGouraud		 						= KM_TRUE;
	StripContext_3DSprites.ObjectControl.nDepthCompare			 				= KM_GREATER;
	StripContext_3DSprites.ObjectControl.nCullingMode			 				= KM_NOCULLING;
	StripContext_3DSprites.ObjectControl.bZWriteDisable							= KM_FALSE;
	StripContext_3DSprites.ImageControl[KM_IMAGE_PARAM1].nSRCBlendingMode		= KM_SRCALPHA;
	StripContext_3DSprites.ImageControl[KM_IMAGE_PARAM1].nDSTBlendingMode		= KM_INVSRCALPHA;
	StripContext_3DSprites.ImageControl[KM_IMAGE_PARAM1].bSRCSelect				= KM_FALSE;
	StripContext_3DSprites.ImageControl[KM_IMAGE_PARAM1].bDSTSelect				= KM_FALSE;
	StripContext_3DSprites.ImageControl[KM_IMAGE_PARAM1].nFogMode				= KM_NOFOG;
	StripContext_3DSprites.ImageControl[KM_IMAGE_PARAM1].bColorClamp			= KM_FALSE;	
	StripContext_3DSprites.ImageControl[KM_IMAGE_PARAM1].bUseAlpha				= KM_TRUE;
	StripContext_3DSprites.ImageControl[KM_IMAGE_PARAM1].bIgnoreTextureAlpha	= KM_FALSE;
	StripContext_3DSprites.ImageControl[KM_IMAGE_PARAM1].nFilterMode			= KM_BILINEAR;
    StripContext_3DSprites.ImageControl[KM_IMAGE_PARAM1].pTextureSurfaceDesc = &DCKtextureList[0].surface;
	kmGenerateStripHead03(&StripHead_3DSprites,&StripContext_3DSprites);

	// 3D Backdrop strip
    kmInitStripContext(KM_STRIPCONTEXT_SYS_GOURAUD | KM_OPAQUE_POLYGON, &StripContext_3DBackdrop);
	memset(&StripContext_3DBackdrop,0,sizeof(StripContext_3DBackdrop));
	memset(&StripHead_3DBackdrop,0,sizeof(StripHead_3DBackdrop));
	StripContext_3DBackdrop.nSize = sizeof(KMSTRIPCONTEXT);
    kmInitStripContext(KM_STRIPCONTEXT_SYS_GOURAUD | KM_OPAQUE_POLYGON, &StripContext_3DBackdrop);
    
	StripContext_3DBackdrop.StripControl.nListType		 						= KM_OPAQUE_POLYGON;
	StripContext_3DBackdrop.StripControl.nUserClipMode	 						= KM_USERCLIP_DISABLE;
	StripContext_3DBackdrop.StripControl.nShadowMode			 				= KM_NORMAL_POLYGON;
	StripContext_3DBackdrop.StripControl.bGouraud		 						= KM_TRUE;
	StripContext_3DBackdrop.ObjectControl.nDepthCompare				 			= KM_GREATER;
	StripContext_3DBackdrop.ObjectControl.nCullingMode			 				= KM_CULLCW;
	StripContext_3DBackdrop.ObjectControl.bZWriteDisable						= KM_FALSE;
	StripContext_3DBackdrop.ImageControl[KM_IMAGE_PARAM1].nSRCBlendingMode		= KM_ONE;	
	StripContext_3DBackdrop.ImageControl[KM_IMAGE_PARAM1].nDSTBlendingMode		= KM_ZERO;
	StripContext_3DBackdrop.ImageControl[KM_IMAGE_PARAM1].bSRCSelect			= KM_FALSE;
	StripContext_3DBackdrop.ImageControl[KM_IMAGE_PARAM1].bDSTSelect			= KM_FALSE;
	StripContext_3DBackdrop.ImageControl[KM_IMAGE_PARAM1].nFogMode				= KM_NOFOG;	
	StripContext_3DBackdrop.ImageControl[KM_IMAGE_PARAM1].bColorClamp			= KM_FALSE;
	StripContext_3DBackdrop.ImageControl[KM_IMAGE_PARAM1].bUseAlpha				= KM_FALSE;
	StripContext_3DBackdrop.ImageControl[KM_IMAGE_PARAM1].bIgnoreTextureAlpha	= KM_FALSE;
	StripContext_3DBackdrop.ImageControl[KM_IMAGE_PARAM1].nFilterMode			= KM_BILINEAR;
    StripContext_3DBackdrop.ImageControl[KM_IMAGE_PARAM1].pTextureSurfaceDesc 	= &DCKtextureList[0].surface;
    StripContext_3DBackdrop.ImageControl[KM_IMAGE_PARAM1].nClampUV 				= KM_CLAMP_UV;
	kmGenerateStripHead03(&StripHead_3DBackdrop,&StripContext_3DBackdrop);


}

void my_setPolyG3(POLY_G3 *packet, TMD_P_FG3I *opcd)
{
	long	xy;
	
	xy = transformedVertices[opcd->v0];
	vertices_G3[0].fX = (float)((short)(xy&0xFFFF));
	vertices_G3[0].fY = (float)((short)(xy>>16));
	vertices_G3[0].u.fInvW = 1.0 / ((float)(transformedDepths[opcd->v0]));
	vertices_G3[0].uBaseRGB.dwPacked = RGBA(packet->r0,packet->g0,packet->b0,255);

	xy = transformedVertices[opcd->v1];
	vertices_G3[1].fX = (float)((short)(xy&0xFFFF));
	vertices_G3[1].fY = (float)((short)(xy>>16));
	vertices_G3[1].u.fInvW = 1.0 / ((float)(transformedDepths[opcd->v1]));
	vertices_G3[1].uBaseRGB.dwPacked = RGBA(packet->r2,packet->g2,packet->b2,255);

	xy = transformedVertices[opcd->v2];
	vertices_G3[2].fX = (float)((short)(xy&0xFFFF));
	vertices_G3[2].fY = (float)((short)(xy>>16));
	vertices_G3[2].u.fInvW = 1.0 / ((float)(transformedDepths[opcd->v2]));
	vertices_G3[2].uBaseRGB.dwPacked = RGBA(packet->r1,packet->g1,packet->b1,255);
							
	kmStartStrip(&vertexBufferDesc, &StripHead_G3);	
	kmSetVertex(&vertexBufferDesc, &vertices_G3[0], KM_VERTEXTYPE_00, sizeof(KMVERTEX_00));
	kmSetVertex(&vertexBufferDesc, &vertices_G3[1], KM_VERTEXTYPE_00, sizeof(KMVERTEX_00));
	kmSetVertex(&vertexBufferDesc, &vertices_G3[2], KM_VERTEXTYPE_00, sizeof(KMVERTEX_00));	
	kmEndStrip(&vertexBufferDesc);
}

void my_setPolyG4(POLY_G4 *packet, TMD_P_FG4I *opcd)
{
	long	xy;
	
	xy = transformedVertices[opcd->v0];
	vertices_G4[0].fX = (float)((short)(xy&0xFFFF));
	vertices_G4[0].fY = (float)((short)(xy>>16));
	vertices_G4[0].u.fInvW = 1.0 / ((float)(transformedDepths[opcd->v0]));
	vertices_G4[0].uBaseRGB.dwPacked = RGBA(packet->r0,packet->g0,packet->b0,255);

	xy = transformedVertices[opcd->v2];
	vertices_G4[1].fX = (float)((short)(xy&0xFFFF));
	vertices_G4[1].fY = (float)((short)(xy>>16));
	vertices_G4[1].u.fInvW = 1.0 / ((float)(transformedDepths[opcd->v1]));
	vertices_G4[1].uBaseRGB.dwPacked = RGBA(packet->r2,packet->g2,packet->b2,255);

	xy = transformedVertices[opcd->v2];
	vertices_G4[2].fX = (float)((short)(xy&0xFFFF));
	vertices_G4[2].fY = (float)((short)(xy>>16));
	vertices_G4[2].u.fInvW = 1.0 / ((float)(transformedDepths[opcd->v2]));
	vertices_G4[2].uBaseRGB.dwPacked = RGBA(packet->r1,packet->g1,packet->b1,255);

	xy = transformedVertices[opcd->v3];
	vertices_G4[3].fX = (float)((short)(xy&0xFFFF));
	vertices_G4[3].fY = (float)((short)(xy>>16));
	vertices_G4[3].u.fInvW = 1.0 / ((float)(transformedDepths[opcd->v3]));
	vertices_G4[3].uBaseRGB.dwPacked = RGBA(packet->r3,packet->g3,packet->b3,255);
							
	kmStartStrip(&vertexBufferDesc, &StripHead_G4);	
	kmSetVertex(&vertexBufferDesc, &vertices_G4[0], KM_VERTEXTYPE_00, sizeof(KMVERTEX_00));
	kmSetVertex(&vertexBufferDesc, &vertices_G4[1], KM_VERTEXTYPE_00, sizeof(KMVERTEX_00));
	kmSetVertex(&vertexBufferDesc, &vertices_G4[2], KM_VERTEXTYPE_00, sizeof(KMVERTEX_00));	
	kmSetVertex(&vertexBufferDesc, &vertices_G4[3], KM_VERTEXTYPE_00, sizeof(KMVERTEX_00));	
	kmEndStrip(&vertexBufferDesc);
}

void my_setPolyFT3(POLY_FT3 *packet, TMD_P_FT3I *opcd)
{
	int		i,ds;
	float	uvDivide;
	long 	*tfv,*tfd;
	long	xy;
	short	x,y;
	short	*ptr;
	float	*uvTable;
	
	tfv = transformedVertices;
	tfd = transformedDepths;

#define GETX(n)( ((SHORTXY *)( (int)(tfv) +(n) ))->x )
#define GETY(n)( ((SHORTXY *)( (int)(tfv) +(n) ))->y )
#define GETD(n)(  *(u_long *)( (int)(tfd) +(n) ) )

	uvTable	= uvLookUpTable4bpp;
//	if(DCKtextureList[opcd->tpage].bpp == 8)
//		uvTable	= uvLookUpTable8bpp;

	xy = transformedVertices[opcd->v0];
	vertices_FT3[0].fX = (float)((short)(xy&0xFFFF));
	vertices_FT3[0].fY = (float)((short)(xy>>16));
	vertices_FT3[0].u.fInvW = 1.0 / ((float)(transformedDepths[opcd->v0]));
	vertices_FT3[0].fU = uvTable[opcd->tu0];
	vertices_FT3[0].fV = uvTable[opcd->tv0];
	vertices_FT3[0].uBaseRGB.dwPacked = RGBA(255,255,255,255);

	xy = transformedVertices[opcd->v1];
	vertices_FT3[1].fX = (float)((short)(xy&0xFFFF));
	vertices_FT3[1].fY = (float)((short)(xy>>16));
	vertices_FT3[1].u.fInvW = 1.0 / ((float)(transformedDepths[opcd->v1]));
	vertices_FT3[1].fU = uvTable[opcd->tu1];
	vertices_FT3[1].fV = uvTable[opcd->tv1];
	vertices_FT3[1].uBaseRGB.dwPacked = RGBA(255,255,255,255);

	xy = transformedVertices[opcd->v2];
	vertices_FT3[2].fX = (float)((short)(xy&0xFFFF));
	vertices_FT3[2].fY = (float)((short)(xy>>16));
	vertices_FT3[2].u.fInvW = 1.0 / ((float)(transformedDepths[opcd->v2]));
	vertices_FT3[2].fU = uvTable[opcd->tu2];
	vertices_FT3[2].fV = uvTable[opcd->tv2];
	vertices_FT3[2].uBaseRGB.dwPacked = RGBA(255,255,255,255);

	// check to see if alpha channel is to be used
	if(DCKtextureList[opcd->tpage].colourKey)
	{
		// change strip if required
		if(opcd->tpage != stripFT3textureID_A)
		{
			kmChangeStripTextureSurface(&StripHead_FT3_Alpha,KM_IMAGE_PARAM1,DCKtextureList[opcd->tpage].surfacePtr);
			stripFT3textureID_A = opcd->tpage;
		}
		
		// test to see if the poly is double sided
		ds = (opcd->dummy & psiDOUBLESIDED);
		if(ds != stripFT3dsided_A)
		{
			if(ds)
			{
				kmChangeStripCullingMode(&StripHead_FT3_Alpha,KM_NOCULLING);
				stripFT3dsided_A = TRUE;
			}
			else
			{
				kmChangeStripCullingMode(&StripHead_FT3_Alpha,KM_CULLCW);
				stripFT3dsided_A = FALSE;
			}
		}			
		kmStartStrip(&vertexBufferDesc, &StripHead_FT3_Alpha);
	}	
	else
	{
		// change strip if required
		if(opcd->tpage != stripFT3textureID)
		{
			kmChangeStripTextureSurface(&StripHead_FT3,KM_IMAGE_PARAM1,DCKtextureList[opcd->tpage].surfacePtr);
			stripFT3textureID = opcd->tpage;
		}

		// test to see if the poly is double sided
		ds = (opcd->dummy & psiDOUBLESIDED);
		if(ds != stripFT3dsided_A)
		{
			if(ds)
			{
				kmChangeStripCullingMode(&StripHead_FT3_Alpha,KM_NOCULLING);
				stripFT3dsided_A = TRUE;
			}
			else
			{
				kmChangeStripCullingMode(&StripHead_FT3_Alpha,KM_CULLCW);
				stripFT3dsided_A = FALSE;
			}
		}	
		kmStartStrip(&vertexBufferDesc, &StripHead_FT3);	
	}	
							
	kmSetVertex(&vertexBufferDesc, &vertices_FT3[0], KM_VERTEXTYPE_03, sizeof(KMVERTEX_03));
	kmSetVertex(&vertexBufferDesc, &vertices_FT3[1], KM_VERTEXTYPE_03, sizeof(KMVERTEX_03));
	kmSetVertex(&vertexBufferDesc, &vertices_FT3[2], KM_VERTEXTYPE_03, sizeof(KMVERTEX_03));	
	kmEndStrip(&vertexBufferDesc);
}

void my_setPolyFT4(POLY_FT4 *packet, TMD_P_FT4I *opcd)
{
	int		i,ds;
	float	uvDivide;
	long 	*tfv,*tfd;
	long	xy;
	short	x,y;
	short	*ptr;
	float	*uvTable;

	tfv = transformedVertices;
	tfd = transformedDepths;

#define GETX(n)( ((SHORTXY *)( (int)(tfv) +(n) ))->x )
#define GETY(n)( ((SHORTXY *)( (int)(tfv) +(n) ))->y )
#define GETD(n)(  *(u_long *)( (int)(tfd) +(n) ) )

	uvTable	= uvLookUpTable4bpp;
//	if(DCKtextureList[opcd->tpage].bpp == 8)
//		uvTable	= uvLookUpTable8bpp;

	xy = transformedVertices[opcd->v0];
	vertices_FT4[0].fX = (float)((short)(xy&0xFFFF));
	vertices_FT4[0].fY = (float)((short)(xy>>16));
	vertices_FT4[0].u.fInvW = 1.0 / ((float)(transformedDepths[opcd->v0]));
	vertices_FT4[0].fU = uvTable[opcd->tu0];
	vertices_FT4[0].fV = uvTable[opcd->tv0];
	vertices_FT4[0].uBaseRGB.dwPacked = RGBA(255,255,255,255);

	xy = transformedVertices[opcd->v1];
	vertices_FT4[1].fX = (float)((short)(xy&0xFFFF));
	vertices_FT4[1].fY = (float)((short)(xy>>16));
	vertices_FT4[1].u.fInvW = 1.0 / ((float)(transformedDepths[opcd->v1]));
	vertices_FT4[1].fU = uvTable[opcd->tu1];
	vertices_FT4[1].fV = uvTable[opcd->tv1];
	vertices_FT4[1].uBaseRGB.dwPacked = RGBA(255,255,255,255);

	xy = transformedVertices[opcd->v2];
	vertices_FT4[2].fX = (float)((short)(xy&0xFFFF));
	vertices_FT4[2].fY = (float)((short)(xy>>16));
	vertices_FT4[2].u.fInvW = 1.0 / ((float)(transformedDepths[opcd->v2]));
	vertices_FT4[2].fU = uvTable[opcd->tu2];
	vertices_FT4[2].fV = uvTable[opcd->tv2];
	vertices_FT4[2].uBaseRGB.dwPacked = RGBA(255,255,255,255);

	xy = transformedVertices[opcd->v3];
	vertices_FT4[3].fX = (float)((short)(xy&0xFFFF));
	vertices_FT4[3].fY = (float)((short)(xy>>16));
	vertices_FT4[3].u.fInvW = 1.0 / ((float)(transformedDepths[opcd->v3]));
	vertices_FT4[3].fU = uvTable[opcd->tu3];
	vertices_FT4[3].fV = uvTable[opcd->tv3];
	vertices_FT4[3].uBaseRGB.dwPacked = RGBA(255,255,255,255);

	// check to see if alpha channel is to be used
	if(DCKtextureList[opcd->tpage].colourKey)
	{
		// change strip if required
		if(opcd->tpage != stripFT4textureID_A)
		{
			kmChangeStripTextureSurface(&StripHead_FT4_Alpha,KM_IMAGE_PARAM1,DCKtextureList[opcd->tpage].surfacePtr);
			stripFT4textureID_A = opcd->tpage;
		}
		
		// test to see if the poly is double sided
		ds = (opcd->dummy & psiDOUBLESIDED);
		if(ds != stripFT4dsided_A)
		{
			if(ds)
			{
				kmChangeStripCullingMode(&StripHead_FT4_Alpha,KM_NOCULLING);
				stripFT4dsided_A = TRUE;
			}
			else
			{
				kmChangeStripCullingMode(&StripHead_FT4_Alpha,KM_CULLCW);
				stripFT4dsided_A = FALSE;
			}
		}	
		kmStartStrip(&vertexBufferDesc, &StripHead_FT4_Alpha);
	}	
	else
	{
		// change strip if required
		// *ASL* 13/08/2000 - Changed to stripFt4textureID
		if(opcd->tpage != stripFT4textureID)
		{
			kmChangeStripTextureSurface(&StripHead_FT4,KM_IMAGE_PARAM1,DCKtextureList[opcd->tpage].surfacePtr);
			stripFT4textureID = opcd->tpage;
		}

		// test to see if the poly is double sided
		ds = (opcd->dummy & psiDOUBLESIDED);
		if(ds != stripFT4dsided)
		{
			if(ds)
			{
				kmChangeStripCullingMode(&StripHead_FT4,KM_NOCULLING);
				stripFT4dsided = TRUE;
			}
			else
			{
				kmChangeStripCullingMode(&StripHead_FT4,KM_CULLCW);
				stripFT4dsided = FALSE;
			}
		}	
		kmStartStrip(&vertexBufferDesc, &StripHead_FT4);	
	}	

	kmSetVertex(&vertexBufferDesc, &vertices_FT4[0], KM_VERTEXTYPE_03, sizeof(KMVERTEX_03));
	kmSetVertex(&vertexBufferDesc, &vertices_FT4[1], KM_VERTEXTYPE_03, sizeof(KMVERTEX_03));
	kmSetVertex(&vertexBufferDesc, &vertices_FT4[2], KM_VERTEXTYPE_03, sizeof(KMVERTEX_03));	
	kmSetVertex(&vertexBufferDesc, &vertices_FT4[3], KM_VERTEXTYPE_03, sizeof(KMVERTEX_03));	
	kmEndStrip(&vertexBufferDesc);
}

void my_setPolyGT3(POLY_GT3 *packet, TMD_P_GT3I *opcd)
{
	int		i;
	float	uvDivide;
	long 	*tfv,*tfd;
	long	xy;
	short	x,y;
	short	*ptr;
	float	*uvTable;
	
	tfv = transformedVertices;
	tfd = transformedDepths;

#define GETX(n)( ((SHORTXY *)( (int)(tfv) +(n) ))->x )
#define GETY(n)( ((SHORTXY *)( (int)(tfv) +(n) ))->y )
#define GETD(n)(  *(u_long *)( (int)(tfd) +(n) ) )

	uvTable	= uvLookUpTable4bpp;
//	if(DCKtextureList[opcd->tpage].bpp == 8)
//		uvTable	= uvLookUpTable8bpp;

	xy = transformedVertices[opcd->v0];
	vertices_GT3[0].fX = (float)((short)(xy&0xFFFF));
	vertices_GT3[0].fY = (float)((short)(xy>>16));
	vertices_GT3[0].u.fInvW = 1.0 / ((float)(transformedDepths[opcd->v0]));
	vertices_GT3[0].fU = uvTable[opcd->tu0];
	vertices_GT3[0].fV = uvTable[opcd->tv0];
	vertices_GT3[0].uBaseRGB.dwPacked = RGBA(opcd->r0,opcd->g0,opcd->b0,255);

	xy = transformedVertices[opcd->v1];
	vertices_GT3[1].fX = (float)((short)(xy&0xFFFF));
	vertices_GT3[1].fY = (float)((short)(xy>>16));
	vertices_GT3[1].u.fInvW = 1.0 / ((float)(transformedDepths[opcd->v1]));
	vertices_GT3[1].fU = uvTable[opcd->tu1];
	vertices_GT3[1].fV = uvTable[opcd->tv1];
	vertices_GT3[1].uBaseRGB.dwPacked = RGBA(opcd->r1,opcd->g1,opcd->b1,255);

	xy = transformedVertices[opcd->v2];
	vertices_GT3[2].fX = (float)((short)(xy&0xFFFF));
	vertices_GT3[2].fY = (float)((short)(xy>>16));
	vertices_GT3[2].u.fInvW = 1.0 / ((float)(transformedDepths[opcd->v2]));
	vertices_GT3[2].fU = uvTable[opcd->tu2];
	vertices_GT3[2].fV = uvTable[opcd->tv2];
	vertices_GT3[2].uBaseRGB.dwPacked = RGBA(opcd->r2,opcd->g2,opcd->b2,255);

	// check to see if alpha channel is to be used
	if(DCKtextureList[opcd->tpage].colourKey)
	{
		// change strip if required
		if(opcd->tpage != stripGT3textureID_A)
		{
			kmChangeStripTextureSurface(&StripHead_GT3_Alpha,KM_IMAGE_PARAM1,DCKtextureList[opcd->tpage].surfacePtr);
			stripGT3textureID_A = opcd->tpage;
		}
		kmStartStrip(&vertexBufferDesc, &StripHead_GT3_Alpha);
	}	
	else
	{
		// change strip if required
		if(opcd->tpage != stripGT3textureID)
		{
			kmChangeStripTextureSurface(&StripHead_GT3,KM_IMAGE_PARAM1,DCKtextureList[opcd->tpage].surfacePtr);
			stripGT3textureID = opcd->tpage;
		}
		kmStartStrip(&vertexBufferDesc, &StripHead_GT3);	
	}	
							
	kmSetVertex(&vertexBufferDesc, &vertices_GT3[0], KM_VERTEXTYPE_03, sizeof(KMVERTEX_03));
	kmSetVertex(&vertexBufferDesc, &vertices_GT3[1], KM_VERTEXTYPE_03, sizeof(KMVERTEX_03));
	kmSetVertex(&vertexBufferDesc, &vertices_GT3[2], KM_VERTEXTYPE_03, sizeof(KMVERTEX_03));	
	kmEndStrip(&vertexBufferDesc);
}

void my_setPolyGT4(POLY_GT4 *packet, TMD_P_GT4I *opcd)
{
	int		i,ds;
	float	uvDivide;
	long 	*tfv,*tfd;
	long	xy;
	short	x,y;
	short	*ptr;
	float	*uvTable;

	tfv = transformedVertices;
	tfd = transformedDepths;

#define GETX(n)( ((SHORTXY *)( (int)(tfv) +(n) ))->x )
#define GETY(n)( ((SHORTXY *)( (int)(tfv) +(n) ))->y )
#define GETD(n)(  *(u_long *)( (int)(tfd) +(n) ) )

	uvTable	= uvLookUpTable4bpp;
//	if(DCKtextureList[opcd->tpage].bpp == 8)
//		uvTable	= uvLookUpTable8bpp;

	xy = transformedVertices[opcd->v0];
	vertices_GT4[0].fX = (float)((short)(xy&0xFFFF));
	vertices_GT4[0].fY = (float)((short)(xy>>16));
	vertices_GT4[0].u.fInvW = 1.0 / ((float)(transformedDepths[opcd->v0]));
	vertices_GT4[0].fU = uvTable[opcd->tu0];
	vertices_GT4[0].fV = uvTable[opcd->tv0];
	vertices_GT4[0].uBaseRGB.dwPacked = RGBA(opcd->r0,opcd->g0,opcd->b0,255);

	xy = transformedVertices[opcd->v1];
	vertices_GT4[1].fX = (float)((short)(xy&0xFFFF));
	vertices_GT4[1].fY = (float)((short)(xy>>16));
	vertices_GT4[1].u.fInvW = 1.0 / ((float)(transformedDepths[opcd->v1]));
	vertices_GT4[1].fU = uvTable[opcd->tu1];
	vertices_GT4[1].fV = uvTable[opcd->tv1];
	vertices_GT4[1].uBaseRGB.dwPacked = RGBA(opcd->r1,opcd->g1,opcd->b1,255);

	xy = transformedVertices[opcd->v2];
	vertices_GT4[2].fX = (float)((short)(xy&0xFFFF));
	vertices_GT4[2].fY = (float)((short)(xy>>16));
	vertices_GT4[2].u.fInvW = 1.0 / ((float)(transformedDepths[opcd->v2]));
	vertices_GT4[2].fU = uvTable[opcd->tu2];
	vertices_GT4[2].fV = uvTable[opcd->tv2];
	vertices_GT4[2].uBaseRGB.dwPacked = RGBA(opcd->r2,opcd->g2,opcd->b2,255);

	xy = transformedVertices[opcd->v3];
	vertices_GT4[3].fX = (float)((short)(xy&0xFFFF));
	vertices_GT4[3].fY = (float)((short)(xy>>16));
	vertices_GT4[3].u.fInvW = 1.0 / ((float)(transformedDepths[opcd->v3]));
	vertices_GT4[3].fU = uvTable[opcd->tu3];
	vertices_GT4[3].fV = uvTable[opcd->tv3];
	vertices_GT4[3].uBaseRGB.dwPacked = RGBA(opcd->r3,opcd->g3,opcd->b3,255);

	// check to see if alpha channel is to be used
	if(DCKtextureList[opcd->tpage].colourKey)
	{
		// change strip if required
		if(opcd->tpage != stripGT4textureID_A)
		{
			kmChangeStripTextureSurface(&StripHead_GT4_Alpha,KM_IMAGE_PARAM1,DCKtextureList[opcd->tpage].surfacePtr);
			stripGT4textureID_A = opcd->tpage;
		}
		
		// test to see if the poly is double sided
		ds = (opcd->dummy & psiDOUBLESIDED);
		if(ds != stripGT4dsided_A)
		{
			if(ds)
			{
				kmChangeStripCullingMode(&StripHead_GT4_Alpha,KM_NOCULLING);
				stripGT4dsided_A = TRUE;
			}
			else
			{
				kmChangeStripCullingMode(&StripHead_GT4_Alpha,KM_CULLCW);
				stripGT4dsided_A = FALSE;
			}
		}	
		kmStartStrip(&vertexBufferDesc, &StripHead_GT4_Alpha);
	}	
	else
	{
		// change strip if required
		if(opcd->tpage != stripGT4textureID)
		{
			kmChangeStripTextureSurface(&StripHead_GT4,KM_IMAGE_PARAM1,DCKtextureList[opcd->tpage].surfacePtr);
			stripGT4textureID = opcd->tpage;
		}

		// test to see if the poly is double sided
		ds = (opcd->dummy & psiDOUBLESIDED);
		if(ds != stripGT4dsided)
		{
			if(ds)
			{
				kmChangeStripCullingMode(&StripHead_GT4,KM_NOCULLING);
				stripGT4dsided = TRUE;
			}
			else
			{
				kmChangeStripCullingMode(&StripHead_GT4,KM_CULLCW);
				stripGT4dsided = FALSE;
			}
		}	
		kmStartStrip(&vertexBufferDesc, &StripHead_GT4);	
	}

	kmSetVertex(&vertexBufferDesc, &vertices_GT4[0], KM_VERTEXTYPE_03, sizeof(KMVERTEX_03));
	kmSetVertex(&vertexBufferDesc, &vertices_GT4[1], KM_VERTEXTYPE_03, sizeof(KMVERTEX_03));
	kmSetVertex(&vertexBufferDesc, &vertices_GT4[2], KM_VERTEXTYPE_03, sizeof(KMVERTEX_03));	
	kmSetVertex(&vertexBufferDesc, &vertices_GT4[3], KM_VERTEXTYPE_03, sizeof(KMVERTEX_03));	
	kmEndStrip(&vertexBufferDesc);
}

void my_setPolyTF4SPR(POLY_FT4 *packet, TMD_P_FT4I *opcd)
{
	int		i,width,height;
	float	uvDivide;
	long 	*tfv,*tfd;
	long	xy;
	short	x,y;
	short	*ptr;

	tfv = transformedVertices;
	tfd = transformedDepths;

	width = ((opcd->v2 * GSh) / (transformedDepths[opcd->v0]*4)) / 2;
	height = ((opcd->v3 * GSh) / (transformedDepths[opcd->v0]*4)) / 2;		//ma was 4
	
#define GETX(n)( ((SHORTXY *)( (int)(tfv) +(n) ))->x )
#define GETY(n)( ((SHORTXY *)( (int)(tfv) +(n) ))->y )
#define GETD(n)(  *(u_long *)( (int)(tfd) +(n) ) )

	xy = transformedVertices[opcd->v0];
	x = (float)((short)(xy&0xFFFF));
	y = (float)((short)(xy>>16));
	
	vertices_TF4SPR[0].fX = (float)(x - width);
	vertices_TF4SPR[0].fY = (float)(y - height);
	vertices_TF4SPR[0].u.fInvW = 1.0 / ((float)(transformedDepths[opcd->v0]));
	vertices_TF4SPR[0].fU = (float)0;
	vertices_TF4SPR[0].fV = (float)1;
	vertices_TF4SPR[0].uBaseRGB.dwPacked = RGBA(opcd->r0,opcd->g0,opcd->b0,255);

	vertices_TF4SPR[1].fX = (float)(x + width);
	vertices_TF4SPR[1].fY = (float)(y - height);
	vertices_TF4SPR[1].u.fInvW = 1.0 / ((float)(transformedDepths[opcd->v0]));
	vertices_TF4SPR[1].fU = (float)0;
	vertices_TF4SPR[1].fV = (float)0;
	vertices_TF4SPR[1].uBaseRGB.dwPacked = RGBA(opcd->r0,opcd->g0,opcd->b0,255);

	vertices_TF4SPR[2].fX = (float)(x - width);
	vertices_TF4SPR[2].fY = (float)(y + height);
	vertices_TF4SPR[2].u.fInvW = 1.0 / ((float)(transformedDepths[opcd->v0]));
	vertices_TF4SPR[2].fU = (float)1;
	vertices_TF4SPR[2].fV = (float)1;
	vertices_TF4SPR[2].uBaseRGB.dwPacked = RGBA(opcd->r0,opcd->g0,opcd->b0,255);

	vertices_TF4SPR[3].fX = (float)(x + width);
	vertices_TF4SPR[3].fY = (float)(y + height);
	vertices_TF4SPR[3].u.fInvW = 1.0 / ((float)(transformedDepths[opcd->v0]));
	vertices_TF4SPR[3].fU = (float)1;
	vertices_TF4SPR[3].fV = (float)0;
	vertices_TF4SPR[3].uBaseRGB.dwPacked = RGBA(opcd->r0,opcd->g0,opcd->b0,255);

	// check to see if alpha channel is to be used
	if(DCKtextureList[opcd->tpage].colourKey)
	{
		// change strip if required
		if(opcd->tpage != stripTF4SPRtextureID_A)
		{
			kmChangeStripTextureSurface(&StripHead_TF4SPR_Alpha,KM_IMAGE_PARAM1,DCKtextureList[opcd->tpage].surfacePtr);
			stripTF4SPRtextureID_A = opcd->tpage;
		}
		kmStartStrip(&vertexBufferDesc, &StripHead_TF4SPR_Alpha);
	}	
	else
	{
		// change strip if required
		if(opcd->tpage != stripTF4SPRtextureID)
		{
			kmChangeStripTextureSurface(&StripHead_TF4SPR,KM_IMAGE_PARAM1,DCKtextureList[opcd->tpage].surfacePtr);
			stripTF4SPRtextureID = opcd->tpage;
		}
		kmStartStrip(&vertexBufferDesc, &StripHead_TF4SPR);	
	}	

	kmSetVertex(&vertexBufferDesc, &vertices_TF4SPR[0], KM_VERTEXTYPE_03, sizeof(KMVERTEX_03));
	kmSetVertex(&vertexBufferDesc, &vertices_TF4SPR[1], KM_VERTEXTYPE_03, sizeof(KMVERTEX_03));
	kmSetVertex(&vertexBufferDesc, &vertices_TF4SPR[2], KM_VERTEXTYPE_03, sizeof(KMVERTEX_03));	
	kmSetVertex(&vertexBufferDesc, &vertices_TF4SPR[3], KM_VERTEXTYPE_03, sizeof(KMVERTEX_03));	
	kmEndStrip(&vertexBufferDesc);
}

void my_setPolyFMA_GT3(POLY_GT3 *packet, FMA_GT3 *opcd)
{
	float	uvDivide;
	long 	*tfv;
	long	*tfd;
	float	*uvTable;
	long	xy;
	short	x,y;
	
	tfv = transformedVertices;
	tfd = transformedDepths;

#define GETX(n)( ((SHORTXY *)( (int)(tfv) +(n) ))->x )
#define GETY(n)( ((SHORTXY *)( (int)(tfv) +(n) ))->y )
#define GETD(n)(  *(u_long *)( (int)(tfd) +(n) ) )

//	uvTable	= uvLookUpTable4bpp;
//	if(DCKtextureList[opcd->tpage].bpp == 8)
		uvTable	= uvLookUpTable8bpp;

	vertices_GT3_FMA[0].fX = GETX(opcd->vert0);
	vertices_GT3_FMA[0].fY = GETY(opcd->vert0);
	vertices_GT3_FMA[0].u.fZ = 1.0 / ((float)GETD(opcd->vert0));
	vertices_GT3_FMA[0].fU = uvTable[opcd->u0];
	vertices_GT3_FMA[0].fV = uvTable[opcd->v0];
	vertices_GT3_FMA[0].uBaseRGB.dwPacked = RGBA(opcd->r0,opcd->g0,opcd->b0,255);

	vertices_GT3_FMA[1].fX = GETX(opcd->vert1);
	vertices_GT3_FMA[1].fY = GETY(opcd->vert1);
	vertices_GT3_FMA[1].u.fZ = 1.0 / ((float)GETD(opcd->vert1));
	vertices_GT3_FMA[1].fU = uvTable[opcd->u1];
	vertices_GT3_FMA[1].fV = uvTable[opcd->v1];
	vertices_GT3_FMA[1].uBaseRGB.dwPacked = RGBA(opcd->r1,opcd->g1,opcd->b1,255);

	vertices_GT3_FMA[2].fX = GETX(opcd->vert2);
	vertices_GT3_FMA[2].fY = GETY(opcd->vert2);
	vertices_GT3_FMA[2].u.fZ = 1.0 / ((float)GETD(opcd->vert2));
	vertices_GT3_FMA[2].fU = uvTable[opcd->u2];
	vertices_GT3_FMA[2].fV = uvTable[opcd->v2];
	vertices_GT3_FMA[2].uBaseRGB.dwPacked = RGBA(opcd->r2,opcd->g2,opcd->b2,255);

	// check to see if alpha channel is to be used
	if(DCKtextureList[opcd->tpage].colourKey)
	{
		// change strip if required
		if(opcd->tpage != stripGT3FMAtextureID_A)
		{
			kmChangeStripTextureSurface(&StripHead_GT3_FMA_Alpha,KM_IMAGE_PARAM1,DCKtextureList[opcd->tpage].surfacePtr);
			stripGT3FMAtextureID_A = opcd->tpage;
		}
		kmStartStrip(&vertexBufferDesc, &StripHead_GT3_FMA_Alpha);
	}	
	else
	{
		// change strip if required
		if(opcd->tpage != stripGT3FMAtextureID)
		{
			kmChangeStripTextureSurface(&StripHead_GT3_FMA,KM_IMAGE_PARAM1,DCKtextureList[opcd->tpage].surfacePtr);
			stripGT3FMAtextureID = opcd->tpage;
		}
		kmStartStrip(&vertexBufferDesc, &StripHead_GT3_FMA);	
	}	

	kmSetVertex(&vertexBufferDesc, &vertices_GT3_FMA[0], KM_VERTEXTYPE_03, sizeof(KMVERTEX_03));
	kmSetVertex(&vertexBufferDesc, &vertices_GT3_FMA[1], KM_VERTEXTYPE_03, sizeof(KMVERTEX_03));
	kmSetVertex(&vertexBufferDesc, &vertices_GT3_FMA[2], KM_VERTEXTYPE_03, sizeof(KMVERTEX_03));	
	kmEndStrip(&vertexBufferDesc);
}

void my_setPolyFMA_GT4(POLY_GT4 *packet, FMA_GT4 *opcd)
{
	float	uvDivide;
	long 	*tfv;
	long	*tfd;
	float	*uvTable;
	
	tfv = transformedVertices;
	tfd = transformedDepths;

#define GETX(n)( ((SHORTXY *)( (int)(tfv) +(n) ))->x )
#define GETY(n)( ((SHORTXY *)( (int)(tfv) +(n) ))->y )
#define GETD(n)(  *(u_long *)( (int)(tfd) +(n) ) )
	
//	uvTable	= uvLookUpTable4bpp;
//	if(DCKtextureList[opcd->tpage].bpp == 8)
		uvTable	= uvLookUpTable8bpp;

	vertices_GT4_FMA[0].fX = GETX(opcd->vert0);
	vertices_GT4_FMA[0].fY = GETY(opcd->vert0);
	vertices_GT4_FMA[0].u.fZ = 1.0 / ((float)GETD(opcd->vert0));
	vertices_GT4_FMA[0].fU = uvTable[opcd->u0];
	vertices_GT4_FMA[0].fV = uvTable[opcd->v0];
	vertices_GT4_FMA[0].uBaseRGB.dwPacked = RGBA(opcd->r0,opcd->g0,opcd->b0,255);

	vertices_GT4_FMA[1].fX = GETX(opcd->vert1);
	vertices_GT4_FMA[1].fY = GETY(opcd->vert1);
	vertices_GT4_FMA[1].u.fZ = 1.0 / ((float)GETD(opcd->vert1));
	vertices_GT4_FMA[1].fU = uvTable[opcd->u1];
	vertices_GT4_FMA[1].fV = uvTable[opcd->v1];
	vertices_GT4_FMA[1].uBaseRGB.dwPacked = RGBA(opcd->r1,opcd->g1,opcd->b1,255);

	vertices_GT4_FMA[2].fX = GETX(opcd->vert2);
	vertices_GT4_FMA[2].fY = GETY(opcd->vert2);
	vertices_GT4_FMA[2].u.fZ = 1.0 / ((float)GETD(opcd->vert2));
	vertices_GT4_FMA[2].fU = uvTable[opcd->u2];
	vertices_GT4_FMA[2].fV = uvTable[opcd->v2];
	vertices_GT4_FMA[2].uBaseRGB.dwPacked = RGBA(opcd->r2,opcd->g2,opcd->b2,255);

	vertices_GT4_FMA[3].fX = GETX(opcd->vert3);
	vertices_GT4_FMA[3].fY = GETY(opcd->vert3);
	vertices_GT4_FMA[3].u.fZ = 1.0 / ((float)GETD(opcd->vert3));
	vertices_GT4_FMA[3].fU = uvTable[opcd->u3];
	vertices_GT4_FMA[3].fV = uvTable[opcd->v3];
	vertices_GT4_FMA[3].uBaseRGB.dwPacked = RGBA(opcd->r3,opcd->g3,opcd->b3,255);

	// check to see if alpha channel is to be used
	if(DCKtextureList[opcd->tpage].colourKey)
	{
		// change strip if required
		if(opcd->tpage != stripGT4FMAtextureID_A)
		{
			kmChangeStripTextureSurface(&StripHead_GT4_FMA_Alpha,KM_IMAGE_PARAM1,DCKtextureList[opcd->tpage].surfacePtr);
			stripGT4FMAtextureID_A = opcd->tpage;
		}
		kmStartStrip(&vertexBufferDesc, &StripHead_GT4_FMA_Alpha);
	}	
	else
	{
		// change strip if required
		if(opcd->tpage != stripGT4FMAtextureID)
		{
			kmChangeStripTextureSurface(&StripHead_GT4_FMA,KM_IMAGE_PARAM1,DCKtextureList[opcd->tpage].surfacePtr);		
			stripGT4FMAtextureID = opcd->tpage;
		}
		kmStartStrip(&vertexBufferDesc, &StripHead_GT4_FMA);	
	}	

	kmSetVertex(&vertexBufferDesc, &vertices_GT4_FMA[0], KM_VERTEXTYPE_03, sizeof(KMVERTEX_03));
	kmSetVertex(&vertexBufferDesc, &vertices_GT4_FMA[1], KM_VERTEXTYPE_03, sizeof(KMVERTEX_03));
	kmSetVertex(&vertexBufferDesc, &vertices_GT4_FMA[2], KM_VERTEXTYPE_03, sizeof(KMVERTEX_03));	
	kmSetVertex(&vertexBufferDesc, &vertices_GT4_FMA[3], KM_VERTEXTYPE_03, sizeof(KMVERTEX_03));	
	kmEndStrip(&vertexBufferDesc);
}

void my_setPolyFMA_GT3_Trans(POLY_GT3 *packet, FMA_GT3 *opcd, uchar alpha)
{
	float	uvDivide;
	long 	*tfv;
	long	*tfd;
	float	*uvTable;
	
	tfv = transformedVertices;
	tfd = transformedDepths;

#define GETX(n)( ((SHORTXY *)( (int)(tfv) +(n) ))->x )
#define GETY(n)( ((SHORTXY *)( (int)(tfv) +(n) ))->y )
#define GETD(n)(  *(u_long *)( (int)(tfd) +(n) ) )

//	uvTable	= uvLookUpTable4bpp;
//	if(DCKtextureList[opcd->tpage].bpp == 8)
		uvTable	= uvLookUpTable8bpp;
	
	vertices_GT3_FMA[0].fX = GETX(opcd->vert0);
	vertices_GT3_FMA[0].fY = GETY(opcd->vert0);
	vertices_GT3_FMA[0].u.fZ = 1.0 / ((float)GETD(opcd->vert0));
	vertices_GT3_FMA[0].fU = uvTable[opcd->u0]*4;
	vertices_GT3_FMA[0].fV = uvTable[opcd->v0]*4;
	vertices_GT3_FMA[0].uBaseRGB.dwPacked = RGBA(opcd->r0,opcd->g0,opcd->b0,alpha);

	vertices_GT3_FMA[1].fX = GETX(opcd->vert1);
	vertices_GT3_FMA[1].fY = GETY(opcd->vert1);
	vertices_GT3_FMA[1].u.fZ = 1.0 / ((float)GETD(opcd->vert1));
	vertices_GT3_FMA[1].fU = uvTable[opcd->u1]*4;
	vertices_GT3_FMA[1].fV = uvTable[opcd->v1]*4;
	vertices_GT3_FMA[1].uBaseRGB.dwPacked = RGBA(opcd->r1,opcd->g1,opcd->b1,alpha);

	vertices_GT3_FMA[2].fX = GETX(opcd->vert2);
	vertices_GT3_FMA[2].fY = GETY(opcd->vert2);
	vertices_GT3_FMA[2].u.fZ = 1.0 / ((float)GETD(opcd->vert2));
	vertices_GT3_FMA[2].fU = uvTable[opcd->u2]*4;
	vertices_GT3_FMA[2].fV = uvTable[opcd->v2]*4;
	vertices_GT3_FMA[2].uBaseRGB.dwPacked = RGBA(opcd->r2,opcd->g2,opcd->b2,alpha);

	// check to see if alpha channel is to be used
	if(DCKtextureList[opcd->tpage].colourKey)
	{
		// change strip if required
		if(opcd->tpage != stripGT3FMAtextureID_A)
		{
			kmChangeStripTextureSurface(&StripHead_GT3_FMA_Alpha,KM_IMAGE_PARAM1,DCKtextureList[opcd->tpage].surfacePtr);
			stripGT3FMAtextureID_A = opcd->tpage;
		}
		kmStartStrip(&vertexBufferDesc, &StripHead_GT3_FMA_Alpha);
	}	
	else
	{
		// change strip if required
		if(opcd->tpage != stripGT3FMAtextureID)
		{
			kmChangeStripTextureSurface(&StripHead_GT3_FMA,KM_IMAGE_PARAM1,DCKtextureList[opcd->tpage].surfacePtr);
			stripGT3FMAtextureID = opcd->tpage;
		}
		kmStartStrip(&vertexBufferDesc, &StripHead_GT3_FMA);	
	}	

	kmSetVertex(&vertexBufferDesc, &vertices_GT3_FMA[0], KM_VERTEXTYPE_03, sizeof(KMVERTEX_03));
	kmSetVertex(&vertexBufferDesc, &vertices_GT3_FMA[1], KM_VERTEXTYPE_03, sizeof(KMVERTEX_03));
	kmSetVertex(&vertexBufferDesc, &vertices_GT3_FMA[2], KM_VERTEXTYPE_03, sizeof(KMVERTEX_03));	
	kmEndStrip(&vertexBufferDesc);
}

void my_setPolyFMA_GT4_Trans(POLY_GT4 *packet, FMA_GT4 *opcd, uchar alpha)
{
	float	uvDivide;
	long 	*tfv;
	long	*tfd;
	float	*uvTable;
	
	tfv = transformedVertices;
	tfd = transformedDepths;

#define GETX(n)( ((SHORTXY *)( (int)(tfv) +(n) ))->x )
#define GETY(n)( ((SHORTXY *)( (int)(tfv) +(n) ))->y )
#define GETD(n)(  *(u_long *)( (int)(tfd) +(n) ) )
	
//	uvTable	= uvLookUpTable4bpp;
//	if(DCKtextureList[opcd->tpage].bpp == 8)
		uvTable	= uvLookUpTable8bpp;

	vertices_GT4_FMA_Trans[0].fX = GETX(opcd->vert0);
	vertices_GT4_FMA_Trans[0].fY = GETY(opcd->vert0);
	vertices_GT4_FMA_Trans[0].u.fZ = 1.0 / ((float)GETD(opcd->vert0));
	vertices_GT4_FMA_Trans[0].fU = uvTable[opcd->u0]*4;
	vertices_GT4_FMA_Trans[0].fV = uvTable[opcd->v0]*4;
	vertices_GT4_FMA_Trans[0].uBaseRGB.dwPacked = RGBA(opcd->r0,opcd->g0,opcd->b0,alpha);

	vertices_GT4_FMA_Trans[1].fX = GETX(opcd->vert1);
	vertices_GT4_FMA_Trans[1].fY = GETY(opcd->vert1);
	vertices_GT4_FMA_Trans[1].u.fZ = 1.0 / ((float)GETD(opcd->vert1));
	vertices_GT4_FMA_Trans[1].fU = uvTable[opcd->u1]*4;
	vertices_GT4_FMA_Trans[1].fV = uvTable[opcd->v1]*4;
	vertices_GT4_FMA_Trans[1].uBaseRGB.dwPacked = RGBA(opcd->r1,opcd->g1,opcd->b1,alpha);

	vertices_GT4_FMA_Trans[2].fX = GETX(opcd->vert2);
	vertices_GT4_FMA_Trans[2].fY = GETY(opcd->vert2);
	vertices_GT4_FMA_Trans[2].u.fZ = 1.0 / ((float)GETD(opcd->vert2));
	vertices_GT4_FMA_Trans[2].fU = uvTable[opcd->u2]*4;
	vertices_GT4_FMA_Trans[2].fV = uvTable[opcd->v2]*4;
	vertices_GT4_FMA_Trans[2].uBaseRGB.dwPacked = RGBA(opcd->r2,opcd->g2,opcd->b2,alpha);

	vertices_GT4_FMA_Trans[3].fX = GETX(opcd->vert3);
	vertices_GT4_FMA_Trans[3].fY = GETY(opcd->vert3);
	vertices_GT4_FMA_Trans[3].u.fZ = 1.0 / ((float)GETD(opcd->vert3));
	vertices_GT4_FMA_Trans[3].fU = uvTable[opcd->u3]*4;
	vertices_GT4_FMA_Trans[3].fV = uvTable[opcd->v3]*4;
	vertices_GT4_FMA_Trans[3].uBaseRGB.dwPacked = RGBA(opcd->r3,opcd->g3,opcd->b3,alpha);

	// check to see if alpha channel is to be used
	if(opcd->tpage != stripGT4FMATransTextureID)
	{
		kmChangeStripTextureSurface(&StripHead_GT4_FMA_Trans,KM_IMAGE_PARAM1,DCKtextureList[opcd->tpage].surfacePtr);
		stripGT4FMATransTextureID = opcd->tpage;
	}
	
	kmStartStrip(&vertexBufferDesc, &StripHead_GT4_FMA_Trans);
	kmSetVertex(&vertexBufferDesc, &vertices_GT4_FMA_Trans[0], KM_VERTEXTYPE_03, sizeof(KMVERTEX_03));
	kmSetVertex(&vertexBufferDesc, &vertices_GT4_FMA_Trans[1], KM_VERTEXTYPE_03, sizeof(KMVERTEX_03));
	kmSetVertex(&vertexBufferDesc, &vertices_GT4_FMA_Trans[2], KM_VERTEXTYPE_03, sizeof(KMVERTEX_03));	
	kmSetVertex(&vertexBufferDesc, &vertices_GT4_FMA_Trans[3], KM_VERTEXTYPE_03, sizeof(KMVERTEX_03));	
	kmEndStrip(&vertexBufferDesc);
}
 