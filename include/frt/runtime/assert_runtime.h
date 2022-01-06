//======---------------------------------------------------------------======//
//                                                                           //
// Copyright 2021 Evan Cox <evanacox00@gmail.com>. All rights reserved.      //
//                                                                           //
// Use of this source code is governed by a BSD-style license that can be    //
// found in the LICENSE.txt file at the root of this project, or at the      //
// following link: https://opensource.org/licenses/BSD-3-Clause              //
//                                                                           //
//======---------------------------------------------------------------======//

#pragma once

#include "frt/types/basic.h"
#include "frt/types/source_location.h"

namespace frt {
  extern "C" {
    /// Maps an assertion back to the place where it occurred
    struct AssertLocation {
      /// Filename where the assertion occurred
      const char* file;
      /// Function name where the assertion occurred
      const char* function;
      /// The line number where the assertion is on
      frt::i32 line;
    };

    /// Function that is called whenever an assertion fails
    ///
    /// \param condition The condition, represented as a string
    /// \param message The message given along with the assertion, if any (`nullptr` otherwise)
    /// \param location The location in the source that the assertion was at
    void __frt_assert_fail(const char* condition, const char* message, AssertLocation location) noexcept;
  }

  /// Called when assertions fail, simply delegates to `__frt_assert_fail`
  ///
  /// \param condition The condition string-ified
  /// \param message A message, or `nullptr`
  /// \param loc The location where the assertion failed
  inline void assert_fail(const char* condition, const char* message, SourceLocation loc = SourceLocation::current()) {
    auto external_loc = AssertLocation{loc.file_name(), loc.function_name(), loc.line()};

    frt::__frt_assert_fail(condition, message, external_loc);
  }
} // namespace frt