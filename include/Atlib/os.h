#ifndef __ATLIB_OS_H
#define __ATLIB_OS_H

#if defined(__linux__) && !(defined(__APPLE__) || defined(__MINGW32__))
#define ATLIB_OS_LINUX
//#elif defined(__MINGW32__) && !(defined(__linux__) || defined(__APPLE__))
//#define ATLIB_OS_WINDOWS
#else
#error "Unsupported OS detected; AtLib only supports Linux as of version 1.1.0"
#endif

#endif
