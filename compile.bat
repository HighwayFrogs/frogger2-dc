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
if not exist build mkdir build
if exist "build\last_compiled_region" set /p OLD_GAME_REGION=<build\last_compiled_region

:: Clean if there's a region switch.
if NOT "%OLD_GAME_REGION%"=="%GAME_REGION%" (
	echo Detected a region change, cleaning up.
	CALL clean.bat
)

:: Save the new region.
echo %GAME_REGION%> build\last_compiled_region

:: Sets the KATANA (Dreamcast SDK) path to point to the KATANA folder in this repository.
SET KATANA_ROOT=%~dp0sdk\KATANA
echo Using SDK Path: %KATANA_ROOT%

echo Setting up Dreamcast SDK
:: Sets up KATANA (Dreamcast SDK)
CALL %KATANA_ROOT%\SetPaths.bat

:: Chooses the game region.
CALL :remove_region_from_code
if "%GAME_REGION%"=="E" replace_line "src\prefix_dc.h" "src\prefix_dc.h" ""%%REGION_PLACEHOLDER%%"" "//#define\tNTSC_VERSION"
if "%GAME_REGION%"=="A" replace_line "src\prefix_dc.h" "src\prefix_dc.h" ""%%REGION_PLACEHOLDER%%"" "#define\tNTSC_VERSION"

:: Build.
echo Compiling Frogger 2 to ELF file.
cd src\
IF EXIST exe\Frogger2.elf DEL exe\Frogger2.elf
IF EXIST exe\1ST_READ.BIN DEL exe\1ST_READ.BIN
gmake
if NOT "%ERRORLEVEL%"=="0" (
    cd ..\..\
	CALL :remove_region_from_code
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

CALL :remove_region_from_code

echo Success
PAUSE
goto :EOF

:remove_region_from_code
replace_line "src\prefix_dc.h" "src\prefix_dc.h" "//#define\tNTSC_VERSION" "%%REGION_PLACEHOLDER%%"
replace_line "src\prefix_dc.h" "src\prefix_dc.h" "#define\tNTSC_VERSION" "%%REGION_PLACEHOLDER%%"
exit /b