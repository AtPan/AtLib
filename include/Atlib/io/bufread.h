#ifndef __ATLIB_BUFREAD_H
#define __ATLIB_BUFREAD_H

/**
 * @file bufread.h
 */

#include "Atlib/types.h"
#include <fcntl.h>
#include <bits/types/FILE.h>
#include <stdio.h>

/**
 * @def __ATLIB_BUFREAD_SIZE
 * @brief The size of the internal buffer of the buffered reader type, in bytes. If not provided, the default value is 4096.
 */

#ifndef __ATLIB_BUFREAD_SIZE
#define __ATLIB_BUFREAD_SIZE 4096
#endif

/**
 * @brief Structure that buffers input data to improve reading from physical or virtual media.
 *
 * A Buffer Reader structure. Used to read in buffered data from a file.
 * The data can be either text (ASCII, UTF-8, etc.), or binary data.
 * The user is responsible for decoding the information. This struct only acts 
 * as an intermediate buffer between a user and the source.
 *
 * The internal buffer's size is controlled by the macro __ATLIB_BUFREAD_SIZE,
 * which defaults to '4096'. To act as an unbuffered reader, define this value
 * to '1'.
 *
 * @warning A valid Buffered Reader, or `bufread_t` object, is any Buffered Reader
 * that has been initialized (see `open_bufread`).
 * The use of any non-valid Buffered Reader object is undefined behavior.
 *
 * @see atlib_bufread_open
 * @see atlib_bufread_close
 */
typedef struct {
    FILE * fh;                      ///< @brief The FILE handler to read from.
    isize to_read;                  ///< @brief The number of unread bytes in the buffer before the next refill.
    int attach;                     ///< @brief If the FILE handler was attached as a handler, or opened by AtLib.
    char * next;                    ///< @brief The pointer to the next byte to read.
    char buf[__ATLIB_BUFREAD_SIZE]; ///< @brief The buffer to store data.
} bufread_t;

/**
 * @brief Initializes a @c bufread_t object to a valid state.
 * @param br A pointer to a @c bufread_t object.
 * @param file_path The path of the file to open.
 * @returns Pointer to @c br on success, or @c nullptr if an error occured.
 *
 * The file specified by in @c file_path is opened in read-only mode.
 *
 * To initialize a @c bufread_t from a FILE pointer instead, use
 * @ref atlib_bufread_fopen instead.
 *
 * @warning If @c br already points to a valid @c bufread_t object, it is
 * overridden and will leave the associated file dangling. Always close a
 * @c bufread_t object.
 *
 * @see atlib_bufread_fopen
 * @see atlib_bufread_close
 */
extern bufread_t * atlib_bufread_open(bufread_t *__restrict br, const char *__restrict file_path);

/**
 * @brief Initializes a @c bufread_t object to a valid state.
 * @param br Pointer to a @c bufread_t object.
 * @param file Pointer to a FILE object.
 * @returns Pointer to @c br on success, or @c nullptr if an error occured.
 *
 * This function takes a pointer to an already opened FILE object, instead
 * of a file path to open. The provided FILE pointer must have been opened in
 * read mode (i.e. r, r+, w+).
 *
 * To initialize a @c bufread_t object from a file path instead, use
 * @ref atlib_bufread_open instead.
 *
 * @warning The caller is responsible for ensuring that the FILE is properly closed.
 * Because AtLib did not open this file, it will not assume responsibility for closing
 * it either.
 *
 * @see atlib_bufread_open
 * @see atlib_bufread_close
 */
extern bufread_t * atlib_bufread_fopen(bufread_t *__restrict br, FILE *__restrict file);

