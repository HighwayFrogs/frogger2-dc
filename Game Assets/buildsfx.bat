@ECHO OFF

:: Build sfx files.
IF EXIST COOKED\sfx DEL /S COOKED\sfx\* /Q
IF NOT EXIST COOKED\sfx MD COOKED\sfx

:: Build .KAT files.
CALL :build_kat ancients ancients
CALL :build_kat Frogger frogger
CALL :build_kat Frontend FRONTEND
CALL :build_kat garden garden
CALL :build_kat halloween halloween
CALL :build_kat lab lab
CALL :build_kat lillie lillie
CALL :build_kat multi multi
CALL :build_kat roobie roobie
CALL :build_kat space space
CALL :build_kat sub sub
CALL :build_kat Superretro superretro
CALL :build_kat Swampy swampy
CALL :build_kat Tad tad
CALL :build_kat tank tank
CALL :build_kat Treeby treeby
CALL :build_kat wart wart
PAUSE

goto :EOF

:build_kat
echo Building sound bank '%2'...
IF EXIST "%2.oss" DEL "%2.oss"
IF EXIST "%2.txt" DEL "%2.txt"

:: Create oss if it isn't already provided.
:: Files already created are used in order to enforce retail version file matching, and should be deleted if changes are desired.
IF EXIST "sfx\%1\%2.oss" (
    COPY "sfx\%1\%2.oss" ".\%2.oss"
) ELSE (
    sfx\MkScript2.exe %2 sfx\%1
    IF NOT "%ERRORLEVEL%"=="0" PAUSE
)

:: Build the .KAT file.
..\katana\Utl\Snd\Pc\MkBank.exe %2.oss
IF NOT "%ERRORLEVEL%"=="0" PAUSE

:: Create txt if it isn't already provided.
:: Files already created here are used in order to enforce retail version file matching and should be deleted if changes are desired.
SET "TXT_FILE_PATH=%2.txt"
IF "%2"=="FRONTEND" SET "TXT_FILE_PATH=frontend.txt"

IF EXIST "sfx\%1\%TXT_FILE_PATH%" (
    COPY "sfx\%1\%TXT_FILE_PATH%" ".\%TXT_FILE_PATH%"
) ELSE (
    :: Can't sort, because we need to maintain the order which mkscript2.exe is going to use.
    DIR sfx\%1\*.wav /b >%TXT_FILE_PATH%
)

del %2.h
del %2.oss

MOVE %2.kat COOKED\sfx\
MOVE %TXT_FILE_PATH% COOKED\sfx\

exit /b