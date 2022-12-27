@ECHO OFF

:: Clear previous output.
IF EXIST COOKED\textures DEL /S COOKED\textures\* /Q
IF NOT EXIST COOKED\textures MD COOKED\textures
IF EXIST COOKED\TIT DEL /S COOKED\TIT\* /Q
IF NOT EXIST COOKED\TIT MD COOKED\TIT

:: TODO: Add animtex.exe support once we find or recreate the binary.

:: Build .SPT files.
ECHO Building .SPT files...
CALL :build_spt 64 ancients ancients.spt ancients.tit
CALL :build_spt 64 City city.spt
CALL :build_spt 64 garden garden.spt garden.tit
CALL :build_spt 64 Halloween halloween.spt halloween.tit
CALL :build_spt 256 Hub hub.spt hub.tit
CALL :build_spt 64 Lab lab.spt lab.tit
CALL :build_spt 64 Space space.spt space.tit
CALL :build_spt 64 Subter sub.spt sub.tit
CALL :build_spt 80 Super super.spt retro.tit
CALL :build_spt 128 system generic.spt generic.tit

:: Build PVR
CALL :build_pvr frogger2 Frogger2.pvr 3 1555 COOKED\textures

:: Build icon.
echo Building save icon.
textures\dc_iconchop.exe textures\frogicon16.bmp saveicon.h
IF NOT "%ERRORLEVEL%"=="0" PAUSE
IF EXIST "..\Frogger\C Application\psx\saveicon.h" DEL /F "..\Frogger\C Application\psx\saveicon.h"
MOVE saveicon.h "..\Frogger\C Application\psx\saveicon.h"

PAUSE
goto :EOF

:build_spt
echo Building texture bank '%2'...

CD textures\%2
..\doschop.exe -u -w%1 -f%3 *.bmp
IF NOT "%ERRORLEVEL%"=="0" PAUSE
move %3 ..\..\COOKED\textures
CD ..\..\

:: Build .TIT
IF NOT "%4"=="" (
  ECHO Would build %4 right now, but can't because we don't have this program yet.
::..\animtex.exe textures\%2 %4
::move %4 COOKED\TIT
)

exit /b

:build_pvr
echo Building image '%1'...

:: pvrconv.exe works, but doesn't create byte for byte match:
:: DOSPVR is better since we have the source code to the tool, and it outputs byte for byte match.
::..\katana\Utl\Gfx\Conv\PVRConv\pvrconv.exe -t -%4 -nomipmap -globalIndex %3 -p %5 textures\%1.bmp %2
..\katana\Utl\Gfx\Conv\PVRTool\DOSPVR.exe -Q -GBIX %3 -TWIDDLE -COLOURFORMAT %4 textures\%1.bmp -OUTPATH %5 -OUTFILE PVR
IF EXIST "%5\%1.PVR" (
  :: We've gotta move into a temp file, because "RENAME" fails on two file names which are the same except for different case. Batch really is pathetic.
  RENAME "%5\%1.PVR" "%1_TEMP.PVR"
  RENAME "%5\%1_TEMP.PVR" "%2"
) ELSE (PAUSE)
exit /b