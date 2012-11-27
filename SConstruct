libs = ['fli', 'm', 'argtable2']
libpath = ['/usr/local/lib', '/usr/lib/x86_64-linux-gnu']
cflags = '-Wall -g -pedantic -std=c99'

Object(['fliflttest.c', 'flicamtest.c', 'fli_common.c'], CCFLAGS = cflags)
Program('fliflttest', ['fliflttest.o', 'fli_common.o'],
  LIBS = libs,
  LIBPATH = libpath,
  CCFLAGS = cflags)
Program('flicamtest', ['flicamtest.o', 'fli_common.o'],
  LIBS = libs,
  LIBPATH = libpath,
  CCFLAGS = cflags)