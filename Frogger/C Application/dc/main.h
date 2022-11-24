#ifndef DCK_MAIN_H_INCLUDED
#define DCK_MAIN_H_INCLUDED

#include "include.h"

#define	R	0
#define	G	1
#define	B	2
#define	A	3

#define PALMODE			0

#define	DREAMCAST_VERSION
#define	MALLOC0			Align32Malloc
#define	FREE			Align32Free

#define PostQuitMessage(x)
#define RunDesignWorkViewer()
#define StartControllerView()
#define RunControllerView()
#define SetCurrentDisplayPage(x,y);		kmBeginScene(&kmSystemConfig);kmBeginPass(&vertexBufferDesc);	
#define SendOTDisp();					kmEndPass(&vertexBufferDesc);kmRender(KM_RENDER_FLIP);kmEndScene(&kmSystemConfig);			

#define SetSp(x)		0
#define ResetDrawingEnvironment();

#define bb_InitXA();
#define XAgetStatus()	1
#define	memoryDestroy();
#define MainReset();
	
extern KMPACKEDARGB 	borderColour;
extern KMDWORD 			FBarea[];
extern PDS_PERIPHERAL 	*per;

extern KMSTRIPCONTEXT 	stripContext00;
extern KMSTRIPHEAD 		stripHead00;
extern KMSTRIPCONTEXT 	StripContext;
extern KMSTRIPHEAD 		StripHead;

extern DCKVERTEX 		*globalVerts;

extern KMSURFACEDESC	TexList[];

extern int				matrixSize;
extern DCKMATRIX		matrixStack[];
extern DCKMATRIX		viewMatrix;

extern DCKVECTOR		camPos;
extern DCKVECTOR		camTarg;
extern float			viewAngle;
extern float			viewDist;

extern GsRVIEW2			camera;

extern KMPACKEDARGB		colourClampMin;
extern KMPACKEDARGB		colourClampMax;

extern displayPageType 	displayPage[2];
extern displayPageType 	*currentDisplayPage;

extern psFont 			*font;
extern psFont 			*fontSmall;

extern int 				drawLandscape;
extern long 			textEntry;	
extern char 			textString[255];

extern long 			drawGame;

extern unsigned long 	*sqrtable;
extern short			*acostable;

extern long 			turbo;

extern struct			gameStateStruct gameState;

extern int				skipSpecFX;

int numSfxPlaying();

extern int	startAudio;

#endif