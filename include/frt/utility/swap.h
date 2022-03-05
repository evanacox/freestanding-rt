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

#include "../types/forward.h"
#include "../types/move.h"
#include "../types/traits.h"

namespace frt {
  namespace internal {
    template <typename T> void swap(T&, T&) = delete;

    template <typename T>
    concept UserDefined = traits::is_class<traits::RemoveReference<T>> || traits::is_enum<traits::RemoveReference<T>>;

    template <typename T, typename U>
    concept ADLHasSwap = (UserDefined<T> || UserDefined<U>) //
        &&requires(T&& t, U&& u) {
      swap(frt::forward<T>(t), frt::forward<U>(u));
    };

    struct Swap {
      template <typename T, typename U> constexpr void operator()(T&& lhs, U&& rhs) const noexcept {
        if constexpr (ADLHasSwap<T, U>) {
          swap(frt::forward<T>(lhs), frt::forward<U>(rhs));
        } else {
          auto temp = frt::forward<traits::RemoveReference<T>>(lhs);
          lhs = frt::forward<traits::RemoveReference<T>>(rhs);
          rhs = frt::forward<traits::RemoveReference<T>>(temp);
        }
      }

      template <typename T, typename U, frt::usize N>
      requires requires(const Swap& swap, T& lhs, U& rhs) {
        swap(lhs, rhs);
      } // NOLINTNEXTLINE(modernize-avoid-c-arrays)
      constexpr void operator()(T (&lhs)[N], U (&rhs)[N]) const noexcept {
        for (auto i = frt::usize{0}; i < N; ++i) {
          (*this)(lhs[i], rhs[i]);
        }
      }
    };
  } // namespace internal

  /// Customization point for swapping two objects.
  ///
  /// The following call signature is exposed:
  /// ```
  /// template <typename T, typename R>
  /// constexpr void swap(T&& a, U&& b) noexcept(/* noexcept-swappable */);
  /// ```
  inline constexpr internal::Swap swap = internal::Swap{};

  /// Exchanges `a`'s value with `new_value`, and returns the old value
  ///
  /// \param a A reference to the value to replace
  /// \param new_value The value to replace it with
  /// \return The old value of `a`
  template <typename T, typename U = T> constexpr T exchange(T& a, T&& new_value) noexcept {
    auto temp = frt::move(a);

    a = frt::forward<U>(new_value);

    return temp;
  }
} // namespace frt