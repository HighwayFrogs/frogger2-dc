/*
	This file is part of Frogger II, Copyright 2000 Interactive Studios Ltd

	File        : backdrop.c
	Description : Backdrop routines and structures
	Date        : 09/03/2000
	Version     : 0.01
*/

#include "include.h"

// -------
// Globals

BACKDROP 		backDrop;
KMSTRIPCONTEXT	backStripContext;
KMSTRIPHEAD		backStripHead;
KMVERTEX_03		backdropVertices[] =
{
{ KM_VERTEXPARAM_NORMAL,                  0,  BACKDROP_HEIGHT, 1.0f, 0.0f, 1.0f, RGBA(255,255,255,255), 0 },
{ KM_VERTEXPARAM_NORMAL,                  0,                0, 1.0f, 0.0f, 0.0f, RGBA(255,255,255,255), 0 },
{ KM_VERTEXPARAM_NORMAL,     BACKDROP_WIDTH,  BACKDROP_HEIGHT, 1.0f, 1.0f, 1.0f, RGBA(255,255,255,255), 0 },
{ KM_VERTEXPARAM_ENDOFSTRIP, BACKDROP_WIDTH,                0, 1.0f, 1.0f, 0.0f, RGBA(255,255,255,255), 0 }
};

BACKDROP 		artBackDrop;
KMSTRIPCONTEXT	artBackStripContext;
KMSTRIPHEAD		artBackStripHead;
KMVERTEX_03		artBackdropVertices[] =
{
{ KM_VERTEXPARAM_NORMAL,                  0,  BACKDROP_HEIGHT, 1.0f, 0.0f, 1.0f, RGBA(255,255,255,255), 0 },
{ KM_VERTEXPARAM_NORMAL,                  0,                0, 1.0f, 0.0f, 0.0f, RGBA(255,255,255,255), 0 },
{ KM_VERTEXPARAM_NORMAL,     BACKDROP_WIDTH,  BACKDROP_HEIGHT, 1.0f, 1.0f, 1.0f, RGBA(255,255,255,255), 0 },
{ KM_VERTEXPARAM_ENDOFSTRIP, BACKDROP_WIDTH,                0, 1.0f, 1.0f, 0.0f, RGBA(255,255,255,255), 0 }
};


// *ASL* 14/08/2000
/* --------------------------------------------------------------------------------
   Function : InitBackdrop
   Purpose : load and initialiase a backdrop
   Parameters : filename
   Returns : 
   Info :
*/

