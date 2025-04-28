#ifndef __ATLIB_STRING_H
#define __ATLIB_STRING_H

#error "The AtLib String module is not supported due to debug/release function collisions"

#include "Atlib/types.h"

/*! \defgroup string 
 * AtLib provides a string library that allows for safe manipulation and handling
 * of text. 
 * @{
 */

/*! \def __ATLIB_STRING_DEFLEN */
#define __ATLIB_STRING_DEFLEN 256

static inline __attribute__((pure, nonnull))
usize atlib_strhash(const char * str) {
    usize hash = 5381;
    i32 c;
    while((c = *str++)) { hash = ((hash << 5) + hash) + c; }
    return hash;
}

/*!
 * The string type for string. This typing allows for AtLib strings
 * to still be used with the C standard-library with no issue. As of now,
 * only ASCII is supported out of the box, Unicode must be manually handled
 * by the programmer if required.
 *
 * **NOTE:** while AtLib strings can be treated like plain C-style
 * strings, the reverse is not true. C-style strings *CANNOT* be treated
 * like AtLib strings, and instead must be passed to atlib_string_lit
 * if wished to be transformed into an AtLib string.
 */
typedef char string_t;

/*!
 * Creates an empty string_t object, initialized to hold __ATLIB_STRING_DEFLEN
 * characters.
 *
 * \return A pointer to a string_t object, or nullptr on error.
 *
 * \since v0.1.0
 */
extern string_t * atlib_string_create(void) __attribute__((malloc));

/*!
 * Creates an empty string_t object, initialized to hold \a _cap capacity
 * of characters.
 *
 * \arg \c _cap The default capacity of the string.
 *
 * \return A pointer to a string_t object, or nullptr on error.
 * \return If \a _cap is 0, the return value is always nullptr.
 *
 * \since v0.1.0
 */
extern string_t * atlib_string_create_capacity(usize _cap) __attribute__((malloc));

/*!
 * Creates a string_t object with the copied contents of \a _str.
 *
 * \arg \c _str The C-style string to wrap.
 *
 * \return A pointer to a string_t object, or nullptr on error.
 * \return If \a _str is nullptr, the return value is always nullptr.
 *
 * \since v0.1.0
 */
extern string_t * atlib_string_lit(const char * _str) __attribute__((malloc));

extern void atlib_string_destroy(string_t *) __attribute__((nonnull));

extern usize atlib_string_len(const string_t *) __attribute__((pure));
extern string_t * atlib_string_copy(string_t *__restrict, const string_t *__restrict);
extern string_t * atlib_string_cat(string_t *__restrict, const string_t *__restrict);
extern usize atlib_string_split(string_t **__restrict, usize, const string_t *__restrict, char);
extern i32 atlib_string_comp(const string_t *, const string_t *) __attribute__((pure));

/*! @} */

#endif
