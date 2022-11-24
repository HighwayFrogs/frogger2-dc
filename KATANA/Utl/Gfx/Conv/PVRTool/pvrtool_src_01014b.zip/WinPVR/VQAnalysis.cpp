/*************************************************
 VQ Analysis
 
   These helper functions generate an image
   based on the VQ compressed image, to assist
   in selecting the correct parameters and
   checking the VQ compression algorithm.
  
**************************************************/


#include <memory.h>
#include <math.h>
#include "VQAnalysis.h"
#include "../Twiddle.h"
#include "../Util.h"

//////////////////////////////////////////////////////////////////////
// Calculate the difference between the two images, multiplied by a given scale factor
//////////////////////////////////////////////////////////////////////
CImage* GenerateDifferenceImage( CImage* pImage, CVQImage* pVQImage, int nMultiplier /*1*/ )
{
    //validate parameters
    if( pImage == NULL || pVQImage == NULL ) return NULL;
    if( pImage->GetWidth() != pVQImage->GetWidth() || pImage->GetHeight() != pVQImage->GetHeight() ) return NULL;

    IndicateLongOperation(true);

    //get image options
    bool bMipMaps = (pImage->GetNumMipMaps() > 1) && (pVQImage->GetNumMipMaps() > 1);
    bool bAlpha = (pImage->HasAlpha() && pVQImage->HasAlpha() );

    //create a new image
    CImage* pRefImage = new CImage();

    MMRGBA* pmmrgba = pRefImage->GetMMRGBA();
    pmmrgba->Init( MMINIT_RGB|MMINIT_ALLOCATE|(bMipMaps?MMINIT_MIPMAP:0)|(bAlpha?MMINIT_ALPHA:0), pImage->GetWidth(), pImage->GetHeight() );

    //loop for each mipmap
    int nMipMaps = min( pImage->GetNumMipMaps(), pVQImage->GetNumMipMaps() );
    int nTempWidth = pImage->GetWidth(), nTempHeight = pImage->GetHeight();
    for( int iMipMap = 0; iMipMap < nMipMaps; iMipMap++ )
    {
        //get pointers to each image
        unsigned char* pRGBImg = pImage->GetMMRGBA()->pRGB[iMipMap];
        unsigned char* pAlphaImg = bAlpha ? pImage->GetMMRGBA()->pAlpha[iMipMap] : NULL;

        unsigned char* pRGBVQ = pVQImage->GetMMRGBA()->pRGB[iMipMap];
        unsigned char* pAlphaVQ = bAlpha ? pVQImage->GetMMRGBA()->pAlpha[iMipMap] : NULL;

        unsigned char* pRGBRef = pmmrgba->pRGB[iMipMap];
        unsigned char* pAlphaRef = bAlpha ? pmmrgba->pAlpha[iMipMap] : NULL;

        //generate image
        int nMax = (nTempWidth == 1) ? 1 : (nTempWidth * nTempHeight), i = 0;
        ImageColourFormat icf = pVQImage->m_icf; 
        if( nTempWidth == 1 ) icf = ICF_565;

        while( i < nMax )
        {
            //read RGBA from both images
            unsigned char r1 = pRGBImg[(i*3)], g1 = pRGBImg[(i*3)+1], b1 = pRGBImg[(i*3)+2], a1 = bAlpha ? pAlphaImg[i] : 0;
            unsigned char r2 = pRGBVQ[(i*3)],  g2 = pRGBVQ[(i*3)+1],  b2 = pRGBVQ[(i*3)+2],  a2 = bAlpha ? pAlphaVQ[i] : 0;

            //convert the source image's RGBA data to 16-bit based on the VQ image's colour format
            if( icf != ICF_YUV422 )
            {
                unsigned short int texel;
                int x = i % nTempWidth;
                ComputeTexel( x, 0, &texel, a1, r1, g1, b1, icf );
                UnpackTexel( x, 0, texel, &a1, &r1, &g1, &b1, icf );           
            }

            //calculate difference
            unsigned char r = Limit255( abs( int(r1) - int(r2) ) * nMultiplier );
            unsigned char g = Limit255( abs( int(g1) - int(g2) ) * nMultiplier );
            unsigned char b = Limit255( abs( int(b1) - int(b2) ) * nMultiplier );
            unsigned char a = Limit255( abs( int(a1) - int(a2) ) * nMultiplier );

            //write out differences
            pRGBRef[ (i*3) ] = r;
            pRGBRef[ (i*3)+1 ] = g;
            pRGBRef[ (i*3)+2 ] = b;
            if( bAlpha ) pAlphaRef[ i ] = a;

            //update index
            i++;
        }

        //update mipmap width & height
        nTempWidth >>= 1;
        nTempHeight >>= 1;
    }

#ifdef _WINDOWS
    pRefImage->CreateAlignedImage();
#endif

    //return the image
    IndicateLongOperation(false);
    return pRefImage;
}











