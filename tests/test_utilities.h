//======---------------------------------------------------------------======//
//                                                                           //
// Copyright 2021-2022 Evan Cox <evanacox00@gmail.com>. All rights reserved. //
//                                                                           //
// Use of this source code is governed by a BSD-style license that can be    //
// found in the LICENSE.txt file at the root of this project, or at the      //
// following link: https://opensource.org/licenses/BSD-3-Clause              //
//                                                                           //
//======---------------------------------------------------------------======//

/*
 * Note: Parts of the code below are adapted from the libc++ test suite, therefore it is
 * also under the LLVM copyright. See the license header below for details:
 */

//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#pragma once

#include "frt/core/iterators.h"
#include "frt/types/move.h"
#include "frt/types/traits.h"

/// Simple aggregate type for tests
template <typename T, typename U> struct AggregatePair { // NOLINT(fuchsia-trailing-return)
  T t;
  U u;
};

template <typename T, typename U> AggregatePair(T, U) -> AggregatePair<T, U>; // NOLINT(fuchsia-trailing-return)

/// Simple wrapper value for tests
template <typename T> struct Wrapper { T value; }; // NOLINT(fuchsia-trailing-return)

template <typename T> Wrapper(T) -> Wrapper<T>; // NOLINT(fuchsia-trailing-return)

/// A template that's always undefined
template <typename T> struct UndefinedTemplate;

template <typename It> class Cpp17TestOutputIterator {
  It it_;

  template <typename U> friend class Cpp17TestOutputIterator;

public:
  using iterator_category = frt::OutputIteratorTag;
  using value_type = void;
  using difference_type = typename frt::IteratorTraits<It>::difference_type;
  using pointer = It;
  using reference = typename frt::IteratorTraits<It>::reference;

  constexpr explicit Cpp17TestOutputIterator(It it) : it_(frt::move(it)) {}
  template <typename U> constexpr Cpp17TestOutputIterator(const Cpp17TestOutputIterator<U>& u) : it_(u.it_) {}

  constexpr reference operator*() const {
    return *it_;
  }

  constexpr Cpp17TestOutputIterator& operator++() {
    ++it_;
    return *this;
  }
  constexpr Cpp17TestOutputIterator operator++(int) {
    return Cpp17TestOutputIterator(it_++);
  }

  friend constexpr It base(const Cpp17TestOutputIterator& i) {
    return i.it_;
  }

  template <typename T> void operator,(T const&) = delete;
};

static_assert(frt::OutputIterator<Cpp17TestOutputIterator<int*>, int>);

template <typename It, typename ItTraits = It> class Cpp17TestInputIterator {
  using Traits = frt::IteratorTraits<ItTraits>;
  It it_;

  template <typename U, typename T> friend class Cpp17TestInputIterator;

public:
  using iterator_category = frt::InputIteratorTag;
  using value_type = typename Traits::value_type;
  using difference_type = typename Traits::difference_type;
  using pointer = It;
  using reference = typename Traits::reference;

  constexpr explicit Cpp17TestInputIterator(It it) : it_(it) {}
  template <typename U, typename T>
  constexpr Cpp17TestInputIterator(const Cpp17TestInputIterator<U, T>& u) : it_(u.it_) {}

  constexpr reference operator*() const {
    return *it_;
  }

  constexpr Cpp17TestInputIterator& operator++() {
    ++it_;
    return *this;
  }
  constexpr Cpp17TestInputIterator operator++(int) {
    return Cpp17TestInputIterator(it_++);
  }

  friend constexpr bool operator==(const Cpp17TestInputIterator& x, const Cpp17TestInputIterator& y) {
    return x.it_ == y.it_;
  }
  friend constexpr bool operator!=(const Cpp17TestInputIterator& x, const Cpp17TestInputIterator& y) {
    return x.it_ != y.it_;
  }

  friend constexpr It base(const Cpp17TestInputIterator& i) {
    return i.it_;
  }

  template <typename T> void operator,(T const&) = delete;
};

