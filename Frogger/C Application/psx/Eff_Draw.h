#ifndef EFF_DRAW_H_INCLUDED
#define	EFF_DRAW_H_INCLUDED

#include "game.h"
#include <isltex.h>
#include "specfx.h"

void DrawSpecialFX();
void ProcessShadows();
void DrawShadow( SVECTOR *pos, FVECTOR *normal, long size, long offset, long alpha, TextureType *tex );
void DrawFXDecal( SPECFX * );
void DrawFXRing( SPECFX * );                           
void DrawFXTrail( SPECFX * );                        
void CalcTrailPoints( SVECTOR * , SPECFX * , int  ); 
void DrawFXLightning( SPECFX * );                    
/* void DrawRandomPolyList( )                           */
/* void TransformAndDrawPolygon( POLYGON );             */

//croak effect stuff
#define NUM_PSX_CROAK_VTX 24
extern SVECTOR *psxCroakVtx;
void CreatePsxCroakVtx(void);
void UpdatePsxCroak(SPECFX *ring);
void DrawPsxCroak(SPECFX *ring);






#endif