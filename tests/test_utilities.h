//======---------------------------------------------------------------======//
//                                                                           //
// Copyright 2021-2022 Evan Cox <evanacox00@gmail.com>. All rights reserved. //
//                                                                           //
// Use of this source code is governed by a BSD-style license that can be    //
// found in the LICENSE.txt file at the root of this project, or at the      //
// following link: https://opensource.org/licenses/BSD-3-Clause              //
//                                                                           //
//======---------------------------------------------------------------======//

#pragma once

namespace tests {
  /// Simple aggregate type for tests
  template <typename T, typename U> struct AggregatePair { // NOLINT(fuchsia-trailing-return)
    T t;
    U u;
  };

  template <typename T, typename U> AggregatePair(T, U) -> AggregatePair<T, U>; // NOLINT(fuchsia-trailing-return)

  /// Simple wrapper value for tests
  template <typename T> struct Wrapper { T value; }; // NOLINT(fuchsia-trailing-return)

  template <typename T> Wrapper(T) -> Wrapper<T>; // NOLINT(fuchsia-trailing-return)

  /// A template that's always undefined
  template <typename T> struct UndefinedTemplate;
} // namespace tests

#define CHECK_SAME_TYPE(type, expr) static_assert(::frt::SameAs<type, decltype(expr)>)