//////////////////////////////////////////////////////////////////////
// Generates an image based on the codebook index usage
//////////////////////////////////////////////////////////////////////
CImage* GenerateCodebookIndicesImage( CVQImage* pVQImage )
{
    //validate parameters
    if( pVQImage == NULL || pVQImage->GetVQ() == NULL ) return NULL;

    IndicateLongOperation(true);

    //get pointer to header and codebook
    unsigned char* pVQ = pVQImage->GetVQ();

    //get the header
    VQFHeader* pHeader = (VQFHeader*)pVQ;
    pVQ += sizeof(VQFHeader);

    //extract infomation from the header
    int nCodeBookSize = pVQImage->GetCodebookSize();
    bool bMipMaps = ( ( pHeader->nMapType & VQF_MAPTYPE_MIPMAPPED ) == VQF_MAPTYPE_MIPMAPPED );

    CImage* pRefImage = new CImage();
    MMRGBA* mmrgba = pRefImage->GetMMRGBA();
    mmrgba->Init( MMINIT_ALLOCATE|MMINIT_RGB|(bMipMaps?MMINIT_MIPMAP:0), pVQImage->GetWidth(), pVQImage->GetHeight() );

    int nNumMipMaps = mmrgba->nMipMaps;

    //get the codebook
    VQFCodeBookEntry* pCodeBook = (VQFCodeBookEntry*)pVQ;
    pVQ += sizeof(VQFCodeBookEntry) * nCodeBookSize;

    //generate the colour map for the codebook representation
    struct { unsigned char r,g,b; } CodebookColourMap[256];
    for( int i = 0; i < 256; i++ )
    {
        CodebookColourMap[i].r = rand() % 256;
        CodebookColourMap[i].g = rand() % 256;
        CodebookColourMap[i].b = rand() % 256;
    }


    //skip 1x1 placeholder byte
    if( bMipMaps ) pVQ++;

    //unpack image
    int iMipMap = bMipMaps ? mmrgba->nMipMaps - 2 : 0;
    int nTempWidth = bMipMaps ? 2 : mmrgba->nWidth;
    int nTempHeight = nTempWidth;
    while( iMipMap >= 0 )
    {
        unsigned char* pRGB = mmrgba->pRGB[iMipMap];

        //compute twiddle bit values
        unsigned long int mask, shift;
        ComputeMaskShift( nTempWidth / 2, nTempHeight / 2, mask, shift );

        //read the image
        int nWrite = 0, nMax = (nTempWidth / 2) * (nTempHeight / 2);
        int x = 0, y = 0;
        while( nWrite < nMax )
        {
            int iPos = CalcUntwiddledPos( x / 2, y / 2, mask, shift );
            unsigned char iCodebook = pVQ[iPos];

            //unpack the twiddled 2x2 block
            bool bDown = true;
            for( int iTexel = 0; iTexel < 4; iTexel++ )
            {
                //determine where to write
                int iWrite = (x + (y * nTempWidth)) * 3;

                //set the pixel for these values
                pRGB[iWrite++] = CodebookColourMap[iCodebook].b;
                pRGB[iWrite++] = CodebookColourMap[iCodebook].g;
                pRGB[iWrite++] = CodebookColourMap[iCodebook].r;

                //adjust x & y so we step through the image in a twiddled way
                if( bDown ) y += 1; else { y -= 1; x += 1; }
                bDown = !bDown;
            }

            //update write position
            nWrite++;
            if( x >= nTempWidth ) { x = 0; y += 2; }
        }

        //move pointer over the mipmap we just unpacked
        pVQ += nMax;

        //update values
        iMipMap--;
        nTempWidth *= 2;
        nTempHeight *= 2;
    }

#ifdef _WINDOWS
    pRefImage->CreateAlignedImage();
#endif

    //return the image
    IndicateLongOperation(false);
    return pRefImage;
}