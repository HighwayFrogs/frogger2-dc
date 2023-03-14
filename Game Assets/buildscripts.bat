@ECHO OFF

:: Clear previous output.
IF EXIST COOKED\scripts DEL /S COOKED\scripts\* /Q
IF NOT EXIST COOKED\scripts MD COOKED\scripts
IF EXIST COOKED\scripts\super1 DEL /S COOKED\scripts\super1\* /Q
IF NOT EXIST COOKED\scripts\super1 MD COOKED\scripts\super1

:: Build scripts.
ECHO Compiling scripts...
CALL :compile_script script-0-0.etc SCRIPT-0-0.FEV
CALL :compile_script script-1-0.etc SCRIPT-1-0.FEV
CALL :compile_script script-1-2.etc SCRIPT-1-2.FEV
CALL :compile_script script-1-3.etc SCRIPT-1-3.FEV
CALL :compile_script script-1-5.etc SCRIPT-1-5.FEV
CALL :compile_script script-2-0.etc SCRIPT-2-0.FEV
CALL :compile_script script-2-2.etc SCRIPT-2-2.FEV
CALL :compile_script script-2-3.etc SCRIPT-2-3.FEV
CALL :compile_script script-3-5.etc SCRIPT-3-5.FEV
CALL :compile_script script-4-0.etc SCRIPT-4-0.FEV
CALL :compile_script script-4-1.etc SCRIPT-4-1.FEV
CALL :compile_script script-4-3.etc SCRIPT-4-3.FEV
CALL :compile_script script-5-0.etc SCRIPT-5-0.FEV
CALL :compile_script script-5-2.etc SCRIPT-5-2.FEV
CALL :compile_script script-5-3.etc SCRIPT-5-3.FEV
CALL :compile_script script-5-4.etc SCRIPT-5-4.FEV
CALL :compile_script script-5-5.etc SCRIPT-5-5.FEV
CALL :compile_script script-5-6.etc SCRIPT-5-6.FEV
CALL :compile_script script-6-0.etc SCRIPT-6-0.FEV
CALL :compile_script script-6-1.etc SCRIPT-6-1.FEV
CALL :compile_script script-6-4.etc SCRIPT-6-4.FEV
CALL :compile_script script-7-0.etc SCRIPT-7-0.FEV
CALL :compile_script script-7-1.etc SCRIPT-7-1.FEV
CALL :compile_script script-7-2.etc SCRIPT-7-2.FEV
CALL :compile_script script-7-3.etc SCRIPT-7-3.FEV
CALL :compile_script script-7-4.etc SCRIPT-7-4.FEV
CALL :compile_script script-7-5.etc SCRIPT-7-5.FEV
CALL :compile_script script-7-6.etc SCRIPT-7-6.FEV
CALL :compile_script script-7-7.etc SCRIPT-7-7.FEV
CALL :compile_script script-7-8.etc SCRIPT-7-8.FEV
CALL :compile_script script-7-9.etc SCRIPT-7-9.FEV
CALL :compile_script script-7-10.etc SCRIPT-7-10.FEV
CALL :compile_script script-7-11.etc SCRIPT-7-11.FEV
CALL :compile_script script-7-15.etc SCRIPT-7-15.FEV
CALL :compile_script script-7-18.etc SCRIPT-7-18.FEV
CALL :compile_script script-8-0.etc SCRIPT-8-0.FEV
CALL :compile_script script-8-3.etc SCRIPT-8-3.FEV
CALL :compile_script super1\script-7-0.etc super1\SCRIPT-7-0.FEV

PAUSE
goto :EOF

:compile_script
SET "INPUT_SCRIPT=scripts\%1"
SET "OUTPUT_FILE=COOKED\scripts\%2"
ECHO Compiling script file '%INPUT_SCRIPT%'...

IF NOT EXIST "%INPUT_SCRIPT%" ..\KATANA\Tools\scripty.exe decompile "scripts\%2" "%INPUT_SCRIPT%"
..\KATANA\Tools\scripty.exe compile "%INPUT_SCRIPT%" "%OUTPUT_FILE%"
IF NOT "%ERRORLEVEL%"=="0" PAUSE

exit /b