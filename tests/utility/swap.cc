//======---------------------------------------------------------------======//
//                                                                           //
// Copyright 2021-2022 Evan Cox <evanacox00@gmail.com>. All rights reserved. //
//                                                                           //
// Use of this source code is governed by a BSD-style license that can be    //
// found in the LICENSE.txt file at the root of this project, or at the      //
// following link: https://opensource.org/licenses/BSD-3-Clause              //
//                                                                           //
//======---------------------------------------------------------------======//

#include "frt/utility/swap.h"
#include "frt/types/move.h"
#include "gtest/gtest.h"

namespace {
  class ADLSwap {
  public:
    explicit ADLSwap(int x) : x_{x} {}

    [[nodiscard]] int x() const noexcept {
      return x_;
    }

    [[nodiscard]] bool adl_swapped() const noexcept {
      return swapped_with_adl_;
    }

    void set_adl_swapped() noexcept {
      swapped_with_adl_ = true;
    }

  private:
    int x_;
    bool swapped_with_adl_ = false;
  };

  void swap(ADLSwap& a, ADLSwap& b) {
    a.set_adl_swapped();
    b.set_adl_swapped();

    auto tmp = frt::move(a);
    a = frt::move(b);
    b = frt::move(tmp);
  }
} // namespace

TEST(FrtUtilitySwap, UsesAdlSwap) {
  ADLSwap a{1};
  ADLSwap b{2};

  frt::swap(a, b);

  EXPECT_EQ(a.x(), 2);
  EXPECT_EQ(b.x(), 1);
  EXPECT_TRUE(a.adl_swapped());
  EXPECT_TRUE(b.adl_swapped());
}

TEST(FrtUtilitySwap, UsesBuiltinSwap) {
  auto a = 1;
  auto b = 2;

  frt::swap(a, b);

  EXPECT_EQ(a, 2);
  EXPECT_EQ(b, 1);
}