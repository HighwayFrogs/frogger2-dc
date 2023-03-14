/*************************************************
 TGA file loading

  Note: These routines have not been tested on
  1 BPP images.

**************************************************/
#pragma pack( push, 1 )
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <windows.h>
#include "TGA.h"
#include "Picture.h"
#include "Util.h"

//constants for TGAHeader::ColourMapType
#define TGA_COLOURMAPTYPE_NONE     (0)
#define TGA_COLOURMAPTYPE_INCLUDED (1)

//constants for TGAHeader::ImageType
#define TGA_IMAGETYPE_NONE                    (0)
#define TGA_IMAGETYPE_UNCOMPRESSED_COLOURMAP  (1)
#define TGA_IMAGETYPE_UNCOMPRESSED_TRUECOLOUR (2)
#define TGA_IMAGETYPE_UNCOMPRESSED_BLACKWHITE (3)
#define TGA_IMAGETYPE_RLE_COLOURMAP           (9)
#define TGA_IMAGETYPE_RLE_TRUECOLOUR          (10)
#define TGA_IMAGETYPE_RLE_BLACKWHITE          (11)

struct TGAHeader 
{
    unsigned char IDLength;
    unsigned char ColourMapType; //0 = no colour map, 1 = map included
    unsigned char ImageType;

    unsigned short int ColourMap_FirstIndex;
    unsigned short int ColourMap_Length;
    unsigned char ColourMap_EntrySize;

    unsigned short int Image_XOrigin, Image_YOrigin;
    unsigned short int Image_Width, Image_Height;
    unsigned char Image_BPP;

    unsigned char Image_Descriptor; //bits 3-0 = alpha bpp??  5-4 = pixel order [00 = bottomleft 01 = bottomright 10 = topleft 11 = topright] 76=unused(00)
};



