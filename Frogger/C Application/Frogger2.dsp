# Microsoft Developer Studio Project File - Name="Frogger2" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=Frogger2 - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "Frogger2.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "Frogger2.mak" CFG="Frogger2 - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "Frogger2 - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "Frogger2 - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "Frogger2 - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "obj"
# PROP Intermediate_Dir "obj"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /YX /FD /c
# ADD CPP /nologo /W3 /GX /O2 /D "NDEBUG" /D "PSX_VERSION" /D "DREAMCAST_VERSION" /D "__SHC__" /D "__SET4__" /D "__SET5__" /D "__KATANA__" /D "_KAMUI2_" /YX /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /machine:I386

!ELSEIF  "$(CFG)" == "Frogger2 - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "obj"
# PROP Intermediate_Dir "obj"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /YX /FD /GZ /c
# ADD CPP /nologo /W3 /Gm /GX /ZI /Od /D "_DEBUG" /D "PSX_VERSION" /D "DREAMCAST_VERSION" /D "__SHC__" /D "__SET4__" /D "__SET5__" /D "__KATANA__" /D "_KAMUI2_" /YX /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept

!ENDIF 

# Begin Target

# Name "Frogger2 - Win32 Release"
# Name "Frogger2 - Win32 Debug"
# Begin Group "Common Files"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\common\actor2.c

!IF  "$(CFG)" == "Frogger2 - Win32 Release"

!ELSEIF  "$(CFG)" == "Frogger2 - Win32 Debug"

# PROP Ignore_Default_Tool 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\common\actor2.h

!IF  "$(CFG)" == "Frogger2 - Win32 Release"

!ELSEIF  "$(CFG)" == "Frogger2 - Win32 Debug"

# PROP Ignore_Default_Tool 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\common\anim.c

!IF  "$(CFG)" == "Frogger2 - Win32 Release"

!ELSEIF  "$(CFG)" == "Frogger2 - Win32 Debug"

# PROP Ignore_Default_Tool 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\common\anim.h

!IF  "$(CFG)" == "Frogger2 - Win32 Release"

!ELSEIF  "$(CFG)" == "Frogger2 - Win32 Debug"

# PROP Ignore_Default_Tool 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\common\babyfrog.c

!IF  "$(CFG)" == "Frogger2 - Win32 Release"

!ELSEIF  "$(CFG)" == "Frogger2 - Win32 Debug"

# PROP Ignore_Default_Tool 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\common\babyfrog.h

!IF  "$(CFG)" == "Frogger2 - Win32 Release"

!ELSEIF  "$(CFG)" == "Frogger2 - Win32 Debug"

# PROP Ignore_Default_Tool 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\common\bbtimer.c

!IF  "$(CFG)" == "Frogger2 - Win32 Release"

!ELSEIF  "$(CFG)" == "Frogger2 - Win32 Debug"

# PROP Ignore_Default_Tool 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\common\bbtimer.h

!IF  "$(CFG)" == "Frogger2 - Win32 Release"

!ELSEIF  "$(CFG)" == "Frogger2 - Win32 Debug"

# PROP Ignore_Default_Tool 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\common\block.c

!IF  "$(CFG)" == "Frogger2 - Win32 Release"

!ELSEIF  "$(CFG)" == "Frogger2 - Win32 Debug"

# PROP Ignore_Default_Tool 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\common\block.h

!IF  "$(CFG)" == "Frogger2 - Win32 Release"

!ELSEIF  "$(CFG)" == "Frogger2 - Win32 Debug"

# PROP Ignore_Default_Tool 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\common\cam.c

!IF  "$(CFG)" == "Frogger2 - Win32 Release"

!ELSEIF  "$(CFG)" == "Frogger2 - Win32 Debug"

# PROP Ignore_Default_Tool 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\common\Cam.h

!IF  "$(CFG)" == "Frogger2 - Win32 Release"

!ELSEIF  "$(CFG)" == "Frogger2 - Win32 Debug"

# PROP Ignore_Default_Tool 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\common\codes.h

!IF  "$(CFG)" == "Frogger2 - Win32 Release"

!ELSEIF  "$(CFG)" == "Frogger2 - Win32 Debug"

# PROP Ignore_Default_Tool 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\common\collect.c

!IF  "$(CFG)" == "Frogger2 - Win32 Release"

!ELSEIF  "$(CFG)" == "Frogger2 - Win32 Debug"

# PROP Ignore_Default_Tool 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\common\collect.h

!IF  "$(CFG)" == "Frogger2 - Win32 Release"

!ELSEIF  "$(CFG)" == "Frogger2 - Win32 Debug"

# PROP Ignore_Default_Tool 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\common\crc32.c

!IF  "$(CFG)" == "Frogger2 - Win32 Release"

!ELSEIF  "$(CFG)" == "Frogger2 - Win32 Debug"

# PROP Ignore_Default_Tool 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\common\crc32.h

!IF  "$(CFG)" == "Frogger2 - Win32 Release"

!ELSEIF  "$(CFG)" == "Frogger2 - Win32 Debug"

# PROP Ignore_Default_Tool 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\common\decomp.h

!IF  "$(CFG)" == "Frogger2 - Win32 Release"

!ELSEIF  "$(CFG)" == "Frogger2 - Win32 Debug"

# PROP Ignore_Default_Tool 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\common\DefData.c

!IF  "$(CFG)" == "Frogger2 - Win32 Release"

!ELSEIF  "$(CFG)" == "Frogger2 - Win32 Debug"

# PROP Ignore_Default_Tool 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\common\DefData.h

!IF  "$(CFG)" == "Frogger2 - Win32 Release"

!ELSEIF  "$(CFG)" == "Frogger2 - Win32 Debug"

