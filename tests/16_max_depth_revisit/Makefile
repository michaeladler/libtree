# exe needs A then B; A needs B; B needs missing C.

.PHONY: check clean

LIBTREE ?= ../../libtree

all: check

libdepth_c_link.so:
	echo 'int c(void){return 1;}' | $(CC) -shared -nostdlib -Wl,-soname,libdepth_c.so -o $@ -x c -

libdepth_b.so: libdepth_c_link.so
	echo 'int c(void); int b(void){return c();}' | $(CC) -shared -nostdlib -Wl,--no-as-needed -Wl,-soname,$@ '-Wl,-rpath,$$ORIGIN' -o $@ -x c - -x none libdepth_c_link.so

libdepth_a.so: libdepth_b.so
	echo 'int b(void); int a(void){return b();}' | $(CC) -shared -nostdlib -Wl,--no-as-needed -Wl,-soname,$@ '-Wl,-rpath,$$ORIGIN' -o $@ -x c - -x none libdepth_b.so

exe: libdepth_a.so libdepth_b.so
	echo 'int a(void); int b(void); int _start(void){return a()+b();}' | $(CC) -nostdlib -Wl,--no-as-needed -Wl,--allow-shlib-undefined '-Wl,-rpath,$$ORIGIN' -o $@ -x c - -x none libdepth_a.so libdepth_b.so

check: exe
	LD_LIBRARY_PATH= $(LIBTREE) --max-depth 1 exe
	@status=0; output=$$(LD_LIBRARY_PATH= $(LIBTREE) --max-depth 2 exe 2>&1) || status=$$?; \
		printf '%s\n' "$$output"; \
		test $$status -eq 28 && printf '%s\n' "$$output" | grep -F 'libdepth_c.so not found'

clean:
	rm -f *.so exe
