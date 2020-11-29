LIBS=
ECFLG = /Zi

ifeq ($(CFG),DEBUG)
#CFLAGS=/nologo /W3 /Gm /ZI /Od /I "../src" /D "WIN32" /D "WIN32_LEAN_AND_MEAN" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /FR".\\" /Fo".\\" /Fd".\\" /FD /GZ
CFLAGS=/nologo /W3 /Gm /ZI /Od /I "../src" /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" $(DDBG) /FR".\\" /Fo".\\" /Fd".\\" /FD /GZ
FPAR=-debug:extended -nologo -c -fpp -CB -names:lowerc -align -extend_sou -warn:all -module:. -threads  -Z7 -Od -traceback -Qinline-debug-info -check:all $(DDBG) -DSTART_FROM_C -I.. -I../ppsutils_f -I. 
LFLAGS=/MTd /Zi
LPAR=orasql9.lib /nologo /subsystem:console -debug -nodefaultlib:libc.lib
else
#CFLAGS=/nologo /W3 /Od /Zp4 /D "WIN32" /D "WIN32_LEAN_AND_MEAN" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /Fo".\\" /Fd".\\"
CFLAGS=/nologo /W3 /Od /Zp4 /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" $(DDBG) /Fo".\\" /Fd".\\"
FPAR=-nologo -c -fpp -CB -names:lowerc -align -extend_source -warn:all -module:. -threads  -O3 $(DDBG) -traceback -Qinline-debug-info -I.. -I../ppsutils_f -I. 
LFLAGS=
LPAR=orasql9.lib /nologo /subsystem:console
endif

ORACLE_HOME=c:/oracle/o9g/
PROC=$(ORACLE_HOME)/bin/proc
LINK=cl
CC = cl
F90=ifort
LK=link

LIB :=C:\Program Files\Microsoft Visual Studio .NET 2003\Vc7\LIB;C:\Program Files\Microsoft Visual Studio .NET 2003/Vc7/PlatformSDK/Lib;C:\Program Files\Intel\Compiler\Fortran\9.1\IA32\lib;$(ORACLE_HOME)/precomp/lib
export LIB

Path :=C:\bin;C:\Program Files\Microsoft Visual Studio .NET 2003\Vc7\bin;C:\bin;C:\Program Files\Microsoft Visual Studio .NET 2003\Vc7\lib;c:/program files/Microsoft Visual Studio .NET 2003/Common7/IDE/;C:\Program Files\Intel\Compiler\Fortran\9.1\IA32\bin;.;c:\WINDOWS;c:\msys\1.0\bin
export Path

all: ll.exe

test.exe: ../test.c
	$(CC) $(CFLAGS) -I. -o test.exe ../test.c

myct.exe: ../myct.c
	$(CC) $(CFLAGS) -I. -o myct.exe ../myct.c

whoami.exe: ../whoami.c
	$(CC) $(CFLAGS) -I. -o whoami.exe ../whoami.c

literal.exe: ../literal.c
	$(CC) $(CFLAGS) -I. -o literal.exe ../literal.c

getcalls.exe: ../getcalls.cpp
	$(CC) $(CFLAGS) -GX -I. -o getcalls.exe ../getcalls.cpp

callgraph.exe: ../callgraph.cpp
	$(CC) $(CFLAGS) -GX -I. -o callgraph.exe ../callgraph.cpp

cleangraph.exe: ../cleangraph.cpp
	$(CC) $(CFLAGS) -GX -I. -o cleangraph.exe ../cleangraph.cpp

cleanfort.exe: ../cleanfort.cpp
	$(CC) $(CFLAGS) -GX -I. -o cleanfort.exe ../cleanfort.cpp

lbltofile.exe: ../lbltofile.cpp
	$(CC) $(CFLAGS) -GX -I. -o lbltofile.exe ../lbltofile.cpp

tosct.exe: ../tosct.cpp
	$(CC) $(CFLAGS) -GX -I. -o tosct.exe ../tosct.cpp

oracle_to_rms.exe: ../oracle_to_rms.F
	$(F90) $(FPAR) /fpp:"-I.. -p" ..\\oracle_to_rms.F /object:oracle_to_rms.o
	$(LK) oracle_to_rms.o $(LPAR) /out:oracle_to_rms.exe

editpath.exe: ../editpath.c
	$(CC) $(CFLAGS) -I. -o editpath.exe ../editpath.c

custfilt.exe: ../custfilt.c
	$(CC) $(CFLAGS) -I. -o custfilt.exe ../custfilt.c

svnfilt.exe: ../svnfilt.c
	$(CC) $(CFLAGS) -I. -o svnfilt.exe ../svnfilt.c

