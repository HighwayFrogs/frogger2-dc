
void UpdateTextureAnimations ( void )
{
	DR_MOVE *siMove;
	RECT	moveRect;

	TEXTUREANIM *cur,*next;

	if ( textureAnimList.numEntries == 0 )
		return;
	
	for ( cur = textureAnimList.head.next; cur != &textureAnimList.head; cur = cur->next )
	{
		if( cur->numFrames > 1 )
		{
			// If special waitTime then advance frame by 1
			if( cur->animation->waitTimes[cur->frame] == -1 )
			{
				cur->lastTime++;

				cur->frame++;
				if( cur->frame >= cur->numFrames )
					cur->frame = 0;
			}
			else
			{
				while( cur->lastTime + cur->animation->waitTimes[cur->frame] < frame )
				{
					cur->lastTime += cur->animation->waitTimes[cur->frame];
					
					cur->frame++;
					if( cur->frame >= cur->numFrames )
						cur->frame = 0;
				}
			}
			// JH: Copy the required texture into vram.

			cur->animation->dest->surfacePtr = &cur->animation->anim[cur->frame]->surface;
//			CopyTexture ( cur->animation->dest, cur->animation->anim[cur->frame], 0 );

			// *ASL* 26/07/2000 - Animate the DC strip header
			kmChangeStripTextureSurface(&cur->animation->dest->stripHead, KM_IMAGE_PARAM1, cur->animation->anim[cur->frame]->surfacePtr);
		}
		
	}
}

