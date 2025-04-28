#ifndef __ATLIB_BUFREAD_H
#define __ATLIB_BUFREAD_H

/**
 * @file bufread.h
 * @brief Defines the types, macros, and function headers that creates the AtLib Buffered Reader API.
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
 * The user is responsible for decoding the information and ensuring proper endianness. This
 * API only acts as a buffer to increase reading speeds.
 *
 * To help with proper decoding of the data, AtLib provides both a big-endian and little-endian
 * version of each integer read function, as well as the ability to specify a default endianness
 * to fallback to if a specific endianness is not provided.
 *
 * For example:
 * @code{.c}
 * bufread_t * br = /\* ... *\/;
 * atlib_bufread_open(br, "test.txt", BUFREAD_READ_LE);
 *
 * u32 a = atlib_bufread_read_u32(br);
 * u64 b = atlib_bufread_read_u64_be(br);
 * @endcode
 *
 * In the above example, variable @c a will be read in little-endian format because the flag @c BUFREAD_READ_LE
 * was specified when opening the buffered reader (for more information on flagss, see @see bufread_flags.h),
 * and the generic version of the function @c atlib_bufread_read_u32 was invoked.
 * Variable @c b will be read in big-endian format because, even through the stream was opened in little-endian
 * mode, the big-endian version of the @c atlib_bufread_read_u64 function was invoked, which overrides the
 * "endianness" of the stream.
 * This way of specifying stream-endianness, while also being able to override that endianness if necessary,
 * provides the programmer flexibility in their handling of data, while allowing convenience in using the API.
 *
 * If no endianness is specified when opening the stream, AtLib will allow the stream to interpret this data
 * in the endianness native to the host machine (again, only in cases where an endain-specific function is
 * @b NOT specified).
 *
 * The internal buffer's size is controlled by the macro @ref __ATLIB_BUFREAD_SIZE,
 * which defaults to '4096', or the size of a standard memory page. 
 *
 * @warning A valid Buffered Reader, or `bufread_t` object, is any Buffered Reader
 * that has been initialized (see @see atlib_bufread_open or @see atlib_bufread_fopen).
 * The use of any non-valid Buffered Reader object is undefined behavior.
 *
 * @see atlib_bufread_open
 * @see atlib_bufread_close
 */
typedef struct {
    FILE * fh;                      ///< @brief The FILE handler to read from.
    isize to_read;                  ///< @brief The number of unread bytes in the buffer before the next refill.
    u32 flags;                      ///< @brief Flags used to track extra features of this buffered reader.
    char * next;                    ///< @brief The pointer to the next byte to read.
    char buf[__ATLIB_BUFREAD_SIZE]; ///< @brief The buffer to store data.
} bufread_t;

/**
 * @brief Initializes a @c bufread_t object to a valid state.
 * @param br A pointer to a @c bufread_t object.
 * @param file_path The path of the file to open.
 * @param br_flags Flags for this buffered stream. See @see bufread_flags.h for more information.
 * @returns Pointer to @c br on success, or @c nullptr if an error occured.
 *
 * The file specified by in @c file_path is opened in read-only mode.
 *
 * To initialize a @c bufread_t from a FILE pointer instead, use
 * @ref atlib_bufread_fopen instead.
 *
 * To initialize a @c bufread_t with custom flags instead of those
 * specified in @c BUFREAD_FLAG_DEFAULT, use @red atlib_bufread_open_ex.
 *
 * @warning If @c br already points to a valid @c bufread_t object, it is
 * overridden and will leave the associated file dangling. Always close a
 * @c bufread_t object.
 *
 * Example:
 * @code{.c}
 * #include <Atlib.h>
 *
 * i32 main(i32 argc, char * argv[]) {
 *     i32 code = 0;
 *
 *     bufread_t * br = atlib_malloc(bufread_t, 1);
 *     if(br == nullptr) { return 1; }
 *
 *     if(atlib_bufread_open(br, "test.txt", 0) == nullptr) { 
 *         code = 2;
 *         goto open_err;
 *     }
 *
 *     /\* ... *\/
 *
 *     atlib_bufread_close(br);
 *
 * open_err:
 *     atlib_free(br);
 *     return code;
 * }
 * @endcode
 *
 * @see atlib_bufread_fopen
 * @see atlib_bufread_close
 *
 * @since AtLib v1.1.0
 */
