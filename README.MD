# Frogger 2: Swampy's Revenge (Dreamcast)  
This is a backup of Frogger 2's official source code from October 10th, 2000.  

This builds a perfect match for the retail PAL submission of Frogger 2: Swampys' Revenge. (Code: T-40603D-50, Version: V1.001)  


The PAL version of Swampy's Revenge was never released, but it was submitted to Sega of Europe for release review.  
Because of that submission, we have enough information to prove this builds a perfect match for an unreleased PAL retail build.  


The retail version of the game was built on October 13th, 2000.  

**Developer:** Blitz Games  

## How to compile and run?
**1)** Run `compile.bat`.  
The resulting output will be called Frogger2.elf.  
It is automatically converted to a file named 1ST_READ.BIN.  
You can find these files at `Frogger\C Application\exe\`.  
**2)** Run `buildcd.bat`.  
A CDI image which can be run in an emulator or burned to a CD is now at `OUTPUT\Frogger2.CDI`.  

## How to switch between PAL and NTSC?  
**1)** Add or remove the line of text `#define	NTSC_VERSION` from `C Application\prefix_dc.h` as a way of choosing between PAL/EUR and NTSC/USA.  
**2)** Run `clean.bat`  
**3)** Run `compile.bat`  

Preserved by [Highway Frogs](https://highwayfrogs.net/)