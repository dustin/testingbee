CFLAGS=-O3 -std=gnu99 -Wall -pedantic

all: example
	./example

bee.a: bee.o
	$(AR) rcs $@ bee.o

bee.o: bee.c bee.h Makefile

example: bee.a example.o

example.o: example.c Makefile

.PHONY: clean
clean:
	rm -rf *.o *.a example