static_assert(frt::InputIterator<Cpp17TestInputIterator<int*>>);

template <typename It> class TestForwardIterator {
  It it_;

  template <typename U> friend class TestForwardIterator;

public:
  using iterator_category = frt::ForwardIteratorTag;
  using value_type = typename frt::IteratorTraits<It>::value_type;
  using difference_type = typename frt::IteratorTraits<It>::difference_type;
  using pointer = It;
  using reference = typename frt::IteratorTraits<It>::reference;

  constexpr TestForwardIterator() : it_() {}
  constexpr explicit TestForwardIterator(It it) : it_(it) {}
  template <typename U> constexpr TestForwardIterator(const TestForwardIterator<U>& u) : it_(u.it_) {}

  constexpr reference operator*() const {
    return *it_;
  }

  constexpr TestForwardIterator& operator++() {
    ++it_;
    return *this;
  }
  constexpr TestForwardIterator operator++(int) {
    return TestForwardIterator(it_++);
  }

  friend constexpr bool operator==(const TestForwardIterator& x, const TestForwardIterator& y) {
    return x.it_ == y.it_;
  }
  friend constexpr bool operator!=(const TestForwardIterator& x, const TestForwardIterator& y) {
    return x.it_ != y.it_;
  }

  friend constexpr It base(const TestForwardIterator& i) {
    return i.it_;
  }

  template <typename T> void operator,(T const&) = delete;
};

template <typename It> class TestBidirectionalIterator {
  It it_;

  template <typename U> friend class TestBidirectionalIterator;

public:
  using iterator_category = frt::BidirectionalIteratorTag;
  using value_type = typename frt::IteratorTraits<It>::value_type;
  using difference_type = typename frt::IteratorTraits<It>::difference_type;
  using pointer = It;
  using reference = typename frt::IteratorTraits<It>::reference;

  constexpr TestBidirectionalIterator() : it_() {}
  constexpr explicit TestBidirectionalIterator(It it) : it_(it) {}
  template <typename U> constexpr TestBidirectionalIterator(const TestBidirectionalIterator<U>& u) : it_(u.it_) {}

  constexpr reference operator*() const {
    return *it_;
  }

  constexpr TestBidirectionalIterator& operator++() {
    ++it_;
    return *this;
  }
  constexpr TestBidirectionalIterator& operator--() {
    --it_;
    return *this;
  }
  constexpr TestBidirectionalIterator operator++(int) {
    return TestBidirectionalIterator(it_++);
  }
  constexpr TestBidirectionalIterator operator--(int) {
    return TestBidirectionalIterator(it_--);
  }

  friend constexpr bool operator==(const TestBidirectionalIterator& x, const TestBidirectionalIterator& y) {
    return x.it_ == y.it_;
  }
  friend constexpr bool operator!=(const TestBidirectionalIterator& x, const TestBidirectionalIterator& y) {
    return x.it_ != y.it_;
  }

  friend constexpr It base(const TestBidirectionalIterator& i) {
    return i.it_;
  }

  template <typename T> void operator,(T const&) = delete;
};

