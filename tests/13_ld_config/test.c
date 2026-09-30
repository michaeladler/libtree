#define main libtree_main
#include "../../libtree.c"
#undef main

#include <assert.h>

int main(int argc, char **argv) {
    assert(argc == 2 || argc == 3);
    struct string_table_t st = {0};
    assert(parse_ld_config_file(&st, argv[1], 0) == 0);
    const char *expected = argc == 2 ? "/usr/lib:/:x:/opt/lib:/:/opt/lib:/:"
                                     : "/cycle:";
    size_t len = strlen(expected);
    size_t repeats = argc == 2 ? 1 : MAX_RECURSION_DEPTH;
    assert(st.n == len * repeats);
    for (size_t i = 0; i < repeats; ++i)
        assert(memcmp(st.arr + i * len, expected, len) == 0);
    free(st.arr);
    return 0;
}