# PROP Ignore_Default_Tool 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\common\define.h

!IF  "$(CFG)" == "Frogger2 - Win32 Release"

!ELSEIF  "$(CFG)" == "Frogger2 - Win32 Debug"

# PROP Ignore_Default_Tool 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\common\enemies.c

!IF  "$(CFG)" == "Frogger2 - Win32 Release"

!ELSEIF  "$(CFG)" == "Frogger2 - Win32 Debug"

# PROP Ignore_Default_Tool 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\common\enemies.h

!IF  "$(CFG)" == "Frogger2 - Win32 Release"

!ELSEIF  "$(CFG)" == "Frogger2 - Win32 Debug"

# PROP Ignore_Default_Tool 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\common\evenfunc.c

!IF  "$(CFG)" == "Frogger2 - Win32 Release"

!ELSEIF  "$(CFG)" == "Frogger2 - Win32 Debug"

# PROP Ignore_Default_Tool 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\common\evenfunc.h

!IF  "$(CFG)" == "Frogger2 - Win32 Release"

!ELSEIF  "$(CFG)" == "Frogger2 - Win32 Debug"

# PROP Ignore_Default_Tool 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\common\event.c

!IF  "$(CFG)" == "Frogger2 - Win32 Release"

!ELSEIF  "$(CFG)" == "Frogger2 - Win32 Debug"

# PROP Ignore_Default_Tool 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\common\event.h

!IF  "$(CFG)" == "Frogger2 - Win32 Release"

!ELSEIF  "$(CFG)" == "Frogger2 - Win32 Debug"

# PROP Ignore_Default_Tool 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\common\fixed.c

!IF  "$(CFG)" == "Frogger2 - Win32 Release"

!ELSEIF  "$(CFG)" == "Frogger2 - Win32 Debug"

# PROP Ignore_Default_Tool 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\common\fixed.h

!IF  "$(CFG)" == "Frogger2 - Win32 Release"

!ELSEIF  "$(CFG)" == "Frogger2 - Win32 Debug"

# PROP Ignore_Default_Tool 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\common\flatpack.c

!IF  "$(CFG)" == "Frogger2 - Win32 Release"

!ELSEIF  "$(CFG)" == "Frogger2 - Win32 Debug"

# PROP Ignore_Default_Tool 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\common\flatpack.h

!IF  "$(CFG)" == "Frogger2 - Win32 Release"

!ELSEIF  "$(CFG)" == "Frogger2 - Win32 Debug"

# PROP Ignore_Default_Tool 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\common\frogger.c

!IF  "$(CFG)" == "Frogger2 - Win32 Release"

!ELSEIF  "$(CFG)" == "Frogger2 - Win32 Debug"

# PROP Ignore_Default_Tool 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\common\frogger.h

!IF  "$(CFG)" == "Frogger2 - Win32 Release"

!ELSEIF  "$(CFG)" == "Frogger2 - Win32 Debug"

# PROP Ignore_Default_Tool 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\common\frogmove.c

!IF  "$(CFG)" == "Frogger2 - Win32 Release"

!ELSEIF  "$(CFG)" == "Frogger2 - Win32 Debug"

# PROP Ignore_Default_Tool 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\common\frogmove.h

!IF  "$(CFG)" == "Frogger2 - Win32 Release"

!ELSEIF  "$(CFG)" == "Frogger2 - Win32 Debug"

# PROP Ignore_Default_Tool 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\common\frontend.c

!IF  "$(CFG)" == "Frogger2 - Win32 Release"

!ELSEIF  "$(CFG)" == "Frogger2 - Win32 Debug"

# PROP Ignore_Default_Tool 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\common\frontend.h

!IF  "$(CFG)" == "Frogger2 - Win32 Release"

!ELSEIF  "$(CFG)" == "Frogger2 - Win32 Debug"

# PROP Ignore_Default_Tool 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\common\game.c

!IF  "$(CFG)" == "Frogger2 - Win32 Release"

!ELSEIF  "$(CFG)" == "Frogger2 - Win32 Debug"

# PROP Ignore_Default_Tool 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\common\game.h

!IF  "$(CFG)" == "Frogger2 - Win32 Release"

!ELSEIF  "$(CFG)" == "Frogger2 - Win32 Debug"

# PROP Ignore_Default_Tool 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\common\general.c

!IF  "$(CFG)" == "Frogger2 - Win32 Release"

!ELSEIF  "$(CFG)" == "Frogger2 - Win32 Debug"

# PROP Ignore_Default_Tool 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\common\general.h

!IF  "$(CFG)" == "Frogger2 - Win32 Release"

!ELSEIF  "$(CFG)" == "Frogger2 - Win32 Debug"

# PROP Ignore_Default_Tool 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\common\hud.c

!IF  "$(CFG)" == "Frogger2 - Win32 Release"

!ELSEIF  "$(CFG)" == "Frogger2 - Win32 Debug"

# PROP Ignore_Default_Tool 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\common\hud.h

!IF  "$(CFG)" == "Frogger2 - Win32 Release"

!ELSEIF  "$(CFG)" == "Frogger2 - Win32 Debug"

# PROP Ignore_Default_Tool 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\common\lang.h

!IF  "$(CFG)" == "Frogger2 - Win32 Release"

!ELSEIF  "$(CFG)" == "Frogger2 - Win32 Debug"

# PROP Ignore_Default_Tool 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\common\language.h

!IF  "$(CFG)" == "Frogger2 - Win32 Release"

!ELSEIF  "$(CFG)" == "Frogger2 - Win32 Debug"

# PROP Ignore_Default_Tool 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\common\layout.c

!IF  "$(CFG)" == "Frogger2 - Win32 Release"

