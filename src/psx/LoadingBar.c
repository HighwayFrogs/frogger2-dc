#include "sonylibs.h"
#include "Shell.h"

#include "loadingbar.h"

#include <kamui2.h>
#include "backdrop.h"

int barProgress = 0;
int barLength = 200;

#define	LOADBAR_XPOS	410 //40
#define	LOADBAR_YPOS	435
#define	LOADBAR_XD		200//560
#define	LOADBAR_YD		20

KMSURFACEDESC	barSurface;
KMSTRIPCONTEXT	barStripContext;
KMSTRIPHEAD		barStripHead;
KMVERTEX_00		barVertices[] =
{
{ KM_VERTEXPARAM_NORMAL,                  0,  BACKDROP_HEIGHT, 1.0f, },
{ KM_VERTEXPARAM_NORMAL,                  0,                0, 1.0f, },
{ KM_VERTEXPARAM_NORMAL,     BACKDROP_WIDTH,  BACKDROP_HEIGHT, 1.0f, },
{ KM_VERTEXPARAM_ENDOFSTRIP, BACKDROP_WIDTH,                0, 1.0f, }
};

void InitProgressBar()
{
	int				i,x,y;
	char			buf[256],*headerPtr,*ptr;
    PKMDWORD   	 	TexturePtr,TwiddledPtr;
  	USHORT			width,height,*pixelPtr,pixel,newPixel;
  	int				headerSize,colFormat,texFormat,texAttrib,attrib;
	char			r,g,b,a;

	// initialise strip context and head
    kmInitStripContext(KM_STRIPCONTEXT_SYS_GOURAUD | KM_OPAQUE_POLYGON, &barStripContext);
	memset(&barStripContext,0,sizeof(barStripContext));
	memset(&barStripHead,0,sizeof(barStripHead));
	barStripContext.nSize = sizeof(KMSTRIPCONTEXT);
    kmInitStripContext(KM_STRIPCONTEXT_SYS_GOURAUD | KM_OPAQUE_POLYGON, &barStripContext);
	barStripContext.StripControl.nListType		 					= KM_OPAQUE_POLYGON;
	barStripContext.StripControl.nUserClipMode	 					= KM_USERCLIP_DISABLE;
	barStripContext.StripControl.nShadowMode			 			= KM_NORMAL_POLYGON;
	barStripContext.StripControl.bGouraud		 					= KM_TRUE;
	barStripContext.ObjectControl.nDepthCompare			 			= KM_GREATER;
	barStripContext.ObjectControl.nCullingMode			 			= KM_NOCULLING;
	barStripContext.ObjectControl.bZWriteDisable					= KM_TRUE;
	barStripContext.ImageControl[KM_IMAGE_PARAM1].nSRCBlendingMode	= KM_ONE;
	barStripContext.ImageControl[KM_IMAGE_PARAM1].nDSTBlendingMode	= KM_ZERO;
	barStripContext.ImageControl[KM_IMAGE_PARAM1].bSRCSelect		= KM_FALSE;
	barStripContext.ImageControl[KM_IMAGE_PARAM1].bDSTSelect		= KM_FALSE;
	barStripContext.ImageControl[KM_IMAGE_PARAM1].nFogMode			= KM_NOFOG;
	barStripContext.ImageControl[KM_IMAGE_PARAM1].bColorClamp		= KM_FALSE;
	barStripContext.ImageControl[KM_IMAGE_PARAM1].bUseAlpha			= KM_FALSE;
	barStripContext.ImageControl[KM_IMAGE_PARAM1].nFilterMode		= KM_BILINEAR;
	kmGenerateStripHead00(&barStripHead,&barStripContext);
}

void StepProgressBar ( int percentAmount )
{
	// JH: Add the required amount onto the bar progress....
	barProgress += percentAmount;

	// JH: Just for safety, let's make sure we can't go above 100%....
	if ( barProgress > 100 )
		barProgress = 100;
	// ENDIF
}

