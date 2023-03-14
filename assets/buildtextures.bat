@ECHO OFF

:: Clear previous output.
IF EXIST COOKED\textures DEL /S COOKED\textures\* /Q
IF NOT EXIST COOKED\textures MD COOKED\textures
IF EXIST COOKED\texanims DEL /S COOKED\texanims\* /Q
IF NOT EXIST COOKED\texanims MD COOKED\texanims

:: Build .SPT files.
ECHO Building .SPT files...
CALL :build_spt 64 ancients ancients.spt
CALL :build_spt 64 City city.spt
CALL :build_spt 64 garden garden.spt
CALL :build_spt 64 Halloween halloween.spt
CALL :build_spt 256 Hub hub.spt
CALL :build_spt 64 Lab lab.spt
CALL :build_spt 64 Space space.spt
CALL :build_spt 64 Subter sub.spt
CALL :build_spt 80 Super super.spt
CALL :build_spt 128 system generic.spt

:: Build TIT files.
ECHO Building .TIT files...
CALL :build_tit ancients ANCIENTS.TIT "00tfin04.txt" "00wtal04.txt"
CALL :build_tit garden GARDEN.TIT "00bwing.txt" "00moa02.txt" "00roll02.txt" "00tfin04.txt"
CALL :build_tit Halloween HALLOWEEN.TIT "aniswamp00.txt" "aniswamp03.txt" "aniswamp04.txt" "Fire0000.txt" "Flame1.txt" "gst01.txt" "halpanel3.txt" "tubarrow.txt" "tubarrow2.txt"
CALL :build_tit Hub HUB.TIT "eyes01.txt" "eyes02.txt" "eyes03.txt"
CALL :build_tit Lab LAB.TIT "00blade02.txt" "00nois04.txt" "bfrog01.txt" "lairfl4.txt" "lbin003.txt" "mastb007.txt" "pipe8.txt" "pipe9.txt" "plas201.txt" "white01.txt"
CALL :build_tit Super RETRO.TIT "00flas04.txt" "00turt04.txt" "efect1.txt" "snakhed1.txt" "snaktal1.txt" "test6a.txt"
CALL :build_tit Space SPACE.TIT "00blade02.txt" "00nois04.txt" "00psy04.txt" "eyes03.txt" "icoarrowd.txt" "plas201.txt"
CALL :build_tit Subter SUB.TIT "00ants04.txt" "00bwing.txt" "00gold04.txt" "00lava04.txt" "00rail04.txt" "00raila04.txt" "Wat0.txt"
CALL :build_tit system GENERIC.TIT "00babyic.txt" "rgarib01.txt" "scoin0001.txt"

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

exit /b

:build_tit
SET "TEXTURE_FOLDERNAME=%1"
SET "TIT_NAME=%2"
for /f "tokens=2,* delims= " %%a in ("%*") do set TXT_INPUT_FILES=%%b
ECHO Building texture animation file '%TIT_NAME%'...
CD textures\%TEXTURE_FOLDERNAME%\animations
..\..\..\..\SDK\Tools\animtex2022.exe "..\..\..\COOKED\texanims\%TIT_NAME%" %TXT_INPUT_FILES%
IF NOT "%ERRORLEVEL%"=="0" PAUSE
CD ..\..\..\
exit /b

:build_pvr
echo Building image '%1'...

:: pvrconv.exe works, but doesn't create byte for byte match:
:: DOSPVR is better since we have the source code to the tool, and it outputs byte for byte match.
::..\SDK\KATANA\Utl\Gfx\Conv\PVRConv\pvrconv.exe -t -%4 -nomipmap -globalIndex %3 -p %5 textures\%1.bmp %2
..\SDK\KATANA\Utl\Gfx\Conv\PVRTool\DOSPVR.exe -Q -GBIX %3 -TWIDDLE -COLOURFORMAT %4 textures\%1.bmp -OUTPATH %5 -OUTFILE PVR
IF EXIST "%5\%1.PVR" (
  :: We've gotta move into a temp file, because "RENAME" fails on two file names which are the same except for different case. Batch really is pathetic.
  RENAME "%5\%1.PVR" "%1_TEMP.PVR"
  RENAME "%5\%1_TEMP.PVR" "%2"
) ELSE (PAUSE)
exit /b