!ELSEIF  "$(CFG)" == "Frogger2 - Win32 Debug"

# PROP Ignore_Default_Tool 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\common\layout.h

!IF  "$(CFG)" == "Frogger2 - Win32 Release"

!ELSEIF  "$(CFG)" == "Frogger2 - Win32 Debug"

# PROP Ignore_Default_Tool 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\common\levplay.c

!IF  "$(CFG)" == "Frogger2 - Win32 Release"

!ELSEIF  "$(CFG)" == "Frogger2 - Win32 Debug"

# PROP Ignore_Default_Tool 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\common\levplay.h

!IF  "$(CFG)" == "Frogger2 - Win32 Release"

!ELSEIF  "$(CFG)" == "Frogger2 - Win32 Debug"

# PROP Ignore_Default_Tool 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\common\libgs.h

!IF  "$(CFG)" == "Frogger2 - Win32 Release"

!ELSEIF  "$(CFG)" == "Frogger2 - Win32 Debug"

# PROP Ignore_Default_Tool 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\common\libgte.h

!IF  "$(CFG)" == "Frogger2 - Win32 Release"

!ELSEIF  "$(CFG)" == "Frogger2 - Win32 Debug"

# PROP Ignore_Default_Tool 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\common\map.c

!IF  "$(CFG)" == "Frogger2 - Win32 Release"

!ELSEIF  "$(CFG)" == "Frogger2 - Win32 Debug"

# PROP Ignore_Default_Tool 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\common\map.h

!IF  "$(CFG)" == "Frogger2 - Win32 Release"

!ELSEIF  "$(CFG)" == "Frogger2 - Win32 Debug"

# PROP Ignore_Default_Tool 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\common\maths.c

!IF  "$(CFG)" == "Frogger2 - Win32 Release"

!ELSEIF  "$(CFG)" == "Frogger2 - Win32 Debug"

# PROP Ignore_Default_Tool 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\common\maths.h

!IF  "$(CFG)" == "Frogger2 - Win32 Release"

!ELSEIF  "$(CFG)" == "Frogger2 - Win32 Debug"

# PROP Ignore_Default_Tool 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\common\memload.c

!IF  "$(CFG)" == "Frogger2 - Win32 Release"

!ELSEIF  "$(CFG)" == "Frogger2 - Win32 Debug"

# PROP Ignore_Default_Tool 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\common\memload.h

!IF  "$(CFG)" == "Frogger2 - Win32 Release"

!ELSEIF  "$(CFG)" == "Frogger2 - Win32 Debug"

# PROP Ignore_Default_Tool 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\common\menus.c

!IF  "$(CFG)" == "Frogger2 - Win32 Release"

!ELSEIF  "$(CFG)" == "Frogger2 - Win32 Debug"

# PROP Ignore_Default_Tool 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\common\menus.h

!IF  "$(CFG)" == "Frogger2 - Win32 Release"

!ELSEIF  "$(CFG)" == "Frogger2 - Win32 Debug"

# PROP Ignore_Default_Tool 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\common\multi.c

!IF  "$(CFG)" == "Frogger2 - Win32 Release"

!ELSEIF  "$(CFG)" == "Frogger2 - Win32 Debug"

# PROP Ignore_Default_Tool 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\common\multi.h

!IF  "$(CFG)" == "Frogger2 - Win32 Release"

!ELSEIF  "$(CFG)" == "Frogger2 - Win32 Debug"

# PROP Ignore_Default_Tool 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\common\newpsx.c

!IF  "$(CFG)" == "Frogger2 - Win32 Release"

!ELSEIF  "$(CFG)" == "Frogger2 - Win32 Debug"

# PROP Ignore_Default_Tool 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\common\newpsx.h

!IF  "$(CFG)" == "Frogger2 - Win32 Release"

!ELSEIF  "$(CFG)" == "Frogger2 - Win32 Debug"

# PROP Ignore_Default_Tool 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\common\options.c

!IF  "$(CFG)" == "Frogger2 - Win32 Release"

!ELSEIF  "$(CFG)" == "Frogger2 - Win32 Debug"

# PROP Ignore_Default_Tool 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\common\options.h

!IF  "$(CFG)" == "Frogger2 - Win32 Release"

!ELSEIF  "$(CFG)" == "Frogger2 - Win32 Debug"

# PROP Ignore_Default_Tool 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\common\overlays.c

!IF  "$(CFG)" == "Frogger2 - Win32 Release"

!ELSEIF  "$(CFG)" == "Frogger2 - Win32 Debug"

# PROP Ignore_Default_Tool 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\common\overlays.h

!IF  "$(CFG)" == "Frogger2 - Win32 Release"

!ELSEIF  "$(CFG)" == "Frogger2 - Win32 Debug"

# PROP Ignore_Default_Tool 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\common\particle.c

!IF  "$(CFG)" == "Frogger2 - Win32 Release"

!ELSEIF  "$(CFG)" == "Frogger2 - Win32 Debug"

# PROP Ignore_Default_Tool 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\common\particle.h

!IF  "$(CFG)" == "Frogger2 - Win32 Release"

!ELSEIF  "$(CFG)" == "Frogger2 - Win32 Debug"

# PROP Ignore_Default_Tool 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\common\path.c

!IF  "$(CFG)" == "Frogger2 - Win32 Release"

!ELSEIF  "$(CFG)" == "Frogger2 - Win32 Debug"

# PROP Ignore_Default_Tool 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\common\path.h

!IF  "$(CFG)" == "Frogger2 - Win32 Release"

!ELSEIF  "$(CFG)" == "Frogger2 - Win32 Debug"

# PROP Ignore_Default_Tool 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\common\platform.c

