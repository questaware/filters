ifndef C_FLG
C_FLG=/ML
endif

VPATH=$(HERE)/../../util  $(HERE)/../../common $(HERE)/../../prog

ifndef C_HOME
#C_HOME=C:/Program Files/Microsoft Visual Studio .NET 2003/VC7
C_HOME=C:/Program Files (x86)/Microsoft Visual Studio 14.0/VC
endif
ifndef C_KIT
C_KIT=C:/Program Files (x86)/Windows Kits
endif

#INCLUDE=$(C_HOME)/ATLMFC/INCLUDE;$(C_HOME)/INCLUDE;$(C_HOME)/PlatformSDK/include/prerelease;$(C_HOME)/PlatformSDK/include;$(C_HOME)/../SDK/v1.1/include;$(C_HOME)/../SDK/v1.1/include/
INCLUDE=$(C_HOME)\INCLUDE;$(C_HOME)\ATLMFC\INCLUDE;$(C_KIT)\10\include\10.0.10150.0\ucrt;$(C_KIT)\NETFXSDK\4.6\include\um;$(C_KIT)\8.1\include\shared;$(C_KIT)\8.1\include\um;$(C_KIT)\8.1\include\winrt;
export INCLUDE

LBC=$(C_HOME)/LIB
SDKLIB=$(LBC);$(C_HOME)/PlatformSDK/Lib
LIB=$(LBC)
LIB=$(LBC);$(C_KIT)/8.1/Lib/winv6.3/um/x86;$(C_KIT)/10/Lib/10.0.10150.0/ucrt/x86
export LIB

Path :=c:/bin;$(C_HOME)/bin;$(C_HOME)/lib;$(C_HOME)/../Common/msdev98/bin/;\
export Path

ifneq ($(DEBUG),1)
#CFLAGS=/nologo /W3 /Gm /ZI /Od /D "WIN32" /D "WIN32_LEAN_AND_MEAN" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /FR".\\" /Fo".\\" /Fd".\\" /FD /GZ
#CFLAGS=$(MM) /nologo /W3 /Gm /ZI /Od /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" $(DDBG) /FR".\\" /Fo".\\" /Fd".\\" /FD /GZ
CFLAGS=$(MM) /nologo /W3 /O1 /Os /Og /Oy /GF $(C_FLG) /D "WIN32" /D "WIN32_LEAN_AND_MEAN" /D "NDEBUG" /D "_CONSOLE"
FPAR=-debug:extended -nologo -c -fpp -CB -names:lowerc -align -extend_sou -warn:all -module:. -threads  -Z7 -Od -traceback -Qinline-debug-info -check:all $(DDBG) -DSTART_FROM_C -I.. -I../ppsutils_f -I.
LFLAGS=
LPAR=orasql9.lib /nologo /subsystem:console -debug -nodefaultlib:libc.lib
else
#CFLAGS=/nologo /W3 /Od /Zp4 /D "WIN32" /D "WIN32_LEAN_AND_MEAN" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /Fo".\\" /Fd".\\"
#CFLAGS=$(MM) /nologo /W3 /Od /Zp4 /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" $(DDBG) /Fo".\\" /Fd".\\"
CFLAGS=$(MM) /nologo /YX /Yd /W3 /Zi /MDd /Od /D WIN32_LEAN_AND_MEAN /D "_DEBUG" /D "_CONSOLE" /D "_MBCS"
FPAR=-nologo -c -fpp -CB -names:lowerc -align -extend_source -warn:all -module:. -threads  -O3 $(DDBG) -traceback -Qinline-debug-info -I.. -I../../ppsutils_f -I. 
LFLAGS= /nologo /debug
LPAR=orasql9.lib /nologo /subsystem:console
endif

ORACLE_HOME=c:/oracle/o9g/
PROC=$(ORACLE_HOME)/bin/proc
LINK=cl
CC = cl
F90=ifort
LK=link

.SUFFIXES: .c .obj .asm

#.c.obj :
#	echo one $<
#	$(CC) -I../ -I../../common/ $(CFLAGS) $< 

%.obj : %.c
	echo two $<
	$(CC) -I../ -I../../common/ $(CFLAGS) -c $< 

%.obj : %.cpp
	$(CC) -I../ -I../../common/ $(CFLAGS) -c $< 

