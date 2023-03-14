/*************************************************
 TIF file loading



**************************************************/
#pragma pack( push, 1 )
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <windows.h>
#include "TIF.h"
#include "Picture.h"
#include "Util.h"

extern unsigned char g_nOpaqueAlpha;

struct TIFHeader
{
    unsigned char byteOrder[2]; //II or MM. II=little endian [intel], MM=big endian [motorola]
    unsigned short int nMagic; //42
    unsigned long int nIFDOffset;
};

struct TIFDirectoryHeader
{
    unsigned short int nEntryCount;
};

struct TIFDirectoryFooter
{
    unsigned short int nIFDOffset;
};

struct TIFDirectoryEntry
{
    unsigned short int tag;
    unsigned short int type;
    unsigned long int nCount;        //number of values, not bytes
    unsigned long int nValueOffset; //contains value of value fits into 4 bytes.

};



//TAG
#define TIF_TAG_IMAGEWIDTH                  0x100
#define TIF_TAG_IMAGEHEIGHT                 0x101
#define TIF_TAG_COMPRESSION                 0x103
#define TIF_TAG_PHOTOMETRICINTERPRETATION   0x106
#define TIF_TAG_ROWSPERSTRIP                0x116
#define TIF_TAG_STRIPOFFSETS                0x111
#define TIF_TAG_STRIPBYTECOUNTS             0x117
#define TIF_TAG_BITSPERSAMPLE               0x102
#define TIF_TAG_COLOURMAP                   0x140
#define TIF_TAG_SAMPLESPERPIXEL             0x115
#define TIF_TAG_EXTRASAMPLES                0x152
#define TIF_TAG_PLANARCONFIGURATION         0x11C
//#define TIF_TAG_
//#define TIF_TAG_

#define TIF_FORMAT_WHITEBLACK   0
#define TIF_FORMAT_BLACKWHITE   1
#define TIF_FORMAT_RGB          2
#define TIF_FORMAT_PALETTE      3
#define TIF_FORMAT_MASK         4



//TYPE:
#define TIF_ENTRY_TYPE_BYTE         1   //8-bit unsigned int
#define TIF_ENTRY_TYPE_ASCII        2   //NUL terminated char
#define TIF_ENTRY_TYPE_SHORT        3   //16-bit unsigned int
#define TIF_ENTRY_TYPE_LONG         4   //32-bit unsigned int
#define TIF_ENTRY_TYPE_RATIONAL     5   //two LONGS: numerator and denominator
//TIFF 6.0 +
#define TIF_ENTRY_TYPE_SBYTE        6   //8-big signed int
#define TIF_ENTRY_TYPE_UNDEFINED    7   //8-bit byte that may contain anything
#define TIF_ENTRY_TYPE_SSHORT       8   //16-bit signed short int
#define TIF_ENTRY_TYPE_SLONG        9   //32-bit signed long int
#define TIF_ENTRY_TYPE_SRATIONAL    10  //two SLONGs: numerator and denominator
#define TIF_ENTRY_TYPE_FLOAT        11  //float
#define TIF_ENTRY_TYPE_DOUBLE       12  //double

//////////////////////////////////////////////////////////////////////
// Helper function to read an array of int values from the TIF file
//////////////////////////////////////////////////////////////////////
void TIFReadIntArray( void** pArray, unsigned char nWordSize, TIFDirectoryEntry* pEntry, unsigned char* pBuffer, unsigned long int& nSize, bool bByteSwap )
{
    //validate
    if( pBuffer == NULL || pEntry == NULL || pEntry->nCount == 0 ) return;
    if( nWordSize != 1 && nWordSize != 2 && nWordSize != 4 ) return;
    
    //remove existing array and allocate a new one
    if( *pArray ) free( *pArray );
    *pArray = malloc( pEntry->nCount * nWordSize );
    memset( *pArray, 0, pEntry->nCount * nWordSize );
    nSize = pEntry->nCount;

    
    //get a pointer to the start of the data
    unsigned char* pPtr = pBuffer + pEntry->nValueOffset;
    if( pEntry->nCount == 1 ) pPtr = (unsigned char*)&pEntry->nValueOffset;

    //read each value and put it in the array
    for( unsigned long int i = 0; i < pEntry->nCount; i++ )
    {
        //get value
        unsigned long int nValue;
        switch( pEntry->type )
        {
            case TIF_ENTRY_TYPE_SHORT: nValue = *((unsigned short int*)pPtr); pPtr += 2; break;
            case TIF_ENTRY_TYPE_LONG:  nValue = *((unsigned long int*)pPtr); pPtr += 4; break;
            default: DisplayStatusMessage( "Invalid TIF field type" ); free( *pArray ); return;
        }

        //write value into array
        switch( nWordSize )
        {
            case 1: ((unsigned char*)*pArray)[i] = (unsigned char)nValue; break;
            case 2: ((unsigned short int*)*pArray)[i] = (unsigned short int)nValue; if( bByteSwap ) ByteSwap( ((unsigned short int*)*pArray)[i] ); break;
            case 4: ((unsigned long int*)*pArray)[i] = (unsigned long int)nValue;   if( bByteSwap ) ByteSwap( ((unsigned long int*)*pArray)[i] ); break;
        }
    }
}


