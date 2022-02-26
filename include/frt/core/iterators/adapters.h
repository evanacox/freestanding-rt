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

#include "./concepts.h"
#include "./iter_base.h"
#include "./operations.h"
#include "./tags.h"
#include "./traits.h"

namespace frt {
  template <BidirectionalIterator I>
  class ReverseIterator : public IteratorInterface<ReverseIterator<I>,
                              BidirectionalIteratorTag,
                              IterValue<I>,
                              IterReference<I>,
                              typename IteratorTraits<I>::pointer,
                              IterDifference<I>> {
  public:
    // NOLINTNEXTLINE(google-explicit-constructor)
    constexpr ReverseIterator(I current) noexcept : current_{current} {}

    [[nodiscard]] constexpr I base() const noexcept {
      return current_;
    }

    [[nodiscard]] constexpr bool eq(ReverseIterator other) const noexcept {
      return other.current_ == current_;
    }

    [[nodiscard]] constexpr decltype(auto) value() const noexcept {
      return *frt::prev(current_);
    }

    constexpr void prev() noexcept {
      ++current_;
    }

    constexpr void next() noexcept {
      --current_;
    }

  private:
    I current_;
  };

  template <RandomAccessIterator I>
  class ReverseIterator<I> : public IteratorInterface<ReverseIterator<I>,
                                 RandomIteratorTag,
                                 IterValue<I>,
                                 IterReference<I>,
                                 typename IteratorTraits<I>::pointer,
                                 IterDifference<I>> {
  public:
    // NOLINTNEXTLINE(google-explicit-constructor)
    constexpr ReverseIterator(I current) noexcept : current_{current} {}

    [[nodiscard]] constexpr I base() const noexcept {
      return current_;
    }

    [[nodiscard]] constexpr bool eq(ReverseIterator other) const noexcept {
      return other.current_ == current_;
    }

    [[nodiscard]] constexpr decltype(auto) value() const noexcept {
      return *frt::prev(current_);
    }

    constexpr void move(IterDifference<I> distance) noexcept {
      current_ += -distance;
    }

    IterDifference<I> distance_to(ReverseIterator other) const noexcept {
      return other.current_ - current_;
    }

    frt::StrongOrder cmp(ReverseIterator other) const noexcept {
      return current_ <=> other.current_;
    }

  private:
    I current_;
  };

  /// Makes a reverse iterator from a given iterator
  ///
  /// \param i The iterator value
  /// \return A reverse iterator
  template <typename Iter> constexpr ReverseIterator<Iter> make_reverse_iterator(Iter i) {
    return ReverseIterator<Iter>(i);
  }
} // namespace frt