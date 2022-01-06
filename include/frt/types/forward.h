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

#include "./traits.h"

namespace frt {
  /// Performs perfect forwarding on an lvalue reference
  ///
  /// \param value The reference to forward
  /// \return The forwarded reference
  template <typename T> constexpr T&& forward(traits::RemoveReference<T>& value) noexcept {
    return static_cast<T&&>(value);
  }

  /// Performs perfect forwarding on an rvalue reference
  ///
  /// \param value The reference to forward
  /// \return The forwarded reference
  template <typename T> constexpr T&& forward(traits::RemoveReference<T>&& value) noexcept {
    return static_cast<T&&>(value);
  }
} // namespace frt