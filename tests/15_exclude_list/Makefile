.PHONY: clean check

all: check

exe_exclude: test.c ../../libtree.c
	$(CC) $(CFLAGS) -std=c99 -Wall -Wextra -Wshadow -pedantic -D_FILE_OFFSET_BITS=64 -o $@ test.c

check: exe_exclude
	./exe_exclude

clean:
	rm -f exe_exclude
