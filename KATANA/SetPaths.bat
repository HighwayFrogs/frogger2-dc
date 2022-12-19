@echo off

rem This batch file prepares a DOS shell for use with Dreamcast compiler tools.

rem Uncomment to allow easier command line text control
doskey /insert

echo %0: Setting environment variables..
echo.
echo ** HITACHI COMPILER **

rem Set the location of the Katana installation.
rem Default: C:\Katana

if not !%KATANA_ROOT%==%KATANA_ROOT%! goto katanarootalreadyset
SET KATANA_ROOT=F:\KATANA
:katanarootalreadyset

rem if !%KATANA_ROOT%==%KATANA_ROOT%! goto error_nokatanaroot
rem Check for the existence of a compatible shell.  At this time, only
rem Microsoft-designed COMMAND.COM is acceptable.

ver | find "Windows"
if errorlevel 1 goto error_notcompatibleshell

rem Add Gnu Make and Hitachi compiler tools to the current path.
SET PATH=%PATH%;%KATANA_ROOT%\Utl\Dev\Make;%KATANA_ROOT%\Utl\Dev\Hitachi;%KATANA_ROOT%\Tools

rem Verify the existence of Gnu Make.

gmake -v > chk_2397.tmp
if not exist chk_2397.tmp goto error_nonmake
del chk_2397.tmp

rem Set include and library directories used by Hitachi compiler tools.
rem Edit these lines to change the default installation.

SET SHC_LIB=%KATANA_ROOT%\Utl\Dev\Hitachi
SET SHC_INC=%KATANA_ROOT%\Include\Shc
SET SHCPP_LIB=%SHC_LIB%
SET SHCPP_INC=%SHC_INC%

rem Verify Hitachi paths.

if !%SHC_LIB%==%SHC_LIB%! goto error_toolpathsnotset
if !%SHC_INC%==%SHC_INC%! goto error_toolpathsnotset
if !%SHCPP_LIB%==%SHCPP_LIB%! goto error_toolpathsnotset
if !%SHCPP_INC%==%SHCPP_INC%! goto error_toolpathsnotset

if not exist %SHC_LIB%\nul goto error_toolpathsnotset
if not exist %SHC_INC%\nul goto error_toolpathsnotset
if not exist %SHCPP_LIB%\nul goto error_toolpathsnotset
if not exist %SHCPP_INC%\nul goto error_toolpathsnotset

rem Set location of temporary files created during compilation.
rem These files (.tmp) may be left behind when a compile is interrupted.
rem Default:  SET SHC_TMP=C:\Windows\Temp

SET SHC_TMP=.
SET SHCPP_TMP=%SHC_TMP%

shc > chk_2397.tmp
if not exist chk_2397.tmp goto error_shccmd
del chk_2397.tmp

goto success

rem -------------------------------------------------------------------
rem ! Error handling
rem -------------------------------------------------------------------

:error_nokatanaroot
echo Error: The path specified by the KATANA_ROOT variable does not
echo exist.  Please edit SETPATHS.BAT.
goto done

:error_notcompatibleshell
echo Error: The command shell is not compatible with SetPaths.
echo Please make COMMAND.COM your default shell and try again.
goto done

:error_nonmake
echo Error: Gnu MAKE could not be found on the default path.  Please
echo edit SETPATHS.BAT.
goto done

:error_shccmd
echo Error: The Hitachi command line tools could not be found.
echo Please edit SETPATHS.BAT.
goto done

:error_toolpathsnotset
echo Error: Environmental variables required by the SH-C compiler could
echo not be determined, or do not point to valid directories.  Please
echo edit SETPATHS.BAT or install SHC Compiler from the SDK.
goto done

:success
echo.
:done