all: ffg.exe chrad.exe

ffg.i: ../ffg.c
#	$(CC) $(CFLAGS) -I.. -E ../../ffg.c

FFGO=strutil.obj args.obj re.obj msdir.obj msexec.obj msera.obj \
ssprintf.obj diffs.obj getline.obj tad.obj \
flook.obj fip.obj grutil.obj

ffg.exe: $(FFGO) ffg.obj
	$(LK) $(LFLAGS) ffg.obj $(FFGO) $(LIBS)
#	$(LINK32) /NODEFAULTLIB $(LINK32_FLAGS)ffg.exe ffg.obj $(FFGO) 

#chrad.obj: ../chrad.c
#	$(CC) $(CFLAGS) -I.. -c ../chrad.c

chrad.exe: chrad.obj sprintf.obj
	$(CC) chrad.obj sprintf.obj

test.exe: ../test.c
	$(CC) $(CFLAGS) -I.. -o test.exe ../test.c

#myct.exe: ../myct.c
#	$(CC) $(CFLAGS) -I.. -o myct.exe ../myct.c

whoami.exe: ../../whoami.c
	$(CC) $(CFLAGS) -I.. -o whoami.exe ../../whoami.c

literal.exe: ../../literal.c
	$(CC) $(CFLAGS) -I.. -o literal.exe ../../literal.c

getcalls.exe: ../../getcalls.cpp
	$(CC) $(CFLAGS) -GX -I.. -o getcalls.exe ../../getcalls.cpp

callgraph.exe: ../../callgraph.cpp
	$(CC) $(CFLAGS) -GX -I.. -o callgraph.exe ../../callgraph.cpp

cleangraph.exe: ../../cleangraph.cpp
	$(CC) $(CFLAGS) -GX -I.. -o cleangraph.exe ../../cleangraph.cpp

cleanfort.exe: ../../cleanfort.cpp
	$(CC) $(CFLAGS) -GX -I.. -o cleanfort.exe ../../cleanfort.cpp

lbltofile.exe: ../../lbltofile.cpp
	$(CC) $(CFLAGS) -GX -I.. -o lbltofile.exe ../../lbltofile.cpp

tosct.exe: ../../tosct.cpp
	$(CC) $(CFLAGS) -GX -I.. -o tosct.exe ../../tosct.cpp

oracle_to_rms.exe: ../../oracle_to_rms.F
	$(F90) $(FPAR) /fpp:"-I.. -p" ..\\oracle_to_rms.F /object:oracle_to_rms.o
	$(LK) oracle_to_rms.o $(LPAR) /out:oracle_to_rms.exe

editpath.exe: ../../editpath.c
	$(CC) $(CFLAGS) -I.. -o editpath.exe ../../editpath.c

custfilt.exe: ../../custfilt.c
	$(CC) $(CFLAGS) -I.. -o custfilt.exe ../../custfilt.c

testre.exe: ../../../util/testre.c strmre.obj
	$(CC) $(CFLAGS) -I.. -o testre.exe ../../../util/testre.c strmre.obj

tprintf.exe: ../tprintf.c sprintf.obj
	$(CC) $(CFLAGS) -I.. -o tprintf.exe ../tprintf.c sprintf.obj

dice.exe: ../../dice.c
	$(CC) $(CFLAGS) -I.. -o dice.exe ../../dice.c

crumple.exe: ../../crumple.c
	$(CC) $(CFLAGS) -I.. -o crumple.exe ../../crumple.c

colsel.exe: ../../colsel.c
	$(CC) $(CFLAGS) -I.. -o colsel.exe ../../colsel.c

labels.exe: ../../labels.c
	$(CC) $(CFLAGS) -I.. -o labels.exe ../../labels.c

extrhtml.exe: ../../extrhtml.cpp
	$(CC) $(CFLAGS) -GX -I.. -o extrhtml.exe ../../extrhtml.cpp

oehtml.exe: ../../oehtml.cpp
	$(CC) $(CFLAGS) -GX -I.. -o oehtml.exe ../../oehtml.cpp

dextrhtml.exe: ../../extrhtml.cpp
	$(CC) $(CFLAGS) $(ECFLG) -GX -I.. -o dextrhtml.exe ../../extrhtml.cpp

