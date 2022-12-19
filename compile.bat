@echo OFF
SET COUNTRY_CODE=%1%

:region_select
if "%GAME_REGION%"=="1" set "GAME_REGION=A"
if "%GAME_REGION%"=="USA" set "GAME_REGION=A"
if "%GAME_REGION%"=="NTSC" set "GAME_REGION=A"
if "%GAME_REGION%"=="2" set "GAME_REGION=E"
if "%GAME_REGION%"=="EUR" set "GAME_REGION=E"
if "%GAME_REGION%"=="PAL" set "GAME_REGION=E"
if "%GAME_REGION%"=="E" goto region_ok
if "%GAME_REGION%"=="A" goto region_ok

echo Please select which version you'd like to build.
echo.
echo 1) NTSC
echo 2) PAL
echo.

set /p GAME_REGION=
goto region_select

:region_ok

:: Load the old region.
set "OLD_GAME_REGION=%GAME_REGION%"
if exist "OUTPUT\last_compiled_region" set /p OLD_GAME_REGION=<OUTPUT\last_compiled_region

:: Clean if there's a region switch.
if NOT "%OLD_GAME_REGION%"=="%GAME_REGION%" (
	echo Detected a region change, cleaning up.
	CALL clean.bat
)

:: Save the new region.
echo %GAME_REGION%> OUTPUT\last_compiled_region

:: Sets the KATANA (Dreamcast SDK) path to point to the KATANA folder in this repository.
SET KATANA_ROOT=%~dp0KATANA
echo Using SDK Path: %KATANA_ROOT%

echo Setting up Dreamcast SDK
:: Sets up KATANA (Dreamcast SDK)
CALL %KATANA_ROOT%\SetPaths.bat

:: Chooses the game region.
replace_line "Frogger\C Application\prefix_dc.h" "Frogger\C Application\prefix_dc.h" "//#define\tNTSC_VERSION" "%%REGION_PLACEHOLDER%%"
replace_line "Frogger\C Application\prefix_dc.h" "Frogger\C Application\prefix_dc.h" "#define\tNTSC_VERSION" "%%REGION_PLACEHOLDER%%"
if "%GAME_REGION%"=="E" replace_line "Frogger\C Application\prefix_dc.h" "Frogger\C Application\prefix_dc.h" ""%%REGION_PLACEHOLDER%%"" "//#define\tNTSC_VERSION"
if "%GAME_REGION%"=="A" replace_line "Frogger\C Application\prefix_dc.h" "Frogger\C Application\prefix_dc.h" ""%%REGION_PLACEHOLDER%%"" "#define\tNTSC_VERSION"

:: Build.
echo Compiling Frogger 2 to ELF file.
cd Frogger\C Application\
gmake
if NOT ERRORLEVEL 0 (
    echo Compilation Failure.
	PAUSE
    exit /b 1
)

:: Convert to 1ST_READ.BIN
echo Compilation Successful. Converting Frogger2.elf to 1ST_READ.BIN
cd exe
elf2bin -s 8C010000 Frogger2.elf 1ST_READ.BIN
certutil -hashfile 1ST_READ.BIN SHA1
cd ..\..\..\

echo Success
PAUSE