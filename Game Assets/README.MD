# Frogger 2 Game Assets

This folder contains an attempted recreation of the asset pipeline to convert game files from their source form to the form the assets are included on disc.  
It does not match perfectly byte for byte, and not all of the original assets in source form could be recovered.  
Information follows for each file format, and information about where the assets were recovered from.  

## Sound
### Sound Effects (.KAT)
Folder: `sfx`  
Source: Dreamcast Backup `F:\Frogger\sfx`  
Sound effects are converted from the `.wav` format to a single `.kat` file which contains each of the sounds in a folder.  
These files are generated using the Dreamcast SDK program `MkBank.exe`.  
A custom program called `mkscript2.exe` made by Blitz Games is used to automatically generate the scripts necessary for `MkBank.exe`.  

**.KAT Files:**  
.KAT files contain the audio data from .wav files.  
Here's a rough description of the file format, at least as seen in Frogger 2.  
```c
struct KAT_FILE {
  int32 soundCount;
  SOUND_HEADER entries[soundCount];
  byte snd_body[the number of bytes all of the audio file bodies take up];
}

struct SOUND_HEADER {
  int32 channelCount; // Number of channels. Always 1 in Frogger 2, for mono audio.
  int32 offset; // The address in the file where sound body data starts.
  int32 size; // The size (in bytes) of the audio body data.
  int32 sampleRate; // The sample rate of the audio (Eg: 11050, 11025)
  int32 loop; // This value appears to be 1 when looping is enabled and 0 when it is not. This value may have other properties too.
  int32 bits; // This value appears to be 16 when looping is enabled and 4 when it is not. This value may have other properties too.
  char  name[20]; // A name of the sound. In Frogger 2 this goes completely empty / unused.
}
```

**.TXT Files:**  
Additionally, a list of all of the sounds is saved to a .TXT file, included on the Dreamcast disc.  
These .txt files are loaded by `psx/newsfx.c` and `psx/audio.c`, and may be used for CRC hashing purposes.  

**Non-matching Files:**
Many of the output files (both .TXT and .KAT) do not match the retail files.  
However, these appear to have been the exact .wav files used to build the retail .KAT and .TXT files.  
Instead, the primary problem appears to be with the filesystem. The programs for building the script and the .txt just ask Windows "hey give me the files in the folder".  
Unfortunately, the factors which Windows uses to choose the order of the list of files it gives the program is not clear, and may have changed since Frogger 2 was built.  
So, manually modified .oss scripts have been included to ensure the correct file order.  
These files should be deleted if new audio files are to be added, which will make 'buildsfx.bat' create new non-matching files each build.  

Unfortunately even after all of that, there are a few files which still have differences:  
 - ancients.kat  
 - halloween.kat  
 - lab.kat  
 - multi.kat  
 - space.kat  
 - sub.kat  

These final differences are caused due to a defect in the `MkBank.exe` program.  
At the boundaries between each sound file, if they're not 4-byte aligned, the program will leave a gap to align the bytes.  
Unfortunately, in this gap it doesn't actually write any data, so it just uses whatever data was present at that memory location before.  
So, we can't get a perfect match of the retail data consistently because this data can be different depending on the execution environment.  
Because the files match perfectly if this defect is ignored, this is as close as we can reasonably get to perfect matches.  

### Music (.ADX)  
TODO: Create Setup  
TODO: Do they match?  

### Sound Animation (.SAM)  
Folder: `sfxanim\`  
Source: ? `?`  
These files map sound effects with character/actor actions, or even individual tiles.  
They were created using the `sfxmap.exe` program found in the Aug 15 ISO\jimsbackup.zip  
TODO: Finish setup.
TODO: Do they match?

## Textures (Images)
### Texture Bank (.SPT)
Folder: `textures\*\*.bmp`  
Source: Dreamcast Backup `C:\Frogger\doschop\`
Texture banks contain the texture/image data from all of the source image files.  
They are created with a proprietary Blitz program called `doschop.exe`.
Source code is available for this program, but it is for a slightly older version than the one used in Frogger 2 Dreamcast.  
The files built here besides `space.spt` and `sub.spt` match retail perfectly.
However, there are a few files which are completely unused on Dreamcast, and those are not currently buildable.  
TODO: Get those two files to match retail, and possibly extract the others, so we can build them.  

### Dreamcast Image (.PVR)
Folder: `backdrops\*.bmp`  
Source: Dreamcast Backup `F:\Frogger\cd\track03\backdrops\`  
PVR files are a standard format utilized by many Dreamcast games, with tools present in the Dreamcast SDK.  
The only non-matching file is the unused `Frogger2.pvr` file.  

### Texture Anim Bank (.TIT)  
Folder: `textures\*\*.txt`  
Source: Dreamcast Backup `C:\Frogger\doschop\`  
These files have information on texture animations.  
These files are built from .txt files in the texture folders by a program called `animtex.exe`.    
Unfortunately, the original program was not found in any of the backups.
However, a recreation has been created/included to take the place of the original.  
TODO: Create setup  
TODO: Do they match

### Game Icon (saveicon.h)  
Folder: `textures\*\*.txt`  
Source: Dreamcast Backup `C:\Frogger\doschop\frogicon16.bmp`  
The game's save icon is saved as `frogicon16.bmp`.  
Using a Blitz proprietary tool called 'dc_iconchop.exe', we can convert this file into saveicon.h, a file which gets compiled as part of the game.  
It matches the retail versions perfectly.  

## FMV (Video, .SFD)
TODO: Create setup  
TODO: Do they match?  

## Level Data
### Entity Data (.DAT)
Contains entity data such as TODO.  
These files were created/edited in a level editor created inside of the Frogger 2 PC version.  
Most likely one of the PC prototype builds can be used to edit these files.  
Unfortunately, due to prototypes containing other early assets like early level models, it may be difficult to do this.  
In the future, there are plans to build a debug build of the final PC version, including the level editor.  
Alternatively, [FrogLord](https://github.com/Kneesnap/FrogLord) will eventually support editing these files too.  
The files included in this repository were pulled directly from the retail Dreamcast version, and as such match perfectly.  

### Collision Data (.COL)
Folder: `?`  
Source: ? `?`  
Collision data has still yet to be researched.  
It seems to have been exported by a program called WorldConv.exe (Found in Aug 15 Backup\jimsbackup.zip), but it doesn't quite line up exactly.  
TODO: Research  
TODO: Check older prototypes to see if the format ever matches, and to see how the format changed over time.  

### Scenic Data (.SCE)
TODO  

### Script (.ETC -> .FEV)
Folder: `?`  
Source: ? `?`  
.FEV files are scripts compiled from .ETC files by a program called `etc`.
Unfortunately, this program has not yet been found, and the only uncompiled script backup is from May 22, 2000 on the May 22 backup ISO.  
TODO: Figure this one out later.  

## 3D Models
### .BFF (?)
TODO  

### .PIL (PSI Model Library)
TODO  

## Stak Files (.STK)
Folder: `StakFiles`  
StakFiles are containers, similar to .zip files, albeit much simpler. They just contain other game files.  
In order to create .STK files, we use the Blitz program `flatstak.exe` with other already existing game files discussed above.   
The StakFiles match the retail release if and only if all of the files which are put into the .STK file match fully.