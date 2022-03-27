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
#include "frt/types/invoke.h"
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
  struct E {};

  template <class T> struct X {
    explicit X(T const&) {}
  };

  template <class T> struct S {
    explicit S(T const&) {}
  };
} // namespace

namespace frt::traits {
  template <typename T> struct CommonTypeTrait<T, ::S<T>> { typedef S<T> type; };

  template <class T> struct CommonTypeTrait<::S<T>, T> { typedef S<T> type; };

  //  P0548
  template <class T> struct CommonTypeTrait<::S<T>, ::S<T>> { typedef S<T> type; };

  template <> struct CommonTypeTrait<::S<long>, long> {};
  template <> struct CommonTypeTrait<long, ::S<long>> {};
  template <> struct CommonTypeTrait<::X<double>, ::X<double>> {};
} // namespace frt::traits

namespace {
  template <class> struct VoidT { typedef void type; };

  template <class Tp> struct always_bool_imp { using type = bool; };
  template <class Tp> using always_bool = typename always_bool_imp<Tp>::type;

  template <class... Args>
  constexpr auto no_common_type_imp(int) -> always_bool<typename frt::traits::CommonTypeTrait<Args...>::type> {
    return false;
  }

  template <class... Args> constexpr bool no_common_type_imp(long) {
    return true;
  }

  template <class... Args> using no_common_type = std::integral_constant<bool, no_common_type_imp<Args...>(0)>;

  template <class T1, class T2> struct TernaryOp {
    static_assert((frt::traits::is_same<typename frt::traits::DecayTrait<T1>::type, T1>), "must be same");
    static_assert((frt::traits::is_same<typename frt::traits::DecayTrait<T2>::type, T2>), "must be same");
    typedef typename frt::traits::DecayTrait<decltype(false ? frt::traits::declval<T1>()
                                                            : frt::traits::declval<T2>())>::type type;
  };

  // -- If sizeof...(T) is zero, there shall be no member type.
  void test_bullet_one() {
    static_assert(no_common_type<>::value, "");
  }

  // If sizeof...(T) is one, let T0 denote the sole type constituting the pack T.
  // The member typedef-name type shall denote the same type as decay_t<T0>.
  void test_bullet_two() {
    static_assert((frt::traits::is_same<frt::traits::CommonTypeTrait<void>::type, void>), "");
    static_assert((frt::traits::is_same<frt::traits::CommonTypeTrait<int>::type, int>), "");
    static_assert((frt::traits::is_same<frt::traits::CommonTypeTrait<int const>::type, int>), "");
    static_assert((frt::traits::is_same<frt::traits::CommonTypeTrait<int volatile[]>::type, int volatile*>), "");
    static_assert((frt::traits::is_same<frt::traits::CommonTypeTrait<void (&)()>::type, void (*)()>), "");

    static_assert((no_common_type<X<double>>::value), "");
  }

  template <class T, class U, class Expect> void test_bullet_three_one_imp() {
    typedef typename frt::traits::DecayTrait<T>::type DT;
    typedef typename frt::traits::DecayTrait<U>::type DU;
    static_assert((!frt::traits::is_same<T, DT> || !frt::traits::is_same<U, DU>), "");
    static_assert((frt::traits::is_same<typename frt::traits::CommonTypeTrait<T, U>::type, Expect>), "");
    static_assert((frt::traits::is_same<typename frt::traits::CommonTypeTrait<U, T>::type, Expect>), "");
    static_assert((frt::traits::is_same<typename frt::traits::CommonTypeTrait<T, U>::type,
                      typename frt::traits::CommonTypeTrait<DT, DU>::type>),
        "");
  }

  // (3.3)
  // -- If sizeof...(T) is two, let the first and second types constituting T be
  //    denoted by T1 and T2, respectively, and let D1 and D2 denote the same types
  //    as decay_t<T1> and decay_t<T2>, respectively.
  // (3.3.1)
  //    -- If is_same_v<T1, D1> is false or is_same_v<T2, D2> is false, let C
  //       denote the same type, if any, as common_type_t<D1, D2>.
  void test_bullet_three_one() {
    // Test that the user provided specialization of common_type is used after
    // decaying T1.
    {
      typedef const S<int> T1;
      typedef int T2;
      test_bullet_three_one_imp<T1, T2, S<int>>();
    }
    // Test a user provided specialization that does not provide a typedef.
    {
      typedef const ::S<long> T1;
      typedef long T2;
      static_assert((no_common_type<T1, T2>::value), "");
      static_assert((no_common_type<T2, T1>::value), "");
    }
    // Test that the ternary operator is not applied when the types are the
    // same.
    {
      typedef const void T1;
      typedef void Expect;
      static_assert((frt::traits::is_same<frt::traits::CommonTypeTrait<T1, T1>::type, Expect>), "");
      static_assert(
          (frt::traits::is_same<frt::traits::CommonTypeTrait<T1, T1>::type, frt::traits::CommonTypeTrait<T1>::type>),
          "");
    }
    {
      typedef int const T1[];
      typedef int const* Expect;
      static_assert((frt::traits::is_same<frt::traits::CommonTypeTrait<T1, T1>::type, Expect>), "");
      static_assert(
          (frt::traits::is_same<frt::traits::CommonTypeTrait<T1, T1>::type, frt::traits::CommonTypeTrait<T1>::type>),
          "");
    }
  }

