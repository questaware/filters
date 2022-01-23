DEBUG=1
DIAG=1
C_HOME=C:\Program Files (x86)\Microsoft Visual Studio\VC98

HERE:=$(CURDIR)/..

include ../Makefile

here:
	echo $(HERE)

vp :
	echo $(VPATH)

db:
	dumpbin /headers ffg.exe
sb:
	editbin /bind /largeaddressaware:no /stack:0x20000,0x2000 /heap:2000000 ffg.exe
