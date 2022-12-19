if not exist OUTPUT md OUTPUT
if not exist OUTPUT\CIM md OUTPUT\CIM
if not exist OUTPUT\GDI md OUTPUT\GDI

:: Load region.
if not exist "OUTPUT\last_compiled_region" (
    echo The game region could not be identified. Try recompiling.
    PAUSE
    goto :EOF
)

set /p GAME_REGION=<OUTPUT\last_compiled_region
if "%GAME_REGION%"=="A" set "GAME_REGION_NAME=NTSC"
if "%GAME_REGION%"=="E" set "GAME_REGION_NAME=PAL"
if "%GAME_REGION%"=="" goto error

if not exist "Frogger\C Application\exe\1ST_READ.BIN" (
    echo Frogger 2 must be compiled before it can be built into a CD.
    PAUSE
    goto :EOF
)

:: Copy ELF to PAL folder.
copy "Frogger\C Application\exe\1ST_READ.BIN" Frogger\cd\%GAME_REGION_NAME%\

subst M: Frogger\cd\

:: Build image.
KATANA\Utl\Dev\CDCraft\CRFGDC.EXE -bld=Frogger\cd\frogger2-%GAME_REGION_NAME%.scr,dsk -benv=imf1,imgmap

:: Move to output folder.
move FROGGER2.CIM OUTPUT\CIM\FROGGER2_%GAME_REGION_NAME%.CIM
move FROGGER2.MAP OUTPUT\CIM\FROGGER2_%GAME_REGION_NAME%.MAP
KATANA\Utl\Dev\CDCraft\CRFGDC.EXE -tck=OUTPUT\CIM\FROGGER2_%GAME_REGION_NAME%.CIM > OUTPUT\CIM\FROGGER2_TOC_%GAME_REGION_NAME%.TXT

:: Cleanup
subst M: /D

:: Convert CIM to GDI
if exist OUTPUT\GDI\%GAME_REGION_NAME% del /S OUTPUT\GDI\%GAME_REGION_NAME%\* /Q
if not exist OUTPUT\GDI\%GAME_REGION_NAME% MD OUTPUT\GDI\%GAME_REGION_NAME%\
cd OUTPUT\GDI\%GAME_REGION_NAME%
..\..\..\Frogger\cd\tools\cim2gdi ..\..\CIM\FROGGER2_%GAME_REGION_NAME%.CIM
RENAME game.gdi "Frogger 2 - Swampy's Revenge.gdi"
cd ..\..\..\

:: Done
echo Build Complete
PAUSE
goto :EOF

:error
echo Failed to get region name.
PAUSE
goto :EOF