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

#include "basic.h"

namespace frt {
  /// Type that allows code to collect information on its current location in
  /// the source code
  class SourceLocation {
  public:
    /// Method that can be used to collect source information for the caller.
    /// Can be used in default arguments to collect information for each location
    /// that calls the function
    ///
    /// \return A source location object
    static constexpr SourceLocation current(const char* file = __builtin_FILE(),
        const char* func = __builtin_FUNCTION(),
        frt::i32 line = __builtin_LINE()) noexcept {
      auto loc = SourceLocation{};

      loc.file_ = file;
      loc.func_ = func;
      loc.line_ = line;

      return loc;
    }

    /// Constructs a source location with unspecified values
    constexpr SourceLocation() noexcept = default;

    /// Gets the name of the file that the location is referencing
    ///
    /// \return The file name
    [[nodiscard]] constexpr const char* file_name() const noexcept {
      return file_;
    }

    /// Gets the name of the function that the location is referencing
    ///
    /// \return The function
    [[nodiscard]] constexpr const char* function_name() const noexcept {
      return func_;
    }

    /// Gets the line number that the location is referencing
    ///
    /// \return The line number
    [[nodiscard]] constexpr frt::i32 line() const noexcept {
      return line_;
    }

  private:
    const char* file_ = "<unknown>";
    const char* func_ = "<unknown>";
    frt::i32 line_ = -1;
  };

  extern "C" {
    /// Effectively a `frt::SourceLocation` but for C FFI
    struct CSourceLocation {
      /// Filename where the assertion occurred
      const char* file;
      /// Function name where the assertion occurred
      const char* function;
      /// The line number where the assertion is on
      frt::i32 line;
    };
  }
} // namespace frt