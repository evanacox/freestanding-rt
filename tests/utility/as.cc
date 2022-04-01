//======---------------------------------------------------------------======//
//                                                                           //
// Copyright 2021-2022 Evan Cox <evanacox00@gmail.com>. All rights reserved. //
//                                                                           //
// Use of this source code is governed by a BSD-style license that can be    //
// found in the LICENSE.txt file at the root of this project, or at the      //
// following link: https://opensource.org/licenses/BSD-3-Clause              //
//                                                                           //
//======---------------------------------------------------------------======//

#include "frt/utility/as.h"
#include "frt/types/traits.h"
#include "gtest/gtest.h"
#include <cassert>

namespace traits = frt::traits;

namespace {
  struct S {
    int i;
  };

  bool operator==(const S& x, const S& y) {
    return x.i == y.i;
  }

  bool operator==(const volatile S& x, const volatile S& y) {
    return x.i == y.i;
  }

  template <typename T> void test(T& t) {
    static_assert(traits::is_const<traits::RemoveReference<decltype(frt::as_const(t))>>);
    static_assert(traits::is_const<traits::RemoveReference<decltype(frt::as_const<T>(t))>>);
    static_assert(traits::is_const<traits::RemoveReference<decltype(frt::as_const<const T>(t))>>);
    static_assert(traits::is_const<traits::RemoveReference<decltype(frt::as_const<volatile T>(t))>>);
    static_assert(traits::is_const<traits::RemoveReference<decltype(frt::as_const<const volatile T>(t))>>);

    EXPECT_EQ(frt::as_const(t), t);
    EXPECT_EQ(frt::as_const<T>(t), t);
    EXPECT_EQ(frt::as_const<const T>(t), t);

    // GTest breaks on volatile
    assert(frt::as_const<volatile T>(t) == t);
    assert(frt::as_const<const volatile T>(t) == t);
  }
} // namespace

TEST(FrtUtilityAs, AsConst) {
  auto i = 3;
  auto d = 1.0;
  S s{2};

  test(i);
  test(d);
  test(s);
}