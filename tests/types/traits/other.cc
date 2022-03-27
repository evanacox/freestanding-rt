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
#include "test_utilities.h"
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
  template <typename T, typename U> bool test_decay() {
    static_assert(frt::SameAs<U, frt::traits::Decay<T>>);
    static_assert(frt::SameAs<U, typename frt::traits::DecayTrait<T>::type>);

    return true;
  }

  TEST(FrtTypesTraits, Decay) {
    test_decay<void, void>();
    test_decay<int, int>();
    test_decay<const volatile int, int>();
    test_decay<int*, int*>();
    test_decay<int[3], int*>();
    test_decay<const int[3], const int*>();
    test_decay<void(), void (*)()>();
    test_decay<int(int) const, int(int) const>();
    test_decay<int(int) volatile, int(int) volatile>();
    test_decay<int(int)&, int(int)&>();
    test_decay<int(int)&&, int(int) &&>();
  }

  template <typename T, typename U> void test_is_base_of() {
    static_assert((frt::traits::IsBaseOfTrait<T, U>::value));
    static_assert((frt::traits::IsBaseOfTrait<const T, U>::value));
    static_assert((frt::traits::IsBaseOfTrait<T, const U>::value));
    static_assert((frt::traits::IsBaseOfTrait<const T, const U>::value));
    static_assert((frt::traits::is_base_of<T, U>));
    static_assert((frt::traits::is_base_of<const T, U>));
    static_assert((frt::traits::is_base_of<T, const U>));
    static_assert((frt::traits::is_base_of<const T, const U>));
  }

  template <typename T, typename U> void test_is_not_base_of() {
    static_assert(!frt::traits::is_base_of<T, U>);
    static_assert(!frt::traits::IsBaseOfTrait<T, U>::value);
  }

  namespace is_base_of {
    struct B {};
    struct B1 : B {};
    struct B2 : B {};
    struct D : private B1, private B2 {};
    struct I0; // incomplete
    union U0;
    union U1 {};
    struct I0;
    struct I1 {};

    TEST(FrtTypesTraits, IsBaseOf) {
      test_is_base_of<B, D>();
      test_is_base_of<B1, D>();
      test_is_base_of<B2, D>();
      test_is_base_of<B, B1>();
      test_is_base_of<B, B2>();
      test_is_base_of<B, B>();

      test_is_not_base_of<D, B>();
      test_is_not_base_of<B&, D&>();
      test_is_not_base_of<B[3], D[3]>();
      test_is_not_base_of<int, int>();

      //  A scalar is never the base class of anything (including incomplete types)
      test_is_not_base_of<int, B>();
      test_is_not_base_of<int, B1>();
      test_is_not_base_of<int, B2>();
      test_is_not_base_of<int, D>();
      test_is_not_base_of<int, I0>();

      //  A scalar never has base classes (including incomplete types)
      test_is_not_base_of<B, int>();
      test_is_not_base_of<B1, int>();
      test_is_not_base_of<B2, int>();
      test_is_not_base_of<D, int>();
      test_is_not_base_of<I0, int>();

      test_is_not_base_of<U0, B>();
      test_is_not_base_of<U0, B1>();
      test_is_not_base_of<U0, B2>();
      test_is_not_base_of<U0, D>();
      test_is_not_base_of<U1, B>();
      test_is_not_base_of<U1, B1>();
      test_is_not_base_of<U1, B2>();
      test_is_not_base_of<U1, D>();
      test_is_not_base_of<U0, I0>();
      test_is_not_base_of<U1, I1>();
      test_is_not_base_of<U0, U1>();
      test_is_not_base_of<U0, int>();
      test_is_not_base_of<U1, int>();
      test_is_not_base_of<I0, int>();
      test_is_not_base_of<I1, int>();

      // A union never has base classes (including incomplete types)
      test_is_not_base_of<B, U0>();
      test_is_not_base_of<B1, U0>();
      test_is_not_base_of<B2, U0>();
      test_is_not_base_of<D, U0>();
      test_is_not_base_of<B, U1>();
      test_is_not_base_of<B1, U1>();
      test_is_not_base_of<B2, U1>();
      test_is_not_base_of<D, U1>();
      test_is_not_base_of<I0, U0>();
      test_is_not_base_of<I1, U1>();
      test_is_not_base_of<U1, U0>();
      test_is_not_base_of<int, U0>();
      test_is_not_base_of<int, U1>();
      test_is_not_base_of<int, I0>();
      test_is_not_base_of<int, I1>();
    }
  } // namespace is_base_of

  namespace is_same {
    template <class T, class U> void test_is_same() {
      static_assert((frt::traits::IsSameTrait<T, U>::value));
      static_assert((!frt::traits::IsSameTrait<const T, U>::value));
      static_assert((!frt::traits::IsSameTrait<T, const U>::value));
      static_assert((frt::traits::IsSameTrait<const T, const U>::value));
      static_assert((frt::traits::is_same<T, U>));
      static_assert((!frt::traits::is_same<const T, U>));
      static_assert((!frt::traits::is_same<T, const U>));
      static_assert((frt::traits::is_same<const T, const U>));
    }

    template <class T, class U> void test_is_same_ref() {
      static_assert((frt::traits::IsSameTrait<T, U>::value));
      static_assert((frt::traits::IsSameTrait<const T, U>::value));
      static_assert((frt::traits::IsSameTrait<T, const U>::value));
      static_assert((frt::traits::IsSameTrait<const T, const U>::value));
      static_assert((frt::traits::is_same<T, U>));
      static_assert((frt::traits::is_same<const T, U>));
      static_assert((frt::traits::is_same<T, const U>));
      static_assert((frt::traits::is_same<const T, const U>));
    }

    template <class T, class U> void test_is_not_same() {
      static_assert((!frt::traits::IsSameTrait<T, U>::value));
    }

    class Class {
    public:
      ~Class();
    };

    TEST(FrtTypesTraits, SameAs) {
      test_is_same<int, int>();
      test_is_same<void, void>();
      test_is_same<Class, Class>();
      test_is_same<int*, int*>();
      test_is_same_ref<int&, int&>();

      test_is_not_same<int, void>();
      test_is_not_same<void, Class>();
      test_is_not_same<Class, int*>();
      test_is_not_same<int*, int&>();
      test_is_not_same<int&, int>();
    }
  } // namespace is_same

  bool not_constant() {
    return frt::traits::is_constant_evaluated();
  }

  TEST(FrtTypesTraits, IsConstantEvaluated) {
    CHECK_SAME_TYPE(bool, frt::traits::is_constant_evaluated());
    static_assert(noexcept(frt::traits::is_constant_evaluated()));
    static_assert(frt::traits::is_constant_evaluated());

    EXPECT_FALSE(frt::traits::is_constant_evaluated());

    static bool local = frt::traits::is_constant_evaluated();
    EXPECT_TRUE(local);

    static bool local2 = not_constant();
    EXPECT_FALSE(local2);

    static_assert(frt::SameAs<frt::traits::BoolConstant<frt::traits::is_constant_evaluated()>, frt::traits::TrueType>);
  }
} // namespace
