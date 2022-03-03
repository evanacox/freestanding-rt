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

#include "../../collections/array.h"
#include "../../types/basic.h"
#include "../../types/traits.h"
#include "../allocator.h"

namespace frt {
  template <typename T, frt::usize Bytes> class StackAllocator {
  public:
    explicit StackAllocator() = default;

  private:
    alignas(T) frt::Array<frt::byte, Bytes> storage_;
  };
} // namespace frt