#include "include.h"		// so we can use mw_pr() which outputs to the debug window
#include "frogger.h"
#include "sg_Chain.h"
#include <am.h>
//ma#include "am_audio.h"
#include "generic.h"
#include <shinobi.h>
#include <sg_sd.h>
#include <sg_mw.h>
#include "dc_timer.h"

#define	TIMER_FRAMES	2
#define	TICKS_PER_FRAME	16666
#define	TIMER_TICKS		TIMER_FRAMES*TICKS_PER_FRAME
#define	TIMER_WIDTH		600
#define	TIMER_XPOS		20
#define	TIMER_YPOS		40	
#define	TIMER_SLICE		TIMER_WIDTH / TIMER_FRAMES
#define	TIMER_HEIGHT	20
#define	TIMER_YD		40

DCTIMER 				timers[NUM_TIMERS];

KMSURFACEDESC			timerbarSurface;
KMSTRIPCONTEXT			timerbarStripContext;
KMSTRIPHEAD				timerbarStripHead;
KMVERTEX_00				timerbarVertices[] =
{
{ KM_VERTEXPARAM_NORMAL,                  0,  BACKDROP_HEIGHT, 0.0f, 0.0f, RGBA(255,255,255,255),},
{ KM_VERTEXPARAM_NORMAL,                  0,                0, 0.0f, 0.0f, RGBA(255,255,255,255),},
{ KM_VERTEXPARAM_NORMAL,     BACKDROP_WIDTH,  BACKDROP_HEIGHT, 0.0f, 0.0f, RGBA(255,255,255,255),},
{ KM_VERTEXPARAM_ENDOFSTRIP, BACKDROP_WIDTH,                0, 0.0f, 0.0f, RGBA(255,255,255,255),}
};

void InitTimerbar()
{
	// initialise strip context and head
    kmInitStripContext(KM_STRIPCONTEXT_SYS_GOURAUD | KM_OPAQUE_POLYGON, &timerbarStripContext);
	memset(&timerbarStripContext,0,sizeof(timerbarStripContext));
	memset(&timerbarStripHead,0,sizeof(timerbarStripHead));
	timerbarStripContext.nSize = sizeof(KMSTRIPCONTEXT);
    kmInitStripContext(KM_STRIPCONTEXT_SYS_GOURAUD | KM_OPAQUE_POLYGON, &timerbarStripContext);
	timerbarStripContext.StripControl.nListType		 					= KM_OPAQUE_POLYGON;
	timerbarStripContext.StripControl.nUserClipMode	 					= KM_USERCLIP_DISABLE;
	timerbarStripContext.StripControl.nShadowMode			 			= KM_NORMAL_POLYGON;
	timerbarStripContext.StripControl.bGouraud		 					= KM_TRUE;
	timerbarStripContext.ObjectControl.nDepthCompare			 		= KM_GREATER;
	timerbarStripContext.ObjectControl.nCullingMode			 			= KM_NOCULLING;
	timerbarStripContext.ObjectControl.bZWriteDisable					= KM_TRUE;
	timerbarStripContext.ImageControl[KM_IMAGE_PARAM1].nSRCBlendingMode	= KM_ONE;
	timerbarStripContext.ImageControl[KM_IMAGE_PARAM1].nDSTBlendingMode	= KM_ZERO;
	timerbarStripContext.ImageControl[KM_IMAGE_PARAM1].bSRCSelect		= KM_FALSE;
	timerbarStripContext.ImageControl[KM_IMAGE_PARAM1].bDSTSelect		= KM_FALSE;
	timerbarStripContext.ImageControl[KM_IMAGE_PARAM1].nFogMode			= KM_NOFOG;
	timerbarStripContext.ImageControl[KM_IMAGE_PARAM1].bColorClamp		= KM_FALSE;
	timerbarStripContext.ImageControl[KM_IMAGE_PARAM1].bUseAlpha		= KM_FALSE;
	timerbarStripContext.ImageControl[KM_IMAGE_PARAM1].nFilterMode		= KM_BILINEAR;
	kmGenerateStripHead00(&timerbarStripHead,&timerbarStripContext);
}

void DCTIMER_RESET()
{
	int	i;
	
	for(i=0;i<NUM_TIMERS;i++)
	{
		timers[i].start = 0;
		timers[i].count = 0;
		timers[i].micro = 0;
	}
}

void DCTIMER_START(int num)
{
	if(num == 0)
		syTmrGenSetCount(0xffffffff);
		
	timers[num].start = syTmrGetCount();
}

void DCTIMER_STOP_ADD(int num)
{
	Uint32	diff,time;
	
	time = syTmrGetCount();
	diff = syTmrDiffCount(timers[num].start, time);
	timers[num].count += diff;
}

void DCTIMER_STOP(int num)
{
	Uint32	diff,time;
	
	time = syTmrGetCount();
	diff = syTmrDiffCount(timers[num].start, time);
	timers[num].count = diff;
}