void SetProgressBar ( int percentage )
{
	// JH: Equal the required amount into the bar progress....
	barProgress = percentage;

	// JH: Just for safety, let's make sure some fool hasn't enter a wrong value....
	if ( barProgress > 100 )
		barProgress = 100;

	if ( barProgress < 0 )
		barProgress = 0;
}

void DrawLoadingBar ( int execute )
{
	int		length;

	utilPrintf("Drawing Loadingbar...............................................\n");

	DrawBackDrop(0, 0);

	// draw the progress bar background
	barVertices[0].fX = LOADBAR_XPOS-2;
	barVertices[0].fY = LOADBAR_YPOS-2;
	barVertices[0].uBaseRGB.dwPacked = RGBA(0,0,0,255);

	barVertices[1].fX = LOADBAR_XPOS-2;
	barVertices[1].fY = LOADBAR_YPOS+LOADBAR_YD+2;
	barVertices[1].uBaseRGB.dwPacked = RGBA(0,0,0,255);

	barVertices[2].fX = LOADBAR_XPOS + barLength+2;
	barVertices[2].fY = LOADBAR_YPOS-2;
	barVertices[2].uBaseRGB.dwPacked = RGBA(0,0,0,255);

	barVertices[3].fX = LOADBAR_XPOS + barLength+2;
	barVertices[3].fY = LOADBAR_YPOS+LOADBAR_YD+2;
	barVertices[3].uBaseRGB.dwPacked = RGBA(0,0,0,255);

	kmStartStrip(&vertexBufferDesc, &barStripHead);	
	kmSetVertex(&vertexBufferDesc, &barVertices[0], KM_VERTEXTYPE_03, sizeof(KMVERTEX_03));
	kmSetVertex(&vertexBufferDesc, &barVertices[1], KM_VERTEXTYPE_03, sizeof(KMVERTEX_03));
	kmSetVertex(&vertexBufferDesc, &barVertices[2], KM_VERTEXTYPE_03, sizeof(KMVERTEX_03));	
	kmSetVertex(&vertexBufferDesc, &barVertices[3], KM_VERTEXTYPE_03, sizeof(KMVERTEX_03));	
	kmEndStrip(&vertexBufferDesc);

	// draw the actual progess bar	
	length = ((float)barProgress/100.0)*barLength;
	
	barVertices[0].fX = LOADBAR_XPOS;
	barVertices[0].fY = LOADBAR_YPOS;
	barVertices[0].uBaseRGB.dwPacked = RGBA(255,0,0,255);

	barVertices[1].fX = LOADBAR_XPOS;
	barVertices[1].fY = LOADBAR_YPOS+LOADBAR_YD;
	barVertices[1].uBaseRGB.dwPacked = RGBA(255,0,0,255);

	barVertices[2].fX = LOADBAR_XPOS + length;
	barVertices[2].fY = LOADBAR_YPOS;
	barVertices[2].uBaseRGB.dwPacked = RGBA(0,255,0,255);

	barVertices[3].fX = LOADBAR_XPOS + length;
	barVertices[3].fY = LOADBAR_YPOS+LOADBAR_YD;
	barVertices[3].uBaseRGB.dwPacked = RGBA(0,255,0,255);

	kmStartStrip(&vertexBufferDesc, &barStripHead);	
	kmSetVertex(&vertexBufferDesc, &barVertices[0], KM_VERTEXTYPE_03, sizeof(KMVERTEX_03));
	kmSetVertex(&vertexBufferDesc, &barVertices[1], KM_VERTEXTYPE_03, sizeof(KMVERTEX_03));
	kmSetVertex(&vertexBufferDesc, &barVertices[2], KM_VERTEXTYPE_03, sizeof(KMVERTEX_03));	
	kmSetVertex(&vertexBufferDesc, &barVertices[3], KM_VERTEXTYPE_03, sizeof(KMVERTEX_03));	
	kmEndStrip(&vertexBufferDesc);
}