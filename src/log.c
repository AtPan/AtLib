#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <stdarg.h>

#include "Atlib/io/log.h"
#include "Atlib/error.h"

log_level_e atlib_log_level(const char * level_name) {
    if(!level_name) return ATLIB_LOG_DEBUG;
#define strncmp(a, b, c) !strncmp(a, b, c)
    if(strncmp(level_name, "DEBUG", ATLIB_ENVIRONMENT_VARIABLE_MAX_LEN)) {
        return ATLIB_LOG_DEBUG;
    }
    if(strncmp(level_name, "INFO", ATLIB_ENVIRONMENT_VARIABLE_MAX_LEN)) {
        return ATLIB_LOG_INFO;
    }
    if(strncmp(level_name, "WARN", ATLIB_ENVIRONMENT_VARIABLE_MAX_LEN)) {
        return ATLIB_LOG_WARN;
    }
    if(strncmp(level_name, "ERROR", ATLIB_ENVIRONMENT_VARIABLE_MAX_LEN)) {
        return ATLIB_LOG_ERROR;
    }
    if(strncmp(level_name, "FATAL", ATLIB_ENVIRONMENT_VARIABLE_MAX_LEN)) {
        return ATLIB_LOG_FATAL;
    }
    return ATLIB_LOG_DEBUG;
#undef strncmp
}

log_t * atlib_log_open(log_t * restrict log, const char * restrict file_name) {
    atlib_compassert(log);

    log->min = atlib_log_level(getenv("ATLIB_LOGLEVEL"));
    if(!atlib_bufwrite_open(&log->bw, file_name)) return NULL;
    return log;
}

log_t * atlib_log_fopen(log_t * restrict log, FILE * restrict file) {
    atlib_compassert(log);
    atlib_compassert(file);

    log->min = atlib_log_level(getenv("ATLIB_LOGLEVEL"));
    if(!atlib_bufwrite_fopen(&log->bw, file)) return NULL;
    return log;
}

void atlib_log_close(log_t * log) {
    atlib_compassert(log);

    atlib_bufwrite_close(&log->bw);
}

void atlib_log_writef(
        log_t *restrict log,
        const char *restrict level,
        const char *restrict file,
        i32 line, const char *restrict fmt, ...)
{
    atlib_compassert(log);
    atlib_compassert(level);
    atlib_compassert(file);
    atlib_compassert(fmt);

    if(log->min > atlib_log_level(level)) return;

    time_t t = time(NULL);
    struct tm * _tm = localtime(&t);

    va_list ap;
    va_start(ap, fmt);

    (void)atlib_bufwrite_writef(&log->bw, "%02d:%02d:%02d %s %s:%d: ",
            _tm->tm_hour, _tm->tm_min, _tm->tm_sec,
            level, file, line);
    (void)atlib_bufwrite_writefv(&log->bw, fmt, ap);
    (void)atlib_bufwrite_flush(&log->bw);

    va_end(ap);
}
