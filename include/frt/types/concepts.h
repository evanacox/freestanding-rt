//======---------------------------------------------------------------======//
//                                                                           //
// Copyright 2021 Evan Cox <evanacox00@gmail.com>. All rights reserved.      //
//                                                                           //
// Use of this source code is governed by a BSD-style license that can be    //
// found in the LICENSE.txt file at the root of this project, or at the      //
// following link: https://opensource.org/licenses/BSD-3-Clause              //
//                                                                           //
//======---------------------------------------------------------------======//

#pragma once

#include "./traits.h"

namespace frt {
  /// Checks if two types are the same
  ///
  /// \tparam T The first type to check
  /// \tparam U The second type to check
  template <typename T, typename U>
  concept SameAs = traits::is_same<T, U>;

  /// Checks if type `T` is one of the types given in `Args`
  ///
  /// \tparam T The type to look for
  /// \tparam Args The types to look for `T` in
  template <typename T, typename... Args>
  concept OneOf = traits::internal::OneOf<T, Args...>;

  /// Checks if a given type is one of C++'s "arithmetic" types, i.e
  /// integral or floating-point
  ///
  /// \tparam T The type to check
  template <typename T>
  concept Arithmetic = traits::is_arithmetic<T>;

  /// Checks if a given type is one of C++'s "integral" types, i.e
  /// `bool`, a char type, or one of the integer types
  ///
  /// \tparam T The type to check
  template <typename T>
  concept Integral = traits::is_integral<T>;

  /// Checks if a given type is both one of the integral types and
  /// is one of the *unsigned* integral types
  ///
  /// \tparam T The type to check
  template <typename T>
  concept Unsigned = traits::is_unsigned<T>;

  /// Checks if a type is trivially copyable
  ///
  /// \tparam T The type to check
  template <typename T>
  concept TriviallyCopyable = traits::is_trivially_copyable<T>;
} // namespace frt