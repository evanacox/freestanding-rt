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
  struct True {
    static constexpr bool value = true;
  };
  struct False {
    static constexpr bool value = false;
  };

  TEST(FrtTypesTraits, Conjunction) {
    static_assert(frt::traits::ConjunctionTrait<>::value);
    static_assert(frt::traits::ConjunctionTrait<std::true_type>::value);
    static_assert(!frt::traits::ConjunctionTrait<std::false_type>::value);

    static_assert(frt::traits::conjunction<>);
    static_assert(frt::traits::conjunction<std::true_type>);
    static_assert(!frt::traits::conjunction<std::false_type>);

    static_assert(frt::traits::ConjunctionTrait<std::true_type, std::true_type>::value);
    static_assert(!frt::traits::ConjunctionTrait<std::true_type, std::false_type>::value);
    static_assert(!frt::traits::ConjunctionTrait<std::false_type, std::true_type>::value);
    static_assert(!frt::traits::ConjunctionTrait<std::false_type, std::false_type>::value);

    static_assert(frt::traits::conjunction<std::true_type, std::true_type>);
    static_assert(!frt::traits::conjunction<std::true_type, std::false_type>);
    static_assert(!frt::traits::conjunction<std::false_type, std::true_type>);
    static_assert(!frt::traits::conjunction<std::false_type, std::false_type>);

    static_assert(frt::traits::ConjunctionTrait<std::true_type, std::true_type, std::true_type>::value);
    static_assert(!frt::traits::ConjunctionTrait<std::true_type, std::false_type, std::true_type>::value);
    static_assert(!frt::traits::ConjunctionTrait<std::false_type, std::true_type, std::true_type>::value);
    static_assert(!frt::traits::ConjunctionTrait<std::false_type, std::false_type, std::true_type>::value);
    static_assert(!frt::traits::ConjunctionTrait<std::true_type, std::true_type, std::false_type>::value);
    static_assert(!frt::traits::ConjunctionTrait<std::true_type, std::false_type, std::false_type>::value);
    static_assert(!frt::traits::ConjunctionTrait<std::false_type, std::true_type, std::false_type>::value);
    static_assert(!frt::traits::ConjunctionTrait<std::false_type, std::false_type, std::false_type>::value);

    static_assert(frt::traits::conjunction<std::true_type, std::true_type, std::true_type>);
    static_assert(!frt::traits::conjunction<std::true_type, std::false_type, std::true_type>);
    static_assert(!frt::traits::conjunction<std::false_type, std::true_type, std::true_type>);
    static_assert(!frt::traits::conjunction<std::false_type, std::false_type, std::true_type>);
    static_assert(!frt::traits::conjunction<std::true_type, std::true_type, std::false_type>);
    static_assert(!frt::traits::conjunction<std::true_type, std::false_type, std::false_type>);
    static_assert(!frt::traits::conjunction<std::false_type, std::true_type, std::false_type>);
    static_assert(!frt::traits::conjunction<std::false_type, std::false_type, std::false_type>);

    static_assert(frt::traits::ConjunctionTrait<True>::value);
    static_assert(!frt::traits::ConjunctionTrait<False>::value);

    static_assert(frt::traits::conjunction<True>);
    static_assert(!frt::traits::conjunction<False>);
  }

  TEST(FrtTypesTraits, Disjunction) {
    static_assert(!frt::traits::DisjunctionTrait<>::value);
    static_assert(frt::traits::DisjunctionTrait<std::true_type>::value);
    static_assert(!frt::traits::DisjunctionTrait<std::false_type>::value);

    static_assert(!frt::traits::disjunction<>);
    static_assert(frt::traits::disjunction<std::true_type>);
    static_assert(!frt::traits::disjunction<std::false_type>);

    static_assert(frt::traits::DisjunctionTrait<std::true_type, std::true_type>::value);
    static_assert(frt::traits::DisjunctionTrait<std::true_type, std::false_type>::value);
    static_assert(frt::traits::DisjunctionTrait<std::false_type, std::true_type>::value);
    static_assert(!frt::traits::DisjunctionTrait<std::false_type, std::false_type>::value);

    static_assert(frt::traits::disjunction<std::true_type, std::true_type>);
    static_assert(frt::traits::disjunction<std::true_type, std::false_type>);
    static_assert(frt::traits::disjunction<std::false_type, std::true_type>);
    static_assert(!frt::traits::disjunction<std::false_type, std::false_type>);

    static_assert(frt::traits::DisjunctionTrait<std::true_type, std::true_type, std::true_type>::value);
    static_assert(frt::traits::DisjunctionTrait<std::true_type, std::false_type, std::true_type>::value);
    static_assert(frt::traits::DisjunctionTrait<std::false_type, std::true_type, std::true_type>::value);
    static_assert(frt::traits::DisjunctionTrait<std::false_type, std::false_type, std::true_type>::value);
    static_assert(frt::traits::DisjunctionTrait<std::true_type, std::true_type, std::false_type>::value);
    static_assert(frt::traits::DisjunctionTrait<std::true_type, std::false_type, std::false_type>::value);
    static_assert(frt::traits::DisjunctionTrait<std::false_type, std::true_type, std::false_type>::value);
    static_assert(!frt::traits::DisjunctionTrait<std::false_type, std::false_type, std::false_type>::value);

    static_assert(frt::traits::disjunction<std::true_type, std::true_type, std::true_type>);
    static_assert(frt::traits::disjunction<std::true_type, std::false_type, std::true_type>);
    static_assert(frt::traits::disjunction<std::false_type, std::true_type, std::true_type>);
    static_assert(frt::traits::disjunction<std::false_type, std::false_type, std::true_type>);
    static_assert(frt::traits::disjunction<std::true_type, std::true_type, std::false_type>);
    static_assert(frt::traits::disjunction<std::true_type, std::false_type, std::false_type>);
    static_assert(frt::traits::disjunction<std::false_type, std::true_type, std::false_type>);
    static_assert(!frt::traits::disjunction<std::false_type, std::false_type, std::false_type>);

    static_assert(frt::traits::DisjunctionTrait<True>::value);
    static_assert(!frt::traits::DisjunctionTrait<False>::value);

    static_assert(frt::traits::disjunction<True>);
    static_assert(!frt::traits::disjunction<False>);
  }

  TEST(FrtTypesTraits, Negation) {
    static_assert(!frt::traits::NegationTrait<std::true_type>::value);
    static_assert(frt::traits::NegationTrait<std::false_type>::value);

    static_assert(!frt::traits::negation<std::true_type>);
    static_assert(frt::traits::negation<std::false_type>);

    static_assert(!frt::traits::NegationTrait<True>::value);
    static_assert(frt::traits::NegationTrait<False>::value);

    static_assert(!frt::traits::negation<True>);
    static_assert(frt::traits::negation<False>);

    static_assert(frt::traits::NegationTrait<std::negation<std::true_type>>::value);
    static_assert(!frt::traits::NegationTrait<std::negation<std::false_type>>::value);
  }
} // namespace