void DCTIMER_PRINTS()
{
	int	i,length,xoffset;	

	// set the colour for the bars
	timerbarVertices[0].uBaseRGB.dwPacked = RGBA(255,255,255,255);	
	timerbarVertices[1].uBaseRGB.dwPacked = RGBA(255,255,255,255);	
	timerbarVertices[2].uBaseRGB.dwPacked = RGBA(255,255,255,255);	
	timerbarVertices[3].uBaseRGB.dwPacked = RGBA(255,255,255,255);	
	
	// print frame bars
	for(i=0;i<TIMER_FRAMES+1;i++)
	{
		timerbarVertices[0].fX = TIMER_XPOS + (TIMER_SLICE*i);
		timerbarVertices[0].fY = TIMER_YPOS - 10;
		timerbarVertices[0].u.fZ = 20000.0;

		timerbarVertices[1].fX = TIMER_XPOS + (TIMER_SLICE*i);
		timerbarVertices[1].fY = TIMER_YPOS + 400 + 10;
		timerbarVertices[1].u.fZ = 20000.0;

		timerbarVertices[2].fX = TIMER_XPOS  + (TIMER_SLICE*i) + 4;
		timerbarVertices[2].fY = TIMER_YPOS - 10;
		timerbarVertices[2].u.fZ = 20000.0;

		timerbarVertices[3].fX = TIMER_XPOS  + (TIMER_SLICE*i) + 4;
		timerbarVertices[3].fY = TIMER_YPOS + 400 + 10;
		timerbarVertices[3].u.fZ = 20000.0;

		kmStartStrip(&vertexBufferDesc, &timerbarStripHead);	
		kmSetVertex(&vertexBufferDesc, &timerbarVertices[0], KM_VERTEXTYPE_03, sizeof(KMVERTEX_03));
		kmSetVertex(&vertexBufferDesc, &timerbarVertices[1], KM_VERTEXTYPE_03, sizeof(KMVERTEX_03));
		kmSetVertex(&vertexBufferDesc, &timerbarVertices[2], KM_VERTEXTYPE_03, sizeof(KMVERTEX_03));	
		kmSetVertex(&vertexBufferDesc, &timerbarVertices[3], KM_VERTEXTYPE_03, sizeof(KMVERTEX_03));	
		kmEndStrip(&vertexBufferDesc);
	}

	// set the colour for the bars
	timerbarVertices[0].uBaseRGB.dwPacked = RGBA(0,255,0,255);	
	timerbarVertices[1].uBaseRGB.dwPacked = RGBA(0,255,0,255);	
	timerbarVertices[2].uBaseRGB.dwPacked = RGBA(0,255,0,255);	
	timerbarVertices[3].uBaseRGB.dwPacked = RGBA(0,255,0,255);	

	// print timer bars
	for(i=0;i<NUM_TIMERS;i++)
	{
		timers[i].micro = syTmrCountToMicro(timers[i].count);		

		if(timers[i].count > 0)
		{
			length = ((float)timers[i].micro / ((float)TIMER_TICKS)) * ((float)TIMER_WIDTH);

			if(i > 0)
			{
				xoffset = ((float)syTmrCountToMicro(timers[i].start - timers[0].start) / ((float)TIMER_TICKS)) * ((float)TIMER_WIDTH);
			}
			else
			{
				xoffset = 0;
			}
				
			timerbarVertices[0].fX = xoffset + TIMER_XPOS;
			timerbarVertices[0].fY = TIMER_YPOS + (TIMER_YD*i);
			timerbarVertices[0].u.fZ = 20000.0;

			timerbarVertices[1].fX = xoffset + TIMER_XPOS;
			timerbarVertices[1].fY = TIMER_YPOS + TIMER_HEIGHT + (TIMER_YD*i);
			timerbarVertices[1].u.fZ = 20000.0;

			timerbarVertices[2].fX = xoffset + TIMER_XPOS + length;
			timerbarVertices[2].fY = TIMER_YPOS + (TIMER_YD*i);
			timerbarVertices[2].u.fZ = 20000.0;

			timerbarVertices[3].fX = xoffset + TIMER_XPOS + length;
			timerbarVertices[3].fY = TIMER_YPOS + TIMER_HEIGHT + (TIMER_YD*i);
			timerbarVertices[3].u.fZ = 20000.0;

			kmStartStrip(&vertexBufferDesc, &timerbarStripHead);	
			kmSetVertex(&vertexBufferDesc, &timerbarVertices[0], KM_VERTEXTYPE_03, sizeof(KMVERTEX_03));
			kmSetVertex(&vertexBufferDesc, &timerbarVertices[1], KM_VERTEXTYPE_03, sizeof(KMVERTEX_03));
			kmSetVertex(&vertexBufferDesc, &timerbarVertices[2], KM_VERTEXTYPE_03, sizeof(KMVERTEX_03));	
			kmSetVertex(&vertexBufferDesc, &timerbarVertices[3], KM_VERTEXTYPE_03, sizeof(KMVERTEX_03));	
			kmEndStrip(&vertexBufferDesc);
		}
		
//		timers[i].start = 0;
		timers[i].count = 0;
		timers[i].micro = 0;		
	}	
}