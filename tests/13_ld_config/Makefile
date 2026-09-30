.PHONY: clean check

all: check

exe_parse: test.c ../../libtree.c
	$(CC) $(CFLAGS) -std=c99 -Wall -Wextra -Wshadow -pedantic -D_FILE_OFFSET_BITS=64 -o $@ test.c

check: exe_parse
	printf '\n \t\n# comment\n  # comment\n /usr/lib \t # comment\n/\nx \t\ninclude included.conf \t # comment\ninclude included.conf\n' > ld.conf
	printf '/opt/lib\n/' > included.conf
	./exe_parse ./ld.conf
	./exe_parse ld.conf
	./exe_parse "$(CURDIR)/ld.conf"
	printf 'include ./self.conf\n/cycle\n' > self.conf
	./exe_parse self.conf cycle
	printf 'include glob*.conf\n/cycle\n' > glob.conf
	./exe_parse glob.conf cycle
	printf 'include loop-b.conf\n/cycle\n' > loop-a.conf
	printf 'include loop-a.conf\n/cycle\n' > loop-b.conf
	./exe_parse loop-a.conf cycle
	printf 'include alias.conf\n/cycle\n' > alias-source.conf
	rm -f alias.conf
	ln -s alias-source.conf alias.conf
	./exe_parse alias-source.conf cycle
	rm alias.conf
	ln alias-source.conf alias.conf
	./exe_parse alias-source.conf cycle

clean:
	rm -f exe_parse ld.conf included.conf self.conf glob.conf loop-a.conf loop-b.conf alias-source.conf alias.conf
