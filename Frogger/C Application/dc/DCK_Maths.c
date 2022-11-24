/*
	(c) 2000 Interactive Studios Ltd.

	File		: DCK_Maths.c
	Programmer	: Andy Eder
	Created		: 19/01/00

	Updated		: 19/01/00	Andy Eder
				: < revision dates here >

----------------------------------------------------------------------------------------------- */

#include <shinobi.h>
#include <kamui2.h>
#include <sg_syhw.h>
#include <sg_sycbl.h>
#include <machine.h>

#include "types.h"
#include "DCK_Types.h"
#include "DCK_Maths.h"


//----- [ GLOBALS ] -----------------------------------------------------------------------------

//  aligned globals for SH4 routines

unsigned long Buffer0[70];
unsigned long Buffer1[10];
unsigned long Buffer2[70];
unsigned long Buffer3[70];
unsigned long Buffer4[10];
unsigned long Buffer5[10];

//----- [ FUNCTION IMPLEMENTATION ] --------------------------------------------------------------

/*	--------------------------------------------------------------------------------
	Function		: DCK_MatrixMakeIdentity
	Purpose			: makes the specified matrix an identity matrix
	Parameters		: DCMATRIX *
	Returns			: void
	Info			: 
*/
void DCK_MatrixMakeIdentity(DCKMATRIX *mtx)
{
	mtx->_11 = 1.0f;		mtx->_12 = 0.0f;		mtx->_13 = 0.0f;		mtx->_14 = 0.0f;
	mtx->_21 = 0.0f;		mtx->_22 = 1.0f;		mtx->_23 = 0.0f;		mtx->_24 = 0.0f;
	mtx->_31 = 0.0f;		mtx->_32 = 0.0f;		mtx->_33 = 1.0f;		mtx->_34 = 0.0f;
	mtx->_41 = 0.0f;		mtx->_42 = 0.0f;		mtx->_43 = 0.0f;		mtx->_44 = 1.0f;
}


/*	--------------------------------------------------------------------------------
	Function		: DCK_MatrixScaleByFactor
	Purpose			: multiplies the elements of matrix 'mtx' by factor
	Parameters		: DCKMATRIX *,const float
	Returns			: void
	Info			: 
*/
void DCK_MatrixScaleByFactor(DCKMATRIX *mtx,const float factor)
{
	mtx->_11 *= factor;		mtx->_12 *= factor;		mtx->_13 *= factor;		mtx->_14 *= factor;
	mtx->_21 *= factor;		mtx->_22 *= factor;		mtx->_23 *= factor;		mtx->_24 *= factor;
	mtx->_31 *= factor;		mtx->_32 *= factor;		mtx->_33 *= factor;		mtx->_34 *= factor;
	mtx->_41 *= factor;		mtx->_42 *= factor;		mtx->_43 *= factor;		mtx->_44 *= factor;
}