//////////////////////////////////////////////////////////////////////
// Loads the given TIF file into the given mmrgba object
//////////////////////////////////////////////////////////////////////
bool LoadTIF( const char* pszFilename, MMRGBA &mmrgba, unsigned long int dwFlags )
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

    //get header
    TIFHeader* pHeader = (TIFHeader*)pPtr;

    //if it's a bigendian file, swap the header
    bool bByteSwap = (memcmp(pHeader->byteOrder, "MM", 2 ) == 0);
    if( bByteSwap ) { ByteSwap( pHeader->nMagic ); ByteSwap( pHeader->nIFDOffset ); }

    //validate header
    if( pHeader->nMagic != 42 )
    {
        free( pBuffer );
        return ReturnError( "Not a TIF file: invalid header", pszFilename );
    }

    
    //values we want
    unsigned long int nWidth = 0, nHeight = 0;
    unsigned short int nCompression = 1, nColourFormat = 0;
    unsigned long int* pStripOffsets = NULL;
    unsigned long int* pStripByteCounts = NULL;
    unsigned long int nRowsPerStrip = 0;
    unsigned short int* pColourMap = NULL;
    unsigned short int* pBitsPerSample = NULL;
    unsigned long int nStripOffsets_Count = 0, nStripByteCounts_Count = 0, nColourMap_Count = 0, nBitsPerSample_Count = 0;
    unsigned short int nExtraSamples = 0, nPlanarConfig = 1, nSamplesPerPixel = 0;



    //read all directories and extract the useful information
    unsigned long int nIFDOffset = pHeader->nIFDOffset;
    while( nIFDOffset != 0 ) 
    {
        //move to directory entry
        pPtr = pBuffer + nIFDOffset;

        //read next directory entry
        TIFDirectoryHeader* pDirectory = (TIFDirectoryHeader*)pPtr;
        pPtr += sizeof(TIFDirectoryHeader);
        if( bByteSwap ) ByteSwap( pDirectory->nEntryCount );

        //read each directory entry
        for( unsigned short int iEntry = 0; iEntry < pDirectory->nEntryCount; iEntry++ )
        {
            //read directory entry & apply byte swap
            TIFDirectoryEntry* pEntry = (TIFDirectoryEntry*)pPtr;
            pPtr += sizeof(TIFDirectoryEntry);
            if( bByteSwap ) { ByteSwap( pEntry->nCount ); ByteSwap( pEntry->nValueOffset ); }
            
            //process entry
            switch( pEntry->tag )
            {
                case TIF_TAG_IMAGEWIDTH:                nWidth = pEntry->nValueOffset;           break;
                case TIF_TAG_IMAGEHEIGHT:               nHeight = pEntry->nValueOffset;          break;
                case TIF_TAG_COMPRESSION:               nCompression = (unsigned short int)pEntry->nValueOffset;     break;
                case TIF_TAG_PHOTOMETRICINTERPRETATION: nColourFormat = (unsigned short int)pEntry->nValueOffset;    break;
                case TIF_TAG_ROWSPERSTRIP:              nRowsPerStrip = pEntry->nValueOffset;    break;
                case TIF_TAG_STRIPOFFSETS:              TIFReadIntArray( (void**)&pStripOffsets, 4, pEntry, pBuffer, nStripOffsets_Count, bByteSwap ); break;
                case TIF_TAG_STRIPBYTECOUNTS:           TIFReadIntArray( (void**)&pStripByteCounts, 4, pEntry, pBuffer, nStripByteCounts_Count, bByteSwap ); break;
                case TIF_TAG_BITSPERSAMPLE:             TIFReadIntArray( (void**)&pBitsPerSample, 2, pEntry, pBuffer, nBitsPerSample_Count, bByteSwap ); break;
                case TIF_TAG_COLOURMAP:                 TIFReadIntArray( (void**)&pColourMap, 2, pEntry, pBuffer, nColourMap_Count, bByteSwap ); break;
                case TIF_TAG_SAMPLESPERPIXEL:           nSamplesPerPixel = (unsigned short int)pEntry->nValueOffset; break;
                case TIF_TAG_EXTRASAMPLES:              nExtraSamples = (unsigned short int)pEntry->nValueOffset; break;
                case TIF_TAG_PLANARCONFIGURATION:       nPlanarConfig = (unsigned short int)pEntry->nValueOffset; break;
                default: //other tags go here...
                    break;
            }
        }

        //read footer
        TIFDirectoryFooter* pFooter = (TIFDirectoryFooter*)pPtr;
        pPtr += sizeof(TIFDirectoryFooter);
        if( bByteSwap ) ByteSwap( pFooter->nIFDOffset );

        //extract new offset
        nIFDOffset = pFooter->nIFDOffset;
    }

    //make sure we got everything we should have and it's in a format we like
    if( nWidth != 0 && nHeight != 0 && pStripOffsets != NULL && pStripByteCounts != NULL &&
        nPlanarConfig == 1 && nCompression == 1 && nColourFormat != TIF_FORMAT_MASK  )
    {
        //calculate the total size of all strip data
        unsigned long int nDataSize = 0, iStrip;
        for( iStrip = 0; iStrip < nStripByteCounts_Count; iStrip++ ) nDataSize += pStripByteCounts[iStrip];

        //compile all strips together into a buffer
        unsigned char* pData = (unsigned char*)malloc( nDataSize );
        unsigned char* pTemp = pData;
        for( iStrip = 0; iStrip < nStripByteCounts_Count; iStrip++ )
        {
            memcpy( pTemp, pBuffer + pStripOffsets[iStrip], pStripByteCounts[iStrip] );
            pTemp += pStripByteCounts[iStrip];
        }

        //free the strips and the file buffer
        free( pStripOffsets );
        free( pStripByteCounts );
        free( pBuffer );

        //get other image properties and create the image
        bool bAlpha = (nExtraSamples > 0) || (nSamplesPerPixel > 3);
        mmrgba.Init( MMINIT_ALLOCATE|MMINIT_RGB|(( bAlpha && (dwFlags & LPF_LOADALPHA) )?MMINIT_ALPHA:0), nWidth, nHeight );
        unsigned char* pRGB = mmrgba.pRGB[0];
        unsigned char* pAlpha = ( bAlpha && (dwFlags & LPF_LOADALPHA)) ? mmrgba.pAlpha[0] : NULL;

        //do byte swap on palette
        if( pColourMap && bByteSwap ) 
        {
            for( unsigned long int i = 0; i < nColourMap_Count; i++ ) ByteSwap( pColourMap[i] );
        }

        //calculate how much row padding there is
        int nRowPad = 0;
        switch( nSamplesPerPixel )
        {
            case 3: nRowPad = (nWidth + 1) / 2; break;
            case 4: nRowPad = (nWidth + 3) / 4; break;
        }


        //process the image
        int iRead = 0, iWrite = 0, iMax = nWidth * nHeight;
        unsigned long int x = 0, y = 0;
        unsigned char bit = 0;
        switch( pBitsPerSample ? pBitsPerSample[0] : 1  )
        {
            case 1: bit = 0x80; break; //80h = 10000000
            case 4: bit = 0xF0; break; //F0h = 11110000
        }
        while( iWrite < iMax )
        {
            unsigned char r, g, b, a = g_nOpaqueAlpha;

            switch( nColourFormat )
            {
                case TIF_FORMAT_WHITEBLACK:
                case TIF_FORMAT_BLACKWHITE:
                    //determine the image's bpp
                    switch( pBitsPerSample ? pBitsPerSample[0] : 1 )
                    {
                        case 1: //1-bpp : 2 colours.
                            r = g = b = (pData[iRead] & bit) ? 0xFF : 0x00;
                            if( (bit >>= 1 ) == 0 ) { iRead++; bit = 0x80; }
                            break;

                        case 4: //4-bpp : 16 colours
                            r = g = b = (pData[iRead] & bit) << ( bit == 0xF0 ? 0 : 4 );
                            if( (bit ^= 0xFF) == 0xF0 ) iRead++;
                            break;

                        case 8: //8-bpp : 256 colours
                            r = g = b = pData[iRead++];
                            break;
                    }

                    //if it's a "0=white" image, invert black & white
                    if( nColourFormat == TIF_FORMAT_WHITEBLACK ) { r ^= 0xFF; g ^= 0xFF; b ^= 0xFF; }
                    break;

                case TIF_FORMAT_RGB: //FIXME
                    switch( nSamplesPerPixel )
                    {
                        case 3: r = pData[iRead]; g = pData[iRead+1]; b = pData[iRead+2]; break;
                        case 4: 
                            r = pData[iRead]; g = pData[iRead+1]; b = pData[iRead+2]; a = pData[iRead+3]; 
                            if( nExtraSamples == 1 ) //alpha is premultiplied. Un-premultiply it
                            {
                                    r *= (1.0 - (a/256.0); g *= (1.0 - (a/256.0); b *= (1.0 - (a/256.0); break;
                            }
                            break;
                    }
                    iRead += nSamplesPerPixel;
                    break;

                case TIF_FORMAT_PALETTE:
                    switch( pBitsPerSample ? pBitsPerSample[0] : 1 )
                    {
                        case 1:
                        {
                            unsigned char iColour = (pData[iRead] & bit) ? 0xFF : 0x00;
                            if( (bit >>= 1 ) == 0 ) { iRead++; bit = 0x80; }
                            r = pColourMap[ iColour ] / 256;
                            g = pColourMap[ iColour + 2 ] / 256; 
                            b = pColourMap[ iColour + 4 ] / 256;
                            break;
                        }

                        case 4:
                        {
                            unsigned char iColour = (pData[iRead] & bit) >> ( bit == 0xF0 ? 4 : 0 );
                            if( (bit ^= 0xFF) == 0xF0 ) iRead++;
                            r = pColourMap[ iColour ] / 256;
                            g = pColourMap[ iColour + 16 ] / 256; 
                            b = pColourMap[ iColour + 32 ] / 256;
                            break;
                        }

                        case 8:
                            switch( nSamplesPerPixel )
                            {
                                case 1:
                                {
                                    unsigned char iColour = pData[iRead++];
                                    r = pColourMap[ iColour ] / 256;
                                    g = pColourMap[ iColour + 256 ] / 256;
                                    b = pColourMap[ iColour + 512 ] / 256;
                                    break;
                                }
                                case 3: //dunno if this one exists or not...
                                    r = pColourMap[ pData[iRead++] ] / 256;
                                    g = pColourMap[ pData[iRead++] + 256 ] / 256;
                                    b = pColourMap[ pData[iRead++] + 512 ] / 256;
                                    break;

                            }
                            break;
                    }

            }

            //write the values into the buffer
            *pRGB++ = b;
            *pRGB++ = g;
            *pRGB++ = r;
            if( pAlpha ) *pAlpha++ = a;

            //keep track of where we are, relative to image orientation
            iWrite++;
            if( ++x > nWidth ) { x = 0; y++; iRead += nRowPad; }

        }

        //clean up
        if( pColourMap ) free( pColourMap );
        if( pBitsPerSample ) free( pBitsPerSample );
        free( pData );
    }
    else
    {
        //clean up
        if( pStripOffsets ) free( pStripOffsets );
        if( pStripByteCounts ) free( pStripByteCounts );
        if( pColourMap ) free( pColourMap );
        if( pBitsPerSample ) free( pBitsPerSample );
        free( pBuffer );
        
        //display error and return
        return ReturnError( "Unsupported image format", pszFilename );
    }


    return true;
}

#pragma pack( pop )

