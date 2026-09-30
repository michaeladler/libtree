.PHONY: clean check

all: check

exe_strtab: test.c ../../libtree.c
	$(CC) $(CFLAGS) -std=c99 -Wall -Wextra -Wshadow -pedantic -D_FILE_OFFSET_BITS=64 -o $@ test.c

check: exe_strtab
	./exe_strtab

clean:
	rm -f exe_strtab exe_elf
