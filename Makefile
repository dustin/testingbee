CFLAGS=-O3

bee.a: bee.o
	$(AR) rcs $@ bee.o

bee.o: bee.c bee.h Makefile

example: bee.a example.o
	$(CC) -o $@ example.o bee.a
	./example

example.o: example.c Makefile
