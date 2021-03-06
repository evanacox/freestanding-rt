//======---------------------------------------------------------------======//
//                                                                           //
// Copyright 2021-2022 Evan Cox <evanacox00@gmail.com>. All rights reserved. //
//                                                                           //
// Use of this source code is governed by a BSD-style license that can be    //
// found in the LICENSE.txt file at the root of this project, or at the      //
// following link: https://opensource.org/licenses/BSD-3-Clause              //
//                                                                           //
//======---------------------------------------------------------------======//

#include "frt/runtime/failures.h"

#ifdef FRT_GENERATE_DEFAULT_TRIED_THROW

void frt::__frt_tried_alloc(const char* /*unused*/, frt::CSourceLocation /*unused*/) noexcept {
  __builtin_trap();
}

#endif

#ifdef FRT_GENERATE_DEFAULT_BOUNDS_FAIL

void frt::__frt_bounds_fail(const char* /*unused*/, frt::CSourceLocation /*unused*/) noexcept {
  __builtin_trap();
}

#endif