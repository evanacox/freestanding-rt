//======---------------------------------------------------------------======//
//                                                                           //
// Copyright 2021-2022 Evan Cox <evanacox00@gmail.com>. All rights reserved. //
//                                                                           //
// Use of this source code is governed by a BSD-style license that can be    //
// found in the LICENSE.txt file at the root of this project, or at the      //
// following link: https://opensource.org/licenses/BSD-3-Clause              //
//                                                                           //
//======---------------------------------------------------------------======//

#pragma once

#include "./compiler.h"

#if defined(FRT_COMPILER_GNULIKE)
#define FRT_ALWAYS_INLINE inline __attribute__((always_inline))
#define FRT_PURE __attribute__((const))
#define FRT_NEVER_INLINE __attribute__((noinline))
#define FRT_COLD __attribute__((cold))
#define FRT_UNLIKELY(expr) __builtin_expect((expr), false)
#define FRT_LIKELY(expr) __builtin_expect((expr), true)
#else
#define FRT_ALWAYS_INLINE inline
#define FRT_PURE
#define FRT_COLD
#define FRT_NEVER_INLINE
#define FRT_UNLIKELY(expr) (expr)
#define FRT_LIKELY(expr) (expr)
#endif

#define FRT_STRINGIFY1(s) #s
#define FRT_STRINGIFY(s) FRT_STRINGIFY1(s)

#if !defined(NDEBUG) && !defined(FRT_DEBUG)
#define FRT_DEBUG
#endif