//////////////////////////////////////////////////////////////////////
// Loads the given TGA file into the given mmrgba object
//////////////////////////////////////////////////////////////////////
bool LoadTGA( const char* pszFilename, MMRGBA &mmrgba, unsigned long int dwFlags )
{
    /* load the image file into a buffer */

    //open the file
    FILE* file = fopen( pszFilename, "rb" );
    if( file == NULL ) return false;

    //load the file into a buffer
    fseek( file, 0, SEEK_END ); int nFileLength = ftell( file ); fseek( file, 0, SEEK_SET );
    unsigned char* pBuffer = (unsigned char*)malloc( nFileLength );
    if( (int)fread( pBuffer, 1, nFileLength, file ) < nFileLength ) { fclose(file); free(pBuffer); return false; };
    fclose(file);

    unsigned char* pPtr = pBuffer;

    //load in the header
    TGAHeader* pHeader = (TGAHeader*)pPtr;
    pPtr += sizeof(TGAHeader);

    //determine what type of image it is
    bool bColourMapped = false, bRLE = false;
    switch( pHeader->ImageType )
    {
        case TGA_IMAGETYPE_NONE: free( pBuffer ); return false; //who'd create an image with no image eh?
        default:
            free( pBuffer );
            return false;

        case TGA_IMAGETYPE_RLE_BLACKWHITE:
            bRLE = true;
        case TGA_IMAGETYPE_UNCOMPRESSED_BLACKWHITE:
            if( MessageBox( NULL, "Warning: black and white targa loading code has not been tested!\nContinue?", "TGA", MB_TASKMODAL|MB_ICONEXCLAMATION|MB_YESNO ) == IDNO )
            {
                free( pBuffer );
                return false;
            }
            break;

        case TGA_IMAGETYPE_RLE_COLOURMAP:
            bRLE = true;
        case TGA_IMAGETYPE_UNCOMPRESSED_COLOURMAP:
            bColourMapped = true;
            break;

        case TGA_IMAGETYPE_RLE_TRUECOLOUR:
            bRLE = true;
        case TGA_IMAGETYPE_UNCOMPRESSED_TRUECOLOUR:
            break;
    }

    //skip over image identification field
    if( pHeader->IDLength > 0 ) pPtr += pHeader->IDLength;

    //load the colour map
    unsigned char* pColourMap = NULL;
    int nColourMapEntrySize = 0;
    if( pHeader->ColourMapType != TGA_COLOURMAPTYPE_NONE )
    {
        nColourMapEntrySize = /*min(*/pHeader->ColourMap_EntrySize/8;//, 8);
        pColourMap = pPtr;
        pPtr += nColourMapEntrySize * pHeader->ColourMap_Length;
    }

    //calculate number of bits per pixel, including alpha
    int nAlphaBits = pHeader->Image_Descriptor & 15;

    //load the image data
    int nImageSize = pHeader->Image_Width * pHeader->Image_Height * (pHeader->Image_BPP / 8);

    /* process the image into RGB and A channels */

    //process image data
    mmrgba.Init( MMINIT_ALLOCATE|MMINIT_RGB|(( nAlphaBits && (dwFlags & LPF_LOADALPHA) )?MMINIT_ALPHA:0), pHeader->Image_Width, pHeader->Image_Height );

    //determine image orientation
    unsigned char imageOrientation = (pHeader->Image_Descriptor & 48) >> 4;
    bool bFlipHorizontal = ((imageOrientation & 1) == 1), bFlipVertical = ((imageOrientation & 2)==0);

    int iRead = 0, iWrite = 0, iMax = pHeader->Image_Width * pHeader->Image_Height;
    int x = bFlipHorizontal ? (pHeader->Image_Width  - 1) : 0;
    int y = bFlipVertical   ? (pHeader->Image_Height - 1) : 0;
    while( iWrite < iMax )
    {
        //default to 1 run of 1 pixels - just like non-RLE image
        unsigned char nRunCount = 1, nPixelRun = 1;

        //set run length for RLE image
        if( bRLE )
        {
            unsigned char nRepetitionCount = pPtr[iRead++];
            if( (nRepetitionCount & 0x80) == 0x80 )
                nRunCount = (nRepetitionCount & 0x7F) + 1;
            else
                nPixelRun = (nRepetitionCount & 0x7F) + 1;
        }

        //loop for each pixel in the raw data run, or just execute once for non RLE
        for( int iPixel = 0; iPixel < nPixelRun; iPixel++ )
        {
            unsigned char r, g, b, a;

            //get the colour values
            if( bColourMapped )
            {
                int iColourMap = (pHeader->ColourMap_FirstIndex + pPtr[iRead++]) * nColourMapEntrySize;
                switch( pHeader->ColourMap_EntrySize / 8 )
                {
                    case 2: //16-bit 1555
                        r = ( (*((unsigned short int *)&pColourMap[iColourMap])) >> 10 ) & 0x1F;
                        g = ( (*((unsigned short int *)&pColourMap[iColourMap])) >> 5 ) & 0x1F;
                        b = ( (*((unsigned short int *)&pColourMap[iColourMap])) ) & 0x1F;
                        a = ( (*((unsigned short int *)&pColourMap[iColourMap])) >> 15 ) & 1;
                        break;

                    case 4: //32-bit
                        a = pColourMap[iColourMap+3];
                        //drop through...
                    case 3: //24-bit
                        b = pColourMap[iColourMap];
                        g = pColourMap[iColourMap+1];
                        r = pColourMap[iColourMap+2];
                        break;
                }
            }
            else
            {
                switch( pHeader->Image_BPP / 8 )
                {
                    case 1:
                        //UNTESTED:
                        r = g = b = pPtr[iRead++];
                        break; 

                    case 2: //16-bit 1555
                    {
                        r = 8 *   ((*((unsigned short int *)&pPtr[iRead]) >> 10 ) & 0x1F );
                        g = 8 *   ((*((unsigned short int *)&pPtr[iRead]) >> 5 ) & 0x1F );
                        b = 8 *   ((*((unsigned short int *)&pPtr[iRead]) ) & 0x1F );
                        a = 255 * ((*((unsigned short int *)&pPtr[iRead]) >> 15 ) & 1 );
                        iRead += 2;
                        break;
                    }

                    case 3: //24-bit
                        b = pPtr[iRead++], g = pPtr[iRead++], r = pPtr[iRead++];
                        break;

                    case 4: //32-bit
                        b = pPtr[iRead++], g = pPtr[iRead++], r = pPtr[iRead++];
                        a = pPtr[iRead++];
                        break;
                }
            }

            //loop for each run or execute once for non RLE
            for( int iRun = 0; iRun < nRunCount; iRun++ )
            {
                //write the values into the appropriate place in the buffer
                int iRGBWrite = (x + (y * pHeader->Image_Width)) * 3;               
                mmrgba.pRGB[0][iRGBWrite++] = b;
                mmrgba.pRGB[0][iRGBWrite++] = g;
                mmrgba.pRGB[0][iRGBWrite++] = r;

                //read alpha channel if we've got one
                if( nAlphaBits && (dwFlags & LPF_LOADALPHA)) mmrgba.pAlpha[0][ x + (y * pHeader->Image_Width) ] = a;

                //keep track of where we are, relative to image orientation
                iWrite++;
                if( bFlipHorizontal ) { if( --x < 0 ) { x = pHeader->Image_Width - 1; if( bFlipVertical ) y--; else y++; } } else if( ++x >= pHeader->Image_Width ) { x = 0; if( bFlipVertical ) y--; else y++; }
            }                   
        }
    }

    //free the image data and return
    free( pBuffer );
    return true;
}

#pragma pack( pop )

