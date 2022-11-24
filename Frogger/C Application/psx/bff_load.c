
// comment out to link into the frogger testbed
// leave in to link into the ActionMan2 testbed
//#define USE_NEW_LIBS


// the "depend" batch doesn't like non-existant headers
//	#include "global.h"
#include "include.h"

/*
Quickie guide to BFF files:-

 A BFF is, simply, a collection of data segments, prefixed by a TYPE, a SIZE, and a name (or rather the CRC of a name)
Data of many different types can just be shoved together into one file.
After loading, the program scans through the list, checks the type of each segment, and calls a "link" routine
for that type, which basically fills in any required pointers.

 Then there's a little routine saying "find a bff segment of "such&such type, with this name-CRC"

*/


// A Macro and a variable that were used in Glover, but aren't in Frogger2
// (But here they are anyway)

//#define BFF_CorrectPointer(header,pointer) {if(pointer) (pointer) = ADD2POINTER((header),((int)(pointer)));}
//BFF_Header *generic_bff;


// We need access to a some ISLPSI internal variable
// (Latest version libs have these in)

int	biggestVertexModel;
extern long *transformedVertices;
extern VERT *transformedNormals;
extern long *transformedDepths;



// Adjust 0...128 texture coordinates into real ones

ULONG FMA_TmapCorrect(ULONG x,ULONG w,ULONG per)
{
	ULONG calc;
	ULONG result;

	calc=(w*per)>>7; 
 	result=calc+x;

	if(result==256)	// Work around the silly playstation's lack of ability to access the top pixel of a texture
		result = 255;

 	if (result>255)
	{
//		printf("tmap correction OV: x=%d,w=%d,per=%d\n",x,w,per);
		result=8;
	}
 	return result;
}

// ===================================================================================
// Turn the offsets & spritex numbers in a FMA MESH BFF-segment data into pointers

