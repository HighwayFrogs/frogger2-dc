/*************************************************
 Bitmap File Loading

  To Do:

    * write a proper bitmap filter, not just use
    LoadImage()


**************************************************/
#pragma pack( push, 1 )


#include <windows.h>
#include "BMP.h"
#include "Util.h"

//////////////////////////////////////////////////////////////////////
// Load the bitmap into the mmrgba object
//////////////////////////////////////////////////////////////////////
bool LoadBMP( const char* pszFilename, MMRGBA &mmrgba, unsigned long int dwFlags )
{
    //load the bitmap
    HBITMAP hBitmap = (HBITMAP)LoadImage( NULL, pszFilename, IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE|LR_CREATEDIBSECTION );
    if( hBitmap == NULL ) return false;

    //get information about the bitmap
    BITMAP bm;
    GetObject( hBitmap, sizeof(bm), &bm );

    //calculate aligned width
    int nPitch = (long)(((long)(bm.bmWidth*3*8) + 31) / 32) * 4;

    //prepare bitmap info structure
    BITMAPINFOHEADER bih;
    memset( &bih, 0, sizeof(bih) );
    bih.biSize = sizeof(BITMAPINFOHEADER);
    bih.biSizeImage = nPitch * bm.bmHeight;
    bih.biBitCount = 24;
    bih.biWidth = bm.bmWidth;
    bih.biHeight = -bm.bmHeight;
    bih.biPlanes = 1;
    bih.biCompression = BI_RGB;
    
    //allocate the return buffer
    mmrgba.Init( MMINIT_RGB, bm.bmWidth, bm.bmHeight );
    unsigned char* pRGBBytes = (unsigned char*)malloc( bih.biSizeImage );

    //get the bitmap's bits
    HDC hDC = GetDC(NULL);
    int m = GetDIBits( hDC, hBitmap, 0, bm.bmHeight, (LPVOID)pRGBBytes, (LPBITMAPINFO)&bih, DIB_RGB_COLORS );
    int n = GetLastError();
    ReleaseDC( NULL, hDC );

    //delete the bitmap handle
    DeleteObject( hBitmap );

    //unalign the bitmap - pack to raw RGB
    for( int i = 1; i < mmrgba.nHeight; i++ ) memcpy( &pRGBBytes[ mmrgba.nWidth * i * 3 ], &pRGBBytes[ nPitch * i ], mmrgba.nWidth * 3 );

    //store this in the mmrgba
    mmrgba.pRGB[0] = pRGBBytes;

    //return 
    return true;
}



#pragma pack( pop )
