SHELL=/bin/sh

GCC=gcc

CFLAGS=-Wall -pedantic -std=c99 -g -I/usr/local/include -L/usr/local/lib -L/usr/lib/x86_64-linux-gnu

.PHONY: all clean

all: fliflttest flicamtest

fliflttest: fli_common.o fliflttest.o
	$(GCC) $(CFLAGS) -o fliflttest fliflttest.o fli_common.o -lfli -lm

flicamtest: fli_common.o flicamtest.o
	$(GCC) $(CFLAGS) -o flicamtest flicamtest.o fli_common.o -lfli -lm

fliflttest.o:
	$(GCC) $(CFLAGS) -c fliflttest.c -lfli -lm

flicamtest.o:
	$(GCC) $(CFLAGS) -c flicamtest.c -lfli -lm

fli_common.o:
	$(GCC) $(CFLAGS) -c fli_common.c -lfli -lm

clean:
	rm -f *.o
	rm -f fliflttest
	rm -f flicamtest