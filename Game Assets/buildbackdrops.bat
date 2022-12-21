@ECHO OFF

:: Build PVR files.
IF EXIST COOKED\backdrops DEL /S COOKED\backdrops\* /Q
IF NOT EXIST COOKED\backdrops MD COOKED\backdrops

:: Build .KAT files.
CALL :build_pvr artwork00.bmp artwork00.pvr 1
CALL :build_pvr artwork01.bmp artwork01.pvr 2
CALL :build_pvr artwork02.bmp artwork02.pvr 2
CALL :build_pvr artwork03.bmp artwork03.pvr 3
CALL :build_pvr artwork04.bmp artwork04.pvr 4
CALL :build_pvr artwork05.bmp artwork05.pvr 5
CALL :build_pvr artwork06.bmp artwork06.pvr 6
CALL :build_pvr artwork07.bmp artwork07.pvr 7
CALL :build_pvr artwork08.bmp artwork08.pvr 8
CALL :build_pvr artwork09.bmp artwork09.pvr 9
CALL :build_pvr artwork10.bmp artwork10.pvr 10
CALL :build_pvr artwork11.bmp artwork11.pvr 11
CALL :build_pvr artwork12.bmp artwork12.pvr 12
CALL :build_pvr artwork13.bmp artwork13.pvr 13
CALL :build_pvr artwork14.bmp artwork14.pvr 14
CALL :build_pvr artwork15.bmp artwork15.pvr 15
CALL :build_pvr artwork16.bmp artwork16.pvr 16
CALL :build_pvr artwork17.bmp artwork17.pvr 17
CALL :build_pvr artwork18.bmp artwork18.pvr 19
CALL :build_pvr artwork19.bmp artwork19.pvr 20
CALL :build_pvr artwork20.bmp artwork20.pvr 21
CALL :build_pvr teamphoto.bmp artwork21.pvr 22
CALL :build_pvr fr2legal.bmp fr2legal.pvr 1
CALL :build_pvr fr2legalEU.bmp fr2legalEU.pvr 1
CALL :build_pvr loading.bmp loading.pvr 3
CALL :build_pvr loadingeu.bmp loadingeu.pvr 1
CALL :build_pvr loadingus.bmp loadingus.pvr 1
CALL :build_pvr sofdec.bmp sofdec.pvr 1
PAUSE

goto :EOF

:build_pvr
echo Building image '%1'...

SET "FORMAT_ARGUMENT=-565"
IF "%2"=="sofdec.pvr" SET "FORMAT_ARGUMENT=-1555"

..\katana\Utl\Gfx\Conv\PVRConv\pvrconv.exe -t %FORMAT_ARGUMENT% -nomipmap -globalIndex %3 -p COOKED\backdrops\ backdrops\%1 %2
IF NOT "%ERRORLEVEL%"=="1" PAUSE
exit /b