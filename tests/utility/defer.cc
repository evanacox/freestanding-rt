//======---------------------------------------------------------------======//
//                                                                           //
// Copyright 2021-2022 Evan Cox <evanacox00@gmail.com>. All rights reserved. //
//                                                                           //
// Use of this source code is governed by a BSD-style license that can be    //
// found in the LICENSE.txt file at the root of this project, or at the      //
// following link: https://opensource.org/licenses/BSD-3-Clause              //
//                                                                           //
//======---------------------------------------------------------------======//

#include "frt/utility/defer.h"
#include "frt/types/move.h"
#include "gtest/gtest.h"

namespace {
  TEST(FrtUtilityDefer, DeferWorks) {
    {
      int x = 2;

      {
        auto _ = frt::defer([&x] {
          x = 1;
        });

        EXPECT_EQ(x, 2);
      }

      EXPECT_EQ(x, 1);
    }

    {
      int x = 4;

      {
        auto d1 = frt::defer([&x] {
          x = 1;
        });

        {
          // move responsibility for running defer into this scope
          auto d2 = frt::move(d1);

          EXPECT_EQ(x, 4);
        } // should run here

        EXPECT_EQ(x, 1);

        x = 2;
      } // d1 shouldn't run here

      EXPECT_EQ(x, 2);
    }

    {
      int x = 3;

      [&x] {
        auto _ = frt::defer([&x] {
          x = 2;
        });

        // 'early return', should still execute
        return;
      }();

      EXPECT_EQ(x, 2);
    }
  }
} // namespace