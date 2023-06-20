/*

	This file is part of Frogger2, (c) 1999 Interactive Studios Ltd.

	File		: fadeout.cpp
	Programmer	: David
	Date		: 
	Purpose		: Transition effects 'n shit

----------------------------------------------------------------------------------------------- */

#include "fadeout.h"
#include "layout.h"
#include "sonylibs.h"
#include "shell.h"
#include <kamui2.h>
#include "backdrop.h"

long fadeoutStart, fadeoutLength;
int fadingOut = 0, keepFade = 0;

int fadedOutCount;

int fadeText = YES;
int flashScreen = NO;

int (*fadeProc)(void);

int startIntensity = 0, endIntensity = 255;

KMSURFACEDESC	fadeSurface;
KMSTRIPCONTEXT	fadeStripContext;
KMSTRIPHEAD		fadeStripHead;
KMVERTEX_00		fadeVertices[] =
{
{ KM_VERTEXPARAM_NORMAL,                  0,  BACKDROP_HEIGHT, 1.0f, },
{ KM_VERTEXPARAM_NORMAL,                  0,                0, 1.0f, },
{ KM_VERTEXPARAM_NORMAL,     BACKDROP_WIDTH,  BACKDROP_HEIGHT, 1.0f, },
{ KM_VERTEXPARAM_ENDOFSTRIP, BACKDROP_WIDTH,                0, 1.0f, }
};

/*	-------------------------------------------------------------------------------
	Function:	ScreenFade
	Params:		start, end intensity (0-255), time in frames
	returns:	
*/
int DrawScreenFade(void)
{
	int col;
	
	if (actFrameCount > (fadeoutStart+fadeoutLength))
	{
		col = endIntensity;
		fadedOutCount++;
	}
	else
		col = startIntensity + ((endIntensity-startIntensity)*(long)(actFrameCount-fadeoutStart))/fadeoutLength;

	// draw the progress fade background
	fadeVertices[0].fX = 0;
	fadeVertices[0].fY = 0;
	fadeVertices[0].u.fZ = 10.00;
	fadeVertices[0].uBaseRGB.dwPacked = RGBA(0, 0, 0, col);

	fadeVertices[1].fX = 0;
	fadeVertices[1].fY = 480;
	fadeVertices[1].u.fZ = 10.00;
	fadeVertices[1].uBaseRGB.dwPacked = RGBA(0, 0, 0, col);

	fadeVertices[2].fX = 640;
	fadeVertices[2].fY = 0;
	fadeVertices[2].u.fZ = 10.00;
	fadeVertices[2].uBaseRGB.dwPacked = RGBA(0, 0, 0, col);

	fadeVertices[3].fX = 640;
	fadeVertices[3].fY = 480;
	fadeVertices[3].u.fZ = 10.00;
	fadeVertices[3].uBaseRGB.dwPacked = RGBA(0, 0, 0, col);

	kmStartStrip(&vertexBufferDesc, &fadeStripHead);	
	kmSetVertex(&vertexBufferDesc, &fadeVertices[0], KM_VERTEXTYPE_03, sizeof(KMVERTEX_03));
	kmSetVertex(&vertexBufferDesc, &fadeVertices[1], KM_VERTEXTYPE_03, sizeof(KMVERTEX_03));
	kmSetVertex(&vertexBufferDesc, &fadeVertices[2], KM_VERTEXTYPE_03, sizeof(KMVERTEX_03));	
	kmSetVertex(&vertexBufferDesc, &fadeVertices[3], KM_VERTEXTYPE_03, sizeof(KMVERTEX_03));	
	kmEndStrip(&vertexBufferDesc);

	return (fadedOutCount > 1);
}


/*	-------------------------------------------------------------------------------
	Function:	ScreenFade
	Params:		start, end intensity (0-255), time in frames
	returns:	
*/
void ScreenFade(int start, int end, long time)
{
	// initialise strip context and head
    kmInitStripContext(KM_STRIPCONTEXT_SYS_GOURAUD | KM_TRANS_POLYGON, &fadeStripContext);
	memset(&fadeStripContext,0,sizeof(fadeStripContext));
	memset(&fadeStripHead,0,sizeof(fadeStripHead));
	fadeStripContext.nSize = sizeof(KMSTRIPCONTEXT);
    kmInitStripContext(KM_STRIPCONTEXT_SYS_GOURAUD | KM_TRANS_POLYGON, &fadeStripContext);
	fadeStripContext.StripControl.nListType		 					= KM_TRANS_POLYGON;
	fadeStripContext.StripControl.nUserClipMode	 					= KM_USERCLIP_DISABLE;
	fadeStripContext.StripControl.nShadowMode			 			= KM_NORMAL_POLYGON;
	fadeStripContext.StripControl.bGouraud		 					= KM_TRUE;
	fadeStripContext.ObjectControl.nDepthCompare			 		= KM_ALWAYS;
	fadeStripContext.ObjectControl.nCullingMode			 			= KM_NOCULLING;
	fadeStripContext.ObjectControl.bZWriteDisable					= KM_TRUE;
	fadeStripContext.ImageControl[KM_IMAGE_PARAM1].nSRCBlendingMode	= KM_SRCALPHA;
	fadeStripContext.ImageControl[KM_IMAGE_PARAM1].nDSTBlendingMode	= KM_INVSRCALPHA;
	fadeStripContext.ImageControl[KM_IMAGE_PARAM1].bSRCSelect		= KM_FALSE;
	fadeStripContext.ImageControl[KM_IMAGE_PARAM1].bDSTSelect		= KM_FALSE;
	fadeStripContext.ImageControl[KM_IMAGE_PARAM1].nFogMode			= KM_NOFOG;
	fadeStripContext.ImageControl[KM_IMAGE_PARAM1].bColorClamp		= KM_FALSE;
	fadeStripContext.ImageControl[KM_IMAGE_PARAM1].bUseAlpha		= KM_TRUE;
	fadeStripContext.ImageControl[KM_IMAGE_PARAM1].nFilterMode		= KM_BILINEAR;
	kmGenerateStripHead00(&fadeStripHead,&fadeStripContext);

	fadeProc = DrawScreenFade;
	fadeoutStart = actFrameCount;
	fadeoutLength = time;
	fadingOut = 1;

	// cause we're using subtractive
	startIntensity = 255-start;
	endIntensity = 255-end;
	fadedOutCount = 0;
	fadeText = YES;
	flashScreen = NO;
}


