libs = ['fli', 'm', 'argtable2', 'cfitsio']
libpath = ['/usr/local/lib', '/usr/lib/x86_64-linux-gnu']
cflags = '-Wall -g -pedantic -std=c99'

Object(['fliflttest.c', 'flicamtest.c', 'fli_common.c', 'argtable_common.c'], CCFLAGS = cflags)
Program('fliflttest', ['fliflttest.o', 'fli_common.o', 'argtable_common.o'],
  LIBS = libs,
  LIBPATH = libpath,
  CCFLAGS = cflags)
Program('flicamtest', ['flicamtest.o', 'fli_common.o', 'argtable_common.o'],
  LIBS = libs,
  LIBPATH = libpath,
  CCFLAGS = cflags)