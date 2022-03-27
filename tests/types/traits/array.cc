//======---------------------------------------------------------------======//
//                                                                           //
// Copyright 2021-2022 Evan Cox <evanacox00@gmail.com>. All rights reserved. //
//                                                                           //
// Use of this source code is governed by a BSD-style license that can be    //
// found in the LICENSE.txt file at the root of this project, or at the      //
// following link: https://opensource.org/licenses/BSD-3-Clause              //
//                                                                           //
//======---------------------------------------------------------------======//

#include "frt/types/concepts.h"
#include "frt/types/traits.h"
#include "gtest/gtest.h"

/*
 * Note: Most of this test is adapted from the libc++ test suite, therefore it is
 * also under the LLVM copyright. See the license header below for details:
 */

//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

namespace {
  enum Enum { zero, one };

  template <typename T, typename U> void test_remove_all_extents() {
    static_assert(frt::SameAs<U, typename frt::traits::RemoveAllExtentsTrait<T>::type>);
    static_assert(frt::SameAs<U, frt::traits::RemoveAllExtents<T>>);
  }
  template <typename T, typename U> void test_remove_extent() {
    static_assert(frt::SameAs<U, typename frt::traits::RemoveExtentTrait<T>::type>);
    static_assert(frt::SameAs<U, frt::traits::RemoveExtent<T>>);
  }

  TEST(FrtTypesTraits, RemoveAllExtents) {
    test_remove_all_extents<int, int>();
    test_remove_all_extents<const Enum, const Enum>();
    test_remove_all_extents<int[], int>();
    test_remove_all_extents<const int[], const int>();
    test_remove_all_extents<int[3], int>();
    test_remove_all_extents<const int[3], const int>();
    test_remove_all_extents<int[][3], int>();
    test_remove_all_extents<const int[][3], const int>();
    test_remove_all_extents<int[2][3], int>();
    test_remove_all_extents<const int[2][3], const int>();
    test_remove_all_extents<int[1][2][3], int>();
    test_remove_all_extents<const int[1][2][3], const int>();
  }

  TEST(FrtTypesTraits, RemoveExtent) {
    test_remove_extent<int, int>();
    test_remove_extent<const Enum, const Enum>();
    test_remove_extent<int[], int>();
    test_remove_extent<const int[], const int>();
    test_remove_extent<int[3], int>();
    test_remove_extent<const int[3], const int>();
    test_remove_extent<int[][3], int[3]>();
    test_remove_extent<const int[][3], const int[3]>();
    test_remove_extent<int[2][3], int[3]>();
    test_remove_extent<const int[2][3], const int[3]>();
    test_remove_extent<int[1][2][3], int[2][3]>();
    test_remove_extent<const int[1][2][3], const int[2][3]>();
  }
} // namespace