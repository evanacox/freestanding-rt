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
#include "frt/collections/array.h"
#include "frt/core/algorithms.h"
#include "test_utilities.h"
#include "gtest/gtest.h"
#include <ranges>

namespace {
  frt::Array array{1, 2, 3, 4, 5, 6, 7, 8, 9, 10};

  struct HasBeginEndAvailableADL {
    friend int* begin(HasBeginEndAvailableADL& /*unused*/) noexcept {
      return array.data();
    }

    friend int* end(HasBeginEndAvailableADL& /*unused*/) noexcept {
      return array.data() + array.size();
    }

    friend const int* begin(const HasBeginEndAvailableADL& /*unused*/) noexcept {
      return array.data();
    }

    friend const int* end(const HasBeginEndAvailableADL& /*unused*/) noexcept {
      return array.data() + array.size();
    }
  };

  TEST(FrtCoreRanges, HasBeginEndAvailableADL) {
    HasBeginEndAvailableADL object;

    // should find hidden overloads with ADL
    EXPECT_EQ(frt::begin(object), array.data());
    EXPECT_EQ(frt::end(object), array.data() + array.size());

    // should call `const` overloads
    EXPECT_EQ(frt::cbegin(object), array.data());
    CHECK_SAME_TYPE(const int*, frt::cbegin(object));
    EXPECT_EQ(frt::cend(object), array.data() + array.size());
    CHECK_SAME_TYPE(const int*, frt::cend(object));

    // should be able to use random-access / comparisons to generate the rest
    EXPECT_EQ(frt::size(object), array.size());
    EXPECT_EQ(frt::ssize(object), array.size());
    EXPECT_FALSE(frt::empty(object));
    EXPECT_EQ(frt::data(object), array.data());

    static_assert(std::ranges::input_range<std::vector<int>&>);
    static_assert(frt::InputRange<std::vector<int>&>);
  }
} // namespace