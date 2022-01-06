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
#include <limits.h>

namespace frt {
  enum class FloatRoundStyle {
    indeterminate = -1,
    toward_zero = 0,
    to_nearest = 1,
    toward_infinity = 2,
    toward_neg_infinity = 3
  };

  enum class FloatDenormStyle { indeterminate = -1, absent = 0, present = 1 };

  namespace internal {
    // computes the number of bits inside `T`
    template <typename T, bool Signed> consteval int compute_digits() noexcept {
      return CHAR_BIT * sizeof(T) - (static_cast<int>(Signed));
    }

    template <int Digits> consteval int compute_digits10() noexcept {
      // digits * log10(2) == digits10, and log10(2) ~= 0.3010299, we can say it's 3/10
      return Digits * 3 / 10; // NOLINT(readability-magic-numbers)
    }

    // computes the minimum value of a given integer type `T`, assuming two's complement
    template <typename T, int Digits, bool Signed> consteval T compute_min() noexcept {
      if constexpr (Signed) {
        // minimum value for a signed integer is a single set bit in the leading position
        return static_cast<T>(static_cast<T>(1) << Digits);
      }

      return static_cast<T>(0);
    }

    // computes the maximum value of a given integer type `T`, assuming two's complement
    template <typename T, T Min, bool Signed> consteval T compute_max() noexcept {
      if constexpr (Signed) {
        // `Min` is a set bit in leading position, while max is every bit except leading set
        return static_cast<T>(static_cast<T>(~0) ^ Min);
      }

      // unsigned max is just all bits set
      return static_cast<T>(~0);
    }

    template <typename T> struct NumericLimitsImpl {
      inline static constexpr bool is_specialized = false;
      inline static constexpr bool is_signed = false;
      inline static constexpr bool is_integer = false;
      inline static constexpr bool is_exact = false;
      inline static constexpr bool has_infinity = false;
      inline static constexpr bool has_quiet_NaN = false;     // NOLINT(readability-identifier-naming)
      inline static constexpr bool has_signaling_NaN = false; // NOLINT(readability-identifier-naming)
      inline static constexpr FloatDenormStyle has_denorm = FloatDenormStyle::absent;
      inline static constexpr bool has_denorm_loss = false;
      inline static constexpr FloatRoundStyle round_style = FloatRoundStyle::toward_zero;
      inline static constexpr bool is_iec559 = false;
      inline static constexpr bool is_bounded = false;
      inline static constexpr bool is_modulo = false;
      inline static constexpr int digits = 0;
      inline static constexpr int digits10 = 0;
      inline static constexpr int max_digits10 = 0;
      inline static constexpr int radix = 0;
      inline static constexpr T min = T();
      inline static constexpr T lowest = T();
      inline static constexpr T max = T();
      inline static constexpr T min_exponent = T();
      inline static constexpr T min_exponent10 = T();
      inline static constexpr T max_exponent = T();
      inline static constexpr T max_exponent10 = T();
      inline static constexpr bool traps = !SameAs<T, bool>; /* divide-by-zero traps */
      inline static constexpr bool tinyness_before = false;
      inline static constexpr T round_error = T();
      inline static constexpr T infinity = T();
      inline static constexpr T quiet_NaN = T();     // NOLINT(readability-identifier-naming)
      inline static constexpr T signaling_NaN = T(); // NOLINT(readability-identifier-naming)
      inline static constexpr T denorm_min = T();
    };

    template <Integral T> struct NumericLimitsImpl<T> {
      inline static constexpr bool is_specialized = false;
      inline static constexpr bool is_signed = traits::is_signed<T>;
      inline static constexpr bool is_integer = true;
      inline static constexpr bool is_exact = true;
      inline static constexpr bool has_infinity = false;
      inline static constexpr bool has_quiet_NaN = false;     // NOLINT(readability-identifier-naming)
      inline static constexpr bool has_signaling_NaN = false; // NOLINT(readability-identifier-naming)
      inline static constexpr FloatDenormStyle has_denorm = FloatDenormStyle::absent;
      inline static constexpr bool has_denorm_loss = false;
      inline static constexpr FloatRoundStyle round_style = FloatRoundStyle::toward_zero;
      inline static constexpr bool is_iec559 = false;
      inline static constexpr bool is_bounded = true;
      inline static constexpr bool is_modulo = !is_signed;
      inline static constexpr int digits = internal::compute_digits<T, is_signed>();
      inline static constexpr int digits10 = internal::compute_digits10<digits>();
      inline static constexpr int max_digits10 = 0;
      inline static constexpr int radix = 2; // everything is two's complement, and therefore base 2
      inline static constexpr T min = internal::compute_min<T, digits, is_signed>();
      inline static constexpr T max = internal::compute_max<T, min, is_signed>();
      inline static constexpr T lowest = min;
      inline static constexpr T min_exponent = T();
      inline static constexpr T min_exponent10 = T();
      inline static constexpr T max_exponent = T();
      inline static constexpr T max_exponent10 = T();
      inline static constexpr bool traps = !SameAs<T, bool>; /* divide-by-zero traps */
      inline static constexpr bool tinyness_before = false;
      inline static constexpr T round_error = T();
      inline static constexpr T infinity = T();
      inline static constexpr T quiet_NaN = T();     // NOLINT(readability-identifier-naming)
      inline static constexpr T signaling_NaN = T(); // NOLINT(readability-identifier-naming)
      inline static constexpr T denorm_min = T();
    };
  } // namespace internal

  /// Gets the limits of a given numeric type.
  template <typename T> struct NumericLimits : internal::NumericLimitsImpl<T> {};
} // namespace frt