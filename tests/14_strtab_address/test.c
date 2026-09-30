#define main libtree_main
#include "../../libtree.c"
#undef main

#include <assert.h>

static void check(int bits, uint64_t first_vaddr, uint64_t second_vaddr,
                  uint64_t second_offset, uint64_t filesz, uint64_t strtab,
                  int expected) {
    FILE *file = fopen("exe_elf", "wb");
    assert(file != NULL);
    unsigned char ident[16] = {0x7f, 'E', 'L', 'F'};
    ident[4] = bits;
    ident[5] = host_is_little_endian() ? 1 : 2;
    assert(fwrite(ident, sizeof(ident), 1, file) == 1);

    if (bits == BITS64) {
        struct header_64_t header = {
            .e_type = ET_DYN,
            .e_version = 1,
            .e_phoff = 16 + sizeof(header),
            .e_ehsize = 16 + sizeof(header),
            .e_phentsize = sizeof(struct prog_64_t),
            .e_phnum = 3,
        };
        struct prog_64_t programs[] = {
            {.p_type = PT_LOAD, .p_offset = 0x200, .p_vaddr = first_vaddr,
             .p_filesz = filesz, .p_memsz = filesz + 0x20},
            {.p_type = PT_LOAD, .p_offset = second_offset,
             .p_vaddr = second_vaddr, .p_filesz = filesz,
             .p_memsz = filesz + 0x20},
            {.p_type = PT_DYNAMIC, .p_offset = 0x100},
        };
        struct dyn_64_t dynamic[] = {
            {.d_tag = DT_STRTAB, .d_val = strtab}, {.d_tag = DT_NULL},
        };
        assert(fwrite(&header, sizeof(header), 1, file) == 1);
        assert(fwrite(programs, sizeof(programs), 1, file) == 1);
        assert(fseek(file, 0x100, SEEK_SET) == 0);
        assert(fwrite(dynamic, sizeof(dynamic), 1, file) == 1);
    } else {
        struct header_32_t header = {
            .e_type = ET_DYN,
            .e_version = 1,
            .e_phoff = 16 + sizeof(header),
            .e_ehsize = 16 + sizeof(header),
            .e_phentsize = sizeof(struct prog_32_t),
            .e_phnum = 3,
        };
        struct prog_32_t programs[] = {
            {.p_type = PT_LOAD, .p_offset = 0x200, .p_vaddr = first_vaddr,
             .p_filesz = filesz, .p_memsz = filesz + 0x20},
            {.p_type = PT_LOAD, .p_offset = second_offset,
             .p_vaddr = second_vaddr, .p_filesz = filesz,
             .p_memsz = filesz + 0x20},
            {.p_type = PT_DYNAMIC, .p_offset = 0x100},
        };
        struct dyn_32_t dynamic[] = {
            {.d_tag = DT_STRTAB, .d_val = strtab}, {.d_tag = DT_NULL},
        };
        assert(fwrite(&header, sizeof(header), 1, file) == 1);
        assert(fwrite(programs, sizeof(programs), 1, file) == 1);
        assert(fseek(file, 0x100, SEEK_SET) == 0);
        assert(fwrite(dynamic, sizeof(dynamic), 1, file) == 1);
    }
    assert(fseek(file, 0x400, SEEK_SET) == 0);
    assert(fputc(0, file) != EOF);
    assert(fclose(file) == 0);

    struct libtree_state_t state = {0};
    libtree_state_init(&state);
    assert(recurse("exe_elf", 0, &state, (struct compat_t){.any = 1},
                   (struct found_t){.how = INPUT}) == expected);
    libtree_state_free(&state);
    assert(remove("exe_elf") == 0);
}

int main(void) {
    for (int bits = BITS32; bits <= BITS64; ++bits) {
        const uint64_t valid[] = {0x1000, 0x101f, 0x2000, 0x201f};
        const uint64_t invalid[] = {0xfff, 0x1020, 0x1030, 0x1800,
                                    0x2020, 0x2030, 0x3000};
        for (size_t i = 0; i < sizeof(valid) / sizeof(valid[0]); ++i)
            check(bits, 0x1000, 0x2000, 0x300, 0x20, valid[i], 0);
        for (size_t i = 0; i < sizeof(invalid) / sizeof(invalid[0]); ++i)
            check(bits, 0x1000, 0x2000, 0x300, 0x20, invalid[i],
                  ERR_INVALID_STRTAB);
        check(bits, 0x1000, 0x2000, 0x300, 0, 0x2000, ERR_INVALID_STRTAB);
    }
    check(BITS64, 0x1000, UINT64_MAX - 0x10, 0x300, 0x20,
          UINT64_MAX - 1, 0);
    check(BITS64, 0x1000, 0x2000, UINT64_MAX - 0x10, 0x20,
          0x201f, ERR_INVALID_STRTAB);
    return 0;
}
