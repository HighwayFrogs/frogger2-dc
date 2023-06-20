@echo OFF

:: Setup directories.
SET FROGGER_DIR=%~dp0src\
SET EXEDIR=%FROGGER_DIR%exe\
SET OBJDIR=%FROGGER_DIR%obj\
SET DWFDIR=%OBJDIR%dwfinf\

:: Delete files.
DEL "%OBJDIR%*.o"
if NOT "%ERRORLEVEL%"=="0" goto error
DEL "%OBJDIR%*.obj"
if NOT "%ERRORLEVEL%"=="0" goto error
DEL "%OBJDIR%*.dwf"
if NOT "%ERRORLEVEL%"=="0" goto error
IF EXIST "%DWFDIR%" DEL "%DWFDIR%*.dwi"
if NOT "%ERRORLEVEL%"=="0" goto error
IF EXIST "%EXEDIR%1ST_READ.BIN" DEL "%EXEDIR%1ST_READ.BIN"
if NOT "%ERRORLEVEL%"=="0" goto error
IF EXIST "%EXEDIR%Frogger2.elf" DEL "%EXEDIR%Frogger2.elf"
if NOT "%ERRORLEVEL%"=="0" goto error

goto done

:: Done
:error
echo An error occurred when cleaning out files.
PAUSE

:done
echo Success
goto :EOF