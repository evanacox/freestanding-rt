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

#include "../../types/concepts.h"
#include "./concepts.h"
#include "./traits.h"

namespace frt {
  namespace internal {
    template <typename Iter>
    using GetIteratorConcept =
        traits::Conditional<RandomAccessIterator<Iter>, RandomIteratorTag, BidirectionalIteratorTag>;

    template <typename Iter>
    using GetIteratorCategory = traits::Conditional<DerivedFrom<internal::IterConcept<Iter>, RandomIteratorTag>,
        RandomIteratorTag,
        typename IteratorTraits<Iter>::iterator_category>;

    template <typename Iter>
    concept InputIterator = internal::LegacyInputIterator<Iter> || InputIterator<Iter>;

    template <typename Iter>
    concept BidirectionalIterator = internal::LegacyBidirectionalIterator<Iter> || BidirectionalIterator<Iter>;
  } // namespace internal

  template <InputIterator Iter, class Distance> constexpr void advance(Iter& iterator, Distance len) {
    auto distance = IterDifference<Iter>(len);

    if constexpr (RandomAccessIterator<Iter> || internal::LegacyRandomAccessIterator<Iter>) {
      // works for negatives as well, taking care of negative requirement
      iterator += distance;
    } else {
      while (distance > 0) {
        --distance;
        ++iterator;
      }

      if constexpr (BidirectionalIterator<Iter> || internal::LegacyBidirectionalIterator<Iter>) {
        while (distance < 0) {
          ++distance;
          --iterator;
        }
      }
    }
  }

  template <internal::InputIterator Iter> constexpr IterDifference<Iter> distance(Iter first, Iter last) {
    if constexpr (RandomAccessIterator<Iter> || internal::LegacyRandomAccessIterator<Iter>) {
      return last - first;
    } else {
      IterDifference<Iter> result = 0;

      while (first != last) {
        ++first;
        ++result;
      }

      return result;
    }
  }

  template <internal::InputIterator Iter> constexpr Iter next(Iter iter, IterDifference<Iter> number_to_advance = 1) {
    frt::advance(iter, number_to_advance);
    return iter;
  }

  template <internal::BidirectionalIterator Iter>
  constexpr Iter prev(Iter iter, IterDifference<Iter> number_to_advance = 1) {
    frt::advance(iter, -number_to_advance);
    return iter;
  }

  template <typename Container>
  constexpr auto begin(Container& c) noexcept(noexcept(c.begin())) -> decltype(c.begin()) {
    return c.begin();
  }

  template <typename Container>
  constexpr auto begin(const Container& c) noexcept(noexcept(c.begin())) -> decltype(c.begin()) {
    return c.begin();
  }

  template <typename T, frt::usize N> constexpr T* begin(T (&array)[N]) noexcept {
    return array;
  }

  template <typename Container>
  constexpr auto cbegin(const Container& c) noexcept(noexcept(frt::begin(c))) -> decltype(frt::begin(c)) {
    return frt::begin(c);
  }

  template <typename Container> constexpr auto end(Container& c) noexcept(noexcept(c.end())) -> decltype(c.end()) {
    return c.end();
  }

  template <typename Container>
  constexpr auto end(const Container& c) noexcept(noexcept(c.end())) -> decltype(c.end()) {
    return c.end();
  }

  template <typename T, frt::usize N> constexpr T* end(T (&array)[N]) noexcept {
    return array + N;
  }

  template <typename Container>
  constexpr auto cend(const Container& c) noexcept(noexcept(frt::end(c))) -> decltype(frt::end(c)) {
    return frt::end(c);
  }

  template <typename Container>
  constexpr auto rbegin(Container& c) noexcept(noexcept(c.rbegin())) -> decltype(c.rbegin()) {
    return c.rbegin();
  }

  template <typename Container>
  constexpr auto rbegin(const Container& c) noexcept(noexcept(c.rbegin())) -> decltype(c.rbegin()) {
    return c.rbegin();
  }

  template <typename T, frt::usize N> constexpr T* rbegin(T (&array)[N]) noexcept {
    return frt::end(array);
  }

  template <typename Container>
  constexpr auto crbegin(const Container& c) noexcept(noexcept(frt::rbegin(c))) -> decltype(frt::rbegin(c)) {
    return frt::rbegin(c);
  }

  template <typename Container> constexpr auto rend(Container& c) noexcept(noexcept(c.rend())) -> decltype(c.rend()) {
    return c.rend();
  }

  template <typename Container>
  constexpr auto rend(const Container& c) noexcept(noexcept(c.rend())) -> decltype(c.rend()) {
    return c.rend();
  }

  template <typename T, frt::usize N> constexpr T* rend(T (&array)[N]) noexcept {
    return frt::begin(array);
  }

  template <typename Container>
  constexpr auto crend(const Container& c) noexcept(noexcept(frt::rend(c))) -> decltype(frt::rend(c)) {
    return frt::rend(c);
  }

  template <typename Container>
  constexpr auto size(const Container& c) noexcept(noexcept(c.isize())) -> decltype(c.size()) {
    return c.isize();
  }

  template <typename Container>
  constexpr auto ssize(const Container& c) noexcept(noexcept(c.isize())) // NOLINT(fuchsia-trailing-return)
      -> traits::CommonType<frt::isize, traits::MakeSigned<decltype(c.size())>> {
    return c.isize();
  }

  // NOLINTNEXTLINE(modernize-avoid-c-arrays)
  template <typename T, frt::usize N> constexpr frt::usize size([[maybe_unused]] const T (&array)[N]) noexcept {
    return N;
  }

  // NOLINTNEXTLINE(modernize-avoid-c-arrays)
  template <typename T, frt::isize N> constexpr frt::isize ssize([[maybe_unused]] const T (&array)[N]) noexcept {
    return N;
  }

  template <typename Container>
  constexpr auto empty(const Container& c) noexcept(noexcept(c.empty())) -> decltype(c.empty()) {
    return c.empty();
  }

  // NOLINTNEXTLINE(modernize-avoid-c-arrays)
  template <typename T, frt::usize N> constexpr bool empty([[maybe_unused]] const T (&array)[N]) noexcept {
    return false;
  }

  template <typename Container> constexpr auto data(Container& c) noexcept(noexcept(c.data())) -> decltype(c.data()) {
    return c.data();
  }

  template <typename Container>
  constexpr auto data(const Container& c) noexcept(noexcept(c.data())) -> decltype(c.data()) {
    return c.data();
  }

  // NOLINTNEXTLINE(modernize-avoid-c-arrays)
  template <typename T, frt::usize N> constexpr T* data(T (&array)[N]) noexcept {
    return array;
  }
} // namespace frt