void InitBackdrop(char * const filename)
{
	int				i,x,y;
	char			buf[256],*headerPtr,*ptr;
    PKMDWORD   	 	TexturePtr,TwiddledPtr;
  	USHORT			width,height,*pixelPtr,pixel,newPixel;
  	int				headerSize,colFormat,texFormat,texAttrib,attrib;
	char			r,g,b,a;

	// exit if backdrop already initialised
	if (backDrop.init == TRUE)
		return;
	backDrop.draw = FALSE;

	// load our backdrop
	sprintf(buf,"%s.pvr",filename);
	if (loadPVRFileIntoSurface(buf, "backdrops", &backDrop.surface, FALSE) == FALSE)
		return;

	// initialise backdrop structure
	backDrop.init = TRUE;
	backDrop.draw = TRUE;
	backDrop.rect.x = 0;
	backDrop.rect.y = 0;
	backDrop.rect.w = 640; 
	backDrop.rect.h = 480;
	backDrop.imageXD = backDrop.surface.u0.USize;
	backDrop.imageYD = backDrop.surface.u1.VSize;

	// setup (u,v) coordinates vertices
	backdropVertices[0].fU = 0;
	backdropVertices[0].fV = 384.0/512;	
	backdropVertices[1].fU = 0;
	backdropVertices[1].fV = 0;	
	backdropVertices[2].fU = 1.0;
	backdropVertices[2].fV = 384.0/512;
	backdropVertices[3].fU = 1.0;
	backdropVertices[3].fV = 0;

	// initialise strip context and head
    kmInitStripContext(KM_STRIPCONTEXT_SYS_GOURAUD | KM_OPAQUE_POLYGON, &backStripContext);
	memset(&backStripContext, 0 ,sizeof(backStripContext));
	memset(&backStripHead, 0, sizeof(backStripHead));
	backStripContext.nSize = sizeof(KMSTRIPCONTEXT);
    kmInitStripContext(KM_STRIPCONTEXT_SYS_GOURAUD | KM_OPAQUE_POLYGON, &backStripContext);
	backStripContext.StripControl.nListType		 					= KM_OPAQUE_POLYGON;
	backStripContext.StripControl.nUserClipMode	 					= KM_USERCLIP_DISABLE;
	backStripContext.StripControl.nShadowMode			 			= KM_NORMAL_POLYGON;
	backStripContext.StripControl.bGouraud		 					= KM_TRUE;
	backStripContext.ObjectControl.nDepthCompare			 		= KM_GREATER;
	backStripContext.ObjectControl.nCullingMode			 			= KM_NOCULLING;
	backStripContext.ObjectControl.bZWriteDisable					= KM_TRUE;
	backStripContext.ImageControl[KM_IMAGE_PARAM1].nSRCBlendingMode	= KM_ONE;
	backStripContext.ImageControl[KM_IMAGE_PARAM1].nDSTBlendingMode	= KM_ZERO;
	backStripContext.ImageControl[KM_IMAGE_PARAM1].bSRCSelect		= KM_FALSE;
	backStripContext.ImageControl[KM_IMAGE_PARAM1].bDSTSelect		= KM_FALSE;
	backStripContext.ImageControl[KM_IMAGE_PARAM1].nFogMode			= KM_NOFOG;
	backStripContext.ImageControl[KM_IMAGE_PARAM1].bColorClamp		= KM_FALSE;
	backStripContext.ImageControl[KM_IMAGE_PARAM1].bUseAlpha		= KM_FALSE;
	backStripContext.ImageControl[KM_IMAGE_PARAM1].nFilterMode		= KM_BILINEAR;
    backStripContext.ImageControl[KM_IMAGE_PARAM1].pTextureSurfaceDesc = &backDrop.surface;
	kmGenerateStripHead03(&backStripHead,&backStripContext);
}


// *ASL* 14/08/2000
/* --------------------------------------------------------------------------------
   Function : DrawLegalBackDrop
   Purpose : draw the currently loaded background image to the screen
   Parameters : 
   Returns : 
   Info :
*/

void DrawLegalBackDrop()
{
	// setup KAMUI..
	kmBeginScene(&kmSystemConfig);
	kmBeginPass(&vertexBufferDesc);

	// only draw if initialised and flaged for drawing
	if (backDrop.init == TRUE && backDrop.draw == TRUE)
	{
		// draw the loade backdrop
		kmStartStrip(&vertexBufferDesc, &backStripHead);	
		kmSetVertex(&vertexBufferDesc, &backdropVertices[0], KM_VERTEXTYPE_03, sizeof(KMVERTEX_03));
		kmSetVertex(&vertexBufferDesc, &backdropVertices[1], KM_VERTEXTYPE_03, sizeof(KMVERTEX_03));
		kmSetVertex(&vertexBufferDesc, &backdropVertices[2], KM_VERTEXTYPE_03, sizeof(KMVERTEX_03));	
		kmSetVertex(&vertexBufferDesc, &backdropVertices[3], KM_VERTEXTYPE_03, sizeof(KMVERTEX_03));	
		kmEndStrip(&vertexBufferDesc);
	}	

	// update the fader
	DrawScreenTransition();
	
	// end KAMUI
	kmEndPass(&vertexBufferDesc);
	kmRender(KM_RENDER_FLIP);
	kmEndScene(&kmSystemConfig);
}


/* --------------------------------------------------------------------------------
   Function : FreeLegalBackdrop
   Purpose : free the currently loaded background image
   Parameters : 
   Returns : 
   Info : 
*/

void FreeLegalBackdrop()
{
	// *ASL* 12/08/2000 - Release currently initialised backdrop
	if (backDrop.init == FALSE)
		return;

	backDrop.init = FALSE;
	backDrop.draw = FALSE;
	kmFreeTexture(&backDrop.surface);

	// clear screen
	kmBeginScene(&kmSystemConfig);
	kmBeginPass(&vertexBufferDesc);
	kmEndPass(&vertexBufferDesc);
	kmRender(KM_RENDER_FLIP);
	kmEndScene(&kmSystemConfig);
}





