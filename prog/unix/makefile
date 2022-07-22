CC=gcc
LK=gcc
CPP=CC -I/opt/STLport-3.12.3/stl/
#CC=c89
#LK=c89
CFLAGS = -g -D_INCLUDE_HPUX_SOURCE -D_INCLUDE_POSIX_SOURCE -DINCLUDE_POSIX4_SOURCE -D_INCLUDE_XOPEN_SOURCE -D_STDARG_INCLUDED -Dhp -I. -I../$(SRC)
LFLAGS=-g -xildoff
LFLAGS=-g
#LFLAGS= -g
SRC=src
USRC=unix
#CFLAGS= -g -nostdinc -I/usr/include -I.
MINCFLAGS= -g -I.
#CFLAGS= -O -D_INCLUDE_HPUX_SOURCE -D_INCLUDE_POSIX_SOURCE -DINCLUDE_POSIX4_SOURCE -D_INCLUDE_XOPEN_SOURCE -D_STDARG_INCLUDED -Dhp -I. -I../$(SRC)
#LFLAGS=-O

all: ffg chrad

ffg.i: ../ffg.c
	$(CC) $(CFLAGS) -S ../ffg.c

ffg.o: ../ffg.c 
	$(CC) -c $(CFLAGS) ../ffg.c

FFGOBJS=args.o strutil.o re.o msdir.o msera.o ssprintf.o diffs.o getline.o tad.o flook.o fip.o grutil.o
ffg: ../ffg.c $(FFGOBJS)
	sed -e 's/^/"/' -e 's?$$?\\n"?' ../ffghlp.txt > helptxt.h
	$(CC) $(CFLAGS) $(LFLAGS) -o ffg ../ffg.c $(FFGOBJS)

hello: ../hello.c
	$(CC) $(CFLAGS) -o hello ../hello.c strutil.o re.o msdir.o msera.o sprintf.o diffs.o getline.o tad.o flook.o fip.o grutil.o

test: ../test.c
	c89  -o test ../test.c

words: ../words.c
	$(CC) $(CFLAGS) -o words ../words.c

inc: ../inc.cpp
	$(CPP) $(CFLAGS) -o inc ../inc.cpp

printabl: ../printabl.c
	$(CC) $(CFLAGS) -o printabl ../printabl.c

tcalc: ../tcalc.c
	$(CC) $(CFLAGS) -o tcalc ../tcalc.c

crumple: ../crumple.c
	$(CC) $(CFLAGS) -o crumple ../crumple.c

mycomm: ../mycomm.c
	$(CC) $(CFLAGS) -o mycomm ../mycomm.c

tprintf: ../tprintf.c sprintf.o
	$(CC) $(CFLAGS) -o tprintf ../tprintf.c sprintf.o

hex.o: ../../util/hex.c
	$(CC) $(CFLAGS) -c ../../util/hex.c

tmp: ../tmp.c
	$(CC) $(CFLAGS) ../tmp.c

grepinc.o: ../grepinc.c
	$(CC) $(CFLAGS) -c ../grepinc.c

grepinc: grepinc.o flook.o sprintf.o strutil.o
	$(LK) $(LFLAGS) -o grepinc grepinc.o flook.o sprintf.o strutil.o

ginc.o: ../ginc.c
	$(CC) $(CFLAGS) -c ../ginc.c

ginc: ginc.o
	$(LK) $(LFLAGS) -o ginc ginc.o

fromby.o: ../fromby.c
	$(CC) $(CFLAGS) -c ../fromby.c

fromby: fromby.o
	$(LK) $(LFLAGS) -o fromby fromby.o

touch.o: ../touch.c
	$(CC) $(CFLAGS) -c ../touch.c

touch: touch.o msdir.o msera.o
	$(LK) $(LFLAGS) -o touch touch.o msdir.o msera.o

sumup.o: ../sumup.c
	$(CC) $(CFLAGS) -c ../sumup.c

sumup: sumup.o
	$(LK) $(LFLAGS) -o sumup sumup.o

sct_offs.o: ../sct_offs.c
	$(CC) $(CFLAGS) -c ../sct_offs.c

sct_offs: sct_offs.o
	$(LK) $(LFLAGS) -o sct_offs sct_offs.o

unlink.o: ../unlink.c
	$(CC) $(CFLAGS) -c ../unlink.c

