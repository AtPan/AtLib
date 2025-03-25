#include "Atlib/io/bufwrite.h"
#include <stdio.h>
#define __ATLIB_NEED_MAIN
#include "Atlib/main.h"
#include "Atlib/error.h"

static bufread_t __br;
bufread_t * const bufstdin = &__br;

static bufwrite_t __bw;
bufwrite_t * const bufstdout = &__bw;

i32 atlib_start() {
    i32 code = 0;

    atlib_error_init();
    if(atlib_bufread_fopen(bufstdin, stdin) == nullptr) {
        code = 1;
        goto stdin_err;
    }
    if(atlib_bufwrite_fopen(bufstdout, stdout) == nullptr) {
        code = 2;
        goto stdin_err;
    }

stdin_err:
    return code;
}

i32 atlib_end() {
    i32 code = 0;

    atlib_bufread_close(bufstdin);
    atlib_bufwrite_close(bufstdout);
    atlib_error_close();

    return code;
}
