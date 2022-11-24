# Microsoft Developer Studio Project File - Name="WinPVR" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=WinPVR - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "WinPVR.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "WinPVR.mak" CFG="WinPVR - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "WinPVR - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "WinPVR - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath "Desktop"
# PROP WCE_FormatVersion "6.0"
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "WinPVR - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /YX /FD /c
# ADD CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /YX /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x809 /d "NDEBUG"
# ADD RSC /l 0x809 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib comctl32.lib version.lib ..\vqdll\vqdll.lib /nologo /subsystem:windows /machine:I386

!ELSEIF  "$(CFG)" == "WinPVR - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /YX /FD /GZ /c
# ADD CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /FR /YX /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x809 /d "_DEBUG"
# ADD RSC /l 0x809 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib comctl32.lib version.lib ..\vqdll\vqdll.lib /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept

!ENDIF 

# Begin Target

# Name "WinPVR - Win32 Release"
# Name "WinPVR - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\Browse.cpp
# End Source File
# Begin Source File

SOURCE=..\C.cpp
# End Source File
# Begin Source File

SOURCE=..\Colour.cpp
# End Source File
# Begin Source File

SOURCE=..\Image.cpp
# End Source File
# Begin Source File

SOURCE=.\Log.cpp
# End Source File
# Begin Source File

SOURCE=..\PIC.cpp
# End Source File
# Begin Source File

SOURCE=..\Picture.cpp
# End Source File
# Begin Source File

SOURCE=..\PVR.cpp
# End Source File
# Begin Source File

SOURCE=..\Resample.cpp
# End Source File
# Begin Source File

SOURCE=..\Twiddle.cpp
# End Source File
# Begin Source File

SOURCE=..\Util.cpp
# End Source File
# Begin Source File

SOURCE=.\VQAnalysis.cpp
# End Source File
# Begin Source File

SOURCE=..\VQCompressor.cpp
# End Source File
# Begin Source File

SOURCE=..\VQF.cpp
# End Source File
# Begin Source File

SOURCE=.\VQFDialog.cpp
# End Source File
# Begin Source File

SOURCE=..\VQImage.cpp
# End Source File
# Begin Source File

SOURCE=.\WinPVR.cpp
# End Source File
# Begin Source File

SOURCE=.\WinUtil.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\Browse.h
# End Source File
# Begin Source File

SOURCE=..\C.h
# End Source File
# Begin Source File

SOURCE=..\Colour.h
# End Source File
# Begin Source File

SOURCE=..\Image.h
# End Source File
# Begin Source File

SOURCE=.\Log.h
# End Source File
# Begin Source File

SOURCE=..\PIC.h
# End Source File
# Begin Source File

SOURCE=..\Picture.h
# End Source File
# Begin Source File

SOURCE=..\PVR.h
# End Source File
# Begin Source File

SOURCE=..\Resample.h
# End Source File
# Begin Source File

SOURCE=..\Twiddle.h
# End Source File
# Begin Source File

SOURCE=..\Util.h
# End Source File
# Begin Source File

SOURCE=.\VQAnalysis.h
# End Source File
# Begin Source File

SOURCE=..\VQCompressor.h
# End Source File
# Begin Source File

SOURCE=..\VQF.h
# End Source File
# Begin Source File

SOURCE=.\VQFDialog.h
# End Source File
# Begin Source File

SOURCE=..\VQImage.h
# End Source File
# Begin Source File

SOURCE=.\WinUtil.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\appicon.ico
# End Source File
# Begin Source File

SOURCE=.\res\appicon.ico
# End Source File
# Begin Source File

SOURCE=.\res\browseicon.ico
# End Source File
# Begin Source File

SOURCE=.\res\browsetoolbar.bmp
# End Source File
# Begin Source File

SOURCE=.\pan.cur
# End Source File
# Begin Source File

SOURCE=.\res\pan.cur
# End Source File
# Begin Source File

SOURCE=.\res\point.cur
# End Source File
# Begin Source File

SOURCE=.\res\pvrfile.ico
# End Source File
# Begin Source File

SOURCE=.\resource.rc
# End Source File
# Begin Source File

SOURCE=.\res\toolbar.bmp
# End Source File
# Begin Source File

SOURCE=.\toolbar.bmp
# End Source File
# End Group
# Begin Group "Library Files"

# PROP Default_Filter ".lib"
# Begin Group "Release"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\paintlib\lib\Release\libjpeg.lib
# End Source File
# Begin Source File

SOURCE=..\paintlib\lib\Release\libpng.lib
# End Source File
# Begin Source File

SOURCE=..\paintlib\lib\Release\libtiff.lib
# End Source File
# Begin Source File

SOURCE=..\paintlib\lib\Release\paintlib.lib
# End Source File
# End Group
# End Group
# Begin Source File

SOURCE=..\todo.txt
# End Source File
# End Target
# End Project
