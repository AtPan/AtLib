#ifndef __ATLIB_H
#define __ATLIB_H

/**
 * @mainpage AtLib API Home
 *
 * Welcome to AtLib! A free, open-source library that aims to add more rigorous
 * testing, fuller debugging, efficiency gains, and zero-cost safety features to C.
 * This is a passion project of mine, and is used throughout many of my projects.
 * Through my - and many others' - C programming careers, we have all needed to
 * develop our own collection of utilities, helper functions, and safety solutions.
 * AtLib is just that: My own collection of utilities that have helped me throughout
 * my programming career and passion projects.
 *
 * All documentation files were generated using Doxygen v1.13.2, whose homepage
 * can be found <a href="https://www.doxygen.nl">here</a>.
 *
 * @section license License and Copyright
 * This library is available under the MIT license. A copy of the license is
 * included with the source code of AtLib, under the file @a LICENSE. Click
 * <a href="https://www.tldrlegal.com/license/mit-license">here</a> for more
 * information regarding the license.
 * 
 * Copyright 2025, Anthony Panarello.
 *
 * @section install_and_use Installation and Usage
 *
 * @subsection install Installation
 *
 * AtLib's only requirement is glibc.
 *
 * To install AtLib, you can either build from 
 * <a href="https://github.com/AtPan/AtLib">source</a>, or install one of the
 * available binaries. Once a binary is either built or installed, using AtLib is
 * as simple as linking it to your project of choice! Ensure the header files are 
 * located by your compiler.
 *
 * @subsection use Usage
 *
 * For each version of AtLib, there are two binaries to choose from. One will 
 * contain extra debugging capabilities and safety checks, at the cost of higher
 * memory usage and slower execution speeds. To switch between the debug and release
 * versions (and subsequently its different features and speeds), the only change
 * is the binary to link against. The code, and expected results, itself will remain
 * the exact same!
 *
 * The purpose of the two versions, with identical code usage, is to ensure all code
 * is safe and correct BEFORE switching to and using a release version. C is known
 * for many pitfalls and undefined behavior. Use the debug version to ensure all
 * code is safe and used properly, then switch to the release version.
 *
 * The release version contains many assumptions and no safety checks to maximize
 * efficiency and execution speeds; it is highly recommended to check for safety
 * by using the debug version first before switching to the release version.
 *
 * To properly use the debug version, define the @c __DEBUG__ macro before including
 * any AtLib header files, or most once convienently through your compiler.
 */

/**
 * @file Atlib.h
 * @brief This file just includes some of the most common headers, types, and functions
 * to use throughout AtLib and those who use it.
 */

#include "Atlib/types.h"
#include "Atlib/memory/slice.h"
#include "Atlib/main.h"
#include "Atlib/memory/string.h"

#endif
