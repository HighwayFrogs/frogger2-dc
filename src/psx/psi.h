

#ifndef __PSITYPES_H__
#define __PSITYPES_H__

#define GPU_COM_TF4SPR (0x64)

//#define YES		1
//#define TRUE	1
//#define ON		1

//#define NO		0
//#define FALSE	0
//#define OFF		0

typedef struct TAGRGBCD
{
	unsigned char r, g, b, code;
} RGBCD;

////////////////////////////////////////////////////////////////////////

//	prim 			struct	 sony type		 prim type			
// POLY_F3;		TMD_P_FG3I	GPU_COM_F3 	   Flat Triangle 			
// POLY_F4;	   	TMD_P_FG4I	GPU_COM_F4	   Flat Quadrangle 
// POLY_G3;		TMD_P_FG3I	GPU_COM_G3 	   Gouraud Triangle 
// POLY_G4;	   	TMD_P_FG4I	GPU_COM_G4	   Gouraud Quadrangle 

// POLY_FT3;	TMD_P_FT3I	GPU_COM_TF3	   Flat Textured Triangle 
// POLY_FT4;	TMD_P_FT4I	GPU_COM_TF4	   Flat Textured Quadrangle 
// POLY_GT3;	TMD_P_GT3I	GPU_COM_TG3	   Gouraud Textured Triangle 
// POLY_GT4;	TMD_P_GT4I	GPU_COM_TG4	   Gouraud Textured Quadrangle 
							

typedef struct {
	long	next;
	unsigned char	out, in, dummy, cd;
	unsigned char	r0, g0, b0, code;
	unsigned char	r1, g1, b1, dummy1;
	unsigned char	r2, g2, b2, dummy2;
	unsigned short n0, v0;
	unsigned short v1, v2;
	int	padto8;
}TMD_P_FG3I;

typedef struct {
	long	next;
	unsigned char	out, in, dummy, cd;
	unsigned char	r0, g0, b0, code;
	unsigned char	r1, g1, b1, dummy1;
	unsigned char	r2, g2, b2, dummy2;
	unsigned char	r3, g3, b3, dummy3;
	unsigned short n0, v0;
	unsigned short v1, v2;
	unsigned short v3, dummy4;
	int	padto8;
}TMD_P_FG4I;

typedef struct {
	long	next;
	unsigned char  out, in, dummy, cd;
	unsigned char  tu0, tv0;
	unsigned short clut;
	unsigned char  tu1, tv1;
	unsigned short tpage;
	unsigned char  tu2, tv2;
	unsigned char  tile,p;
	unsigned char  r0, g0, b0, code;
	unsigned short n0, v0;
	unsigned short v1, v2;
} TMD_P_FT3I;


typedef struct {
	long  next;
	unsigned char  out, in, dummy, cd;
	unsigned char  tu0, tv0;			
	unsigned short clut;
	unsigned char  tu1, tv1;
	unsigned short tpage;
	unsigned char  tu2, tv2;
	unsigned char  tile,p0;
	unsigned char  tu3, tv3;
	unsigned short p1;
	unsigned char  r0, g0, b0, code;
	
	unsigned short v0;
	unsigned short v1, v2;
	unsigned short v3;
	unsigned short	n0;
	unsigned short	padto8;
}TMD_P_FT4I;


typedef struct {
	long	next;
	unsigned char  out, in, dummy, cd;

	unsigned char  tu0, tv0;
	unsigned short clut;

	unsigned char  tu1, tv1;
	unsigned short tpage;

	unsigned char  tu2, tv2;
	unsigned short v0;

	unsigned char  r0, g0, b0, code;
	unsigned char  r1, g1, b1, tile;	  
	unsigned char  r2, g2, b2, code2;
	unsigned short v1;
	unsigned short v2;
	RGBCD avgTexCol;
}TMD_P_GT3I;

typedef struct {
	long next;
	unsigned char  out, in, dummy, cd;

	unsigned char  tu0, tv0;
	unsigned short clut;

	unsigned char  tu1, tv1;
	unsigned short tpage;

	unsigned char  tu2, tv2;
	unsigned short v0;

	unsigned char  tu3, tv3;
	unsigned short v1;

	unsigned char  r0, g0, b0, code;
	unsigned char  r1, g1, b1, tile;
	unsigned char  r2, g2, b2, code2;
	unsigned char  r3, g3, b3, code3;
	unsigned short v2, v3;
	RGBCD avgTexCol;
}TMD_P_GT4I;

//#define NUMOFSIDES 		3	   //NB 0=SPRITE 1=LINE 2=TRI 3=QUAD
//#define TEXTURED   		4
#define psiDOUBLESIDED 	1
//#define LIT	 			16
//#define TILEDH			32
//#define TILEDV			64
#define psiTRANSPAR		2
//#define GOURAUD     	512


#endif //__PSITYPES_H__