hm.exe: ../../hm.c
	$(CC) $(CFLAGS) -GX -I.. -Fehm.exe ../../hm.c

words.obj: ../../words.c
	$(CC) $(CFLAGS) -I.. -c ../../words.c

words.exe: words.obj
	$(CC) $(LFLAGS) words.obj $(LIBS)

dateconv.obj: ../../dateconv.c
	$(CC) $(CFLAGS) -I.. -c ../../dateconv.c

dateconv.exe: dateconv.obj
	$(CC) $(LFLAGS) dateconv.obj $(LIBS)

slugcomma.obj: ../../slugcomma.c
	$(CC) $(CFLAGS) -I.. -c ../../slugcomma.c

slugcomma.exe: slugcomma.obj
	$(CC) $(LFLAGS) slugcomma.obj $(LIBS)

untab.exe: ../../untab.c
	$(CC) $(CFLAGS) -I.. ../../untab.c

striphtm.exe: ../../striphtm.c ssprintf.obj
	$(CC) -o striphtm.exe $(CFLAGS) -I.. ../../striphtm.c ssprintf.obj

hex.obj: ../../../../util/hex.c
	$(CC) $(CFLAGS) -I.. -c ../../util/hex.c

tmp.obj: ../tmp.c
	$(CC) $(CFLAGS) -I.. -c ../tmp.c

tmp.exe: tmp.obj
	$(LK) $(LFLAGS) /ST:1400 tmp, tmp, , ;

sameword.obj: ../../sameword.c
	$(CC) $(CFLAGS) -I.. -c ../../sameword.c

sameword.exe: ../../sameword.c strutil.obj
	$(CC) $(CFLAGS) -I.. -o sameword.exe ../../sameword.c strutil.obj

tocol.obj: ../../tocol.c
	$(CC) $(CFLAGS) -I.. -c ../../tocol.c

tocol.exe: tocol.obj sprint3.obj strutil.obj memarch.obj
	$(LK) $(LFLAGS) /ST:1400 tocol.obj sprint3.obj strutil.obj memarch.obj $(LIBS)

srename.obj: ../../srename.c
	$(CC) $(CFLAGS) -I.. -c ../../srename.c

srename.exe: srename.obj msdir.obj sprintf.obj
	$(LK) $(LFLAGS) /ST:1400 /out:srename.exe srename.obj msdir.obj sprintf.obj

grepinc.obj: ../../grepinc.c
	$(CC) $(CFLAGS) -I.. -c ../../grepinc.c

grepinc.exe: 
	$(LK) $(LFLAGS) /ST:1400 grepinc.obj msexec.obj flook.obj $(LIBS)

ginc.exe: ginc.obj
	$(LK) $(LFLAGS) /ST:1200 ginc, ginc, , ;

fromby.obj: ../../fromby.c
	$(CC) $(CFLAGS) -I.. -c ../../fromby.c

fromby.exe: fromby.obj
	$(LK) $(LFLAGS) /ST:1200 fromby, fromby, , ;

touch.exe: touch.obj msdir.obj msera.obj
	$(LK) $(LFLAGS) /ST:1200 touch.obj msdir.obj msera.obj $(LIBS)

cp.exe: cp.obj msdir.obj msera.obj
	$(CC) -o cp.exe cp.obj msdir.obj msera.obj

pm.exe: ../../pm.c msdir.obj
	$(CC) $(CFLAGS) -I.. -o pm.exe ../../pm.c msdir.obj

sumup.exe: sumup.obj
	$(LINK32) $(LFLAGS) sumup.obj $(LIBS)

sct_offs.exe: sct_offs.obj
	$(LK) $(LFLAGS) /ST:1200 sct_offs, sct_offs, , ;

unlink.exe: unlink.obj
	$(LK) $(LFLAGS) /ST:1200  unlink, unlink, , ;

dirorg.exe: dirorg.obj
	$(LK) $(LFLAGS) /ST:1200  dirorg, dirorg, , ;

bin2hex.exe: bin2hex.obj
	$(LINK32) $(LFLAGS) bin2hex.obj $(LIBS)

dumpdisk.exe: dumpdisk.obj
	$(LK) $(LFLAGS) /ST:1200  dumpdisk, dumpdisk, , ;

