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
  template <typename T, typename U> void test_add_const_imp() {
    static_assert(frt::SameAs<const U, typename frt::traits::AddConstTrait<T>::type>);
    static_assert(frt::SameAs<const U, frt::traits::AddConst<T>>);
  }

  template <typename T> void test_add_const() {
    test_add_const_imp<T, const T>();
    test_add_const_imp<const T, const T>();
    test_add_const_imp<volatile T, volatile const T>();
    test_add_const_imp<const volatile T, const volatile T>();
  }

  template <typename T, typename U> void test_add_cv_imp() {
    static_assert(frt::SameAs<const volatile U, typename frt::traits::AddCVTrait<T>::type>);
    static_assert(frt::SameAs<const volatile U, frt::traits::AddCV<T>>);
  }

  template <typename T> void test_add_cv() {
    test_add_cv_imp<T, const volatile T>();
    test_add_cv_imp<const T, const volatile T>();
    test_add_cv_imp<volatile T, volatile const T>();
    test_add_cv_imp<const volatile T, const volatile T>();
  }

  template <typename T, typename U> void test_add_volatile_imp() {
    static_assert(frt::SameAs<volatile U, typename frt::traits::AddVolatileTrait<T>::type>);
    static_assert(frt::SameAs<volatile U, frt::traits::AddVolatile<T>>);
  }

  template <typename T> void test_add_volatile() {
    test_add_volatile_imp<T, volatile T>();
    test_add_volatile_imp<const T, const volatile T>();
    test_add_volatile_imp<volatile T, volatile T>();
    test_add_volatile_imp<const volatile T, const volatile T>();
  }

  template <typename T, typename U> void test_remove_const_imp() {
    static_assert(frt::SameAs<U, typename frt::traits::RemoveConstTrait<T>::type>);
    static_assert(frt::SameAs<U, frt::traits::RemoveConst<T>>);
  }

  template <typename T> void test_remove_const() {
    test_remove_const_imp<T, T>();
    test_remove_const_imp<const T, T>();
    test_remove_const_imp<volatile T, volatile T>();
    test_remove_const_imp<const volatile T, volatile T>();
  }
  template <typename T, typename U> void test_remove_cv_imp() {
    static_assert(frt::SameAs<U, typename frt::traits::RemoveCVTrait<T>::type>);
    static_assert(frt::SameAs<U, frt::traits::RemoveCV<T>>);
  }

  template <typename T> void test_remove_cv() {
    test_remove_cv_imp<T, T>();
    test_remove_cv_imp<const T, T>();
    test_remove_cv_imp<volatile T, T>();
    test_remove_cv_imp<const volatile T, T>();
  }

  template <typename T, typename U> void test_remove_volatile_imp() {
    static_assert(frt::SameAs<U, typename frt::traits::RemoveVolatileTrait<T>::type>);
    static_assert(frt::SameAs<U, frt::traits::RemoveVolatile<T>>);
  }

  template <typename T> void test_remove_volatile() {
    test_remove_volatile_imp<T, T>();
    test_remove_volatile_imp<const T, const T>();
    test_remove_volatile_imp<volatile T, T>();
    test_remove_volatile_imp<const volatile T, const T>();
  }

  TEST(FrtTypesTraits, AddConst) {
    test_add_const<void>();
    test_add_const<int>();
    test_add_const<int[3]>();
    test_add_const<int&>();
    test_add_const<const int&>();
    test_add_const<int*>();
    test_add_const<const int*>();
  }

  TEST(FrtTypesTraits, AddCV) {
    test_add_cv<void>();
    test_add_cv<int>();
    test_add_cv<int[3]>();
    test_add_cv<int&>();
    test_add_cv<const int&>();
    test_add_cv<int*>();
    test_add_cv<const int*>();
  }

  TEST(FrtTypesTraits, AddVolatile) {
    test_add_volatile<void>();
    test_add_volatile<int>();
    test_add_volatile<int[3]>();
    test_add_volatile<int&>();
    test_add_volatile<const int&>();
    test_add_volatile<int*>();
    test_add_volatile<const int*>();
  }

  TEST(FrtTypesTraits, RemoveConst) {
    test_remove_const<void>();
    test_remove_const<int>();
    test_remove_const<int[3]>();
    test_remove_const<int&>();
    test_remove_const<const int&>();
    test_remove_const<int*>();
    test_remove_const<const int*>();
  }

  TEST(FrtTypesTraits, RemoveVolatile) {
    test_remove_volatile<void>();
    test_remove_volatile<int>();
    test_remove_volatile<int[3]>();
    test_remove_volatile<int&>();
    test_remove_volatile<const int&>();
    test_remove_volatile<int*>();
    test_remove_volatile<const int*>();
  }

  TEST(FrtTypesTraits, RemoveCV) {
    test_remove_cv<void>();
    test_remove_cv<int>();
    test_remove_cv<int[3]>();
    test_remove_cv<int&>();
    test_remove_cv<const int&>();
    test_remove_cv<int*>();
    test_remove_cv<const int*>();
  }
} // namespace