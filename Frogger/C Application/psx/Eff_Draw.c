
#include "particle.h"
#include "specfx.h"
#include "Eff_Draw.h"
#include "cam.h"
#include "enemies.h"
#include "platform.h"
#include "collect.h"
#include "frogmove.h"
#include "main.h"
#include "tongue.h"
#include "map_draw.h"
#include "actor2.h"
#include "fixed.h"
#include "layout.h"

/*ma
#define gte_stotz_cpu(r)\
asm(\
  "mfc2 %0, $7;"\
  :\
  : "r" (r)\
)

#define addPrimLen(ot,p,len,tempvar) \
{\
	tempvar = *(u_long *)(ot); \
    (*(u_long *)(ot) = (u_long)(p)); \
	tempvar |= (len<<24); \
	*(unsigned long *)(p) = tempvar; \
}
*/


// *ASL* 28/06/2000
/* --------------------------------------------------------------------------------
   Function : DrawSpecialFX
   Purpose : draw all effects from sfxlist
   Parameters : 
   Returns : 
   Info : 
*/

void DrawSpecialFX(void)
{
	SPECFX	*fx;
	int		i;

	ProcessShadows();

	// draw all special effects from list
	if (sfxList.count > 0)
		for (fx = sfxList.head.next; fx != &sfxList.head; fx = fx->next)
		{
			if (fx->Draw)
				fx->Draw(fx);
		}

	// draw frog tongue 
	for (i=0; i<NUM_FROGS; i++)
		if (tongue[i].flags & TONGUE_BEINGUSED)
		{
			DrawTongue(i);
		}
}






/*	--------------------------------------------------------------------------------
	Function		: ProcessShadows
	Purpose			: processes the shadows
	Parameters		: 
	Returns			: void
	Info			: 
*/
void ProcessShadows()
{
	FVECTOR vec, up;
	SVECTOR pos, tilePos;
	ENEMY *nme;
	PLATFORM *plat;
	TextureType *tex;
	long i, scale;
	fixed height, size;
	short alpha;
	
	tex = txtrSolidRing;

	for( i=0; i<NUM_FROGS; i++ )
		if( frog[i]->actor->shadow && frog[i]->draw && frog[i]->actor->shadow->draw )
		{
			// Get proper positions if a frog is on a platform
			if( currPlatform[i] )
			{
				SetVectorSS( &tilePos, &currPlatform[i]->pltActor->actor->position );
				SetVectorFF( &up, &currPlatform[i]->inTile[0]->normal );
			}
			else
			{
				SetVectorSS( &tilePos, &currTile[i]->centre );
				SetVectorFF( &up, &currTile[i]->normal );
			}

			// Calculate height above ground
			SetVectorSS( &pos, &frog[i]->actor->position );
			SubVectorFSS( &vec, &pos, &tilePos );
			height = DotProductFF( &vec, &up );

			scale = max( FMul(height,8), 4096 );

			size = FDiv( frog[i]->actor->shadow->radius, scale );
			alpha = FDiv( ToFixed(frog[i]->actor->shadow->alpha), scale )>>13;

			DrawShadow( &pos, &up, size>>12, -(height>>12)+10, alpha, tex );
		}

	//------------------------------------------------------------------------------------------------

	// process enemy shadows
	for(nme = enemyList.head.next; nme != &enemyList.head; nme = nme->next)
	{
		if( !nme->active || !nme->nmeActor )
			continue;

		if(nme->nmeActor->actor->shadow && nme->inTile && !nme->nmeActor->clipped )
		{
			SetVectorSS( &pos, &nme->nmeActor->actor->position );
			SetVectorSS( &tilePos, &nme->inTile->centre );
			SetVectorFF( &up, &nme->inTile->normal );

			SubVectorFSS( &vec, &pos, &tilePos );
			height = DotProductFF( &vec, &up );

			scale = max( FMul(height,8), 4096 );

			size = FDiv( nme->nmeActor->actor->shadow->radius, scale );
			alpha = FDiv( ToFixed(nme->nmeActor->actor->shadow->alpha), scale )>>13;

			if (nme->path->nodes[nme->path->fromNode].worldTile==nme->inTile)
				DrawShadow( &pos, &up, size>>12, -(height>>12)+10, alpha, tex );
			else
				DrawShadow( &pos, &up, size>>12, -(height>>12)+10, alpha, tex );
		}
	}

	// process platform shadows
	for(plat = platformList.head.next; plat != &platformList.head; plat = plat->next)
	{
		if( !plat->active || !plat->pltActor )
			continue;

		if(plat->pltActor->actor->shadow && plat->inTile[0] && !plat->pltActor->clipped )
		{
			SetVectorSS( &pos, &plat->pltActor->actor->position );
			SetVectorSS( &tilePos, &plat->inTile[0]->centre );
			SetVectorFF( &up, &plat->inTile[0]->normal );

			SubVectorFSS( &vec, &pos, &tilePos );
			height = DotProductFF( &vec, &up );

			scale = max( FMul(height,8), 4096 );

			size = FDiv( plat->pltActor->actor->shadow->radius, scale );
			alpha = FDiv( ToFixed(plat->pltActor->actor->shadow->alpha), scale )>>13;

			DrawShadow( &pos, &up, size>>12, -(height>>12)+10, alpha, tex );
		}
	}
}







