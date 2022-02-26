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

#include "../types/concepts.h"
#include "../types/traits.h"
#include "./limits.h"
#include "./memory.h"

namespace frt {
  /// Indicates the endian-ness of the platform
  enum class Endian { little = __ORDER_LITTLE_ENDIAN__, big = __ORDER_BIG_ENDIAN__, native = __BYTE_ORDER__ };

  /// Bit-copies an object of type `From` into an object of type `To`
  ///
  /// \param value The value to bit-cast
  /// \return An object of type `To` with a bit representation copied from `value`
  template <TriviallyCopyable To, TriviallyCopyable From>
  [[nodiscard]] constexpr To bit_cast(const From& value) noexcept requires(sizeof(To) == sizeof(From)) {
    return __builtin_bit_cast(To, value);
  }

  /// Reverses the order of the bytes in the representation of `value`
  ///
  /// \param value The value to reverse the bytes of
  /// \return Returns a value of type `T` with the bytes of `value` reversed
  template <Integral T> [[nodiscard]] FRT_PURE constexpr T byte_swap(T value) noexcept {
    if constexpr (sizeof(T) == 1) {
      return value;
    } else if constexpr (sizeof(T) == 2) {
      return __builtin_bswap16(value);
    } else if constexpr (sizeof(T) == 4) {
      return __builtin_bswap32(value);
    } else if constexpr (sizeof(T) == 8) {
      return __builtin_bswap64(value);
    } else {
      static_assert(traits::internal::dependent_false<T>, "unsupported type for `byte_swap`");
    }
  }

  /// Returns the number of set bits in `value`
  ///
  /// \param value The value to check set bits of
  /// \return The number of set bits in `value`
  template <UnsignedIntegral T> [[nodiscard]] FRT_PURE constexpr int popcount(T value) noexcept {
    if constexpr (SameAs<T, unsigned long>) { // NOLINT(google-runtime-int)
      return __builtin_popcountll(value);
    } else if constexpr (SameAs<T, unsigned long long>) { // NOLINT(google-runtime-int)
      return __builtin_popcountl(value);
    } else {
      // promoted to `unsigned int`
      return __builtin_popcount(value);
    }
  }

  /// Checks if the value is a power of 2
  ///
  /// \param value The value to check
  /// \return True if the value is a power of 2, false otherwise
  template <UnsignedIntegral T> [[nodiscard]] FRT_PURE constexpr bool has_single_bit(T value) noexcept {
    return frt::popcount(value) == 1;
  }

  /// Counts the number of leading zeroes in the bit representation of `value`
  ///
  /// \param value The value to check
  /// \return The number of leading zeroes in the bit representation
  template <UnsignedIntegral T> [[nodiscard]] FRT_PURE constexpr int countl_zero(T value) noexcept {
    // GCC makes it UB to call the `clz` builtins with `0`
    if (value == 0) {
      return NumericLimits<T>::digits;
    }

    if constexpr (sizeof(T) <= sizeof(unsigned)) {
      constexpr auto diff = NumericLimits<unsigned>::digits - NumericLimits<T>::digits;

      // promoted to `unsigned int`, need to make sure we remove extra bits from total
      return __builtin_clz(value) - diff;
    } else if constexpr (sizeof(T) <= sizeof(unsigned long)) { // NOLINT(google-runtime-int)
      return __builtin_clzl(value);
    } else {
      return __builtin_clzll(value);
    }
  }

  /// Counts the number of leading one bits in the bit representation of `value`
  ///
  /// \param value The value to check
  /// \return The number of leading ones in the bit representation
  template <UnsignedIntegral T> [[nodiscard]] FRT_PURE constexpr int countl_one(T value) noexcept {
    // GCC makes it UB to call the `clz` builtins with `0`
    if (value == NumericLimits<T>::max) {
      return NumericLimits<T>::digits;
    }

    // need to account for promotion with the cast
    return countl_zero(static_cast<T>(~value));
  }

  /// Counts the number of trailing zeroes in the bit representation of `value`
  ///
  /// \param value The value to check
  /// \return The number of leading zeroes in the bit representation
  template <UnsignedIntegral T> [[nodiscard]] FRT_PURE constexpr int countr_zero(T value) noexcept {
    // GCC makes it UB to call the `clz` builtins with `0`
    if (value == 0) {
      return NumericLimits<T>::digits;
    }

    if constexpr (sizeof(T) <= sizeof(unsigned)) {
      return __builtin_ctz(value);
    } else if constexpr (sizeof(T) <= sizeof(unsigned long)) { // NOLINT(google-runtime-int)
      return __builtin_ctzl(value);
    } else {
      return __builtin_ctzll(value);
    }
  }

  /// Counts the number of trailing one bits in the bit representation of `value`
  ///
  /// \param value The value to check
  /// \return The number of leading ones in the bit representation
  template <UnsignedIntegral T> [[nodiscard]] FRT_PURE constexpr int countr_one(T value) noexcept {
    // GCC makes it UB to call the `clz` builtins with `0`
    if (value == NumericLimits<T>::max) {
      return NumericLimits<T>::digits;
    }

    // need to account for promotion with the cast
    return frt::countl_zero(static_cast<T>(~value));
  }

