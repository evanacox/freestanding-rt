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

namespace frt {
  namespace internal {
    template <bool CanThrow, typename... Ts> class Visitor;

    template <bool CanThrow, typename T> class Visitor<CanThrow, T> {
    public:
      virtual void visit(T& object) noexcept(!CanThrow) = 0;
    };

    template <bool CanThrow, typename T, typename... Ts>
    class Visitor<CanThrow, T, Ts...> : public Visitor<CanThrow, T>, public Visitor<CanThrow, Ts...> {};
  } // namespace internal

  /// Creates a base Visitor type for a bounded set of types.
  ///
  /// For each type `T` in `Ts`, `Visitor` has a member function with the following signature:
  /// ```
  /// virtual void visit(T& object) noexcept = 0;
  /// ```
  template <typename... Ts> class Visitor : public internal::Visitor<false, Ts...> {};

  /// Creates a base Visitor type for a bounded set of types, but allows each visit
  /// member function to throw.
  ///
  /// For each type `T` in `Ts`, `Visitor` has a member function with the following signature:
  /// ```
  /// virtual void visit(T& object) = 0;
  /// ```
  template <typename... Ts> class ThrowingVisitor : public internal::Visitor<false, Ts...> {};
} // namespace frt