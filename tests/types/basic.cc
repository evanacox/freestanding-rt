//======---------------------------------------------------------------======//
//                                                                           //
// Copyright 2021-2022 Evan Cox <evanacox00@gmail.com>. All rights reserved. //
//                                                                           //
// Use of this source code is governed by a BSD-style license that can be    //
// found in the LICENSE.txt file at the root of this project, or at the      //
// following link: https://opensource.org/licenses/BSD-3-Clause              //
//                                                                           //
//======---------------------------------------------------------------======//

#include "frt/types/basic.h"

// other headers may include C++ stuff that we don't want
#ifndef offsetof
#error offsetof is undefined!
#endif

#include "frt/types/concepts.h"
#include <cstddef>

static_assert(sizeof(frt::isize) == sizeof(std::ptrdiff_t) && sizeof(frt::isize) == sizeof(void*));
static_assert(alignof(frt::isize) == alignof(std::ptrdiff_t) && alignof(frt::isize) == alignof(void*));
static_assert(frt::SignedIntegral<frt::isize>);

static_assert(sizeof(frt::usize) == sizeof(std::size_t) && sizeof(frt::usize) == sizeof(void*));
static_assert(alignof(frt::usize) == alignof(std::size_t) && alignof(frt::usize) == alignof(void*));
static_assert(frt::UnsignedIntegral<frt::usize>);

static_assert(sizeof(frt::nullptr_t) == sizeof(void*));
static_assert(frt::SameAs<decltype(nullptr), frt::nullptr_t>);

static_assert(sizeof(frt::ubyte) == 1);
static_assert(sizeof(frt::ibyte) == 1);

struct F {
  int x;
};

static_assert(noexcept(offsetof(F, x)));
static_assert(offsetof(F, x) == 0);