LD_LIBRARY_PATH=

.PHONY: all check clean
all: check

libshared.so:
	echo 'int shared(void){return 1;}' | $(CC) -shared -nostdlib -Wl,-soname,$@ -o $@ -x c -

libmissing.so:
	echo 'int missing(void){return 1;}' | $(CC) -shared -nostdlib -Wl,-soname,$@ -o $@ -x c -

libleft.so: libshared.so libmissing.so
	echo 'extern int shared(void); extern int missing(void); int left(void){return shared()+missing();}' | $(CC) -shared -nostdlib -Wl,-soname,$@ -o $@ -x c - -L. -lshared -lmissing

libright.so: libshared.so libmissing.so
	echo 'extern int shared(void); extern int missing(void); int right(void){return shared()+missing();}' | $(CC) -shared -nostdlib -Wl,-soname,$@ -o $@ -x c - -L. -lshared -lmissing

libnoname.so:
	echo 'int noname(void){return 1;}' | $(CC) -shared -nostdlib -o $@ -x c -

libalias.so: libnoname.so
	cp libnoname.so $@

libdirect.so:
	echo 'int direct(void){return 1;}' | $(CC) -shared -nostdlib -o $@ -x c -

exe: libleft.so libright.so libnoname.so libalias.so libdirect.so
	echo 'int main(void){return 0;}' | $(CC) -Wl,--no-as-needed -Wl,--disable-new-dtags '-Wl,-rpath,$(CURDIR)' -Wl,-rpath-link,. -o $@ -x c - -x none -L. -lleft -lright -lnoname -lalias $(CURDIR)/libdirect.so

static:
	echo 'void _start(void){for(;;){}}' | $(CC) -nostdlib -static -o $@ -x c -

static-pie:
	echo 'void _start(void){for(;;){}}' | $(CC) -nostdlib -static-pie -o $@ -x c -

exe32:
	echo 'void _start(void){for(;;){}}' | $(CC) -m32 -nostdlib -o $@ -x c -

