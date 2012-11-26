SHELL=/bin/sh

GCC=gcc

CFLAGS=-Wall -pedantic -fPIC -ansi -std=c99 -g -I/usr/local/include -L/usr/local/lib -L/usr/lib/x86_64-linux-gnu

.PHONY: all clean distclean

all: fliflttest flicamtest

fliflttest:
	$(GCC) $(CFLAGS) fliflttest.c fli_common.c -o fliflttest -lfli -lm

flicamtest:
	$(GCC) $(CFLAGS) flicamtest.c fli_common.c -o flicamtest -lfli -lm

clean:
	rm -f fliflttest
	rm -f flicamtest

distclean: clean
	rm -f fliflttest
	rm -f flicamtest