void DrawShadow( SVECTOR *pos, FVECTOR *normal, long size, long offset, long alpha, TextureType *tex )
{
	FVECTOR tempV[4];
	SVECTOR vT[4];
	IQUATERNION cross, q;
	long i, colour = 8421504;
	fixed t;
	static SPECFX fx;

//PUTS THE SPRITES RGB'S IN COLOUR, FIRST HALVING THEIR VALUES
//	colour = 255>>1;// 191;;
//	colour += 255>>1 <<8;//255<<8;
//	colour += 255>>1 <<16;//0<<16;

	vT[0].vx = size;
	vT[0].vy = offset;
	vT[0].vz = size;
	
	vT[1].vx = size;
	vT[1].vy = offset;
	vT[1].vz = -size;

	vT[2].vx = -size;
	vT[2].vy = offset;
	vT[2].vz = -size;
	
	vT[3].vx = -size;
	vT[3].vy = offset;
	vT[3].vz = size;

	// Rotate to be around normal
	CrossProductFFF( (FVECTOR *)&cross, normal, &upVec );
	MakeUnit( (FVECTOR *)&cross );
	t = DotProductFF( normal, &upVec );
	cross.w = -arccos(t);
	fixedGetQuaternionFromRotation( &q, &cross );

	for( i=3; i>=0; i-- )
	{
		SetVectorFS(&tempV[i],&vT[i]);
	   	RotateVectorByQuaternionFF(&tempV[i],&tempV[i],&q);
	  	SetVectorSF(&vT[i],&tempV[i]);								

		AddToVectorSS( &vT[i], pos );
	}

	fx.tex = tex;
	fx.flags = SPRITE_SUBTRACTIVE;
	fx.a = alpha;
	fx.zDepthOff = -1;	//bring the shadow closer to the camera
	fx.r = fx.g = fx.b = 0xff;
	
	Print3D3DSprite ( &fx, vT, colour );
}



		   
void DrawFXDecal( SPECFX *ripple )
{
	FVECTOR tempV[4];
	SVECTOR vT[4];
	IQUATERNION q1, q2, q3;
	fixed t;
	unsigned long colour;
	long i;

//PUTS THE SPRITES RGB'S IN COLOUR, FIRST HALVING THEIR VALUES
	colour = ripple->r>>1;
	colour += (ripple->g>>1)<<8;
	colour += (ripple->b>>1)<<16;

//set the size of the effect
	vT[0].vx = (ripple->scale.vx>>12);
	vT[0].vy = 0;
	vT[0].vz = (ripple->scale.vz>>12);

	vT[1].vx = (ripple->scale.vx>>12);
	vT[1].vy = 0;
	vT[1].vz = (-ripple->scale.vz>>12);

	vT[2].vx = (-ripple->scale.vx>>12) ;
	vT[2].vy = 0;
	vT[2].vz = (-ripple->scale.vz>>12) ;
	
	vT[3].vx = (-ripple->scale.vx>>12);
	vT[3].vy = 0;
	vT[3].vz = (ripple->scale.vz>>12) ;


// Rotate to be around normal
	CrossProductFFF((FVECTOR *)&q1, &ripple->normal, &upVec ); 
	MakeUnit((FVECTOR *)&q1 );								   
	t = DotProductFF( &ripple->normal, &upVec );			  	   
	q1.w = -arccos(t);								   
	fixedGetQuaternionFromRotation( &q2, &q1 );		   

	if( ripple->type == FXTYPE_GARIBCOLLECT )		   
	{												   
		// Rotate around axis						   
		SetVectorFF((FVECTOR *)&q1, &ripple->normal ); 
		q1.w = ripple->angle;						   
		fixedGetQuaternionFromRotation( &q3, &q1 );	   
		fixedQuaternionMultiply( &q1, &q2, &q3 );	   
	}												   
	else SetQuaternion( &q1, &q2 );					   


	for( i=3; i>=0; i-- )
	{

		SetVectorFS(&tempV[i],&vT[i]);
	   	RotateVectorByQuaternionFF(&tempV[i],&tempV[i],&q1);
	  	SetVectorSF(&vT[i],&tempV[i]);								

	//add world coords 
		vT[i].vx += ripple->origin.vx;
		vT[i].vy += ripple->origin.vy;
		vT[i].vz += ripple->origin.vz;

	}

//	#ifdef PSX_VERSION
	Print3D3DSprite ( ripple, vT, colour );
//	#else
//	Print3D3DSprite ( ripple->tex, vT, colour,ripple->a );
//	#endif
}



// 



/* --------------------------------------------------------------------------------
   Function : DrawFXRing
   Purpose : draw special effects ring (used in croak)
   Parameters : special effects def pointer
   Returns : 
   Info : 
*/

