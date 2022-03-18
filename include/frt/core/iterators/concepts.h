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

#include "../../platform/compare.h"
#include "../../types/concepts.h"
#include "../../types/forward.h"
#include "../pointers.h"
#include "./tags.h"
#include "./traits.h"

namespace frt {

  namespace internal {
    /// Checks that the user of the interface has provided a valid iterator tag
    /// \tparam T
    template <typename T>
    concept IteratorTag = OneOf<T, InputTag, ForwardTag, BidirectionTag, RandomTag, ContiguousTag, OutputTag>;

    /// Checks that `T` isn't doing anything weird for a `Derived` type
    ///
    /// \tparam T The `Derived` type to check
    template <typename T>
    concept SaneDerived = traits::is_class<T> && SameAs<T, traits::RemoveCVRef<T>>;

    /// Equivalent to the `__Referenceable` exposition-only concept. Checks if
    /// an iterator is able to be dereferenced
    template <typename T>
    concept __Referenceable = requires(T& t) {
      { *t } -> NonVoid;
    };

    template <typename T>
    concept HasEq = requires(const T& one, const T& two) {
      { one.eq(two) } -> SameAs<bool>;
    };

    template <typename T>
    concept HasCmp = requires(const T& one, const T& two) {
      { one.cmp(two) } -> SameAs<StrongOrder>;
    };

    template <typename T>
    concept HasRequired = Movable<T> &&(HasEq<T> || HasCmp<T>);

    template <typename T, typename DiffType>
    concept DistanceAndMove = requires(T iter, const T citer, DiffType n) {
      // clang-format off
    { citer.distance_to(citer) } noexcept -> SameAs<DiffType>;
      // clang-format on
      iter.move(n);
    };

    /// Checks if `T` is a valid `Derived` type to form a contiguous iterator
    ///
    /// \tparam T The `Derived` type to check
    template <typename T, typename Pointer, typename DiffType>
    concept ContiguousIteratorReady = DistanceAndMove<T, DiffType> && HasRequired<T> && requires(const T citer,
        DiffType n) {
      { citer.address() } -> SameAs<Pointer>;
    };

    /// Checks if `T` is a valid `Derived` type to form a random-access iterator
    ///
    /// \tparam T The `Derived` type to check
    template <typename T, typename Reference, typename Pointer, typename DiffType>
    concept RandomAccessIteratorReady =
        // contiguous iterators may not have `value()` but they still turn into valid `random_access_iterator`s
        // so we need to check if it's contiguous first
        // clang-format off
      ContiguousIteratorReady<T, Pointer, DiffType> || (DistanceAndMove<T, DiffType> && HasRequired<T> && requires(const T ci,
          DiffType n) {
        { ci.value() } -> SameAs<Reference>;
      }
    );
    // clang-format on

    /// Checks if `T` is a valid `Derived` type to form an input iterator
    ///
    /// \tparam T The `Derived` type to check
    template <typename T, typename Reference, typename Pointer, typename DiffType>
    concept InputIteratorReady =
        // If T is a random access iterator, it may not have .next(), so we need to check that first
        // clang-format off
      RandomAccessIteratorReady<T, Reference, Pointer, DiffType> || (HasRequired<T> && requires(T iter, const T ci) {
        { ci.value() } -> SameAs<Reference>;
        iter.next();
      }
    );
    // clang-format on

    /// Checks if `T` is a valid `Derived` type to form a forward iterator
    ///
    /// \tparam T The `Derived` type to check
    template <typename T, typename Reference, typename Pointer, typename DiffType>
    concept ForwardIteratorReady = InputIteratorReady<T, Reference, Pointer, DiffType> && Regular<T>;

    /// Checks if `T` is a valid `Derived` type to form a bidirectional iterator
    ///
    /// \tparam T The `Derived` type to check
    template <typename T, typename Reference, typename Pointer, typename DiffType>
    // clang-format off
    concept BidirectionalIteratorReady = (RandomAccessIteratorReady<T, Reference, Pointer, DiffType>) || (
      // If T is a random access iterator, it may not have .next(), so we need to check that first
      ForwardIteratorReady<T, Reference, Pointer, DiffType> && requires(T iter) {
        iter.prev();
      }
    );
    // clang-format on

