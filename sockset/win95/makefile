CC = cl
LK = link
CFLAGS = /Zi /AL
LFLAGS = /B /NOI /DEBUG
#CFLAGS = /Gs
#LFLAGS = /B /NOI

!IF "$(CFG)" == ""
CFG=sockset - Win32 Debug
!MESSAGE No configuration specified. Defaulting to sockset - Win32 Debug.
!ENDIF 

!IF  "$(CFG)" == "sockset - Win32 Release" || "$(CFG)" == "rel"
CPP_PROJ=/nologo /ML /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /Fp"$(INTDIR)\sockset.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib wsock32.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /incremental:no /pdb:"$(OUTDIR)\sockset.pdb" /machine:I386 /out:"$(OUTDIR)\sockset.exe" 
!ELSE
CPP_PROJ=/nologo /MLd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /Fp"$(INTDIR)\sockset.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib wsock32.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /incremental:yes /pdb:"$(OUTDIR)\sockset.pdb" /debug /machine:I386 /out:"$(OUTDIR)\sockset.exe" /pdbtype:sept 
!ENDIF 

INTDIR = .
OUTDIR = .


all: socclnt.exe 


strutil.obj: ../../util/strutil.c
	$(CC) $(CPP_PROJ) -I. -c -o strutil.obj ../../util/strutil.c

soctest.obj: ../soctest.cpp
	$(CC) $(CPP_PROJ) -I. -c ../soctest.cpp

socclnt.obj: ../socclnt.cpp
	$(CC) $(CPP_PROJ) -I. -c ../socclnt.cpp

socserv.obj: ../socserv.c
	$(CC) $(CPP_PROJ) -I. -c ../socserv.c

socclnt.exe: socclnt.obj strutil.obj
	$(LK) $(LINK32_FLAGS) socclnt.obj strutil.obj

socserv.exe: socserv.obj strutil.obj
	$(LK) $(LINK32_FLAGS) socserv.obj strutil.obj

soctest.exe: soctest.obj strutil.obj
	$(LK) $(LINK32_FLAGS) soctest.obj strutil.obj


test.obj: ../test.cpp
	$(CC) $(CPP_PROJ) -I. -c ../test.cpp

test.exe: test.obj
	$(LK) test.obj


clean:
	del *.obj
	del *.exe
	del *.pdb
	del *.pch
	del *.ilk
	del *.idb

