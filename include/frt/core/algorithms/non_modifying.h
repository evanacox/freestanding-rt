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

#include "../../types/invoke.h"
#include "../../types/traits.h"
#include "../../utility/functional.h"
#include "../iterators.h"
#include "./ranges.h"

namespace frt {
  namespace algorithm_internal {
    // clang-format off
    template <typename T, typename I, typename Proj>
    concept RangeIterEquals = IndirectBinaryPredicate <EqualTo, Projected<I, Proj>, const T*>;
    // clang-format on

    struct Find {
      template <InputIterator I, SentinelFor<I> S, typename T, ProjFor<I> Proj = Identity>
      [[nodiscard]] constexpr I operator()(I first, S last, const T& value, Proj proj = {}) const
          noexcept(traits::is_nothrow_invocable<Proj, IterValue<I>>) requires RangeIterEquals<T, I, Proj> {
        for (; first != last; ++first) {
          if (EqualTo{}(frt::invoke(proj, *first), value)) {
            return first;
          }
        }

        return first;
      }

      template <InputRange R, typename T, ProjFor<RangeIterator<R>> Proj = Identity>
      [[nodiscard]] constexpr BorrowedIterator<R> operator()(R&& range, const T& value, Proj proj = {}) const
          noexcept(traits::is_nothrow_invocable<Proj, RangeValue<R>>) {
        return (*this)(frt::begin(range), frt::end(range), value, frt::ref(proj));
      }
    };
  } // namespace algorithm_internal

  inline constexpr algorithm_internal::Find find{};

  namespace algorithm_internal {
    struct FindIf {
      template <InputIterator I,
          SentinelFor<I> S,
          ProjFor<I> Proj = Identity,
          IndirectUnaryPredicate<Projected<I, Proj>> Pred>
      [[nodiscard]] constexpr I operator()(I first, S last, Pred pred, Proj proj = {}) const
          noexcept(traits::is_nothrow_invocable<Proj, IterValue<I>>) {
        for (; first != last; ++first) {
          if (frt::invoke(pred, frt::invoke(proj, *first))) {
            return first;
          }
        }

        return first;
      }

      template <InputRange R,
          ProjFor<RangeIterator<R>> Proj = Identity,
          IndirectUnaryPredicate<Projected<RangeIterator<R>, Proj>> Pred>
      [[nodiscard]] constexpr BorrowedIterator<R> operator()(R&& range, Pred pred, Proj proj = {}) const
          noexcept(traits::is_nothrow_invocable<Proj, RangeValue<R>>) {
        return (*this)(frt::begin(range), frt::end(range), frt::ref(pred), frt::ref(proj));
      }
    };
  } // namespace algorithm_internal

  inline constexpr algorithm_internal::FindIf find_if{};

  namespace algorithm_internal {
    struct FindIfNot {
      template <InputIterator I,
          SentinelFor<I> S,
          ProjFor<I> Proj = Identity,
          IndirectUnaryPredicate<Projected<I, Proj>> Pred>
      [[nodiscard]] constexpr I operator()(I first, S last, Pred pred, Proj proj = {}) const
          noexcept(traits::is_nothrow_invocable<Proj, IterValue<I>>) {
        for (; first != last; ++first) {
          if (!frt::invoke(pred, frt::invoke(proj, *first))) {
            return first;
          }
        }

        return first;
      }

      template <InputRange R,
          ProjFor<RangeIterator<R>> Proj = Identity,
          IndirectUnaryPredicate<Projected<RangeIterator<R>, Proj>> Pred>
      [[nodiscard]] constexpr BorrowedIterator<R> operator()(R&& range, Pred pred, Proj proj = {}) const
          noexcept(traits::is_nothrow_invocable<Proj, RangeValue<R>>) {
        return (*this)(frt::begin(range), frt::end(range), frt::ref(pred), frt::ref(proj));
      }
    };
  } // namespace algorithm_internal

  inline constexpr algorithm_internal::FindIfNot find_if_not{};

  namespace algorithm_internal {
    struct AllOf {
      template <InputIterator I,
          SentinelFor<I> S,
          ProjFor<I> Proj = Identity,
          IndirectUnaryPredicate<Projected<I, Proj>> Pred>
      [[nodiscard]] constexpr bool operator()(I first, S last, Pred pred, Proj proj = {}) const
          noexcept(traits::is_nothrow_invocable<Proj, IterValue<I>>) {
        return frt::find_if_not(first, last, frt::ref(pred), frt::ref(proj)) == last;
      }

      template <InputRange R,
          ProjFor<RangeIterator<R>> Proj = Identity,
          IndirectUnaryPredicate<Projected<RangeIterator<R>, Proj>> Pred>
      [[nodiscard]] constexpr bool operator()(R&& range, Pred pred, Proj proj = {}) const
          noexcept(traits::is_nothrow_invocable<Proj, RangeValue<R>>) {
        return (*this)(frt::begin(range), frt::end(range), frt::ref(pred), frt::ref(proj));
      }
    };
  } // namespace algorithm_internal

  inline constexpr algorithm_internal::AllOf all_of{};

  namespace algorithm_internal {
    struct AnyOf {
      template <InputIterator I,
          SentinelFor<I> S,
          ProjFor<I> Proj = Identity,
          IndirectUnaryPredicate<Projected<I, Proj>> Pred>
      [[nodiscard]] constexpr bool operator()(I first, S last, Pred pred, Proj proj = {}) const
          noexcept(traits::is_nothrow_invocable<Proj, IterValue<I>>) {
        return frt::find_if(first, last, frt::ref(pred), frt::ref(proj)) != last;
      }

      template <InputRange R,
          ProjFor<RangeIterator<R>> Proj = Identity,
          IndirectUnaryPredicate<Projected<RangeIterator<R>, Proj>> Pred>
      [[nodiscard]] constexpr bool operator()(R&& range, Pred pred, Proj proj = {}) const
          noexcept(traits::is_nothrow_invocable<Proj, RangeValue<R>>) {
        return (*this)(frt::begin(range), frt::end(range), frt::ref(pred), frt::ref(proj));
      }
    };
  } // namespace algorithm_internal

  inline constexpr algorithm_internal::AnyOf any_of{};

  namespace algorithm_internal {
    struct NoneOf {
      template <InputIterator I,
          SentinelFor<I> S,
          ProjFor<I> Proj = Identity,
          IndirectUnaryPredicate<Projected<I, Proj>> Pred>
      [[nodiscard]] constexpr bool operator()(I first, S last, Pred pred, Proj proj = {}) const
          noexcept(traits::is_nothrow_invocable<Proj, IterValue<I>>) {
        return frt::find_if(first, last, frt::ref(pred), frt::ref(proj)) == last;
      }

      template <InputRange R,
          ProjFor<RangeIterator<R>> Proj = Identity,
          IndirectUnaryPredicate<Projected<RangeIterator<R>, Proj>> Pred>
      [[nodiscard]] constexpr bool operator()(R&& range, Pred pred, Proj proj = {}) const
          noexcept(traits::is_nothrow_invocable<Proj, RangeValue<R>>) {
        return (*this)(frt::begin(range), frt::end(range), frt::ref(pred), frt::ref(proj));
      }
    };
  } // namespace algorithm_internal

  inline constexpr algorithm_internal::NoneOf none_of{};
} // namespace frt