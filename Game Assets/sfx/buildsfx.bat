@ECHO OFF

:: Build sfx files.
IF EXIST ..\COOKED\sfx DEL /S ..\COOKED\sfx\* /Q
IF NOT EXIST ..\COOKED\sfx MD ..\COOKED\sfx

:: Build .KAT files.
CALL :build_kat ancients 1
CALL :build_kat Frogger 0
CALL :build_kat Frontend 1
CALL :build_kat garden 0
CALL :build_kat halloween 1
CALL :build_kat lab 1
CALL :build_kat lillie 0
CALL :build_kat multi 1
CALL :build_kat roobie 0
CALL :build_kat space 1
CALL :build_kat sub 1
CALL :build_kat Superretro 1
CALL :build_kat Swampy 0
CALL :build_kat Tad 0
CALL :build_kat tank 0
CALL :build_kat Treeby 0
CALL :build_kat wart 0
PAUSE

goto :EOF

:build_kat
echo Building sound bank '%1'...
IF EXIST %1.oss DEL %1.oss
MkScript2.exe %1 %1
IF NOT "%ERRORLEVEL%"=="0" PAUSE
..\..\katana\Utl\Snd\Pc\MkBank.exe %1.oss
IF NOT "%ERRORLEVEL%"=="0" PAUSE


dir %1\*.wav /b /O >%1.txt
IF "%2"=="1" (
    ..\..\KATANA\Tools\replace_line.exe %1.txt %1.txt "frog_gib.wav\n" ""
    ..\..\KATANA\Tools\replace_line.exe %1.txt %1.txt "frogmowed.wav\n" "frogmowed.wav\nfrog_gib.wav\n"
)

del %1.h
del %1.oss

MOVE %1.kat ..\COOKED\sfx\
MOVE %1.txt ..\COOKED\sfx\

exit /b