  /// If `x != 0`, calculates the minimum number of bits required to represent `value`, i.e
  /// `1 + floor(log2(value))`. If `x == 0`, returns `0`.
  ///
  /// This function is equivalent  to `T(NumericLimits<T>::digits) - T(countl_zero(value))`, ignoring type conversions.
  ///
  /// \param value The value to find the bit width of
  /// \return The minimum number of bits.
  template <UnsignedIntegral T> FRT_PURE constexpr T bit_width(T value) noexcept {
    // ex: 8-bit
    //
    //     value: 0101 1101 (93)
    //  expected: 0000 0111 (7)
    //                  ^ digits<T> - leading zeroes = 7
    //
    return static_cast<T>(NumericLimits<T>::digits) - static_cast<T>(frt::countl_zero(value));
  }

  /// Rounds up to the next largest power of 2 that's greater than or equal to `value`.
  ///
  /// The behavior is undefined if that value is not representable inside of type `T`.
  ///
  /// \param value The value to round up to the next power of 2
  /// \return The next power of 2 that's greater than or equal to `value`
  template <UnsignedIntegral T> FRT_PURE constexpr T bit_ceil(T value) noexcept {
    // ex: 8-bit
    //
    //     value: 0001 1111 (31)
    //  expected: 0010 0000 (32)
    //              ^
    //              this is 1 shifted exactly `bit_width(value - 1)` bits left
    //
    // note: (value - 1) to make powers of 2 slide down one, so the calculation still works
    if (value <= static_cast<T>(1)) {
      // `value - 1` would be kind of broken if we did that with `value == 0`
      return 1;
    }

    FRT_ASSERT(frt::bit_width<T>(value - static_cast<T>(1)) != NumericLimits<T>::digits,
        "next largest power of 2 is not representable inside of `T`");

    return static_cast<T>(1) << frt::bit_width<T>(value - static_cast<T>(1));
  }

  /// Rounds down to the smallest power of 2 that's not greater than `value`.
  ///
  /// \param value The value to round up to the next power of 2
  /// \return The next power of 2 that's greater than or equal to `value`
  template <UnsignedIntegral T> FRT_PURE constexpr T bit_floor(T value) noexcept {
    // ex: 8-bit
    //
    //     value: 0001 1111 (31)
    //  expected: 0001 0000 (16)
    //               ^
    //               this is 1 shifted exactly (digits<T> - leading_zeroes(value)) - 1 bits left
    //
    if (value == 0) {
      return 0;
    }

    return static_cast<T>(1) << frt::bit_width<T>(value);
  }

  /// Computes `x modulo y` where `y` is a power of 2, but does it in the fancy way that
  /// avoids needing to perform a modulo/division. Behavior is undefined if `y` is not a power of 2.
  ///
  /// \param x The dividend of the division operation
  /// \param y The divisor of the division operation
  /// \return Returns the remainder of the division, aka `x mod y`
  template <UnsignedIntegral T> FRT_PURE constexpr T modulo_pow2(T x, T y) noexcept {
    // ex:
    //         0111 1111 (127)
    //     mod 0001 0000 (16)
    //       = 0000 1111 (15)
    //         ^^^^ we want to shave off all bits until the first bit **after** the set bit of `y`
    //
    FRT_ASSERT(y != 0, "cannot calculate `mod 0`");
    FRT_ASSERT(frt::has_single_bit(y), "`y` must be a power of 2 for `modulo_pow2` to work!");

    return x & (y - 1);
  }

  /// Performs a circular left shift. See https://en.wikipedia.org/wiki/Circular_shift
  ///
  /// \param value The value to shift
  /// \param shift_by The number of bits to circular-shift by
  /// \return `value` shifted
  template <UnsignedIntegral T> FRT_PURE constexpr T rotl(T value, int shift_by) noexcept {
    static_assert(frt::has_single_bit(NumericLimits<T>::digits), "please use a better platform");

    auto s = static_cast<T>(shift_by);
    auto bits = static_cast<T>(NumericLimits<T>::digits);

    // GCC and Clang can both recognize this and turn it into the platform's rotate instruction
    return (value << frt::modulo_pow2(s, bits)) | (value >> frt::modulo_pow2(-s, bits));
  }

  /// Performs a circular right shift. See https://en.wikipedia.org/wiki/Circular_shift
  ///
  /// \param value The value to shift
  /// \param shift_by The number of bits to circular-shift by
  /// \return `value` shifted
  template <UnsignedIntegral T> FRT_PURE constexpr T rotr(T value, int shift_by) noexcept {
    static_assert(frt::has_single_bit(NumericLimits<T>::digits), "please use a better platform");

    auto s = static_cast<T>(shift_by);
    auto bits = static_cast<T>(NumericLimits<T>::digits);

    // GCC and Clang can both recognize this and turn it into the platform's rotate instruction
    return (value >> frt::modulo_pow2(s, bits)) | (value << frt::modulo_pow2(-s, bits));
  }
} // namespace frt
