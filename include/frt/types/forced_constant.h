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

#include "./move.h"

namespace frt {
  ///
  ///
  /// \tparam T
  template <typename T> class Constant {
  public:
    ///
    ///
    /// \param value
    consteval Constant(T value) noexcept : value_{value} {} // NOLINT(google-explicit-constructor)

    constexpr Constant(const Constant&) noexcept = default;

    constexpr Constant(Constant&&) noexcept = default;

    constexpr operator T&() noexcept { // NOLINT(google-explicit-constructor)
      return value_;
    }

  private:
    T value_;
  };
} // namespace frt