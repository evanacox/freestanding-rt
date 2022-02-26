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

#include "../platform/macros.h"
#include "../types/basic.h"
#include "../types/source_location.h"

namespace frt {
  extern "C" {
    /// Function that is called whenever an assertion fails
    ///
    /// \param condition The condition, represented as a string
    /// \param message The message given along with the assertion, if any (`nullptr` otherwise)
    /// \param location The location in the source that the assertion was at
    [[noreturn]] void __frt_assert_fail(const char* condition,
        const char* message,
        frt::CSourceLocation location) noexcept;
  }

  /// Called when assertions fail, simply delegates to `__frt_assert_fail`
  ///
  /// \param condition The condition string-ified
  /// \param message A message, or `nullptr`
  /// \param loc The location where the assertion failed
  [[noreturn]] inline void assert_fail(const char* condition,
      const char* message,
      SourceLocation loc = SourceLocation::current()) {
    auto external_loc = CSourceLocation{loc.file_name(), loc.function_name(), loc.line()};

    if (message == nullptr) {
      message = "<empty message>";
    }

    frt::__frt_assert_fail(condition, message, external_loc);
  }
} // namespace frt

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
    if (!(condition)) {                                                                                                \
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

#ifdef FRT_GENERATE_DEFAULT_ASSERT_FAIL
inline constexpr bool generated_assert_fail = true;
#else
inline constexpr bool generated_assert_fail = false;
#endif