    /// Checks if `T` is a valid `Derived` type to form an output iterator
    ///
    /// \tparam T The type to check
    /// \tparam Reference The reference type for `T`
    template <typename T, typename Reference, typename Pointer, typename DiffType>
    concept OutputIteratorReady = InputIteratorReady<T, Reference, Pointer, DiffType>;
  } // namespace internal

  template <typename Out, typename T>
  concept IndirectlyWritable = requires(Out&& o, T&& t) {
    *o = frt::forward<T>(t);
    *frt::forward<Out>(o) = frt::forward<T>(t);
    const_cast<const IterReference<Out>&&>(*o) = frt::forward<T>(t);
    const_cast<const IterReference<Out>&&>(*frt::forward<Out>(o)) = frt::forward<T>(t);
  };

  template <class I>
  concept WeaklyIncrementable = DefaultInitializable<I> && Movable<I> && requires(I i) {
    typename IterDifference<I>;
    requires traits::is_signed<IterDifference<I>>;
    { ++i } -> SameAs<I&>;
    i++;
  };

  template <class I>
  concept Incrementable = Regular<I> && WeaklyIncrementable<I> && requires(I i) {
    { i++ } -> SameAs<I>;
  };

  template <typename I>
  concept InputOrOutputIterator = WeaklyIncrementable<I> && requires(I i) {
    { *i } -> internal::Referenceable;
  };

  template <typename S, typename I>
  concept SentinelFor = SemiRegular<S> && InputOrOutputIterator<I> && __WeaklyEqualityComparableWith<S, I>;

  // customization point
  template <typename S, typename I> constexpr bool disable_sized_sentinel_for = false;

  template <typename S, typename I>
  concept SizedSentinelFor = SentinelFor<S, I> && !disable_sized_sentinel_for<traits::RemoveCV<S>,
                                 traits::RemoveCV<I>> && requires(const I& i, const S& s) {
    { s - i } -> SameAs<IterDifference<I>>;
    { i - s } -> SameAs<IterDifference<I>>;
  };

  // clang-format off
  template <typename I>
  concept InputIterator = InputOrOutputIterator<I>
      && IndirectlyReadable<I>
      && DerivedFrom<internal::IterConcept<I>, InputIteratorTag>
      && requires {
    typename internal::IterConcept<I>;
  };

  template <typename I, typename T>
  concept OutputIterator = InputOrOutputIterator<I>
      && IndirectlyWritable<I, T>
      && requires(I i, T&& t) {
    *i++ = frt::forward<T>(t); // not required to be equality-preserving
  };

  template <typename I>
  concept ForwardIterator = InputIterator<I>
      && DerivedFrom<internal::IterConcept<I>, ForwardIteratorTag>
      && Incrementable<I>
      && SentinelFor<I, I>;

  template <typename I>
  concept BidirectionalIterator = ForwardIterator<I>
      && DerivedFrom<internal::IterConcept<I>, BidirectionalIteratorTag>
      && requires(I i) {
    { --i } -> SameAs<I&>;
    { i-- } -> SameAs<I>;
  };

  template <typename I>
  concept RandomAccessIterator = BidirectionalIterator<I>
      && DerivedFrom<internal::IterConcept<I>, RandomIteratorTag>
      && TotallyOrdered<I>
      && SizedSentinelFor<I, I>
      && requires(I i, const I j, const IterDifference<I> n) {
    { i += n } -> SameAs<I&>;
    { j + n } -> SameAs<I>;
    { n + j } -> SameAs<I>;
    { i -= n } -> SameAs<I&>;
    { j - n } -> SameAs<I>;
    { j[n] } -> SameAs<IterReference<I>>;
  };