template <typename It> class TestRandomAccessIterator {
  It it_;

  template <typename U> friend class TestRandomAccessIterator;

public:
  using iterator_category = frt::RandomAccessIteratorTag;
  using value_type = typename frt::IteratorTraits<It>::value_type;
  using difference_type = typename frt::IteratorTraits<It>::difference_type;
  using pointer = It;
  using reference = typename frt::IteratorTraits<It>::reference;

  constexpr TestRandomAccessIterator() : it_() {}
  constexpr explicit TestRandomAccessIterator(It it) : it_(it) {}
  template <typename U> constexpr TestRandomAccessIterator(const TestRandomAccessIterator<U>& u) : it_(u.it_) {}

  constexpr reference operator*() const {
    return *it_;
  }
  constexpr reference operator[](difference_type n) const {
    return it_[n];
  }

  constexpr TestRandomAccessIterator& operator++() {
    ++it_;
    return *this;
  }
  constexpr TestRandomAccessIterator& operator--() {
    --it_;
    return *this;
  }
  constexpr TestRandomAccessIterator operator++(int) {
    return TestRandomAccessIterator(it_++);
  }
  constexpr TestRandomAccessIterator operator--(int) {
    return TestRandomAccessIterator(it_--);
  }

  constexpr TestRandomAccessIterator& operator+=(difference_type n) {
    it_ += n;
    return *this;
  }
  constexpr TestRandomAccessIterator& operator-=(difference_type n) {
    it_ -= n;
    return *this;
  }
  friend constexpr TestRandomAccessIterator operator+(TestRandomAccessIterator x, difference_type n) {
    x += n;
    return x;
  }
  friend constexpr TestRandomAccessIterator operator+(difference_type n, TestRandomAccessIterator x) {
    x += n;
    return x;
  }
  friend constexpr TestRandomAccessIterator operator-(TestRandomAccessIterator x, difference_type n) {
    x -= n;
    return x;
  }
  friend constexpr difference_type operator-(TestRandomAccessIterator x, TestRandomAccessIterator y) {
    return x.it_ - y.it_;
  }

  friend constexpr bool operator==(const TestRandomAccessIterator& x, const TestRandomAccessIterator& y) {
    return x.it_ == y.it_;
  }
  friend constexpr bool operator!=(const TestRandomAccessIterator& x, const TestRandomAccessIterator& y) {
    return x.it_ != y.it_;
  }
  friend constexpr bool operator<(const TestRandomAccessIterator& x, const TestRandomAccessIterator& y) {
    return x.it_ < y.it_;
  }
  friend constexpr bool operator<=(const TestRandomAccessIterator& x, const TestRandomAccessIterator& y) {
    return x.it_ <= y.it_;
  }
  friend constexpr bool operator>(const TestRandomAccessIterator& x, const TestRandomAccessIterator& y) {
    return x.it_ > y.it_;
  }
  friend constexpr bool operator>=(const TestRandomAccessIterator& x, const TestRandomAccessIterator& y) {
    return x.it_ >= y.it_;
  }

  friend constexpr It base(const TestRandomAccessIterator& i) {
    return i.it_;
  }

  template <typename T> void operator,(T const&) = delete;
};