unlink: unlink.o
	$(LK) $(LFLAGS) -o unlink unlink.o

dirorg.o: ../dirorg.c
	$(CC) $(CFLAGS) -c ../dirorg.c

dirorg: dirorg.o
	$(LK) $(LFLAGS) -o dirorg dirorg.o

bin2hex.o: ../bin2hex.c
	$(CC) $(CFLAGS) -c ../bin2hex.c

bin2hex: bin2hex.o
	$(LK) $(LFLAGS) -o bin2hex bin2hex.o

slowcat.o: ../slowcat.c
	$(CC) $(CFLAGS) -c ../slowcat.c

slowcat: slowcat.o
	$(LK) $(LFLAGS) -o slowcat slowcat.o

dumpdisk.o: ../dumpdisk.c
	$(CC) $(CFLAGS) -c ../dumpdisk.c

dumpdisk: dumpdisk.o
	$(LK) $(LFLAGS) -o dumpdisk dumpdisk.o

tail.o: ../tail.c
	$(CC) $(CFLAGS) -c ../tail.c

tail: tail.o
	$(LK) $(LFLAGS) -o tail tail.o

hex2bin.o: ../hex2bin.c
	$(CC) $(CFLAGS) -c ../hex2bin.c

hex2bin: hex2bin.o
	$(LK) $(LFLAGS) -o hex2bin hex2bin.o

invtags.o: ../invtags.c
	$(CC) $(CFLAGS) -c ../invtags.c

invtags: invtags.o msdir.o strutil.o
	$(LK) $(LFLAGS) -o invtags invtags.o msdir.o strutil.o

chars: ../chars.c
	$(CC) $(CFLAGS) -o chars ../chars.c

untab: ../untab.c
	$(CC) $(CFLAGS) -o untab ../untab.c

timediff.o: ../timediff.c
	$(CC) $(CFLAGS) -c ../timediff.c

timediff: timediff.o
	$(LK) $(LFLAGS) -o timediff timediff.o

filter.o: ../filter.c
	$(CC) $(CFLAGS) -c ../filter.c

filter: filter.o
	$(LK) $(LFLAGS) -o filter filter.o

clay.o: ../clay.c
	$(CC) $(CFLAGS) -c ../clay.c
clay: clay.o
	c89 -o clay clay.o

teststk.o: ../teststk.c
	$(CC) $(CFLAGS) -c ../teststk.c

machstk.o: ../../util/machstk.c
	$(CC) $(CFLAGS) -c ../../util/machstk.c

teststk: teststk.o machstk.o
	$(LK) $(LFLAGS) -o teststk teststk.o machstk.o

msfname.o: ../../common/msfname.c
	$(CC) $(CFLAGS) -c ../../common/msfname.c

msfname.i: ../../common/msfname.c
	$(CC) $(CFLAGS) -E ../../common/msfname.c

dostou: ../dostou.c msfname.o
	$(CC) $(CFLAGS) -o dostou ../dostou.c msfname.o

di: ../dostou.c msfname.o
	$(CC) $(CFLAGS) -S -I. ../dostou.c msfname.o

sieve: ../sieve.c
	$(CC) $(CFLAGS) -o sieve ../sieve.c

chrad: ../chrad.c sprintf.o
	$(CC) $(CFLAGS) -o chrad ../chrad.c sprintf.o

uniq: ../uniq.c
	$(CC) $(CFLAGS) -o uniq ../uniq.c

edpath.o: ../edpath.c
	$(CC) $(CFLAGS) -c ../edpath.c

edpath: edpath.o re.o
	$(LK) -o edpath edpath.o re.o

spread.o: ../spread.c
	$(CC) $(CFLAGS) -c ../spread.c

spread: spread.o sprint3.o memarch.o
	$(LK) -o spread spread.o sprint3.o memarch.o

sameword.o: ../sameword.c
	$(CC) $(CFLAGS) -c ../sameword.c

sameword: sameword.o strutil.o
	$(LK) -o sameword sameword.o strutil.o

tocol.o: ../tocol.c
	$(CC) $(CFLAGS) -c ../tocol.c

tocol: tocol.o sprint3.o strutil.o memarch.o
	$(LK) -o tocol tocol.o sprint3.o strutil.o memarch.o

strmpath.o: ../../util/strmpath.c
	$(CC) $(CFLAGS) -c ../../util/strmpath.c

