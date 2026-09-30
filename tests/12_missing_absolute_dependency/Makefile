LD_LIBRARY_PATH=

.PHONY: clean

all: check

libabsolute.so:
	echo 'int f(void){return 1;}' | $(CC) -shared "-Wl,-soname,$(CURDIR)/missing.so" -o $@ -nostdlib -x c -

exe_absolute: libabsolute.so
	echo 'int f(void); int _start(void){return f();}' | $(CC) -o $@ -Wl,--no-as-needed -nostdlib libabsolute.so -x c -

check: exe_absolute
	@status=0; output=$$(LD_LIBRARY_PATH= ../../libtree exe_absolute 2>&1) || status=$$?; \
		test $$status -eq 28 && printf '%s\n' "$$output" | grep -F '$(CURDIR)/missing.so not found'

clean:
	rm -f *.so exe*

CURDIR ?= $(.CURDIR)
