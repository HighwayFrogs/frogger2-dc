@echo OFF

REM Sets the KATANA (Dreamcast SDK) path to point to the KATANA folder in this repository.
SET KATANA_ROOT=%~dp0KATANA
echo Using SDK Path: %KATANA_ROOT%

echo Setting up Dreamcast SDK
REM Sets up KATANA (Dreamcast SDK)
CALL %KATANA_ROOT%\SetPaths.bat

REM Build.
echo Compiling Frogger 2 to ELF file.
cd Frogger\C Application\
gmake
if NOT ERRORLEVEL 0 (
    echo Compilation Failure.
	PAUSE
    exit /b 1
)

REM Convert to 1ST_READ.BIN
echo Compilation Successful. Converting Frogger2.elf to 1ST_READ.BIN
cd exe
elf2bin -s 8C010000 Frogger2.elf 1ST_READ.BIN
certutil -hashfile 1ST_READ.BIN SHA1
cd ..\..\..\

echo Success
PAUSE