#include "Atlib/types.h"

#ifndef __ATLIB_NEED_SLICE
#error "Do not include \"Atlib/memory/slicedef.h\" directly; use \"slice.h\" instead."
#endif
#undef __ATLIB_NEED_SLICE

#ifndef __ATLIB_SLICEDEF_H
#define __ATLIB_SLICEDEF_H
#define __ATLIB_MAGIC_NUMBER ((u64)0x6655779988)
#define __ATLIB_MAGIC_NUMBER_BUF ((const u8 []){0x66, 0x55, 0x77, 0x99, 0x88})
#define __ATLIB_ALLOC_ENTRIES_MAX 0x100

struct __slice {
    u64 blksize;
    u64 n;
    u64 MAGIC;
} __attribute__((aligned(sizeof(void *))));

struct __alloc_entry {
    const char * fname;
    u32 ln;
    u64 mem;
    void * ptr;
};

extern struct __slice * __atlib_as_slice(const void *) __attribute__((nothrow, nonnull, const));
#endif
