#ifndef __ATLIB_ERROR_H
#define __ATLIB_ERROR_H

/**
 * @file error.h
 */

#define ATLIB_ASSERT_ERRCODE 255

#include "Atlib/io/log.h"

/**
 * @def __ATOUT__
 * @brief The string name of the file to represent as the "stdout log".
 * If not provided, the default file to open is "out.txt".
 */

#ifndef __ATOUT__
#define __ATOUT__ "out.txt"
#endif

/**
 * @def __ATERR__
 * @brief The string name of the file to represent as the "stderr log".
 * If not provided, the default is the same as __ATOUT__.
 */

#ifndef __ATERR__
#define __ATERR__ __ATOUT__
#endif

/**
 * @def atlib_compassert(e)
 * @brief Assertion macro that only functions in debug mode. Otherwise it is a noop, no matter the expression.
 * @param e The expression to evaluate.
 */

/**
 * @var atout
 * @brief Global pointer to a log, as a "stdout log" equivalent.
 */

/**
 * @var aterr
 * @brief Global pointer to a log, as a "stderr log" equivalent.
 */

/**
 * @def atlib_assert(e)
 * @brief AtLib's assertion wrapper. If the expression is false, the program's execution will quit.
 * @param e The assertion to evaluate.
 */

/**
 * @fn __atlib_assert_func(expval, expression, file, line)
 * @brief AtLib's assertion function called by @ref atlib_assert and @ref atlib_compassert. It is not recommended to use this function manually.
 * @param expval The result of the evaluated expression.
 * @param expression A string of the expression.
 * @param file The name of the file the assertion is in.
 * @param line The line number the assertion is on.
 *
 * It is not recommended to use this function manually, instead use the macro @ref atlib_assert or @ref atlib_compassert
 * to avoid setting the arguments by hand. The macro can call upon the C-preprocessor to handle many of this function's
 * arguments for the user. Avoid using this function by hand where possible.
 */

#ifdef __DEBUG__
#  define atlib_compassert(exp) __atlib_assert_func((isize)(exp), #exp, __FILE__, __LINE__)
#  define atout __atlib_atout(__FILE__, __LINE__)
#  define aterr __atlib_aterr(__FILE__, __LINE__)
extern log_t * __atlib_atout(const char * file, i32 line) __attribute__ ((returns_nonnull));
extern log_t * __atlib_aterr(const char * file, i32 line) __attribute__ ((returns_nonnull));
#else
#  define atlib_compassert(exp) do {} while(0)
extern log_t * aterr;
extern log_t * atout;
#endif

#define atlib_assert(exp) __atlib_assert_func((isize)(exp), #exp, __FILE__, __LINE__)
extern void __atlib_assert_func(isize expval, const char * expression, const char * file, i32 line);

#ifdef __ATLIB_NEED_MAIN
extern void atlib_error_init(void);
extern void atlib_error_close(void);
#endif

#endif