/*	--------------------------------------------------------------------------------
	Function		: DCK_MatrixMultiply
	Purpose			: multiplies two matrices
	Parameters		: DCKMATRIX *,DCKMATRIX *,DCKMATRIX *
	Returns			: void
	Info			: 
*/
void DCK_MatrixMultiply(DCKMATRIX *destMtx,DCKMATRIX *src1Mtx,DCKMATRIX *src2Mtx)
{
	// matrix 1
	pSH4Matrix1->_11 = src1Mtx->_11;	
	pSH4Matrix1->_12 = src1Mtx->_12;
	pSH4Matrix1->_13 = src1Mtx->_13;
	pSH4Matrix1->_14 = src1Mtx->_14;

	pSH4Matrix1->_21 = src1Mtx->_21;
	pSH4Matrix1->_22 = src1Mtx->_22;
	pSH4Matrix1->_23 = src1Mtx->_23;
	pSH4Matrix1->_24 = src1Mtx->_24;

	pSH4Matrix1->_31 = src1Mtx->_31;
	pSH4Matrix1->_32 = src1Mtx->_32;
	pSH4Matrix1->_33 = src1Mtx->_33;
	pSH4Matrix1->_34 = src1Mtx->_34;

	pSH4Matrix1->_41 = src1Mtx->_41;
	pSH4Matrix1->_42 = src1Mtx->_42;
	pSH4Matrix1->_43 = src1Mtx->_43;
	pSH4Matrix1->_44 = src1Mtx->_44;

	// matrix 2
	pSH4Matrix2->_11 = src2Mtx->_11;
	pSH4Matrix2->_12 = src2Mtx->_21;
	pSH4Matrix2->_13 = src2Mtx->_31;
	pSH4Matrix2->_14 = src2Mtx->_41;

	pSH4Matrix2->_21 = src2Mtx->_12;
	pSH4Matrix2->_22 = src2Mtx->_22;
	pSH4Matrix2->_23 = src2Mtx->_32;
	pSH4Matrix2->_24 = src2Mtx->_42;

	pSH4Matrix2->_31 = src2Mtx->_13;
	pSH4Matrix2->_32 = src2Mtx->_23;
	pSH4Matrix2->_33 = src2Mtx->_33;
	pSH4Matrix2->_34 = src2Mtx->_43;

	pSH4Matrix2->_41 = src2Mtx->_14;
	pSH4Matrix2->_42 = src2Mtx->_24;
	pSH4Matrix2->_43 = src2Mtx->_34;
	pSH4Matrix2->_44 = src2Mtx->_44;
	
    // can use dot product instead of SH4 Matrix Multiplication - mtrx4mul(pSH4Matrix1,pSH4Matrix2);

	// resulting matrix
	destMtx->_11 = fipr(&pSH4Matrix1->_11,&pSH4Matrix2->_11);
	destMtx->_12 = fipr(&pSH4Matrix1->_11,&pSH4Matrix2->_21);
	destMtx->_13 = fipr(&pSH4Matrix1->_11,&pSH4Matrix2->_31);
	destMtx->_14 = fipr(&pSH4Matrix1->_11,&pSH4Matrix2->_41);

	destMtx->_21 = fipr(&pSH4Matrix1->_21,&pSH4Matrix2->_11);
	destMtx->_22 = fipr(&pSH4Matrix1->_21,&pSH4Matrix2->_21);
	destMtx->_23 = fipr(&pSH4Matrix1->_21,&pSH4Matrix2->_31);
	destMtx->_24 = fipr(&pSH4Matrix1->_21,&pSH4Matrix2->_41);

	destMtx->_31 = fipr(&pSH4Matrix1->_31,&pSH4Matrix2->_11);
	destMtx->_32 = fipr(&pSH4Matrix1->_31,&pSH4Matrix2->_21);
	destMtx->_33 = fipr(&pSH4Matrix1->_31,&pSH4Matrix2->_31);
	destMtx->_34 = fipr(&pSH4Matrix1->_31,&pSH4Matrix2->_41);

	destMtx->_41 = fipr(&pSH4Matrix1->_41,&pSH4Matrix2->_11);
	destMtx->_42 = fipr(&pSH4Matrix1->_41,&pSH4Matrix2->_21);
	destMtx->_43 = fipr(&pSH4Matrix1->_41,&pSH4Matrix2->_31);
	destMtx->_44 = fipr(&pSH4Matrix1->_41,&pSH4Matrix2->_41);
}


/*	--------------------------------------------------------------------------------
	Function		: DCK_MatrixMakeTranslate
	Purpose			: makes the specified matrix a translation matrix
	Parameters		: DCKMATRIX *,float,float,float
	Returns			: void
	Info			: 
*/
void DCK_MatrixMakeTranslate(DCKMATRIX *mtx,float tX,float tY,float tZ)
{
	mtx->_11 = 1.0f;		mtx->_12 = 0.0f;		mtx->_13 = 0.0f;		mtx->_14 = 0.0f;
	mtx->_21 = 0.0f;		mtx->_22 = 1.0f;		mtx->_23 = 0.0f;		mtx->_24 = 0.0f;
	mtx->_31 = 0.0f;		mtx->_32 = 0.0f;		mtx->_33 = 1.0f;		mtx->_34 = 0.0f;
	mtx->_41 = tX;			mtx->_42 = tY;			mtx->_43 = tZ;			mtx->_44 = 1.0f;
}


/*	--------------------------------------------------------------------------------
	Function		: DCK_MatrixMakeScale
	Purpose			: makes the specified matrix a scale matrix
	Parameters		: DCKMATRIX *,float,float,float
	Returns			: void
	Info			: 
*/
void DCK_MatrixMakeScale(DCKMATRIX *mtx,float sX,float sY,float sZ)
{
	mtx->_11 = sX;			mtx->_12 = 0.0f;		mtx->_13 = 0.0f;		mtx->_14 = 0.0f;
	mtx->_21 = 0.0f;		mtx->_22 = sY;			mtx->_23 = 0.0f;		mtx->_24 = 0.0f;
	mtx->_31 = 0.0f;		mtx->_32 = 0.0f;		mtx->_33 = sZ;			mtx->_34 = 0.0f;
	mtx->_41 = 0.0f;		mtx->_42 = 0.0f;		mtx->_43 = 0.0f;		mtx->_44 = 1.0f;
}


/*	--------------------------------------------------------------------------------
	Function		: DCK_MatrixMakeXRotate
	Purpose			: makes the specified matrix a rotation matrix about the x-axis
	Parameters		: DCKMATRIX *,float
	Returns			: void
	Info			: 
*/
void DCK_MatrixMakeXRotate(DCKMATRIX *mtx,float rX)
{
	float sine 		= sinf(rX);
	float cosine	= cosf(rX);
	
	mtx->_11 = 1.0f;		mtx->_12 = 0.0f;		mtx->_13 = 0.0f;		mtx->_14 = 0.0f;
	mtx->_21 = 0.0f;		mtx->_22 = cosine;		mtx->_23 = sine;		mtx->_24 = 0.0f;
	mtx->_31 = 0.0f;		mtx->_32 = -sine;		mtx->_33 = cosine;		mtx->_34 = 0.0f;
	mtx->_41 = 0.0f;		mtx->_42 = 0.0f;		mtx->_43 = 0.0f;		mtx->_44 = 1.0f;
}


