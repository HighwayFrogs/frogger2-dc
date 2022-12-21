@ECHO OFF

:: Build sfx files.
IF EXIST ..\COOKED\sfx DEL /S ..\COOKED\sfx\* /Q
IF NOT EXIST ..\COOKED\sfx MD ..\COOKED\sfx

:: Build .KAT files.
CALL :build_kat ancients
CALL :build_kat Frogger
CALL :build_kat Frontend
CALL :build_kat garden
CALL :build_kat halloween
CALL :build_kat lab
CALL :build_kat lillie
CALL :build_kat multi
CALL :build_kat roobie
CALL :build_kat space
CALL :build_kat sub
CALL :build_kat Superretro
CALL :build_kat Swampy
CALL :build_kat Tad
CALL :build_kat tank
CALL :build_kat Treeby
CALL :build_kat wart
PAUSE

goto :EOF

:build_kat
echo Building sound bank '%1'...
del %2.oss
MkScript2.exe %1 %1
IF NOT "%ERRORLEVEL%"=="0" PAUSE
..\..\katana\Utl\Snd\Pc\MkBank.exe %1.oss
IF NOT "%ERRORLEVEL%"=="0" PAUSE
dir %1\*.wav /b >%1.txt
del %1.h
del %1.oss

MOVE %1.kat ..\COOKED\sfx\
MOVE %1.txt ..\COOKED\sfx\
::copy 

exit /b