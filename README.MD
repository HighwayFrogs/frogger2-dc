![Frogger 2 Logo](/_repo/Frogger2Logo.png)
# Frogger 2: Swampy's Revenge (Dreamcast)  
This is a backup of Frogger 2's official source code from October 10th, 2000.  
It can build a perfect match for the NTSC and PAL retail versions of Frogger 2.  
This was achieved by reverse engineering the NTSC version's changes.  

**Developer:** Blitz Games  

## How to compile and run?  
**1)** Run `compile.bat`.  
The resulting output will be called Frogger2.elf.  
It is automatically converted to a file named 1ST_READ.BIN.  
You can find these files at `Frogger\C Application\exe\`.  
**2)** Run `buildcdi.bat` or `buildgdi.bat`.  
A CD image will now be available in the `OUTPUT\` folder.  
A CDI image is one which is built with modern tools and can be burned to a CD and played on a retail console or emulator.  
A GDI image is the master disc image format. It can be played in emulators, and requires official GD-R discs in order to play on a retail console.  

## Retail Build Information:  
Both of the retail builds are built to a perfect byte for byte match from this repository.  

**Retail NTSC:**  
Executable Date: September 10th, 2000 (V1.001)  
Product ID: T40604N  
SHA1 Hash (1ST_READ.BIN): `3dbddec07e78e0e8c5830bbbf60c9ecdb17e4a42`  
Use [gditools](https://sourceforge.net/projects/dcisotools/) to work with this build, instead of `jc-gdrom-explorer`, which doesn't work for some reason.  
This repository can build a perfect match for this build, and thus can be considered "NTSC master".  

**Retail PAL:**  
Executable Date: October 10th, 2000 (V1.001)  
Product ID: T-40603D-50  
SHA1 Hash (1ST_READ.BIN): `089e2f9d1c04e26e0fa947101c4db15fee9c43e2`  
This build was never (officially) released, but it was submitted to Sega of Europe for approval.  
However, this repository can build a perfect match for the build submitted to Sega, and thus can be considered as "PAL master".  


**Other Executable:**  
There is another executable with SHA1 hash `bfee1b19ee7b28d19ff54e3fbedc1042dd54f51d` with a date of October 13th, 2000.  
This executable appears to be a copy of the NTSC version which had the Dreamcast SDK tampered with to support a new display mode.  
It is not known where it came from, but it does not appear legitimate.  
This is because the changes appear to be done to the compiled binary, and not to source code.  
As such, it is likely this executable was modified by a scene group, and is not supported by this repository.  

## Asset Information
There is an unfinished setup for converting game assets to the Dreamcast version in the `assets\` folder.  
It was created to better understand the game formats, and to create recreations.  
However, in the time since it was started, the "End of Project" tape backup was successfully recovered.  
This backup contains a full backup of final game assets, as well as all of the original tools used to manage game assets + their source code.  
The attempt to finish the asset pipeline before releasing the repository was abandoned because of the availability of the original.  

Preserved by [Highway Frogs](https://highwayfrogs.net/)