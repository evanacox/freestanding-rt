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

#include "../types/concepts.h"
#include "../types/forward.h"

namespace frt {
  struct Identity {
    template <typename T> constexpr T&& operator()(T&& t) const noexcept {
      return frt::forward<T>(t);
    }
  };

  struct EqualTo {
    template <typename T, typename U>
    requires frt::EqualityComparableWith<T, U>
    constexpr bool operator()(T&& t, U&& u) const
        noexcept(noexcept(static_cast<bool>(frt::forward<T>(t) == frt::forward<U>(u)))) {
      return static_cast<bool>(frt::forward<T>(t) == frt::forward<U>(u));
    }
  };
} // namespace frt