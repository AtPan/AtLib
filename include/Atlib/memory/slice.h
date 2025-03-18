#ifndef __ATLIB_SLICE_H
#define __ATLIB_SLICE_H

#ifdef __DEBUG__
#include "../types.h"

#  define atlib_malloc(t, n)  __atlib_malloc(sizeof(t), n)
#  define atlib_malloc_raw(b) __atlib_malloc(b, 1)
#  define atlib_calloc(t, n)  __atlib_calloc(sizeof(t), n)
#  define atlib_realloc(p, n) __atlib_realloc(p, n)
#  define atlib_index(p, i)   __atlib_index_ptr(p, i)
#  define atlib_free(p)       __atlib_free(p)

extern void * __atlib_malloc(usize, usize) __attribute__((malloc));
extern void * __atlib_calloc(usize, usize) __attribute__((malloc));
extern void * __atlib_realloc(void *, usize);
extern void * __atlib_index_ptr(const void *, usize) __attribute__((nonnull));
extern void   __atlib_free(void *) __attribute__((nonnull));
extern i32 atlib_is_slice(const void *);

#else

#  include <stdlib.h>
#  define atlib_malloc(t, n)  malloc(sizeof(t) * n)
#  define atlib_calloc(t, n)  calloc(n, sizeof(t))
#  define atlib_realloc(p, n) realloc(p, n)
#  define atlib_index(p, i)   (&((p)[i]))
#  define atlib_free(p)       free(p)

#endif /* __DEBUG__ */

/**
 * @def atlib_malloc(t, n)
 * @brief AtLib memory allocation wrapper macro.
 * @param t The data type to allocate for.
 * @param n The number of objects of the aforementioned data type to allocate.
 * @returns A pointer to the allocated memory, or @c nullptr if an error occured.
 *
 * In debug mode, the wrapper adds additional header data to the allocated memory
 * to ensure data safety. In release mode, the wrapper calls glibc @c malloc.
 *
 * Example Usage:
 * ```c
 * bufread_t * br = atlib_malloc(bufread_t, 1);
 * char * str = atlib_malloc(char, 256);
 * ```
 */

/**
 * @def atlib_calloc(t, n)
 * @brief AtLib wrapper macro for the calloc function.
 * @param t The data type to allocate for.
 * @param n The number of objects to allocate for.
 * @returns A pointer to the allocated memory, or @c nullptr if an error occured.
 *
 * In debug mode, the wrapper adds additional header data to the allocated memory.
 * In release mode, the wrapper calls glibc @c calloc.
 */

/**
 * @def atlib_realloc(p, n)
 * @brief AtLib wrapper macro for the realloc function.
 * @param p The pointer to the allocated memory to resize.
 * @param n The number of bytes to resize to.
 */

#endif