!IF  "$(CFG)" == "Frogger2 - Win32 Release"

!ELSEIF  "$(CFG)" == "Frogger2 - Win32 Debug"

# PROP Ignore_Default_Tool 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\common\platform.h

!IF  "$(CFG)" == "Frogger2 - Win32 Release"

!ELSEIF  "$(CFG)" == "Frogger2 - Win32 Debug"

# PROP Ignore_Default_Tool 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\common\prefix_dc.h

!IF  "$(CFG)" == "Frogger2 - Win32 Release"

!ELSEIF  "$(CFG)" == "Frogger2 - Win32 Debug"

# PROP Ignore_Default_Tool 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\common\ptexture.c

!IF  "$(CFG)" == "Frogger2 - Win32 Release"

!ELSEIF  "$(CFG)" == "Frogger2 - Win32 Debug"

# PROP Ignore_Default_Tool 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\common\ptexture.h

!IF  "$(CFG)" == "Frogger2 - Win32 Release"

!ELSEIF  "$(CFG)" == "Frogger2 - Win32 Debug"

# PROP Ignore_Default_Tool 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\common\savegame.c
# End Source File
# Begin Source File

SOURCE=.\common\savegame.h
# End Source File
# Begin Source File

SOURCE=.\common\script.c

!IF  "$(CFG)" == "Frogger2 - Win32 Release"

!ELSEIF  "$(CFG)" == "Frogger2 - Win32 Debug"

# PROP Ignore_Default_Tool 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\common\script.h

!IF  "$(CFG)" == "Frogger2 - Win32 Release"

!ELSEIF  "$(CFG)" == "Frogger2 - Win32 Debug"

# PROP Ignore_Default_Tool 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\common\sndview.c

!IF  "$(CFG)" == "Frogger2 - Win32 Release"

!ELSEIF  "$(CFG)" == "Frogger2 - Win32 Debug"

# PROP Ignore_Default_Tool 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\common\sndview.h

!IF  "$(CFG)" == "Frogger2 - Win32 Release"

!ELSEIF  "$(CFG)" == "Frogger2 - Win32 Debug"

# PROP Ignore_Default_Tool 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\common\sonylibs.h

!IF  "$(CFG)" == "Frogger2 - Win32 Release"

!ELSEIF  "$(CFG)" == "Frogger2 - Win32 Debug"

# PROP Ignore_Default_Tool 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\common\specfx.c

!IF  "$(CFG)" == "Frogger2 - Win32 Release"

!ELSEIF  "$(CFG)" == "Frogger2 - Win32 Debug"

# PROP Ignore_Default_Tool 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\common\specfx.h

!IF  "$(CFG)" == "Frogger2 - Win32 Release"

!ELSEIF  "$(CFG)" == "Frogger2 - Win32 Debug"

# PROP Ignore_Default_Tool 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\common\sprite.c

!IF  "$(CFG)" == "Frogger2 - Win32 Release"

!ELSEIF  "$(CFG)" == "Frogger2 - Win32 Debug"

# PROP Ignore_Default_Tool 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\common\sprite.h

!IF  "$(CFG)" == "Frogger2 - Win32 Release"

!ELSEIF  "$(CFG)" == "Frogger2 - Win32 Debug"

# PROP Ignore_Default_Tool 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\common\story.c

!IF  "$(CFG)" == "Frogger2 - Win32 Release"

!ELSEIF  "$(CFG)" == "Frogger2 - Win32 Debug"

# PROP Ignore_Default_Tool 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\common\story.h

!IF  "$(CFG)" == "Frogger2 - Win32 Release"

!ELSEIF  "$(CFG)" == "Frogger2 - Win32 Debug"

# PROP Ignore_Default_Tool 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\common\textover.c

!IF  "$(CFG)" == "Frogger2 - Win32 Release"

!ELSEIF  "$(CFG)" == "Frogger2 - Win32 Debug"

# PROP Ignore_Default_Tool 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\common\textover.h

!IF  "$(CFG)" == "Frogger2 - Win32 Release"

!ELSEIF  "$(CFG)" == "Frogger2 - Win32 Debug"

# PROP Ignore_Default_Tool 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\common\tongue.c

!IF  "$(CFG)" == "Frogger2 - Win32 Release"

!ELSEIF  "$(CFG)" == "Frogger2 - Win32 Debug"

# PROP Ignore_Default_Tool 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\common\tongue.h

!IF  "$(CFG)" == "Frogger2 - Win32 Release"

!ELSEIF  "$(CFG)" == "Frogger2 - Win32 Debug"

# PROP Ignore_Default_Tool 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\common\training.c

!IF  "$(CFG)" == "Frogger2 - Win32 Release"

!ELSEIF  "$(CFG)" == "Frogger2 - Win32 Debug"

# PROP Ignore_Default_Tool 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\common\training.h

!IF  "$(CFG)" == "Frogger2 - Win32 Release"

!ELSEIF  "$(CFG)" == "Frogger2 - Win32 Debug"

# PROP Ignore_Default_Tool 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\common\types.c

!IF  "$(CFG)" == "Frogger2 - Win32 Release"

!ELSEIF  "$(CFG)" == "Frogger2 - Win32 Debug"

# PROP Ignore_Default_Tool 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\common\types.h

!IF  "$(CFG)" == "Frogger2 - Win32 Release"

!ELSEIF  "$(CFG)" == "Frogger2 - Win32 Debug"

# PROP Ignore_Default_Tool 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\common\types2d.h

!IF  "$(CFG)" == "Frogger2 - Win32 Release"

!ELSEIF  "$(CFG)" == "Frogger2 - Win32 Debug"

# PROP Ignore_Default_Tool 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\common\ultra64.h

