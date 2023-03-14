#ifndef _VQANALYSIS_H_
#define _VQANALYSIS_H_


#include "../Image.h"
#include "../VQImage.h"

CImage* GenerateDifferenceImage( CImage* pImage, CVQImage* pVQImage, int nMultiplier = 1 );
CImage* GenerateCodebookIndicesImage( CVQImage* pVQImage );

#endif //_VQANALYSIS_H_
