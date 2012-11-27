SHELL=/bin/sh

GCC=gcc

CFLAGS=-Wall -pedantic -std=c99 -g -I/usr/local/include -L/usr/local/lib -L/usr/lib/x86_64-linux-gnu
LFLAGS=-lfli -lm -largtable2

.PHONY: all clean

all: fliflttest flicamtest

fliflttest: fli_common.o fliflttest.o
	$(GCC) $(CFLAGS) -o fliflttest fliflttest.o fli_common.o $(LFLAGS)

flicamtest: fli_common.o flicamtest.o
	$(GCC) $(CFLAGS) -o flicamtest flicamtest.o fli_common.o $(LFLAGS)

fliflttest.o:
	$(GCC) $(CFLAGS) -c fliflttest.c $(LFLAGS)

flicamtest.o:
	$(GCC) $(CFLAGS) -c flicamtest.c $(LFLAGS)

fli_common.o:
	$(GCC) $(CFLAGS) -c fli_common.c $(LFLAGS)

clean:
	rm -f *.o
	rm -f fliflttest
	rm -f flicamtest