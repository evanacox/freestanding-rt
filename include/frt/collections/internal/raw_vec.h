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

#include "./member_types.h"

namespace frt::internal {
  template <typename T, typename Traits> class RawVec {
    using Alloc = typename Traits::Allocator;

  public:
    constexpr explicit RawVec() = default;

  private:
    [[no_unique_address]] Alloc alloc_;
  };
} // namespace frt::internal

void f() {}