tail.exe: tail.obj
	$(LK) $(LFLAGS) /ST:1200  tail, tail, , ;

hex2bin.exe: hex2bin.obj
	$(LK) $(LFLAGS) /ST:1200  hex2bin, hex2bin, , ;

invtags.exe: invtags.obj msdir.obj strutil.obj
	$(CC) -o invtags invtags.obj msdir.obj strutil.obj

untab: ../../untab.c
	$(CC) $(CFLAGS) -I.. -o untab ../../untab.c

timediff.exe: timediff.obj
	$(LK) $(LFLAGS) /ST:1400  timediff, timediff, , ;

shellsrt.exe: shellsrt.obj
	$(LK) $(LFLAGS) /ST:1400  shellsrt, shellsrt, , ;

clay.exe: ../../clay.c
	$(CC) $(CFLAGS) -I.. -oclay.exe ../../clay.c

trans.exe: ../../trans.c
	$(CC) $(CFLAGS) -I.. /Fetrans.exe ../../trans.c

tcalc.exe: ../../tcalc.c
	$(CC) $(CFLAGS) -I.. /Fetcalc.exe ../../tcalc.c

dostou.exe: ../dostou.c msfname.obj
	$(CC) $(CFLAGS) -I.. -o dostou.exe ../dostou.c msfname.obj

sieve.exe: ../../sieve.c
	$(CC) $(CFLAGS) -I.. -o sieve.exe ../../sieve.c

sievesql.exe: ../../sievesql.c
	$(CC) $(CFLAGS) -I.. -o sievesql.exe ../../sievesql.c

svncone.exe: svncone.obj
	$(CC) svncone.obj

uniq.exe: uniq.obj msdir.obj
	$(LINK32) $(LFLAGS) uniq.obj msdir.obj $(LIBS)

spread.exe: spread.obj sprint3.obj
	$(LK) /B /ST:1400 $(LFLAGS) spread.obj sprint3.obj $(LIBS)

spath.exe: spath.obj strmpath.obj msdir.obj
	$(CC) $(LFLAGS) spath.obj strmpath.obj msdir.obj $(LIBS)

flook.obj: ../../../common/flook.c
	$(CC) $(CFLAGS) -oflook.obj -I.. -I../../../util -c ../../../common/flook.c

msdir.obj: ../../../common/msdir.c
	$(CC) $(CFLAGS) -omsdir.obj -DMSD_ERASE=1 -I.. -c ../../../common/msdir.c

fip.obj: ../../../common/fip.c
	$(CC) $(CFLAGS) -ofip.obj -DMSD_ERASE=1 -I.. -c ../../../common/fip.c

sprintf.obj: ../../../common/sprintf.c
	$(CC) $(CFLAGS) -I.. -c -osprintf.obj ../../../common/sprintf.c

ssprintf.obj: ../../../common/sprintf.c
	type ..\..\..\common\sprintf.c > ssprintf.c
	$(CC) $(CFLAGS) -ossprintf.obj -DUSE_T -I.. -I../../../common -c ssprintf.c

sprint3.obj: ../../common/sprintf.c
	type ..\..\common\sprintf.c > sprint3.c
	$(CC) $(CFLAGS) -DALL_32BIT=1 -I.. -I../../common -c sprint3.c
	del sprint3.c

docrypt.obj: ../../docrypt.c
	$(CC) $(CFLAGS) -I.. -c ../../docrypt.c

docrypt.exe: docrypt.obj crypt.obj msdir.obj sprintf.obj
	$(LK) $(LFLAGS) /ST:2000 docrypt.obj crypt.obj msdir.obj sprintf.obj $(LIBS)

tree.exe: tree.obj
	$(LK) $(LFLAGS) /ST:2000 /nologo /subsystem:console tree.obj kernel32.lib smsapi.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib

clean :
	-del *.obj
	-del *.pch
	-del *.pdb
	-del *.ilk
	-del *.exe

.c.asm :
	$(CC) -I../ -I../../common/ /Fait.asm $(CPP_PROJ) $< 

.cpp.asm :
	$(CC) -I../ -I../../common/ /Fait.asm $(CPP_PROJ) $< 

inc :
	rem $(INCLUDE)
