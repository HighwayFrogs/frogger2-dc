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
Folder: `music`  
Source: Dreamcast Backup `F:\Frogger\music`  
Music is converted from the `.wav` format to the `.adx` format by a Dreamcast SDK program called `adxencd.exe`.  
Something interesting to note is that the retail Dreamcast version (and by extension the backup this repository is based on) uses outdated versions of the game music.  
Meaning, the retail Dreamcast version contains pre-release music.  
The converted adx files match the retail version files perfectly, assuming the original audio files are used.  

### Sound Animation (.SAM)  
Folder: `sfxanim`  
Source: Aug 15 ISO `\FroggerProject\PSX\Code\Cd\Maps\`  
These files map sound effects with character/actor actions, or even individual tiles.  
They were created using the `sfxmap.exe` program found in the Aug 15 ISO\jimsbackup.zip  
The build of `sfxmap.exe` used here has been modified slightly to give control over the output directory.  
The built files are a perfect match for the .SAM files found on the retail Dreamcast release.  

**File Format:**  
This file format is best expressed as pseudocode instead of a struct.
```cs
while (!reader.HasReachedEOF()) {
  uint32 identifier = reader.ReadUInt32(); // Could be either an actor uid or a tile ID, based on whether the EntryType is TILE or something else.
  EntryType = (EntryType) reader.ReadUInt32();
  
  List<uint32> hashes = new List<uint32>(); // Hashes are of the .wav file name, minus the extension. They can be set to zero by putting a "0" in the spot of the file name in the source file.
  uint32 num = reader.PeekUInt32(); // There's always a minimum value of one, which is why the loop is structured this way.
  do {
    _ = reader.ReadUInt32(); // Skip the number.
    hashes.add(num);
    num = reader.PeekUInt32();
  } while (num < EntryType.MAX_ENTRY);
}

enum EntryType {
  PLAYER, // 0 Player with full animation range
  MULTIPLAYER_CHARACTER, // 1 Multiplayer character
  ENEMY, // 2 Enemy
  SCENIC, // 3 Scenic
  TILE, // 4 Tile based
  MAX_ENTRY // End of list.
}
```

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
TODO: Detail file format.  

### Dreamcast Image (.PVR)
Folder: `backdrops\*.bmp`  
Source: Dreamcast Backup `F:\Frogger\cd\track03\backdrops\`  
PVR files are a standard format utilized by many Dreamcast games, with tools present in the Dreamcast SDK.  
The only non-matching file is the unused `Frogger2.pvr` file.  

### Texture Anim Bank (.TIT, Texture Interpolation)  
Folder: `textures\*\*.txt`  
Source: Extracted from retail Dreamcast NTSC  
These files have information on texture animations.  
These files are built from .txt files in the texture folders by a program called `animtex.exe`.  
Unfortunately, the original program was not found in any of the backups.  
However, a recreation has been created/included to take the place of the original.  
The files created by this setup perfectly match the retail files.  

**File Format:**  
struct TITFile {
  uint32 numAnimations;
  ANIMATION animations[numAnimations];
}

struct ANIMATION {
  uint32 numFrames;
  uint32 fileNameCrc; // Upper-case, without extension. Eg: '00wtal04.txt' -> '00WTAL04'
}

struct ANIMATION_FRAME {
  uint32 imageNameCrc; // Upper-case, without extension. Eg: '00wtal04.bmp' -> '00WTAL04'
  int32 waitTime; // TODO: What units is this in? When '"' is supplied in the text file it seems to mean copy the last wait time.
}
```

### Game Icon (saveicon.h)  
File: `textures\frogicon16.bmp`  
Source: Dreamcast Backup `C:\Frogger\doschop\frogicon16.bmp`  
The game's save icon is saved as `frogicon16.bmp`.  
Using a Blitz proprietary tool called 'dc_iconchop.exe', we can convert this file into saveicon.h, a file which gets compiled as part of the game.  
It matches the retail versions perfectly.  

## FMV (Video, .SFD)
Folder: `fmv`  
Source: Dreamcast `F:\Frogger\cd\track03\fmv`  
Unfortunately, the FMV setup is not as straightforward as we'd like.  
The tool which converts `.avi` files to `.SFD` files has a GUI and does not appear that it can be run without that GUI.  
The correct .SFD files have been included in their built form because there is no good way around this.  
Please visit [this document](fmv/README.MD) for an explanation on how to create SFD files.  