spath.o: ../spath.c
	$(CC) $(CFLAGS) -c ../spath.c

spath: spath.o strmpath.o msdir.o sprintf.o
	$(LK) $(LFLAGS) -o spath spath.o strmpath.o msdir.o sprintf.o -lm -L/usr/lib/libc.a -L /opt/ba/lib

strutil.o: ../../util/strutil.c
	$(CC) $(CFLAGS) -c -o strutil.o ../../util/strutil.c

strutil.i: ../../util/strutil.c
	$(CC) $(CFLAGS) -E ../../util/strutil.c

re.o: ../../util/re.c
	$(CC) $(CFLAGS) -c -o re.o ../../util/re.c

grutil.o: ../../util/grutil.c
	$(CC) $(CFLAGS) -c -o grutil.o ../../util/grutil.c

tad.o: ../../util/tad.c
	$(CC) $(CFLAGS) -c -o tad.o ../../util/tad.c

diffs.o: ../../util/diffs.c
	$(CC) $(CFLAGS) -c -o diffs.o ../../util/diffs.c

map.o: ../../util/map.c
	$(CC) $(CFLAGS) -c -o map.o ../../util/map.c

memarch.o: ../../util/memarch.c
	$(CC) $(CFLAGS) -c -o memarch.o ../../util/memarch.c

getline.o: ../../util/getline.c
	$(CC) $(CFLAGS) -c -o getline.o ../../util/getline.c

msdir.o: ../../common/msdir.c
	$(CC) $(CFLAGS) -D MSD_ERASE=1 -I. -c ../../common/msdir.c

msdir.i: ../../common/msdir.c
	$(CC) -E $(CFLAGS) -D MSD_ERASE=1 -I. ../../common/msdir.c

fip.o: ../../common/fip.c
	$(CC) $(CFLAGS) -D MSD_ERASE=1 -I. -c ../../common/fip.c

sprintf.o: ../../common/sprintf.c
	$(CC) -c $(MINCFLAGS) -o sprintf.o ../../common/sprintf.c

ssprintf.o: ../../common/sprintf.c
	cp ../../common/sprintf.c ssprintf.c
	$(CC) -c $(MINCFLAGS) -D USE_T -I. -I../../common ssprintf.c
	rm ssprintf.c

sprintf.i: ../../common/sprintf.c
	$(CC) $(CFLAGS) -S  ../../common/sprintf.c

sprint3.o: ../../common/sprintf.c
	$(CC) -c $(MINCFLAGS) -D ALL_32BIT=1 -D CHK_S=1 -I. ../../common/sprintf.c -o sprint3.o 

args.o: ../../common/args.c
	$(CC) $(CFLAGS) -I. -c ../../common/args.c

msexec.o: ../../common/msexec.c
	$(CC) $(CFLAGS) -c ../../common/msexec.c

msera.o: ../../common/msera.c
	$(CC) $(CFLAGS) -c ../../common/msera.c

flook.o: ../../common/flook.c
	$(CC) $(CFLAGS) -I. -I../../util -c ../../common/flook.c

docrypt.o: ../docrypt.c
	$(CC) $(CFLAGS) -c ../docrypt.c

crypt.o: ../../common/crypt.c
	$(CC) $(CFLAGS) -c ../../common/crypt.c

docrypt: docrypt.o crypt.o msdir.o
	$(LK) $(LFLAGS) -o docrypt docrypt.o crypt.o msdir.o

logmsg.o: ../../util/logmsg.c
	$(CC) $(CFLAGS) -c -o logmsg.o ../../util/logmsg.c

lout: logmsg.o ../../util/lout.cpp
	$(CC) $(CFLAGS) -I. -o lout logmsg.o -DSTANDALONE=1 ../../util/lout.cpp

rpm:
	rm -rf ~/rpmbuild
	mkdir -p ~/rpmbuild/BUILD
	mkdir -p ~/rpmbuild/RPMS
	mkdir -p ~/rpmbuild/SRPMS
	mkdir -p ~/rpmbuild/SPECS
	cp filters.spec ~/rpmbuild/SPECS/
	mkdir -p ~/rpmbuild/SOURCES
	cd ../..;tar cfz ~/rpmbuild/SOURCES/filters-1.2.tar.gz -T fdindex

clean :
	rm -f *.o


