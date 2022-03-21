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
        traits::Conditional<RandomAccessIterator<Iter>, RandomAccessIteratorTag, BidirectionalIteratorTag>;

    template <typename Iter>
    using GetIteratorCategory = traits::Conditional<DerivedFrom<internal::IterConcept<Iter>, RandomAccessIteratorTag>,
        RandomAccessIteratorTag,
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
} // namespace frt