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
  namespace is_convertible {
    template <typename T, typename U> void test_is_convertible() {
      static_assert((frt::traits::is_convertible<T, U>));
      static_assert((frt::traits::is_convertible<const T, U>));
      static_assert((frt::traits::is_convertible<T, const U>));
      static_assert((frt::traits::is_convertible<const T, const U>));
      static_assert((frt::traits::IsConvertibleTrait<T, U>::value));
      static_assert((frt::traits::IsConvertibleTrait<const T, U>::value));
      static_assert((frt::traits::IsConvertibleTrait<T, const U>::value));
      static_assert((frt::traits::IsConvertibleTrait<const T, const U>::value));
      static_assert((frt::ConvertibleTo<T, U>));
      static_assert((frt::ConvertibleTo<const T, U>));
      static_assert((frt::ConvertibleTo<T, const U>));
      static_assert((frt::ConvertibleTo<const T, const U>));
    }

    template <typename T, typename U> void test_is_not_convertible() {
      static_assert((!frt::traits::is_convertible<T, U>));
      static_assert((!frt::traits::is_convertible<const T, U>));
      static_assert((!frt::traits::is_convertible<T, const U>));
      static_assert((!frt::traits::is_convertible<const T, const U>));
      static_assert((!frt::traits::IsConvertibleTrait<T, U>::value));
      static_assert((!frt::traits::IsConvertibleTrait<const T, U>::value));
      static_assert((!frt::traits::IsConvertibleTrait<T, const U>::value));
      static_assert((!frt::traits::IsConvertibleTrait<const T, const U>::value));
      static_assert(!(frt::ConvertibleTo<T, U>));
      static_assert(!(frt::ConvertibleTo<const T, U>));
      static_assert(!(frt::ConvertibleTo<T, const U>));
      static_assert(!(frt::ConvertibleTo<const T, const U>));
    }

    using Function = void();
    using ConstFunction = void() const;
    using Array = char[1];

    struct StringType {
      StringType(const char*) {}
    };

    class NonCopyable {
    public:
      NonCopyable(const NonCopyable&) = delete;
    };

    template <typename T> class CannotInstantiate { inline static constexpr auto x = T::ThisExpressionWillBlowUp; };

    TEST(FrtTypesTraits, ConvertibleTo) {
      test_is_convertible<void, void>();
      test_is_not_convertible<void, Function>();
      test_is_not_convertible<void, Function&>();
      test_is_not_convertible<void, Function*>();
      test_is_not_convertible<void, Array>();
      test_is_not_convertible<void, Array&>();
      test_is_not_convertible<void, char>();
      test_is_not_convertible<void, char&>();
      test_is_not_convertible<void, char*>();
      test_is_not_convertible<char, void>();

      // Function
      test_is_not_convertible<Function, void>();
      test_is_not_convertible<Function, Function>();
      test_is_convertible<Function, Function&>();
      test_is_convertible<Function, Function*>();
      test_is_convertible<Function, Function* const>();

      static_assert((frt::traits::is_convertible<Function, Function&&>));

      test_is_not_convertible<Function, Array>();
      test_is_not_convertible<Function, Array&>();
      test_is_not_convertible<Function, char>();
      test_is_not_convertible<Function, char&>();
      test_is_not_convertible<Function, char*>();

      // Function&
      test_is_not_convertible<Function&, void>();
      test_is_not_convertible<Function&, Function>();
      test_is_convertible<Function&, Function&>();

      test_is_convertible<Function&, Function*>();
      test_is_not_convertible<Function&, Array>();
      test_is_not_convertible<Function&, Array&>();
      test_is_not_convertible<Function&, char>();
      test_is_not_convertible<Function&, char&>();
      test_is_not_convertible<Function&, char*>();

      // Function*
      test_is_not_convertible<Function*, void>();
      test_is_not_convertible<Function*, Function>();
      test_is_not_convertible<Function*, Function&>();
      test_is_convertible<Function*, Function*>();

      test_is_not_convertible<Function*, Array>();
      test_is_not_convertible<Function*, Array&>();
      test_is_not_convertible<Function*, char>();
      test_is_not_convertible<Function*, char&>();
      test_is_not_convertible<Function*, char*>();

      // Non-referencable function type
      static_assert((!frt::traits::is_convertible<ConstFunction, Function>));
      static_assert((!frt::traits::is_convertible<ConstFunction, Function*>));
      static_assert((!frt::traits::is_convertible<ConstFunction, Function&>));
      static_assert((!frt::traits::is_convertible<ConstFunction, Function&&>));
      static_assert((!frt::traits::is_convertible<Function*, ConstFunction>));
      static_assert((!frt::traits::is_convertible<Function&, ConstFunction>));
      static_assert((!frt::traits::is_convertible<ConstFunction, ConstFunction>));
      static_assert((!frt::traits::is_convertible<ConstFunction, void>));

      // Array
      test_is_not_convertible<Array, void>();
      test_is_not_convertible<Array, Function>();
      test_is_not_convertible<Array, Function&>();
      test_is_not_convertible<Array, Function*>();
      test_is_not_convertible<Array, Array>();

      static_assert((!frt::traits::is_convertible<Array, Array&>));
      static_assert((frt::traits::is_convertible<Array, const Array&>));
      static_assert((!frt::traits::is_convertible<Array, const volatile Array&>));

      static_assert((!frt::traits::is_convertible<const Array, Array&>));
      static_assert((frt::traits::is_convertible<const Array, const Array&>));
      static_assert((!frt::traits::is_convertible<Array, volatile Array&>));
      static_assert((!frt::traits::is_convertible<Array, const volatile Array&>));

      static_assert((frt::traits::is_convertible<Array, Array&&>));
      static_assert((frt::traits::is_convertible<Array, const Array&&>));
      static_assert((frt::traits::is_convertible<Array, volatile Array&&>));
      static_assert((frt::traits::is_convertible<Array, const volatile Array&&>));
      static_assert((frt::traits::is_convertible<const Array, const Array&&>));
      static_assert((!frt::traits::is_convertible<Array&, Array&&>));
      static_assert((!frt::traits::is_convertible<Array&&, Array&>));

      test_is_not_convertible<Array, char>();
      test_is_not_convertible<Array, char&>();

      static_assert((frt::traits::is_convertible<Array, char*>));
      static_assert((frt::traits::is_convertible<Array, const char*>));
      static_assert((frt::traits::is_convertible<Array, char* const>));
      static_assert((frt::traits::is_convertible<Array, char* const volatile>));

      static_assert((!frt::traits::is_convertible<const Array, char*>));
      static_assert((frt::traits::is_convertible<const Array, const char*>));

      static_assert((!frt::traits::is_convertible<char[42][42], char*>));
      static_assert((!frt::traits::is_convertible<char[][1], char*>));

      // Array&
      test_is_not_convertible<Array&, void>();
      test_is_not_convertible<Array&, Function>();
      test_is_not_convertible<Array&, Function&>();
      test_is_not_convertible<Array&, Function*>();
      test_is_not_convertible<Array&, Array>();

      static_assert((frt::traits::is_convertible<Array&, Array&>));
      static_assert((frt::traits::is_convertible<Array&, const Array&>));
      static_assert((!frt::traits::is_convertible<const Array&, Array&>));
      static_assert((frt::traits::is_convertible<const Array&, const Array&>));

      test_is_not_convertible<Array&, char>();
      test_is_not_convertible<Array&, char&>();

      static_assert((frt::traits::is_convertible<Array&, char*>));
      static_assert((frt::traits::is_convertible<Array&, const char*>));
      static_assert((!frt::traits::is_convertible<const Array&, char*>));
      static_assert((frt::traits::is_convertible<const Array&, const char*>));

      static_assert((frt::traits::is_convertible<Array, StringType>));
      static_assert((frt::traits::is_convertible<char(&)[], StringType>));

      // char
      test_is_not_convertible<char, void>();
      test_is_not_convertible<char, Function>();
      test_is_not_convertible<char, Function&>();
      test_is_not_convertible<char, Function*>();
      test_is_not_convertible<char, Array>();
      test_is_not_convertible<char, Array&>();

      test_is_convertible<char, char>();

      static_assert((!frt::traits::is_convertible<char, char&>));
      static_assert((frt::traits::is_convertible<char, const char&>));
      static_assert((!frt::traits::is_convertible<const char, char&>));
      static_assert((frt::traits::is_convertible<const char, const char&>));

      test_is_not_convertible<char, char*>();

      // char&
      test_is_not_convertible<char&, void>();
      test_is_not_convertible<char&, Function>();
      test_is_not_convertible<char&, Function&>();
      test_is_not_convertible<char&, Function*>();
      test_is_not_convertible<char&, Array>();
      test_is_not_convertible<char&, Array&>();

      test_is_convertible<char&, char>();

      static_assert((frt::traits::is_convertible<char&, char&>));
      static_assert((frt::traits::is_convertible<char&, const char&>));
      static_assert((!frt::traits::is_convertible<const char&, char&>));
      static_assert((frt::traits::is_convertible<const char&, const char&>));

      test_is_not_convertible<char&, char*>();

      // char*
      test_is_not_convertible<char*, void>();
      test_is_not_convertible<char*, Function>();
      test_is_not_convertible<char*, Function&>();
      test_is_not_convertible<char*, Function*>();
      test_is_not_convertible<char*, Array>();
      test_is_not_convertible<char*, Array&>();

      test_is_not_convertible<char*, char>();
      test_is_not_convertible<char*, char&>();

      static_assert((frt::traits::is_convertible<char*, char*>));
      static_assert((frt::traits::is_convertible<char*, const char*>));
      static_assert((!frt::traits::is_convertible<const char*, char*>));
      static_assert((frt::traits::is_convertible<const char*, const char*>));

      // NonCopyable
      static_assert((frt::traits::is_convertible<NonCopyable&, NonCopyable&>));
      static_assert((frt::traits::is_convertible<NonCopyable&, const NonCopyable&>));
      static_assert((frt::traits::is_convertible<NonCopyable&, const volatile NonCopyable&>));
      static_assert((frt::traits::is_convertible<NonCopyable&, volatile NonCopyable&>));
      static_assert((frt::traits::is_convertible<const NonCopyable&, const NonCopyable&>));
      static_assert((frt::traits::is_convertible<const NonCopyable&, const volatile NonCopyable&>));
      static_assert((frt::traits::is_convertible<volatile NonCopyable&, const volatile NonCopyable&>));
      static_assert((frt::traits::is_convertible<const volatile NonCopyable&, const volatile NonCopyable&>));
      static_assert((!frt::traits::is_convertible<const NonCopyable&, NonCopyable&>));
      // This test requires Access control SFINAE which we only have in C++11 or when
      // we are using the compiler builtin for is_convertible.
      test_is_not_convertible<NonCopyable&, NonCopyable>();

      // Ensure that CannotInstantiate is not instantiated by is_convertible when it is not needed.
      // For example CannotInstantiate is instatiated as a part of ADL lookup for arguments of type CannotInstantiate*.
      static_assert((frt::traits::is_convertible<CannotInstantiate<int>*, CannotInstantiate<int>*>));
    }
  } // namespace is_convertible

  namespace nothrow_convertible {
    struct A {};
    struct B {
    public:
      operator A() {
        return a;
      }
      A a;
    };

    class C {};
    class D {
    public:
      operator C() noexcept {
        return c;
      }
      C c;
    };

    TEST(FrtTypesTraits, NothrowConvertible) {
      static_assert((frt::traits::IsNothrowConvertibleTrait<int, double>::value));
      static_assert(!(frt::traits::IsNothrowConvertibleTrait<int, char*>::value));

      static_assert(!(frt::traits::IsNothrowConvertibleTrait<A, B>::value));
      static_assert((frt::traits::IsNothrowConvertibleTrait<D, C>::value));

      static_assert((frt::traits::is_nothrow_convertible<int, double>));
      static_assert(!(frt::traits::is_nothrow_convertible<int, char*>));

      static_assert(!(frt::traits::is_nothrow_convertible<A, B>));
      static_assert((frt::traits::is_nothrow_convertible<D, C>));

      static_assert((frt::traits::is_nothrow_convertible<const void, void>));
      static_assert((frt::traits::is_nothrow_convertible<volatile void, void>));
      static_assert((frt::traits::is_nothrow_convertible<void, const void>));
      static_assert((frt::traits::is_nothrow_convertible<void, volatile void>));

      static_assert(!(frt::traits::is_nothrow_convertible<int[], double[]>));
      static_assert(!(frt::traits::is_nothrow_convertible<int[], int[]>));
      static_assert(!(frt::traits::is_nothrow_convertible<int[10], int[10]>));
      static_assert(!(frt::traits::is_nothrow_convertible<int[10], double[10]>));
      static_assert(!(frt::traits::is_nothrow_convertible<int[5], double[10]>));
      static_assert(!(frt::traits::is_nothrow_convertible<int[10], A[10]>));

      typedef void V();
      typedef int I();
      static_assert(!(frt::traits::is_nothrow_convertible<V, V>));
      static_assert(!(frt::traits::is_nothrow_convertible<V, I>));
    }
  } // namespace nothrow_convertible
} // namespace