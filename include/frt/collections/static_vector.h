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

#include "../types/basic.h"
#include "../types/traits.h"

namespace frt {
  template <typename T, frt::size N> class StaticVector {
  public:
    explicit StaticVector() = default;

    constexpr T& push_back(T& value) noexcept;

    constexpr T& push_back(T&& value) noexcept;

    [[nodiscard]] constexpr frt::size size() const noexcept {
      return size_;
    }

    [[nodiscard]] constexpr frt::size capacity() const noexcept {
      return N;
    }

  private:
    frt::size size_;
    alignas(T) frt::byte storage_[N * sizeof(T)] = {0}; // NOLINT(modernize-avoid-c-arrays)
  };
} // namespace frt