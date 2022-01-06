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
#include "frt/runtime/assert_runtime.h"

#if defined(FRT_COMPILER_GNULIKE)
#define FRT_ALWAYS_INLINE inline __attribute__((always_inline))
#define FRT_PURE __attribute__((const))
#else
#define FRT_ALWAYS_INLINE inline
#define FRT_PURE
#endif

#define FRT_STRINGIFY1(s) #s
#define FRT_STRINGIFY(s) FRT_STRINGIFY1(s)

#if !defined(NDEBUG) && !defined(FRT_DEBUG)
#define FRT_DEBUG
#endif

#if defined(FRT_DEBUG)

//
// the code below is still sort-of magic to me, see the below SO post
// for a better explanation of what is actually going on
//
// https://stackoverflow.com/a/8814003
//

// case with no message, we give `nullptr` to `assert_fail`
#define FRT_INTERNAL_ASSERT1(condition)                                                                                \
  do {                                                                                                                 \
    if (!condition) {                                                                                                  \
      ::frt::assert_fail(FRT_STRINGIFY(condition), nullptr);                                                           \
    }                                                                                                                  \
  } while (false)

// case with a message, we pass it on to `assert_fail`
#define FRT_INTERNAL_ASSERT2(condition, message)                                                                       \
  do {                                                                                                                 \
    if (!(condition)) {                                                                                                \
      ::frt::assert_fail(FRT_STRINGIFY(condition), message);                                                           \
    }                                                                                                                  \
  } while (false)

// delegates to the correct macro
#define FRT_INTERNAL_ASSERT_X(x, A, B, FUNC, ...) FUNC

// actual user-facing macro
#define FRT_ASSERT(...)                                                                                                \
  FRT_INTERNAL_ASSERT_X(, ##__VA_ARGS__, FRT_INTERNAL_ASSERT2(__VA_ARGS__), FRT_INTERNAL_ASSERT1(__VA_ARGS__))

#else
// turn it into a no-op
#define FRT_ASSERT(...) ((void)0)
#endif