/*	--------------------------------------------------------------------------------
	Function		: DCK_MatrixMakeYRotate
	Purpose			: makes the specified matrix a rotation matrix about the y-axis
	Parameters		: DCKMATRIX *,float
	Returns			: void
	Info			: 
*/
void DCK_MatrixMakeYRotate(DCKMATRIX *mtx,float rY)
{
	float sine 		= sinf(rY);
	float cosine	= cosf(rY);
	
	mtx->_11 = cosine;		mtx->_12 = 0.0f;		mtx->_13 = -sine;		mtx->_14 = 0.0f;
	mtx->_21 = 0.0f;		mtx->_22 = 1.0f;		mtx->_23 = 0.0f;		mtx->_24 = 0.0f;
	mtx->_31 = sine;		mtx->_32 = 0.0f;		mtx->_33 = cosine;		mtx->_34 = 0.0f;
	mtx->_41 = 0.0f;		mtx->_42 = 0.0f;		mtx->_43 = 0.0f;		mtx->_44 = 1.0f;
}


/*	--------------------------------------------------------------------------------
	Function		: DCK_MatrixMakeZRotate
	Purpose			: makes the specified matrix a rotation matrix about the z-axis
	Parameters		: DCKMATRIX *,float
	Returns			: void
	Info			: 
*/
void DCK_MatrixMakeZRotate(DCKMATRIX *mtx,float rZ)
{
	float sine 		= sinf(rZ);
	float cosine	= cosf(rZ);
	
	mtx->_11 = cosine;		mtx->_12 = sine;		mtx->_13 = 0.0f;		mtx->_14 = 0.0f;
	mtx->_21 = -sine;		mtx->_22 = cosine;		mtx->_23 = 0.0f;		mtx->_24 = 0.0f;
	mtx->_31 = 0.0f;		mtx->_32 = 0.0f;		mtx->_33 = 1.0f;		mtx->_34 = 0.0f;
	mtx->_41 = 0.0f;		mtx->_42 = 0.0f;		mtx->_43 = 0.0f;		mtx->_44 = 1.0f;
}


/*	--------------------------------------------------------------------------------
	Function		: DCK_MatrixTransformVector
	Purpose			: transforms the specified vector using the given world transformation matrix
	Parameters		: DCKVECTOR *,DCKMATRIX *,DCKVECTOR *
	Returns			: void
	Info			: destVec receives the transformed vector
*/
void DCK_MatrixTransformVector(DCKVECTOR *destVec,DCKMATRIX *mtx,DCKVECTOR *srcVec)
{
	register i;

	// ready the matrix for Hitachi builtin routines
	// change order to get matrix in correct format for vector operations
	
//	destVec->v[X] = (srcVec->v[X] * (mtx->_11)) + (srcVec->v[Y] * (mtx->_21))  + (srcVec->v[Z] * (mtx->_31)) + mtx->_41;
//	destVec->v[Y] = (srcVec->v[X] * (mtx->_12)) + (srcVec->v[Y] * (mtx->_22))  + (srcVec->v[Z] * (mtx->_32)) + mtx->_42;
//	destVec->v[Z] = (srcVec->v[X] * (mtx->_13)) + (srcVec->v[Y] * (mtx->_23))  + (srcVec->v[Z] * (mtx->_33)) + mtx->_43;
		
	pSH4Matrix1->_11 = mtx->_11;
	pSH4Matrix1->_12 = mtx->_21;
	pSH4Matrix1->_13 = mtx->_31;
	pSH4Matrix1->_14 = mtx->_41;

	pSH4Matrix1->_21 = mtx->_12;
	pSH4Matrix1->_22 = mtx->_22;
	pSH4Matrix1->_23 = mtx->_32;
	pSH4Matrix1->_24 = mtx->_42;

	pSH4Matrix1->_31 = mtx->_13;		// should be 3 or 4
	pSH4Matrix1->_32 = mtx->_23;
	pSH4Matrix1->_33 = mtx->_33;
	pSH4Matrix1->_34 = mtx->_43;

	// transform vertex with world matrix
	pSH4Vector1[0] = srcVec->v[X];
	pSH4Vector1[1] = srcVec->v[Y];
	pSH4Vector1[2] = srcVec->v[Z];
	pSH4Vector1[3] = 1.0f;

	// fipr calculates the dot product (use 32-bit aligned vars)
	destVec->v[X] = fipr(pSH4Vector1,&pSH4Matrix1->_11);
	destVec->v[Y] = fipr(pSH4Vector1,&pSH4Matrix1->_21);
	destVec->v[Z] = fipr(pSH4Vector1,&pSH4Matrix1->_31);

}



