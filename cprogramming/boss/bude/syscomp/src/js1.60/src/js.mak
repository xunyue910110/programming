# Microsoft Developer Studio Generated NMAKE File, Format Version 4.20
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Console Application" 0x0103
# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102
# TARGTYPE "Win32 (x86) Static Library" 0x0104

!IF "$(CFG)" == ""
CFG=jsshell - Win32 Debug
!MESSAGE No configuration specified.  Defaulting to jsshell - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "js - Win32 Release" && "$(CFG)" != "js - Win32 Debug" &&\
 "$(CFG)" != "jsshell - Win32 Release" && "$(CFG)" != "jsshell - Win32 Debug" &&\
 "$(CFG)" != "fdlibm - Win32 Release" && "$(CFG)" != "fdlibm - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE on this makefile
!MESSAGE by defining the macro CFG on the command line.  For example:
!MESSAGE 
!MESSAGE NMAKE /f "js.mak" CFG="jsshell - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "js - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "js - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "jsshell - Win32 Release" (based on "Win32 (x86) Console Application")
!MESSAGE "jsshell - Win32 Debug" (based on "Win32 (x86) Console Application")
!MESSAGE "fdlibm - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "fdlibm - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE 
!ERROR An invalid configuration is specified.
!ENDIF 

!IF "$(OS)" == "Windows_NT"
NULL=
!ELSE 
NULL=nul
!ENDIF 
################################################################################
# Begin Project
# PROP Target_Last_Scanned "jsshell - Win32 Debug"

!IF  "$(CFG)" == "js - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "js___Wi1"
# PROP BASE Intermediate_Dir "js___Wi1"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Target_Dir ""
OUTDIR=.\Release
INTDIR=.\Release

ALL : "fdlibm - Win32 Release" "$(OUTDIR)\js32.dll"

CLEAN : 
	-@erase "$(INTDIR)\jsapi.obj"
	-@erase "$(INTDIR)\jsarena.obj"
	-@erase "$(INTDIR)\jsarray.obj"
	-@erase "$(INTDIR)\jsatom.obj"
	-@erase "$(INTDIR)\jsbool.obj"
	-@erase "$(INTDIR)\jscntxt.obj"
	-@erase "$(INTDIR)\jsdate.obj"
	-@erase "$(INTDIR)\jsdbgapi.obj"
	-@erase "$(INTDIR)\jsdhash.obj"
	-@erase "$(INTDIR)\jsdtoa.obj"
	-@erase "$(INTDIR)\jsemit.obj"
	-@erase "$(INTDIR)\jsexn.obj"
	-@erase "$(INTDIR)\jsfun.obj"
	-@erase "$(INTDIR)\jsgc.obj"
	-@erase "$(INTDIR)\jshash.obj"
	-@erase "$(INTDIR)\jsinterp.obj"
	-@erase "$(INTDIR)\jslock.obj"
	-@erase "$(INTDIR)\jslog2.obj"
	-@erase "$(INTDIR)\jslong.obj"
	-@erase "$(INTDIR)\jsmath.obj"
	-@erase "$(INTDIR)\jsnum.obj"
	-@erase "$(INTDIR)\jsobj.obj"
	-@erase "$(INTDIR)\jsopcode.obj"
	-@erase "$(INTDIR)\jsparse.obj"
	-@erase "$(INTDIR)\jsprf.obj"
	-@erase "$(INTDIR)\jsregexp.obj"
	-@erase "$(INTDIR)\jsscan.obj"
	-@erase "$(INTDIR)\jsscope.obj"
	-@erase "$(INTDIR)\jsscript.obj"
	-@erase "$(INTDIR)\jsstr.obj"
	-@erase "$(INTDIR)\jsutil.obj"
	-@erase "$(INTDIR)\jsxdrapi.obj"
	-@erase "$(INTDIR)\jsxml.obj"
	-@erase "$(INTDIR)\prmjtime.obj"
	-@erase "$(OUTDIR)\js32.dll"
	-@erase "$(OUTDIR)\js32.exp"
	-@erase "$(OUTDIR)\js32.lib"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D _X86_=1 /D "_WINDOWS" /YX /c
# ADD CPP /nologo /MD /W3 /GX /O2 /D "NDEBUG" /D _X86_=1 /D "_WINDOWS" /D "WIN32" /D "XP_WIN" /D "JSFILE" /D "EXPORT_JS_API" /YX /c
CPP_PROJ=/nologo /MD /W3 /GX /O2 /D "NDEBUG" /D _X86_=1 /D "_WINDOWS" /D "WIN32" /D\
 "XP_WIN" /D "JSFILE" /D "EXPORT_JS_API" /Fp"$(INTDIR)/js.pch" /YX\
 /Fo"$(INTDIR)/" /c 
CPP_OBJS=.\Release/
CPP_SBRS=.\.