/**
 * @brief Closes and invalidates a @c bufread_t object.
 * @param br Pointer to @c bufread_t object to close.
 *
 * Once passed to this function, the same pointer should not be used
 * again unless re-initialized again with either @ref atlib_bufread_open 
 * or @ref atlib_bufread_fopen.
 *
 * @warning AtLib will only close the associated file handle if and only if
 * the file was opened by AtLib through @ref atlib_bufread_open. The
 * caller is responsible for ensuring the file is closed if initialized
 * through @ref atlib_bufread_fopen.
 */
extern void atlib_bufread_close(bufread_t * br);

/**
 * @brief Reads at most @c n bytes from @c br into @c buf, or until @c '\n' has been encountered.
 * @param br Pointer to a valid @c bufread_t object to read from.
 * @param buf The buffer to read into.
 * @param n The number of bytes to read. Must be non-negative (>0).
 * @returns The number of bytes read.
 *
 * Will read the next "line" of text, ending at the first occurence of a new-line character.
 * If a new-line character is not found within the first @c n bytes, then the function quits
 * with only having read the first @c n bytes.
 */
extern isize atlib_bufread_read_nline(bufread_t *__restrict br, void *__restrict buf, isize n);

/*
 * @brief Reads @c n bytes from @c br into @c buf.
 * @param br Pointer to a valid @c bufread_t object to read from.
 * @param buf The buffer to read into.
 * @param n The number of bytes to read.
 * @returns The number of bytes read. If less than @c n, then @c EOF has been encountered.
 */
extern isize atlib_bufread_readn(bufread_t *__restrict br, void *__restrict buf, isize n);

/**
 * @brief Reads a @c u8 from @c br.
 * @param br Pointer to a valid @c bufread_t to read from.
 * @returns The next u8 in the stream.
 * If @c br has reached @c EOF, then this function will return 0. In such cases,
 * to determine if the next value was 0 or if @c EOF was reached, call @ref
 * atlib_bufread_eof to determine.
 *
 * @see atlib_bufread_eof
 */
extern u8 atlib_bufread_read_u8(bufread_t * br);

/**
 * @brief Reads a @c u16 from @c br.
 * @param br Pointer to a valid @c bufread_t to read from.
 * @returns The next u16 in the stream.
 * If @c br has reached @c EOF, then this function will return 0. In such cases,
 * to determine if the next value was 0 or if @c EOF was reached, call @ref
 * atlib_bufread_eof to determine.
 *
 * @see atlib_bufread_eof
 */
extern u16 atlib_bufread_read_u16(bufread_t * br);

/**
 * @brief Reads a @c u32 from @c br.
 * @param br Pointer to a valid @c bufread_t to read from.
 * @returns The next u32 in the stream.
 * If @c br has reached @c EOF, then this function will return 0. In such cases,
 * to determine if the next value was 0 or if @c EOF was reached, call @ref
 * atlib_bufread_eof to determine.
 *
 * @see atlib_bufread_eof
 */
extern u32 atlib_bufread_read_u32(bufread_t * br);

/**
 * @brief Reads a @c u64 from @c br.
 * @param br Pointer to a valid @c bufread_t to read from.
 * @returns The next u64 in the stream.
 * If @c br has reached @c EOF, then this function will return 0. In such cases,
 * to determine if the next value was 0 or if @c EOF was reached, call @ref
 * atlib_bufread_eof to determine.
 *
 * @see atlib_bufread_eof
 */
extern u64 atlib_bufread_read_u64(bufread_t * br);

/**
 * @brief Reads a @c i8 from @c br.
 * @param br Pointer to a valid @c bufread_t to read from.
 * @returns The next i8 in the stream.
 * If @c br has reached @c EOF, then this function will return 0. In such cases,
 * to determine if the next value was 0 or if @c EOF was reached, call @ref
 * atlib_bufread_eof to determine.
 *
 * @see atlib_bufread_eof
 */
extern i8 atlib_bufread_read_i8(bufread_t * br);
/**
 * @brief Reads a @c i16 from @c br.
 * @param br Pointer to a valid @c bufread_t to read from.
 * @returns The next i16 in the stream.
 * If @c br has reached @c EOF, then this function will return 0. In such cases,
 * to determine if the next value was 0 or if @c EOF was reached, call @ref
 * atlib_bufread_eof to determine.
 *
 * @see atlib_bufread_eof
 */
