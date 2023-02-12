@ECHO off

SET TOOL_FOLDER=Frogger\cd\tools
SET EXECUTABLE_LOCATION=Frogger\C Application\exe\1ST_READ.BIN
SET OUTPUT_FOLDER=OUTPUT
SET CD_FILES=OUTPUT\CDI\Files
SET TEMP_ISO_FILE=%OUTPUT_FOLDER%\TEMP.ISO

:: Load region.
if not exist "OUTPUT\last_compiled_region" (
    echo The game region could not be identified. Try recompiling.
    PAUSE
    goto :EOF
)

set /p GAME_REGION=<OUTPUT\last_compiled_region
if "%GAME_REGION%"=="A" set "GAME_REGION_NAME=NTSC"
if "%GAME_REGION%"=="E" set "GAME_REGION_NAME=PAL"
if "%GAME_REGION%"=="" goto error

:: Verify game is compiled.
if not exist "%EXECUTABLE_LOCATION%" (
    echo Frogger 2 must be compiled first.
    PAUSE
    goto :EOF
)

:: Cleanup output folder.
del /S "%OUTPUT_FOLDER%\CDI\%GAME_REGION_NAME%\*" /Q
if not exist "%OUTPUT_FOLDER%\CDI\%GAME_REGION_NAME%" md "%OUTPUT_FOLDER%\CDI\%GAME_REGION_NAME%"

:: Setup CD File Folder
if not exist %CD_FILES% (
    CALL :create_build_folder
)

:: Copy compiled game.
IF EXIST "%CD_FILES%\1ST_READ.BIN" DEL "%CD_FILES%\1ST_READ.BIN"  >nul
IF EXIST "%CD_FILES%\IP.BIN" DEL "%CD_FILES%\IP.BIN"  >nul
IF EXIST "%CD_FILES%\LANGUAGE.TXT" DEL "%CD_FILES%\LANGUAGE.TXT"  >nul
IF EXIST "%CD_FILES%\BACKDROPS\LOADINGEU.PVR" DEL "%CD_FILES%\BACKDROPS\LOADINGEU.PVR" >nul
copy "%EXECUTABLE_LOCATION%" .\ <nul
copy "Frogger\cd\%GAME_REGION_NAME%\IP.BIN" .\ <nul
copy "Frogger\cd\%GAME_REGION_NAME%\LANGUAGE.TXT" "%CD_FILES%\" >nul
copy "Frogger\cd\%GAME_REGION_NAME%\LOADINGEU.PVR" "%CD_FILES%\BACKDROPS\" >nul

:: Fix boot file.
echo.
echo Fixing the main executable...
(
echo 1ST_READ.BIN
echo IP.BIN
echo 0
)|"%TOOL_FOLDER%\binhack32"

:: Move the fixed files into the CD folder.
move 1ST_READ.BIN "%CD_FILES%" >nul
move IP.BIN "%CD_FILES%" >nul

echo.
echo Creating ISO...
"%TOOL_FOLDER%\mkisofs" -C 0,0 -G "%CD_FILES%\IP.BIN" -V FROGGER2 -joliet -rock -l -o "%TEMP_ISO_FILE%" "%CD_FILES%"

echo.
echo Creating CDI from ISO...
"%TOOL_FOLDER%\cdi4dc" "%TEMP_ISO_FILE%" "%OUTPUT_FOLDER%\CDI\%GAME_REGION_NAME%\FROGGER2.cdi" -d

::Delete files.
DEL %TEMP_ISO_FILE%

echo Build Complete
PAUSE
GOTO :EOF

