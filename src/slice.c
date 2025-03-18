#ifdef __DEBUG__
#include "Atlib/memory/slice.h"
#include "Atlib/error.h"
#include "Atlib/io/log.h"
#include <stdlib.h>

#define __ATLIB_NEED_SLICE
#include "Atlib/memory/slicedef.h"

void * __atlib_malloc(usize s, usize n) {
    __slice * p = malloc((s * n) + sizeof(__slice));
    if(p == nullptr) return nullptr;
    p->slice.ptr = (void *)((usize)p + sizeof(__slice));
    p->slice.MAGIC_NUMBER = __ATLIB_MAGIC_NUMBER;
    p->slice.len = 0;
    p->slice.cap = n;
    p->slice.sizemem = s;
    return p->slice.ptr;
}

void * __atlib_calloc(usize s, usize n) {
    __slice * p = calloc((n * s) + sizeof(__slice), 1);
    if(p == nullptr) return nullptr;
    p->slice.ptr = (void *)((usize)p + sizeof(__slice));
    p->slice.MAGIC_NUMBER = __ATLIB_MAGIC_NUMBER;
    p->slice.len = 0;
    p->slice.cap = n;
    p->slice.sizemem = s;
    return p->slice.ptr;
}

void * __atlib_realloc(void * ptr, usize n) {
    if(ptr == nullptr) return __atlib_malloc(sizeof(char), n);
    else if(n == 0) { __atlib_free(ptr); return nullptr; }
    __slice * p = __atlib_as_slice(ptr);
    atlib_compassert(p->slice.MAGIC_NUMBER == __ATLIB_MAGIC_NUMBER);
    if(n % p->slice.sizemem) atlib_dbglog_warn(atout, "Calling atlib_realloc with a suspicious number of bytes: "
            "%ld is not a multiple of the size of the data this pointer (0x%08lX) was originally allocated for (%ld)\n",
            n, (usize)ptr, p->slice.sizemem);
    p = realloc(p, n + sizeof(__slice));
    p->slice.ptr = (void *)((usize)p + sizeof(__slice));
    return p->slice.ptr;
}

void * __atlib_index_ptr(const void * ptr, usize i) {
    atlib_compassert(ptr);
    __slice * p = __atlib_as_slice(ptr);
    atlib_compassert(p->slice.MAGIC_NUMBER == __ATLIB_MAGIC_NUMBER);
    atlib_compassert(i < p->slice.cap);
    return &((u8 (*)[p->slice.sizemem])p->slice.ptr)[i];
}

void __atlib_free(void * p) {
    __slice * v = __atlib_as_slice(p);
    atlib_compassert(v->slice.MAGIC_NUMBER == __ATLIB_MAGIC_NUMBER);
    free(v);
}

__slice * __atlib_as_slice(const void * v) {
    __slice * p = (__slice *)((usize)v - sizeof(__slice));
    atlib_compassert(p->slice.MAGIC_NUMBER == __ATLIB_MAGIC_NUMBER);
    return p;
}

i32 atlib_is_slice(const void * v) {
    if(v == nullptr) return 0;
    __slice * p = __atlib_as_slice(v);
    return p->slice.MAGIC_NUMBER == __ATLIB_MAGIC_NUMBER;
}
#endif
