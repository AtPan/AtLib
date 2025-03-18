#ifndef __ATLIB_OPTIONAL_H
#define __ATLIB_OPTIONAL_H

/**
 * @file optional.h
 */

#include "Atlib/types.h"

/**
 * @brief Tags used to keep track of the optional type.
 */
enum atlib_optional_tag {
    ATLIB_OPTIONAL_NONE = 0,
    ATLIB_OPTIONAL_i8,
    ATLIB_OPTIONAL_i16,
    ATLIB_OPTIONAL_i32,
    ATLIB_OPTIONAL_i64,
    ATLIB_OPTIONAL_u8,
    ATLIB_OPTIONAL_u16,
    ATLIB_OPTIONAL_u32,
    ATLIB_OPTIONAL_u64,
    ATLIB_OPTIONAL_f32,
    ATLIB_OPTIONAL_f64,
    ATLIB_OPTIONAL_ptr,
};

/**
 * @brief Tagged union to represent the existence, or absence, of a type.
 * 
 * Due to the complexity of manually configuring, synchronizing, and using tagged 
 * unions (and unions in general), it is recommended to use one of the pre-built
 * structs through the macros defined in this file instead.
 *
 * @see ATLIB_OPTIONAL_NONE
 * @see ATLIB_OPTIONAL_SOME
 */
typedef struct {
    enum atlib_optional_tag tag;    ///< @brief Tag of the union.
    union {
        i8  _i8;                    ///< @brief Union's @c i8 representation.
        i16 _i16;                   ///< @brief Union's @c i16 representation.
        i32 _i32;					///< @brief Union's @c i32 representation.
        i64 _i64;					///< @brief Union's @c i64 representation.
        u8  _u8;					///< @brief Union's @c u8 representation.
        u16 _u16;					///< @brief Union's @c u16 representation.
        u32 _u32;					///< @brief Union's @c u32 representation.
        u64 _u64;					///< @brief Union's @c u64 representation.
        f32 _f32;					///< @brief Union's @c f32 representation.
        f64 _f64;					///< @brief Union's @c f64 representation.
        void * ptr;
    };
} optional_t;

#define ATLIB_OPT_STR2(_t) ATLIB_OPTIONAL_ ## _t
#define ATLIB_OPT_STR(_t) ATLIB_OPT_STR2(_t)

/**
 * @def ATLIB_OPTIONAL_NONE
 * @brief Pre-built optional struct that represents the @c None type, or the absence of a value.
 */
#define ATLIB_OPTIONAL_NONE ((optional_t){.tag = ATLIB_OPTIONAL_NONE})

/**
 * @def ATLIB_OPTIONAL_SOME(t, d)
 * @brief Pre-built optional struct that represents the @c Some type, or the presence of a value.
 * @param t Type of the data to store in the optional.
 * @param d Data to store within the optional.
 * @returns A properly-tagged optional.
 *
 * Proper values of @c t will be the final segment of values in the @ref atlib_optional_tag enum.
 * The only exception is the @c ATLIB_OPTIONAL_NONE enum variant, which will not compile if used.
 */
#define ATLIB_OPTIONAL_SOME(t, d) ((optional_t){.tag = ATLIB_OPT_STR(t), ._ ## t = (d)})

/**
 * @brief Used to validate the tag of an optional struct.
 * @param o Optional structure to validate.
 * @param tag Tag to use to validate the optional.
 * @returns Non-zero if @c o is tagged with tag @c t, zero otherwise.
 */
static inline __attribute__((always_inline, pure)) 
    i32 __atlib_optional_tag_validate(optional_t o, enum atlib_optional_tag tag) 
    { return o.tag == tag; }

#endif
