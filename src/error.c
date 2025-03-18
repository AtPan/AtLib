#include <stdlib.h>
#include <stdio.h>
#include "Atlib/error.h"

#ifdef atout
#undef atout
#endif

#ifdef aterr
#undef aterr
#endif

static log_t __atout = {0};
static log_t __aterr = {0};

log_t * atout = NULL;
log_t * aterr = NULL;

log_t * __atlib_atout(const char * file, i32 line) {
    if(!atout || (usize)atout != (usize)&__atout) {
        fprintf(stderr, "%s:%d: FATAL: Retrieving unset `atout`! Did you forget to call `atlib_error_init`? "
                "Are you retrieving after calling `atlib_error_close`?\n",
                file, line);
        exit(ATLIB_ASSERT_ERRCODE);
    }
    else return atout;
}

log_t * __atlib_aterr(const char * file, i32 line) {
    if(!aterr || (usize)aterr != (usize)&__aterr) {
        fprintf(stderr, "%s:%d: FATAL: Retrieving unset `aterr`! Did you forget to call `atlib_error_init`? "
                "Are you retrieving after calling `atlib_error_close`?\n",
                file, line);
        exit(ATLIB_ASSERT_ERRCODE);
    }
    else return aterr;
}

void atlib_error_init() {
    if(!atout) atout = &__atout;
    if(!aterr) aterr = &__aterr;

    atout = atlib_log_open(atout, __ATOUT__);
    aterr = atlib_log_open(aterr, __ATERR__);
}

void atlib_error_close() {
    if(atout) atlib_log_close(atout);
    if(aterr) atlib_log_close(aterr);
    atout = aterr = NULL;
}

void __atlib_assert_func(isize expval, const char * expression, const char * file, i32 line) {
    if(!aterr || (usize)aterr != (usize)&__aterr) {
        fprintf(stderr, "%s:%d: WARNING: Call to `atlib_assert` with invalid `aterr`! "
                "Did you forget to call `atlib_error_open`? Did you call after `atlib_error_close`?\n",
                file, line);
        return;
    }
    if(expval) return;

    atlib_log_writef(aterr, "FATAL (ASSERT)", file, line, "`atlib_assert(%s)` FAILED\n", expression);
    exit(ATLIB_ASSERT_ERRCODE);
}
