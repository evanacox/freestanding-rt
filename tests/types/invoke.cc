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

TEST(FrtTypesInvoke, Invoke) {
  using AbominableFunc = void(...) const;

  //  Non-callable things
  {
    static_assert(!frt::Invocable<void>);
    static_assert(!frt::Invocable<const void>);
    static_assert(!frt::Invocable<volatile void>);
    static_assert(!frt::Invocable<const volatile void>);
    static_assert(!frt::Invocable<std::nullptr_t>);
    static_assert(!frt::Invocable<int>);
    static_assert(!frt::Invocable<double>);

    static_assert(!frt::Invocable<int[]>);
    static_assert(!frt::Invocable<int[3]>);

    static_assert(!frt::Invocable<int*>);
    static_assert(!frt::Invocable<const int*>);
    static_assert(!frt::Invocable<int const*>);

    static_assert(!frt::Invocable<int&>);
    static_assert(!frt::Invocable<const int&>);
    static_assert(!frt::Invocable<int&&>);

    static_assert(!frt::Invocable<std::vector<int>>);
    static_assert(!frt::Invocable<std::vector<int*>>);
    static_assert(!frt::Invocable<std::vector<int**>>);

    static_assert(!frt::Invocable<AbominableFunc>);

    //  with parameters
    static_assert(!frt::Invocable<int, int>);
    static_assert(!frt::Invocable<int, double, float>);
    static_assert(!frt::Invocable<int, char, float, double>);
    static_assert(!frt::Invocable<Sink, AbominableFunc>);
    static_assert(!frt::Invocable<Sink, void>);
    static_assert(!frt::Invocable<Sink, const volatile void>);

    static_assert(!frt::traits::is_invocable_r<int, void>);
    static_assert(!frt::traits::is_invocable_r<int, const void>);
    static_assert(!frt::traits::is_invocable_r<int, volatile void>);
    static_assert(!frt::traits::is_invocable_r<int, const volatile void>);
    static_assert(!frt::traits::is_invocable_r<int, std::nullptr_t>);
    static_assert(!frt::traits::is_invocable_r<int, int>);
    static_assert(!frt::traits::is_invocable_r<int, double>);

    static_assert(!frt::traits::is_invocable_r<int, int[]>);
    static_assert(!frt::traits::is_invocable_r<int, int[3]>);

    static_assert(!frt::traits::is_invocable_r<int, int*>);
    static_assert(!frt::traits::is_invocable_r<int, const int*>);
    static_assert(!frt::traits::is_invocable_r<int, int const*>);

    static_assert(!frt::traits::is_invocable_r<int, int&>);
    static_assert(!frt::traits::is_invocable_r<int, const int&>);
    static_assert(!frt::traits::is_invocable_r<int, int&&>);

    static_assert(!frt::traits::is_invocable_r<int, std::vector<int>>);
    static_assert(!frt::traits::is_invocable_r<int, std::vector<int*>>);
    static_assert(!frt::traits::is_invocable_r<int, std::vector<int**>>);
    static_assert(!frt::traits::is_invocable_r<void, AbominableFunc>);

    //  with parameters
    static_assert(!frt::traits::is_invocable_r<int, int, int>);
    static_assert(!frt::traits::is_invocable_r<int, int, double, float>);
    static_assert(!frt::traits::is_invocable_r<int, int, char, float, double>);
    static_assert(!frt::traits::is_invocable_r<void, Sink, AbominableFunc>);
    static_assert(!frt::traits::is_invocable_r<void, Sink, void>);
    static_assert(!frt::traits::is_invocable_r<void, Sink, const volatile void>);
  }
  {
    using Fn = int (Tag::*)(int);
    using RFn = int (Tag::*)(int)&&;
    // INVOKE bullet 1, 2 and 3
    {
      // Bullet 1
      static_assert(frt::Invocable<Fn, Tag&, int>);
      static_assert(frt::Invocable<Fn, DerFromTag&, int>);
      static_assert(frt::Invocable<RFn, Tag&&, int>);
      static_assert(!frt::Invocable<RFn, Tag&, int>);
      static_assert(!frt::Invocable<Fn, Tag&>);
      static_assert(!frt::Invocable<Fn, Tag const&, int>);
    }
    {
      // Bullet 2
      using T = frt::ReferenceWrapper<Tag>;
      using DT = frt::ReferenceWrapper<DerFromTag>;
      using CT = frt::ReferenceWrapper<const Tag>;
      static_assert(frt::Invocable<Fn, T&, int>);
      static_assert(frt::Invocable<Fn, DT&, int>);
      static_assert(frt::Invocable<Fn, const T&, int>);
      static_assert(frt::Invocable<Fn, T&&, int>);
      static_assert(!frt::Invocable<Fn, CT&, int>);
      static_assert(!frt::Invocable<RFn, T, int>);
    }

    {
      // Bullet 3
      using T = Tag*;
      using DT = DerFromTag*;
      using CT = const Tag*;
      using ST = std::unique_ptr<Tag>;
      static_assert(frt::Invocable<Fn, T&, int>);
      static_assert(frt::Invocable<Fn, DT&, int>);
      static_assert(frt::Invocable<Fn, const T&, int>);
      static_assert(frt::Invocable<Fn, T&&, int>);
      static_assert(frt::Invocable<Fn, ST, int>);
      static_assert(!frt::Invocable<Fn, CT&, int>);
      static_assert(!frt::Invocable<RFn, T, int>);
    }
  }
  {
    // Bullets 4, 5 and 6
    using Fn = int(Tag::*);
    static_assert(!frt::Invocable<Fn>);
    {
      // Bullet 4
      static_assert(frt::Invocable<Fn, Tag&>);
      static_assert(frt::Invocable<Fn, DerFromTag&>);
      static_assert(frt::Invocable<Fn, Tag&&>);
      static_assert(frt::Invocable<Fn, Tag const&>);
    }
    {
      // Bullet 5
      using T = std::reference_wrapper<Tag>;
      using DT = std::reference_wrapper<DerFromTag>;
      using CT = std::reference_wrapper<const Tag>;
      static_assert(frt::Invocable<Fn, T&>);
      static_assert(frt::Invocable<Fn, DT&>);
      static_assert(frt::Invocable<Fn, const T&>);
      static_assert(frt::Invocable<Fn, T&&>);
      static_assert(frt::Invocable<Fn, CT&>);
    }
    {
      // Bullet 6
      using T = Tag*;
      using DT = DerFromTag*;
      using CT = const Tag*;
      using ST = std::unique_ptr<Tag>;
      static_assert(frt::Invocable<Fn, T&>);
      static_assert(frt::Invocable<Fn, DT&>);
      static_assert(frt::Invocable<Fn, const T&>);
      static_assert(frt::Invocable<Fn, T&&>);
      static_assert(frt::Invocable<Fn, ST>);
      static_assert(frt::Invocable<Fn, CT&>);
    }
  }
  {    // INVOKE bullet 7
      {// Function pointer
          using Fp = void(*)(Tag&, int);
  static_assert(frt::Invocable<Fp, Tag&, int>);
  static_assert(frt::Invocable<Fp, DerFromTag&, int>);
  static_assert(!frt::Invocable<Fp, const Tag&, int>);
  static_assert(!frt::Invocable<Fp>);
  static_assert(!frt::Invocable<Fp, Tag&>);
}
{
  // Function reference
  using Fp = void (&)(Tag&, int);
  static_assert(frt::Invocable<Fp, Tag&, int>);
  static_assert(frt::Invocable<Fp, DerFromTag&, int>);
  static_assert(!frt::Invocable<Fp, const Tag&, int>);
  static_assert(!frt::Invocable<Fp>);
  static_assert(!frt::Invocable<Fp, Tag&>);
}
{
  // Function object
  using Fn = NotCallableWithInt;
  static_assert(frt::Invocable<Fn, Tag>);
  static_assert(!frt::Invocable<Fn, int>);
}
}
{
  // Check that the conversion to the return type is properly checked
  using Fn = int (*)();
  static_assert(frt::traits::is_invocable_r<Implicit, Fn>);
  static_assert(frt::traits::is_invocable_r<double, Fn>);
  static_assert(frt::traits::is_invocable_r<const volatile void, Fn>);
  static_assert(!frt::traits::is_invocable_r<Explicit, Fn>);
}
{
  // Check for is_invocable_v
  using Fn = void (*)();
  static_assert(frt::Invocable_v<Fn>);
  static_assert(!frt::Invocable_v<Fn, int>);
}
{
  // Check for is_invocable_r_v
  using Fn = void (*)();
  static_assert(frt::traits::is_invocable_r_v<void, Fn>);
  static_assert(!frt::traits::is_invocable_r_v<int, Fn>);
}
}