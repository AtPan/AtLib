#include "Atlib/types.h"

#ifndef __ATLIB_NEED_SLICE
#error "Do not include \"Atlib/memory/slicedef.h\" directly; use \"slice.h\" instead."
#endif
#undef __ATLIB_NEED_SLICE

#ifndef __ATLIB_SLICEDEF_H
#define __ATLIB_SLICEDEF_H
#define __ATLIB_MAGIC_NUMBER 0x6655779988
struct __slice_mem {
    u64 MAGIC_NUMBER;
    u64 sizemem;
    u64 cap;
    u64 len;
    u8 * ptr;
};
union __slice {
    struct __slice_mem slice;
    void * align;
};
typedef union __slice __slice;
extern __slice * __atlib_as_slice(const void *) __attribute__((nonnull, const));
#endif
