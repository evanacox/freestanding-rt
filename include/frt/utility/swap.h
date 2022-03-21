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
  namespace swap_internal {
    template <typename T>
    concept UserDefined = traits::is_class<traits::RemoveReference<T>> || traits::is_enum<traits::RemoveReference<T>>;

    template <typename T, typename U>
    concept ADLHasSwap = (UserDefined<T> || UserDefined<U>) //
        &&requires(T&& t, U&& u) {
      swap(frt::forward<T>(t), frt::forward<U>(u));
    };

    struct Swap {
    private:
      template <typename T, typename U> [[nodiscard]] constexpr static bool is_noexcept() noexcept {
        if constexpr (ADLHasSwap<T, U>) {
          return noexcept(swap(frt::forward<T>(traits::declval<T&&>()), frt::forward<U>(traits::declval<U&&>())));
        }

        return traits::is_nothrow_move_assignable<
            traits::CommonType<traits::RemoveReference<T>, traits::RemoveReference<U>>>;
      }

    public:
      template <typename T, typename U>
      constexpr void operator()(T&& lhs, U&& rhs) const noexcept(is_noexcept<T, U>()) {
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
      constexpr void operator()(T (&lhs)[N], U (&rhs)[N]) const noexcept(noexcept((*this)(lhs[0], rhs[0]))) {
        for (auto i = frt::usize{0}; i < N; ++i) {
          (*this)(lhs[i], rhs[i]);
        }
      }
    };
  } // namespace swap_internal

  inline namespace swap_access {
    /// Customization point for swapping two objects.
    ///
    /// The following call signature is exposed:
    /// ```
    /// template <typename T, typename R>
    /// constexpr void swap(T&& a, U&& b) noexcept(/* noexcept-swappable */);
    /// ```
    inline constexpr swap_internal::Swap swap = swap_internal::Swap{};
  } // namespace swap_access
} // namespace frt