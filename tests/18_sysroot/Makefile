# --sysroot / --rootfs: resolve absolute search paths inside a rootfs dir.

.PHONY: all check clean

LD_LIBRARY_PATH=

all: check

root/usr/lib/liba.so:
	mkdir -p root/usr/lib root/opt/conf root/opt/rpath root/opt/abs root/etc/ld.so.conf.d
	echo 'int a(void){return 1;}' | $(CC) -shared -Wl,-soname,liba.so -o $@ -nostdlib -x c -

root/opt/conf/libb.so: root/usr/lib/liba.so
	echo 'int b(void){return 1;}' | $(CC) -shared -Wl,-soname,libb.so -o $@ -nostdlib -x c -
	printf 'include /etc/ld.so.conf.d/*.conf\n' > root/etc/ld.so.conf
	printf '/opt/conf\n' > root/etc/ld.so.conf.d/x.conf

root/opt/rpath/libc2.so: root/usr/lib/liba.so
	echo 'int c(void){return 1;}' | $(CC) -shared -Wl,-soname,libc2.so -o $@ -nostdlib -x c -

root/opt/abs/libd.so: root/usr/lib/liba.so
	echo 'int d(void){return 1;}' | $(CC) -shared -Wl,-soname,/opt/abs/libd.so -o $@ -nostdlib -x c -

root/bin/exe: root/usr/lib/liba.so root/opt/conf/libb.so root/opt/rpath/libc2.so root/opt/abs/libd.so
	mkdir -p root/bin
	echo 'int a(void); int b(void); int c(void); int d(void); int _start(void){return a()+b()+c()+d();}' | $(CC) -o $@ -Wl,--no-as-needed -Wl,-rpath,/opt/rpath -Wno-implicit-function-declaration -nostdlib -x c - -x none root/usr/lib/liba.so root/opt/conf/libb.so root/opt/rpath/libc2.so root/opt/abs/libd.so

check: root/bin/exe
	! ../../libtree root/bin/exe
	../../libtree --sysroot root root/bin/exe
	../../libtree --rootfs $(CURDIR)/root/ -p root/bin/exe
	../../libtree --sysroot $(CURDIR)/root --format ldd root/bin/exe | grep -F '$(CURDIR)/root/opt/conf/libb.so'
	! ../../libtree --sysroot 2>/dev/null

clean:
	rm -rf root