testre.exe: ../../util/testre.c strmre.obj
	$(CC) $(CFLAGS) -I. -o testre.exe ../../util/testre.c strmre.obj

strmre.obj: ../../util/strmre.c
	$(CC) $(CFLAGS) -I. -c ../../util/strmre.c

tprintf.exe: ../tprintf.c sprintf.obj
	$(CC) $(CFLAGS) -I. -o tprintf.exe ../tprintf.c sprintf.obj

dice.exe: ../dice.c
	$(CC) $(CFLAGS) -I. -o dice.exe ../dice.c

crumple.exe: ../crumple.c
	$(CC) $(CFLAGS) -I. -o crumple.exe ../crumple.c

colsel.exe: ../colsel.c
	$(CC) $(CFLAGS) -I. -o colsel.exe ../colsel.c

labels.exe: ../labels.c
	$(CC) $(CFLAGS) -I. -o labels.exe ../labels.c

extrhtml.exe: ../extrhtml.cpp
	$(CC) $(CFLAGS) -GX -I. -o extrhtml.exe ../extrhtml.cpp

oehtml.exe: ../oehtml.cpp
	$(CC) $(CFLAGS) -GX -I. -o oehtml.exe ../oehtml.cpp

dextrhtml.exe: ../extrhtml.cpp
	$(CC) $(CFLAGS) $(ECFLG) -GX -I. -o dextrhtml.exe ../extrhtml.cpp

hm.exe: ../hm.c
	$(CC) $(CFLAGS) -GX -I. -Fehm.exe ../hm.c

words.obj: ../words.c
	$(CC) $(CFLAGS) -I. -c ../words.c

words.exe: words.obj
	$(CC) $(LFLAGS) words.obj $(LIBS)

dateconv.obj: ../dateconv.c
	$(CC) $(CFLAGS) -I. -c ../dateconv.c

dateconv.exe: dateconv.obj
	$(CC) $(LFLAGS) dateconv.obj $(LIBS)

slugcomma.obj: ../slugcomma.c
	$(CC) $(CFLAGS) -I. -c ../slugcomma.c

slugcomma.exe: slugcomma.obj
	$(CC) $(LFLAGS) slugcomma.obj $(LIBS)

untab.exe: ../untab.c
	$(CC) $(CFLAGS) -I. ../untab.c

striphtm.exe: ../striphtm.c ssprintf.obj
	$(CC) -o striphtm.exe $(CFLAGS) -I. ../striphtm.c ssprintf.obj

hex.obj: ../../util/hex.c
	$(CC) $(CFLAGS) -I. -c ../../util/hex.c

tmp.obj: ../tmp.c
	$(CC) $(CFLAGS) -I. -c ../tmp.c

tmp.exe: tmp.obj
	$(LK) $(LFLAGS) /ST:1400 tmp, tmp, , ;

sameword.obj: ../sameword.c
	$(CC) $(CFLAGS) -I. -c ../sameword.c

sameword.exe: ../sameword.c strutil.obj
	$(CC) $(CFLAGS) -I. -o sameword.exe ../sameword.c strutil.obj

tocol.obj: ../tocol.c
	$(CC) $(CFLAGS) -I. -c ../tocol.c

tocol.exe: tocol.obj sprint3.obj strutil.obj memarch.obj
	$(LK) $(LFLAGS) /ST:1400 tocol+sprint3+strutil+memarch, tocol, , ;

srename.obj: ../srename.c
	$(CC) $(CFLAGS) -I. -c ../srename.c

srename.exe: srename.obj msdir.obj sprintf.obj
	$(LK) $(LFLAGS) /ST:1400 /out:srename.exe srename.obj msdir.obj sprintf.obj

grepinc.obj: ../grepinc.c
	$(CC) $(CFLAGS) -I. -c ../grepinc.c

grepinc.exe: grepinc.obj msexec.obj flook.obj
	$(LK) $(LFLAGS) /ST:1400 grepinc+msexec+flook, grepinc, , ;

ginc.obj: ../ginc.c
	$(CC) $(CFLAGS) -I. -c ../ginc.c

ginc.exe: ginc.obj
	$(LK) $(LFLAGS) /ST:1200 ginc, ginc, , ;

fromby.obj: ../fromby.c
	$(CC) $(CFLAGS) -I. -c ../fromby.c

fromby.exe: fromby.obj
	$(LK) $(LFLAGS) /ST:1200 fromby, fromby, , ;

touch.obj: ../touch.c
	$(CC) $(CFLAGS) -I. -c ../touch.c

touch.exe: touch.obj msdir.obj msera.obj
	$(LK) $(LFLAGS) /ST:1200 touch+msdir+msera, touch, , ;

cp.obj: ../cp.c
	$(CC) $(CFLAGS) -I. -c ../cp.c