## Level Data
### Entity Data (.DAT)
Folder: `entities`  
Source: Extracted from retail Dreamcast build.  
Contains entity data such as TODO.  
These files were created/edited in a level editor created inside of the Frogger 2 PC version.  
Most likely one of the PC prototype builds can be used to edit these files.  
Unfortunately, due to prototypes containing other early assets like early level models, it may be difficult to do this.  
In the future, there are plans to build a debug build of the final PC version, including the level editor.  
Alternatively, [FrogLord](https://github.com/Kneesnap/FrogLord) will eventually support editing these files too.  
The files included in this repository were pulled directly from the retail Dreamcast version, and as such match perfectly.  
TODO: Research.  
TODO: Come up with a usable setup.  
TODO: Detail file format.  

### Collision Data (.COL)
Folder: `collision`  
Source: Extracted from retail Dreamcast build.  
TODO: Research  
TODO: Check older prototypes to see if the format ever matches, and to see how the format changed over time.  
TODO: Make sure our obe restorations include data from here.

**Program:**  
A program called `WorldConv.exe` was used to convert .ase level exports into .COL files.  
Unfortunately, all of our backups of this tool are too early to match the file format exactly.  
We have executables from 1998-12-07, 1999-04-06, 1999-05-24, 1999-09-08, and source code from 2000-02-21.  

**File Format:**  
```c
struct COLFile {
  int32 tileCount;
  int32 startTileOffset[4]; // 1 start tile for each player.
  int32 babyCount;
  int32 babyTileIndices[babyCount];
  GAMETILE tiles[tileCount];
OPTIONAL: // This data may not be present.
  CAM_BOX_LIST cam_boxes;
}

struct GAMETILE {
  int32 tileIndices[4]; // north, south, east and west
  uint32 state; // state of tile
  SVECTOR centre;	// centre position
  SVECTOR normal;	// face normal
  SVECTOR dirVector[4]; // the 4 possible camera vectors for this tile
}

struct CAM_BOX_LIST {
  int32 numPlanes;
  CAM_PLANE planes[numPlanes];
  int32 numBoxes;
  CAM_BOX boxes[numBoxes];
}

struct CAM_PLANE {
  FVECTOR	normal;
  fixed k;
  uint32 status;
}

struct CAM_BOX
{
  int32 numPlanes;
  int32 planesStartIndex;
}
```

### Scenic Data (.SCE)
Folder: `scenics`  
Source: Extracted from retail Dreamcast build.  
TODO: Research  
TODO: Check older prototypes to see if the format ever matches, and to see how the format changed over time.  
TODO: Make sure our obe restorations include data from here.

**Program:**  
A program called `ObjectConv.exe`/'objConv.exe' was used to convert .ase level exports into .SCE (& .dll) files.  
Unfortunately, all of our backups of this tool are too early to match the file format exactly.  
We have executables from 1998-12-15, 1999-01-07, 1999-01-14, 1999-05-29, and no source code.  
The tool provided in this repository was first reverse engineered from the most recent build, and then updated to output the right file format.  

**File Format:**  
```c
struct SCE_FILE {
  int32 scenicCount; // The number of scenics.
  SCENIC scenics[scenicCount];
}

struct SCENIC {
  int32 offsetToNextScenic; // Pointer to next scenic in bytes, from the start of the first scenic. -1 means end.
  char name[16]; // The name of the .obe/.psi model.
  SVECTOR pos; // The position to place the scenic at. [Sizes, 8 bytes (2 are padding)]
  IQUATERNION rot; // The rotation to place the scenic with. [Size: 16 bytes]
}
```

### Script (.ETC -> .FEV)
Folder: `scripts`  
Source: Extracted from retail Dreamcast, and decompiled with `scripty`.  
.FEV files are scripts compiled from .ETC files by a program called `etc`.
Unfortunately, this program has not yet been found, and the only uncompiled script backup is from May 22, 2000 on the May 22 backup ISO.  
TODO: Figure this one out later.  
TODO: Maybe document file format?  

## 3D Models
### .BFF (?)
TODO  

### .PIL (PSI Model Library)
TODO  

## Stak Files (.STK)
Folder: `StakFiles`  
Source: Extracted from retail Dreamcast build.  
StakFiles are containers, similar to .zip files, albeit much simpler. They just contain other game files.  
In order to create .STK files, we use the Blitz program `flatstak.exe` with other already existing game files discussed above.   
The StakFiles match the retail release if and only if all of the files which are put into the .STK file match fully.  

**flatstak.exe Modification:**
The flatstak.exe program in the state I found it in did not work properly. This is because it would get a space character in the file names.  
For example, `FROGGER.PIL` would be hashed as `FROGGER.PIL `. It seems like the issue was known, because there was a '-s' flag to fix it.  
Unfortunately, the fix is looking for the space character before the extension, ie it would fix `FROGGER .PIL` instead.  
I fixed this by replacing the period character '.' with NULL, and the position it starts searching for the character to be the end of the string (NULL), it would immediately stop looking for the extension and start searching at the end of the string.  
Changes:  
 - Byte 0000D0B6/0040DCB6, 0xFF -> 0x00, (Changes an offset to the final character in a string to instead be an offset to the null terminator of the string.)  
 - Byte 0000D0C8/0040DCC8, '.' -> '\00', 0x2E -> 0x00 (Changes the search to search for NULL instead of a period.)  

**PSX Comparison:**  
The PSX version is extremely similar, with the only differences to StakFiles in the following:  
 - ancients1.stk (Differences: `ENTITY-1-0.DAT`)  
 - frontend.stk (Differences: `ENTITY-8-0.DAT`)  
 - generic.stk (Dreamcast adds `FROGLET01.PSI` to `FROGLET05.PSI`, and `SWAMPY.PSI`)  
 - lab2.stk (Differences: `LAB3.COL`)  