void BFF_Link_FMA_Mesh(FMA_MESH_HEADER *mesh)
{
	int i;
	TextureType 	**tmaps;
	TextureType 	*tmap;
	unsigned short	r,g,b;

	tmaps = ADD2POINTER(mesh, (int)mesh->tmap_crcs);

	mesh->gt3s = ADD2POINTER(mesh,(int)mesh->gt3s);
	mesh->gt4s = ADD2POINTER(mesh,(int)mesh->gt4s);
	mesh->sprs = ADD2POINTER(mesh,(int)mesh->sprs);	
	mesh->verts = ADD2POINTER(mesh,(int)mesh->verts);

	if(mesh->n_verts > biggestVertexModel)
	{
		// When using the new libs, you update "biggestVertexModel" during loading, and
		// call a PSI.C function that allocates the memory after you've loaded all the shapes

		// take out later
/*		if(transformedVertices != 0)
		{
			 Align32Free(transformedVertices);
			 Align32Free(transformedDepths);
			 Align32Free(transformedDepths2);
			 Align32Free(transformedNormals);
		}
*/
		biggestVertexModel = mesh->n_verts;
		
/*		transformedVertices = (long*)Align32Malloc( biggestVertexModel*sizeof(SVECTOR));
		transformedDepths = (long*)Align32Malloc( biggestVertexModel*sizeof(SVECTOR));
		transformedDepths2 = (float*)Align32Malloc( biggestVertexModel*sizeof(float));
		transformedNormals = (VERT*)Align32Malloc( biggestVertexModel*sizeof(VERT));
*/	}

	// Having done that, let's get down to the real work of converting the loaded memory block
	// into a usable one with pointers and real-world texture info.
		
	// Number one - We've got a list of texture CRC's to convert into pointers.
	// printf("Mesh has %d textures\n",mesh->n_tmaps);
/*	for(i = 0; i < mesh->n_tmaps; i++)
	{
		tmap = textureFindCRCInAllBanks((unsigned long)tmaps[i]);
		if(!tmap)
		{
			utilPrintf("World Texture (crc &%8x) not found\n",(unsigned long)tmaps[i]);
		}
		tmaps[i] = tmap;
	}
*/
	// Number two. Every polygon contains a texture number, which needs converting
	// into a clut & a tpage. They also need their tu and tv adjusting.
	for(i = 0; i < mesh->n_gt3s; i++)
	{	
		mesh->gt3s[i].tpage = FindTextureCRC((unsigned long)tmaps[mesh->gt3s[i].tpage]);	

		if((mesh->gt3s[i].pad2 & 32)||(mesh->gt3s[i].pad2 & 64))
			continue;
			
		// r0
		r = mesh->gt3s[i].r0 << 1;
		if(r < 255)
			mesh->gt3s[i].r0 = r;
		else
			mesh->gt3s[i].r0 = 255;
		
		g = mesh->gt3s[i].g0 << 1;
		if(g < 255)
			mesh->gt3s[i].g0 = g;
		else
			mesh->gt3s[i].g0 = 255;
			
		b = mesh->gt3s[i].b0 << 1;
		if(b < 255)
			mesh->gt3s[i].b0 = b;
		else
			mesh->gt3s[i].b0 = 255;

		// r1
		r = mesh->gt3s[i].r1 << 1;
		if(r < 255)
			mesh->gt3s[i].r1 = r;
		else
			mesh->gt3s[i].r1 = 255;
		
		g = mesh->gt3s[i].g1 << 1;
		if(g < 255)
			mesh->gt3s[i].g1 = g;
		else
			mesh->gt3s[i].g1 = 255;
			
		b = mesh->gt3s[i].b1 << 1;
		if(b < 255)
			mesh->gt3s[i].b1 = b;
		else
			mesh->gt3s[i].b1 = 255;

		// r2
		r = mesh->gt3s[i].r2 << 1;
		if(r < 255)
			mesh->gt3s[i].r2 = r;
		else
			mesh->gt3s[i].r2 = 255;
		
		g = mesh->gt3s[i].g2 << 1;
		if(g < 255)
			mesh->gt3s[i].g2 = g;
		else
			mesh->gt3s[i].g2 = 255;
			
		b = mesh->gt3s[i].b2 << 1;
		if(b < 255)
			mesh->gt3s[i].b2 = b;
		else
			mesh->gt3s[i].b2 = 255;
						
//		tmap = tmaps[mesh->gt3s[i].tpage];
//		if(tmap)
//		{
//			mesh->gt3s[i].clut = tmap->clut;
//			mesh->gt3s[i].tpage = tmap->tpage;

//			mesh->gt3s[i].u0 = FMA_TmapCorrect(tmap->x,tmap->w,mesh->gt3s[i].u0);
//			mesh->gt3s[i].v0 = FMA_TmapCorrect(tmap->y,tmap->h,mesh->gt3s[i].v0);
//			mesh->gt3s[i].u1 = FMA_TmapCorrect(tmap->x,tmap->w,mesh->gt3s[i].u1);
//			mesh->gt3s[i].v1 = FMA_TmapCorrect(tmap->y,tmap->h,mesh->gt3s[i].v1);
//			mesh->gt3s[i].u2 = FMA_TmapCorrect(tmap->x,tmap->w,mesh->gt3s[i].u2);
//			mesh->gt3s[i].v2 = FMA_TmapCorrect(tmap->y,tmap->h,mesh->gt3s[i].v2);

// Multiply the vertex numbers by 4.
// A nice little optimisation when looking the coordinates up
// (this code now moved to the PC compiler end)
//			mesh->gt3s[i].vert0 *= 4;
//			mesh->gt3s[i].vert1 *= 4;
//			mesh->gt3s[i].vert2 *= 4;
//		}
		
	}

	for(i = 0; i < mesh->n_gt4s; i++)
	{
		mesh->gt4s[i].tpage = FindTextureCRC((unsigned long)tmaps[mesh->gt4s[i].tpage]);

		if((mesh->gt4s[i].pad2 & 32)||(mesh->gt4s[i].pad2 & 64))
			continue;
		
		// r0
		r = mesh->gt4s[i].r0 << 1;
		if(r < 255)
			mesh->gt4s[i].r0 = r;
		else
			mesh->gt4s[i].r0 = 255;
		
		g = mesh->gt4s[i].g0 << 1;
		if(g < 255)
			mesh->gt4s[i].g0 = g;
		else
			mesh->gt4s[i].g0 = 255;
			
		b = mesh->gt4s[i].b0 << 1;
		if(b < 255)
			mesh->gt4s[i].b0 = b;
		else
			mesh->gt4s[i].b0 = 255;

		// r1
		r = mesh->gt4s[i].r1 << 1;
		if(r < 255)
			mesh->gt4s[i].r1 = r;
		else
			mesh->gt4s[i].r1 = 255;
		
		g = mesh->gt4s[i].g1 << 1;
		if(g < 255)
			mesh->gt4s[i].g1 = g;
		else
			mesh->gt4s[i].g1 = 255;
			
		b = mesh->gt4s[i].b1 << 1;
		if(b < 255)
			mesh->gt4s[i].b1 = b;
		else
			mesh->gt4s[i].b1 = 255;

		// r2
		r = mesh->gt4s[i].r2 << 1;
		if(r < 255)
			mesh->gt4s[i].r2 = r;
		else
			mesh->gt4s[i].r2 = 255;
		
		g = mesh->gt4s[i].g2 << 1;
		if(g < 255)
			mesh->gt4s[i].g2 = g;
		else
			mesh->gt4s[i].g2 = 255;
			
		b = mesh->gt4s[i].b2 << 1;
		if(b < 255)
			mesh->gt4s[i].b2 = b;
		else
			mesh->gt4s[i].b2 = 255;

		// r3
		r = mesh->gt4s[i].r3 << 1;
		if(r < 255)
			mesh->gt4s[i].r3 = r;
		else
			mesh->gt4s[i].r3 = 255;
		
		g = mesh->gt4s[i].g3 << 1;
		if(g < 255)
			mesh->gt4s[i].g3 = g;
		else
			mesh->gt4s[i].g3 = 255;
			
		b = mesh->gt4s[i].b3 << 1;
		if(b < 255)
			mesh->gt4s[i].b3 = b;
		else
			mesh->gt4s[i].b3 = 255;

//		tmap = tmaps[mesh->gt4s[i].tpage];
//		if(tmap)
//		{
//			mesh->gt4s[i].clut = tmap->clut;
//			mesh->gt4s[i].tpage = tmap->tpage;

//			mesh->gt4s[i].u0 = FMA_TmapCorrect(tmap->x,tmap->w,mesh->gt4s[i].u0);
//			mesh->gt4s[i].v0 = FMA_TmapCorrect(tmap->y,tmap->h,mesh->gt4s[i].v0);
//			mesh->gt4s[i].u1 = FMA_TmapCorrect(tmap->x,tmap->w,mesh->gt4s[i].u1);
//			mesh->gt4s[i].v1 = FMA_TmapCorrect(tmap->y,tmap->h,mesh->gt4s[i].v1);
//			mesh->gt4s[i].u2 = FMA_TmapCorrect(tmap->x,tmap->w,mesh->gt4s[i].u2);
//			mesh->gt4s[i].v2 = FMA_TmapCorrect(tmap->y,tmap->h,mesh->gt4s[i].v2);
//			mesh->gt4s[i].u3 = FMA_TmapCorrect(tmap->x,tmap->w,mesh->gt4s[i].u3);
//			mesh->gt4s[i].v3 = FMA_TmapCorrect(tmap->y,tmap->h,mesh->gt4s[i].v3);

//			mesh->gt4s[i].vert0 *= 4;
//			mesh->gt4s[i].vert1 *= 4;
//			mesh->gt4s[i].vert2 *= 4;
//			mesh->gt4s[i].vert3 *= 4;
//		}
	}

	for(i = 0; i < mesh->n_sprs; i++)
	{
		mesh->sprs[i].tpage = FindTextureCRC((unsigned long)tmaps[mesh->sprs[i].tpage]);

		// r0
		r = mesh->sprs[i].r0 << 1;
		if(r < 255)
			mesh->sprs[i].r0 = r;
		else
			mesh->sprs[i].r0 = 255;
		
		g = mesh->sprs[i].g0 << 1;
		if(g < 255)
			mesh->sprs[i].g0 = g;
		else
			mesh->sprs[i].g0 = 255;
			
		b = mesh->sprs[i].b0 << 1;
		if(b < 255)
			mesh->sprs[i].b0 = b;
		else
			mesh->sprs[i].b0 = 255;

/*		tmap = tmaps [ mesh->sprs[i].tpage ];
		if ( tmap )
		{
			mesh->sprs[i].clut = tmap->clut;
			mesh->sprs[i].tpage = tmap->tpage;


			mesh->sprs[i].u0 = FMA_TmapCorrect(tmap->x,tmap->w,mesh->sprs[i].u0);
			mesh->sprs[i].v0 = FMA_TmapCorrect(tmap->y,tmap->h,mesh->sprs[i].v0);
			mesh->sprs[i].u1 = FMA_TmapCorrect(tmap->x,tmap->w,mesh->sprs[i].u1);
			mesh->sprs[i].v1 = FMA_TmapCorrect(tmap->y,tmap->h,mesh->sprs[i].v1);
			mesh->sprs[i].u2 = FMA_TmapCorrect(tmap->x,tmap->w,mesh->sprs[i].u2);
			mesh->sprs[i].v2 = FMA_TmapCorrect(tmap->y,tmap->h,mesh->sprs[i].v2);
			mesh->sprs[i].u3 = FMA_TmapCorrect(tmap->x,tmap->w,mesh->sprs[i].u3);
			mesh->sprs[i].v3 = FMA_TmapCorrect(tmap->y,tmap->h,mesh->sprs[i].v3);
//			mesh->gt4s[i].vert0 *= 4;
//			mesh->gt4s[i].vert1 *= 4;
//			mesh->gt4s[i].vert2 *= 4;
//			mesh->gt4s[i].vert3 *= 4;
		}
*/
	}	
/*
// Tra-la. The BFF memory-image is now a usable, drawable, thing.
*/
}

