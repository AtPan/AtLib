#ifndef __ATLIB_BUFWRITE_H
#define __ATLIB_BUFWRITE_H

#include "Atlib/types.h"
#include <bits/types/FILE.h>
#include <stdio.h>

typedef __builtin_va_list va_list;

#ifndef __ATLIB_BUFWRITE_SIZE
#define __ATLIB_BUFWRITE_SIZE 4096
#endif

/**
 * @brief Structure that buffers outgoing data to improve the efficiency of writing to physical media.
 *
 * A Buffer Writer structure. Used to write buffered data to a file.
 * The data can be either text (ASCII, UTF-8, etc.), or binary data.
 * The user is responsible for ecoding the text. This struct only acts 
 * as an intermediate buffer between a user and a file.
 * The internal buffer's size is controlled by the macro __ATLIB_BUFWRITE_SIZE,
 * which defaults to '4096'. To act as an unbuffered writer, define this value
 * to '1'.
 *
 * @warning A valid Buffered Writer, or `bufwrite_t` object, is any Buffered Writer
 * that has been initialized through @c atlib_bufwrite_open. Usage of a non-valid
 * Buffered Writer is undefined behavior.
 *
 * @see atlib_bufwrite_open
 * @see atlib_bufwrite_close
 */
typedef struct {
    FILE * fh;                          ///< @brief The file handler attached to this buffered writer.
    isize to_write;                     ///< @brief The number of bytes currently free in the buffer.
    char * next;                        ///< @brief The pointer to the next byte to write to.
    char buf[__ATLIB_BUFWRITE_SIZE];    ///< @brief The buffer to store data.
} bufwrite_t;

/**
 * @brief Initializes @c bw to buffer outgoing inforamtion to the file referred to by @c file_path.
 * @param bw Pointer to a @c bufwrite_t object.
 * @param file_path Path to file to open.
 * @returns @c bw, pointing to a valid @c bufwrite_t object on success, or @c nullptr on error.
 */
extern bufwrite_t * atlib_bufwrite_open(bufwrite_t *__restrict bw, const char *__restrict file_path);

/**
 * @brief Initializes @c bw to buffer outgoing inforamtion to @c file.
 * @param bw Pointer to a @c bufwrite_t object.
 * @param file Pointer to a @c FILE object, opened in any mode that is NOT "r".
 * @returns @c bw, pointing to a valid @c bufwrite_t object on success, or @c nullptr on error.
 */
extern bufwrite_t * atlib_bufwrite_fopen(bufwrite_t *__restrict bw, FILE *__restrict file);

/**
 * @brief Closes @c bw and uninitializes a valid @c bufwrite_t object.
 * @param bw Pointer to a valid @c bufwrite_t object.
 */
extern void atlib_bufwrite_close(bufwrite_t * bw);

/**
 * @brief Flushes all pending writes to the underlying media.
 * @param bw Pointer to a valid @c bw object.
 * @returns Number of bytes written to the underlying media.
 */
extern usize atlib_bufwrite_flush(bufwrite_t * bw);

/**
 * @brief Writes an object pointed to by @c data of @c n bytes to @c bw.
 * @param bw Pointer to a valid @c bufwrite_t object.
 * @param data Pointer to an object to write.
 * @param n Number of bytes to write.
 * @returns Number of bytes written in the current buffer.
 *
 * If @c n is larger than what the current buffer is available is capable of holding, the remaining
 * space is filled and flushed, and the process is restarted with an empty buffer.
 */
extern usize atlib_bufwrite_write(bufwrite_t *__restrict bw, const void *__restrict data, isize n);

/**
 * @brief Writes the formatted data to @c bw.
 * @param bw Pointer to a valid @c bufwrite_t object.
 * @param fmt String format to write to @c bw.
 * @returns Number of bytes written into the current buffer.
 */
extern usize __attribute__((format (printf, 2, 3)))
    atlib_bufwrite_writef(bufwrite_t *__restrict bw, const char *__restrict fmt, ...);

/**
 * @brief Writes the formatted data to @c bw, using the argument list @c ap.
 * @param bw Pointer to a valid @c bufwrite_t object.
 * @param fmt String format to write to @c bw.
 * @param ap Variable argument list to format with.
 * @returns Number of bytes written into the current buffer.
 */
extern usize atlib_bufwrite_writefv(bufwrite_t *__restrict bw, const char *__restrict fmt, va_list ap);

/**
 * @brief Writes raw bytes of @c v to @c bw.
 * @param bw Pointer to a valid @c bufwrite_t object.
 * @param v Bytes to write to @c bw.
 */
extern void atlib_bufwrite_write_u8(bufwrite_t *__restrict bw, u8 v);

/**
 * @brief Writes raw bytes of @c v to @c bw.
 * @param bw Pointer to a valid @c bufwrite_t object.
 * @param v Bytes to write to @c bw.
 */
extern void atlib_bufwrite_write_u16(bufwrite_t *__restrict bw, u16 v);

/**
 * @brief Writes raw bytes of @c v to @c bw.
 * @param bw Pointer to a valid @c bufwrite_t object.
 * @param v Bytes to write to @c bw.
 */
extern void atlib_bufwrite_write_u32(bufwrite_t *__restrict bw, u32 v);

/**
 * @brief Writes raw bytes of @c v to @c bw.
 * @param bw Pointer to a valid @c bufwrite_t object.
 * @param v Bytes to write to @c bw.
 */
extern void atlib_bufwrite_write_u64(bufwrite_t *__restrict bw, u64 v);

/**
 * @brief Writes raw bytes of @c v to @c bw.
 * @param bw Pointer to a valid @c bufwrite_t object.
 * @param v Bytes to write to @c bw.
 */
extern void atlib_bufwrite_write_i8(bufwrite_t *__restrict bw, i8 v);

/**
 * @brief Writes raw bytes of @c v to @c bw.
 * @param bw Pointer to a valid @c bufwrite_t object.
 * @param v Bytes to write to @c bw.
 */
extern void atlib_bufwrite_write_i16(bufwrite_t *__restrict bw, i16 v);

/**
 * @brief Writes raw bytes of @c v to @c bw.
 * @param bw Pointer to a valid @c bufwrite_t object.
 * @param v Bytes to write to @c bw.
 */
extern void atlib_bufwrite_write_i32(bufwrite_t *__restrict bw, i32 v);

/**
 * @brief Writes raw bytes of @c v to @c bw.
 * @param bw Pointer to a valid @c bufwrite_t object.
 * @param v Bytes to write to @c bw.
 */
extern void atlib_bufwrite_write_i64(bufwrite_t *__restrict bw, i64 v);

/**
 * @brief Finds the byte position of the current stream.
 * @param bw Pointer to the stream.
 * @returns Byte position of the current stream.
 */
static inline usize atlib_bufwrite_pos(const bufwrite_t * bw) { return ftell(bw->fh) + __ATLIB_BUFWRITE_SIZE - bw->to_write; }

/**
 * @brief Finds the byte position of the unbuffered stream.
 * @param bw Pointer to the stream.
 * @returns Byte position of the current underlying stream.
 */
static inline usize atlib_bufwrite_fpos(const bufwrite_t * bw) { return ftell(bw->fh); }

/**
 * @brief Checks if the stream is errored.
 * @param bw Pointer to the stream.
 * @returns Non-zero if the stream is errored, zero otherwise.
 */
static inline usize atlib_bufwrite_err(const bufwrite_t * bw) { return ferror(bw->fh); }

#endif