cp.exe: cp.obj msdir.obj msera.obj
	$(CC) -o cp.exe cp.obj msdir.obj msera.obj

pm.exe: ../pm.c msdir.obj
	$(CC) $(CFLAGS) -I. -o pm.exe ../pm.c msdir.obj

sumup.obj: ../sumup.c
	$(CC) $(CFLAGS) -I. -c ../sumup.c

sumup.exe: sumup.obj
	$(LINK) $(LFLAGS) sumup.obj $(LIBS)

sct_offs.obj: ../sct_offs.c
	$(CC) $(CFLAGS) -I. -c ../sct_offs.c

sct_offs.exe: sct_offs.obj
	$(LK) $(LFLAGS) /ST:1200 sct_offs, sct_offs, , ;

unlink.obj: ../unlink.c
	$(CC) $(CFLAGS) -I. -c ../unlink.c

unlink.exe: unlink.obj
	$(LK) $(LFLAGS) /ST:1200  unlink, unlink, , ;

dirorg.obj: ../dirorg.c
	$(CC) $(CFLAGS) -I. -c ../dirorg.c

dirorg.exe: dirorg.obj
	$(LK) $(LFLAGS) /ST:1200  dirorg, dirorg, , ;

bin2hex.obj: ../bin2hex.c
	$(CC) $(CFLAGS) -I. -c ../bin2hex.c

bin2hex.exe: bin2hex.obj
	$(LINK) $(LFLAGS) bin2hex.obj $(LIBS)

dumpdisk.obj: ../dumpdisk.c
	$(CC) $(CFLAGS) -I. -c ../dumpdisk.c

dumpdisk.exe: dumpdisk.obj
	$(LK) $(LFLAGS) /ST:1200  dumpdisk, dumpdisk, , ;

tail.obj: ../tail.c
	$(CC) $(CFLAGS) -I. -c ../tail.c

tail.exe: tail.obj
	$(LK) $(LFLAGS) /ST:1200  tail, tail, , ;

hex2bin.obj: ../hex2bin.c
	$(CC) $(CFLAGS) -I. -c ../hex2bin.c

hex2bin.exe: hex2bin.obj
	$(LK) $(LFLAGS) /ST:1200  hex2bin, hex2bin, , ;

invtags.obj: ../invtags.c
	$(CC) $(CFLAGS) -I. -c ../invtags.c

invtags.exe: invtags.obj msdir.obj strutil.obj
	$(CC) -o invtags invtags.obj msdir.obj strutil.obj

chars: ../chars.c
	$(CC) $(CFLAGS) -I. -o chars ../chars.c

untab: ../untab.c
	$(CC) $(CFLAGS) -I. -o untab ../untab.c

timediff.obj: ../timediff.c
	$(CC) $(CFLAGS) -I. -c ../timediff.c

timediff.exe: timediff.obj
	$(LK) $(LFLAGS) /ST:1400  timediff, timediff, , ;

shellsrt.obj: ../../util/shellsrt.c
	$(CC) $(CFLAGS) -I. -c ../../util/shellsrt.c

shellsrt.exe: shellsrt.obj
	$(LK) $(LFLAGS) /ST:1400  shellsrt, shellsrt, , ;

clay.exe: ../clay.c
	$(CC) $(CFLAGS) -I. -oclay.exe ../clay.c

trans.exe: ../trans.c
	$(CC) $(CFLAGS) -I. /Fetrans.exe ../trans.c

tcalc.exe: ../tcalc.c
	$(CC) $(CFLAGS) -I. /Fetcalc.exe ../tcalc.c

msfname.obj: ../../common/msfname.c
	$(CC) $(CFLAGS) -I. -c ../../common/msfname.c

dostou.exe: ../dostou.c msfname.obj
	$(CC) $(CFLAGS) -I. -o dostou.exe ../dostou.c msfname.obj

sieve.exe: ../sieve.c
	$(CC) $(CFLAGS) -I. -o sieve.exe ../sieve.c

sievesql.exe: ../sievesql.c
	$(CC) $(CFLAGS) -I. -o sievesql.exe ../sievesql.c

chrad.obj: ../chrad.c
	$(CC) $(CFLAGS) -I. -c ../chrad.c

chrad.exe: chrad.obj sprintf.obj
	$(CC) chrad.obj sprintf.obj

svncone.obj: ../svncone.c
	$(CC) $(CFLAGS) -I. -c ../svncone.c

svncone.exe: svncone.obj
	$(CC) svncone.obj

uniq.obj: ../uniq.c
	$(CC) $(CFLAGS) -I. -c ../uniq.c

uniq.exe: uniq.obj msdir.obj
	$(LK) $(LFLAGS) /ST:1400  uniq+msdir, uniq, , ;

