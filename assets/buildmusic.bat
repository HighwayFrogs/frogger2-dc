@ECHO OFF

:: Clean old output.
IF EXIST COOKED\music DEL /S COOKED\music\* /Q
IF NOT EXIST COOKED\music MD COOKED\music

:: Build .ADX files.
CALL :build_adx 01Garden.wav track01.adx
CALL :build_adx 02Ancients.wav track02.adx
CALL :build_adx 03Space.wav track03.adx
CALL :build_adx 04Subterranean.wav track04.adx
CALL :build_adx 05Laboratory.wav track05.adx
CALL :build_adx 06Halloween.wav track06.adx
CALL :build_adx 07retro.wav track07.adx
CALL :build_adx 08Title.wav track08.adx
CALL :build_adx 09levelc.wav track09.adx
CALL :build_adx 10gameover.wav track10.adx
CALL :build_adx 11EOL.wav track11.adx
PAUSE

goto :EOF

:build_adx
echo Building music track '%2' from '%1'...
SET "INPUT_FILE=music\%1"
SET "OUTPUT_FILE=COOKED\music\%2"

IF NOT EXIST "%INPUT_FILE%" (
    ECHO.
    ECHO The file '%INPUT_FILE%' was not found.
    ECHO This is probably because you have cloned the repository from Git. (Nothing wrong with that!^)
    ECHO Please refer to the text file inside the 'music\' folder for information on why this is and how to fix it.
    ECHO.
    PAUSE
	GOTO :build_adx
)

:: Build the .ADX file.
..\SDK\KATANA\Utl\Gfx\Conv\Movie\ADX\adxencd.exe "%INPUT_FILE%" "%OUTPUT_FILE%"
IF NOT "%ERRORLEVEL%"=="0" PAUSE

:: Verifies it has been created.
IF NOT EXIST "%OUTPUT_FILE%" (
    ECHO The output file '%OUTPUT_FILE%' seems not to have been created.
    PAUSE
)

exit /b