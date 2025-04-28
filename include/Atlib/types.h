#ifndef __ATLIB_TYPES_H
#define __ATLIB_TYPES_H

/**
 * @file types.h
 * @brief Defines common types used throughout AtLib.
 * Uses definitions from the @c bits glib header directory to base type definitions from.
 * This is done to ensure accuracy in type width specifiers across many systems.
 *
 * For example, when using a @c u32, the user can be certain they are working with 4 bytes
 * of contiguous memory that is interpreted as unsigned.
 */

#include <bits/types.h>
#include <bits/stdint-intn.h>
#include <bits/stdint-uintn.h>

#define ATAPI extern __attribute__((nonnull, nothrow))

#ifdef NULL
#undef NULL
#endif

#if (defined(__STDC__) && defined(__STDC_VERSION__) && __STDC_VERSION__ >= 202311L)
#include <stddef.h>
#else
#define nullptr ((void *)0)
typedef void * nullptr_t;
#endif
#define NULL nullptr

/**
 * @def NULL
 * @brief A simple macro designating an invalid memory address, typically used to show that something does not exist.
 * Same as nullptr.
 * @see nullptr
 */

/**
 * @def nullptr
 * @brief A simple macro designating an invalid memory address, typically used to show that something does not exist.
 * Same as NULL.
 * @see NULL
 */

typedef uint8_t u8;             ///< @brief An unsigned, 8-bit value.
typedef uint16_t u16;           ///< @brief An unsigned, 16-bit value.
typedef uint32_t u32;           ///< @brief An unsigned, 32-bit value.
typedef uint64_t u64;           ///< @brief An unsigned, 64-bit value.
typedef __uintmax_t usize;      ///< @brief An unsigned value. No size determination, useful for designating only an unsigned value.

typedef int8_t i8;              ///< @brief A signed, 8-bit value.
typedef int16_t i16;            ///< @brief A signed, 16-bit value.
typedef int32_t i32;            ///< @brief A signed, 32-bit value.
typedef int64_t i64;            ///< @brief A signed, 64-bit value.
typedef __intmax_t isize;       ///< @brief A signed value. No size determination, useful for designating only a signed value.

typedef float f32;              ///< @brief A 32-bit floating-point value.
typedef double f64;             ///< @brief A 64-bit floating-point value.
typedef long double f128;       ///< @brief A 128-bit floating-point value.

#endif
