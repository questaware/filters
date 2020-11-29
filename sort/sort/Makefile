#
#
#
CC = gcc
# CC = cc

# When to switch from n*log(n) to n^2 method (gamasort & sedgesort)
CUTOFF=15
# number of array elements in test
N_ELEMENTS=50000

# CFLAGS = -ag -DCUTOFF=$(CUTOFF)
CFLAGS = -g -O -DCUTOFF=$(CUTOFF)

SORTOBJ = sorted.o insort.o shellsort.o gamasort.o \
	quicksort.o quickersort.o sedgesort.o heapsort.o

all: sorttest bstest

sorttest: sorttest.o sort.a
	$(CC) $(CFLAGS) sorttest.o sort.a -o $@ -lm

bstest: bstest.o bsearch.o
	$(CC) $(CFLAGS) bstest.o bsearch.o -o $@

sort.a: $(SORTOBJ)
	ar rv $@ $(SORTOBJ)

clean clobber c: .FORCE
	-rm sorttest bstest *.o *.a

.c.o:
	$(CC) $(CFLAGS) -c $*.c

tar tgz: sort-all.tgz

sort-all.tgz: .FORCE
	cd ..; tar cvf - sort/*.[ch] sort/Makefile sort/gpl.txt | \
		gzip -9 > $@ && mv $@ sort/

.FORCE: ;

test check: all
	@echo === checking sort routines
	@sorttest $(N_ELEMENTS) 99999999 && echo === OK
	@echo === checking binary-search routines
	@bstest 7007 && echo === OK

$(SORTOBJ): sort.h