// Turn the FMA CRCs in a FMA MAP BFF-segment data into pointers
// This is an easy one. The world just has a list of mesh-name CRC's, which
// need converting into pointers. This is done by BFF_Finding them, even
// though they're actually part of the same file as the FMA_WORLD!
void BFF_Link_FMA_World(FMA_WORLD *world)
{
	int i;
	unsigned long *crcs;
	FMA_MESH_HEADER *mesh;

#define meshes ((FMA_MESH_HEADER **)crcs)

	meshes = ADD2POINTER(world, sizeof(FMA_WORLD));
	for(i = 0; i < world->n_meshes;i++)
	{
		mesh = (void *)BFF_FindObject(BFF_FMA_MESH_ID,crcs[i]);
		if(!mesh)
		{
			utilPrintf("Mesh %d of the world not found (crc = &%8x)\n",i,crcs[i]);
		}
		meshes[i] = mesh;
	}
#undef meshes
}

// ===================================================================================
//                GENERAL   BFF   HANDLING   STUFF
// ===================================================================================

#define MAX_BFF_FILES 4
BFF_Header *BFF_Pointers[MAX_BFF_FILES] = {NULL,NULL,NULL,NULL};
int BFF_Lens[MAX_BFF_FILES];

BFF_Header *BFF_FindObject(int id, unsigned long crc)
{
	int i;
	BFF_Header *work;
	for(i = 0; i < MAX_BFF_FILES; i++)
	{
		if(BFF_Pointers[i])
		{
//			printf(" scanning bff %d",i);
			work = BFF_Pointers[i];
			for(;;)
			{
				if(work->id == id && (crc == 0 || work->crc == crc))
				{
//					printf("\n");
					return work;
				}
				if(work->len & 0x80000000)
					break;
				work = ADD2POINTER(work,(int)work->len);
			}
//			printf("\n");
		}
	}
	return NULL;	// yes there are occasions when we need to try to find things that aren't there
}

