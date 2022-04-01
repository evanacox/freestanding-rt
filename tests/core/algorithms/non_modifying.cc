//======---------------------------------------------------------------======//
//                                                                           //
// Copyright 2021-2022 Evan Cox <evanacox00@gmail.com>. All rights reserved. //
//                                                                           //
// Use of this source code is governed by a BSD-style license that can be    //
// found in the LICENSE.txt file at the root of this project, or at the      //
// following link: https://opensource.org/licenses/BSD-3-Clause              //
//                                                                           //
//======---------------------------------------------------------------======//

#define FRT_HAVE_STDLIB
#include "frt/core/iterators/tags.h"
#undef FRT_HAVE_STDLIB

#include "frt/collections.h"
#include "frt/core/algorithms.h"
#include "test_utilities.h"
#include "gmock/gmock.h"
#include "gtest/gtest.h"
#include <algorithm>
#include <vector>

namespace {
  TEST(FrtCoreAlgorithmsNonModifying, Find) {
    {
      std::vector<int> vec{1, 2, 3, 4, 5, 6, 7, 8, 9, 10}; // NOLINT(readability-magic-numbers)

      EXPECT_EQ((frt::find(vec, 5)), vec.begin() + 4);
      EXPECT_EQ((frt::find(vec.begin(), vec.end(), 5)), vec.begin() + 4);
      EXPECT_EQ((frt::find(vec, 11)), vec.end());
      EXPECT_EQ((frt::find(vec.begin(), vec.end(), 11)), vec.end());
      EXPECT_EQ((frt::find(vec.begin(), vec.begin() + 3, 5)), vec.begin() + 3);
      EXPECT_EQ((frt::find(vec.begin(), vec.begin(), 3)), vec.begin());
    }

    {
      frt::Array arr{-1, 42, 3582, 15, 3, 64, 2222222};

      EXPECT_EQ((frt::find(arr, 3582)), arr.begin() + 2);
      EXPECT_EQ((frt::find(arr.begin(), arr.end(), 3582)), arr.begin() + 2);
      EXPECT_EQ((frt::find(arr, -2)), arr.end());
      EXPECT_EQ((frt::find(arr.begin(), arr.end(), -2)), arr.end());
    }
  }
} // namespace