void DrawFXRing(SPECFX *fx)
{
	unsigned long	vx, i, j, vxj;
	VERT			vT[5], vTPrev[2];
	TextureType		*tEntry;
	FVECTOR			scale, normal;
	VECTOR			tempVect;
	SVECTOR			pos, m;
	IQUATERNION		q1, q3, cross;
	SVECTOR			fxpos;
	fixed			tilt, tilt2, t;
	int				zeroZ;
	int				r, g, b;
	MATRIX			tMtrx, rMtrx, sMtrx, tempMtrx;
	long			otz;


	// load texture entry.. NULL?
	if ((tEntry = fx->tex) == NULL)
		return;

	zeroZ = 0;

	vTPrev[0].vz = 0;
	vTPrev[1].vz = 0;
	vTPrev[2].vz = 0;
	vTPrev[3].vz = 0;
		
	SetVectorFF(&scale, &fx->scale);
	
	// scale down the scale vector
	scale.vx /= 150L;
	scale.vy /= 150L;
	scale.vz /= 150L;

//	scale.vx = 500;	//asl my line
//	scale.vy = 500;	//asl my line
//	scale.vz = 500;	//asl my line

	SetVectorFF(&normal, &fx->normal);
	SetVectorSS(&pos, &fx->origin);

//	pos.vx -= 1000;	//asl my line
//	pos.vz -= 1000;	//asl my line

	// translate to current effect position
	tMtrx = GsIDMATRIX;
	tMtrx.t[0] = -pos.vx;
	tMtrx.t[1] = -pos.vy;
	tMtrx.t[2] =  pos.vz;

	// Rotate around axis
	SetVectorFF((FVECTOR*)&q1, &normal);
	
	// rotate to be around the normal
	CrossProductFFF((FVECTOR *)&cross, (FVECTOR *)&q1, &upVec);
	MakeUnit((FVECTOR *)&cross);
	t = DotProductFF((FVECTOR *)&q1, &upVec);
	cross.w = -arccos(t);
	fixedGetQuaternionFromRotation(&q3, &cross);

	// combine the rotations and push
	QuatToPSXMatrix(&q3, &rMtrx);

	tilt2 = fx->tilt;

	r = (fx->r*fx->a) >>8;
	g = (fx->g*fx->a) >>8;
	b = (fx->b*fx->a) >>8;

	// initialise our effect vertices
	memset(vT, 0, sizeof(VERT)*4);
	memset(vTPrev, 0, sizeof(VERT)*2);					// *ASL* 28/06/2000 - !!make sure these are initialised!!

	tEntry->u0 = 0;
	tEntry->v0 = 0;
	tEntry->u1 = 1;
	tEntry->v1 = 0;
	tEntry->u2 = 1;
	tEntry->v2 = 1;
	tEntry->u3 = 0;
	tEntry->v3 = 1;
	
	vT[0].tu = tEntry->u2;
	vT[0].tv = tEntry->v2;
	vT[1].tu = tEntry->u0;
	vT[1].tv = tEntry->v0;
	vT[2].tu = tEntry->u1;
	vT[2].tv = tEntry->v1;
	vT[3].tu = tEntry->u3;
	vT[3].tv = tEntry->v3;
	

	for (i=0, vx=0; i < NUM_RINGSEGS; i++, vx+=2)
	{
		// transform to proper coords
		for (j=0, zeroZ=0; j<4; j++)
		{
			if (i && j<2 && vTPrev[0].vz && vTPrev[1].vz)
				memcpy(vT, vTPrev, sizeof(VERT)*2);
			else
			{
				FVECTOR tempFVect;
				int flg;

				vxj = (vx+j)%(NUM_RINGSEGS<<1);
				vT[j].vx = ringVtx[vxj].vx;
				vT[j].vy = ringVtx[vxj].vy;
				vT[j].vz = ringVtx[vxj].vz;

				// Slant the polys
				tilt = (!(i&1)?(j==0||j==3):(j==1||j==2)) ? 4096 : tilt2;
				vT[j].tv = tEntry->v2-(vT[j].tv-tEntry->v0);

				tempFVect.vx = FMul(tilt,scale.vx>>4);
				tempFVect.vy = FMul(tilt,scale.vy>>4);
				tempFVect.vz = FMul(tilt,scale.vz>>4);
				sMtrx = GsIDMATRIX;
				ScaleMatrix(&sMtrx, (VECTOR *)&tempFVect);

				// Transform point by combined matrix
				tempVect.vx = -vT[j].vx;
				tempVect.vy = -vT[j].vy;
				tempVect.vz =  vT[j].vz;
				ApplyMatrixLV(&sMtrx, &tempVect, &tempVect);
				ApplyMatrixLV(&rMtrx, &tempVect, &tempVect);

				tempVect.vx -= pos.vx;
				tempVect.vy -= pos.vy;
				tempVect.vz += pos.vz;

				if(j==3)
				{
					fxpos.vx = -tempVect.vx;
					fxpos.vy = -tempVect.vy;
					fxpos.vz = tempVect.vz;
				}

				gte_SetTransMatrix(&GsWSMATRIX);
				gte_SetRotMatrix(&GsWSMATRIX);
				gte_ldlv0(&tempVect);
				gte_rtps();
				gte_stsxy(&m.vx);
//ma			gte_stszotz(&otz);				//asl
//ma			m.vz = (short)otz;				//asl
//ma			m.vz>>=2;
				m.vz = (screenxy[2].vz >> 2);

//ma			gte_stflg(&flg);				// screen z/4 as otz

				// Assign back to vT array
				vT[j].vx = m.vx;
				vT[j].vy = m.vy;
				vT[j].vz = m.vz;				//asl
//asl				if( (m.vz<60) || (m.vz>1000) )
//asl					zeroZ++;
//asl				else
//asl					vT[j].vz = m.vz;
			}
		}

		if (zeroZ == 0)
		{
			// load the correct texture u's into the vertices
			vT[0].tu = tEntry->u2;
			vT[1].tu = tEntry->u0;
			vT[2].tu = tEntry->u1;
			vT[3].tu = tEntry->u3;

			// copy print vertices to previous vertices. Saves re-calculating them again above
			memcpy( vTPrev, &vT[2], sizeof(VERT)*2 );
			// this really isn't needed as i suspect they are drawing two tris
			memcpy( &vT[4], &vT[0], sizeof(VERT) );

			// setup all dreamcast vertices
			vertices_GT4[0].fX = vT[0].vx;
			vertices_GT4[0].fY = vT[0].vy;
			vertices_GT4[0].u.fZ = 1.0f / (float)vT[0].vz;
			vertices_GT4[0].fU = vT[0].tu;
			vertices_GT4[0].fV = vT[0].tv;
			vertices_GT4[0].uBaseRGB.dwPacked = RGBA(fx->r, fx->g, fx->b, fx->a);

			vertices_GT4[1].fX = vT[1].vx;
			vertices_GT4[1].fY = vT[1].vy;
			vertices_GT4[1].u.fZ = 1.0f / (float)vT[1].vz;
			vertices_GT4[1].fU = vT[1].tu;
			vertices_GT4[1].fV = vT[1].tv;
			vertices_GT4[1].uBaseRGB.dwPacked = RGBA(fx->r, fx->g, fx->b, fx->a);

			vertices_GT4[2].fX = vT[3].vx;
			vertices_GT4[2].fY = vT[3].vy;
			vertices_GT4[2].u.fZ = 1.0f / (float)vT[3].vz;
			vertices_GT4[2].fU = vT[3].tu;
			vertices_GT4[2].fV = vT[3].tv;
			vertices_GT4[2].uBaseRGB.dwPacked = RGBA(fx->r, fx->g, fx->b, fx->a);

			vertices_GT4[3].fX = vT[2].vx;
			vertices_GT4[3].fY = vT[2].vy;
			vertices_GT4[3].u.fZ = 1.0f / (float)vT[2].vz;
			vertices_GT4[3].fU = vT[2].tu;
			vertices_GT4[3].fV = vT[2].tv;
			vertices_GT4[3].uBaseRGB.dwPacked = RGBA(fx->r, fx->g, fx->b, fx->a);

			// initialise additive polygon strip header
			kmChangeStripTextureSurface(&StripHead_Sprites_Add,KM_IMAGE_PARAM1,tEntry->surfacePtr);

			// initiliase strip vertices
			kmStartStrip(&vertexBufferDesc, &StripHead_Sprites_Add);
			kmSetVertex(&vertexBufferDesc, &vertices_GT4[0], KM_VERTEXTYPE_03, sizeof(KMVERTEX_03));
			kmSetVertex(&vertexBufferDesc, &vertices_GT4[1], KM_VERTEXTYPE_03, sizeof(KMVERTEX_03));
			kmSetVertex(&vertexBufferDesc, &vertices_GT4[2], KM_VERTEXTYPE_03, sizeof(KMVERTEX_03));	
			kmSetVertex(&vertexBufferDesc, &vertices_GT4[3], KM_VERTEXTYPE_03, sizeof(KMVERTEX_03));	

			// ..and terminate
			kmEndStrip(&vertexBufferDesc);			

			if(((actFrameCount MOD 2) == 0) && (i&1))
			{
				SPECFX *trail;

				if((trail = CreateSpecialEffect(FXTYPE_TWINKLE, &fxpos, &fx->normal, 81920, 0, 0, 2048)))
				{
					trail->tilt = 8192;
					if(i&2)
						SetFXColour(trail, 180, 220, 180);
					else
						SetFXColour(trail, fx->r, fx->g, fx->b);
				}
			}
		}
		else
		{
			vTPrev[0].vz = vTPrev[1].vz = 0;
		}
	}
}