ATAPI bufread_t * atlib_bufread_open(bufread_t *__restrict br, const char *__restrict file_path, u32 br_flags);

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
 * Example:
 * @code{.c}
 * #include <Atlib.h>
 * #include <stdio.h>
 * 
 * i32 main(i32 argc, char * argv[]) {
 *     i32 code = 0;
 *
 *     bufread_t * br = atlib_malloc(bufread_t, 1);
 *     if(br == nullptr) { return 1; }
 *
 *     FILE * fh = fopen("test.txt", "r");
 *     if(fh == NULL) { code = 2; goto file_err; };
 *
 *     if(atlib_bufread_open(br, fh) == nullptr) { 
 *         code = 3;
 *         goto open_err;
 *     }
 *
 *     /\* ... *\/
 *
 *     atlib_bufread_close(br);
 *
 * open_err:
 *     fclose(fh);
 * file_err:
 *     atlib_free(br);
 *     return code;
 * }
 * @endcode
 *
 * @see atlib_bufread_open
 * @see atlib_bufread_close
 *
 * @since AtLib v1.0.0
 */
ATAPI bufread_t * atlib_bufread_fopen(bufread_t *__restrict br, FILE *__restrict file);

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
 *
 * Example:
 * @code{.c}
 * #include <Atlib.h>
 *
 * i32 main(i32 argc, char * argv[]) {
 *     i32 code = 0;
 *
 *     bufread_t * br = atlib_malloc(bufread_t, 1);
 *     if(br == nullptr) { return 1; }
 *
 *     if(atlib_bufread_open(br, "test.txt", 0) == nullptr) { 
 *         code = 2;
 *         goto open_err;
 *     }
 *
 *     /\* ... *\/
 *
 *     atlib_bufread_close(br);
 *
 * open_err:
 *     atlib_free(br);
 *     return code;
 * }
 * @endcode
 *
 * @since AtLib v1.0.0
 */
ATAPI void atlib_bufread_close(bufread_t * br);

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
 *
 * @since AtLib v1.1.0
 */
ATAPI i64 atlib_bufread_read_nline(bufread_t *__restrict br, void *__restrict buf, u32 n);

/*
 * @brief Reads @c n bytes from @c br into @c buf.
 * @param br Pointer to a valid @c bufread_t object to read from.
 * @param buf The buffer to read into.
 * @param n The number of bytes to read.
 * @returns The number of bytes read. If less than @c n, then @c EOF has been encountered.
 *
 * This function is best suited to reading large chunks of bytes from the underlying media
 * to some external buffer. Due to the bytewise copying used, this function is only useful in
 * scenarios where the data being read is itself one byte in length (i.e. ASCII strings),
 * or the underlying stream's endianness matches the host machine. If your scenario is one
 * that is neither of these two listed above, this function is not the right solution (and
 * certainly not a portable one) to your problem.
 *
 * @deprecated This function has been replaced by @ref atlib_bufread_read, but remains
 * for legacy purposes.
 *
 * @since AtLib v1.0.0
 */
ATAPI __attribute__((deprecated)) i64 atlib_bufread_readn(bufread_t *__restrict br, void *__restrict buf, u32 n);

/**
 * @brief Reads @c n blocks of @c blk bytes from the stream @c br to the buffer @c buf.
 * @param br Pointer to a valid @c bufread_t object to read from.
 * @param buf Pointer to the buffer to read into.
 * @param blk Size of each block to read, in bytes.
 * @param n Number of blocks to read.
 * @returns Number of blocks successfully read into the buffer.
 *
 * This function is optimized for effeciently reading large blocks of data at once, and is AtLib's
 * reflection of glibc's @c fread. This function is suitable for reading blocks of data bytewise;
 * that is, blocks of data byte-by-byte. Therefor, the endianness of the data being transferred is
 * effectively ignored by AtLib. If this is of concern, use one of the atlib_bufread_read_* functions
 * to exercise more precision over the interpretation of the data stream.
 *
 * Example:
 * @code{.c}
 * bufread_t * br = /\* ... *\/; // Assume valid buffered reader setup. For more information, see atlib_bufread_open.
 * struct { i32 x; i32 y; } points[5];
 * u32 n = atlib_bufread_read(br, buf, 2 * sizeof(i32), 5);
 * if(n != 5) { /\* Handle Error Here *\/ }
 * @endcode
 *
 * @since AtLib v1.1.0
 */
ATAPI u32 atlib_bufread_read(bufread_t *__restrict br, void *__restrict buf, u32 blk, u32 n);