BFF_Header *BFF_LoadFile(char *filename)
{
	BFF_Header *addr;
	BFF_Header *work;
	BFF_Header *ending;
	int i,j;
	int lastfilelength;
	int	numObjects = 0;
	
// Oh lovely, Frogger's makefile didn't warn me about duff numbers of parameters being sent to functions...
// Someone's not got the warning level set high enough!

	addr = (BFF_Header*)FindStakFileInAllBanks ( filename, &lastfilelength );

	//addr = (BFF_Header*)getFileFromStack ( stakFile, filename, &lastfilelength );

//	addr = (void *)fileLoad(filename, &lastfilelength);

	if(!addr)
		return NULL;

	ending = ADD2POINTER(addr, lastfilelength);

	work = addr;

// Using the file length, scan through the file and mark the last entry in the BFF file,
// because the "findobject" routines won't know the length.
	for(;;)
	{
		if((void *)ADD2POINTER(work,(int)work->len) >= (void *)ending)
		{
			work->len |= 0x80000000;	// mark as the last one
			break;
		}
		work = ADD2POINTER(work,(int)work->len);
		numObjects++;
	}

// Mark the BFF as being available for searching for objects.
// This needs to be done BEFORE linking, because some objects want to look up
// the addresses of other objects in the same file.
	for(i = 0; i < MAX_BFF_FILES; i++)
	{
		if(!BFF_Pointers[i])
		{
			BFF_Pointers[i] = addr;
			BFF_Lens[i] = lastfilelength;
			break;
		}
	}

// Essentially, convert any "offsets" in the loaded objects into "pointers", and
// any other work that needs doing to convert the loaded data into usable data.
// This means checking the type of the data, and calling a routine to "link"
// that type.
	numObjects = 0;
	work = addr;
	for(;;)
	{
		switch(work->id)
		{
			case BFF_FMA_MESH_ID:
				BFF_Link_FMA_Mesh((FMA_MESH_HEADER *)work);
				break;
			case BFF_FMA_WORLD_ID:
				BFF_Link_FMA_World((FMA_WORLD *)work);
				break;

			default:
				utilPrintf("Unknown or out-of-date BFF class %d (%c%c%c%d)\n",
					work->id,
					((char *)(&work->id))[0],
					((char *)(&work->id))[1],
					((char *)(&work->id))[2],
					(int)(((char *)(&work->id))[3])
					);
				CRASH;
		}

		if(work->len & 0x80000000)
			break;
		work = ADD2POINTER(work,(int)work->len);
		numObjects++;
		
//		DB("ok\n");
	}
//	DB("linking all done\n");

	return addr;
}