:create_build_folder
echo Setting up the %CD_FILES% folder, which holds the files to be written to the CD. Please wait...
md %CD_FILES% <nul
md %CD_FILES%\BACKDROPS <nul
md %CD_FILES%\DEMOS <nul
md %CD_FILES%\FMV <nul
md %CD_FILES%\FONTS <nul
md %CD_FILES%\INCBIN <nul
md %CD_FILES%\MUSIC <nul
md %CD_FILES%\SFX <nul
md %CD_FILES%\STAKFILES <nul
md %CD_FILES%\TEXTURES <nul
:: copy "Frogger\cd\track01\EuropeWarn.da" "%CD_FILES%\EUROPEWARN.DA"
copy "Frogger\cd\track03\audio64.drv" "%CD_FILES%\AUDIO64.DRV" <nul
:: copy "Frogger\cd\track03\language.txt" "%CD_FILES%\LANGUAGE.TXT" <nul
copy "Frogger\cd\track03\backdrops\artwork00.pvr" "%CD_FILES%\BACKDROPS\ARTWORK00.PVR" <nul
copy "Frogger\cd\track03\backdrops\artwork01.pvr" "%CD_FILES%\BACKDROPS\ARTWORK01.PVR" <nul
copy "Frogger\cd\track03\backdrops\artwork02.pvr" "%CD_FILES%\BACKDROPS\ARTWORK02.PVR" <nul
copy "Frogger\cd\track03\backdrops\artwork03.pvr" "%CD_FILES%\BACKDROPS\ARTWORK03.PVR" <nul
copy "Frogger\cd\track03\backdrops\artwork04.pvr" "%CD_FILES%\BACKDROPS\ARTWORK04.PVR" <nul
copy "Frogger\cd\track03\backdrops\artwork05.pvr" "%CD_FILES%\BACKDROPS\ARTWORK05.PVR" <nul
copy "Frogger\cd\track03\backdrops\artwork06.pvr" "%CD_FILES%\BACKDROPS\ARTWORK06.PVR" <nul
copy "Frogger\cd\track03\backdrops\artwork07.pvr" "%CD_FILES%\BACKDROPS\ARTWORK07.PVR" <nul
copy "Frogger\cd\track03\backdrops\artwork08.pvr" "%CD_FILES%\BACKDROPS\ARTWORK08.PVR" <nul
copy "Frogger\cd\track03\backdrops\artwork09.pvr" "%CD_FILES%\BACKDROPS\ARTWORK09.PVR" <nul
copy "Frogger\cd\track03\backdrops\artwork10.pvr" "%CD_FILES%\BACKDROPS\ARTWORK10.PVR" <nul
copy "Frogger\cd\track03\backdrops\artwork11.pvr" "%CD_FILES%\BACKDROPS\ARTWORK11.PVR" <nul
copy "Frogger\cd\track03\backdrops\artwork12.pvr" "%CD_FILES%\BACKDROPS\ARTWORK12.PVR" <nul
copy "Frogger\cd\track03\backdrops\artwork13.pvr" "%CD_FILES%\BACKDROPS\ARTWORK13.PVR" <nul
copy "Frogger\cd\track03\backdrops\artwork14.pvr" "%CD_FILES%\BACKDROPS\ARTWORK14.PVR" <nul
copy "Frogger\cd\track03\backdrops\artwork15.pvr" "%CD_FILES%\BACKDROPS\ARTWORK15.PVR" <nul
copy "Frogger\cd\track03\backdrops\artwork16.pvr" "%CD_FILES%\BACKDROPS\ARTWORK16.PVR" <nul
copy "Frogger\cd\track03\backdrops\artwork17.pvr" "%CD_FILES%\BACKDROPS\ARTWORK17.PVR" <nul
copy "Frogger\cd\track03\backdrops\artwork18.pvr" "%CD_FILES%\BACKDROPS\ARTWORK18.PVR" <nul
copy "Frogger\cd\track03\backdrops\artwork19.pvr" "%CD_FILES%\BACKDROPS\ARTWORK19.PVR" <nul
copy "Frogger\cd\track03\backdrops\artwork20.pvr" "%CD_FILES%\BACKDROPS\ARTWORK20.PVR" <nul
copy "Frogger\cd\track03\backdrops\artwork21.pvr" "%CD_FILES%\BACKDROPS\ARTWORK21.PVR" <nul
copy "Frogger\cd\track03\backdrops\fr2legal.pvr" "%CD_FILES%\BACKDROPS\FR2LEGAL.PVR" <nul
copy "Frogger\cd\track03\backdrops\fr2legalEU.pvr" "%CD_FILES%\BACKDROPS\FR2LEGALEU.PVR" <nul
copy "Frogger\cd\track03\backdrops\loading.pvr" "%CD_FILES%\BACKDROPS\LOADING.PVR" <nul
:: copy "Frogger\cd\track03\backdrops\loadingeu.pvr" "%CD_FILES%\BACKDROPS\LOADINGEU.PVR" <nul
copy "Frogger\cd\track03\backdrops\loadingus.pvr" "%CD_FILES%\BACKDROPS\LOADINGUS.PVR" <nul
copy "Frogger\cd\track03\backdrops\sofdec.pvr" "%CD_FILES%\BACKDROPS\SOFDEC.PVR" <nul
copy "Frogger\cd\track03\demos\record-0-0.key" "%CD_FILES%\DEMOS\RECORD-0-0.KEY" <nul
copy "Frogger\cd\track03\demos\record-1-0.key" "%CD_FILES%\DEMOS\RECORD-1-0.KEY" <nul
copy "Frogger\cd\track03\demos\record-6-1.key" "%CD_FILES%\DEMOS\RECORD-6-1.KEY" <nul
copy "Frogger\cd\track03\fmv\02s.sfd" "%CD_FILES%\FMV\02S.SFD" <nul
copy "Frogger\cd\track03\fmv\03s.sfd" "%CD_FILES%\FMV\03S.SFD" <nul
copy "Frogger\cd\track03\fmv\04s.sfd" "%CD_FILES%\FMV\04S.SFD" <nul
copy "Frogger\cd\track03\fmv\05s.sfd" "%CD_FILES%\FMV\05S.SFD" <nul
copy "Frogger\cd\track03\fmv\06s.sfd" "%CD_FILES%\FMV\06S.SFD" <nul
copy "Frogger\cd\track03\fmv\07s.sfd" "%CD_FILES%\FMV\07S.SFD" <nul
copy "Frogger\cd\track03\fmv\08s.sfd" "%CD_FILES%\FMV\08S.SFD" <nul
copy "Frogger\cd\track03\fmv\09s.sfd" "%CD_FILES%\FMV\09S.SFD" <nul
copy "Frogger\cd\track03\fmv\10s.sfd" "%CD_FILES%\FMV\10S.SFD" <nul
copy "Frogger\cd\track03\fmv\11s.sfd" "%CD_FILES%\FMV\11S.SFD" <nul
copy "Frogger\cd\track03\fmv\12s.sfd" "%CD_FILES%\FMV\12S.SFD" <nul
copy "Frogger\cd\track03\fmv\ataris.sfd" "%CD_FILES%\FMV\ATARIS.SFD" <nul
copy "Frogger\cd\track03\fmv\blitzs.sfd" "%CD_FILES%\FMV\BLITZS.SFD" <nul
copy "Frogger\cd\track03\fmv\HLogo.sfd" "%CD_FILES%\FMV\HLOGO.SFD" <nul
copy "Frogger\cd\track03\fmv\intro.sfd" "%CD_FILES%\FMV\INTRO.SFD" <nul
copy "Frogger\cd\track03\Fonts\font.pvr" "%CD_FILES%\FONTS\FONT.PVR" <nul
copy "Frogger\cd\track03\Fonts\smallfont.pvr" "%CD_FILES%\FONTS\SMALLFONT.PVR" <nul
copy "Frogger\cd\track03\incbin\acostab.bin" "%CD_FILES%\INCBIN\ACOSTAB.BIN" <nul
copy "Frogger\cd\track03\incbin\Sqrtable.bin" "%CD_FILES%\INCBIN\SQRTABLE.BIN" <nul
copy "Frogger\cd\track03\music\track01.adx" "%CD_FILES%\MUSIC\TRACK01.ADX" <nul
copy "Frogger\cd\track03\music\track02.adx" "%CD_FILES%\MUSIC\TRACK02.ADX" <nul
copy "Frogger\cd\track03\music\track03.adx" "%CD_FILES%\MUSIC\TRACK03.ADX" <nul
copy "Frogger\cd\track03\music\track04.adx" "%CD_FILES%\MUSIC\TRACK04.ADX" <nul
copy "Frogger\cd\track03\music\track05.adx" "%CD_FILES%\MUSIC\TRACK05.ADX" <nul
copy "Frogger\cd\track03\music\track06.adx" "%CD_FILES%\MUSIC\TRACK06.ADX" <nul
copy "Frogger\cd\track03\music\track07.adx" "%CD_FILES%\MUSIC\TRACK07.ADX" <nul
copy "Frogger\cd\track03\music\track08.adx" "%CD_FILES%\MUSIC\TRACK08.ADX" <nul
copy "Frogger\cd\track03\music\track09.adx" "%CD_FILES%\MUSIC\TRACK09.ADX" <nul
copy "Frogger\cd\track03\music\track10.adx" "%CD_FILES%\MUSIC\TRACK10.ADX" <nul
copy "Frogger\cd\track03\music\track11.adx" "%CD_FILES%\MUSIC\TRACK11.ADX" <nul
copy "Frogger\cd\track03\sfx\ancients.kat" "%CD_FILES%\SFX\ANCIENTS.KAT" <nul
copy "Frogger\cd\track03\sfx\ancients.txt" "%CD_FILES%\SFX\ANCIENTS.TXT" <nul
copy "Frogger\cd\track03\sfx\frogger.kat" "%CD_FILES%\SFX\FROGGER.KAT" <nul
copy "Frogger\cd\track03\sfx\frogger.txt" "%CD_FILES%\SFX\FROGGER.TXT" <nul
copy "Frogger\cd\track03\sfx\FRONTEND.kat" "%CD_FILES%\SFX\FRONTEND.KAT" <nul
copy "Frogger\cd\track03\sfx\frontend.txt" "%CD_FILES%\SFX\FRONTEND.TXT" <nul
copy "Frogger\cd\track03\sfx\garden.kat" "%CD_FILES%\SFX\GARDEN.KAT" <nul
copy "Frogger\cd\track03\sfx\garden.txt" "%CD_FILES%\SFX\GARDEN.TXT" <nul
copy "Frogger\cd\track03\sfx\halloween.kat" "%CD_FILES%\SFX\HALLOWEEN.KAT" <nul
copy "Frogger\cd\track03\sfx\halloween.txt" "%CD_FILES%\SFX\HALLOWEEN.TXT" <nul
copy "Frogger\cd\track03\sfx\lab.kat" "%CD_FILES%\SFX\LAB.KAT" <nul
copy "Frogger\cd\track03\sfx\lab.txt" "%CD_FILES%\SFX\LAB.TXT" <nul
copy "Frogger\cd\track03\sfx\lillie.kat" "%CD_FILES%\SFX\LILLIE.KAT" <nul
copy "Frogger\cd\track03\sfx\lillie.txt" "%CD_FILES%\SFX\LILLIE.TXT" <nul
copy "Frogger\cd\track03\sfx\multi.kat" "%CD_FILES%\SFX\MULTI.KAT" <nul
copy "Frogger\cd\track03\sfx\multi.txt" "%CD_FILES%\SFX\MULTI.TXT" <nul
copy "Frogger\cd\track03\sfx\roobie.kat" "%CD_FILES%\SFX\ROOBIE.KAT" <nul
copy "Frogger\cd\track03\sfx\roobie.txt" "%CD_FILES%\SFX\ROOBIE.TXT" <nul
copy "Frogger\cd\track03\sfx\space.kat" "%CD_FILES%\SFX\SPACE.KAT" <nul
copy "Frogger\cd\track03\sfx\space.txt" "%CD_FILES%\SFX\SPACE.TXT" <nul
copy "Frogger\cd\track03\sfx\sub.kat" "%CD_FILES%\SFX\SUB.KAT" <nul
copy "Frogger\cd\track03\sfx\sub.txt" "%CD_FILES%\SFX\SUB.TXT" <nul
copy "Frogger\cd\track03\sfx\superretro.kat" "%CD_FILES%\SFX\SUPERRETRO.KAT" <nul
copy "Frogger\cd\track03\sfx\superretro.txt" "%CD_FILES%\SFX\SUPERRETRO.TXT" <nul
copy "Frogger\cd\track03\sfx\swampy.kat" "%CD_FILES%\SFX\SWAMPY.KAT" <nul
copy "Frogger\cd\track03\sfx\swampy.txt" "%CD_FILES%\SFX\SWAMPY.TXT" <nul
copy "Frogger\cd\track03\sfx\tad.kat" "%CD_FILES%\SFX\TAD.KAT" <nul
copy "Frogger\cd\track03\sfx\tad.txt" "%CD_FILES%\SFX\TAD.TXT" <nul
copy "Frogger\cd\track03\sfx\tank.kat" "%CD_FILES%\SFX\TANK.KAT" <nul
copy "Frogger\cd\track03\sfx\tank.txt" "%CD_FILES%\SFX\TANK.TXT" <nul
copy "Frogger\cd\track03\sfx\treeby.kat" "%CD_FILES%\SFX\TREEBY.KAT" <nul
copy "Frogger\cd\track03\sfx\treeby.txt" "%CD_FILES%\SFX\TREEBY.TXT" <nul
copy "Frogger\cd\track03\sfx\wart.kat" "%CD_FILES%\SFX\WART.KAT" <nul
copy "Frogger\cd\track03\sfx\wart.txt" "%CD_FILES%\SFX\WART.TXT" <nul
copy "Frogger\cd\track03\StakFiles\ancients1.stk" "%CD_FILES%\STAKFILES\ANCIENTS1.STK" <nul
copy "Frogger\cd\track03\StakFiles\ancients2.stk" "%CD_FILES%\STAKFILES\ANCIENTS2.STK" <nul
copy "Frogger\cd\track03\StakFiles\ancients3.stk" "%CD_FILES%\STAKFILES\ANCIENTS3.STK" <nul
copy "Frogger\cd\track03\StakFiles\ancientsmulti.stk" "%CD_FILES%\STAKFILES\ANCIENTSMULTI.STK" <nul
copy "Frogger\cd\track03\StakFiles\citymulti.stk" "%CD_FILES%\STAKFILES\CITYMULTI.STK" <nul
copy "Frogger\cd\track03\StakFiles\frogger.stk" "%CD_FILES%\STAKFILES\FROGGER.STK" <nul
copy "Frogger\cd\track03\StakFiles\frontend.stk" "%CD_FILES%\STAKFILES\FRONTEND.STK" <nul
copy "Frogger\cd\track03\StakFiles\garden1.stk" "%CD_FILES%\STAKFILES\GARDEN1.STK" <nul
copy "Frogger\cd\track03\StakFiles\generic.stk" "%CD_FILES%\STAKFILES\GENERIC.STK" <nul
copy "Frogger\cd\track03\StakFiles\halloween1.stk" "%CD_FILES%\STAKFILES\HALLOWEEN1.STK" <nul
copy "Frogger\cd\track03\StakFiles\halloween2.stk" "%CD_FILES%\STAKFILES\HALLOWEEN2.STK" <nul
copy "Frogger\cd\track03\StakFiles\halloween3.stk" "%CD_FILES%\STAKFILES\HALLOWEEN3.STK" <nul
copy "Frogger\cd\track03\StakFiles\lab1.stk" "%CD_FILES%\STAKFILES\LAB1.STK" <nul
copy "Frogger\cd\track03\StakFiles\lab2.stk" "%CD_FILES%\STAKFILES\LAB2.STK" <nul
copy "Frogger\cd\track03\StakFiles\lab3.stk" "%CD_FILES%\STAKFILES\LAB3.STK" <nul
copy "Frogger\cd\track03\StakFiles\labmulti1.stk" "%CD_FILES%\STAKFILES\LABMULTI1.STK" <nul
copy "Frogger\cd\track03\StakFiles\labmulti2.stk" "%CD_FILES%\STAKFILES\LABMULTI2.STK" <nul
copy "Frogger\cd\track03\StakFiles\labmulti3.stk" "%CD_FILES%\STAKFILES\LABMULTI3.STK" <nul
copy "Frogger\cd\track03\StakFiles\lillie.stk" "%CD_FILES%\STAKFILES\LILLIE.STK" <nul
copy "Frogger\cd\track03\StakFiles\mfrogger.stk" "%CD_FILES%\STAKFILES\MFROGGER.STK" <nul
copy "Frogger\cd\track03\StakFiles\mlillie.stk" "%CD_FILES%\STAKFILES\MLILLIE.STK" <nul
copy "Frogger\cd\track03\StakFiles\mroobie.stk" "%CD_FILES%\STAKFILES\MROOBIE.STK" <nul
copy "Frogger\cd\track03\StakFiles\mswampy.stk" "%CD_FILES%\STAKFILES\MSWAMPY.STK" <nul
copy "Frogger\cd\track03\StakFiles\mtad.stk" "%CD_FILES%\STAKFILES\MTAD.STK" <nul
copy "Frogger\cd\track03\StakFiles\mtank.stk" "%CD_FILES%\STAKFILES\MTANK.STK" <nul
copy "Frogger\cd\track03\StakFiles\mtoad.stk" "%CD_FILES%\STAKFILES\MTOAD.STK" <nul
copy "Frogger\cd\track03\StakFiles\mtreeby.stk" "%CD_FILES%\STAKFILES\MTREEBY.STK" <nul
copy "Frogger\cd\track03\StakFiles\retro1.stk" "%CD_FILES%\STAKFILES\RETRO1.STK" <nul
copy "Frogger\cd\track03\StakFiles\retro10.stk" "%CD_FILES%\STAKFILES\RETRO10.STK" <nul
copy "Frogger\cd\track03\StakFiles\retro2.stk" "%CD_FILES%\STAKFILES\RETRO2.STK" <nul
copy "Frogger\cd\track03\StakFiles\retro3.stk" "%CD_FILES%\STAKFILES\RETRO3.STK" <nul
copy "Frogger\cd\track03\StakFiles\retro4.stk" "%CD_FILES%\STAKFILES\RETRO4.STK" <nul
copy "Frogger\cd\track03\StakFiles\retro5.stk" "%CD_FILES%\STAKFILES\RETRO5.STK" <nul
copy "Frogger\cd\track03\StakFiles\retro6.stk" "%CD_FILES%\STAKFILES\RETRO6.STK" <nul
copy "Frogger\cd\track03\StakFiles\retro7.stk" "%CD_FILES%\STAKFILES\RETRO7.STK" <nul
copy "Frogger\cd\track03\StakFiles\retro8.stk" "%CD_FILES%\STAKFILES\RETRO8.STK" <nul
copy "Frogger\cd\track03\StakFiles\retro9.stk" "%CD_FILES%\STAKFILES\RETRO9.STK" <nul
copy "Frogger\cd\track03\StakFiles\retromulti1.stk" "%CD_FILES%\STAKFILES\RETROMULTI1.STK" <nul
copy "Frogger\cd\track03\StakFiles\retromulti2.stk" "%CD_FILES%\STAKFILES\RETROMULTI2.STK" <nul
copy "Frogger\cd\track03\StakFiles\retromulti3.stk" "%CD_FILES%\STAKFILES\RETROMULTI3.STK" <nul
copy "Frogger\cd\track03\StakFiles\retromulti4.stk" "%CD_FILES%\STAKFILES\RETROMULTI4.STK" <nul
copy "Frogger\cd\track03\StakFiles\roobie.stk" "%CD_FILES%\STAKFILES\ROOBIE.STK" <nul
copy "Frogger\cd\track03\StakFiles\space1.stk" "%CD_FILES%\STAKFILES\SPACE1.STK" <nul
copy "Frogger\cd\track03\StakFiles\space2.stk" "%CD_FILES%\STAKFILES\SPACE2.STK" <nul
copy "Frogger\cd\track03\StakFiles\space3.stk" "%CD_FILES%\STAKFILES\SPACE3.STK" <nul
copy "Frogger\cd\track03\StakFiles\sub1.stk" "%CD_FILES%\STAKFILES\SUB1.STK" <nul
copy "Frogger\cd\track03\StakFiles\sub2.stk" "%CD_FILES%\STAKFILES\SUB2.STK" <nul
copy "Frogger\cd\track03\StakFiles\sub3.stk" "%CD_FILES%\STAKFILES\SUB3.STK" <nul
copy "Frogger\cd\track03\StakFiles\super1.stk" "%CD_FILES%\STAKFILES\SUPER1.STK" <nul
copy "Frogger\cd\track03\StakFiles\swampy.stk" "%CD_FILES%\STAKFILES\SWAMPY.STK" <nul
copy "Frogger\cd\track03\StakFiles\tad.stk" "%CD_FILES%\STAKFILES\TAD.STK" <nul
copy "Frogger\cd\track03\StakFiles\tank.stk" "%CD_FILES%\STAKFILES\TANK.STK" <nul
copy "Frogger\cd\track03\StakFiles\toad.stk" "%CD_FILES%\STAKFILES\TOAD.STK" <nul
copy "Frogger\cd\track03\StakFiles\training.stk" "%CD_FILES%\STAKFILES\TRAINING.STK" <nul
copy "Frogger\cd\track03\StakFiles\treeby.stk" "%CD_FILES%\STAKFILES\TREEBY.STK" <nul
copy "Frogger\cd\track03\Textures\ancients.spt" "%CD_FILES%\TEXTURES\ANCIENTS.SPT" <nul
copy "Frogger\cd\track03\Textures\city.spt" "%CD_FILES%\TEXTURES\CITY.SPT" <nul
copy "Frogger\cd\track03\Textures\Frogger2.pvr" "%CD_FILES%\TEXTURES\FROGGER2.PVR" <nul
copy "Frogger\cd\track03\Textures\garden.spt" "%CD_FILES%\TEXTURES\GARDEN.SPT" <nul
copy "Frogger\cd\track03\Textures\garib.spt" "%CD_FILES%\TEXTURES\GARIB.SPT" <nul
copy "Frogger\cd\track03\Textures\generic.spt" "%CD_FILES%\TEXTURES\GENERIC.SPT" <nul
copy "Frogger\cd\track03\Textures\halloween.spt" "%CD_FILES%\TEXTURES\HALLOWEEN.SPT" <nul
copy "Frogger\cd\track03\Textures\hub.spt" "%CD_FILES%\TEXTURES\HUB.SPT" <nul
copy "Frogger\cd\track03\Textures\lab.spt" "%CD_FILES%\TEXTURES\LAB.SPT" <nul
copy "Frogger\cd\track03\Textures\language.spt" "%CD_FILES%\TEXTURES\LANGUAGE.SPT" <nul
copy "Frogger\cd\track03\Textures\Newbaby.spt" "%CD_FILES%\TEXTURES\NEWBABY.SPT" <nul
copy "Frogger\cd\track03\Textures\retro.spt" "%CD_FILES%\TEXTURES\RETRO.SPT" <nul
copy "Frogger\cd\track03\Textures\space.spt" "%CD_FILES%\TEXTURES\SPACE.SPT" <nul
copy "Frogger\cd\track03\Textures\sub.spt" "%CD_FILES%\TEXTURES\SUB.SPT" <nul
copy "Frogger\cd\track03\Textures\super.spt" "%CD_FILES%\TEXTURES\SUPER.SPT" <nul
copy "Frogger\cd\track03\Textures\titles.spt" "%CD_FILES%\TEXTURES\TITLES.SPT" <nul

echo CD files directory has been setup.
exit /b

:error
echo Failed to get region name.
PAUSE
goto :EOF