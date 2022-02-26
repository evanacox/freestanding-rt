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
    /// Called whenever something that was supposed to be zero-allocation (or at least
    /// statically-known to allocate X amount) tries to allocate somehow.
    ///
    /// This is mostly called when hitting limits on statically-sized containers.
    ///
    /// \param message A message detailing what the issue was (never `nullptr`)
    /// \param location The location where the allocation happened
    [[noreturn]] void __frt_tried_alloc(const char* message, CSourceLocation location) noexcept;

    /// Called whenever bounds-checked functions detect an out-of-bounds access.
    ///
    /// \param message A message detailing what the issue was (never `nullptr`)
    /// \param location The location where bounds checking failure happened
    [[noreturn]] void __frt_bounds_fail(const char* message, CSourceLocation location) noexcept;
  }

  /// Called when something that shouldn't allocate hit a situation where
  /// it would have to, simply delegates to `__frt_tried_alloc`
  ///
  /// \param message A message, or `nullptr`
  /// \param loc The location where the allocation happened
  [[noreturn]] inline void tried_alloc(const char* message, SourceLocation loc = SourceLocation::current()) noexcept {
    auto external_loc = CSourceLocation{loc.file_name(), loc.function_name(), loc.line()};

    if (message == nullptr) {
      message = "<empty message>";
    }

    frt::__frt_tried_alloc(message, external_loc);
  }

#ifdef FRT_GENERATE_DEFAULT_TRIED_THROW
  inline constexpr bool generated_tried_alloc = true;
#else
  inline constexpr bool generated_tried_alloc = false;
#endif

  /// Called when code that would have thrown `out_of_range` in `std`-land is executed
  /// it doesn't throw, it simply delegates to `__frt_bounds_fail`. It's effectively a
  /// bounds checking failure.
  ///
  /// \param message A message, or `nullptr`
  /// \param loc The location where the bounds checking failed
  [[noreturn]] inline void bounds_fail(const char* message, SourceLocation loc = SourceLocation::current()) noexcept {
    auto external_loc = CSourceLocation{loc.file_name(), loc.function_name(), loc.line()};

    if (message == nullptr) {
      message = "<empty message>";
    }

    frt::__frt_bounds_fail(message, external_loc);
  }

#ifdef FRT_GENERATE_DEFAULT_BOUNDS_FAIL
  inline constexpr bool generated_bounds_fail = true;
#else
  inline constexpr bool generated_bounds_fail = false;
#endif
} // namespace frt