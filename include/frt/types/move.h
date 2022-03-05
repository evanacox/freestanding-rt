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
#include "./traits.h"

namespace frt {
  /// Indicates that the object passed in may be "moved from," produces an xvalue.
  ///
  /// \param value The value to indicate is movable
  /// \return An xvalue created from `value`
  template <typename T> FRT_ALWAYS_INLINE constexpr traits::RemoveReference<T>&& move(T&& value) noexcept {
    return static_cast<traits::RemoveReference<T>&&>(value);
  }
} // namespace frt