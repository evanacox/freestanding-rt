//======---------------------------------------------------------------======//
//                                                                           //
// Copyright 2021-2022 Evan Cox <evanacox00@gmail.com>. All rights reserved. //
//                                                                           //
// Use of this source code is governed by a BSD-style license that can be    //
// found in the LICENSE.txt file at the root of this project, or at the      //
// following link: https://opensource.org/licenses/BSD-3-Clause              //
//                                                                           //
//======---------------------------------------------------------------======//

#include "frt/types/invoke.h"
#include "frt/types/concepts.h"
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
  namespace invoke_throws {
    struct Tag {};
    struct DerFromTag : Tag {};

    struct Implicit {
      Implicit(int /*unused*/) {} // NOLINT(google-explicit-constructor)
    };

    struct Explicit {
      explicit Explicit(int /*unused*/) {}
    };

    struct NotCallableWithInt {
      int operator()(int /*unused*/) = delete;

      int operator()(Tag /*unused*/) {
        return 0;
      }
    };

    struct Sink {
      template <typename... Args> void operator()(Args&&... /*unused*/) const {}
    };

    template <typename F, typename... Args> constexpr void invocable() {
      static_assert((frt::Invocable<F, Args...>));
      static_assert((frt::traits::is_invocable<F, Args...>));
      static_assert((frt::traits::IsInvocableTrait<F, Args...>::value));
    }

    template <typename F, typename... Args> constexpr void not_invocable() {
      static_assert((!frt::Invocable<F, Args...>));
      static_assert((!frt::traits::is_invocable<F, Args...>));
      static_assert((!frt::traits::IsInvocableTrait<F, Args...>::value));
    }

    template <typename R, typename F, typename... Args> constexpr void invocable_r() {
      static_assert((frt::traits::is_invocable_r<R, F, Args...>));
      static_assert((frt::traits::IsInvocableRTrait<R, F, Args...>::value));
    }

    template <typename R, typename F, typename... Args> constexpr void not_invocable_r() {
      static_assert((!frt::traits::is_invocable_r<R, F, Args...>));
      static_assert((!frt::traits::IsInvocableRTrait<R, F, Args...>::value));
    }

    TEST(FrtTypesInvoke, Invoke) {
      using AbominableFunc = void(...) const;

      //  Non-callable things
      {
        not_invocable<void>();
        not_invocable<const void>();
        not_invocable<volatile void>();
        not_invocable<const volatile void>();
        not_invocable<std::nullptr_t>();
        not_invocable<int>();
        not_invocable<double>();

        not_invocable<int[]>();
        not_invocable<int[3]>();

        not_invocable<int*>();
        not_invocable<const int*>();
        not_invocable<int const*>();

        not_invocable<int&>();
        not_invocable<const int&>();
        not_invocable<int&&>();

        not_invocable<std::vector<int>>();
        not_invocable<std::vector<int*>>();
        not_invocable<std::vector<int**>>();

        not_invocable<AbominableFunc>();

        //  with parameters
        not_invocable<int, int>();
        not_invocable<int, double, float>();
        not_invocable<int, char, float, double>();
        not_invocable<Sink, AbominableFunc>();
        not_invocable<Sink, void>();
        not_invocable<Sink, const volatile void>();

        not_invocable_r<int, void>();
        not_invocable_r<int, const void>();
        not_invocable_r<int, volatile void>();
        not_invocable_r<int, const volatile void>();
        not_invocable_r<int, std::nullptr_t>();
        not_invocable_r<int, int>();
        not_invocable_r<int, double>();

        not_invocable_r<int, int[]>();
        not_invocable_r<int, int[3]>();

        not_invocable_r<int, int*>();
        not_invocable_r<int, const int*>();
        not_invocable_r<int, int const*>();

        not_invocable_r<int, int&>();
        not_invocable_r<int, const int&>();
        not_invocable_r<int, int&&>();

        not_invocable_r<int, std::vector<int>>();
        not_invocable_r<int, std::vector<int*>>();
        not_invocable_r<int, std::vector<int**>>();
        not_invocable_r<void, AbominableFunc>();

        //  with parameters
        not_invocable_r<int, int, int>();
        not_invocable_r<int, int, double, float>();
        not_invocable_r<int, int, char, float, double>();
        not_invocable_r<void, Sink, AbominableFunc>();
        not_invocable_r<void, Sink, void>();
        not_invocable_r<void, Sink, const volatile void>();
      }
      {
        using Fn = int (Tag::*)(int);
        using RFn = int (Tag::*)(int)&&;
        // INVOKE bullet 1, 2 and 3

        {
          // Bullet 1
          invocable<Fn, Tag&, int>();
          invocable<Fn, DerFromTag&, int>();
          invocable<RFn, Tag&&, int>();
          not_invocable<RFn, Tag&, int>();
          not_invocable<Fn, Tag&>();
          not_invocable<Fn, Tag const&, int>();
        }
        {
          // Bullet 2
          using T = frt::ReferenceWrapper<Tag>;
          using DT = frt::ReferenceWrapper<DerFromTag>;
          using CT = frt::ReferenceWrapper<const Tag>;
          invocable<Fn, T&, int>();
          invocable<Fn, DT&, int>();
          invocable<Fn, const T&, int>();
          invocable<Fn, T&&, int>();
          not_invocable<Fn, CT&, int>();
          not_invocable<RFn, T, int>();
        }

        {
          // Bullet 3
          using T = Tag*;
          using DT = DerFromTag*;
          using CT = const Tag*;
          using ST = std::unique_ptr<Tag>;
          invocable<Fn, T&, int>();
          invocable<Fn, DT&, int>();
          invocable<Fn, const T&, int>();
          invocable<Fn, T&&, int>();
          invocable<Fn, ST, int>();
          not_invocable<Fn, CT&, int>();
          not_invocable<RFn, T, int>();
        }
      }
      {
        // Bullets 4, 5 and 6
        using Fn = int(Tag::*);
        not_invocable<Fn>();
        {
          // Bullet 4
          invocable<Fn, Tag&>();
          invocable<Fn, DerFromTag&>();
          invocable<Fn, Tag&&>();
          invocable<Fn, Tag const&>();
        }
        {
          // Bullet 5
          using T = frt::ReferenceWrapper<Tag>;
          using DT = frt::ReferenceWrapper<DerFromTag>;
          using CT = frt::ReferenceWrapper<const Tag>;
          invocable<Fn, T&>();
          invocable<Fn, DT&>();
          invocable<Fn, const T&>();
          invocable<Fn, T&&>();
          invocable<Fn, CT&>();
        }
        {
          // Bullet 6
          using T = Tag*;
          using DT = DerFromTag*;
          using CT = const Tag*;
          using ST = std::unique_ptr<Tag>;
          invocable<Fn, T&>();
          invocable<Fn, DT&>();
          invocable<Fn, const T&>();
          invocable<Fn, T&&>();
          invocable<Fn, ST>();
          invocable<Fn, CT&>();
        }
      }

      // INVOKE bullet 7

      {
        // clang-format breaks if there's nothing before another {
        [[maybe_unused]] int x{};

        // Function pointer
        {
          using Fp = void (*)(Tag&, int);
          invocable<Fp, Tag&, int>();
          invocable<Fp, DerFromTag&, int>();
          not_invocable<Fp, const Tag&, int>();
          not_invocable<Fp>();
          not_invocable<Fp, Tag&>();
        }
        {
          // Function reference
          using Fp = void (&)(Tag&, int);
          invocable<Fp, Tag&, int>();
          invocable<Fp, DerFromTag&, int>();
          not_invocable<Fp, const Tag&, int>();
          not_invocable<Fp>();
          not_invocable<Fp, Tag&>();
        }
        {
          // Function object
          using Fn = NotCallableWithInt;
          invocable<Fn, Tag>();
          not_invocable<Fn, int>();
        }

        {
          // Check that the conversion to the return type is properly checked
          using Fn = int (*)();
          invocable_r<Implicit, Fn>();
          invocable_r<double, Fn>();
          invocable_r<const volatile void, Fn>();
          not_invocable_r<Explicit, Fn>();
        }
        {
          // Check for is_invocable_v
          using Fn = void (*)();
          invocable<Fn>();
          not_invocable<Fn, int>();
        }
        {
          // Check for is_invocable_r_v
          using Fn = void (*)();
          invocable_r<void, Fn>();
          not_invocable_r<int, Fn>();
        }
      }
    }
  } // namespace invoke_throws

  namespace invoke_nothrow {
    struct Tag {};

    struct Implicit {
      Implicit(int) noexcept {}
    };

    struct ThrowsImplicit {
      ThrowsImplicit(int) {}
    };

    struct Explicit {
      explicit Explicit(int) noexcept {}
    };

    template <bool IsNoexcept, class Ret, class... Args> struct CallObject {
      Ret operator()(Args&&...) const noexcept(IsNoexcept);
    };

    struct Sink {
      template <class... Args> void operator()(Args&&...) const noexcept {}
    };

    template <class Fn, class... Args> constexpr bool throws_invocable() {
      return std::is_invocable<Fn, Args...>::value && !std::is_nothrow_invocable<Fn, Args...>::value;
    }

    template <class Ret, class Fn, class... Args> constexpr bool throws_invocable_r() {
      return std::is_invocable_r<Ret, Fn, Args...>::value && !std::is_nothrow_invocable_r<Ret, Fn, Args...>::value;
    }

    template <typename... Args> constexpr void nothrow_invocable() {
      static_assert(frt::traits::is_nothrow_invocable<Args...>);
      static_assert(frt::traits::IsNothrowInvocableTrait<Args...>::value);
    }

    template <typename... Args> constexpr void not_nothrow_invocable() {
      static_assert(!frt::traits::is_nothrow_invocable<Args...>);
      static_assert(!frt::traits::IsNothrowInvocableTrait<Args...>::value);
    }

    template <typename... Args> constexpr void nothrow_invocable_r() {
      static_assert(frt::traits::is_nothrow_invocable_r<Args...>);
      static_assert(frt::traits::IsNothrowInvocableRTrait<Args...>::value);
    }

    template <typename... Args> constexpr void not_nothrow_invocable_r() {
      static_assert(!frt::traits::is_nothrow_invocable_r<Args...>);
      static_assert(!frt::traits::IsNothrowInvocableRTrait<Args...>::value);
    }

    void test_noexcept_function_pointers() {
      struct Dummy {
        void foo() noexcept {}
        static void bar() noexcept {}
      };

      // Check that PMF's and function pointers actually work and that
      // is_nothrow_invocable returns true for noexcept PMF's and function
      // pointers.
      nothrow_invocable<decltype(&Dummy::foo), Dummy&>();
      nothrow_invocable<decltype(&Dummy::bar)>();
    }

    TEST(FrtTypesInvoke, InvokeNothrow) {
      using AbominableFunc = void(...) const noexcept;
      //  Non-callable things
      {
        not_nothrow_invocable<void>();
        not_nothrow_invocable<const void>();
        not_nothrow_invocable<volatile void>();
        not_nothrow_invocable<const volatile void>();
        not_nothrow_invocable<std::nullptr_t>();
        not_nothrow_invocable<int>();
        not_nothrow_invocable<double>();

        not_nothrow_invocable<int[]>();
        not_nothrow_invocable<int[3]>();

        not_nothrow_invocable<int*>();
        not_nothrow_invocable<const int*>();
        not_nothrow_invocable<int const*>();

        not_nothrow_invocable<int&>();
        not_nothrow_invocable<const int&>();
        not_nothrow_invocable<int&&>();

        not_nothrow_invocable<int, std::vector<int>>();
        not_nothrow_invocable<int, std::vector<int*>>();
        not_nothrow_invocable<int, std::vector<int**>>();

        not_nothrow_invocable<AbominableFunc>();

        //  with parameters
        not_nothrow_invocable<int, int>();
        not_nothrow_invocable<int, double, float>();
        not_nothrow_invocable<int, char, float, double>();
        not_nothrow_invocable<Sink, AbominableFunc>();
        not_nothrow_invocable<Sink, void>();
        not_nothrow_invocable<Sink, const volatile void>();

        not_nothrow_invocable_r<int, void>();
        not_nothrow_invocable_r<int, const void>();
        not_nothrow_invocable_r<int, volatile void>();
        not_nothrow_invocable_r<int, const volatile void>();
        not_nothrow_invocable_r<int, std::nullptr_t>();
        not_nothrow_invocable_r<int, int>();
        not_nothrow_invocable_r<int, double>();

        not_nothrow_invocable_r<int, int[]>();
        not_nothrow_invocable_r<int, int[3]>();

        not_nothrow_invocable_r<int, int*>();
        not_nothrow_invocable_r<int, const int*>();
        not_nothrow_invocable_r<int, int const*>();

        not_nothrow_invocable_r<int, int&>();
        not_nothrow_invocable_r<int, const int&>();
        not_nothrow_invocable_r<int, int&&>();

        not_nothrow_invocable_r<int, std::vector<int>>();
        not_nothrow_invocable_r<int, std::vector<int*>>();
        not_nothrow_invocable_r<int, std::vector<int**>>();
        not_nothrow_invocable_r<void, AbominableFunc>();

        //  with parameters
        not_nothrow_invocable_r<int, int, int>();
        not_nothrow_invocable_r<int, int, double, float>();
        not_nothrow_invocable_r<int, int, char, float, double>();
        not_nothrow_invocable_r<void, Sink, AbominableFunc>();
        not_nothrow_invocable_r<void, Sink, void>();
        not_nothrow_invocable_r<void, Sink, const volatile void>();
      }

      {
        // Check that the conversion to the return type is properly checked
        using Fn = CallObject<true, int>;
        nothrow_invocable_r<Implicit, Fn>();
        nothrow_invocable_r<double, Fn>();
        nothrow_invocable_r<const volatile void, Fn>();
        static_assert(throws_invocable_r<ThrowsImplicit, Fn>());
        static_assert(!std::is_nothrow_invocable<Fn(), Explicit>());
      }
      {
        // Check that the conversion to the parameters is properly checked
        using Fn = CallObject<true, void, const Implicit&, const ThrowsImplicit&>;
        nothrow_invocable<Fn, Implicit&, ThrowsImplicit&>();
        nothrow_invocable<Fn, int, ThrowsImplicit&>();
        static_assert(throws_invocable<Fn, int, int>());
        not_nothrow_invocable<Fn>();
      }
      {
        // Check that the noexcept-ness of function objects is checked.
        using Fn = CallObject<true, void>;
        using Fn2 = CallObject<false, void>;
        nothrow_invocable<Fn>();
        static_assert(throws_invocable<Fn2>());
      }
      {
        // Check that PMD derefs are noexcept
        using Fn = int(Tag::*);
        nothrow_invocable<Fn, Tag&>();
        nothrow_invocable_r<Implicit, Fn, Tag&>();
        static_assert(throws_invocable_r<ThrowsImplicit, Fn, Tag&>());
      }
      {
        // Check for is_nothrow_invocable_v
        using Fn = CallObject<true, int>;
        static_assert(std::is_nothrow_invocable_v<Fn>);
        static_assert(!std::is_nothrow_invocable_v<Fn, int>);
      }
      {
        // Check for is_nothrow_invocable_r_v
        using Fn = CallObject<true, int>;
        static_assert(std::is_nothrow_invocable_r_v<void, Fn>);
        static_assert(!std::is_nothrow_invocable_r_v<int, Fn, int>);
      }
      test_noexcept_function_pointers();
    }
  } // namespace invoke_nothrow
} // namespace