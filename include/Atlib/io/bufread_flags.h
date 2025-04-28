#ifndef __ATLIB_BUFREAD_FLAGS_H
#define __ATLIB_BUFREAD_FLAGS_H

/**
 * @file bufread_flags.h
 */

#include "Atlib/io/endian.h"

/**
 * @def BUFREAD_FH_ATTACH
 * @brief Describes that this stream has an attached file handler handled external
 * of AtLib, avoiding cleaning the file up when closing the stream.
 */
#define BUFREAD_FH_ATTACH       ((u32)(1))

/**
 * @def BUFREAD_READ_BE
 * @brief Signals that this stream will interpret all multi-byte structures,
 * such as integers, in Big-Endian format.
 */
#define BUFREAD_READ_BE         ((u32)(1 << 1))

/**
 * @def BUFREAD_READ_LE
 * @brief Signals that this stream will interpret all multi-byte structures,
 * such as integers, in Little-Endian format.
 */
#define BUFREAD_READ_LE         ((u32)(1 << 2))

/**
 * @def BUFREAD_READ_NATIVE
 * @brief Signals that this stream will interpret all multi-byte structures,
 * such as integers, in the Endian-ness of the native machine.
 *
 * This macro is set to be @ref BUFREAD_READ_BE if @ref ATLIB_ENDIAN is set
 * to @ref ATLIB_BIG_ENDIAN, and @ref BUFREAD_READ_LE if @ref ATLIB_ENDIAN
 * is set to @ref ATLIB_LITTLE_ENDIAN.
 */
#if ATLIB_ENDIAN == ATLIB_BIG_ENDIAN
#define BUFREAD_READ_NATIVE     BUFREAD_READ_BE
#elif ATLIB_ENDIAN == ATLIB_LITTLE_ENDIAN
#define BUFREAD_READ_NATIVE     BUFREAD_READ_LE
#else
#error "ATLIB_ENDIAN has been modified or native endian-ness cannot be determined;" \
        "Do not directly modify this macro to ensure proper endian detection"
#endif

/**
 * @def BUFREAD_FLAG_DEFAULT
 * @brief Default set of flags to fallback on when creating a buffered stream,
 * when the user does not provide any to AtLib.
 */
#define BUFREAD_FLAG_DEFAULT    \
    (BUFREAD_READ_NATIVE)

#endif