void DrawFXTrail( SPECFX *trail )
{
 	unsigned long colour, i = trail->start;
 	SVECTOR vT[4], vTPrev[2];
 
 	if( trail->start == trail->end )
 		return;


//PUTS THE SPRITES RGB'S IN COLOUR, FIRST HALVING THEIR VALUES
	colour = trail->r>>1;
	colour += (trail->g>>1)<<8;
	colour += (trail->b>>1)<<16; 
 
 	vTPrev[0].vz = 0;
 	vTPrev[1].vz = 0;
 
 	do
 	{
 		/*********-[ First 2 points ]-********/
 		if( i != trail->start && vTPrev[0].vz && vTPrev[1].vz )
 			memcpy( vT, vTPrev, sizeof(SVECTOR)*2 );			// Previously transformed vertices
 		else
 			CalcTrailPoints( vT, trail, i );
 
 		/*********-[ Next 2 points ]-********/
 		CalcTrailPoints( &vT[2], trail, (i+1)%trail->numP );
 		memcpy( vTPrev, &vT[2], sizeof(SVECTOR)*2 ); 			// Store first 2 vertices of the next segment
 
//add world coords 
		vT[0].vx += trail->origin.vx;
		vT[0].vy += trail->origin.vy;
		vT[0].vz += trail->origin.vz;

		vT[1].vx += trail->origin.vx;
		vT[1].vy += trail->origin.vy;
		vT[1].vz += trail->origin.vz;

		vT[2].vx += trail->origin.vx;
		vT[2].vy += trail->origin.vy;
		vT[2].vz += trail->origin.vz;

		vT[3].vx += trail->origin.vx;
		vT[3].vy += trail->origin.vy;
		vT[3].vz += trail->origin.vz;

//		#ifdef PSX_VERSION
		Print3D3DSprite ( trail, vT, colour );
//		#else
//		Print3D3DSprite ( trail->tex, vT, colour,ripple->a );
//		#endif

 
 		if( ++i >= trail->numP ) i=0;
 
 	} while( i != trail->end );
}




void CalcTrailPoints( SVECTOR *vT, SPECFX *trail, int i )
{
	FVECTOR tempV[2];
	IQUATERNION q = {0,0,0,4096};
	SVECTOR pos;

	AddVectorSSS( &pos, &trail->origin, &trail->particles[i].pos );

	if( trail->type == FXTYPE_BILLBOARDTRAIL )	// Calculate screen align rotation
	{
		FVECTOR normal;
		IQUATERNION cross;
		fixed t;

		SubVectorFFS( &normal, &currCamSource, &pos );
		MakeUnit( &normal );
		CrossProductFFF( (FVECTOR *)&cross, &normal, &upVec );
		MakeUnit( (FVECTOR *)&cross );
		t = DotProductFF( &normal, &upVec );
		cross.w = arccos(t);
		fixedGetQuaternionFromRotation( &q, &cross );
	}

	vT[0].vx = trail->particles[i].poly[0].vx;
	vT[0].vy = trail->particles[i].poly[0].vy;
	vT[0].vz = trail->particles[i].poly[0].vz;
	vT[1].vx = trail->particles[i].poly[1].vx;
	vT[1].vy = trail->particles[i].poly[1].vy;
	vT[1].vz = trail->particles[i].poly[1].vz;


	SetVectorFS(&tempV[0], &vT[0]);

	RotateVectorByQuaternionFF(&tempV[0],&tempV[0],&q);

	SetVectorSF(&vT[0],&tempV[0]);

	SetVectorFS(&tempV[1], &vT[1]);

	RotateVectorByQuaternionFF(&tempV[1],&tempV[1],&q);

	SetVectorSF(&vT[1],&tempV[1]);

 	ScaleVector(&vT[0],SCALE);
 	ScaleVector(&vT[1],SCALE);

}



							   