  // (3.3)
  // -- If sizeof...(T) is two, let the first and second types constituting T be
  //    denoted by T1 and T2, respectively, and let D1 and D2 denote the same types
  //    as decay_t<T1> and decay_t<T2>, respectively.
  // (3.3.1)
  //    -- If [...]
  // (3.3.2)
  //    -- Otherwise, let C denote the same type, if any, as
  //       decay_t<decltype(false ? declval<D1>() : declval<D2>())>
  void test_bullet_three_two() {
    {
      typedef int const* T1;
      typedef int* T2;
      typedef TernaryOp<T1, T2>::type Expect;
      static_assert((frt::traits::is_same<frt::traits::CommonTypeTrait<T1, T2>::type, Expect>), "");
      static_assert((frt::traits::is_same<frt::traits::CommonTypeTrait<T2, T1>::type, Expect>), "");
    }
    // Test that there is no ::type member when the ternary op is ill-formed
    {
      typedef int T1;
      typedef void T2;
      static_assert((no_common_type<T1, T2>::value), "");
      static_assert((no_common_type<T2, T1>::value), "");
    }
    {
      typedef int T1;
      typedef X<int> T2;
      static_assert((no_common_type<T1, T2>::value), "");
      static_assert((no_common_type<T2, T1>::value), "");
    }
    // Test that the ternary operator is not applied when the types are the
    // same.
    {
      typedef void T1;
      typedef void Expect;
      static_assert((frt::traits::is_same<frt::traits::CommonTypeTrait<T1, T1>::type, Expect>), "");
      static_assert(
          (frt::traits::is_same<frt::traits::CommonTypeTrait<T1, T1>::type, frt::traits::CommonTypeTrait<T1>::type>),
          "");
    }
  }

  // (3.4)
  // -- If sizeof...(T) is greater than two, let T1, T2, and R, respectively,
  // denote the first, second, and (pack of) remaining types constituting T.
  // Let C denote the same type, if any, as common_type_t<T1, T2>. If there is
  // such a type C, the member typedef-name type shall denote the
  // same type, if any, as common_type_t<C, R...>. Otherwise, there shall be
  // no member type.
  void test_bullet_four() {
    { // test that there is no ::type member
      static_assert((no_common_type<int, E>::value), "");
      static_assert((no_common_type<int, int, E>::value), "");
#if TEST_STD_VER >= 11
      static_assert((no_common_type<int, int, E, int>::value), "");
      static_assert((no_common_type<int, int, int, E>::value), "");
#endif
    }
  }

  // The example code specified in Note B for common_type
  namespace note_b_example {
    typedef bool (&PF1)();
    typedef short (*PF2)(long);

    struct S {
      operator PF2() const;
      double operator()(char, int&);
      void fn(long) const;
      char data;
    };

    typedef void (S::*PMF)(long) const;
    typedef char S::*PMD;

    using std::unique_ptr;

    template <typename...> struct FrtResultOfTrait {};

    template <typename F, typename... Args> struct FrtResultOfTrait<F(Args...)> {
      using type = frt::traits::InvokeResult<F, Args...>;
    };

    template <typename... Args> using FrtResultOf = typename FrtResultOfTrait<Args...>::type;

    static_assert((frt::traits::is_same<FrtResultOf<S(int)>, short>), "Error!");
    static_assert((frt::traits::is_same<FrtResultOf<S&(unsigned char, int&)>, double>), "Error!");
    static_assert((frt::traits::is_same<FrtResultOf<PF1()>, bool>), "Error!");
    static_assert((frt::traits::is_same<FrtResultOf<PMF(unique_ptr<S>, int)>, void>), "Error!");
    static_assert((frt::traits::is_same<FrtResultOf<PMD(S)>, char&&>), "Error!");
    static_assert((frt::traits::is_same<FrtResultOf<PMD(const S*)>, const char&>), "Error!");

  } // namespace note_b_example