template <typename It> class TestContiguousIterator {
  static_assert(frt::traits::is_pointer<It>, "Things probably break in this case");

  It it_;

  template <typename U> friend class TestContiguousIterator;

public:
  using iterator_category = frt::ContiguousIteratorTag;
  using value_type = typename frt::IteratorTraits<It>::value_type;
  using difference_type = typename frt::IteratorTraits<It>::difference_type;
  using pointer = It;
  using reference = typename frt::IteratorTraits<It>::reference;
  using element_type = frt::traits::RemovePointer<It>;

  constexpr It base() const {
    return it_;
  }

  constexpr TestContiguousIterator() : it_() {}
  constexpr explicit TestContiguousIterator(It it) : it_(it) {}
  template <typename U> constexpr TestContiguousIterator(const TestContiguousIterator<U>& u) : it_(u.it_) {}

  constexpr reference operator*() const {
    return *it_;
  }
  constexpr pointer operator->() const {
    return it_;
  }
  constexpr reference operator[](difference_type n) const {
    return it_[n];
  }

  constexpr TestContiguousIterator& operator++() {
    ++it_;
    return *this;
  }
  constexpr TestContiguousIterator& operator--() {
    --it_;
    return *this;
  }
  constexpr TestContiguousIterator operator++(int) {
    return TestContiguousIterator(it_++);
  }
  constexpr TestContiguousIterator operator--(int) {
    return TestContiguousIterator(it_--);
  }

  constexpr TestContiguousIterator& operator+=(difference_type n) {
    it_ += n;
    return *this;
  }
  constexpr TestContiguousIterator& operator-=(difference_type n) {
    it_ -= n;
    return *this;
  }
  friend constexpr TestContiguousIterator operator+(TestContiguousIterator x, difference_type n) {
    x += n;
    return x;
  }
  friend constexpr TestContiguousIterator operator+(difference_type n, TestContiguousIterator x) {
    x += n;
    return x;
  }
  friend constexpr TestContiguousIterator operator-(TestContiguousIterator x, difference_type n) {
    x -= n;
    return x;
  }
  friend constexpr difference_type operator-(TestContiguousIterator x, TestContiguousIterator y) {
    return x.it_ - y.it_;
  }

  friend constexpr bool operator==(const TestContiguousIterator& x, const TestContiguousIterator& y) {
    return x.it_ == y.it_;
  }
  friend constexpr bool operator!=(const TestContiguousIterator& x, const TestContiguousIterator& y) {
    return x.it_ != y.it_;
  }
  friend constexpr bool operator<(const TestContiguousIterator& x, const TestContiguousIterator& y) {
    return x.it_ < y.it_;
  }
  friend constexpr bool operator<=(const TestContiguousIterator& x, const TestContiguousIterator& y) {
    return x.it_ <= y.it_;
  }
  friend constexpr bool operator>(const TestContiguousIterator& x, const TestContiguousIterator& y) {
    return x.it_ > y.it_;
  }
  friend constexpr bool operator>=(const TestContiguousIterator& x, const TestContiguousIterator& y) {
    return x.it_ >= y.it_;
  }

  friend constexpr It base(const TestContiguousIterator& i) {
    return i.it_;
  }

  template <typename T> void operator,(T const&) = delete;
};

template <typename It> class TestSpaceshipContiguousIterator {
  static_assert(frt::traits::is_pointer<It>, "Things probably break in this case");

  It it_;

  template <typename U> friend class TestSpaceshipContiguousIterator;

public:
  using iterator_category = frt::ContiguousIteratorTag;
  using value_type = typename frt::IteratorTraits<It>::value_type;
  using difference_type = typename frt::IteratorTraits<It>::difference_type;
  using pointer = It;
  using reference = typename frt::IteratorTraits<It>::reference;
  using element_type = frt::traits::RemovePointer<It>;

  constexpr It base() const {
    return it_;
  }

  constexpr TestSpaceshipContiguousIterator() : it_() {}
  constexpr explicit TestSpaceshipContiguousIterator(It it) : it_(it) {}
  template <typename U>
  constexpr TestSpaceshipContiguousIterator(const TestSpaceshipContiguousIterator<U>& u) : it_(u.it_) {}

  constexpr reference operator*() const {
    return *it_;
  }
  constexpr pointer operator->() const {
    return it_;
  }
  constexpr reference operator[](difference_type n) const {
    return it_[n];
  }

  constexpr TestSpaceshipContiguousIterator& operator++() {
    ++it_;
    return *this;
  }
  constexpr TestSpaceshipContiguousIterator& operator--() {
    --it_;
    return *this;
  }
  constexpr TestSpaceshipContiguousIterator operator++(int) {
    return TestSpaceshipContiguousIterator(it_++);
  }
  constexpr TestSpaceshipContiguousIterator operator--(int) {
    return TestSpaceshipContiguousIterator(it_--);
  }

  constexpr TestSpaceshipContiguousIterator& operator+=(difference_type n) {
    it_ += n;
    return *this;
  }
  constexpr TestSpaceshipContiguousIterator& operator-=(difference_type n) {
    it_ -= n;
    return *this;
  }
  friend constexpr TestSpaceshipContiguousIterator operator+(TestSpaceshipContiguousIterator x, difference_type n) {
    x += n;
    return x;
  }
  friend constexpr TestSpaceshipContiguousIterator operator+(difference_type n, TestSpaceshipContiguousIterator x) {
    x += n;
    return x;
  }
  friend constexpr TestSpaceshipContiguousIterator operator-(TestSpaceshipContiguousIterator x, difference_type n) {
    x -= n;
    return x;
  }
  friend constexpr difference_type operator-(TestSpaceshipContiguousIterator x, TestSpaceshipContiguousIterator y) {
    return x.it_ - y.it_;
  }

  friend constexpr auto operator<=>(const TestSpaceshipContiguousIterator& x,
      const TestSpaceshipContiguousIterator& y) {
    return x.it_ <=> y.it_;
  }
  friend constexpr bool operator==(const TestSpaceshipContiguousIterator& x, const TestSpaceshipContiguousIterator& y) {
    return x.it_ == y.it_;
  }

  template <typename T> void operator,(T const&) = delete;
};