spread.obj: ../spread.c
	$(CC) $(CFLAGS) -I. -c ../spread.c

spread.exe: spread.obj sprint3.obj
	$(LK) /B /ST:1400 $(LFLAGS) spread+sprint3, spread, , ;

strmpath.obj: ../../util/strmpath.c
	$(CC) $(CFLAGS) -I. -c ../../util/strmpath.c

spath.obj: ../spath.c
	$(CC) $(CFLAGS) -I. -c ../spath.c

spath.exe: spath.obj strmpath.obj msdir.obj
	$(CC) $(LFLAGS) spath.obj strmpath.obj msdir.obj $(LIBS)

ll.obj: ../ll.c
	$(CC) $(CFLAGS) -I. -oll.obj -c ../ll.c

ll.i: ../ll.c
	$(CC) $(CFLAGS) -I. -E ../ll.c

strutil.obj: ../../util/strutil.c
	$(CC) $(CFLAGS) -ostrutil.obj -I. -c ../../util/strutil.c

re.obj: ../../util/re.c
	$(CC) $(CFLAGS) -ore.obj -I. -c ../../util/re.c

grutil.obj: ../../util/grutil.c
	$(CC) $(CFLAGS) -ogrutil.obj -I. -c ../../util/grutil.c

tad.obj: ../../util/tad.c
	$(CC) $(CFLAGS) -otad.obj -I. -c ../../util/tad.c

diffs.obj: ../../util/diffs.c
	$(CC) $(CFLAGS) -odiffs.obj -I. -c ../../util/diffs.c

map.obj: ../../util/map.c
	$(CC) $(CFLAGS) -I. -c ../../util/map.c

getline.obj: ../../util/getline.c
	$(CC) $(CFLAGS) -ogetline.obj -I. -c ../../util/getline.c

msdir.obj: ../../common/msdir.c
	$(CC) $(CFLAGS) -omsdir.obj -DMSD_ERASE=1 -I. -c ../../common/msdir.c

fip.obj: ../../common/fip.c
	$(CC) $(CFLAGS) -ofip.obj -DMSD_ERASE=1 -I. -c ../../common/fip.c

sprintf.obj: ../../common/sprintf.c
	$(CC) $(CFLAGS) -I. -c -osprintf.obj ../../common/sprintf.c

ssprintf.obj: ../../common/sprintf.c
	cp ../../common/sprintf.c ssprintf.c
	$(CC) $(CFLAGS) -ossprintf.obj -DUSE_T -I. -I../../common -c ssprintf.c

sprint3.obj: ../../common/sprintf.c
	copy ..\..\common\sprintf.c sprint3.c
	$(CC) $(CFLAGS) -DALL_32BIT=1 -I. -I../../common -c sprint3.c
	del sprint3.c

args.obj: ../../common/args.c
	$(CC) $(CFLAGS) -oargs.obj -I. -c ../../common/args.c

msexec.obj: ../../common/msexec.c
	$(CC) $(CFLAGS) -omsexec.obj -I. -c ../../common/msexec.c

msera.obj: ../../common/msera.c
	$(CC) $(CFLAGS) -omsera.obj -I. -c ../../common/msera.c

flook.obj: ../../common/flook.c
	$(CC) $(CFLAGS) -oflook.obj -I. -I../../util -c ../../common/flook.c

SOBJS = args.obj strutil.obj re.obj msdir.obj msexec.obj msera.obj ssprintf.obj diffs.obj getline.obj flook.obj tad.obj fip.obj grutil.obj

ll.exe: ll.obj strutil.obj args.obj re.obj msdir.obj msexec.obj msera.obj ssprintf.obj diffs.obj getline.obj tad.obj flook.obj fip.obj grutil.obj
#	$(CC) -GX -I. -o ll.exe ../ll.c $(SOBJS) $(LIBS)
	$(LINK) $(LFLAGS) ll.obj $(SOBJS) $(LIBS)

docrypt.obj: ../docrypt.c
	$(CC) $(CFLAGS) -I. -c ../docrypt.c

crypt.obj: ../../common/crypt.c
	$(CC) $(CFLAGS) -I. -c ../../common/crypt.c

docrypt.exe: docrypt.obj crypt.obj msdir.obj sprintf.obj
	$(LK) $(LFLAGS) /ST:2000  docrypt+crypt+msdir+sprintf, docrypt, , ;



tree.obj: ../tree.cpp
	$(CC) $(CFLAGS) -I. -c ../tree.cpp

tree.exe: tree.obj
	$(LK) $(LFLAGS) /ST:2000  /nologo /subsystem:console tree.obj kernel32.lib smsapi.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib  



CLEAN :
	rm -f *.obj


