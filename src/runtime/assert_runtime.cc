//======---------------------------------------------------------------======//
//                                                                           //
// Copyright 2021-2022 Evan Cox <evanacox00@gmail.com>. All rights reserved. //
//                                                                           //
// Use of this source code is governed by a BSD-style license that can be    //
// found in the LICENSE.txt file at the root of this project, or at the      //
// following link: https://opensource.org/licenses/BSD-3-Clause              //
//                                                                           //
//======---------------------------------------------------------------======//

#include "frt/runtime/assert_runtime.h"

#ifndef FRT_GENERATE_DEFAULT_ASSERT_HANDLER

void frt::__frt_assert_fail(const char* /*unused*/, const char* /*unused*/, frt::AssertLocation /*unused*/) noexcept {
  __builtin_trap();
}

#endif
