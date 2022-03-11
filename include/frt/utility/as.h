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
#include "../types/traits.h"

namespace frt {
  /// Adds a `const` to a reference
  ///
  /// \param ref The object to get a `const T&` from
  /// \return `ref` as a const reference
  template <typename T> [[nodiscard]] FRT_ALWAYS_INLINE constexpr frt::traits::AddConst<T>& as_const(T& ref) noexcept {
    return ref;
  }
} // namespace frt