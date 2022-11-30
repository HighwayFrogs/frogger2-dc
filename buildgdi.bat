del /S OUTPUT\* /Q
if not exist OUTPUT md OUTPUT
if not exist OUTPUT\CIM md OUTPUT\CIM
if not exist OUTPUT\GDI md OUTPUT\GDI

if not exist "Frogger\C Application\exe\1ST_READ.BIN" (
    echo Frogger 2 must be compiled before it can be built into a CD.
    PAUSE
    goto :EOF
)

:: Copy ELF to track05.
copy "Frogger\C Application\exe\1ST_READ.BIN" Frogger\cd\track05\

subst M: Frogger\cd\

:: Build image.
KATANA\Utl\Dev\CDCraft\CRFGDC.EXE -bld=Frogger\cd\frogger2.scr,dsk -benv=imf1,imgmap

:: Move to output folder.
move FROGGER2.CIM OUTPUT\CIM\
move FROGGER2.MAP OUTPUT\CIM\
KATANA\Utl\Dev\CDCraft\CRFGDC.EXE -tck=OUTPUT\CIM\FROGGER2.CIM > OUTPUT\CIM\frogger2_toc.txt

:: Cleanup
subst M: /D

:: Convert CIM to GDI
cd OUTPUT\GDI
..\..\Frogger\cd\tools\cim2gdi ..\CIM\FROGGER2.CIM
RENAME game.gdi "Frogger 2 - Swampy's Revenge.gdi"
cd ..\..\

:: Done
echo Build Complete
PAUSE
goto :EOF
