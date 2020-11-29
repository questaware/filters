CC = \gener\c600\bin\cl
LK = \gener\c600\bin\link
#CFLAGS = /Zi /AL
#LFLAGS = /B /NOI /CODEV
CFLAGS = /AL /Ocegilsa /Gs
LFLAGS = /B /NOI
SCFLAGS = /Zi /AL
SLFLAGS = /B /NOI /CODEV

all: stree.exe

test.exe: ../test.c
	$(CC) $(CFLAGS) -I. -o test.exe ../test.c

longdemo.exe: longdemo.c
	$(CC) $(CFLAGS) -I. -o longdemo.exe longdemo.c

testre.exe: ../../util/testre.c strmre.obj
	$(CC) $(CFLAGS) -I. -o testre.exe ../../util/testre.c strmre.obj

strmre.obj: ../../util/strmre.c
	$(CC) $(CFLAGS) -I. -c ../../util/strmre.c

tprintf.exe: ../tprintf.c sprintf.obj
	$(CC) $(CFLAGS) -I. -o tprintf.exe ../tprintf.c sprintf.obj

untab.exe: ../untab.c
	$(CC) $(CFLAGS) -I. -o untab.exe ../untab.c

crumple.exe: ../crumple.c
	$(CC) $(CFLAGS) -I. -o crumple.exe ../crumple.c

words.exe: ../words.c
	$(CC) $(CFLAGS) -I. -o words.exe ../words.c

hex.obj: ../../util/hex.c
	$(CC) $(CFLAGS) -I. -c ../../util/hex.c

tmp.obj: ../tmp.c
	$(CC) $(CFLAGS) -I. -c ../tmp.c

tmp.exe: tmp.obj
	$(LK) $(LFLAGS) /ST:1400 tmp, tmp, , ;

printabl.obj: ../printabl.c
	$(CC) $(CFLAGS) -I. -c ../printabl.c

printabl.exe: printabl.obj
	$(LK) $(LFLAGS) /ST:1400 printabl, printabl, , ;

sameword.obj: ../sameword.c
	$(CC) $(CFLAGS) -I. -c ../sameword.c

sameword.exe: sameword.obj strutil.obj
	$(LK) $(LFLAGS) /ST:1400 sameword+strutil, sameword, , ;

striphtm.obj: ../striphtm.c
	$(CC) $(CFLAGS) -I. -c ../striphtm.c

striphtm.exe: striphtm.obj strutil.obj
	$(LK) $(LFLAGS) /ST:1400 striphtm+strutil, striphtm, , ;

tocol.obj: ../tocol.c
	$(CC) $(CFLAGS) -I. -c ../tocol.c

tocol.exe: tocol.obj sprint3.obj strutil.obj memarch.obj
	$(LK) $(LFLAGS) /ST:1400 tocol+sprint3+strutil+memarch, tocol, , ;

srename.obj: ../srename.c
	$(CC) $(CFLAGS) -I. -c ../srename.c

srename.exe: srename.obj msdir.obj sprintf.obj
	$(LK) $(LFLAGS) /ST:1400 srename+msdir+sprintf, srename, , ;

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

touch.exe: touch.obj ssdir.obj
	$(LK) $(LFLAGS) /ST:1200 touch+ssdir, touch, , ;

sumup.obj: ../sumup.c
	$(CC) $(CFLAGS) -I. -c ../sumup.c

sumup.exe: sumup.obj
	$(LK) $(LFLAGS) /ST:1200 sumup, sumup, , ;

sct_offs.obj: ../sct_offs.c
	$(CC) $(CFLAGS) -I. -c ../sct_offs.c

sct_offs.exe: sct_offs.obj
	$(LK) $(LFLAGS) /ST:1200 sct_offs, sct_offs, , ;

unlink.obj: ../unlink.c
	$(CC) $(CFLAGS) -I. -c ../unlink.c

unlink.exe: unlink.obj
	$(LK) $(LFLAGS) /ST:1200  unlink, unlink, , ;