void DrawFXLightning( SPECFX *fx )
{
	SVECTOR vT[4], vTPrev[2], tempSvect;
	TextureType *tEntry;
	PARTICLE *p;
	long i=0, otz, clipped, sz;
	POLY_FT4 *ft4;
	KMUINT32	rgba;
	
	if( !(tEntry = (fx->tex)) )
		return;

	if(gameState.mode == LEVELCOMPLETE_MODE)
		return;

//	gte_SetTransMatrix(&GsWSMATRIX);
//	gte_SetRotMatrix(&GsWSMATRIX);

	vTPrev[0].vz = 0;
	vTPrev[1].vz = 0;
	
	p = fx->particles;
	while( i < fx->numP-1 )
	{
		clipped = 0;
		// Copy in previous transformed vertices, if they exist
		if( i && vTPrev[0].vz && vTPrev[1].vz )
			memcpy( (long *)vT, (long *)vTPrev, 4 );
		else
		{
			// Otherwise, transform them again
			tempSvect.vx = -p->poly[0].vx;
			tempSvect.vy = -p->poly[0].vy;
			tempSvect.vz = p->poly[0].vz;
			gte_ldv0(&tempSvect);
			gte_rtps();
			gte_stsxy(&vT[0].vx);
			gte_stszotz(&otz);
			vT[0].vz = otz;
			gte_stsz(&sz);
//			clipped += ( vT[0].vx > 640 || vT[0].vx < 0 ||
//						vT[0].vy > 480 || vT[0].vy < 0/* ||
//						sz<20 || sz>fog.max*/);

			tempSvect.vx = -p->poly[1].vx;
			tempSvect.vy = -p->poly[1].vy;
			tempSvect.vz = p->poly[1].vz;
			gte_ldv0(&tempSvect);
			gte_rtps();
			gte_stsxy(&vT[1].vx);
			gte_stszotz(&otz);
			vT[1].vz = otz;
			gte_stsz(&sz);
//			clipped += ( vT[1].vx > 640 || vT[1].vx < 0 ||
//						vT[1].vy > 480 || vT[1].vy < 0/* ||
//						sz<20 || sz>fog.max*/);
		}

		tempSvect.vx = -p->next->poly[1].vx;
		tempSvect.vy = -p->next->poly[1].vy;
		tempSvect.vz = p->next->poly[1].vz;
		gte_ldv0(&tempSvect);
		gte_rtps();
		gte_stsxy(&vT[2].vx);
		gte_stszotz(&otz);
		vT[2].vz = otz;
		gte_stsz(&sz);
//		clipped += ( vT[2].vx > 640 || vT[2].vx < 0 ||
//					vT[2].vy > 480 || vT[2].vy < 0/* ||
//					sz<20 || sz>fog.max*/);

		tempSvect.vx = -p->next->poly[0].vx;
		tempSvect.vy = -p->next->poly[0].vy;
		tempSvect.vz = p->next->poly[0].vz;
		gte_ldv0(&tempSvect);
		gte_rtps();
		gte_stsxy(&vT[3].vx);
		gte_stszotz(&otz);
		vT[3].vz = otz;
		gte_stsz(&sz);
//		clipped += ( vT[3].vx > 640 || vT[3].vx < 0 ||
//					vT[3].vy > 480 || vT[3].vy < 0 /*||
//					sz<20 || sz>fog.max*/);

		// Store first 2 vertices of the next segment
		memcpy( (long *)vTPrev, (long *)&vT[3], 2 );
		memcpy( (long *)&vTPrev[1], (long *)&vT[2], 2 );

//		if(((vT[0].vx < 640)&&(vT[1].vx < 640)&&(vT[2].vx < 640)&&(vT[3].vx < 640))&&		
//		((vT[0].vx >= 0)&&(vT[1].vx >= 0)&&(vT[2].vx >= 0)&&(vT[3].vx >= 0))&&
//		((vT[0].vy < 480)&&(vT[1].vy < 480)&&(vT[2].vy < 480)&&(vT[3].vy < 480))&&
//		((vT[0].vy >= 0)&&(vT[1].vy >= 0)&&(vT[2].vy >= 0)&&(vT[3].vy >= 0)))
//			clipped = FALSE;
			
		vT[0].vz = vT[0].vz << 2;
		vT[1].vz = vT[1].vz << 2;
		vT[2].vz = vT[2].vz << 2;
		vT[3].vz = vT[3].vz << 2;
	
/*		if(vT[0].vz == 0)
			vT[0].vz = 1;
		if(vT[1].vz == 0)
			vT[1].vz = 1;
		if(vT[2].vz == 0)
			vT[2].vz = 1;
		if(vT[3].vz == 0)
			vT[3].vz = 1;
*/		
		// Draw polys, if they're not clipped
		if( !clipped )
		{
			rgba = RGBA(fx->r,fx->g,fx->b,fx->a);

			vertices_GT4[0].fX = vT[0].vx;
			vertices_GT4[0].fY = vT[0].vy;
			vertices_GT4[0].u.fZ = 1.0 / ((float)vT[0].vz);
			vertices_GT4[0].fU = 1.0;
			vertices_GT4[0].fV = 0.0;
			vertices_GT4[0].uBaseRGB.dwPacked = rgba;

			vertices_GT4[1].fX = vT[1].vx;
			vertices_GT4[1].fY = vT[1].vy;
			vertices_GT4[1].u.fZ = 1.0 / ((float)vT[1].vz);
			vertices_GT4[1].fU = 0.0;
			vertices_GT4[1].fV = 0.0;
			vertices_GT4[1].uBaseRGB.dwPacked = rgba;

			vertices_GT4[2].fX = vT[2].vx;
			vertices_GT4[2].fY = vT[2].vy;
			vertices_GT4[2].u.fZ = 1.0 / ((float)vT[2].vz);
			vertices_GT4[2].fU = 1.0;
			vertices_GT4[2].fV = 1.0;
			vertices_GT4[2].uBaseRGB.dwPacked = rgba;

			vertices_GT4[3].fX = vT[3].vx;
			vertices_GT4[3].fY = vT[3].vy;
			vertices_GT4[3].u.fZ = 1.0 / ((float)vT[3].vz);
			vertices_GT4[3].fU = 0.0;
			vertices_GT4[3].fV = 1.0;
			vertices_GT4[3].uBaseRGB.dwPacked = rgba;

//			if(fx->tex->colourKey)
//			{
//				kmChangeStripTextureSurface(&StripHead_3DSprites,KM_IMAGE_PARAM1,fx->tex->surfacePtr);
//				kmStartStrip(&vertexBufferDesc, &StripHead_3DSprites);	
//			}
//			else
//			{
				kmChangeStripTextureSurface(&StripHead_Sprites_Add,KM_IMAGE_PARAM1,fx->tex->surfacePtr);
				kmStartStrip(&vertexBufferDesc, &StripHead_Sprites_Add);	
//			}	
//			kmChangeStripTextureSurface(&StripHead_3DSprites,KM_IMAGE_PARAM1,fx->tex->surfacePtr);
	
//			kmStartStrip(&vertexBufferDesc, &StripHead_3DSprites);	
			kmSetVertex(&vertexBufferDesc, &vertices_GT4[0], KM_VERTEXTYPE_03, sizeof(KMVERTEX_03));
			kmSetVertex(&vertexBufferDesc, &vertices_GT4[1], KM_VERTEXTYPE_03, sizeof(KMVERTEX_03));
			kmSetVertex(&vertexBufferDesc, &vertices_GT4[2], KM_VERTEXTYPE_03, sizeof(KMVERTEX_03));	
			kmSetVertex(&vertexBufferDesc, &vertices_GT4[3], KM_VERTEXTYPE_03, sizeof(KMVERTEX_03));	
			kmEndStrip(&vertexBufferDesc);		
		
/*			BEGINPRIM(ft4, POLY_FT4);
			setPolyFT4(ft4);
			*((long*)&ft4->x0) = *((long*)&vT[0].vx);
			*((long*)&ft4->x1) = *((long*)&vT[1].vx);
			*((long*)&ft4->x2) = *((long*)&vT[2].vx);
			*((long*)&ft4->x3) = *((long*)&vT[3].vx);

			ft4->r0 = (p->r*p->a)>>8;
			ft4->g0 = (p->g*p->a)>>8;
			ft4->b0 = (p->b*p->a)>>8;
			ft4->u0 = tEntry->u0;
			ft4->v0 = tEntry->v0;
			ft4->u1 = tEntry->u1;
			ft4->v1 = tEntry->v1;
			ft4->u2 = tEntry->u2;
			ft4->v2 = tEntry->v2;
			ft4->u3 = tEntry->u3;
			ft4->v3 = tEntry->v3;
			ft4->tpage = tEntry->tpage;
			ft4->clut  = tEntry->clut;

			ft4->code  |= 2;//semi-trans on
 			ft4->tpage |= 32;//add

			ENDPRIM(ft4, (vT[0].vz+vT[1].vz+vT[2].vz+vT[3].vz)>>4, POLY_FT4);
*/			
		}

		i++;
		p = p->next;
	} 
}
 
