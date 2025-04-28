#ifndef __ATLIB_SLICE_H
#define __ATLIB_SLICE_H

/**
 * @file slice.h
 */

#ifdef __DEBUG__
#include "../types.h"

#  define atlib_malloc(t, n)  __atlib_malloc(sizeof(t), n, __FILE__, __LINE__)
#  define atlib_malloc_raw(b) __atlib_malloc(b, 1, __FILE__, __LINE__)
#  define atlib_calloc(t, n)  __atlib_calloc(sizeof(t), n, __FILE__, __LINE__)
#  define atlib_realloc(p, n) __atlib_realloc(p, n, __FILE__, __LINE__)
#  define atlib_free(p)       __atlib_free(p, __FILE__, __LINE__)
#  define memcpy(dest, src, n) __atlib_memcpy(dest, src, n, __FILE__, __LINE__)

ATAPI void * __atlib_malloc(isize, isize, const char *, u32) __attribute__((malloc,warn_unused_result));
ATAPI void * __atlib_calloc(isize, isize, const char *, u32) __attribute__((malloc,warn_unused_result));
extern void * __atlib_realloc(void *, isize, const char *, u32) __attribute__((nothrow,nonnull(3),warn_unused_result));
ATAPI void   __atlib_free(void *, const char *, u32);
ATAPI void   __atlib_memory_cleanup(void) __attribute__((destructor,error("Do not call __atlib_memory_cleanup manually.")));
ATAPI void * __atlib_memcpy(void * dest, const void * src, isize n, const char * restrict fname, u32 ln);

#else

#  include <stdlib.h>
#  define atlib_malloc(t, n)  malloc(sizeof(t) * n)
#  define atlib_malloc_raw(b) malloc(b)
#  define atlib_calloc(t, n)  calloc(n, sizeof(t))
#  define atlib_realloc(p, n) realloc(p, n)
#  define atlib_free(p)       free(p)

#endif /* __DEBUG__ */

/**
 * @def atlib_malloc(t, n)
 * @brief Allocates memory for @c n objects of type @c t.
 * @param t Type to allocate for.
 * @param n Number of objects to allocate.
 * @returns Pointer to allocated memory, or @c nullptr on error.
 *
 * This is a macro wrapper that AtLib uses to control memory requests depending on the build type.
 * In debug mode, this wrapper calls the internal function @c __atlib_malloc, which trakcs the size of
 * the memory request, where the memory was requested, among other information. This information is
 * used to ensure the user is properly managing their memory, and checked upon application termination
 * through the internal function @c __atlib_memory_cleanup. If inproper usage is detected, 
 *
 * Outside of debug mode, this macro makes a simple call to the glibc function @c malloc. Use debug
 * mode to ensure proper usage first and solve improper usage, then exit debug mode to take advantage
 * of C's and glibc's speed.
 */

/**
 * @def atlib_malloc_raw(b)
 * @brief Allocates enough memory to accomidate @c bytes.
 * @param b Size of buffer to allocate, in bytes.
 * @returns Pointer to allocated memory, or @c nullptr on error.
 *
 * Similar to @c atlib_malloc, but instead uses a direct byte amount instead of object type
 * and number of objects. See @ref atlib_malloc for more information. 
 * Equal to @c atlib_malloc(u8,b).
 *
 * @see atlib_malloc
 */

/**
 * @def atlib_calloc(t, n)
 * @brief Allocates enough zero-initialized memory to accomidate @c n objects of type @c t.
 * @param t Type to allocate for.
 * @param n Number of objects to allocate.
 * @returns Pointer to allocated memory, or @c nullptr on error.
 *
 * Calls @c atlib_malloc under the hood, and initializes the memory to zero. For more information,
 * see @ref atlib_malloc.
 *
 * @see atlib_malloc
 */

/**
 * @def atlib_realloc(p, n)
 * @brief Modifies size of buffer pointed to by @c p to size @c n, in bytes.
 * @param p Pointer to buffer to resize.
 * @param n Number of bytes to resize to.
 * @returns Pointer to allocated buffer, or @c nullptr on error.
 *
 * AtLib's implementation of @c realloc will use the size information of the initial type
 * the buffer was allocated for to sanity check the new resize request.
 *
 * For example:
 *
 * @code{.c}
 * i32 * intbuf = atlib_malloc(i32, 4); // 16 bytes
 * intbuf = atlib_realloc(intbuf, 11);  // 11 is NOT a multiple of type i32, thus a warning is logged
 * atlib_free(intbuf);
 * @endcode
 *
 * The above code will log a warning to @ref aterr stating that the new request is not large enough to
 * fit an even number of objects the pointer was allocated for. This is done to ensure that the proper
 * amount of memory is available to avoid out-of-bounds accessing and writing past what was allocated.
 *
 * This function calls glibc's @c realloc under the hood, so all of the same quirks as glibc's
 * implementation applies.
 *
 * @warning The pointer passed to this function must be a pointer returned from @c atlib_malloc,
 * @c atlib_malloc_raw, @c atlib_calloc, or @c atlib_realloc.
 *
 * @see atlib_malloc
 */

/**
 * @def atlib_free(p)
 * @brief Frees a buffer pointed to by @c p and returns its memory to the system.
 * @param p Pointer returned by @ref atlib_malloc, @ref atlib_malloc_raw, @ref atlib_calloc, or @ref atlib_realloc.
 */

/**
 * @def memcpy(dest, src, n)
 * @brief Macro wrapper of glibc's @c memcpy for safe aliasing bounds check.
 * @param dest Pointer to memory buffer to write to.
 * @param src Pointer to memory buffer to read from.
 * @param n Number of bytes to transfer from @c src to @c dest.
 *
 * This wrapper simply checks if @c dest or @c src alias with regards to length @c n bytes.
 * If there is any aliasing detected, a warning is logged to @ref aterr.
 */

#endif
