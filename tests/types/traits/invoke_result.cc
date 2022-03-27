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
  struct wat {
    wat& operator*() {
      return *this;
    }
    void foo();
  };

  struct F {};
  struct FD : public F {};

  template <typename T, typename U> struct test_invoke_result;

  template <typename Fn, typename... Args, typename Ret> struct test_invoke_result<Fn(Args...), Ret> {
    static void call() {
      static_assert(frt::traits::is_invocable<Fn, Args...>);
      static_assert(frt::traits::IsInvocableTrait<Fn, Args...>::value);
      static_assert(frt::traits::is_invocable_r<Ret, Fn, Args...>);
      static_assert(frt::traits::IsInvocableRTrait<Ret, Fn, Args...>::value);
      static_assert(frt::SameAs<Ret, typename frt::traits::InvokeResultTrait<Fn, Args...>::type>);
      static_assert(frt::SameAs<Ret, frt::traits::InvokeResult<Fn, Args...>>);
    }
  };

  template <typename T, typename U> void test_result_of_imp() {
    test_invoke_result<T, U>::call();
  }

  TEST(FrtTypesTraits, InvokeResult) {
    {
      typedef char F::*PMD;
      test_result_of_imp<PMD(F&), char&>();
      test_result_of_imp<PMD(F const&), char const&>();
      test_result_of_imp<PMD(F volatile&), char volatile&>();
      test_result_of_imp<PMD(F const volatile&), char const volatile&>();

      test_result_of_imp<PMD(F &&), char&&>();
      test_result_of_imp<PMD(F const&&), char const&&>();
      test_result_of_imp<PMD(F volatile &&), char volatile&&>();
      test_result_of_imp<PMD(F const volatile&&), char const volatile&&>();

      test_result_of_imp<PMD(F), char&&>();
      test_result_of_imp<PMD(F const), char&&>();

      test_result_of_imp<PMD(FD&), char&>();
      test_result_of_imp<PMD(FD const&), char const&>();
      test_result_of_imp<PMD(FD volatile&), char volatile&>();
      test_result_of_imp<PMD(FD const volatile&), char const volatile&>();

      test_result_of_imp<PMD(FD &&), char&&>();
      test_result_of_imp<PMD(FD const&&), char const&&>();
      test_result_of_imp<PMD(FD volatile &&), char volatile&&>();
      test_result_of_imp<PMD(FD const volatile&&), char const volatile&&>();

      test_result_of_imp<PMD(FD), char&&>();
      test_result_of_imp<PMD(FD const), char&&>();

      test_result_of_imp<PMD(std::unique_ptr<F>), char&>();
      test_result_of_imp<PMD(std::unique_ptr<F const>), const char&>();
      test_result_of_imp<PMD(std::unique_ptr<FD>), char&>();
      test_result_of_imp<PMD(std::unique_ptr<FD const>), const char&>();

      test_result_of_imp<PMD(frt::ReferenceWrapper<F>), char&>();
      test_result_of_imp<PMD(frt::ReferenceWrapper<F const>), const char&>();
      test_result_of_imp<PMD(frt::ReferenceWrapper<FD>), char&>();
      test_result_of_imp<PMD(frt::ReferenceWrapper<FD const>), const char&>();
    }
    {
      test_result_of_imp<int (F::*(F&))()&, int>();
      test_result_of_imp<int (F::*(F&))() const&, int>();
      test_result_of_imp<int (F::*(F&))() volatile&, int>();
      test_result_of_imp<int (F::*(F&))() const volatile&, int>();
      test_result_of_imp<int (F::*(F const&))() const&, int>();
      test_result_of_imp<int (F::*(F const&))() const volatile&, int>();
      test_result_of_imp<int (F::*(F volatile&))() volatile&, int>();
      test_result_of_imp<int (F::*(F volatile&))() const volatile&, int>();
      test_result_of_imp<int (F::*(F const volatile&))() const volatile&, int>();

      test_result_of_imp<int (F::*(F &&))()&&, int>();
      test_result_of_imp<int (F::*(F &&))() const&&, int>();
      test_result_of_imp<int (F::*(F &&))() volatile&&, int>();
      test_result_of_imp<int (F::*(F &&))() const volatile&&, int>();
      test_result_of_imp<int (F::*(F const&&))() const&&, int>();
      test_result_of_imp<int (F::*(F const&&))() const volatile&&, int>();
      test_result_of_imp<int (F::*(F volatile &&))() volatile&&, int>();
      test_result_of_imp<int (F::*(F volatile &&))() const volatile&&, int>();
      test_result_of_imp<int (F::*(F const volatile&&))() const volatile&&, int>();

      test_result_of_imp<int (F::*(F))()&&, int>();
      test_result_of_imp<int (F::*(F))() const&&, int>();
      test_result_of_imp<int (F::*(F))() volatile&&, int>();
      test_result_of_imp<int (F::*(F))() const volatile&&, int>();
      test_result_of_imp<int (F::*(F const))() const&&, int>();
      test_result_of_imp<int (F::*(F const))() const volatile&&, int>();
    }
    {
      test_result_of_imp<int (F::*(FD&))()&, int>();
      test_result_of_imp<int (F::*(FD&))() const&, int>();
      test_result_of_imp<int (F::*(FD&))() volatile&, int>();
      test_result_of_imp<int (F::*(FD&))() const volatile&, int>();
      test_result_of_imp<int (F::*(FD const&))() const&, int>();
      test_result_of_imp<int (F::*(FD const&))() const volatile&, int>();
      test_result_of_imp<int (F::*(FD volatile&))() volatile&, int>();
      test_result_of_imp<int (F::*(FD volatile&))() const volatile&, int>();
      test_result_of_imp<int (F::*(FD const volatile&))() const volatile&, int>();

      test_result_of_imp<int (F::*(FD &&))()&&, int>();
      test_result_of_imp<int (F::*(FD &&))() const&&, int>();
      test_result_of_imp<int (F::*(FD &&))() volatile&&, int>();
      test_result_of_imp<int (F::*(FD &&))() const volatile&&, int>();
      test_result_of_imp<int (F::*(FD const&&))() const&&, int>();
      test_result_of_imp<int (F::*(FD const&&))() const volatile&&, int>();
      test_result_of_imp<int (F::*(FD volatile &&))() volatile&&, int>();
      test_result_of_imp<int (F::*(FD volatile &&))() const volatile&&, int>();
      test_result_of_imp<int (F::*(FD const volatile&&))() const volatile&&, int>();

      test_result_of_imp<int (F::*(FD))()&&, int>();
      test_result_of_imp<int (F::*(FD))() const&&, int>();
      test_result_of_imp<int (F::*(FD))() volatile&&, int>();
      test_result_of_imp<int (F::*(FD))() const volatile&&, int>();
      test_result_of_imp<int (F::*(FD const))() const&&, int>();
      test_result_of_imp<int (F::*(FD const))() const volatile&&, int>();
    }
    {
      test_result_of_imp<int (F::*(frt::ReferenceWrapper<F>))(), int>();
      test_result_of_imp<int (F::*(frt::ReferenceWrapper<const F>))() const, int>();
      test_result_of_imp<int (F::*(std::unique_ptr<F>))(), int>();
      test_result_of_imp<int (F::*(std::unique_ptr<const F>))() const, int>();
    }
    test_result_of_imp<decltype (&wat::foo)(wat), void>();
  }
} // namespace