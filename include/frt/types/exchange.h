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

#include "./forward.h"

namespace frt {
  /// Exchanges `a`'s value with `new_value`, and returns the old value
  ///
  /// \param a A reference to the value to replace
  /// \param new_value The value to replace it with
  /// \return The old value of `a`
  template <typename T, typename U = T> [[nodiscard]] constexpr T exchange(T& a, T&& new_value) noexcept {
    auto temp = frt::move(a);

    a = frt::forward<U>(new_value);

    return temp;
  }
} // namespace frt