template <typename Iter> // ADL base() for everything else (including pointers)
constexpr Iter base(Iter i) {
  return i;
}

template <typename It> class Cpp20TestInputIterator {
  It it_;

public:
  using value_type = frt::IterValue<It>;
  using difference_type = frt::IterDifference<It>;
  using iterator_concept = frt::InputIteratorTag;

  constexpr explicit Cpp20TestInputIterator(It it) : it_(it) {}
  Cpp20TestInputIterator(Cpp20TestInputIterator&&) = default;
  Cpp20TestInputIterator& operator=(Cpp20TestInputIterator&&) = default;
  constexpr decltype(auto) operator*() const {
    return *it_;
  }
  constexpr Cpp20TestInputIterator& operator++() {
    ++it_;
    return *this;
  }
  constexpr void operator++(int) {
    ++it_;
  }

  friend constexpr It base(const Cpp20TestInputIterator& i) {
    return i.it_;
  }

  template <typename T> void operator,(T const&) = delete;
};

template <frt::InputOrOutputIterator> struct IterValueOrVoid { using type = void; };

template <frt::InputIterator I> struct IterValueOrVoid<I> { using type = frt::IterValue<I>; };

// Iterator adaptor that counts the number of times the iterator has had a successor/predecessor
// operation called. Has two recorders:
// * `stride_count`, which records the total number of calls to an op++, op--, op+=, or op-=.
// * `stride_displacement`, which records the displacement of the calls. This means that both
//   op++/op+= will increase the displacement counter by 1, and op--/op-= will decrease the
//   displacement counter by 1.
template <typename It> class StrideCountingIterator {
public:
  using value_type = typename IterValueOrVoid<It>::type;
  using difference_type = frt::IterDifference<It>;
  using iterator_concept = frt::traits::Conditional<frt::ContiguousIterator<It>,
      frt::ContiguousIteratorTag,
      frt::traits::Conditional<frt::RandomAccessIterator<It>,
          frt::RandomAccessIteratorTag,
          frt::traits::Conditional<frt::BidirectionalIterator<It>,
              frt::BidirectionalIteratorTag,
              frt::traits::Conditional<frt::ForwardIterator<It>,
                  frt::ForwardIteratorTag,
                  frt::traits::Conditional<frt::InputIterator<It>,
                      frt::InputIteratorTag,
                      /* else */ frt::OutputIteratorTag>>>>>;

  StrideCountingIterator() requires frt::DefaultInitializable<It>
  = default;

  constexpr explicit StrideCountingIterator(It const& it) : base_(base(it)) {}

  friend constexpr It base(StrideCountingIterator const& it) {
    return It(it.base_);
  }

  constexpr difference_type stride_count() const {
    return stride_count_;
  }

  constexpr difference_type stride_displacement() const {
    return stride_displacement_;
  }

  constexpr decltype(auto) operator*() const {
    return *It(base_);
  }

  constexpr decltype(auto) operator[](difference_type n) const {
    return It(base_)[n];
  }

  constexpr StrideCountingIterator& operator++() {
    It tmp(base_);
    base_ = base(++tmp);
    ++stride_count_;
    ++stride_displacement_;
    return *this;
  }

  constexpr void operator++(int) {
    ++*this;
  }

  constexpr StrideCountingIterator operator++(int) requires frt::ForwardIterator<It> {
    auto temp = *this;
    ++*this;
    return temp;
  }

  constexpr StrideCountingIterator& operator--() requires frt::BidirectionalIterator<It> {
    It tmp(base_);
    base_ = base(--tmp);
    ++stride_count_;
    --stride_displacement_;
    return *this;
  }

  constexpr StrideCountingIterator operator--(int) requires frt::BidirectionalIterator<It> {
    auto temp = *this;
    --*this;
    return temp;
  }

  constexpr StrideCountingIterator& operator+=(difference_type const n) requires frt::RandomAccessIterator<It> {
    It tmp(base_);
    base_ = base(tmp += n);
    ++stride_count_;
    ++stride_displacement_;
    return *this;
  }

  constexpr StrideCountingIterator& operator-=(difference_type const n) requires frt::RandomAccessIterator<It> {
    It tmp(base_);
    base_ = base(tmp -= n);
    ++stride_count_;
    --stride_displacement_;
    return *this;
  }

  friend constexpr StrideCountingIterator operator+(StrideCountingIterator it,
      difference_type n) requires frt::RandomAccessIterator<It> {
    return it += n;
  }

  friend constexpr StrideCountingIterator operator+(difference_type n,
      StrideCountingIterator it) requires frt::RandomAccessIterator<It> {
    return it += n;
  }

  friend constexpr StrideCountingIterator operator-(StrideCountingIterator it,
      difference_type n) requires frt::RandomAccessIterator<It> {
    return it -= n;
  }

  friend constexpr difference_type operator-(StrideCountingIterator const& x,
      StrideCountingIterator const& y) requires frt::SizedSentinelFor<It, It> {
    return base(x) - base(y);
  }

  constexpr bool operator==(StrideCountingIterator const& other) const requires frt::SentinelFor<It, It> {
    return It(base_) == It(other.base_);
  }

  friend constexpr bool operator<(StrideCountingIterator const& x,
      StrideCountingIterator const& y) requires frt::RandomAccessIterator<It> {
    return It(x.base_) < It(y.base_);
  }

  friend constexpr bool operator>(StrideCountingIterator const& x,
      StrideCountingIterator const& y) requires frt::RandomAccessIterator<It> {
    return It(x.base_) > It(y.base_);
  }

  friend constexpr bool operator<=(StrideCountingIterator const& x,
      StrideCountingIterator const& y) requires frt::RandomAccessIterator<It> {
    return It(x.base_) <= It(y.base_);
  }

  friend constexpr bool operator>=(StrideCountingIterator const& x,
      StrideCountingIterator const& y) requires frt::RandomAccessIterator<It> {
    return It(x.base_) >= It(y.base_);
  }

  template <typename T> void operator,(T const&) = delete;

private:
  decltype(base(frt::traits::declval<It>())) base_;
  difference_type stride_count_ = 0;
  difference_type stride_displacement_ = 0;
};

