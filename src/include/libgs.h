#ifndef _LIBGS_H_INCLUDE
#define _LIBGS_H_INCLUDE

typedef unsigned char PACKET;

typedef struct
{
	short			vx, vy, vz;
	unsigned char 	tu, tv;
} VERT;

typedef struct
{
	long    vpx, vpy, vpz;
	long    vrx, vry, vrz;
	long    rz;
} GsRVIEW2;

#endif