/**
 * @brief Reads a @c u8 from @c br.
 * @param br Pointer to a valid @c bufread_t to read from.
 * @returns The next u8 in the stream.
 *
 * When the stream reaches @c EOF, the return value of this function will always
 * be 0. Due to the impossibility of determining if the next byte in the stream 
 * is @c 0x00, or @c EOF was reached, the function @ref atlib_bufread_eof can be 
 * used to determine.
 *
 * @see atlib_bufread_eof
 *
 * @since AtLib v1.0.0
 */
ATAPI u8 atlib_bufread_read_u8(bufread_t * br);

/**
 * @brief Reads a @c u16 from @c br.
 * @param br Pointer to a valid @c bufread_t to read from.
 * @returns The next u16 in the stream.
 *
 * When the stream reaches @c EOF, the return value of this function will always
 * be 0. Due to the impossibility of determining if the next 16-bit value in the
 * stream is @c 0x00, or @c EOF was reached, the function @ref atlib_bufread_eof 
 * can be used to determine.
 *
 * The endianness of how the value is determined by the endianness flag set on 
 * stream creation. See @ref atlib_bufread_open for more information.
 *
 * @see atlib_bufread_eof
 * @see atlib_bufread_open
 * @see BUFREAD_READ_NATIVE
 *
 * @since AtLib v1.0.0
 */
ATAPI u16 atlib_bufread_read_u16(bufread_t * br);

/**
 * @brief Reads a @c u16 from @c br in big-endian format.
 * @param br Pointer to a valid @c bufread_t to read from.
 * @returns Next u16 in the stream.
 *
 * When the stream reaches @c EOF, the return value of this function will always
 * be 0. Due to the impossibility of determining if the next 16-bit value in the
 * stream is @c 0x00, or @c EOF was reached, the function @ref atlib_bufread_eof 
 * can be used to determine.
 *
 * @see atlib_bufread_eof
 * @see atlib_bufread_open
 * @see BUFREAD_READ_NATIVE
 *
 * @since AtLib v1.1.0
 */
ATAPI u16 atlib_bufread_read_u16_be(bufread_t * br);

/**
 * @brief Reads a @c u16 from @c br in little-endian format.
 * @param br Pointer to a valid @c bufread_t to read from.
 * @returns Next u16 in the stream.
 *
 * When the stream reaches @c EOF, the return value of this function will always
 * be 0. Due to the impossibility of determining if the next 16-bit value in the
 * stream is @c 0x00, or @c EOF was reached, the function @ref atlib_bufread_eof 
 * can be used to determine.
 *
 * @see atlib_bufread_eof
 * @see atlib_bufread_open
 * @see BUFREAD_READ_NATIVE
 *
 * @since AtLib v1.1.0
 */
ATAPI u16 atlib_bufread_read_u16_le(bufread_t * br);

/**
 * @brief Reads a @c u32 from @c br.
 * @param br Pointer to a valid @c bufread_t to read from.
 * @returns The next u32 in the stream.
 *
 * If @c br has reached @c EOF, then this function will return 0. In such cases,
 * to determine if the next value was 0 or if @c EOF was reached, call @ref
 * atlib_bufread_eof to determine.
 *
 * @see atlib_bufread_eof
 *
 * @since AtLib v1.0.0
 */
ATAPI u32 atlib_bufread_read_u32(bufread_t * br);

/**
 * @brief Reads a @c u32 from @c br in big-endian.
 * @param br Pointer to a valid @c bufread_t to read from.
 * @returns The next u32 in the stream.
 *
 * If @c br has reached @c EOF, then this function will return 0. In such cases,
 * to determine if the next value was 0 or if @c EOF was reached, call @ref
 * atlib_bufread_eof to determine.
 *
 * @see atlib_bufread_eof
 *
 * @since AtLib v1.1.0
 */
ATAPI u32 atlib_bufread_read_u32_be(bufread_t * br);

/**
 * @brief Reads a @c u32 from @c br in little-endian format.
 * @param br Pointer to a valid @c bufread_t to read from.
 * @returns The next u32 in the stream.
 *
 * If @c br has reached @c EOF, then this function will return 0. In such cases,
 * to determine if the next value was 0 or if @c EOF was reached, call @ref
 * atlib_bufread_eof to determine.
 *
 * @see atlib_bufread_eof
 *
 * @since AtLib v1.1.0
 */
ATAPI u32 atlib_bufread_read_u32_le(bufread_t * br);

