#ifndef __ATLIB_LOG_H
#define __ATLIB_LOG_H

/**
 * @file log.h
 */

#include "Atlib/io/bufwrite.h"

#define ATLIB_ENVIRONMENT_VARIABLE_MAX_LEN 2048

/**
 * @brief Enumeration of logging levels to help aggregate logging information.
 */
typedef enum {
    ATLIB_LOG_DEBUG = 0,
    ATLIB_LOG_INFO,
    ATLIB_LOG_WARN,
    ATLIB_LOG_ERROR,
    ATLIB_LOG_FATAL = 255,
} log_level_e;

/**
 * @brief Structure to help organize logging using buffered writing and minimum levels.
 * @see atlib_log_open
 * @see atlib_log_close
 */
typedef struct {
    bufwrite_t bw;          ///< @brief The buffered writer to log.
    log_level_e min;        ///< @brief Theh minimum level of logging to log.
} log_t;

extern log_level_e atlib_log_level(const char * level_name);

/**
 * @brief Opens and initializes a @c log_t structure. File name must satisfy requirements of @ref atlib_bufwrite_open.
 * @param log Pointer to a @c log_t object.
 * @param file_name File path of file to start logging to.
 * @returns @c log on success, or @ref nullptr on error.
 *
 * The logging functionality of AtLib uses @ref bufwrite_t and the buffered writer API to handle interacting
 * with the file itself.
 *
 * @see atlib_bufwrite_open
 */
extern log_t * atlib_log_open(log_t *__restrict log, const char *__restrict file_name);

/**
 * @brief Opens and initializes a @c log_t structure. File must satisfy requirements of @ref atlib_bufwrite_fopen.
 * @param log Pointer to a @c log_t object.
 * @param file Pointer to a valid FILE object that can be written to.
 * @returns @c log on success, or @ref nullptr on error.
 * @see atlib_bufwrite_fopen
 */
extern log_t * atlib_log_fopen(log_t *__restrict log, FILE *__restrict file);

/**
 * @brief Closes a @c log_t object.
 * @param log Pointer to a valid @c log_t object.
 */
extern void atlib_log_close(log_t * log);

/**
 * @brief Writes a formatted value to @c log with a provided logging level.
 * @param log Pointer to a valid @c log_t object.
 * @param level String name of the logging level to mark the message.
 * @param file Name of the file.
 * @param line Line number.
 * @param fmt String formatting to log.
 */
extern void __attribute__((format (printf, 5, 6)))
    atlib_log_writef(log_t *__restrict log,
        const char *__restrict level, const char *__restrict file, i32 line,
        const char *__restrict fmt, ...);

/**
 * @def atlib_log_debug(log, fmt, ...)
 * @brief Logs a debugging message to @c log.
 * @param log Pointer to a valid @c log_t object.
 * @param fmt String fomratting to log.
 */
#define atlib_log_debug(log, fmt, ...) atlib_log_writef(log, "DEBUG", __FILE__, __LINE__, fmt, __VA_ARGS__)

/**
 * @def atlib_log_info(log, fmt, ...)
 * @brief Logs an information message to @c log.
 * @param log Pointer to a valid @c log_t object.
 * @param fmt String fomratting to log.
 */
#define atlib_log_info(log, fmt, ...) atlib_log_writef(log, "INFO", __FILE__, __LINE__, fmt, __VA_ARGS__)

/**
 * @def atlib_log_warn(log, fmt, ...)
 * @brief Logs a warning message to @c log.
 * @param log Pointer to a valid @c log_t object.
 * @param fmt String fomratting to log.
 */
#define atlib_log_warn(log, fmt, ...) atlib_log_writef(log, "WARN", __FILE__, __LINE__, fmt, __VA_ARGS__)

/**
 * @def atlib_log_error(log, fmt, ...)
 * @brief Logs an error message to @c log.
 * @param log Pointer to a valid @c log_t object.
 * @param fmt String fomratting to log.
 */
#define atlib_log_error(log, fmt, ...) atlib_log_writef(log, "ERROR", __FILE__, __LINE__, fmt, __VA_ARGS__)

/**
 * @def atlib_log_fatal(log, fmt, ...)
 * @brief Logs a fatal message to @c log.
 * @param log Pointer to a valid @c log_t object.
 * @param fmt String fomratting to log.
 */
#define atlib_log_fatal(log, fmt, ...) atlib_log_writef(log, "FATAL", __FILE__, __LINE__, fmt, __VA_ARGS__)

#ifdef __DEBUG__
#  define atlib_dbglog_debug(log, fmt, ...) atlib_log_writef(log, "DEBUG", __FILE__, __LINE__, fmt, __VA_ARGS__)
#  define atlib_dbglog_info(log, fmt, ...) atlib_log_writef(log, "INFO", __FILE__, __LINE__, fmt, __VA_ARGS__)
#  define atlib_dbglog_warn(log, fmt, ...) atlib_log_writef(log, "WARN", __FILE__, __LINE__, fmt, __VA_ARGS__)
#  define atlib_dbglog_error(log, fmt, ...) atlib_log_writef(log, "ERROR", __FILE__, __LINE__, fmt, __VA_ARGS__)
#  define atlib_dbglog_fatal(log, fmt, ...) atlib_log_writef(log, "FATAL", __FILE__, __LINE__, fmt, __VA_ARGS__)
#  define atlib_debug_loop(type, start, end, increment, statements) for(type __i = (type)(start); __i < (type)(end); __i += (type)(increment)) { statements; }
#else
#  define atlib_dbglog_debug(...) if (0) {}
#  define atlib_dbglog_info(...) if (0) {}
#  define atlib_dbglog_warn(...) if (0) {}
#  define atlib_dbglog_error(...) if(0) {}
#  define atlib_dbglog_fatal(...) if(0) {}
#  define atlib_debug_loop(...) do {} while (0)
#endif

#endif