!IF  "$(CFG)" == "Frogger2 - Win32 Release"

!ELSEIF  "$(CFG)" == "Frogger2 - Win32 Debug"

# PROP Ignore_Default_Tool 1

!ENDIF 

# End Source File
# End Group
# Begin Group "Dreamcast Files"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\dc\am_audio.c

!IF  "$(CFG)" == "Frogger2 - Win32 Release"

!ELSEIF  "$(CFG)" == "Frogger2 - Win32 Debug"

# PROP Ignore_Default_Tool 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\dc\am_audio.h

!IF  "$(CFG)" == "Frogger2 - Win32 Release"

!ELSEIF  "$(CFG)" == "Frogger2 - Win32 Debug"

# PROP Ignore_Default_Tool 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\dc\as.c
# End Source File
# Begin Source File

SOURCE=.\dc\as.h
# End Source File
# Begin Source File

SOURCE=.\dc\BackDrop.c

!IF  "$(CFG)" == "Frogger2 - Win32 Release"

!ELSEIF  "$(CFG)" == "Frogger2 - Win32 Debug"

# PROP Ignore_Default_Tool 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\dc\BackDrop.h

!IF  "$(CFG)" == "Frogger2 - Win32 Release"

!ELSEIF  "$(CFG)" == "Frogger2 - Win32 Debug"

# PROP Ignore_Default_Tool 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\dc\bpacsetup.c

!IF  "$(CFG)" == "Frogger2 - Win32 Release"

!ELSEIF  "$(CFG)" == "Frogger2 - Win32 Debug"

# PROP Ignore_Default_Tool 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\dc\bpacsetup.h

!IF  "$(CFG)" == "Frogger2 - Win32 Release"

!ELSEIF  "$(CFG)" == "Frogger2 - Win32 Debug"

# PROP Ignore_Default_Tool 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\dc\bpamsetup.c

!IF  "$(CFG)" == "Frogger2 - Win32 Release"

!ELSEIF  "$(CFG)" == "Frogger2 - Win32 Debug"

# PROP Ignore_Default_Tool 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\dc\bpamsetup.h

!IF  "$(CFG)" == "Frogger2 - Win32 Release"

!ELSEIF  "$(CFG)" == "Frogger2 - Win32 Debug"

# PROP Ignore_Default_Tool 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\dc\bpamsfx.c

!IF  "$(CFG)" == "Frogger2 - Win32 Release"

!ELSEIF  "$(CFG)" == "Frogger2 - Win32 Debug"

# PROP Ignore_Default_Tool 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\dc\bpamsfx.h

!IF  "$(CFG)" == "Frogger2 - Win32 Release"

!ELSEIF  "$(CFG)" == "Frogger2 - Win32 Debug"

# PROP Ignore_Default_Tool 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\dc\bpamstream.c

!IF  "$(CFG)" == "Frogger2 - Win32 Release"

!ELSEIF  "$(CFG)" == "Frogger2 - Win32 Debug"

# PROP Ignore_Default_Tool 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\dc\bpamstream.h

!IF  "$(CFG)" == "Frogger2 - Win32 Release"

!ELSEIF  "$(CFG)" == "Frogger2 - Win32 Debug"

# PROP Ignore_Default_Tool 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\dc\bpbuttons.h

!IF  "$(CFG)" == "Frogger2 - Win32 Release"

!ELSEIF  "$(CFG)" == "Frogger2 - Win32 Debug"

# PROP Ignore_Default_Tool 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\dc\bpprintf.c

!IF  "$(CFG)" == "Frogger2 - Win32 Release"

!ELSEIF  "$(CFG)" == "Frogger2 - Win32 Debug"

# PROP Ignore_Default_Tool 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\dc\bpprintf.h

!IF  "$(CFG)" == "Frogger2 - Win32 Release"

!ELSEIF  "$(CFG)" == "Frogger2 - Win32 Debug"

# PROP Ignore_Default_Tool 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\dc\dc_timer.c

!IF  "$(CFG)" == "Frogger2 - Win32 Release"

!ELSEIF  "$(CFG)" == "Frogger2 - Win32 Debug"

# PROP Ignore_Default_Tool 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\dc\dc_timer.h

!IF  "$(CFG)" == "Frogger2 - Win32 Release"

!ELSEIF  "$(CFG)" == "Frogger2 - Win32 Debug"

# PROP Ignore_Default_Tool 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\dc\DCK_Maths.c

!IF  "$(CFG)" == "Frogger2 - Win32 Release"

!ELSEIF  "$(CFG)" == "Frogger2 - Win32 Debug"

# PROP Ignore_Default_Tool 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\dc\DCK_Maths.h

!IF  "$(CFG)" == "Frogger2 - Win32 Release"

!ELSEIF  "$(CFG)" == "Frogger2 - Win32 Debug"

# PROP Ignore_Default_Tool 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\dc\DCK_System.c

!IF  "$(CFG)" == "Frogger2 - Win32 Release"

!ELSEIF  "$(CFG)" == "Frogger2 - Win32 Debug"

# PROP Ignore_Default_Tool 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\dc\DCK_System.h

!IF  "$(CFG)" == "Frogger2 - Win32 Release"

!ELSEIF  "$(CFG)" == "Frogger2 - Win32 Debug"

# PROP Ignore_Default_Tool 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\dc\DCK_Texture.c

!IF  "$(CFG)" == "Frogger2 - Win32 Release"

!ELSEIF  "$(CFG)" == "Frogger2 - Win32 Debug"

# PROP Ignore_Default_Tool 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\dc\DCK_Texture.h

!IF  "$(CFG)" == "Frogger2 - Win32 Release"

!ELSEIF  "$(CFG)" == "Frogger2 - Win32 Debug"

