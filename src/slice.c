#ifdef __DEBUG__
#include <string.h>
#include "Atlib/memory/slice.h"
#include "Atlib/error.h"
#include "Atlib/io/log.h"
#include <stdlib.h>

#define __ATLIB_NEED_SLICE
#include "Atlib/memory/slicedef.h"

#undef memcpy

static struct __alloc_entry entries[__ATLIB_ALLOC_ENTRIES_MAX] = {0};
static i32 entries_n = 0;

void * __atlib_malloc(isize blk, isize n, const char * fname, u32 ln) {
    if(blk < 0 || n < 0) {
        atlib_log_writef(aterr, "WARN", fname, ln,
                "Usage of \"atlib_malloc(%ld, %ld)\" @ \"%s:%d\" is suspicious.\n",
                blk, n, fname, ln);
        return nullptr;
    }
    const isize membuf_len = sizeof(struct __slice) + (blk * n) + sizeof(__ATLIB_MAGIC_NUMBER_BUF);
    char * p = malloc(membuf_len);
    if(p == nullptr) return p;
    memset(p, 0, sizeof(struct __slice));
    memcpy(p, &(struct __slice){.blksize = blk, .n = n, .MAGIC = __ATLIB_MAGIC_NUMBER}, sizeof(struct __slice));
    entries[entries_n++] = (struct __alloc_entry) {
        .fname = fname,
        .ln = ln,
        .mem = blk * n,
        .ptr = (void *)p,
    };
    memcpy((char *)((usize)p + membuf_len - sizeof(__ATLIB_MAGIC_NUMBER_BUF)), __ATLIB_MAGIC_NUMBER_BUF, sizeof(__ATLIB_MAGIC_NUMBER_BUF));
    return (void *)((usize)p + sizeof(struct __slice));
}

void * __atlib_calloc(isize s, isize n, const char * fname, u32 ln) {
    void * p = __atlib_malloc(s, n, fname, ln);
    memset(p, 0, s * n);
    return p;
}

void * __atlib_realloc(void * restrict ptr, isize n, const char * restrict fname, u32 ln) {
    if(ptr == nullptr) return __atlib_malloc(sizeof(u8), n, fname, ln);
    if(n == 0) { __atlib_free(ptr, fname, ln); return nullptr; }
    struct __slice * p = __atlib_as_slice(ptr);

    if(n % p->blksize) {
        atlib_log_writef(aterr, "WARN", fname, ln,
                "Calling \"atlib_realloc(0x%08lx, %ld)\" with suspicious resize request: "
                "\"%ld\" is not a multiple of the data type (size \"%ld\") this memory was originally allocated with.\n",
                (usize)ptr, n, n, p->blksize);
    }

    i32 i = 0;
    for(; i < entries_n && (usize)entries[i].ptr != (usize)p; i++);
    if(i == entries_n) {
        atlib_log_writef(aterr, "WARN", fname, ln,
                "Calling \"atlib_realloc(0x%08lx, %ld)\" with suspicious pointer: "
                "Pointer \"0x%08lx\" is not a pointer returned from \"atlib_malloc\" or \"atlib_calloc\".\n",
                (usize)p, n, (usize)p);
        entries_n++;
    }
    memset(&p[n + sizeof(struct __slice)], 0, sizeof(__ATLIB_MAGIC_NUMBER_BUF));
    p = realloc(p, n + sizeof(struct __slice) + sizeof(__ATLIB_MAGIC_NUMBER_BUF));
    entries[i] = (struct __alloc_entry) {
        .fname = fname,
        .ln = ln,
        .mem = n,
        .ptr = (void *)p,
    };
    *p = (struct __slice) {
        .blksize = p->blksize,
        .n = n / p->blksize,
        .MAGIC = __ATLIB_MAGIC_NUMBER,
    };
    memcpy(&p[n + sizeof(struct __slice)], __ATLIB_MAGIC_NUMBER_BUF, sizeof(__ATLIB_MAGIC_NUMBER_BUF));
    return (void *)((usize)p + sizeof(struct __slice));
}

void __atlib_free(void * restrict p, const char * restrict fname, u32 ln) {
    struct __slice * v = __atlib_as_slice(p);
    i32 i = 0;
    for(; i < entries_n && (usize)entries[i].ptr != (usize)v; i++);
    if(i == entries_n || v->MAGIC != __ATLIB_MAGIC_NUMBER) {
        atlib_log_writef(aterr, "WARN", fname, ln,
                "Calling \"atlib_free(0x%08lx)\" with invalid address. "
                "Pointer \"0x%08lx\" was not returned from \"atlib_malloc\" or \"atlib_calloc\".\n",
                (usize)p, (usize)p);
    }
    if(memcmp(&((char *)p)[v->n * v->blksize], __ATLIB_MAGIC_NUMBER_BUF, sizeof(__ATLIB_MAGIC_NUMBER_BUF))) {
        atlib_log_writef(aterr, "WARN", fname, ln,
                "HEAP CORRUPTION DETECTED! AtLib detected that the application wrote to memory past the allocated heap buffer "
                "with pointer \"0x%08lx\" (allocated \"%s:%d\").\n",
                (usize)p, entries[i].fname, entries[i].ln);
    }
    for(; i < entries_n - 1; i++) {
        entries[i] = entries[i+1];
    }
    memset(&entries[--entries_n], 0, sizeof(*entries));
    free(v);
}

void * __atlib_memcpy(void * dest, const void * src, isize n, const char * restrict fname, u32 ln) {
    const i32 alias = (usize)dest != (usize)src
        && (((usize)dest < (usize)src && (usize)dest + n < (usize)src)
        || ((usize)dest > (usize)src && (usize)dest + n > (usize)src));
    if(!alias) {
        atlib_log_writef(aterr, "WARN", fname, ln, "Dest (0x%08lx) and Src (0x%08lx) memory areas overlap!"
                "Consider using \"memmove\" instead for correct behavior.\n",
                (usize)dest, (usize)src);
    }
    return memcpy(dest, src, n);
}

struct __slice * __atlib_as_slice(const void * v) {
    struct __slice * p = (struct __slice *)((usize)v - sizeof(struct __slice));
    return p;
}

void __atlib_memory_cleanup() {
    for(u32 i = 0; i < __ATLIB_ALLOC_ENTRIES_MAX; i++) {
        if(entries[i].ptr == nullptr) continue;
        usize ptrint = (usize)__atlib_as_slice(entries[i].ptr);
        u64 mem = entries[i].mem;
        const char * fname = entries[i].fname;
        u32 ln = entries[i].ln;
        atlib_log_writef(aterr, "WARN", entries[i].fname, entries[i].ln,
                "MEMORY LEAK DETECTED! "
                "AtLib detected that buffer \"0x%08lx\" (%ld bytes) was never freed. "
                "Allocated @ \"%s:%d\".\n",
                ptrint, mem, fname, ln);
        free(entries[i].ptr);
    }
}
#endif
