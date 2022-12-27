@ECHO OFF

:: Build PVR files.
IF EXIST COOKED\backdrops DEL /S COOKED\backdrops\* /Q
IF NOT EXIST COOKED\backdrops MD COOKED\backdrops
IF EXIST COOKED\NTSC\ DEL /S COOKED\NTSC\*.pvr /Q
IF NOT EXIST COOKED\NTSC MD COOKED\NTSC
IF EXIST COOKED\PAL\ DEL /S COOKED\PAL\*.pvr /Q
IF NOT EXIST COOKED\PAL MD COOKED\PAL


:: Build .KAT files.
CALL :build_pvr artwork00 artwork00.pvr 1 565
CALL :build_pvr artwork01 artwork01.pvr 2 565
CALL :build_pvr artwork02 artwork02.pvr 2 565
CALL :build_pvr artwork03 artwork03.pvr 3 565
CALL :build_pvr artwork04 artwork04.pvr 4 565
CALL :build_pvr artwork05 artwork05.pvr 5 565
CALL :build_pvr artwork06 artwork06.pvr 6 565
CALL :build_pvr artwork07 artwork07.pvr 7 565
CALL :build_pvr artwork08 artwork08.pvr 8 565
CALL :build_pvr artwork09 artwork09.pvr 9 565
CALL :build_pvr artwork10 artwork10.pvr 10 565
CALL :build_pvr artwork11 artwork11.pvr 11 565
CALL :build_pvr artwork12 artwork12.pvr 12 565
CALL :build_pvr artwork13 artwork13.pvr 13 565
CALL :build_pvr artwork14 artwork14.pvr 14 565
CALL :build_pvr artwork15 artwork15.pvr 15 565
CALL :build_pvr artwork16 artwork16.pvr 16 565
CALL :build_pvr artwork17 artwork17.pvr 17 565
CALL :build_pvr artwork18 artwork18.pvr 19 565
CALL :build_pvr artwork19 artwork19.pvr 20 565
CALL :build_pvr artwork20 artwork20.pvr 21 565
CALL :build_pvr teamphoto artwork21.pvr 22 565
CALL :build_pvr fr2legal fr2legal.pvr 1 565
CALL :build_pvr fr2legalEU fr2legalEU.pvr 1 565
CALL :build_pvr loading loading.pvr 3 565
CALL :build_pvr loadingeu_NTSCDISC loadingeu.pvr 1 565 COOKED\NTSC
CALL :build_pvr loadingeu_PALDISC loadingeu.pvr 1 565 COOKED\PAL
CALL :build_pvr loadingus loadingus.pvr 1 565
CALL :build_pvr sofdec sofdec.pvr 1 1555
PAUSE

goto :EOF

:build_pvr
echo Building image '%1'...

SET "OUTPUT_PATH=COOKED\backdrops"
IF NOT "%5"=="" SET "OUTPUT_PATH=%5"

:: pvrconv.exe works, but doesn't create byte for byte match:
:: DOSPVR is better since we have the source code to the tool, and it outputs byte for byte match.
:: ..\katana\Utl\Gfx\Conv\PVRConv\pvrconv.exe -t -%4 -nomipmap -globalIndex %3 -p %OUTPUT_PATH% backdrops\%1.bmp %2
..\katana\Utl\Gfx\Conv\PVRTool\DOSPVR.exe -Q -GBIX %3 -TWIDDLE -COLOURFORMAT %4 backdrops\%1.bmp -OUTPATH %OUTPUT_PATH% -OUTFILE pvr
IF EXIST "%OUTPUT_PATH%\%1.PVR" (
  :: We've gotta move into a temp file, because "RENAME" fails on two file names which are the same except for different case. Batch really is pathetic.
  RENAME "%OUTPUT_PATH%\%1.PVR" "%1_TEMP.PVR"
  RENAME "%OUTPUT_PATH%\%1_TEMP.PVR" "%2"
) ELSE (PAUSE)
exit /b