# PROP Ignore_Default_Tool 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\dc\DCK_Types.h

!IF  "$(CFG)" == "Frogger2 - Win32 Release"

!ELSEIF  "$(CFG)" == "Frogger2 - Win32 Debug"

# PROP Ignore_Default_Tool 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\dc\generic.h
# End Source File
# Begin Source File

SOURCE=.\dc\gte.c

!IF  "$(CFG)" == "Frogger2 - Win32 Release"

!ELSEIF  "$(CFG)" == "Frogger2 - Win32 Debug"

# PROP Ignore_Default_Tool 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\dc\gte.h

!IF  "$(CFG)" == "Frogger2 - Win32 Release"

!ELSEIF  "$(CFG)" == "Frogger2 - Win32 Debug"

# PROP Ignore_Default_Tool 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\dc\include.h

!IF  "$(CFG)" == "Frogger2 - Win32 Release"

!ELSEIF  "$(CFG)" == "Frogger2 - Win32 Debug"

# PROP Ignore_Default_Tool 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\dc\main.c

!IF  "$(CFG)" == "Frogger2 - Win32 Release"

!ELSEIF  "$(CFG)" == "Frogger2 - Win32 Debug"

# PROP Ignore_Default_Tool 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\dc\main.h

!IF  "$(CFG)" == "Frogger2 - Win32 Release"

!ELSEIF  "$(CFG)" == "Frogger2 - Win32 Debug"

# PROP Ignore_Default_Tool 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\dc\psxconv.h

!IF  "$(CFG)" == "Frogger2 - Win32 Release"

!ELSEIF  "$(CFG)" == "Frogger2 - Win32 Debug"

# PROP Ignore_Default_Tool 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\dc\QueueSfx.c
# End Source File
# Begin Source File

SOURCE=.\dc\QueueSfx.h
# End Source File
# Begin Source File

SOURCE=.\dc\sbinit.c

!IF  "$(CFG)" == "Frogger2 - Win32 Release"

!ELSEIF  "$(CFG)" == "Frogger2 - Win32 Debug"

# PROP Ignore_Default_Tool 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\dc\strsfx.h

!IF  "$(CFG)" == "Frogger2 - Win32 Release"

!ELSEIF  "$(CFG)" == "Frogger2 - Win32 Debug"

# PROP Ignore_Default_Tool 1

!ENDIF 

# End Source File
# End Group
# Begin Group "PSX Files"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\psx\audio.c

!IF  "$(CFG)" == "Frogger2 - Win32 Release"

!ELSEIF  "$(CFG)" == "Frogger2 - Win32 Debug"

# PROP Ignore_Default_Tool 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\psx\audio.h

!IF  "$(CFG)" == "Frogger2 - Win32 Release"

!ELSEIF  "$(CFG)" == "Frogger2 - Win32 Debug"

# PROP Ignore_Default_Tool 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\psx\backup.c
# End Source File
# Begin Source File

SOURCE=.\psx\backup.h
# End Source File
# Begin Source File

SOURCE=.\psx\bbpsiani.c

!IF  "$(CFG)" == "Frogger2 - Win32 Release"

!ELSEIF  "$(CFG)" == "Frogger2 - Win32 Debug"

# PROP Ignore_Default_Tool 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\psx\bbpsiani.h

!IF  "$(CFG)" == "Frogger2 - Win32 Release"

!ELSEIF  "$(CFG)" == "Frogger2 - Win32 Debug"

# PROP Ignore_Default_Tool 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\psx\bff_load.c

!IF  "$(CFG)" == "Frogger2 - Win32 Release"

!ELSEIF  "$(CFG)" == "Frogger2 - Win32 Debug"

# PROP Ignore_Default_Tool 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\psx\bff_load.h

!IF  "$(CFG)" == "Frogger2 - Win32 Release"

!ELSEIF  "$(CFG)" == "Frogger2 - Win32 Debug"

# PROP Ignore_Default_Tool 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\psx\CR_lang.c
# End Source File
# Begin Source File

SOURCE=.\psx\CR_lang.h
# End Source File
# Begin Source File

SOURCE=.\psx\custom.c

!IF  "$(CFG)" == "Frogger2 - Win32 Release"

!ELSEIF  "$(CFG)" == "Frogger2 - Win32 Debug"

# PROP Ignore_Default_Tool 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\psx\custom2.c

!IF  "$(CFG)" == "Frogger2 - Win32 Release"

!ELSEIF  "$(CFG)" == "Frogger2 - Win32 Debug"

# PROP Ignore_Default_Tool 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\psx\Eff_Draw.c

!IF  "$(CFG)" == "Frogger2 - Win32 Release"

!ELSEIF  "$(CFG)" == "Frogger2 - Win32 Debug"

# PROP Ignore_Default_Tool 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\psx\Eff_Draw.h

!IF  "$(CFG)" == "Frogger2 - Win32 Release"

!ELSEIF  "$(CFG)" == "Frogger2 - Win32 Debug"

# PROP Ignore_Default_Tool 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\psx\fadeout.c

!IF  "$(CFG)" == "Frogger2 - Win32 Release"

!ELSEIF  "$(CFG)" == "Frogger2 - Win32 Debug"

# PROP Ignore_Default_Tool 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\psx\fadeout.h

!IF  "$(CFG)" == "Frogger2 - Win32 Release"

!ELSEIF  "$(CFG)" == "Frogger2 - Win32 Debug"

# PROP Ignore_Default_Tool 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\psx\islcard.c
# End Source File
# Begin Source File

SOURCE=.\psx\islcard.h
# End Source File
# Begin Source File

SOURCE=.\psx\islfile.c

!IF  "$(CFG)" == "Frogger2 - Win32 Release"

