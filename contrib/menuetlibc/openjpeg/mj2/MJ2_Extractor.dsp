# Microsoft Developer Studio Project File - Name="MJ2_Extractor" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Console Application" 0x0103

CFG=MJ2_Extractor - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "MJ2_Extractor.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "MJ2_Extractor.mak" CFG="MJ2_Extractor - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "MJ2_Extractor - Win32 Release" (based on "Win32 (x86) Console Application")
!MESSAGE "MJ2_Extractor - Win32 Debug" (based on "Win32 (x86) Console Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "MJ2_Extractor - Win32 Release"

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
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /YX /FD /c
# ADD CPP /nologo /W3 /GX /O2 /I "../libopenjpeg" /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /D "OPJ_STATIC" /YX /FD /c
# ADD BASE RSC /l 0x809 /d "NDEBUG"
# ADD RSC /l 0x809 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /machine:I386 /nodefaultlib:"libcmt"
# SUBTRACT LINK32 /pdb:none

!ELSEIF  "$(CFG)" == "MJ2_Extractor - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "MJ2_Extractor___Win32_Debug"
# PROP BASE Intermediate_Dir "MJ2_Extractor___Win32_Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "MJ2_Extractor___Win32_Debug"
# PROP Intermediate_Dir "MJ2_Extractor___Win32_Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /YX /FD /GZ /c
# ADD CPP /nologo /MT /W3 /Gm /GX /ZI /Od /I "../libopenjpeg" /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /D "OPJ_STATIC" /FR /YX /FD /GZ /c
# ADD BASE RSC /l 0x809 /d "_DEBUG"
# ADD RSC /l 0x809 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /debug /machine:I386 /pdbtype:sept
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /debug /machine:I386 /nodefaultlib:"libcmt" /pdbtype:sept
# SUBTRACT LINK32 /pdb:none

!ENDIF 

# Begin Target

# Name "MJ2_Extractor - Win32 Release"
# Name "MJ2_Extractor - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\extract_j2k_from_mj2.c
# End Source File
# Begin Source File

SOURCE=.\mj2.c
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\mj2.h
# End Source File
# End Group
# Begin Group "OpenJPEG Header Files"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\libopenjpeg\bio.h
# End Source File
# Begin Source File

SOURCE=..\libopenjpeg\cio.h
# End Source File
# Begin Source File

SOURCE=..\libopenjpeg\dwt.h
# End Source File
# Begin Source File

SOURCE=..\libopenjpeg\event.h
# End Source File
# Begin Source File

SOURCE=..\libopenjpeg\fix.h
# End Source File
# Begin Source File

SOURCE=..\libopenjpeg\image.h
# End Source File
# Begin Source File

SOURCE=..\libopenjpeg\int.h
# End Source File
# Begin Source File

SOURCE=..\libopenjpeg\j2k.h
# End Source File
# Begin Source File

SOURCE=..\libopenjpeg\j2k_lib.h
# End Source File
# Begin Source File

SOURCE=..\libopenjpeg\jp2.h
# End Source File
# Begin Source File

SOURCE=..\libopenjpeg\jpt.h
# End Source File
# Begin Source File

SOURCE=..\libopenjpeg\mct.h
# End Source File
# Begin Source File

SOURCE=..\libopenjpeg\mqc.h
# End Source File
# Begin Source File

SOURCE=..\libopenjpeg\openjpeg.h
# End Source File
# Begin Source File

SOURCE=..\libopenjpeg\opj_includes.h
# End Source File
# Begin Source File

SOURCE=..\libopenjpeg\pi.h
# End Source File
# Begin Source File

SOURCE=..\libopenjpeg\raw.h
# End Source File
# Begin Source File

SOURCE=..\libopenjpeg\t1.h
# End Source File
# Begin Source File

SOURCE=..\libopenjpeg\t2.h
# End Source File
# Begin Source File

SOURCE=..\libopenjpeg\tcd.h
# End Source File
# Begin Source File

SOURCE=..\libopenjpeg\tgt.h
# End Source File
# End Group
# End Target
# End Project