SVECTOR *psxCroakVtx = NULL;
void CreatePsxCroakVtx(void)
{
	fixed angStep = 4096/NUM_PSX_CROAK_VTX;
	fixed ang=0;
	int i;

	if(!psxCroakVtx)
	{
		psxCroakVtx = MALLOC0( sizeof(VECTOR) * NUM_PSX_CROAK_VTX );
		if(!psxCroakVtx)
			utilPrintf("\n\nMALLOC ERROR ! ! ! psxCroakVtx\n\n");
	}


	for(i=0; i<NUM_PSX_CROAK_VTX; i++)
	{
		//coords with radius 4096
		psxCroakVtx[i].vx = rsin(ang);
		psxCroakVtx[i].vy = 0;
		psxCroakVtx[i].vz = rcos(ang);

		//set radius to 512 (1 tile)
		psxCroakVtx[i].vx >>= 3;
		psxCroakVtx[i].vz >>= 3;

		ang += angStep;
	}
}

void UpdatePsxCroak(SPECFX *ring)
{
	int fo;
	fixed speed;

 	if( ring->follow )
 		SetVectorSS( &ring->origin, &ring->follow->position );

	fo = (ring->fade * gameSpeed)>>12;
	if( ring->a > fo ) ring->a -= fo;
	else ring->a = 0;

	ring->speed += FMul(ring->accn, gameSpeed);
	speed = FMul(ring->speed, gameSpeed);

	ring->scale.vx += speed;
	ring->scale.vy += speed>>2;
	ring->scale.vz += speed;

	ring->angle += (ring->spin * gameSpeed)>>12;
	
	if( actFrameCount > ring->lifetime )
		DeallocateFX(ring,1);
}

