@echo OFF

:: Setup directories.
SET FROGGER_DIR=%~dp0Frogger\C Application\
SET EXEDIR=%FROGGER_DIR%exe\
SET OBJDIR=%FROGGER_DIR%obj\
SET DWFDIR=%OBJDIR%dwfinf\

:: Delete files.
DEL "%OBJDIR%*.o"
DEL "%OBJDIR%*.obj"
DEL "%OBJDIR%*.dwf"
IF EXIST "%DWFDIR%" DEL "%DWFDIR%*.dwi"
IF EXIST "%EXEDIR%1ST_READ.BIN DEL "%EXEDIR%1ST_READ.BIN"
IF EXIST "%EXEDIR%Frogger2.elf DEL "%EXEDIR%Frogger2.elf"

:: Done
echo Success
PAUSE