# Frogger 2 FMV Conversion
The program for FMV conversion unfortunately only runs in a GUI, there is no CLI option.  
As such, it makes the most sense to include the baked `.SFD` files 


## Where are the source files?
The .avi files may not be included depending on where you got this repository.  
This is because they are too large for the git repository. (Over a gigabyte in total)  
If you're unaware, this repository is part of a larger release of Frogger 2 development archives.  
This release contains among other things various FMV video backups.  

Here are the original input files which were used to create the .SFD Files in the Dreamcast Version:  
```
Audio Files: August 4 and August 7 FMV Backup CDs

Story FMVs [02s.sfd -> 12s.sfd + intro.sfd]:
 - Video files are the 640x480 avis from July 27, rescaled to 640x240.

ataris.sfd: I believe this is on an art disk.
blitzs.sfd:
 - Video: blitzs.avi, Dreamcast Backup, F:\Frogger\movies
 - Audio: ?
hlogo.sfd:
 - Video: HLogo.avi, Dreamcast Backup, F:\Frogger\movies
 - Audio: HLogo.wav, Dreamcast Backup, F:\Frogger\movies
```

## Original Export Settings
The original program used these default export settings on the videos:  
```
Duration: 1 to {max video frame}
Video: 3600 kbits/sec (TODO: CONFIRM)
FPS: 25
Resolution: 640x240
Audio: 44100 Hz Stereo

Special Processing:
 - Crop Processing: None
 - Interlace Processing: None
 - Resizing: High-Quality Interpolation
```

## Matching?
It's most likely possible to make matching files for most of the SFDs, but I couldn't figure out the exact settings. I got close, but not quite perfect.  
I think it's probably OK to leave it there given the nature of this file. There isn't much benefit to getting a perfect match for this kind of file.  