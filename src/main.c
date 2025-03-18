#include <stdio.h>
#define __ATLIB_NEED_MAIN
#include "Atlib/main.h"
#include "Atlib/error.h"

static bufread_t __br;
bufread_t * bufstdin = &__br;

i32 atlib_start() {
    i32 code = 0;

    atlib_error_init();
    if(atlib_bufread_fopen(bufstdin, stdin) == NULL) {
        code = 1;
        goto stdin_err;
    }

stdin_err:
    return code;
}

i32 atlib_end() {
    i32 code = 0;

    atlib_bufread_close(bufstdin);
    atlib_error_close();

    return code;
}