.c{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.cpp{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.cxx{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.c{$(CPP_SBRS)}.sbr:
   $(CPP) $(CPP_PROJ) $<  

.cpp{$(CPP_SBRS)}.sbr:
   $(CPP) $(CPP_PROJ) $<  

.cxx{$(CPP_SBRS)}.sbr:
   $(CPP) $(CPP_PROJ) $<  

MTL=mktyplib.exe
# ADD BASE MTL /nologo /D "NDEBUG" /win32
# ADD MTL /nologo /D "NDEBUG" /win32
MTL_PROJ=/nologo /D "NDEBUG" /win32 
RSC=rc.exe
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/js.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /machine:I386 /out:"Release/js32.dll"
# SUBTRACT LINK32 /nodefaultlib
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib\
 advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib\
 odbccp32.lib /nologo /subsystem:windows /dll /incremental:no\
 /pdb:"$(OUTDIR)/js32.pdb" /machine:I386 /out:"$(OUTDIR)/js32.dll"\
 /implib:"$(OUTDIR)/js32.lib" /opt:ref /opt:noicf
LINK32_OBJS= \
	"$(INTDIR)\jsapi.obj" \
	"$(INTDIR)\jsarena.obj" \
	"$(INTDIR)\jsarray.obj" \
	"$(INTDIR)\jsatom.obj" \
	"$(INTDIR)\jsbool.obj" \
	"$(INTDIR)\jscntxt.obj" \
	"$(INTDIR)\jsdate.obj" \
	"$(INTDIR)\jsdbgapi.obj" \
	"$(INTDIR)\jsdhash.obj" \
	"$(INTDIR)\jsdtoa.obj" \
	"$(INTDIR)\jsemit.obj" \
	"$(INTDIR)\jsexn.obj" \
	"$(INTDIR)\jsfun.obj" \
	"$(INTDIR)\jsgc.obj" \
	"$(INTDIR)\jshash.obj" \
	"$(INTDIR)\jsinterp.obj" \
	"$(INTDIR)\jslock.obj" \
	"$(INTDIR)\jslog2.obj" \
	"$(INTDIR)\jslong.obj" \
	"$(INTDIR)\jsmath.obj" \
	"$(INTDIR)\jsnum.obj" \
	"$(INTDIR)\jsobj.obj" \
	"$(INTDIR)\jsopcode.obj" \
	"$(INTDIR)\jsparse.obj" \
	"$(INTDIR)\jsprf.obj" \
	"$(INTDIR)\jsregexp.obj" \
	"$(INTDIR)\jsscan.obj" \
	"$(INTDIR)\jsscope.obj" \
	"$(INTDIR)\jsscript.obj" \
	"$(INTDIR)\jsstr.obj" \
	"$(INTDIR)\jsutil.obj" \
	"$(INTDIR)\jsxdrapi.obj" \
	"$(INTDIR)\jsxml.obj" \
	"$(INTDIR)\prmjtime.obj" \
	"$(OUTDIR)\fdlibm.lib"

"$(OUTDIR)\js32.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "js - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "js___Wi2"
# PROP BASE Intermediate_Dir "js___Wi2"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Target_Dir ""
OUTDIR=.\Debug
INTDIR=.\Debug

ALL : "fdlibm - Win32 Debug" "$(OUTDIR)\js32.dll"

CLEAN : 
	-@erase "$(INTDIR)\jsapi.obj"
	-@erase "$(INTDIR)\jsarena.obj"
	-@erase "$(INTDIR)\jsarray.obj"
	-@erase "$(INTDIR)\jsatom.obj"
	-@erase "$(INTDIR)\jsbool.obj"
	-@erase "$(INTDIR)\jscntxt.obj"
	-@erase "$(INTDIR)\jsdate.obj"
	-@erase "$(INTDIR)\jsdbgapi.obj"
	-@erase "$(INTDIR)\jsdhash.obj"
	-@erase "$(INTDIR)\jsdtoa.obj"
	-@erase "$(INTDIR)\jsemit.obj"
	-@erase "$(INTDIR)\jsexn.obj"
	-@erase "$(INTDIR)\jsfun.obj"
	-@erase "$(INTDIR)\jsgc.obj"
	-@erase "$(INTDIR)\jshash.obj"
	-@erase "$(INTDIR)\jsinterp.obj"
	-@erase "$(INTDIR)\jslock.obj"
	-@erase "$(INTDIR)\jslog2.obj"
	-@erase "$(INTDIR)\jslong.obj"
	-@erase "$(INTDIR)\jsmath.obj"
	-@erase "$(INTDIR)\jsnum.obj"
	-@erase "$(INTDIR)\jsobj.obj"
	-@erase "$(INTDIR)\jsopcode.obj"
	-@erase "$(INTDIR)\jsparse.obj"
	-@erase "$(INTDIR)\jsprf.obj"
	-@erase "$(INTDIR)\jsregexp.obj"
	-@erase "$(INTDIR)\jsscan.obj"
	-@erase "$(INTDIR)\jsscope.obj"
	-@erase "$(INTDIR)\jsscript.obj"
	-@erase "$(INTDIR)\jsstr.obj"
	-@erase "$(INTDIR)\jsutil.obj"
	-@erase "$(INTDIR)\jsxdrapi.obj"
	-@erase "$(INTDIR)\jsxml.obj"
	-@erase "$(INTDIR)\prmjtime.obj"
	-@erase "$(OUTDIR)\js32.dll"
	-@erase "$(OUTDIR)\js32.exp"
	-@erase "$(OUTDIR)\js32.ilk"
	-@erase "$(OUTDIR)\js32.lib"
	-@erase "$(OUTDIR)\js32.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D _X86_=1 /D "_WINDOWS" /YX /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /Zi /Od /D "_DEBUG" /D "DEBUG" /D _X86_=1 /D "_WINDOWS" /D "WIN32" /D "XP_WIN" /D "JSFILE" /D "EXPORT_JS_API" /YX /c
CPP_PROJ=/nologo /MDd /W3 /Gm /GX /Zi /Od /D "_DEBUG" /D "DEBUG" /D _X86_=1 /D "_WINDOWS"\
 /D "WIN32" /D "XP_WIN" /D "JSFILE" /D "EXPORT_JS_API" /Fp"$(INTDIR)/js.pch" /YX\
 /Fo"$(INTDIR)/" /Fd"$(INTDIR)/" /c 
CPP_OBJS=.\Debug/
CPP_SBRS=.\.

.c{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.cpp{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.cxx{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.c{$(CPP_SBRS)}.sbr:
   $(CPP) $(CPP_PROJ) $<  

.cpp{$(CPP_SBRS)}.sbr:
   $(CPP) $(CPP_PROJ) $<  

.cxx{$(CPP_SBRS)}.sbr:
   $(CPP) $(CPP_PROJ) $<  

MTL=mktyplib.exe
# ADD BASE MTL /nologo /D "_DEBUG" /win32
# ADD MTL /nologo /D "_DEBUG" /win32
MTL_PROJ=/nologo /D "_DEBUG" /win32 
RSC=rc.exe
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/js.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /debug /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /debug /machine:I386 /out:"Debug/js32.dll"
# SUBTRACT LINK32 /nodefaultlib
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib\
 advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib\
 odbccp32.lib /nologo /subsystem:windows /dll /incremental:yes\
 /pdb:"$(OUTDIR)/js32.pdb" /debug /machine:I386 /out:"$(OUTDIR)/js32.dll"\
 /implib:"$(OUTDIR)/js32.lib" 
LINK32_OBJS= \
	"$(INTDIR)\jsapi.obj" \
	"$(INTDIR)\jsarena.obj" \
	"$(INTDIR)\jsarray.obj" \
	"$(INTDIR)\jsatom.obj" \
	"$(INTDIR)\jsbool.obj" \
	"$(INTDIR)\jscntxt.obj" \
	"$(INTDIR)\jsdate.obj" \
	"$(INTDIR)\jsdbgapi.obj" \
	"$(INTDIR)\jsdhash.obj" \
	"$(INTDIR)\jsdtoa.obj" \
	"$(INTDIR)\jsemit.obj" \
	"$(INTDIR)\jsexn.obj" \
	"$(INTDIR)\jsfun.obj" \
	"$(INTDIR)\jsgc.obj" \
	"$(INTDIR)\jshash.obj" \
	"$(INTDIR)\jsinterp.obj" \
	"$(INTDIR)\jslock.obj" \
	"$(INTDIR)\jslog2.obj" \
	"$(INTDIR)\jslong.obj" \
	"$(INTDIR)\jsmath.obj" \
	"$(INTDIR)\jsnum.obj" \
	"$(INTDIR)\jsobj.obj" \
	"$(INTDIR)\jsopcode.obj" \
	"$(INTDIR)\jsparse.obj" \
	"$(INTDIR)\jsprf.obj" \
	"$(INTDIR)\jsregexp.obj" \
	"$(INTDIR)\jsscan.obj" \
	"$(INTDIR)\jsscope.obj" \
	"$(INTDIR)\jsscript.obj" \
	"$(INTDIR)\jsstr.obj" \
	"$(INTDIR)\jsutil.obj" \
	"$(INTDIR)\jsxdrapi.obj" \
	"$(INTDIR)\jsxml.obj" \
	"$(INTDIR)\prmjtime.obj" \
	"$(OUTDIR)\fdlibm.lib"

"$(OUTDIR)\js32.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "jsshell - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "jsshell\Release"
# PROP BASE Intermediate_Dir "jsshell\Release"
# PROP BASE Target_Dir "jsshell"
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Target_Dir "jsshell"
OUTDIR=.\Release
INTDIR=.\Release

ALL : "js - Win32 Release" "$(OUTDIR)\jsshell.exe"

CLEAN : 
	-@erase "$(INTDIR)\js.obj"
	-@erase "$(OUTDIR)\jsshell.exe"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /YX /c
# ADD CPP /nologo /MD /W3 /GX /O2 /D "NDEBUG" /D "_CONSOLE" /D "WIN32" /D "XP_WIN" /D "JSFILE" /YX /c
CPP_PROJ=/nologo /MD /W3 /GX /O2 /D "NDEBUG" /D "_CONSOLE" /D "WIN32" /D\
 "XP_WIN" /D "JSFILE" /Fp"$(INTDIR)/jsshell.pch" /YX /Fo"$(INTDIR)/" /c 
CPP_OBJS=.\Release/
CPP_SBRS=.\.

.c{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.cpp{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.cxx{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.c{$(CPP_SBRS)}.sbr:
   $(CPP) $(CPP_PROJ) $<  

.cpp{$(CPP_SBRS)}.sbr:
   $(CPP) $(CPP_PROJ) $<  

.cxx{$(CPP_SBRS)}.sbr:
   $(CPP) $(CPP_PROJ) $<  

RSC=rc.exe
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/jsshell.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /machine:I386
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib\
 advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib\
 odbccp32.lib /nologo /subsystem:console /incremental:no\
 /pdb:"$(OUTDIR)/jsshell.pdb" /machine:I386 /out:"$(OUTDIR)/jsshell.exe" 
LINK32_OBJS= \
	"$(INTDIR)\js.obj" \
	"$(OUTDIR)\js32.lib"

"$(OUTDIR)\jsshell.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "jsshell - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "jsshell\jsshell_"
# PROP BASE Intermediate_Dir "jsshell\jsshell_"
# PROP BASE Target_Dir "jsshell"
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Target_Dir "jsshell"
OUTDIR=.\Debug
INTDIR=.\Debug

ALL : "js - Win32 Debug" "$(OUTDIR)\jsshell.exe"

CLEAN : 
	-@erase "$(INTDIR)\js.obj"
	-@erase "$(OUTDIR)\jsshell.exe"
	-@erase "$(OUTDIR)\jsshell.ilk"
	-@erase "$(OUTDIR)\jsshell.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
# ADD BASE CPP /nologo /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /YX /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /Zi /Od /D "_CONSOLE" /D "_DEBUG" /D "WIN32" /D "XP_WIN" /D "JSFILE" /D "DEBUG" /YX /c
CPP_PROJ=/nologo /MDd /W3 /Gm /GX /Zi /Od /D "_CONSOLE" /D "_DEBUG" /D "WIN32"\
 /D "XP_WIN" /D "JSFILE" /D "DEBUG" /Fp"$(INTDIR)/jsshell.pch" /YX\
 /Fo"$(INTDIR)/" /Fd"$(INTDIR)/" /c 
CPP_OBJS=.\Debug/
CPP_SBRS=.\.

.c{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.cpp{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.cxx{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.c{$(CPP_SBRS)}.sbr:
   $(CPP) $(CPP_PROJ) $<  

.cpp{$(CPP_SBRS)}.sbr:
   $(CPP) $(CPP_PROJ) $<  

.cxx{$(CPP_SBRS)}.sbr:
   $(CPP) $(CPP_PROJ) $<  

RSC=rc.exe
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/jsshell.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /debug /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /debug /machine:I386
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib\
 advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib\
 odbccp32.lib /nologo /subsystem:console /incremental:yes\
 /pdb:"$(OUTDIR)/jsshell.pdb" /debug /machine:I386 /out:"$(OUTDIR)/jsshell.exe" 
LINK32_OBJS= \
	"$(INTDIR)\js.obj" \
	"$(OUTDIR)\js32.lib"

"$(OUTDIR)\jsshell.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "fdlibm - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "fdlibm\Release"
# PROP BASE Intermediate_Dir "fdlibm\Release"
# PROP BASE Target_Dir "fdlibm"
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Target_Dir "fdlibm"
OUTDIR=.\Release
INTDIR=.\Release

ALL : "$(OUTDIR)\fdlibm.lib"

CLEAN : 
	-@erase "$(INTDIR)\e_atan2.obj"
	-@erase "$(INTDIR)\e_pow.obj"
	-@erase "$(INTDIR)\e_sqrt.obj"
	-@erase "$(INTDIR)\k_standard.obj"
	-@erase "$(INTDIR)\s_atan.obj"
	-@erase "$(INTDIR)\s_copysign.obj"
	-@erase "$(INTDIR)\s_fabs.obj"
	-@erase "$(INTDIR)\s_finite.obj"
	-@erase "$(INTDIR)\s_isnan.obj"
	-@erase "$(INTDIR)\s_matherr.obj"
	-@erase "$(INTDIR)\s_rint.obj"
	-@erase "$(INTDIR)\s_scalbn.obj"
	-@erase "$(INTDIR)\w_atan2.obj"
	-@erase "$(INTDIR)\w_pow.obj"
	-@erase "$(INTDIR)\w_sqrt.obj"
	-@erase "$(OUTDIR)\fdlibm.lib"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D _X86_=1 /D "_WINDOWS" /YX /c
# ADD CPP /nologo /MD /W3 /GX /O2 /D "NDEBUG" /D "WIN32" /D _X86_=1 /D "_WINDOWS" /D "_IEEE_LIBM" /YX /c
CPP_PROJ=/nologo /MD /W3 /GX /O2 /D "NDEBUG" /D "WIN32" /D _X86_=1 /D "_WINDOWS" /D\
 "_IEEE_LIBM" /D "XP_WIN" /I .\ /Fp"$(INTDIR)/fdlibm.pch" /YX /Fo"$(INTDIR)/" /c 
CPP_OBJS=.\Release/
CPP_SBRS=.\.

.c{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.cpp{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.cxx{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.c{$(CPP_SBRS)}.sbr:
   $(CPP) $(CPP_PROJ) $<  

.cpp{$(CPP_SBRS)}.sbr:
   $(CPP) $(CPP_PROJ) $<  

.cxx{$(CPP_SBRS)}.sbr:
   $(CPP) $(CPP_PROJ) $<  

BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/fdlibm.bsc" 
BSC32_SBRS= \
	
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo
LIB32_FLAGS=/nologo /out:"$(OUTDIR)/fdlibm.lib" 
LIB32_OBJS= \
	"$(INTDIR)\e_atan2.obj" \
	"$(INTDIR)\e_pow.obj" \
	"$(INTDIR)\e_sqrt.obj" \
	"$(INTDIR)\k_standard.obj" \
	"$(INTDIR)\s_atan.obj" \
	"$(INTDIR)\s_copysign.obj" \
	"$(INTDIR)\s_fabs.obj" \
	"$(INTDIR)\s_finite.obj" \
	"$(INTDIR)\s_isnan.obj" \
	"$(INTDIR)\s_matherr.obj" \
	"$(INTDIR)\s_rint.obj" \
	"$(INTDIR)\s_scalbn.obj" \
	"$(INTDIR)\w_atan2.obj" \
	"$(INTDIR)\w_pow.obj" \
	"$(INTDIR)\w_sqrt.obj"

"$(OUTDIR)\fdlibm.lib" : "$(OUTDIR)" $(DEF_FILE) $(LIB32_OBJS)
    $(LIB32) @<<
  $(LIB32_FLAGS) $(DEF_FLAGS) $(LIB32_OBJS)
<<

!ELSEIF  "$(CFG)" == "fdlibm - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "fdlibm\Debug"
# PROP BASE Intermediate_Dir "fdlibm\Debug"
# PROP BASE Target_Dir "fdlibm"
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Target_Dir "fdlibm"
OUTDIR=.\Debug
INTDIR=.\Debug

ALL : "$(OUTDIR)\fdlibm.lib"

CLEAN : 
	-@erase "$(INTDIR)\e_atan2.obj"
	-@erase "$(INTDIR)\e_pow.obj"
	-@erase "$(INTDIR)\e_sqrt.obj"
	-@erase "$(INTDIR)\k_standard.obj"
	-@erase "$(INTDIR)\s_atan.obj"
	-@erase "$(INTDIR)\s_copysign.obj"
	-@erase "$(INTDIR)\s_fabs.obj"
	-@erase "$(INTDIR)\s_finite.obj"
	-@erase "$(INTDIR)\s_isnan.obj"
	-@erase "$(INTDIR)\s_matherr.obj"
	-@erase "$(INTDIR)\s_rint.obj"
	-@erase "$(INTDIR)\s_scalbn.obj"
	-@erase "$(INTDIR)\w_atan2.obj"
	-@erase "$(INTDIR)\w_pow.obj"
	-@erase "$(INTDIR)\w_sqrt.obj"
	-@erase "$(OUTDIR)\fdlibm.lib"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
# ADD BASE CPP /nologo /W3 /GX /Z7 /Od /D "WIN32" /D "_DEBUG" /D _X86_=1 /D "_WINDOWS" /YX /c
# ADD CPP /nologo /MDd /W3 /GX /Z7 /Od /D "_DEBUG" /D "WIN32" /D _X86_=1 /D "_WINDOWS" /D "_IEEE_LIBM" /YX /c
CPP_PROJ=/nologo /MDd /W3 /GX /Z7 /Od /D "_DEBUG" /D "WIN32" /D _X86_=1 /D "_WINDOWS" /D\
 "_IEEE_LIBM" /D "XP_WIN" -I .\ /Fp"$(INTDIR)/fdlibm.pch" /YX /Fo"$(INTDIR)/" /c 
CPP_OBJS=.\Debug/
CPP_SBRS=.\.

.c{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.cpp{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.cxx{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.c{$(CPP_SBRS)}.sbr:
   $(CPP) $(CPP_PROJ) $<  

.cpp{$(CPP_SBRS)}.sbr:
   $(CPP) $(CPP_PROJ) $<  

.cxx{$(CPP_SBRS)}.sbr:
   $(CPP) $(CPP_PROJ) $<  

BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/fdlibm.bsc" 
BSC32_SBRS= \
	
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo
LIB32_FLAGS=/nologo /out:"$(OUTDIR)/fdlibm.lib" 
LIB32_OBJS= \
	"$(INTDIR)\e_atan2.obj" \
	"$(INTDIR)\e_pow.obj" \
	"$(INTDIR)\e_sqrt.obj" \
	"$(INTDIR)\k_standard.obj" \
	"$(INTDIR)\s_atan.obj" \
	"$(INTDIR)\s_copysign.obj" \
	"$(INTDIR)\s_fabs.obj" \
	"$(INTDIR)\s_finite.obj" \
	"$(INTDIR)\s_isnan.obj" \
	"$(INTDIR)\s_matherr.obj" \
	"$(INTDIR)\s_rint.obj" \
	"$(INTDIR)\s_scalbn.obj" \
	"$(INTDIR)\w_atan2.obj" \
	"$(INTDIR)\w_pow.obj" \
	"$(INTDIR)\w_sqrt.obj"

"$(OUTDIR)\fdlibm.lib" : "$(OUTDIR)" $(DEF_FILE) $(LIB32_OBJS)
    $(LIB32) @<<
  $(LIB32_FLAGS) $(DEF_FLAGS) $(LIB32_OBJS)
<<

!ENDIF 

################################################################################
# Begin Target

# Name "js - Win32 Release"
# Name "js - Win32 Debug"

!IF  "$(CFG)" == "js - Win32 Release"

!ELSEIF  "$(CFG)" == "js - Win32 Debug"

!ENDIF 

################################################################################
# Begin Source File

SOURCE=.\jsapi.c

!IF  "$(CFG)" == "js - Win32 Release"

DEP_CPP_JSAPI=\
	".\js.msg"\
	".\jsapi.h"\
	".\jsarena.h"\
	".\jsarray.h"\
	".\jsatom.h"\
	".\jsbool.h"\
	".\jsclist.h"\
	".\jscntxt.h"\
	".\jscompat.h"\
	".\jsconfig.h"\
	".\jscpucfg.h"\
	".\jsdate.h"\
	".\jsemit.h"\
	".\jsexn.h"\
	".\jsfile.h"\
	".\jsfun.h"\
	".\jsgc.h"\
	".\jshash.h"\
	".\jsinterp.h"\
	".\jslock.h"\
	".\jslong.h"\
	".\jsmath.h"\
	".\jsnum.h"\
	".\jsobj.h"\
	".\jsopcode.h"\
	".\jsopcode.tbl"\
	".\jsosdep.h"\
	".\jsotypes.h"\
	".\jsparse.h"\
	".\jsprf.h"\
	".\jsprvtd.h"\
	".\jspubtd.h"\
	".\jsregexp.h"\
	".\jsscan.h"\
	".\jsscope.h"\
	".\jsscript.h"\
	".\jsstddef.h"\
	".\jsstr.h"\
	".\jstypes.h"\
	".\jsutil.h"\
	".\jsxml.h"\
	{$(INCLUDE)}"\sys\types.h"\
	
NODEP_CPP_JSAPI=\
	".\jsautocfg.h"\
	".\prcvar.h"\
	".\prlock.h"\
	

"$(INTDIR)\jsapi.obj" : $(SOURCE) $(DEP_CPP_JSAPI) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "js - Win32 Debug"

DEP_CPP_JSAPI=\
	".\js.msg"\
	".\jsapi.h"\
	".\jsarena.h"\
	".\jsarray.h"\
	".\jsatom.h"\
	".\jsbool.h"\
	".\jsclist.h"\
	".\jscntxt.h"\
	".\jscompat.h"\
	".\jsconfig.h"\
	".\jscpucfg.h"\
	".\jsdate.h"\
	".\jsemit.h"\
	".\jsexn.h"\
	".\jsfile.h"\
	".\jsfun.h"\
	".\jsgc.h"\
	".\jshash.h"\
	".\jsinterp.h"\
	".\jslock.h"\
	".\jslong.h"\
	".\jsmath.h"\
	".\jsnum.h"\
	".\jsobj.h"\
	".\jsopcode.h"\
	".\jsopcode.tbl"\
	".\jsosdep.h"\
	".\jsotypes.h"\
	".\jsparse.h"\
	".\jsprf.h"\
	".\jsprvtd.h"\
	".\jspubtd.h"\
	".\jsregexp.h"\
	".\jsscan.h"\
	".\jsscope.h"\
	".\jsscript.h"\
	".\jsstddef.h"\
	".\jsstr.h"\
	".\jstypes.h"\
	".\jsutil.h"\
	".\jsxml.h"\
	{$(INCLUDE)}"\sys\types.h"\
	
NODEP_CPP_JSAPI=\
	".\jsautocfg.h"\
	".\prcvar.h"\
	".\prlock.h"\
	

"$(INTDIR)\jsapi.obj" : $(SOURCE) $(DEP_CPP_JSAPI) "$(INTDIR)"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\jsarena.c

!IF  "$(CFG)" == "js - Win32 Release"

DEP_CPP_JSARE=\
	".\jsarena.h"\
	".\jsbit.h"\
	".\jscompat.h"\
	".\jscpucfg.h"\
	".\jslong.h"\
	".\jsosdep.h"\
	".\jsotypes.h"\
	".\jsstddef.h"\
	".\jstypes.h"\
	".\jsutil.h"\
	{$(INCLUDE)}"\sys\types.h"\
	
NODEP_CPP_JSARE=\
	".\jsautocfg.h"\
	

"$(INTDIR)\jsarena.obj" : $(SOURCE) $(DEP_CPP_JSARE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "js - Win32 Debug"

DEP_CPP_JSARE=\
	".\jsarena.h"\
	".\jsbit.h"\
	".\jscompat.h"\
	".\jscpucfg.h"\
	".\jslong.h"\
	".\jsosdep.h"\
	".\jsotypes.h"\
	".\jsstddef.h"\
	".\jstypes.h"\
	".\jsutil.h"\
	{$(INCLUDE)}"\sys\types.h"\
	
NODEP_CPP_JSARE=\
	".\jsautocfg.h"\
	

"$(INTDIR)\jsarena.obj" : $(SOURCE) $(DEP_CPP_JSARE) "$(INTDIR)"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\jsarray.c

!IF  "$(CFG)" == "js - Win32 Release"

DEP_CPP_JSARR=\
	".\js.msg"\
	".\jsapi.h"\
	".\jsarena.h"\
	".\jsarray.h"\
	".\jsatom.h"\
	".\jsclist.h"\
	".\jscntxt.h"\
	".\jscompat.h"\
	".\jsconfig.h"\
	".\jscpucfg.h"\
	".\jsfun.h"\
	".\jsgc.h"\
	".\jshash.h"\
	".\jsinterp.h"\
	".\jslock.h"\
	".\jslong.h"\
	".\jsnum.h"\
	".\jsobj.h"\
	".\jsosdep.h"\
	".\jsotypes.h"\
	".\jsprvtd.h"\
	".\jspubtd.h"\
	".\jsregexp.h"\
	".\jsscope.h"\
	".\jsstddef.h"\
	".\jsstr.h"\
	".\jstypes.h"\
	".\jsutil.h"\
	{$(INCLUDE)}"\sys\types.h"\
	
NODEP_CPP_JSARR=\
	".\jsautocfg.h"\
	".\prcvar.h"\
	".\prlock.h"\
	

"$(INTDIR)\jsarray.obj" : $(SOURCE) $(DEP_CPP_JSARR) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "js - Win32 Debug"

DEP_CPP_JSARR=\
	".\js.msg"\
	".\jsapi.h"\
	".\jsarena.h"\
	".\jsarray.h"\
	".\jsatom.h"\
	".\jsclist.h"\
	".\jscntxt.h"\
	".\jscompat.h"\
	".\jsconfig.h"\
	".\jscpucfg.h"\
	".\jsfun.h"\
	".\jsgc.h"\
	".\jshash.h"\
	".\jsinterp.h"\
	".\jslock.h"\
	".\jslong.h"\
	".\jsnum.h"\
	".\jsobj.h"\
	".\jsosdep.h"\
	".\jsotypes.h"\
	".\jsprvtd.h"\
	".\jspubtd.h"\
	".\jsregexp.h"\
	".\jsscope.h"\
	".\jsstddef.h"\
	".\jsstr.h"\
	".\jstypes.h"\
	".\jsutil.h"\
	{$(INCLUDE)}"\sys\types.h"\
	
NODEP_CPP_JSARR=\
	".\jsautocfg.h"\
	".\prcvar.h"\
	".\prlock.h"\
	

"$(INTDIR)\jsarray.obj" : $(SOURCE) $(DEP_CPP_JSARR) "$(INTDIR)"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\jsatom.c

!IF  "$(CFG)" == "js - Win32 Release"

DEP_CPP_JSATO=\
	".\js.msg"\
	".\jsapi.h"\
	".\jsarena.h"\
	".\jsatom.h"\
	".\jsclist.h"\
	".\jscntxt.h"\
	".\jscompat.h"\
	".\jsconfig.h"\
	".\jscpucfg.h"\
	".\jsgc.h"\
	".\jshash.h"\
	".\jsinterp.h"\
	".\jslock.h"\
	".\jslong.h"\
	".\jsnum.h"\
	".\jsobj.h"\
	".\jsopcode.h"\
	".\jsopcode.tbl"\
	".\jsosdep.h"\
	".\jsotypes.h"\
	".\jsprf.h"\
	".\jsprvtd.h"\
	".\jspubtd.h"\
	".\jsregexp.h"\
	".\jsscope.h"\
	".\jsstddef.h"\
	".\jsstr.h"\
	".\jstypes.h"\
	".\jsutil.h"\
	{$(INCLUDE)}"\sys\types.h"\
	
NODEP_CPP_JSATO=\
	".\jsautocfg.h"\
	".\prcvar.h"\
	".\prlock.h"\
	

"$(INTDIR)\jsatom.obj" : $(SOURCE) $(DEP_CPP_JSATO) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "js - Win32 Debug"

DEP_CPP_JSATO=\
	".\js.msg"\
	".\jsapi.h"\
	".\jsarena.h"\
	".\jsatom.h"\
	".\jsclist.h"\
	".\jscntxt.h"\
	".\jscompat.h"\
	".\jsconfig.h"\
	".\jscpucfg.h"\
	".\jsgc.h"\
	".\jshash.h"\
	".\jsinterp.h"\
	".\jslock.h"\
	".\jslong.h"\
	".\jsnum.h"\
	".\jsobj.h"\
	".\jsopcode.h"\
	".\jsopcode.tbl"\
	".\jsosdep.h"\
	".\jsotypes.h"\
	".\jsprf.h"\
	".\jsprvtd.h"\
	".\jspubtd.h"\
	".\jsregexp.h"\
	".\jsscope.h"\
	".\jsstddef.h"\
	".\jsstr.h"\
	".\jstypes.h"\
	".\jsutil.h"\
	{$(INCLUDE)}"\sys\types.h"\
	
NODEP_CPP_JSATO=\
	".\jsautocfg.h"\
	".\prcvar.h"\
	".\prlock.h"\
	

"$(INTDIR)\jsatom.obj" : $(SOURCE) $(DEP_CPP_JSATO) "$(INTDIR)"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\jsbool.c

!IF  "$(CFG)" == "js - Win32 Release"

DEP_CPP_JSBOO=\
	".\js.msg"\
	".\jsapi.h"\
	".\jsarena.h"\
	".\jsatom.h"\
	".\jsbool.h"\
	".\jsclist.h"\
	".\jscntxt.h"\
	".\jscompat.h"\
	".\jsconfig.h"\
	".\jscpucfg.h"\
	".\jsgc.h"\
	".\jshash.h"\
	".\jsinterp.h"\
	".\jslock.h"\
	".\jslong.h"\
	".\jsnum.h"\
	".\jsobj.h"\
	".\jsosdep.h"\
	".\jsotypes.h"\
	".\jsprf.h"\
	".\jsprvtd.h"\
	".\jspubtd.h"\
	".\jsregexp.h"\
	".\jsscope.h"\
	".\jsstddef.h"\
	".\jsstr.h"\
	".\jstypes.h"\
	".\jsutil.h"\
	{$(INCLUDE)}"\sys\types.h"\
	
NODEP_CPP_JSBOO=\
	".\jsautocfg.h"\
	".\prcvar.h"\
	".\prlock.h"\
	

"$(INTDIR)\jsbool.obj" : $(SOURCE) $(DEP_CPP_JSBOO) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "js - Win32 Debug"

DEP_CPP_JSBOO=\
	".\js.msg"\
	".\jsapi.h"\
	".\jsarena.h"\
	".\jsatom.h"\
	".\jsbool.h"\
	".\jsclist.h"\
	".\jscntxt.h"\
	".\jscompat.h"\
	".\jsconfig.h"\
	".\jscpucfg.h"\
	".\jsgc.h"\
	".\jshash.h"\
	".\jsinterp.h"\
	".\jslock.h"\
	".\jslong.h"\
	".\jsnum.h"\
	".\jsobj.h"\
	".\jsosdep.h"\
	".\jsotypes.h"\
	".\jsprf.h"\
	".\jsprvtd.h"\
	".\jspubtd.h"\
	".\jsregexp.h"\
	".\jsscope.h"\
	".\jsstddef.h"\
	".\jsstr.h"\
	".\jstypes.h"\
	".\jsutil.h"\
	{$(INCLUDE)}"\sys\types.h"\
	
NODEP_CPP_JSBOO=\
	".\jsautocfg.h"\
	".\prcvar.h"\
	".\prlock.h"\
	

"$(INTDIR)\jsbool.obj" : $(SOURCE) $(DEP_CPP_JSBOO) "$(INTDIR)"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\jscntxt.c

!IF  "$(CFG)" == "js - Win32 Release"

DEP_CPP_JSCNT=\
	".\js.msg"\
	".\jsapi.h"\
	".\jsarena.h"\
	".\jsatom.h"\
	".\jsclist.h"\
	".\jscntxt.h"\
	".\jscompat.h"\
	".\jsconfig.h"\
	".\jscpucfg.h"\
	".\jsdbgapi.h"\
	".\jsexn.h"\
	".\jsgc.h"\
	".\jshash.h"\
	".\jsinterp.h"\
	".\jslock.h"\
	".\jslong.h"\
	".\jsobj.h"\
	".\jsopcode.h"\
	".\jsopcode.tbl"\
	".\jsosdep.h"\
	".\jsotypes.h"\
	".\jsprf.h"\
	".\jsprvtd.h"\
	".\jspubtd.h"\
	".\jsregexp.h"\
	".\jsscan.h"\
	".\jsscope.h"\
	".\jsscript.h"\
	".\jsstddef.h"\
	".\jsstr.h"\
	".\jstypes.h"\
	".\jsutil.h"\
	{$(INCLUDE)}"\sys\types.h"\
	
NODEP_CPP_JSCNT=\
	".\jsautocfg.h"\
	".\prcvar.h"\
	".\prlock.h"\
	

"$(INTDIR)\jscntxt.obj" : $(SOURCE) $(DEP_CPP_JSCNT) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "js - Win32 Debug"

DEP_CPP_JSCNT=\
	".\js.msg"\
	".\jsapi.h"\
	".\jsarena.h"\
	".\jsatom.h"\
	".\jsclist.h"\
	".\jscntxt.h"\
	".\jscompat.h"\
	".\jsconfig.h"\
	".\jscpucfg.h"\
	".\jsdbgapi.h"\
	".\jsexn.h"\
	".\jsgc.h"\
	".\jshash.h"\
	".\jsinterp.h"\
	".\jslock.h"\
	".\jslong.h"\
	".\jsobj.h"\
	".\jsopcode.h"\
	".\jsopcode.tbl"\
	".\jsosdep.h"\
	".\jsotypes.h"\
	".\jsprf.h"\
	".\jsprvtd.h"\
	".\jspubtd.h"\
	".\jsregexp.h"\
	".\jsscan.h"\
	".\jsscope.h"\
	".\jsscript.h"\
	".\jsstddef.h"\
	".\jsstr.h"\
	".\jstypes.h"\
	".\jsutil.h"\
	{$(INCLUDE)}"\sys\types.h"\
	
NODEP_CPP_JSCNT=\
	".\jsautocfg.h"\
	".\prcvar.h"\
	".\prlock.h"\
	

"$(INTDIR)\jscntxt.obj" : $(SOURCE) $(DEP_CPP_JSCNT) "$(INTDIR)"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\jsdate.c

!IF  "$(CFG)" == "js - Win32 Release"

DEP_CPP_JSDAT=\
	".\js.msg"\
	".\jsapi.h"\
	".\jsarena.h"\
	".\jsatom.h"\
	".\jsclist.h"\
	".\jscntxt.h"\
	".\jscompat.h"\
	".\jsconfig.h"\
	".\jscpucfg.h"\
	".\jsdate.h"\
	".\jsdtoa.h"\
	".\jsgc.h"\
	".\jshash.h"\
	".\jsinterp.h"\
	".\jslock.h"\
	".\jslong.h"\
	".\jsnum.h"\
	".\jsobj.h"\
	".\jsosdep.h"\
	".\jsotypes.h"\
	".\jsprf.h"\
	".\jsprvtd.h"\
	".\jspubtd.h"\
	".\jsregexp.h"\
	".\jsscope.h"\
	".\jsstddef.h"\
	".\jsstr.h"\
	".\jstypes.h"\
	".\jsutil.h"\
	".\prmjtime.h"\
	{$(INCLUDE)}"\sys\types.h"\
	
NODEP_CPP_JSDAT=\
	".\jsautocfg.h"\
	".\prcvar.h"\
	".\prlock.h"\
	

"$(INTDIR)\jsdate.obj" : $(SOURCE) $(DEP_CPP_JSDAT) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "js - Win32 Debug"

DEP_CPP_JSDAT=\
	".\js.msg"\
	".\jsapi.h"\
	".\jsarena.h"\
	".\jsatom.h"\
	".\jsclist.h"\
	".\jscntxt.h"\
	".\jscompat.h"\
	".\jsconfig.h"\
	".\jscpucfg.h"\
	".\jsdate.h"\
	".\jsdtoa.h"\
	".\jsgc.h"\
	".\jshash.h"\
	".\jsinterp.h"\
	".\jslock.h"\
	".\jslong.h"\
	".\jsnum.h"\
	".\jsobj.h"\
	".\jsosdep.h"\
	".\jsotypes.h"\
	".\jsprf.h"\
	".\jsprvtd.h"\
	".\jspubtd.h"\
	".\jsregexp.h"\
	".\jsscope.h"\
	".\jsstddef.h"\
	".\jsstr.h"\
	".\jstypes.h"\
	".\jsutil.h"\
	".\prmjtime.h"\
	{$(INCLUDE)}"\sys\types.h"\
	
NODEP_CPP_JSDAT=\
	".\jsautocfg.h"\
	".\prcvar.h"\
	".\prlock.h"\
	

"$(INTDIR)\jsdate.obj" : $(SOURCE) $(DEP_CPP_JSDAT) "$(INTDIR)"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\jsdbgapi.c

!IF  "$(CFG)" == "js - Win32 Release"

DEP_CPP_JSDBG=\
	".\js.msg"\
	".\jsapi.h"\
	".\jsarena.h"\
	".\jsatom.h"\
	".\jsclist.h"\
	".\jscntxt.h"\
	".\jscompat.h"\
	".\jsconfig.h"\
	".\jscpucfg.h"\
	".\jsdbgapi.h"\
	".\jsfun.h"\
	".\jsgc.h"\
	".\jshash.h"\
	".\jsinterp.h"\
	".\jslock.h"\
	".\jslong.h"\
	".\jsobj.h"\
	".\jsopcode.h"\
	".\jsopcode.tbl"\
	".\jsosdep.h"\
	".\jsotypes.h"\
	".\jsprvtd.h"\
	".\jspubtd.h"\
	".\jsregexp.h"\
	".\jsscope.h"\
	".\jsscript.h"\
	".\jsstddef.h"\
	".\jsstr.h"\
	".\jstypes.h"\
	".\jsutil.h"\
	{$(INCLUDE)}"\sys\types.h"\
	
NODEP_CPP_JSDBG=\
	".\jsautocfg.h"\
	".\prcvar.h"\
	".\prlock.h"\
	

"$(INTDIR)\jsdbgapi.obj" : $(SOURCE) $(DEP_CPP_JSDBG) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "js - Win32 Debug"

DEP_CPP_JSDBG=\
	".\js.msg"\
	".\jsapi.h"\
	".\jsarena.h"\
	".\jsatom.h"\
	".\jsclist.h"\
	".\jscntxt.h"\
	".\jscompat.h"\
	".\jsconfig.h"\
	".\jscpucfg.h"\
	".\jsdbgapi.h"\
	".\jsfun.h"\
	".\jsgc.h"\
	".\jshash.h"\
	".\jsinterp.h"\
	".\jslock.h"\
	".\jslong.h"\
	".\jsobj.h"\
	".\jsopcode.h"\
	".\jsopcode.tbl"\
	".\jsosdep.h"\
	".\jsotypes.h"\
	".\jsprvtd.h"\
	".\jspubtd.h"\
	".\jsregexp.h"\
	".\jsscope.h"\
	".\jsscript.h"\
	".\jsstddef.h"\
	".\jsstr.h"\
	".\jstypes.h"\
	".\jsutil.h"\
	{$(INCLUDE)}"\sys\types.h"\
	
NODEP_CPP_JSDBG