extern i16 atlib_bufread_read_i16(bufread_t * br);
/**
 * @brief Reads a @c i32 from @c br.
 * @param br Pointer to a valid @c bufread_t to read from.
 * @returns The next i32 in the stream.
 * If @c br has reached @c EOF, then this function will return 0. In such cases,
 * to determine if the next value was 0 or if @c EOF was reached, call @ref
 * atlib_bufread_eof to determine.
 *
 * @see atlib_bufread_eof
 */
extern i32 atlib_bufread_read_i32(bufread_t * br);
/**
 * @brief Reads a @c i54 from @c br.
 * @param br Pointer to a valid @c bufread_t to read from.
 * @returns The next i54 in the stream.
 * If @c br has reached @c EOF, then this function will return 0. In such cases,
 * to determine if the next value was 0 or if @c EOF was reached, call @ref
 * atlib_bufread_eof to determine.
 *
 * @see atlib_bufread_eof
 */
extern i64 atlib_bufread_read_i64(bufread_t * br);

/**
 * @brief Skips @c n bytes in the stream, effectively throwing them away.
 * @param br Pointer to a valid @c bufread_t object to skip.
 * @param n The number of bytes to skip.
 * 
 * If the skip is large enough, a buffer refresh will be required.
 * If skipping too far, @c br will encounter @c EOF.
 *
 * @see atlib_bufread_rewind
 * @see atlib_bufread_seek
 */
extern void atlib_bufread_skip(bufread_t * br, isize n);

/**
 * @brief Rewinds @c n bytes in the stream.
 * @param br A valid @c bufread_t object to rewind.
 * @param n Number of bytes to rewind.
 *
 * If the rewind is large enough, a buffer refresh will be required.
 * If rewinding far enough, the file handler is returned to the start.
 * If @c EOF is set, it will be removed.
 *
 * @see atlib_bufread_skip
 * @see atlib_bufread_seek
 */
extern void atlib_bufread_rewind(bufread_t * br, usize n);

/**
 * @brief Will set @c br to the @c n'th byte of the stream. 
 * @param br Pointer to a valid @c bufread_t object.
 * @param n Position in the file/media to set @c br to.
 *
 * Where @ref atlib_bufread_skip and @ref atlib_bufread_rewind are relative 
 * to the current stream position of @c br, this function is absolute, or 
 * relative to the beginning of the file/media.
 */
extern void atlib_bufread_seek(bufread_t * br, usize n);

/**
 * @brief Provides the position @c br is in its file/media.
 * @param br Pointer to a valid @c bufread_t.
 */
static inline usize atlib_bufread_pos(const bufread_t * br) { return ftell(br->fh) - br->to_read; }

/**
 * @brief Provides the position @c br is in its file/media, without compensating for the buffer.
 * @param br Pointer to a valid @c bufread_t.
 * Returns the position the file is at, without compensating for what has been read in the current file.
 */
static inline usize atlib_bufread_fpos(const bufread_t * br) { return ftell(br->fh); }

/**
 * @brief Provides if @c br has encountered an error, and cannot continue to read.
 * @param br Pointer to a valid @c bufread_t.
 * @returns Non-zero if the underlying file/media has encountered an error.
 */
static inline usize atlib_bufread_err(const bufread_t * br) { return ferror(br->fh); }

/**
 * @brief Provides if @c br has encountered EOF, and cannot continue to read.
 * @param br Pointer to a valid @c bufread_t.
 * @returns Non-zero if the underlying file/media has encountered EOF.
 */
static inline usize atlib_bufread_eof(const bufread_t * br) { return feof(br->fh); }

#endif /* __ATLIB_BUFREAD_H */
