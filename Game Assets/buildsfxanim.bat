@ECHO OFF

:: Setup folders.
IF EXIST COOKED\sfxanim DEL /S COOKED\sfxanim\* /Q
IF NOT EXIST COOKED\sfxanim MD COOKED\sfxanim

:: Build .sam files.
CALL :build_sam sfxanim00
CALL :build_sam sfxanim10
CALL :build_sam sfxanim11
CALL :build_sam sfxanim12
CALL :build_sam sfxanim13
CALL :build_sam sfxanim20
CALL :build_sam sfxanim21
CALL :build_sam sfxanim22
CALL :build_sam sfxanim30
::CALL :build_sam sfxanim31
::CALL :build_sam sfxanim32
::CALL :build_sam sfxanim33
CALL :build_sam sfxanim40
CALL :build_sam sfxanim41
CALL :build_sam sfxanim42
::CALL :build_sam sfxanim43
CALL :build_sam sfxanim50
CALL :build_sam sfxanim51
CALL :build_sam sfxanim52
CALL :build_sam sfxanim53
CALL :build_sam sfxanim54
CALL :build_sam sfxanim55
CALL :build_sam sfxanim60
CALL :build_sam sfxanim61
CALL :build_sam sfxanim62
CALL :build_sam sfxanim70
CALL :build_sam sfxanim71
CALL :build_sam sfxanim72
CALL :build_sam sfxanim73
CALL :build_sam sfxanim74
CALL :build_sam sfxanim75
CALL :build_sam sfxanim76
CALL :build_sam sfxanim77
CALL :build_sam sfxanim78
CALL :build_sam sfxanim79
CALL :build_sam sfxanim7a
CALL :build_sam sfxanim7b
::CALL :build_sam sfxanim7c
::CALL :build_sam sfxanim7d
CALL :build_sam sfxanim80
CALL :build_sam sfxanim83
PAUSE

goto :EOF

:build_sam
echo Building sfxanim '%1'...

SET "OUTPUT_FILE=COOKED\sfxanim\%1.sam"
IF NOT "%2"=="" SET "OUTPUT_FILE=%2"

sfxanim\sfxmap.exe "sfxanim\%1.txt" "%OUTPUT_FILE%"
SET "EXIT_CODE=%ERRORLEVEL%"
IF NOT "%EXIT_CODE%"=="0" (
  ECHO Exit Code: %EXIT_CODE%
  PAUSE
)

exit /b