  template <typename I>
  concept ContiguousIterator = RandomAccessIterator<I>
      && DerivedFrom<internal::IterConcept<I>, ContiguousIteratorTag>
      && traits::is_lvalue_reference<IterReference<I>>
      && SameAs<IterValue<I>, traits::RemoveCVRef<IterReference<I>>>
      && requires(const I& i) {
    { frt::to_address(i) } -> SameAs<traits::AddPointer<IterReference<I>>>;
  };

  template <typename F, typename I>
  concept IndirectlyUnaryInvocable = IndirectlyReadable<I>
      && CopyConstructible<F>
      && Invocable<F&, IterValue<I>&>
      && Invocable<F&, IterReference<I>>
      && Invocable<F&, IterCommonReference<I>>
      && CommonReferenceWith<traits::InvokeResult<F&, IterValue<I>&>, traits::InvokeResult<F&, IterReference<I>>>;

  template <typename F, typename I>
  concept IndirectlyRegularUnaryInvocable = IndirectlyReadable<I>
      && CopyConstructible<F>
      && RegularInvocable<F&, IterValue<I>&>
      && RegularInvocable<F&, IterReference<I>>
      && RegularInvocable<F&, IterCommonReference<I>>
      && CommonReferenceWith<traits::InvokeResult<F&, IterValue<I>&>, traits::InvokeResult<F&, IterReference<I>>>;

  template <typename F, typename I>
  concept IndirectUnaryPredicate = IndirectlyReadable<I>
      && CopyConstructible<F>
      && Predicate<F&, IterValue<I>&>
      && Predicate<F&, IterReference<I>>
      && Predicate<F&, IterCommonReference<I>>;

  template <typename F, typename I1, typename I2>
  concept IndirectBinaryPredicate = IndirectlyReadable<I1>
      && IndirectlyReadable<I2>
      && CopyConstructible<F>
      && Predicate<F&, IterValue<I1>&, IterValue<I2>&>
      && Predicate<F&, IterValue<I1>&, IterReference<I2>>
      && Predicate<F&, IterReference<I1>, IterValue<I2>&>
      && Predicate<F&, IterReference<I1>, IterReference<I2>>
      && Predicate<F&, IterCommonReference<I1>, IterCommonReference<I2>>;

  template <typename F, typename I1, typename I2 = I1>
  concept IndirectEquivalenceRelation = IndirectlyReadable<I1>
      && IndirectlyReadable<I2>
      && CopyConstructible<F>
      && EquivalenceRelation<F&, IterValue<I1>&, IterValue<I2>&>
      && EquivalenceRelation<F&, IterValue<I1>&, IterReference<I2>>
      && EquivalenceRelation<F&, IterReference<I1>, IterValue<I2>&>
      && EquivalenceRelation<F&, IterReference<I1>, IterReference<I2>>
      && EquivalenceRelation<F&, IterCommonReference<I1>, IterCommonReference<I2>>;

  template <typename In, typename Out>
  concept IndirectlyMovable = IndirectlyReadable<In> && IndirectlyWritable<Out, IterRvalueReference<In>>;

  template <typename In, typename Out>
  concept IndirectlyMovableStorable = IndirectlyMovable<In, Out>
      && IndirectlyWritable<Out, IterValue<In>>
      && Movable<IterValue<In>>
      && ConstructibleFrom<IterValue<In>, IterRvalueReference<In>>
      && AssignableFrom<IterValue<In>&, IterRvalueReference<In>>;

  template <typename In, typename Out>
  concept IndirectlyCopyable = IndirectlyReadable<In> && IndirectlyWritable<Out, IterReference<In>>;

  template <typename In, typename Out>
  concept IndirectlyCopyableStorable = IndirectlyCopyable<In, Out>
      && IndirectlyWritable <Out, IterValue<In>&>
      && IndirectlyWritable<Out, const IterValue<In>&>&& IndirectlyWritable<Out, IterValue<In>&&>
      && IndirectlyWritable<Out, const IterValue<In>&&>&& Copyable<IterValue<In>>
      && ConstructibleFrom<IterValue<In>, IterReference<In>>
      && AssignableFrom<IterValue<In>&, IterReference<In>>;
  // clang-format on
} // namespace frt