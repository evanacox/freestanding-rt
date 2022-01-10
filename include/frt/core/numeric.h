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

#include "../types/concepts.h"
#include "../types/traits.h"
#include "../utility/swap.h"
#include "./bit.h"

namespace frt {
  /// Computes the absolute value of an integral value
  ///
  /// \param value The value to compute the absolute value of
  /// \return `(value < 0) ? -value : value`
  template <Integral T> [[nodiscard]] FRT_PURE constexpr T abs(T value) noexcept {
    if constexpr (UnsignedIntegral<T>) {
      return value;
    } else {
      return (value < static_cast<T>(0)) ? -value : value;
    }
  }

  /// Computes the absolute value of a `float` value
  ///
  /// \param value The value to compute the absolute value of
  /// \return The absolute value
  [[nodiscard]] FRT_PURE constexpr float abs(float value) noexcept {
    return __builtin_fabsf(value);
  }

  /// Computes the absolute value of a `double` value
  ///
  /// \param value The value to compute the absolute value of
  /// \return The absolute value
  [[nodiscard]] FRT_PURE constexpr double abs(double value) noexcept {
    return __builtin_fabs(value);
  }

  /// Computes the absolute value of a `double` value
  ///
  /// \param value The value to compute the absolute value of
  /// \return The absolute value
  [[nodiscard]] FRT_PURE constexpr long double abs(long double value) noexcept {
    return __builtin_fabsl(value);
  }

  namespace internal {
    // currently, this uses the Binary GCD algorithm, aka Stein's Algorithm.
    // see https://en.wikipedia.org/wiki/Binary_GCD_algorithm for details
    //
    // as far as I can see, any other optimizations on top of this algorithm are
    // much less consistent with being faster, but this algorithm is at least
    // much faster than a naive recursive implementation of Euclid's GCD algorithm
    template <UnsignedIntegral T> constexpr T gcd_stein(T u, T v) noexcept {
      auto i = frt::countr_zero(u);
      auto j = frt::countr_zero(v);
      auto k = (i < j) ? i : j;
      u >>= i;
      v >>= j;

      while (true) {
        if (u > v) {
          frt::swap(u, v);
        }

        v -= u;

        if (v == 0) {
          return u << k;
        }

        v >>= frt::countr_zero(v);
      }
    }

    template <typename T> constexpr T lcm(T u, T v) noexcept {
      // lcm(a, b) = |ab| / gcd(a, b)
      // note: it's safe to use gcd_stein here, `u` and `v` are not 0
      return (u * v) / gcd_stein(u, v);
    }
  } // namespace internal

  /// Finds the greatest common divisor between two integers
  ///
  /// \param x The first integer
  /// \param y The second integer
  /// \return The GCD between the two integers
  template <Integral T, Integral U> constexpr traits::CommonType<T, U> gcd(T x, U y) noexcept {
    static_assert(!OneOf<bool, T, U>, "cannot find the gcd between two booleans!");
    using Result = traits::MakeUnsigned<traits::CommonType<T, U>>;

    return (x == 0)   ? y // gcd(0, y) = y and gcd(x, 0) = x
           : (y == 0) ? x
                      : internal::gcd_stein(frt::abs<Result>(x), frt::abs<Result>(y));
  }

  /// Calculates the least common multiple for two integers
  ///
  /// \param x The first integer
  /// \param y The second integer
  /// \return The least common multiple for the two integers
  template <Integral T, Integral U> constexpr traits::CommonType<T, U> lcm(T x, U y) noexcept {
    static_assert(!OneOf<bool, T, U>, "cannot find the gcd between two booleans!");
    using Result = traits::MakeUnsigned<traits::CommonType<T, U>>;

    return (x == 0 || y == 0) ? 0 : internal::lcm(frt::abs<Result>(x), frt::abs<Result>(y));
  }
} // namespace frt