!ELSEIF  "$(CFG)" == "Frogger2 - Win32 Debug"

# PROP Ignore_Default_Tool 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\psx\islfile.h

!IF  "$(CFG)" == "Frogger2 - Win32 Release"

!ELSEIF  "$(CFG)" == "Frogger2 - Win32 Debug"

# PROP Ignore_Default_Tool 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\psx\islfont.c

!IF  "$(CFG)" == "Frogger2 - Win32 Release"

!ELSEIF  "$(CFG)" == "Frogger2 - Win32 Debug"

# PROP Ignore_Default_Tool 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\psx\islfont.h

!IF  "$(CFG)" == "Frogger2 - Win32 Release"

!ELSEIF  "$(CFG)" == "Frogger2 - Win32 Debug"

# PROP Ignore_Default_Tool 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\psx\isllocal.c

!IF  "$(CFG)" == "Frogger2 - Win32 Release"

!ELSEIF  "$(CFG)" == "Frogger2 - Win32 Debug"

# PROP Ignore_Default_Tool 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\psx\isllocal.h

!IF  "$(CFG)" == "Frogger2 - Win32 Release"

!ELSEIF  "$(CFG)" == "Frogger2 - Win32 Debug"

# PROP Ignore_Default_Tool 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\psx\islpad.c

!IF  "$(CFG)" == "Frogger2 - Win32 Release"

!ELSEIF  "$(CFG)" == "Frogger2 - Win32 Debug"

# PROP Ignore_Default_Tool 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\psx\islpad.h

!IF  "$(CFG)" == "Frogger2 - Win32 Release"

!ELSEIF  "$(CFG)" == "Frogger2 - Win32 Debug"

# PROP Ignore_Default_Tool 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\psx\islpsi.h

!IF  "$(CFG)" == "Frogger2 - Win32 Release"

!ELSEIF  "$(CFG)" == "Frogger2 - Win32 Debug"

# PROP Ignore_Default_Tool 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\psx\isltex.c

!IF  "$(CFG)" == "Frogger2 - Win32 Release"

!ELSEIF  "$(CFG)" == "Frogger2 - Win32 Debug"

# PROP Ignore_Default_Tool 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\psx\isltex.h

!IF  "$(CFG)" == "Frogger2 - Win32 Release"

!ELSEIF  "$(CFG)" == "Frogger2 - Win32 Debug"

# PROP Ignore_Default_Tool 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\psx\islutil.c

!IF  "$(CFG)" == "Frogger2 - Win32 Release"

!ELSEIF  "$(CFG)" == "Frogger2 - Win32 Debug"

# PROP Ignore_Default_Tool 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\psx\islutil.h

!IF  "$(CFG)" == "Frogger2 - Win32 Release"

!ELSEIF  "$(CFG)" == "Frogger2 - Win32 Debug"

# PROP Ignore_Default_Tool 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\psx\islvideo.c

!IF  "$(CFG)" == "Frogger2 - Win32 Release"

!ELSEIF  "$(CFG)" == "Frogger2 - Win32 Debug"

# PROP Ignore_Default_Tool 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\psx\islvideo.h

!IF  "$(CFG)" == "Frogger2 - Win32 Release"

!ELSEIF  "$(CFG)" == "Frogger2 - Win32 Debug"

# PROP Ignore_Default_Tool 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\psx\islxa.c
# End Source File
# Begin Source File

SOURCE=.\psx\islxa.h
# End Source File
# Begin Source File

SOURCE=.\psx\lcdicons.h
# End Source File
# Begin Source File

SOURCE=.\psx\Libgpu.h

!IF  "$(CFG)" == "Frogger2 - Win32 Release"

!ELSEIF  "$(CFG)" == "Frogger2 - Win32 Debug"

# PROP Ignore_Default_Tool 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\psx\Libpad.h

!IF  "$(CFG)" == "Frogger2 - Win32 Release"

!ELSEIF  "$(CFG)" == "Frogger2 - Win32 Debug"

# PROP Ignore_Default_Tool 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\psx\LoadingBar.c

!IF  "$(CFG)" == "Frogger2 - Win32 Release"

!ELSEIF  "$(CFG)" == "Frogger2 - Win32 Debug"

# PROP Ignore_Default_Tool 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\psx\LoadingBar.h

!IF  "$(CFG)" == "Frogger2 - Win32 Release"

!ELSEIF  "$(CFG)" == "Frogger2 - Win32 Debug"

# PROP Ignore_Default_Tool 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\psx\map_draw.c

!IF  "$(CFG)" == "Frogger2 - Win32 Release"

!ELSEIF  "$(CFG)" == "Frogger2 - Win32 Debug"

# PROP Ignore_Default_Tool 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\psx\map_draw.h

!IF  "$(CFG)" == "Frogger2 - Win32 Release"

!ELSEIF  "$(CFG)" == "Frogger2 - Win32 Debug"

# PROP Ignore_Default_Tool 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\psx\memcard.c
# End Source File
# Begin Source File

SOURCE=.\psx\memcard.h
# End Source File
# Begin Source File

SOURCE=.\psx\newsfx.h
# End Source File
# Begin Source File

SOURCE=.\psx\objects.c

!IF  "$(CFG)" == "Frogger2 - Win32 Release"

!ELSEIF  "$(CFG)" == "Frogger2 - Win32 Debug"

# PROP Ignore_Default_Tool 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\psx\objects.h

!IF  "$(CFG)" == "Frogger2 - Win32 Release"

!ELSEIF  "$(CFG)" == "Frogger2 - Win32 Debug"

# PROP Ignore_Default_Tool 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\psx\psi.c

!IF  "$(CFG)" == "Frogger2 - Win32 Release"

