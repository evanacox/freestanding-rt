//======---------------------------------------------------------------======//
//                                                                           //
// Copyright 2021 Evan Cox <evanacox00@gmail.com>. All rights reserved.      //
//                                                                           //
// Use of this source code is governed by a BSD-style license that can be    //
// found in the LICENSE.txt file at the root of this project, or at the      //
// following link: https://opensource.org/licenses/BSD-3-Clause              //
//                                                                           //
//======---------------------------------------------------------------======//

#pragma once

#if defined(__amd64__) || defined(__x86_64__)
#define FRT_ARCH_X86_64
#elif defined(__aarch64__)
#define FRT_ARCH_ARM64
#elif defined(__arm__) || defined(__thumb__)
#define FRT_ARCH_ARM32
#endif

#if defined(FRT_ARCH_ARM64) || defined(FRT_ARCH_ARM32)
#define FRT_ARCH_ARM
#endif

#if defined(__SIZEOF_INT128__)
#define FRT_SUPPORTS_INT128
#endif