/**
 * @brief Reads a @c u64 from @c br.
 * @param br Pointer to a valid @c bufread_t to read from.
 * @returns The next u64 in the stream.
 *
 * If @c br has reached @c EOF, then this function will return 0. In such cases,
 * to determine if the next value was 0 or if @c EOF was reached, call @ref
 * atlib_bufread_eof to determine.
 *
 * @see atlib_bufread_eof
 *
 * @since AtLib v1.0.0
 */
ATAPI u64 atlib_bufread_read_u64(bufread_t * br);

/**
 * @brief Reads a @c u64 from @c br in big-endian format.
 * @param br Pointer to a valid @c bufread_t to read from.
 * @returns The next u64 in the stream.
 *
 * If @c br has reached @c EOF, then this function will return 0. In such cases,
 * to determine if the next value was 0 or if @c EOF was reached, call @ref
 * atlib_bufread_eof to determine.
 *
 * @see atlib_bufread_eof
 *
 * @since AtLib v1.1.0
 */
ATAPI u64 atlib_bufread_read_u64_be(bufread_t * br);

/**
 * @brief Reads a @c u64 from @c br in little-endian format.
 * @param br Pointer to a valid @c bufread_t to read from.
 * @returns The next u64 in the stream.
 *
 * If @c br has reached @c EOF, then this function will return 0. In such cases,
 * to determine if the next value was 0 or if @c EOF was reached, call @ref
 * atlib_bufread_eof to determine.
 *
 * @see atlib_bufread_eof
 *
 * @since AtLib v1.1.0
 */
ATAPI u64 atlib_bufread_read_u64_le(bufread_t * br);

/**
 * @brief Reads a @c i8 from @c br.
 * @param br Pointer to a valid @c bufread_t to read from.
 * @returns The next i8 in the stream.
 *
 * If @c br has reached @c EOF, then this function will return 0. In such cases,
 * to determine if the next value was 0 or if @c EOF was reached, call @ref
 * atlib_bufread_eof to determine.
 *
 * @see atlib_bufread_eof
 *
 * @since AtLib v1.0.0
 */
ATAPI i8 atlib_bufread_read_i8(bufread_t * br);

/**
 * @brief Reads a @c i16 from @c br.
 * @param br Pointer to a valid @c bufread_t to read from.
 * @returns The next i16 in the stream.
 *
 * If @c br has reached @c EOF, then this function will return 0. In such cases,
 * to determine if the next value was 0 or if @c EOF was reached, call @ref
 * atlib_bufread_eof to determine.
 *
 * @see atlib_bufread_eof
 *
 * @since AtLib v1.0.0
 */
ATAPI i16 atlib_bufread_read_i16(bufread_t * br);

/**
 * @brief Reads a @c i16 from @c br in big-endian format.
 * @param br Pointer to a valid @c bufread_t to read from.
 * @returns The next i16 in the stream.
 *
 * If @c br has reached @c EOF, then this function will return 0. In such cases,
 * to determine if the next value was 0 or if @c EOF was reached, call @ref
 * atlib_bufread_eof to determine.
 *
 * @see atlib_bufread_eof
 *
 * @since AtLib v1.1.0
 */
ATAPI i16 atlib_bufread_read_i16_be(bufread_t * br);

/**
 * @brief Reads a @c i16 from @c br in little-endian format.
 * @param br Pointer to a valid @c bufread_t to read from.
 * @returns The next i16 in the stream.
 *
 * If @c br has reached @c EOF, then this function will return 0. In such cases,
 * to determine if the next value was 0 or if @c EOF was reached, call @ref
 * atlib_bufread_eof to determine.
 *
 * @see atlib_bufread_eof
 *
 * @since AtLib v1.1.0
 */
ATAPI i16 atlib_bufread_read_i16_le(bufread_t * br);

/**
 * @brief Reads a @c i32 from @c br.
 * @param br Pointer to a valid @c bufread_t to read from.
 * @returns The next i32 in the stream.
 *
 * If @c br has reached @c EOF, then this function will return 0. In such cases,
 * to determine if the next value was 0 or if @c EOF was reached, call @ref
 * atlib_bufread_eof to determine.
 *
 * @see atlib_bufread_eof
 *
 * @since AtLib v1.0.0
 */
ATAPI i32 atlib_bufread_read_i32(bufread_t * br);

/**
 * @brief Reads a @c i32 from @c br in big-endian format.
 * @param br Pointer to a valid @c bufread_t to read from.
 * @returns The next i32 in the stream.
 *
 * If @c br has reached @c EOF, then this function will return 0. In such cases,
 * to determine if the next value was 0 or if @c EOF was reached, call @ref
 * atlib_bufread_eof to determine.
 *
 * @see atlib_bufread_eof
 *
 * @since AtLib v1.1.0
 */
