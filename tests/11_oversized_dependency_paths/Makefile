LD_LIBRARY_PATH=

.PHONY: clean

all: check

libabsolute.so:
	@soname=$$(printf '/%04096d' 0 | tr 0 x); \
	echo 'int f(void){return 1;}' | $(CC) -shared -Wl,-soname,$$soname -o $@ -nostdlib -x c -

libsearch.so:
	@soname=$$(printf '%04095d' 0 | tr 0 x); \
	echo 'int f(void){return 1;}' | $(CC) -shared -Wl,-soname,$$soname -o $@ -nostdlib -x c -

exe_absolute: libabsolute.so
	echo 'int f(void); int _start(void){return f();}' | $(CC) -o $@ -Wl,--no-as-needed -nostdlib libabsolute.so -x c -

exe_search: libsearch.so
	echo 'int f(void); int _start(void){return f();}' | $(CC) -o $@ -Wl,--no-as-needed -Wl,-rpath,. -nostdlib libsearch.so -x c -

libshort.so:
	echo 'int f(void){return 1;}' | $(CC) -shared -Wl,-soname,$@ -o $@ -nostdlib -x c -

exe_path: libshort.so
	echo 'int f(void); int _start(void){return f();}' | $(CC) -o $@ -Wl,--no-as-needed -Wl,-z,nodefaultlib -nostdlib libshort.so -x c -

check: exe_absolute exe_search exe_path
	@status=0; LD_LIBRARY_PATH= timeout 5 ../../libtree exe_absolute >/dev/null 2>&1 || status=$$?; test $$status -eq 28
	@status=0; LD_LIBRARY_PATH= timeout 5 ../../libtree exe_search >/dev/null 2>&1 || status=$$?; test $$status -eq 28
	@set -e; for length in 4095 4096 8192; do \
		long=$$(printf '%0*d' $$length 0 | tr 0 x); \
		for paths in "$$long." "$$long.:missing"; do \
			status=0; LD_LIBRARY_PATH="$$paths" timeout 5 ../../libtree exe_path >/dev/null 2>&1 || status=$$?; \
			test $$status -eq 28; \
		done; \
		LD_LIBRARY_PATH="$$long.:." timeout 5 ../../libtree exe_path >/dev/null 2>&1; \
	done

clean:
	rm -f *.so exe*
