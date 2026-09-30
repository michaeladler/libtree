#define main libtree_main
#include "../../libtree.c"
#undef main

#include <assert.h>

int main(void) {
    for (size_t i = 0; i < sizeof(exclude_list) / sizeof(exclude_list[0]); ++i) {
        char name[128];
        const char *suffixes[] = {"", ".6", ".1.2.3"};
        for (size_t j = 0; j < sizeof(suffixes) / sizeof(suffixes[0]); ++j) {
            snprintf(name, sizeof(name), "%s%s", exclude_list[i], suffixes[j]);
            assert(is_in_exclude_list(name));
        }
    }

    char names[][64] = {"libc.so_custom.so", "libm.sox", "libc.so_custom.so.6",
                        "libm.sox.1", "libc", "libother.so.1", "", ".", "123"};
    for (size_t i = 0; i < sizeof(names) / sizeof(names[0]); ++i)
        assert(!is_in_exclude_list(names[i]));
    return 0;
}
