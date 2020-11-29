CCFLAGS = -ggdb -I. -O
#CCFLAGS = -O -I.
#LFLAGS= -ggdb -lx
#LFLAGS=-Xlinker -M
LFLAGS=


all: socserv soctest socclnt


strutil.o: ../../util/strutil.c
	cc $(CCFLAGS) -c ../../util/strutil.c

socserv: ../socserv.c  strutil.o
	cc $(CCFLAGS) -o socserv ../socserv.c strutil.o

soctest: ../soctest.c  strutil.o
	cc $(CCFLAGS) -o soctest ../soctest.c strutil.o

socclnt: ../socclnt.c  util.o
	cc $(CCFLAGS) -o socclnt ../socclnt.c strutil.o