/* --------------------------------------------------------------------------------
   Function 	: DrawBackDrop
   Purpose 	: Draw the current background image to the screen
   Parameters 	: none
	Returns 	: none
	Info 		:
*/

void DrawBackDrop(int num, int i)
{
	int	fogFade;
	
	fogFade = 100;

//	kmBeginScene(&kmSystemConfig);
//	kmBeginPass(&vertexBufferDesc);
        
	if((backDrop.init)&&(backDrop.draw))
	{
		kmStartStrip(&vertexBufferDesc, &backStripHead);	
		kmSetVertex(&vertexBufferDesc, &backdropVertices[0], KM_VERTEXTYPE_03, sizeof(KMVERTEX_03));
		kmSetVertex(&vertexBufferDesc, &backdropVertices[1], KM_VERTEXTYPE_03, sizeof(KMVERTEX_03));
		kmSetVertex(&vertexBufferDesc, &backdropVertices[2], KM_VERTEXTYPE_03, sizeof(KMVERTEX_03));	
		kmSetVertex(&vertexBufferDesc, &backdropVertices[3], KM_VERTEXTYPE_03, sizeof(KMVERTEX_03));	
		kmEndStrip(&vertexBufferDesc);
	}	
	
//	kmEndPass(&vertexBufferDesc);
				
//	kmRender(KM_RENDER_FLIP);
//	kmEndScene(&kmSystemConfig);
}

/* --------------------------------------------------------------------------------
   Function	: FreeBackdrop
   Purpose : Free the current background image
   Parameters : 
   Returns : 
   Info : release the backdrop.. no backdrops are kept loaded
*/

void FreeBackdrop(void)
{
	FreeLegalBackdrop();
}


/*	--------------------------------------------------------------------------------
	Function 	: InitArtBackdrop
	Purpose 	: Load and setup a art background image
	Parameters 	: filename
	Returns 	: none
	Info 		:
*/

extern int currentArt;

