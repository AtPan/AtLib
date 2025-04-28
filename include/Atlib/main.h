#ifndef __ATLIB_MAIN_H
#define __ATLIB_MAIN_H

/**
 * @file main.h
 * Provides a global pointer to the buffered stdin object.
 */

#include "Atlib/io/bufread.h"
#include "Atlib/io/bufwrite.h"

#ifndef __ATLIB_NEED_MAIN

/**
 * @brief A global pointer to a buffered reader that wraps @c stdin.
 * This buffered reader is automatically initialized, and will be automatically
 * closed without any concern from the user. 
 *
 * However, stdin is still available if the user wishes to use raw input instead!
 *
 * @warning Do not mix usage of @c bufstdin and @c stdin. Only use one throughout
 * your program to avoid synchronization errors.
 */
extern bufread_t * bufstdin;
extern bufwrite_t * bufstdout;

#else
#include "types.h"

/**
 * @brief Initializes AtLib's error module and buffers stdin.
 * @warning Do not call this function manually! This function is automatically
 * called before @c main, the user has no need to call it by hand.
 */
extern i32 atlib_start() __attribute__((constructor));

/**
 * @brief Closes AtLib's error module and closes the buffered wrapper of stdin.
 * @warning Do not call this function manually! This function is automatically
 * closed after @c main exits, the user has no need to call it by hand.
 */
extern i32 atlib_end() __attribute__((destructor));

#endif /* __ATLIB_NEED_MAIN */

#endif /* __ATLIB_MAIN_H */