template <typename It> class SentinelWrapper {
public:
  explicit SentinelWrapper() = default;
  constexpr explicit SentinelWrapper(const It& it) : base_(base(it)) {}
  constexpr bool operator==(const It& other) const {
    return base_ == base(other);
  }
  friend constexpr It base(const SentinelWrapper& s) {
    return It(s.base_);
  }

private:
  decltype(base(frt::traits::declval<It>())) base_;
};

template <typename It> class SizedSentinel {
public:
  explicit SizedSentinel() = default;
  constexpr explicit SizedSentinel(const It& it) : base_(base(it)) {}
  constexpr bool operator==(const It& other) const {
    return base_ == base(other);
  }
  friend constexpr auto operator-(const SizedSentinel& s, const It& i) {
    return s.base_ - base(i);
  }
  friend constexpr auto operator-(const It& i, const SizedSentinel& s) {
    return base(i) - s.base_;
  }
  friend constexpr It base(const SizedSentinel& s) {
    return It(s.base_);
  }

private:
  decltype(base(frt::traits::declval<It>())) base_;
};

struct IteratorTraitsCpp17TestIterator {
  int& operator*();

  IteratorTraitsCpp17TestIterator& operator++();

  IteratorTraitsCpp17TestIterator operator++(int);
};

struct IteratorTraitsCpp17TestProxyIterator {
  int operator*();

