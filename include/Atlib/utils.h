#pragma once

#define __STRV(...) #__VA_ARGS__

#define __DEBUG_WRAP_FUNC(func, ...) __FDEBUG_WRAP_FUNC(stderr, func, __VA_ARGS__)
#define __FDEBUG_WRAP_FUNC(fp, func, ...) \
        do { \
            fprintf(fp, "DEBUG_WRAP_FUNC: Calling %s(%s) at %s:%d\n",\
                    __STRV(func), __STRV(__VA_ARGS__), __FILE__, __LINE__);\
            fflush(fp);\
        } while (0);
#define __FDEBUG_WRAP_FUNC_RET(fp, ret, func, ...) \
            do { \
                fprintf(fp, "DEBUG_WRAP_FUNC_RET: Calling *(%s) = %s(%s) on %s:%d\n",\
                        __STRV(ret), __STRV(func), __STRV(__VA_ARGS__), __FILE__, __LINE__);\
                fflush(fp);\
                *(ret) = (func)(__VA_ARGS__);\
            } while (0);
#define __DEBUG_WRAP_FUNC_RET(ret, func, ...) __FDEBUG_WRAP_FUNC_RET(stderr, ret, func, __VA_ARGS__)

#define __2FTDEBUG_DUMP(fp, type, val) \
        do { \
            fprintf(fp, "%s %s @ 0x%08X: %ld\n", __STRV(type), __STRV(val), &(val), val);\
            fflush(fp);\
        } while (0);
#define __FTDEBUG_DUMP(fp, type, val) __2FTDEBUG_DUMP(fp, type, val)
#define __FDEBUG_DUMP(fp, val) __2FTDEBUG_DUMP(fp, typeof(val), val)
#define __DEBUG_DUMP(val) __2FTDEBUG_DUMP(stderr, typeof(val), val)

#define __INTERNAL_OPTIONAL(type) \
    typedef struct { \
        unsigned char is_some; \
        type val; \
    } type##_optional_t; \
    static inline type##_optional_t type##_optional_none(void) { return (type##_optional_t){0}; } \
    static inline type##_optional_t type##_optional_some(type val) { return (type##_optional_t){.is_some = 1, .val = val}; } \
    static inline type              type##_optional_unwrap(type##_optional_t opt) { return opt.val; }
#define CREATE_OPTIONAL(type) __INTERNAL_OPTIONAL(type)

#define __INTERNAL_RESULT(type, error) \
    typedef union { \
        unsigned char is_ok; \
        struct { unsigned char is_ok; error val; } err; \
        struct { unsigned char is_ok; type val;  } ok; \
    } type##_##error##_result_t; \
    static inline type##_##error##_result_t type##_##error##_result_ok(type val) { return (type##_##error##_result_t){.ok={.is_ok=1, .val=val}}; } \
    static inline type##_##error##_result_t type##_##error##_result_err(error val) { return (type##_##error##_result_t){.err={.is_ok=0,.val=val}}; } \
    static inline type type##_##error##_result_unwrap_ok(type##_##error##_result_t res) { return res.ok.val; } \
    static inline error type##_##error##_result_unwrap_err(type##_##error##_result_t res) { return res.err.val; }
#define CREATE_RESULT(type, error) __INTERNAL_RESULT(type, error)