ln.obj: ../ln.c
	$(CC) $(CFLAGS) -I. -c ../ln.c

ln.exe: ln.obj
	$(LK) $(LFLAGS) /ST:1200  ln, ln, , ;

dirorg.obj: ../dirorg.c
	$(CC) $(CFLAGS) -I. -c ../dirorg.c

dirorg.exe: dirorg.obj
	$(LK) $(LFLAGS) /ST:1200  dirorg, dirorg, , ;

bin2hex.obj: ../bin2hex.c
	$(CC) $(CFLAGS) -I. -c ../bin2hex.c

bin2hex.exe: bin2hex.obj
	$(LK) $(LFLAGS) /ST:1200  bin2hex, bin2hex, , ;

tailmf.obj: ../tailmf.c
	$(CC) $(CFLAGS) -I. -c ../tailmf.c

tailmf.exe: tailmf.obj
	$(LK) $(LFLAGS) /ST:1200  tailmf, tailmf, , ;

slowcat.obj: ../slowcat.c
	$(CC) $(CFLAGS) -I. -c ../slowcat.c

slowcat.exe: slowcat.obj
	$(LK) $(LFLAGS)  slowcat, slowcat, , ;

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

invtags.exe: invtags.obj ssdir.obj strutil.obj
	$(LK) $(LFLAGS) /ST:1400  invtags+ssdir+strutil, invtags, , ;

chars: ../chars.c
	$(CC) $(CFLAGS) -I. -o chars ../chars.c

untab: ../untab.c
	$(CC) $(CFLAGS) -I. -o untab ../untab.c

timediff.obj: ../timediff.c
	$(CC) $(CFLAGS) -I. -c ../timediff.c

timediff.exe: timediff.obj ssdir.obj
	$(LK) $(LFLAGS) /ST:1400  timediff+ssdir, timediff, , ;

filter.obj: ../filter.c
	$(CC) $(CFLAGS) -I. -c ../filter.c

filter.exe: filter.obj
	$(LK) $(LFLAGS) /ST:1400  filter, filter, , ;

shellsrt.obj: ../../util/shellsrt.c
	$(CC) $(CFLAGS) -I. -c ../../util/shellsrt.c

shellsrt.exe: shellsrt.obj
	$(LK) $(LFLAGS) /ST:1400  shellsrt, shellsrt, , ;

clay.obj: ../clay.c
	$(CC) $(CFLAGS) -I. -c ../clay.c

clay.exe: clay.obj
	$(LK) $(LFLAGS) /ST:1400  clay, clay, , ;

trans.obj: ../trans.c
	$(CC) $(CFLAGS) -I. -c ../trans.c

trans.exe: trans.obj
	$(LK) $(LFLAGS) /ST:1400  trans, trans, , ;

msfname.obj: ../../common/msfname.c
	$(CC) $(CFLAGS) -I. -c ../../common/msfname.c

dostou.exe: ../dostou.c msfname.obj
	$(CC) $(CFLAGS) -I. -o dostou.exe ../dostou.c msfname.obj

sieve.exe: ../sieve.c
	$(CC) $(CFLAGS) -I. -o sieve.exe ../sieve.c

chrad.obj: ../chrad.c sprint3.obj
	$(CC) $(CFLAGS) -I. -c ../chrad.c

chrad.exe: chrad.obj sprintf.obj
	$(LK) $(LFLAGS) /ST:1000  chrad+sprint3, chrad, , ;

uniq.obj: ../uniq.c
	$(CC) $(CFLAGS) -I. -c ../uniq.c

uniq.exe: uniq.obj ssdir.obj
	$(LK) $(LFLAGS) /ST:1400  uniq+ssdir, uniq, , ;

spread.obj: ../spread.c
	$(CC) $(CFLAGS) -I. -c ../spread.c

spread.exe: spread.obj sprint3.obj
	$(LK) /B /ST:1400 $(LFLAGS) spread+sprint3, spread, , ;

strmpath.obj: ../../util/strmpath.c
	$(CC) $(CFLAGS) -I. -c ../../util/strmpath.c