  IteratorTraitsCpp17TestProxyIterator& operator++();

  IteratorTraitsCpp17TestIterator operator++(int);
};

struct IteratorTraitsCpp17TestInputIterator {
  using difference_type = int;
  using value_type = long;

  int& operator*();
  IteratorTraitsCpp17TestInputIterator& operator++();
  IteratorTraitsCpp17TestInputIterator operator++(int);

  bool operator==(IteratorTraitsCpp17TestInputIterator const&) const;
};

struct IteratorTraitsCpp17TestProxyInputIterator {
  using difference_type = int;
  using value_type = long;

  int operator*();
  IteratorTraitsCpp17TestProxyInputIterator& operator++();
  IteratorTraitsCpp17TestInputIterator operator++(int);
  bool operator==(IteratorTraitsCpp17TestProxyInputIterator const&) const;
};

struct IteratorTraitsCpp17TestForwardIterator {
  using difference_type = int;
  using value_type = int;

  int& operator*();
  IteratorTraitsCpp17TestForwardIterator& operator++();
  IteratorTraitsCpp17TestForwardIterator operator++(int);

  bool operator==(IteratorTraitsCpp17TestForwardIterator const&) const;
};

struct IteratorTraitsCpp17TestBidirectionalIterator {
  using difference_type = int;
  using value_type = int;

  int& operator*();
  IteratorTraitsCpp17TestBidirectionalIterator& operator++();
  IteratorTraitsCpp17TestBidirectionalIterator operator++(int);
  IteratorTraitsCpp17TestBidirectionalIterator& operator--();
  IteratorTraitsCpp17TestBidirectionalIterator operator--(int);

  bool operator==(IteratorTraitsCpp17TestBidirectionalIterator const&) const;
};

struct IteratorTraitsCpp17TestRandomAccessiterator {
  using difference_type = int;
  using value_type = int;

  int& operator*();
  int& operator[](difference_type);
  IteratorTraitsCpp17TestRandomAccessiterator& operator++();
  IteratorTraitsCpp17TestRandomAccessiterator operator++(int);
  IteratorTraitsCpp17TestRandomAccessiterator& operator--();
  IteratorTraitsCpp17TestRandomAccessiterator operator--(int);

  bool operator==(IteratorTraitsCpp17TestRandomAccessiterator const&) const;
  bool operator<(IteratorTraitsCpp17TestRandomAccessiterator const&) const;
  bool operator>(IteratorTraitsCpp17TestRandomAccessiterator const&) const;
  bool operator<=(IteratorTraitsCpp17TestRandomAccessiterator const&) const;
  bool operator>=(IteratorTraitsCpp17TestRandomAccessiterator const&) const;

  IteratorTraitsCpp17TestRandomAccessiterator& operator+=(difference_type);
  IteratorTraitsCpp17TestRandomAccessiterator& operator-=(difference_type);

  friend IteratorTraitsCpp17TestRandomAccessiterator operator+(IteratorTraitsCpp17TestRandomAccessiterator,
      difference_type);
  friend IteratorTraitsCpp17TestRandomAccessiterator operator+(difference_type,
      IteratorTraitsCpp17TestRandomAccessiterator);
  friend IteratorTraitsCpp17TestRandomAccessiterator operator-(IteratorTraitsCpp17TestRandomAccessiterator,
      difference_type);
  friend difference_type operator-(IteratorTraitsCpp17TestRandomAccessiterator,
      IteratorTraitsCpp17TestRandomAccessiterator);
};

#define CHECK_SAME_TYPE(type, expr) static_assert(::frt::SameAs<type, decltype(expr)>)