!ELSEIF  "$(CFG)" == "Frogger2 - Win32 Debug"

# PROP Ignore_Default_Tool 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\psx\psi.h

!IF  "$(CFG)" == "Frogger2 - Win32 Release"

!ELSEIF  "$(CFG)" == "Frogger2 - Win32 Debug"

# PROP Ignore_Default_Tool 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\psx\psiactor.c

!IF  "$(CFG)" == "Frogger2 - Win32 Release"

!ELSEIF  "$(CFG)" == "Frogger2 - Win32 Debug"

# PROP Ignore_Default_Tool 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\psx\psiactor.h

!IF  "$(CFG)" == "Frogger2 - Win32 Release"

!ELSEIF  "$(CFG)" == "Frogger2 - Win32 Debug"

# PROP Ignore_Default_Tool 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\psx\psitypes.h

!IF  "$(CFG)" == "Frogger2 - Win32 Release"

!ELSEIF  "$(CFG)" == "Frogger2 - Win32 Debug"

# PROP Ignore_Default_Tool 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\psx\psxsprite.c

!IF  "$(CFG)" == "Frogger2 - Win32 Release"

!ELSEIF  "$(CFG)" == "Frogger2 - Win32 Debug"

# PROP Ignore_Default_Tool 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\psx\psxsprite.h

!IF  "$(CFG)" == "Frogger2 - Win32 Release"

!ELSEIF  "$(CFG)" == "Frogger2 - Win32 Debug"

# PROP Ignore_Default_Tool 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\psx\psxtongue.c

!IF  "$(CFG)" == "Frogger2 - Win32 Release"

!ELSEIF  "$(CFG)" == "Frogger2 - Win32 Debug"

# PROP Ignore_Default_Tool 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\psx\psxtongue.h

!IF  "$(CFG)" == "Frogger2 - Win32 Release"

!ELSEIF  "$(CFG)" == "Frogger2 - Win32 Debug"

# PROP Ignore_Default_Tool 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\psx\quatern.c

!IF  "$(CFG)" == "Frogger2 - Win32 Release"

!ELSEIF  "$(CFG)" == "Frogger2 - Win32 Debug"

# PROP Ignore_Default_Tool 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\psx\quatern.h

!IF  "$(CFG)" == "Frogger2 - Win32 Release"

!ELSEIF  "$(CFG)" == "Frogger2 - Win32 Debug"

# PROP Ignore_Default_Tool 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\psx\saveicon.h
# End Source File
# Begin Source File

SOURCE=.\psx\shell.h
# End Source File
# Begin Source File

SOURCE=.\psx\temp_psx.c

!IF  "$(CFG)" == "Frogger2 - Win32 Release"

!ELSEIF  "$(CFG)" == "Frogger2 - Win32 Debug"

# PROP Ignore_Default_Tool 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\psx\temp_psx.h

!IF  "$(CFG)" == "Frogger2 - Win32 Release"

!ELSEIF  "$(CFG)" == "Frogger2 - Win32 Debug"

# PROP Ignore_Default_Tool 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\psx\textures.c

!IF  "$(CFG)" == "Frogger2 - Win32 Release"

!ELSEIF  "$(CFG)" == "Frogger2 - Win32 Debug"

# PROP Ignore_Default_Tool 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\psx\textures.h

!IF  "$(CFG)" == "Frogger2 - Win32 Release"

!ELSEIF  "$(CFG)" == "Frogger2 - Win32 Debug"

# PROP Ignore_Default_Tool 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\psx\Usr.c

!IF  "$(CFG)" == "Frogger2 - Win32 Release"

!ELSEIF  "$(CFG)" == "Frogger2 - Win32 Debug"

# PROP Ignore_Default_Tool 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\psx\Usr.h

!IF  "$(CFG)" == "Frogger2 - Win32 Release"

!ELSEIF  "$(CFG)" == "Frogger2 - Win32 Debug"

# PROP Ignore_Default_Tool 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\psx\water.c

!IF  "$(CFG)" == "Frogger2 - Win32 Release"

!ELSEIF  "$(CFG)" == "Frogger2 - Win32 Debug"

# PROP Ignore_Default_Tool 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\psx\water.h

!IF  "$(CFG)" == "Frogger2 - Win32 Release"

!ELSEIF  "$(CFG)" == "Frogger2 - Win32 Debug"

# PROP Ignore_Default_Tool 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\psx\world.c

!IF  "$(CFG)" == "Frogger2 - Win32 Release"

!ELSEIF  "$(CFG)" == "Frogger2 - Win32 Debug"

# PROP Ignore_Default_Tool 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\psx\world.h

!IF  "$(CFG)" == "Frogger2 - Win32 Release"

!ELSEIF  "$(CFG)" == "Frogger2 - Win32 Debug"

# PROP Ignore_Default_Tool 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\psx\World_Eff.c

!IF  "$(CFG)" == "Frogger2 - Win32 Release"

!ELSEIF  "$(CFG)" == "Frogger2 - Win32 Debug"

# PROP Ignore_Default_Tool 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\psx\World_Eff.h

!IF  "$(CFG)" == "Frogger2 - Win32 Release"

!ELSEIF  "$(CFG)" == "Frogger2 - Win32 Debug"

# PROP Ignore_Default_Tool 1

!ENDIF 

# End Source File
# End Group
# Begin Source File

SOURCE=..\cd\track03\language.txt
# End Source File
# Begin Source File

SOURCE=.\makefile
# PROP Ignore_Default_Tool 1
# End Source File
# Begin Source File

SOURCE=.\prefix_dc.h
# End Source File
# Begin Source File

SOURCE=.\todo.txt
# End Source File
# End Target
# End Project
