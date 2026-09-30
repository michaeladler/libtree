.PHONY: clean check

all: check

exe_parse: test.c ../../libtree.c
	$(CC) $(CFLAGS) -std=c99 -Wall -Wextra -Wshadow -pedantic -D_FILE_OFFSET_BITS=64 -o $@ test.c

check: exe_parse
	printf '\n \t\n# comment\n  # comment\n /usr/lib \t # comment\n/\nx \t\ninclude included.conf \t # comment\n' > ld.conf
	printf '/opt/lib\n/' > included.conf
	./exe_parse ./ld.conf

clean:
	rm -f exe_parse ld.conf included.conf
