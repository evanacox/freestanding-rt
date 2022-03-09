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

#include "../platform/new.h"
#include "../types/concepts.h"
#include "../types/forward.h"
#include "../types/traits.h"

namespace frt {
  /// In-place constructs an object of type `T` at the location pointed to
  /// by `location`, forwarding `args` into `T`'s constructor.
  ///
  /// \param location The location to construct a new object at
  /// \param args The arguments to forward into `T`'s constructor
  /// \return A pointer to the new object
  template <typename T, typename... Args>
  constexpr auto construct_at(T* location, Args&&... args) noexcept(
      noexcept(::new (static_cast<void*>(nullptr)) T(traits::declval<Args>()...)))
      -> decltype(::new (static_cast<void*>(nullptr)) T(traits::declval<Args>()...)) //
      requires ConstructibleFrom<T, Args...> {
    return ::new (static_cast<void*>(location)) T(frt::forward<Args>(args)...);
  }
} // namespace frt