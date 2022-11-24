#ifndef _LIBGTE_H_INCLUDE
#define _LIBGTE_H_INCLUDE

typedef struct
{
	// long word type 3D vector
	long	vx, vy;
	long	vz, pad;
} VECTOR;

typedef struct
{	
	// short word type 3D vector
	short	vx, vy;
	short	vz, pad;
} SVECTOR;

#endif