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

#ifdef FRT_GENERATE_DEFAULT_STD_COMPARE

#include "../types/basic.h"

void* operator new(frt::usize count, void* ptr);

void* operator new[](frt::usize count, void* ptr);

#else

// simply delegate to `<new>` if we're able to
#include <new>

#endif