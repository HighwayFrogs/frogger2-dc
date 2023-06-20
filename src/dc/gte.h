/*
	This file is part of Frogger II, Copyright 2000 Interactive Studios Ltd

	File        : gte.h
	Description : Gte playstation emulaution routines and structures
	Date        : 09/03/2000
	Version     : 0.01
*/

#include "include.h"

#ifndef __GTE_H__
#define __GTE_H__

#define GPU_COM_F3    0x20
#define GPU_COM_TF3   0x24
#define GPU_COM_G3    0x30
#define GPU_COM_TG3   0x34

#define GPU_COM_F4    0x28
#define GPU_COM_TF4   0x2c
#define GPU_COM_G4    0x38
#define GPU_COM_TG4   0x3c

#define GPU_COM_NF3   0x21
#define GPU_COM_NTF3  0x25
#define GPU_COM_NG3   0x31
#define GPU_COM_NTG3  0x35

#define GPU_COM_NF4   0x29
#define GPU_COM_NTF4  0x2d
#define GPU_COM_NG4   0x39
#define GPU_COM_NTG4  0x3d

typedef struct
{
	SHORT	m[3][3];
	LONG	t[3];
}MATRIX;

typedef struct
{
	unsigned char	r,g,b,cd;
}CVECTOR;

typedef struct
{
	SHORT	vx,vy;
}DVECTOR;

typedef struct
{
	long			vx,vy,vz;
	unsigned char 	r,g,b;
}GsF_LIGHT;

typedef struct
{
	VECTOR	scale;
	SVECTOR	rotate;
	VECTOR	trans;
}GsCOORD2PARAM;

typedef struct _GsCOORDINATE2
{
	unsigned long			flg;
	MATRIX					coord;
	MATRIX					workm;
	GsCOORD2PARAM			*param;
	struct GsCOORDINATE2	*super;
	struct GsCOORDINATE2	*sub;	
}GsCOORDINATE2;

// -------
// Globals

extern MATRIX	GsIDMATRIX;
extern MATRIX	GsWSMATRIX;

extern SVECTOR vr0;
extern SVECTOR vr1;
extern SVECTOR vr2;

extern CVECTOR cr;
extern CVECTOR cr0;
extern CVECTOR cr1;
extern CVECTOR cr2;

extern short cbk[3];

extern unsigned char lcr0[3];
extern unsigned char lcr1[3];
extern unsigned char lcr2[3];

extern SVECTOR screenxy[4];

extern long opz;

extern long otz;

extern long dc;

extern int dqa;

extern int dqb;

extern int GSh;
extern int GSx;
extern int GSy;

// *ASL* 26/07/2000 - Screen geometry floats
extern float fGSx;
extern float fGSy;
extern float fGSh;
extern float fGShHalf;
extern float fGShLimit;

// *ASL* 11/08/2000 - Transformation floats
extern float fTransVector[3];
extern float fRotMatrix[3][3];

extern MATRIX RotMatrix;

extern short ls0[3];
extern short ls1[3];
extern short ls2[3];

extern int LMode;
extern VECTOR TransVector;
extern VECTOR farc;
extern SVECTOR sv;
extern VECTOR lv;

// strip stuff
extern KMSTRIPCONTEXT	StripContext_G3;
extern KMSTRIPHEAD		StripHead_G3;
extern KMVERTEX_00		vertices_G3[];

extern KMSTRIPCONTEXT	StripContext_G4;
extern KMSTRIPHEAD		StripHead_G4;
extern KMVERTEX_00		vertices_G4[];

extern KMSTRIPCONTEXT	StripContext_FT3;
extern KMSTRIPHEAD		StripHead_FT3;
extern KMSTRIPCONTEXT	StripContext_FT3_Alpha;
extern KMSTRIPHEAD		StripHead_FT3_Alpha;
extern KMVERTEX_03		vertices_FT3[];

extern KMSTRIPCONTEXT	StripContext_FT4;
extern KMSTRIPHEAD		StripHead_FT4;
extern KMSTRIPCONTEXT	StripContext_FT4_Alpha;
extern KMSTRIPHEAD		StripHead_FT4_Alpha;
extern KMVERTEX_03		vertices_FT4[];

extern KMSTRIPCONTEXT	StripContext_GT3;
extern KMSTRIPHEAD		StripHead_GT3;
extern KMSTRIPCONTEXT	StripContext_GT3_Alpha;
extern KMSTRIPHEAD		StripHead_GT3_Alpha;
extern KMVERTEX_03		vertices_GT3[];

extern KMSTRIPCONTEXT	StripContext_GT4;
extern KMSTRIPHEAD		StripHead_GT4;
extern KMSTRIPCONTEXT	StripContext_GT4_Alpha;
extern KMSTRIPHEAD		StripHead_GT4_Alpha;
extern KMVERTEX_03		vertices_GT4[];

