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
#include "frt/collections/array.h"
#include "frt/types/move.h"
#include "gmock/gmock.h"
#include "gtest/gtest.h"

using ::testing::ElementsAre;

namespace {
  struct UsedADL {};

  inline constexpr auto adl = UsedADL{};

  class ADLSwap {
  public:
    ADLSwap(int x) : x_{x} {} // NOLINT(google-explicit-constructor)

    ADLSwap(int x, UsedADL /*unused*/) : x_{x}, swapped_with_adl_{true} {}

    [[nodiscard]] int x() const noexcept {
      return x_;
    }

    [[nodiscard]] bool adl_swapped() const noexcept {
      return swapped_with_adl_;
    }

    void set_adl_swapped() noexcept {
      swapped_with_adl_ = true;
    }

    friend void swap(ADLSwap& a, ADLSwap& b) {
      a.set_adl_swapped();
      b.set_adl_swapped();

      auto tmp = frt::move(a);
      a = frt::move(b);
      b = frt::move(tmp);
    }

    [[maybe_unused]] friend bool operator==(ADLSwap a, ADLSwap b) = default;

    [[maybe_unused]] friend std::ostream& operator<<(std::ostream& os, ADLSwap a) {
      return os << '{' << a.x_ << ", " << (a.swapped_with_adl_ ? "true" : "false") << '}';
    }

  private:
    int x_;
    bool swapped_with_adl_ = false;
  };
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

TEST(FrtUtilitySwap, UsesAdlSwapArray) {
  ADLSwap one[] = {{1}, {2}, {3}, {4}}; // NOLINT(modernize-avoid-c-arrays)
  ADLSwap two[] = {{4}, {3}, {2}, {1}}; // NOLINT(modernize-avoid-c-arrays)

  frt::swap(one, two);

  EXPECT_THAT(one, ElementsAre(ADLSwap{4, adl}, ADLSwap{3, adl}, ADLSwap{2, adl}, ADLSwap{1, adl}));
  EXPECT_THAT(two, ElementsAre(ADLSwap{1, adl}, ADLSwap{2, adl}, ADLSwap{3, adl}, ADLSwap{4, adl}));
}

TEST(FrtUtilitySwap, UsesAdlSwapFrtArray) {
  frt::Array one{ADLSwap{1}, ADLSwap{2}, ADLSwap{3}, ADLSwap{4}}; // NOLINT(modernize-avoid-c-arrays)
  frt::Array two{ADLSwap{4}, ADLSwap{3}, ADLSwap{2}, ADLSwap{1}}; // NOLINT(modernize-avoid-c-arrays)

  frt::swap(one, two);

  // if it used the default move-swap instead of the hidden friend swap, these would not have ADL flag enabled
  EXPECT_THAT(one, ElementsAre(ADLSwap{4, adl}, ADLSwap{3, adl}, ADLSwap{2, adl}, ADLSwap{1, adl}));
  EXPECT_THAT(two, ElementsAre(ADLSwap{1, adl}, ADLSwap{2, adl}, ADLSwap{3, adl}, ADLSwap{4, adl}));
}