void InitArtBackdrop(char * const filename, int halfolour)
{
	int				i,x,y;
	char			buf[256],*headerPtr,*ptr;
    PKMDWORD   	 	TexturePtr,TwiddledPtr;
  	USHORT			width,height,*pixelPtr,pixel,newPixel;
  	int				headerSize,colFormat,texFormat,texAttrib,attrib;
	char			r,g,b,a;
	KMUINT32		rgba;

	artBackDrop.draw = TRUE;

	sprintf(buf,"%s.pvr",filename);
	
	artBackDrop.init = TRUE;
	artBackDrop.rect.x = 0;			// leave the y till the update
	artBackDrop.rect.y = 0;
	artBackDrop.rect.w = 640; 
	artBackDrop.rect.h = 480;

	// load the artBackDrop into a VRAM surface
	loadPVRFileIntoSurface(buf, "backdrops", &artBackDrop.surface, FALSE);
	artBackDrop.imageXD = artBackDrop.surface.u0.USize;
	artBackDrop.imageYD = artBackDrop.surface.u1.VSize;

	if((currentArt == 0)||(currentArt == 4)||(currentArt == 5)||(currentArt == 6)||
		(currentArt == 10)||(currentArt == 11)||(currentArt == 12))
		rgba = RGBA(128,128,128,255);
	else
		rgba = RGBA(255,255,255,255);

	if(halfolour)
		rgba = RGBA(255,255,255,255);

	// setup (u,v) coordinates vertices
	artBackdropVertices[0].fU = 0;
	artBackdropVertices[0].fV = 384.0/512;	
	artBackdropVertices[0].u.fZ = 1.0;
	artBackdropVertices[0].uBaseRGB.dwPacked = rgba;

	artBackdropVertices[1].fU = 0;
	artBackdropVertices[1].fV = 0;	
	artBackdropVertices[1].u.fZ = 1.0;
	artBackdropVertices[1].uBaseRGB.dwPacked = rgba;
	
	artBackdropVertices[2].fU = 1.0;
	artBackdropVertices[2].fV = 384.0/512;
	artBackdropVertices[2].u.fZ = 1.0;
	artBackdropVertices[2].uBaseRGB.dwPacked = rgba;
	
	artBackdropVertices[3].fU = 1.0;
	artBackdropVertices[3].fV = 0;
	artBackdropVertices[3].u.fZ = 1.0;
	artBackdropVertices[3].uBaseRGB.dwPacked = rgba;
	
	// initialise strip context and head
    kmInitStripContext(KM_STRIPCONTEXT_SYS_GOURAUD | KM_TRANS_POLYGON, &artBackStripContext);
	memset(&artBackStripContext,0,sizeof(artBackStripContext));
	memset(&artBackStripHead,0,sizeof(artBackStripHead));
	artBackStripContext.nSize = sizeof(KMSTRIPCONTEXT);
    kmInitStripContext(KM_STRIPCONTEXT_SYS_GOURAUD | KM_TRANS_POLYGON, &artBackStripContext);
	artBackStripContext.StripControl.nListType		 					= KM_TRANS_POLYGON;
	artBackStripContext.StripControl.nUserClipMode	 					= KM_USERCLIP_DISABLE;
	artBackStripContext.StripControl.nShadowMode			 			= KM_NORMAL_POLYGON;
	artBackStripContext.StripControl.bGouraud		 					= KM_TRUE;
	artBackStripContext.ObjectControl.nDepthCompare			 			= KM_ALWAYS;
	artBackStripContext.ObjectControl.nCullingMode			 			= KM_NOCULLING;
	artBackStripContext.ObjectControl.bZWriteDisable					= KM_FALSE;
	artBackStripContext.ImageControl[KM_IMAGE_PARAM1].nSRCBlendingMode	= KM_SRCALPHA;
	artBackStripContext.ImageControl[KM_IMAGE_PARAM1].nDSTBlendingMode	= KM_INVSRCALPHA;
	artBackStripContext.ImageControl[KM_IMAGE_PARAM1].bSRCSelect		= KM_FALSE;
	artBackStripContext.ImageControl[KM_IMAGE_PARAM1].bDSTSelect		= KM_FALSE;
	artBackStripContext.ImageControl[KM_IMAGE_PARAM1].nFogMode			= KM_NOFOG;
	artBackStripContext.ImageControl[KM_IMAGE_PARAM1].bColorClamp		= KM_FALSE;
	artBackStripContext.ImageControl[KM_IMAGE_PARAM1].bUseAlpha			= KM_FALSE;
	artBackStripContext.ImageControl[KM_IMAGE_PARAM1].nFilterMode		= KM_BILINEAR;
    artBackStripContext.ImageControl[KM_IMAGE_PARAM1].pTextureSurfaceDesc = &artBackDrop.surface;
	kmGenerateStripHead03(&artBackStripHead,&artBackStripContext);
}

/*	--------------------------------------------------------------------------------
	Function 	: DrawArtBackDrop
	Purpose 	: Draw the current background image to the screen
	Parameters 	: none
	Returns 	: none
	Info 		:
*/

void DrawArtBackDrop(int num, int i)
{
	int	fogFade;
	
	fogFade = 100;

//	kmBeginScene(&kmSystemConfig);
//	kmBeginPass(&vertexBufferDesc);
        
	if(artBackDrop.draw)
	{
		kmStartStrip(&vertexBufferDesc, &artBackStripHead);	
		kmSetVertex(&vertexBufferDesc, &artBackdropVertices[0], KM_VERTEXTYPE_03, sizeof(KMVERTEX_03));
		kmSetVertex(&vertexBufferDesc, &artBackdropVertices[1], KM_VERTEXTYPE_03, sizeof(KMVERTEX_03));
		kmSetVertex(&vertexBufferDesc, &artBackdropVertices[2], KM_VERTEXTYPE_03, sizeof(KMVERTEX_03));	
		kmSetVertex(&vertexBufferDesc, &artBackdropVertices[3], KM_VERTEXTYPE_03, sizeof(KMVERTEX_03));	
		kmEndStrip(&vertexBufferDesc);
	}	
	
//	kmEndPass(&vertexBufferDesc);
				
//	kmRender(KM_RENDER_FLIP);
//	kmEndScene(&kmSystemConfig);
}

/*	--------------------------------------------------------------------------------
	Function 	: FreeArtBackdrop
	Purpose 	: Free the current background image
	Parameters 	: none
	Returns 	: none
	Info 		:
*/

void FreeArtBackdrop(void)
{
	if(!artBackDrop.draw)
		return;

	artBackDrop.draw = FALSE;
	kmFreeTexture(&artBackDrop.surface);
}
