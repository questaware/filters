CDIR=\gener\c600
CC=cl
#CFLAGS = /Zi /AL /I../ibmpc
#LFLAGS = /NOI /CODEVIEW
CFLAGS = /AL /Ocegilsa /I../ibmpc
LFLAGS = /B /ST:1500 /NOI /MAP

all: tar.exe

dllcvt: ../dllcvt.c ../dllcvt.h
	$(CC) $(CFLAGS) ../dllcvt.c

dumpdll: ../dumpdll.c
	$(CC) $(CFLAGS) ../dumpdll.c

strutil.obj: ../../util/strutil.c
	$(CC) $(CFLAGS) -I. -c ../../util/strutil.c

msdir.obj: ../../common/msdir.c
	$(CC) $(CFLAGS) -I. -c ../../common/msdir.c

sprintf.obj: ../../common/sprintf.c
	$(CC) $(CFLAGS) -I. -c ../../common/sprintf.c

strutil.s: ../../util/strutil.c
	$(CC) $(CFLAGS) /Fc -I. -c ../../util/strutil.c

strutil.e: ../../util/strutil.c
	$(CC) $(CFLAGS) /E -I. -c ../../util/strutil.c

dict.obj: ../../util/dict.c
	$(CC) $(CFLAGS) -I. -c ../../util/dict.c

tar.obj: ../tar.c 
	$(CC) $(CFLAGS) -I. -c ../tar.c

rdflop.obj: ../../common/rdflop.c
	$(CC) $(CFLAGS) -I. -c ../../common/rdflop.c

tar.exe: tar.obj strutil.obj rdflop.obj dict.obj strutil.obj sprintf.obj msdir.obj
	$(CDIR)\bin\link /B /ST:4000 $(LFLAGS) tar+rdflop+dict+strutil+sprintf+msdir, tar, , ;

lineqns.obj: ../lineqns.c
	$(CC) $(CFLAGS) -I. -c ../lineqns.c
	
lineqns.exe: lineqns.obj
	\$(CDIR)\bin\link /B /ST:4000 $(LFLAGS) lineqns, lineqns, , ;


ec.obj: ../ec.c
	$(CC) $(CFLAGS) -I. -c ../ec.c

ec.exe: lineqns.obj ec.obj
	\$(CDIR)\bin\link /B /ST:4000 $(LFLAGS) lineqns+ec, ec, , ;

CLEAN :
	rm -f *.obj

test.exe: test.c
	$(CC) $(CFLAGS) -I. test.c


isprime.exe: ../isprime.c
	$(CC) $(CFLAGS) -I. ../isprime.c

