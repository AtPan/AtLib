#include "Atlib/memory/string.h"
#include "Atlib/error.h"
#include "Atlib/memory/slice.h"
#include "Atlib/types.h"
#include <string.h>

#define __ATLIB_NEED_SLICE
#include "Atlib/memory/slicedef.h"

string_t * atlib_string_create() {
    return atlib_string_create_capacity(__ATLIB_STRING_DEFLEN);
}

string_t * atlib_string_lit(const char * src) {
    string_t * s = atlib_string_create_capacity(strlen(src));
    __atlib_as_slice(s)->slice.len = strlen(src);
    memcpy(s, src, __atlib_as_slice(s)->slice.len);

    return s;
}

string_t * atlib_string_create_capacity(usize capacity) {
    return atlib_calloc(u8, capacity);
}

void atlib_string_destroy(string_t * s) {
    atlib_free(s);
}

usize atlib_string_len(const string_t * s) {
    if(s == nullptr) return 0;
    struct __slice_mem m = __atlib_as_slice(s)->slice;
    return m.len;
}

string_t * atlib_string_copy(string_t * restrict dst, const string_t * restrict src) {
    atlib_compassert(dst != nullptr);
    atlib_compassert(src != nullptr);

    dst = atlib_realloc(dst, atlib_string_len(src));
    __atlib_as_slice(dst)->slice.len = atlib_string_len(src);
    memcpy(dst, src, __atlib_as_slice(dst)->slice.len);
    return dst;
}

string_t * atlib_string_cat(string_t * restrict dst, const string_t * restrict src) {
    atlib_compassert(dst != nullptr);
    atlib_compassert(src != nullptr);

    __slice * dstslc = __atlib_as_slice(dst);
    __slice * srcslc = __atlib_as_slice(src);
    u64 len = srcslc->slice.len + dstslc->slice.len;

    dst = atlib_realloc(dst, len);
    dstslc = __atlib_as_slice(dst);
    memcpy((u8 *)((usize)dst + dstslc->slice.len), src, srcslc->slice.len);
    dstslc->slice.len = len;
    
    return dst;
}

usize atlib_string_split(string_t ** restrict dst, usize n, const string_t * restrict src, char c) {
    atlib_compassert(src != nullptr);
    atlib_compassert(c != '\0');

    u32 ind[256] = {0};
    u32 _n = 1;
    for(u32 i = 0; i < atlib_string_len(src) && _n < 256; i++) {
        if(*((u8 *)atlib_index(src, i)) == c) {
            ind[_n++] = i;
        }
    }
    ind[_n] = atlib_string_len(src);
    
    if(n == 0) {
        *dst = atlib_string_create_capacity(atlib_string_len(src));
        *dst = atlib_string_cat(*dst, src);
        return 1;
    }

    usize i;
    for(i = 0; i < _n && i < n; i++) {
        u32 len = ind[i + 1] - ind[i];
        string_t * s = atlib_string_create_capacity(len);
        __atlib_as_slice(s)->slice.len = len;
        memcpy(s, &src[len], len);
        dst[i] = s;
    }
    return i;
}

i32 atlib_string_comp(const string_t * a, const string_t * b) {
    const usize alen = atlib_string_len(a);
    const usize blen = atlib_string_len(b);
    usize i;

    for(i = 0; i < alen && i < blen && a[i] == b[i]; i++);
    return a[i] - b[i];
}