ATAPI i32 atlib_bufread_read_i32_be(bufread_t * br);

/**
 * @brief Reads a @c i32 from @c br in little-endian format.
 * @param br Pointer to a valid @c bufread_t to read from.
 * @returns The next i32 in the stream.
 *
 * If @c br has reached @c EOF, then this function will return 0. In such cases,
 * to determine if the next value was 0 or if @c EOF was reached, call @ref
 * atlib_bufread_eof to determine.
 *
 * @see atlib_bufread_eof
 *
 * @since AtLib v1.1.0
 */
ATAPI i32 atlib_bufread_read_i32_le(bufread_t * br);

/**
 * @brief Reads a @c i54 from @c br.
 * @param br Pointer to a valid @c bufread_t to read from.
 * @returns The next i54 in the stream.
 *
 * If @c br has reached @c EOF, then this function will return 0. In such cases,
 * to determine if the next value was 0 or if @c EOF was reached, call @ref
 * atlib_bufread_eof to determine.
 *
 * @see atlib_bufread_eof
 *
 * @since AtLib v1.0.0
 */
ATAPI i64 atlib_bufread_read_i64(bufread_t * br);

/**
 * @brief Reads a @c i54 from @c br in big-endian format.
 * @param br Pointer to a valid @c bufread_t to read from.
 * @returns The next i54 in the stream.
 *
 * If @c br has reached @c EOF, then this function will return 0. In such cases,
 * to determine if the next value was 0 or if @c EOF was reached, call @ref
 * atlib_bufread_eof to determine.
 *
 * @see atlib_bufread_eof
 *
 * @since AtLib v1.1.0
 */
ATAPI i64 atlib_bufread_read_i64_be(bufread_t * br);

/**
 * @brief Reads a @c i54 from @c br in little-endian format.
 * @param br Pointer to a valid @c bufread_t to read from.
 * @returns The next i54 in the stream.
 *
 * If @c br has reached @c EOF, then this function will return 0. In such cases,
 * to determine if the next value was 0 or if @c EOF was reached, call @ref
 * atlib_bufread_eof to determine.
 *
 * @see atlib_bufread_eof
 *
 * @since AtLib v1.1.0
 */
ATAPI i64 atlib_bufread_read_i64_le(bufread_t * br);

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
 *
 * @since AtLib v1.0.0
 */
ATAPI void atlib_bufread_skip(bufread_t * br, isize n);

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
 *
 * @since AtLib v1.0.0
 */
ATAPI void atlib_bufread_rewind(bufread_t * br, usize n);

/**
 * @brief Will set @c br to the @c n'th byte of the stream. 
 * @param br Pointer to a valid @c bufread_t object.
 * @param n Position in the file/media to set @c br to.
 *
 * Where @ref atlib_bufread_skip and @ref atlib_bufread_rewind are relative 
 * to the current stream position of @c br, this function is absolute, or 
 * relative to the beginning of the file/media.
 *
 * @since AtLib v1.0.0
 */
ATAPI void atlib_bufread_seek(bufread_t * br, usize n);

/**
 * @brief Provides the position @c br is in its file/media.
 * @param br Pointer to a valid @c bufread_t.
 * @since AtLib v1.0.0
 */
static inline usize atlib_bufread_pos(const bufread_t * br) { return ftell(br->fh) - br->to_read; }

/**
 * @brief Provides the position @c br is in its file/media, without compensating for the buffer.
 * @param br Pointer to a valid @c bufread_t.
 * @returns Position the file is at, without compensating for what has been read in the current file.
 * @since AtLib v1.0.0
 */
static inline usize atlib_bufread_fpos(const bufread_t * br) { return ftell(br->fh); }

/**
 * @brief Provides if @c br has encountered an error, and cannot continue to read.
 * @param br Pointer to a valid @c bufread_t.
 * @returns Non-zero if the underlying file/media has encountered an error.
 * @since AtLib v1.0.0
 */
static inline usize atlib_bufread_err(const bufread_t * br) { return ferror(br->fh); }

/**
 * @brief Provides if @c br has encountered EOF, and cannot continue to read.
 * @param br Pointer to a valid @c bufread_t.
 * @returns Non-zero if the underlying file/media has encountered EOF.
 * @since AtLib v1.0.0
 */
static inline usize atlib_bufread_eof(const bufread_t * br) { return feof(br->fh); }

#endif /* __ATLIB_BUFREAD_H */