void DrawPsxCroak(SPECFX *ring)
{
	IQUATERNION q1, q2, q3, cross;
	fixed t;
	MATRIX rotMtx;
	VECTOR absPos;
	VECTOR scaledVtx;
	SVECTOR scPos;
	long sz;
	int otz;
	POLY_FT4 *ft4;
	int width, height;
	int i;
	KMUINT32	rgba;

	for(i=0; i<NUM_PSX_CROAK_VTX; i++)
	{
		//don't draw this sprite if scale is too big,
		//because it can wrap the pos, and appear back on screen.
		#define MAX_RING_SCALE (10<<12)
		if( (ring->scale.vx>MAX_RING_SCALE) || (ring->scale.vy>MAX_RING_SCALE) || (ring->scale.vz>MAX_RING_SCALE) )
			continue;

/*
		//spinning circle
		SetVectorFF( (FVECTOR*)&q1, &ring->normal );
		q1.w = ring->angle;
		fixedGetQuaternionFromRotation( &q2, &q1 );

		QuatToPSXMatrix(&q2, &rotMtx);
		ApplyMatrix(&rotMtx, &psxCroakVtx[i], &absPos);

		//apply scale
		//BB - TEMP BOTCH TO GET TO WORK ON SPACE (SORT OF WORK, ANYWAY)
//		if(player[0].worldNum==WORLDID_SPACE && player[0].levelNum==LEVELID_SPACE3)
//		{
			absPos.vx = (absPos.vx*ring->scale.vx)>>12;
			absPos.vy = (absPos.vy*ring->scale.vy)>>12;
			absPos.vz = (absPos.vz*ring->scale.vz)>>12;
//		}
//		else
//		{
//			absPos.vx = FMul(absPos.vx, ring->scale.vx);
//			absPos.vy = FMul(absPos.vy, ring->scale.vy);
//			absPos.vz = FMul(absPos.vz, ring->scale.vz);
//		}
		
		//position around centre
		absPos.vx += -ring->origin.vx;
		absPos.vy += -ring->origin.vy;
		absPos.vz += ring->origin.vz;
*/

/*
		//quat to rotate current point by spinning angle
		SetVectorFF( (FVECTOR*)&q1, &ring->normal );
		q1.w = ring->angle;
		fixedGetQuaternionFromRotation( &q2, &q1 );

		//quat to rotate to plane (normal)
		CrossProductFFF( (FVECTOR*)&cross, (FVECTOR*)&q1, &upVec );
		MakeUnit( (FVECTOR*)&cross );
		t = DotProductFF( (FVECTOR*)&q1, &upVec );
		cross.w = -arccos(t);
		fixedGetQuaternionFromRotation(&q3, &cross);

		//combine and apply quats to current point
		fixedQuaternionMultiply(&q1, &q3, &q2);
		QuatToPSXMatrix(&q1, &rotMtx);
		ApplyMatrix(&rotMtx, &psxCroakVtx[i], &absPos);

		//apply scale
		absPos.vx = (absPos.vx*ring->scale.vx)>>12;
		absPos.vy = (absPos.vy*ring->scale.vy)>>12;
		absPos.vz = (absPos.vz*ring->scale.vz)>>12;

		//center around effect's origin
//		absPos.vx += ring->origin.vx;
//		absPos.vy += ring->origin.vy;
//		absPos.vz += ring->origin.vz;
		absPos.vx += -ring->origin.vx;
		absPos.vy += -ring->origin.vy;
		absPos.vz +=  ring->origin.vz;
*/

		//quat to rotate current point by spinning angle
		SetVectorFF( (FVECTOR*)&q1, &ring->normal );
		q1.w = ring->angle;
		fixedGetQuaternionFromRotation( &q2, &q1 );

		//quat to rotate to plane (normal)
		CrossProductFFF( (FVECTOR*)&cross, (FVECTOR*)&q1, &upVec );
		MakeUnit( (FVECTOR*)&cross );
		t = DotProductFF( (FVECTOR*)&q1, &upVec );
		cross.w = -arccos(t);
		fixedGetQuaternionFromRotation(&q3, &cross);

		//scale pre-transformed vertex
		SetVectorVS(&scaledVtx, &psxCroakVtx[i]);
		scaledVtx.vx = (scaledVtx.vx*ring->scale.vx)>>12;
		scaledVtx.vy = (scaledVtx.vy*ring->scale.vy)>>12;
		scaledVtx.vz = (scaledVtx.vz*ring->scale.vz)>>12;

		//combine and apply quats to current point
		fixedQuaternionMultiply(&q1, &q3, &q2);
		QuatToPSXMatrix(&q1, &rotMtx);
//		ApplyMatrix(&rotMtx, &psxCroakVtx[i], &absPos);
		ApplyMatrixLV(&rotMtx, &scaledVtx, &absPos);

		//apply scale
//		absPos.vx = (absPos.vx*ring->scale.vx)>>12;
//		absPos.vy = (absPos.vy*ring->scale.vy)>>12;
//		absPos.vz = (absPos.vz*ring->scale.vz)>>12;

		//center around effect's origin
//		absPos.vx += ring->origin.vx;
//		absPos.vy += ring->origin.vy;
//		absPos.vz += ring->origin.vz;
		absPos.vx += -ring->origin.vx;
		absPos.vy += -ring->origin.vy;
		absPos.vz +=  ring->origin.vz;






		//calc screen xyz
		gte_SetTransMatrix(&GsWSMATRIX);
		gte_SetRotMatrix(&GsWSMATRIX);
//		gte_ldv0(&absPos);
		gte_ldlv0(&absPos);
		gte_rtps();
		gte_stsxy(&scPos.vx);
		gte_stsz(&sz);	//screen z/4 as otz
		gte_stotz(&otz);	//screen z/4 as otz
	
		scPos.vz = sz;

		//screen clip
		if(scPos.vx<0)
			if(scPos.vx>640)
				if(scPos.vy<0)
					if(scPos.vy>480)
						continue;
			
		//calc width
		if(scPos.vz)
		{
			width = (ring->tex->w*xFOV)/scPos.vz;
			width >>= 1;

			height = (ring->tex->w*yFOV)/scPos.vz;
			height >>= 1;
		}
		else
			width = 1;

		// draw polygon
//		rgba = RGBA(ring->r,ring->g,ring->b,ring->a);
		rgba = RGBA(255,255,255,ring->a);

		vertices_GT4[0].fX = (float)scPos.vx-width;
		vertices_GT4[0].fY = (float)scPos.vy-width;
		vertices_GT4[0].u.fZ = sz;
		vertices_GT4[0].fU = (float)1;
		vertices_GT4[0].fV = (float)0;
		vertices_GT4[0].uBaseRGB.dwPacked = rgba;

		vertices_GT4[1].fX = (float)scPos.vx+width;
		vertices_GT4[1].fY = (float)scPos.vy-width;
		vertices_GT4[1].u.fZ = sz;
		vertices_GT4[1].fU = (float)0;
		vertices_GT4[1].fV = (float)0;
		vertices_GT4[1].uBaseRGB.dwPacked = rgba;

		vertices_GT4[2].fX = (float)scPos.vx-width;
		vertices_GT4[2].fY = (float)scPos.vy+width;
		vertices_GT4[2].u.fZ = sz;
		vertices_GT4[2].fU = (float)1;
		vertices_GT4[2].fV = (float)1;
		vertices_GT4[2].uBaseRGB.dwPacked = rgba;

		vertices_GT4[3].fX = (float)scPos.vx+width;
		vertices_GT4[3].fY = (float)scPos.vy+width;
		vertices_GT4[3].u.fZ = sz;
		vertices_GT4[3].fU = (float)0;
		vertices_GT4[3].fV = (float)1;
		vertices_GT4[3].uBaseRGB.dwPacked = rgba;

		kmChangeStripTextureSurface(&StripHead_Sprites,KM_IMAGE_PARAM1,ring->tex->surfacePtr);
	
		kmStartStrip(&vertexBufferDesc, &StripHead_Sprites);	
		kmSetVertex(&vertexBufferDesc, &vertices_GT4[0], KM_VERTEXTYPE_03, sizeof(KMVERTEX_03));
		kmSetVertex(&vertexBufferDesc, &vertices_GT4[1], KM_VERTEXTYPE_03, sizeof(KMVERTEX_03));
		kmSetVertex(&vertexBufferDesc, &vertices_GT4[2], KM_VERTEXTYPE_03, sizeof(KMVERTEX_03));	
		kmSetVertex(&vertexBufferDesc, &vertices_GT4[3], KM_VERTEXTYPE_03, sizeof(KMVERTEX_03));	
		kmEndStrip(&vertexBufferDesc);
		
/*		//draw poly
		BEGINPRIM(ft4, POLY_FT4);
		setPolyFT4(ft4);
		ft4->x0 = scPos.vx-width;
		ft4->y0 = scPos.vy-width;
		ft4->x1 = scPos.vx+width;
		ft4->y1 = scPos.vy-width;
		ft4->x2 = scPos.vx-width;
		ft4->y2 = scPos.vy+width;
		ft4->x3 = scPos.vx+width;
		ft4->y3 = scPos.vy+width;
//		ft4->r0 = 128;
//		ft4->g0 = 128;
//		ft4->b0 = 128;
		ft4->r0 = ring->a;
		ft4->g0 = ring->a;
		ft4->b0 = ring->a;
//		ft4->r0 = (ring->r*ring->a)>>8;
//		ft4->g0 = (ring->g*ring->a)>>8;
//		ft4->b0 = (ring->b*ring->a)>>8;
		ft4->u0 = ring->tex->u0;
		ft4->v0 = ring->tex->v0;
		ft4->u1 = ring->tex->u1;
		ft4->v1 = ring->tex->v1;
		ft4->u2 = ring->tex->u2;
		ft4->v2 = ring->tex->v2;
		ft4->u3 = ring->tex->u3;
		ft4->v3 = ring->tex->v3;
		ft4->tpage = ring->tex->tpage;
		ft4->clut  = ring->tex->clut;
//		setSemiTrans(ft4, 1);
		ft4->code  |= 2;//semi-trans on
 		ft4->tpage |= 32;//add
// 		ft4->tpage = si->tpage | 64;//sub
		ENDPRIM(ft4, 1, POLY_FT4);
//		ENDPRIM(ft4, otz, POLY_FT4);
*/
	}	
}

