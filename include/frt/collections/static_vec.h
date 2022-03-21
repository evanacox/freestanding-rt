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

#include "../core/limits.h"
#include "../types/basic.h"
#include "../types/traits.h"
#include "./internal/raw_vec.h"

namespace frt {
  template <typename T, frt::usize N> class StaticVec {
  public:
    explicit StaticVec() = default;

    constexpr T& push_back(T& value) noexcept;

    constexpr T& push_back(T&& value) noexcept;

    [[nodiscard]] constexpr frt::isize size() const noexcept {
      return size_;
    }

    [[nodiscard]] constexpr frt::isize capacity() const noexcept {
      return N;
    }

    [[nodiscard]] constexpr frt::isize max_size() const noexcept {
      return NumericLimits<frt::isize>::max;
    }

  private:
    frt::isize size_ = 0;
    alignas(T) frt::ubyte storage_[N * sizeof(T)] = {0}; // NOLINT(modernize-avoid-c-arrays)
  };
} // namespace frt