// Releasing a file's dead trivial. Just free the memory, and mark it as
// not to be scanned in future "findobject"s
// As if by magic, all the contents are freed.
// (Unless the "link" routines do any mallocs - in which case there'll need to be
// a full BFF-scan here)

void BFF_ReleaseFile(BFF_Header *header)
{
	int i;
	if(!header)
		return;
	for(i = 0; i < MAX_BFF_FILES; i++)
	{
		if(BFF_Pointers[i] == header)
		{
			BFF_Pointers[i] = NULL;
			break;
		}
	}
	Align32Free(header);
}

// A little diagnostic check that you can use to ensure that you've not
// kept a pointer to a BFF object in a file that has since been discarded.
// In practice, this should never be used, natch.

int BFF_IsInBFF(void *addr)
{
	void *base;
	int i;

	for(i = 0; i < MAX_BFF_FILES; i++)
	{
		base = BFF_Pointers[i];
		if(base)
		{
			if(base <= addr && ADD2POINTER(base,BFF_Lens[i]) > addr)
				return 1;
			break;
		}
	}
	return 0;
}

BFF_Header *BFF_FindNamedObject(int id, char *name)
{
	int i;
	unsigned long crc;

	BFF_Header *work;

	crc = utilStr2CRC(name);
	for(i = 0; i < MAX_BFF_FILES; i++)
	{
		if(BFF_Pointers[i])
		{
//			printf(" scanning bff %d",i);
			work = BFF_Pointers[i];
			for(;;)
			{

//				printf(".");
				if(work->id == id && (crc == 0 || work->crc == crc))
				{
//					printf("\n");
					return work;
				}
				if(work->len & 0x80000000)
					break;
				work = ADD2POINTER(work,work->len);
			}
//			printf("\n");
		}
	}
//	printf("null\n");
	return 0;	// yes there are occasions when we need to try to find things that aren't there
}
