#ifndef __ATLIB_ENDIAN_H
#define __ATLIB_ENDIAN_H

#include "Atlib/types.h"

#define ATLIB_BIG_ENDIAN        (4321)
#define ATLIB_LITTLE_ENDIAN     (5678)

#ifdef __BYTE_ORDER__
#  if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
#    define ATLIB_ENDIAN ATLIB_LITTLE_ENDIAN
#  else
#    define ATLIB_ENDIAN ATLIB_BIG_ENDIAN
#  endif
#elif defined(__amd64__) || defined(__amd64) || defined(__x86_64__) || defined(__x86_64)
#  define ATLIB_ENDIAN ATLIB_LITTLE_ENDIAN
#elif defined(__BIG_ENDIAN__) || defined(__BIG_ENDIAN) || defined(_BIG_ENDIAN)
#  define ATLIB_ENDIAN ATLIB_BIG_ENDIAN
#elif defined(_ARCH_PPC) || defined(__PPC__) || defined(__PPC) || defined(PPC) || defined(__powerpc__) || defined(__powerpc)
#  define ATLIB_ENDIAN ATLIB_BIG_ENDIAN
#else
#  define ATLIB_ENDIAN ATLIB_BIG_ENDIAN
#endif

#define ATLIB_WORDSIZE ((i32)(sizeof(void *)))

#endif