void StopFadeout()
{
	fadeProc = NULL;
	fadingOut = 0;
}

/*	-------------------------------------------------------------------------------
	Function:	MainWndProc
	Params:		As WNDPROC
	returns:	0 for success, other to pass on to mdx default handler
*/
void DrawScreenTransition(void)
{
//	if (KEYPRESS(DIK_F))
//		ScreenFade(0, 255, 60);

	if (fadeProc)
		if (fadeProc())
		{
			fadingOut = 0;
			
			if (!keepFade)
				fadeProc = NULL;
		}
}

/*	-------------------------------------------------------------------------------
	Function:	BlankScreen
	Params:		start, end intensity (0-255), time in frames
	returns:	
*/

void BlankScreen(void)
{
	int col = 255;

	// initialise strip context and head
    kmInitStripContext(KM_STRIPCONTEXT_SYS_GOURAUD | KM_TRANS_POLYGON, &fadeStripContext);
	memset(&fadeStripContext,0,sizeof(fadeStripContext));
	memset(&fadeStripHead,0,sizeof(fadeStripHead));
	fadeStripContext.nSize = sizeof(KMSTRIPCONTEXT);
    kmInitStripContext(KM_STRIPCONTEXT_SYS_GOURAUD | KM_TRANS_POLYGON, &fadeStripContext);
	fadeStripContext.StripControl.nListType		 					= KM_TRANS_POLYGON;
	fadeStripContext.StripControl.nUserClipMode	 					= KM_USERCLIP_DISABLE;
	fadeStripContext.StripControl.nShadowMode			 			= KM_NORMAL_POLYGON;
	fadeStripContext.StripControl.bGouraud		 					= KM_TRUE;
	fadeStripContext.ObjectControl.nDepthCompare			 		= KM_ALWAYS;
	fadeStripContext.ObjectControl.nCullingMode			 			= KM_NOCULLING;
	fadeStripContext.ObjectControl.bZWriteDisable					= KM_TRUE;
	fadeStripContext.ImageControl[KM_IMAGE_PARAM1].nSRCBlendingMode	= KM_SRCALPHA;
	fadeStripContext.ImageControl[KM_IMAGE_PARAM1].nDSTBlendingMode	= KM_INVSRCALPHA;
	fadeStripContext.ImageControl[KM_IMAGE_PARAM1].bSRCSelect		= KM_FALSE;
	fadeStripContext.ImageControl[KM_IMAGE_PARAM1].bDSTSelect		= KM_FALSE;
	fadeStripContext.ImageControl[KM_IMAGE_PARAM1].nFogMode			= KM_NOFOG;
	fadeStripContext.ImageControl[KM_IMAGE_PARAM1].bColorClamp		= KM_FALSE;
	fadeStripContext.ImageControl[KM_IMAGE_PARAM1].bUseAlpha		= KM_TRUE;
	fadeStripContext.ImageControl[KM_IMAGE_PARAM1].nFilterMode		= KM_BILINEAR;
	kmGenerateStripHead00(&fadeStripHead,&fadeStripContext);

	// draw the progress fade background
	fadeVertices[0].fX = 0;
	fadeVertices[0].fY = 0;
	fadeVertices[0].u.fZ = 10.00;
	fadeVertices[0].uBaseRGB.dwPacked = RGBA(0, 0, 0, col);

	fadeVertices[1].fX = 0;
	fadeVertices[1].fY = 480;
	fadeVertices[1].u.fZ = 10.00;
	fadeVertices[1].uBaseRGB.dwPacked = RGBA(0, 0, 0, col);

	fadeVertices[2].fX = 640;
	fadeVertices[2].fY = 0;
	fadeVertices[2].u.fZ = 10.00;
	fadeVertices[2].uBaseRGB.dwPacked = RGBA(0, 0, 0, col);

	fadeVertices[3].fX = 640;
	fadeVertices[3].fY = 480;
	fadeVertices[3].u.fZ = 10.00;
	fadeVertices[3].uBaseRGB.dwPacked = RGBA(0, 0, 0, col);

	kmStartStrip(&vertexBufferDesc, &fadeStripHead);	
	kmSetVertex(&vertexBufferDesc, &fadeVertices[0], KM_VERTEXTYPE_03, sizeof(KMVERTEX_03));
	kmSetVertex(&vertexBufferDesc, &fadeVertices[1], KM_VERTEXTYPE_03, sizeof(KMVERTEX_03));
	kmSetVertex(&vertexBufferDesc, &fadeVertices[2], KM_VERTEXTYPE_03, sizeof(KMVERTEX_03));	
	kmSetVertex(&vertexBufferDesc, &fadeVertices[3], KM_VERTEXTYPE_03, sizeof(KMVERTEX_03));	
	kmEndStrip(&vertexBufferDesc);
}