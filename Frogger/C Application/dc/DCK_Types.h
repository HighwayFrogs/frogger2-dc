/*
	(c) 2000 Interactive Studios Ltd.

	File		: DCK_Types.h
	Programmer	: Andy Eder
	Created		: 19/01/00

	Updated		: 19/01/00	Andy Eder
				: < revision dates here >

----------------------------------------------------------------------------------------------- */

#ifndef DCK_TYPES_H_INCLUDED
#define DCK_TYPES_H_INCLUDED

//----- [ DEFINES ] ------------------------------------------------------------------------------

#define RGBA(r,g,b,a)		((KMUINT32)(((a) << 24) | ((r) << 16) | ((g) << 8) | (b)))
#define RGBA4444(r,g,b,a)	((USHORT)(((a) << 12) | ((r) << 8) | ((g) << 4) | (b)))
#define RGBA1555(r,g,b,a)	((USHORT)(((a) << 15) | ((r) << 10) | ((g) << 5) | (b)))
#define RGB565(r,g,b)		((USHORT)(((r) << 11) | ((g) << 5) | (b)))

#define MOD		%

#define Max(p1,p2)	(((p1) > (p2)) ? (p1) : (p2))
#define Min(p1,p2)	(((p1) > (p2)) ? (p2) : (p1))

#define BOOL	int

//#define QEPSILON	((float)0.00001)
//#define QHALFPI		((float)1.570796326794895)

//----- [ MATH DATA STRUCTURES ] -----------------------------------------------------------------

typedef float km_vector[3];

typedef struct _DCKQUATERNION
{
	float	v[4];					// float vector components for x,y,z,w
	
} DCKQUATERNION, *PDCKQUATERNION;

typedef struct
{
	float	x, y, z, w;
} QUATERNION;

// NOTES :	This matrix structure (DCKMATRIX) is equivalent to the Direct3D D3DMATRIX.
typedef struct _DCKMATRIX
{
	float	_11,_12,_13,_14;
	float	_21,_22,_23,_24;
	float	_31,_32,_33,_34;
	float	_41,_42,_43,_44;

} DCKMATRIX, *PDCKMATRIX;

typedef struct
{
	float	v[3];
} DCKVECTOR;

typedef struct
{
	char	v[4];
} DCKBYTEVECTOR;

typedef struct
{
	signed char	v[4];
} DCKSBYTEVECTOR;

typedef struct
{
	short	v[2];
} DCKSHORT2DVECTOR;

typedef struct
{
	float	v[2];
} DCKFLOAT2DVECTOR;

typedef struct
{
	short	v[3];
} DCKSHORTVECTOR;

typedef struct
{
	union
	{
		QUATERNION	quat;
		DCKVECTOR	vect;
	}u;
	int	time;
} KEYFRAME;

typedef struct tagBMPHeaderType	// BMP file header
{				
	long 	filesize;
	short	reserved[2];
	long 	headersize;
	long 	infoSize;
	long 	width;
	long 	depth;
	short	biPlanes;
	short 	bits;
	long 	biCompression;
	long 	biSizeImage;
	long 	biXPelsPerMeter;
	long 	biYPelsPerMeter;
	long 	biClrUsed;
	long 	biClrImportant;
} BMPHeaderType;

//----- [ GEOMETRY DATA STRUCTURES ] -------------------------------------------------------------

/* NOTES :	This vertex structure (DCKVERTEX) is equivalent to the Direct3D D3DTLVERTEX. This makes
			possible the casting from one type to the other. The exception is that D3D's u and v
			texture coordinates must be multiplied by the 1/w value.
*/			
typedef struct _DCKVERTEX
{
	float		sX;					// projected x screen coordinate (0 <= sX < screen_width)
	float		sY;					// projected y screen coordinate (0 <= sY < screen_height)
	float		sZ;					// screen z (0.01 <= sZ <= 1.0) where 1.0 is at infinity
	
	float		invW;				// 1/w for perspective projection (should be 1.0/sZ; sZ is ignored)

	KMUINT32	colourMaterial;		// material colour (equivalent to D3DCOLOR type)
	KMUINT32	colourSpecular;		// specular colour (equivalent to D3DCOLOR type)
	
	float		uOverW;				// u texture coordinate
	float		vOverW;				// v texture coordinate

	// please note !
	USHORT		vertno;				// zero-base index into the parent's vertex list
	float		u,v;				// texture coords at this point
	UCHAR		r,g,b,a;			// colour for colour based gouraud shading

} DCKVERTEX, *PDCKVERTEX;

//***** THESE ARE JUST TEMP STRUCTURES FOR TESTING ******************************************

typedef struct _DCKPOLYGON
{
	int				numVerts;			// number of vertices in this polygon
	int				vIndex[4];			// indices of vertices comprising this polygon
	DCKVERTEX		verts[4];

	unsigned char	c[4][4];			// colours (RGBA) for each vertex in this polygon
	float			tc[4][2];			// texture coordinates for each vertex in this polygon
	
	unsigned char	r,g,b,a;			// polygon material colour (if not using vertex colour)
	
	int				textureId;			// texture id
	int				textureNum;			// texture number in list
	float			utile,vtile;		// tiling values

	USHORT			flags;
	char			translucencymode;
	char			gouraudmode;
	
} DCKPOLYGON, *PDCKPOLYGON;

typedef struct _DCKOBJECT
{
	char				ident[32];
	int					objectId;

	int					numMoveKeys;	// move, rot and scale keys for animation
	int					numRotateKeys;
	int					numScaleKeys;
	KEYFRAME			*moveKeys;	
	KEYFRAME			*rotateKeys;	
	KEYFRAME			*scaleKeys;	
		
	int					numVerts;		// number of vertices in this object
	int					numPolys;		// number of polygons in this object
		
	DCKVECTOR			*verts;			// the object's vertices
	DCKPOLYGON			*polys;			// the object's polygons
	
	unsigned char		r,g,b,a;		// object material colour (if not using polygon or vertex colour)

	USHORT				flags;
	UCHAR				gouraudmode;
	UCHAR				translucency;
	UCHAR				translucencymode;
	
	struct	_DCKOBJECT	*next;
	struct	_DCKOBJECT	*children;
	
} DCKOBJECT, *PDCKOBJECT;

//***************************************************************************************************


#endif