check: exe static static-pie exe32
	@set -eu; \
	interpreter=$$(readelf -l exe | sed -n 's/.*Requesting program interpreter: \(.*\)]/\1/p'); \
	export LD_LIBRARY_PATH=$$(dirname "$$interpreter"); \
	export NO_COLOR=1; \
	../../libtree exe > default.out; \
	../../libtree --format tree exe > tree.out; cmp default.out tree.out; \
	../../libtree --format ldd exe > ldd.out; \
	../../libtree --format=ldd exe > equals.out; cmp ldd.out equals.out; \
	../../libtree --format ldd -p -vvv exe > flags.out; cmp ldd.out flags.out; \
	for args in '--format' '--format=' '--format=unknown' '--format unknown'; do \
		if ../../libtree $$args > invalid.out 2> invalid.err; then exit 1; fi; \
		test -s invalid.err; test ! -s invalid.out; \
	done; \
	if ../../libtree --format '' exe > invalid.out 2> invalid.err; then exit 1; fi; \
	test -s invalid.err; test ! -s invalid.out; \
	for lib in libleft.so libright.so libshared.so libmissing.so libnoname.so libalias.so; do \
		test "$$(grep -Fc "$$lib => $(CURDIR)/$$lib" ldd.out)" = 1; \
	done; \
	grep -F "$(CURDIR)/libdirect.so => $(CURDIR)/libdirect.so" ldd.out; \
	grep -F 'libc.so' ldd.out; \
	test "$$(grep -Fxc "$$(printf '\t')$$interpreter" ldd.out)" = 1; \
	! grep -F "=> $$interpreter" ldd.out; \
	! grep -Eq '\[|├|└|│|Paths considered|linux-vdso|0x' ldd.out; \
	! grep -q "$$(printf '\033')" ldd.out; \
	! grep -q '^exe:' ldd.out; \
	../../libtree --format ldd exe exe > multi.out; \
	test "$$(grep -c '^exe:$$' multi.out)" = 2; \
	test "$$(grep -Fc 'libshared.so => ' multi.out)" = 2; \
	../../libtree --format ldd --max-depth 0 exe > zero.out; \
	test "$$(wc -l < zero.out)" = 1; grep -Fx "$$(printf '\t')$$interpreter" zero.out; \
	../../libtree --format ldd --max-depth 1 exe > depth.out; \
	grep -F 'libleft.so => ' depth.out; ! grep -F 'libshared.so => ' depth.out; \
	for input in static static-pie libshared.so; do \
		../../libtree --format ldd "$$input" > static.out; \
		test "$$(wc -l < static.out)" = 1; \
		grep -Fx "$$(printf '\t')statically linked" static.out; \
	done; \
	readelf -h static-pie | grep -q 'DYN'; readelf -l static-pie | grep -q DYNAMIC; \
	interpreter32=$$(readelf -l exe32 | sed -n 's/.*Requesting program interpreter: \(.*\)]/\1/p'); \
	../../libtree --format ldd exe32 > elf32.out; \
	grep -Fx "$$(printf '\t')$$interpreter32" elf32.out; \
	ldd ./exe | sed -E '/linux-vdso|linux-gate/d; s/ \(0x[[:xdigit:]]+\)//; s/^[[:space:]]*//; /^\//s/ => .*//' | sort > runtime.out; \
	sed -E 's/^[[:space:]]*//; /^\//s/ => .*//' ldd.out | sort > normalized.out; cmp runtime.out normalized.out; \
	mv libalias.so libalias.saved; ln libnoname.so libalias.so; \
	trap 'rm -f libalias.so; mv libalias.saved libalias.so' EXIT HUP INT TERM; \
	../../libtree --format ldd exe > alias.out; \
	test "$$(grep -Ec 'lib(noname|alias).so => ' alias.out)" = 1; \
	rm libalias.so; mv libalias.saved libalias.so; trap - EXIT HUP INT TERM; \
	mv libmissing.so libmissing.saved; mv libdirect.so libdirect.saved; \
	trap 'mv libmissing.saved libmissing.so; mv libdirect.saved libdirect.so' EXIT HUP INT TERM; \
	status=0; ../../libtree --format ldd exe > missing.out 2> missing.err || status=$$?; \
	test "$$status" = 28; \
	test "$$(grep -Fc 'libmissing.so => not found' missing.out)" = 1; \
	test "$$(grep -Fc '$(CURDIR)/libdirect.so => not found' missing.out)" = 1; \
	grep -Fx 'Error [exe]: Not all dependencies were found' missing.err; \
	../../libtree --format ldd exe exe > multi-missing.out 2> multi-missing.err || test $$? = 28; \
	test "$$(grep -Fc 'libmissing.so => not found' multi-missing.out)" = 2; \
	mv libmissing.saved libmissing.so; mv libdirect.saved libdirect.so; trap - EXIT HUP INT TERM; \
	for input in exe exe32; do \
		for length in invalid empty oversized; do \
			case "$$length" in \
				invalid) printf 'invalid-interpreter' > interpreter.bin ;; \
				empty) : > interpreter.bin ;; \
				oversized) dd if=/dev/zero of=interpreter.bin bs=4097 count=1 2>/dev/null ;; \
			esac; \
			objcopy --update-section .interp=interpreter.bin "$$input" malformed; \
			status=0; ../../libtree --format ldd malformed > malformed.out 2> malformed.err || status=$$?; \
			test "$$status" = 19; grep -F 'Invalid ELF program header' malformed.err; \
		done; \
		offset=$$(readelf -l "$$input" | awk '/INTERP/{print $$2}'); \
		dd if="$$input" of=malformed bs=1 count=$$(($$offset + 1)) 2>/dev/null; \
		status=0; ../../libtree --format ldd malformed > malformed.out 2> malformed.err || status=$$?; \
		test "$$status" = 19; grep -F 'Invalid ELF program header' malformed.err; \
	done

clean:
	rm -f *.so *.saved exe exe32 static static-pie malformed interpreter.bin *.out *.err

CURDIR ?= $(.CURDIR)