extern KMSTRIPCONTEXT	StripContext_TF4SPR;
extern KMSTRIPHEAD		StripHead_TF4SPR;
extern KMSTRIPCONTEXT	StripContext_TF4SPR_Alpha;
extern KMSTRIPHEAD		StripHead_TF4SPR_Alpha;
extern KMVERTEX_03		vertices_TF4SPR[];

extern KMSTRIPCONTEXT	StripContext_GT3_FMA;
extern KMSTRIPHEAD		StripHead_GT3_FMA;
extern KMSTRIPCONTEXT	StripContext_GT3_FMA_Alpha;
extern KMSTRIPHEAD		StripHead_GT3_FMA_Alpha;
extern KMVERTEX_03		vertices_GT3_FMA[];

extern KMSTRIPCONTEXT	StripContext_GT4_FMA;
extern KMSTRIPHEAD		StripHead_GT4_FMA;
extern KMSTRIPCONTEXT	StripContext_GT4_FMA_Alpha;
extern KMSTRIPHEAD		StripHead_GT4_FMA_Alpha;
extern KMSTRIPCONTEXT	StripContext_GT4_FMA_Add;
extern KMSTRIPHEAD		StripHead_GT4_FMA_Add;
extern KMVERTEX_03		vertices_GT4_FMA[];

// *ASL* 07/08/2000 - Vertex4 strips
extern KMSTRIPHEAD		StripHead_GT4_FMA_Vertex4;
extern KMSTRIPHEAD		StripHead_GT4_FMA_Alpha_Vertex4;
extern KMVERTEX_04		vertices_GT4_FMA_Vertex4[];

extern KMSTRIPCONTEXT	StripContext_GT3_FMA_Trans;
extern KMSTRIPHEAD		StripHead_GT3_FMA_Trans;
extern KMVERTEX_03		vertices_GT3_FMA_Trans[];
extern KMSTRIPCONTEXT	StripContext_GT4_FMA_Trans;
extern KMSTRIPHEAD		StripHead_GT4_FMA_Trans;
extern KMVERTEX_03		vertices_GT4_FMA_Trans[];

extern KMSTRIPCONTEXT	StripContext_Sprites;
extern KMSTRIPHEAD		StripHead_Sprites;
extern KMSTRIPCONTEXT	StripContext_Sprites_Add;
extern KMSTRIPHEAD		StripHead_Sprites_Add;
extern KMSTRIPCONTEXT	StripContext_Sprites_Sub;
extern KMSTRIPHEAD		StripHead_Sprites_Sub;
extern KMVERTEX_03		vertices_Sprites[];

extern KMVERTEX_03		vertices_Sprites_Add[];

extern KMSTRIPCONTEXT	StripContext_SpritesNoTex;
extern KMSTRIPHEAD		StripHead_SpritesNoTex;
extern KMSTRIPCONTEXT	StripContext_SpritesNoTex_Sub;
extern KMSTRIPHEAD		StripHead_SpritesNoTex_Sub;
extern KMVERTEX_00		vertices_SpritesNoTex[];

extern KMSTRIPCONTEXT	StripContext_3DSprites;
extern KMSTRIPHEAD		StripHead_3DSprites;
extern KMVERTEX_03		vertices_3DSprites[];

extern KMSTRIPCONTEXT	StripContext_3DBackdrop;
extern KMSTRIPHEAD		StripHead_3DBackdrop;

extern int	stripFT3textureID;
extern int	stripFT3textureID_A;
extern int	stripFT3dsided;
extern int	stripFT3dsided_A;

extern int	stripFT4textureID;
extern int	stripFT4textureID_A;
extern int	stripFT4dsided;
extern int	stripFT4dsided_A;

extern int	stripGT3textureID;
extern int	stripGT3textureID_A;
extern int	stripGT3dsided;
extern int	stripGT3dsided_A;

extern int	stripGT4textureID;
extern int	stripGT4textureID_A;
extern int	stripGT4dsided;
extern int	stripGT4dsided_A;

extern int	stripGT3FMAtextureID;
extern int	stripGT3FMAtextureID_A;

extern int	stripGT4FMAtextureID;
extern int	stripGT4FMAtextureID_A;
extern int	stripGT4FMAtextureID_Add;

extern int	stripTF4SPRtextureID;
extern int	stripTF4SPRtextureID_A;

extern int	stripGT3FMATransTextureID;
extern int	stripGT4FMATransTextureID;

extern int	strip3DBackdropTextureID;

extern float	uvLookUpTable4bpp[32];
extern float	uvLookUpTable8bpp[128];

MATRIX* ScaleMatrix(MATRIX *m, VECTOR *v);
void gte_MulMatrix0(MATRIX *m0, MATRIX *m1, MATRIX *m2);

//void setPolyGT3(POLY_FT4 *packet);

#endif
