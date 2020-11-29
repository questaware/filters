CC=cc
CCFLAGS =-ggdb
DOSFLAGS= -dos -LARGE -M2le -F 3500

all: tar

renlc: ../renlc.c
	$(CC) $(CCFLAGS) -I. -o renlc ../renlc.c

invtags: ../invtags.c
	$(CC) $(CCFLAGS) -I. -o invtags ../invtags.c


strutil.o: ../../util/strutil.c
	$(CC) -I. -c ../../util/strutil.c

dict.o: ../../util/dict.c
	$(CC) $(CCFLAGS) -I. -c ../../util/$(@:.o=.c)

tar.o: ../tar.c 
	$(CC) $(CCFLAGS) -I. -c ../$(@:.o=.c)

rdflop.o: ../../common/rdflop.c 
	$(CC) $(CCFLAGS) -I. -c ../../common/$(@:.o=.c)

msdir.o: ../../common/msdir.c
	$(CC) $(CCFLAGS) -I. -c ../../common/msdir.c

tar: tar.o strutil.o rdflop.o msdir.o dict.o strutil.o
	$(CC) $(CCFLAGS) -o tar tar.o rdflop.o msdir.o dict.o strutil.o

CLEAN :
	rm -f *.o

