@ECHO OFF

if not exist build md build
if not exist build\CIM md build\CIM
if not exist build\GDI md build\GDI

:: Load region.
if not exist "build\last_compiled_region" (
    echo The game region could not be identified. Try recompiling.
    PAUSE
    goto :EOF
)

set /p GAME_REGION=<build\last_compiled_region
if "%GAME_REGION%"=="A" set "GAME_REGION_NAME=NTSC"
if "%GAME_REGION%"=="E" set "GAME_REGION_NAME=PAL"
if "%GAME_REGION%"=="" goto error

if not exist "Frogger\C Application\exe\1ST_READ.BIN" (
    echo Frogger 2 must be compiled before it can be built into a CD.
    PAUSE
    goto :EOF
)

:: Copy ELF to region folder.
copy "Frogger\C Application\exe\1ST_READ.BIN" cd\%GAME_REGION_NAME%\

subst M: cd\

:: Build image.
sdk\KATANA\Utl\Dev\CDCraft\CRFGDC.EXE -bld=cd\frogger2-%GAME_REGION_NAME%.scr,dsk -benv=imf1,imgmap

:: Move to output folder.
move FROGGER2.CIM build\CIM\FROGGER2_%GAME_REGION_NAME%.CIM
move FROGGER2.MAP build\CIM\FROGGER2_%GAME_REGION_NAME%.MAP
sdk\KATANA\Utl\Dev\CDCraft\CRFGDC.EXE -tck=build\CIM\FROGGER2_%GAME_REGION_NAME%.CIM > build\CIM\FROGGER2_TOC_%GAME_REGION_NAME%.TXT

:: Cleanup
subst M: /D

:: Convert CIM to GDI
if exist build\GDI\%GAME_REGION_NAME% del /S build\GDI\%GAME_REGION_NAME%\* /Q
if not exist build\GDI\%GAME_REGION_NAME% MD build\GDI\%GAME_REGION_NAME%\
cd build\GDI\%GAME_REGION_NAME%
..\..\..\cd\tools\cim2gdi ..\..\CIM\FROGGER2_%GAME_REGION_NAME%.CIM
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