enum { TOP=0x1, BOTTOM=0x2, LEFT=0x4, RIGHT=0x8, INWARD=0x10, OUTWARD=0x20 };

int OutcodeCheck( SVECTOR *p1, SVECTOR *p2 )
{
	unsigned long oc1=0, oc2=0;
	SVECTOR t1, t2;
	long z1, z2;
	short checky = 480;

	// Tranform first point
	t2.vx = -p1->vx;
	t2.vy = -p1->vy;
	t2.vz = p1->vz;

	gte_ldv0(&t2);
	gte_rtps();
	gte_stsxy((long *)&t1.vx);
	gte_stsz(&z1);

	// Point clippage
	if( z1 < 20 )
		oc1 |= INWARD;
	else if( z1 > fog.max )
		oc1 |= OUTWARD;

	if( t1.vx < 0 )
		oc1 |= LEFT;
	else if( t1.vx > 640 )
		oc1 |= RIGHT;

	if( t1.vy < -checky )
		oc1 |= TOP;
	else if( t1.vy > checky )
		oc1 |= BOTTOM;

	// Point1 is onscreen
	if( !oc1 )
		return 0;

	// Transform second point
	t1.vx = -p2->vx;
	t1.vy = -p2->vy;
	t1.vz = p2->vz;

	gte_ldv0(&t1);
	gte_rtps();
	gte_stsxy((long *)&t2.vx);
	gte_stsz(&z2);

	// Point clippage
	if( z2 < 20 )
		oc2 |= INWARD;
	else if( z2 > fog.max )
		oc2 |= OUTWARD;

	if( t2.vx < 0 )
		oc2 |= LEFT;
	else if( t2.vx > 640 )
		oc2 |= RIGHT;

	if( t2.vy < -checky )
		oc2 |= TOP;
	else if( t2.vy > checky )
		oc2 |= BOTTOM;

	// Point2 is onscreen
	if( !oc2 )
		return 0;
	// If both offscreen on the same side, clip.
	if( oc1 & oc2 )
		return 1;
	// Off different sides - crossing screen
	return 0;
}