spath.obj: ../spath.c
	$(CC) $(CFLAGS) -I. -c ../spath.c

spath.exe: spath.obj strmpath.obj msdir.obj
	$(LK) /B /ST:1400 $(LFLAGS) spath+strmpath+msdir, spath, , ;

stree.obj: ../stree.c
	$(CC) $(CFLAGS) -I. -c ../stree.c

stree.i: ../stree.c
	$(CC) $(CFLAGS) -I. -E ../stree.c

stree.exe: stree.obj strutil.obj args.obj re.obj msdir.obj msexec.obj msera.obj ssprintf.obj diffs.obj getline.obj tad.obj flook.obj fip.obj grutil.obj
	$(LK) $(LFLAGS) /ST:1200  stree+strutil+args+re+msdir+msexec+msera+ssprintf+diffs+getline+flook+tad+fip+grutil, stree, , ;

strutil.obj: ../../util/strutil.c
	$(CC) $(CFLAGS) -I. -c -o strutil.obj ../../util/strutil.c

re.obj: ../../util/re.c
	$(CC) $(CFLAGS) -I. -c -o re.obj ../../util/re.c

grutil.obj: ../../util/grutil.c
	$(CC) $(CFLAGS) -I. -c -o grutil.obj ../../util/grutil.c

tad.obj: ../../util/tad.c
	$(CC) $(CFLAGS) -I. -c -o tad.obj ../../util/tad.c

diffs.obj: ../../util/diffs.c
	$(CC) $(CFLAGS) -I. -c -o diffs.obj ../../util/diffs.c

map.obj: ../../util/map.c
	$(CC) $(CFLAGS) -I. -c -o map.obj ../../util/map.c

getline.obj: ../../util/getline.c
	$(CC) $(CFLAGS) -I. -c -o getline.obj ../../util/getline.c

msdir.obj: ../../common/msdir.c
	$(CC) $(CFLAGS) /D MSD_ERASE=1 /D NOPUSHPOP=0 -I. -c ../../common/msdir.c

ssdir.obj: ../../common/msdir.c
	copy ..\..\common\msdir.c ssdir.c
	$(CC) $(CFLAGS) -I../../common /D MSD_ERASE=1 /D NOPUSHPOP=1 -I. -c ssdir.c

fip.obj: ../../common/fip.c
	$(CC) $(CFLAGS) /D MSD_ERASE=1 -I. -c ../../common/fip.c

sprintf.obj: ../../common/sprintf.c
	$(CC) $(CFLAGS) -I. -c -o sprintf.obj ../../common/sprintf.c

ssprintf.obj: ../../common/sprintf.c
	copy ..\..\common\sprintf.c ssprintf.c
	$(CC) $(CFLAGS) /D USE_T -I. -I../../common -c ssprintf.c

sprint3.obj: ../../common/sprintf.c
	copy ..\..\common\sprintf.c sprint3.c
	$(CC) $(CFLAGS) /D ALL_32BIT=1 -I. -I../../common -c sprint3.c
	del sprint3.c

args.obj: ../../common/args.c
	$(CC) $(CFLAGS) -I. -c ../../common/args.c

msexec.obj: ../../common/msexec.c
	$(CC) $(CFLAGS) -I. -c ../../common/msexec.c

msera.obj: ../../common/msera.c
	$(CC) $(CFLAGS) -I. -c ../../common/msera.c

flook.obj: ../../common/flook.c
	$(CC) $(CFLAGS) -I. -I../../util -c ../../common/flook.c

docrypt.obj: ../docrypt.c
	$(CC) $(CFLAGS) -I. -c ../docrypt.c

crypt.obj: ../../common/crypt.c
	$(CC) $(CFLAGS) -I. -c ../../common/crypt.c

docrypt.exe: docrypt.obj crypt.obj ssdir.obj sprintf.obj
	$(LK) $(LFLAGS) /ST:2000  docrypt+crypt+ssdir+sprintf, docrypt, , ;


CLEAN :
	rm -f *.obj