  TEST(FrtTypesTraits, CommonType) {
    static_assert((frt::traits::is_same<frt::traits::CommonTypeTrait<int>::type, int>), "");
    static_assert((frt::traits::is_same<frt::traits::CommonTypeTrait<char>::type, char>), "");
    static_assert((frt::traits::is_same<frt::traits::CommonType<int>, int>), "");
    static_assert((frt::traits::is_same<frt::traits::CommonType<char>, char>), "");

    static_assert((frt::traits::is_same<frt::traits::CommonTypeTrait<int>::type, int>), "");
    static_assert((frt::traits::is_same<frt::traits::CommonTypeTrait<const int>::type, int>), "");
    static_assert((frt::traits::is_same<frt::traits::CommonTypeTrait<volatile int>::type, int>), "");
    static_assert((frt::traits::is_same<frt::traits::CommonTypeTrait<const volatile int>::type, int>), "");

    static_assert((frt::traits::is_same<frt::traits::CommonTypeTrait<int, int>::type, int>), "");
    static_assert((frt::traits::is_same<frt::traits::CommonTypeTrait<int, const int>::type, int>), "");

    static_assert((frt::traits::is_same<frt::traits::CommonTypeTrait<long, const int>::type, long>), "");
    static_assert((frt::traits::is_same<frt::traits::CommonTypeTrait<const long, int>::type, long>), "");
    static_assert((frt::traits::is_same<frt::traits::CommonTypeTrait<long, volatile int>::type, long>), "");
    static_assert((frt::traits::is_same<frt::traits::CommonTypeTrait<volatile long, int>::type, long>), "");
    static_assert((frt::traits::is_same<frt::traits::CommonTypeTrait<const long, const int>::type, long>), "");

    static_assert((frt::traits::is_same<frt::traits::CommonTypeTrait<double, char>::type, double>), "");
    static_assert((frt::traits::is_same<frt::traits::CommonTypeTrait<short, char>::type, int>), "");
    static_assert((frt::traits::is_same<frt::traits::CommonType<double, char>, double>), "");
    static_assert((frt::traits::is_same<frt::traits::CommonType<short, char>, int>), "");

    static_assert((frt::traits::is_same<frt::traits::CommonTypeTrait<double, char, long long>::type, double>), "");
    static_assert((frt::traits::is_same<frt::traits::CommonTypeTrait<unsigned, char, long long>::type, long long>), "");
    static_assert((frt::traits::is_same<frt::traits::CommonType<double, char, long long>, double>), "");
    static_assert((frt::traits::is_same<frt::traits::CommonType<unsigned, char, long long>, long long>), "");

    static_assert((frt::traits::is_same<frt::traits::CommonTypeTrait<void>::type, void>), "");
    static_assert((frt::traits::is_same<frt::traits::CommonTypeTrait<const void>::type, void>), "");
    static_assert((frt::traits::is_same<frt::traits::CommonTypeTrait<volatile void>::type, void>), "");
    static_assert((frt::traits::is_same<frt::traits::CommonTypeTrait<const volatile void>::type, void>), "");

    static_assert((frt::traits::is_same<frt::traits::CommonTypeTrait<void, const void>::type, void>), "");
    static_assert((frt::traits::is_same<frt::traits::CommonTypeTrait<const void, void>::type, void>), "");
    static_assert((frt::traits::is_same<frt::traits::CommonTypeTrait<void, volatile void>::type, void>), "");
    static_assert((frt::traits::is_same<frt::traits::CommonTypeTrait<volatile void, void>::type, void>), "");
    static_assert((frt::traits::is_same<frt::traits::CommonTypeTrait<const void, const void>::type, void>), "");

    static_assert((frt::traits::is_same<frt::traits::CommonTypeTrait<int, S<int>>::type, S<int>>), "");
    static_assert((frt::traits::is_same<frt::traits::CommonTypeTrait<int, S<int>, S<int>>::type, S<int>>), "");
    static_assert((frt::traits::is_same<frt::traits::CommonTypeTrait<int, int, S<int>>::type, S<int>>), "");

    test_bullet_one();
    test_bullet_two();
    test_bullet_three_one();
    test_bullet_three_two();
    test_bullet_four();

    //  P0548
    static_assert((frt::traits::is_same<frt::traits::CommonTypeTrait<S<int>>::type, S<int>>), "");
    static_assert((frt::traits::is_same<frt::traits::CommonTypeTrait<S<int>, S<int>>::type, S<int>>), "");

    static_assert((frt::traits::is_same<frt::traits::CommonTypeTrait<int>::type, int>), "");
    static_assert((frt::traits::is_same<frt::traits::CommonTypeTrait<const int>::type, int>), "");
    static_assert((frt::traits::is_same<frt::traits::CommonTypeTrait<volatile int>::type, int>), "");
    static_assert((frt::traits::is_same<frt::traits::CommonTypeTrait<const volatile int>::type, int>), "");

    static_assert((frt::traits::is_same<frt::traits::CommonTypeTrait<int, int>::type, int>), "");
    static_assert((frt::traits::is_same<frt::traits::CommonTypeTrait<const int, int>::type, int>), "");
    static_assert((frt::traits::is_same<frt::traits::CommonTypeTrait<int, const int>::type, int>), "");
    static_assert((frt::traits::is_same<frt::traits::CommonTypeTrait<const int, const int>::type, int>), "");

    // Test that we're really variadic in C++11
    static_assert(frt::traits::is_same<frt::traits::CommonTypeTrait<int, int, int, int, int, int, int, int>::type, int>,
        "");
  }
} // namespace