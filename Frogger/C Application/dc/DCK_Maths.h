/*
	(c) 2000 Interactive Studios Ltd.

	File		: DCK_Maths.h
	Programmer	: Andy Eder
	Created		: 19/01/00

	Updated		: 19/01/00	Andy Eder
				: < revision dates here >

----------------------------------------------------------------------------------------------- */

#ifndef DCK_MATHS_H_INCLUDED
#define DCK_MATHS_H_INCLUDED


//----- [ DEFINES ] -----------------------------------------------------------------------------

#define	pSH4Matrix		((DCKMATRIX *)(((unsigned long)Buffer0+7)&0xFFFFFFF8))
#define	pSH4Vector		((float *)(((unsigned long)Buffer1+7)&0xFFFFFFF8))
#define	pSH4Matrix1		((DCKMATRIX *)(((unsigned long)Buffer2+7)&0xFFFFFFF8))
#define	pSH4Matrix2		((DCKMATRIX *)(((unsigned long)Buffer3+7)&0xFFFFFFF8))
#define	pSH4Vector1		((float *)(((unsigned long)Buffer4+7)&0xFFFFFFF8))
#define	pSH4Vector2		((float *)(((unsigned long)Buffer5+7)&0xFFFFFFF8))

//----- [ GLOBALS ] -----------------------------------------------------------------------------

extern	unsigned long Buffer0[70];
extern	unsigned long Buffer1[10];
extern	unsigned long Buffer2[70];
extern	unsigned long Buffer3[70];
extern	unsigned long Buffer4[10];
extern	unsigned long Buffer5[10];

//----- [ FUNCTION PROTOTYPES ] -----------------------------------------------------------------

// general matrix routines

void DCK_MatrixMakeIdentity(DCKMATRIX *mtx);
void DCK_MatrixScaleByFactor(DCKMATRIX *mtx,const float factor);
void DCK_MatrixMultiply(DCKMATRIX *destMtx,DCKMATRIX *src1Mtx,DCKMATRIX *src2Mtx);

// matrix transformation-related routines

void DCK_MatrixMakeTranslate(DCKMATRIX *mtx,float tX,float tY,float tZ);
void DCK_MatrixMakeScale(DCKMATRIX *mtx,float sX,float sY,float sZ);
void DCK_MatrixMakeXRotate(DCKMATRIX *mtx,float rX);
void DCK_MatrixMakeYRotate(DCKMATRIX *mtx,float rY);
void DCK_MatrixMakeZRotate(DCKMATRIX *mtx,float rZ);

void DCK_MatrixTransformVector(DCKVECTOR *destVec,DCKMATRIX *mtx,DCKVECTOR *srcVec);

#endif
