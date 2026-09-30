#define main libtree_main
#include "../../libtree.c"
#undef main

#include <assert.h>

int main(int argc, char **argv) {
    assert(argc == 2);
    struct string_table_t st = {0};
    assert(parse_ld_config_file(&st, argv[1]) == 0);
    const char expected[] = "/usr/lib:/:x:/opt/lib:/:";
    assert(st.n == sizeof(expected) - 1);
    assert(memcmp(st.arr, expected, st.n) == 0);
    free(st.arr);
    return 0;
}
