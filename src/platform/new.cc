//======---------------------------------------------------------------======//
//                                                                           //
// Copyright 2021-2022 Evan Cox <evanacox00@gmail.com>. All rights reserved. //
//                                                                           //
// Use of this source code is governed by a BSD-style license that can be    //
// found in the LICENSE.txt file at the root of this project, or at the      //
// following link: https://opensource.org/licenses/BSD-3-Clause              //
//                                                                           //
//======---------------------------------------------------------------======//

#include "frt/platform/new.h"

#ifdef FRT_GENERATE_PLACEMENT_NEW

// as far as I can tell, compilers never actually call these (because the implementations
// are quite literally standard-mandated to just be `return ptr`). However,
// because there's no guarantee that `::operator new(size_t, void*)` is an exception,
// we provide an implementation just in case.

void* operator new(frt::usize /*unused*/, void* ptr) {
  return ptr;
}

void* operator new[](frt::usize /*unused*/, void* ptr) {
  return ptr;
}

#endif