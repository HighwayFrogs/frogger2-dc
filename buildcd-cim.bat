del /S OUTPUT\* /Q
if not exist OUTPUT md OUTPUT

if not exist "Frogger\C Application\exe\1ST_READ.BIN" (
    echo Frogger 2 must be compiled before it can be built into a CD.
    PAUSE
    goto :EOF
)

REM Copy ELF to track05.
copy "Frogger\C Application\exe\1ST_READ.BIN" Frogger\cd\track05\

subst M: Frogger\cd\

REM Build image.
KATANA\Utl\Dev\CDCraft\CRFGDC.EXE -bld=Frogger\cd\frogger2.scr,dsk

REM Move to output folder.
move FROGGER2.CIM OUTPUT\
KATANA\Utl\Dev\CDCraft\CRFGDC.EXE -tck=OUTPUT\FROGGER2.CIM > OUTPUT\frogger2_toc.txt

REM Cleanup
subst M: /D

REM Convert CIM to GDI
REM I couldn't make this work yet.
REM md OUTPUT\GDI

echo Build Complete
PAUSE
goto :EOF
