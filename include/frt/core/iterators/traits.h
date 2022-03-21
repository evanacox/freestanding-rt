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

#include "../../types/basic.h"
#include "../../types/traits.h"
#include "./tags.h"

namespace frt {
  template <typename I> struct IncrementableTraits;

  namespace internal {
    template <typename T>
    concept Referenceable = requires(T& a) {
      a;
    };

    template <typename T>
    concept Dereferenceable = Referenceable<T> && requires(T& a) {
      *a;
    };

    template <class I>
    concept LegacyIterator = Copyable<I> && requires(I i) {
      { *i } -> Referenceable;
      { ++i } -> SameAs<I&>;
      { *i++ } -> Referenceable;
    };

    template <typename T>
    concept HasNoDiffType = (!requires { typename T::difference_type; }) && requires(const T& a, const T& b) {
      { a - b } -> Integral;
    };

    // incrementable_traits
    template <typename> struct IncrementableTraitsImpl {};

    template <typename T>
    requires traits::is_object<T>
    struct IncrementableTraitsImpl<T*> {
      using difference_type = frt::isize;
    };

    template <typename T> struct IncrementableTraitsImpl<const T> : IncrementableTraitsImpl<T> {};

    template <typename T>
    requires requires {
      typename T::difference_type;
    }
    struct IncrementableTraitsImpl<T> {
      using difference_type = typename T::difference_type;
    };

    template <HasNoDiffType T> struct IncrementableTraitsImpl<T> {
      using difference_type = traits::MakeSigned<decltype(traits::declval<T>() - traits::declval<T>())>;
    };
  } // namespace internal

  template <typename T> struct IncrementableTraits : internal::IncrementableTraitsImpl<T> {};

  namespace internal {
    template <typename T> struct IndirectlyReadableTraitsPointerImpl {};

    template <typename T>
    requires traits::is_object<T>
    struct IndirectlyReadableTraitsPointerImpl<T> {
      using value_type = traits::RemoveCV<T>;
    };

    template <typename I> struct IndirectlyReadableTraitsImpl {};

    template <typename I> struct IndirectlyReadableTraitsImpl<I*> : IndirectlyReadableTraitsPointerImpl<I> {};

    template <typename I>
    requires traits::is_array<I>
    struct IndirectlyReadableTraitsImpl<I> {
      using value_type = traits::RemoveCV<traits::RemoveExtent<I>>;
    };

    template <typename I> struct IndirectlyReadableTraitsImpl<const I> : IndirectlyReadableTraitsImpl<I> {};

    template <typename I>
    requires requires {
      typename I::value_type;
    }
    struct IndirectlyReadableTraitsImpl<I> {
      using value_type = traits::RemoveCV<typename I::value_type>;
    };

    template <typename I>
    requires requires {
      typename I::element_type;
    }
    struct IndirectlyReadableTraitsImpl<I> {
      using element_type = traits::RemoveCV<typename I::element_type>;
    };
  } // namespace internal

  template <typename I> struct IndirectlyReadableTraits : internal::IndirectlyReadableTraitsImpl<I> {};

  namespace internal {
    template <typename I> struct IteratorTraitsImpl;
  }

  template <typename I> struct IteratorTraits : internal::IteratorTraitsImpl<I> {
    using __frt_iterator_traits_primary = void;
  };

  template <typename T>
  requires(traits::is_object<T>) struct IteratorTraits<T*> {
    using difference_type = frt::isize;
    using value_type = T;
    using pointer = T*;
    using reference = T&;
    using iterator_category = frt::RandomAccessIteratorTag;
    using iterator_concept = frt::ContiguousIteratorTag;
    using __frt_iterator_traits_primary = void;
  };

  namespace internal {
    template <typename I>
    concept IsFromPrimary = requires {
      typename IteratorTraits<I>::__frt_iterator_traits_primary;
    };

    template <typename I> struct IterConceptTraits { using type = IteratorTraits<I>; };

    template <IsFromPrimary I> struct IterConceptTraits<I> { using type = I; };

    template <typename I> struct IterConcept2 {
      using type = traits::EnableIf<IsFromPrimary<I>, frt::RandomAccessIteratorTag>;
    };

    template <typename I>
    requires requires {
      typename IterConceptTraits<I>::iterator_category;
    }
    struct IterConcept2<I> {
      using type = typename IterConceptTraits<I>::iterator_category;
    };

    template <typename I> struct IterConcept1 : IterConcept2<I> {};

    template <typename I>
    requires requires {
      typename IterConceptTraits<I>::iterator_concept;
    }
    struct IterConcept1<I> {
      using type = typename IterConceptTraits<I>::iterator_concept;
    };

    template <typename I> using IterConcept = typename IterConcept1<I>::type;
  } // namespace internal

  template <typename I>
  using IterValue = traits::Conditional<internal::IsFromPrimary<traits::RemoveCVRef<I>>,
      typename IndirectlyReadableTraits<traits::RemoveCVRef<I>>::value_type,
      typename IteratorTraits<traits::RemoveCVRef<I>>::value_type>;

  template <internal::Dereferenceable I> using IterReference = decltype(*traits::declval<I&>());

  template <typename I>
  using IterDifference = traits::Conditional<internal::IsFromPrimary<traits::RemoveCVRef<I>>,
      typename IncrementableTraits<traits::RemoveCVRef<I>>::difference_type,
      typename IteratorTraits<traits::RemoveCVRef<I>>::difference_type>;

  template <internal::Dereferenceable I>
  using IterRvalueReference = typename IteratorTraits<traits::RemoveCVRef<I>>::value_type;

  template <typename I>
  concept IndirectlyReadable =
      CommonReferenceWith<IterReference<I> &&, IterValue<I>&>&& CommonReferenceWith<IterReference<I>&&,
          IterRvalueReference<I>&&>&& CommonReferenceWith<IterRvalueReference<I>&&,
          const IterValue<I>&>&& requires(const I in) {
    typename IterValue<I>;
    typename IterReference<I>;
    typename IterRvalueReference<I>;
    { *in } -> SameAs<IterReference<I>>;
  };

  template <IndirectlyReadable I> using IterCommonReference = traits::CommonReference<IterReference<I>, IterValue<I>&>;

  namespace internal {
    template <typename I>
    concept LegacyInputIterator = LegacyIterator<I> && EqualityComparable<I> && requires(I i) {
      typename IncrementableTraits<I>::difference_type;
      typename IndirectlyReadableTraits<I>::value_type;
      typename traits::CommonReference<IterReference<I>&&, typename IndirectlyReadableTraits<I>::value_type&>;
      *i++;
      typename traits::CommonReference<decltype(*i++)&&, typename IndirectlyReadableTraits<I>::value_type&>;
      requires SignedIntegral<typename IncrementableTraits<I>::difference_type>;
    };

    template <typename I>
    concept LegacyForwardIterator =
        LegacyInputIterator<I> && ConstructibleFrom<I> && traits::is_lvalue_reference<IterReference<I>> && SameAs
        < traits::RemoveCVRef<IterReference<I>>,
    typename IndirectlyReadableTraits<I>::value_type > &&requires(I i) {
      { i++ } -> ConvertibleTo<const I&>;
      { *i++ } -> SameAs<IterReference<I>>;
    };

    template <typename I>
    concept LegacyBidirectionalIterator = LegacyForwardIterator<I> && requires(I i) {
      { --i } -> SameAs<I&>;
      { i-- } -> ConvertibleTo<const I&>;
      { *i-- } -> SameAs<IterReference<I>>;
    };

    template <typename I>
    concept LegacyRandomAccessIterator = LegacyBidirectionalIterator<I> && TotallyOrdered<I> && requires(I i,
        typename IncrementableTraits<I>::difference_type n) {
      { i += n } -> SameAs<I&>;
      { i -= n } -> SameAs<I&>;
      { i + n } -> SameAs<I>;
      { n + i } -> SameAs<I>;
      { i - n } -> SameAs<I>;
      { i - i } -> SameAs<decltype(n)>;
      { i[n] } -> ConvertibleTo<IterReference<I>>;
    };

    template <typename Iter>
    concept HasAllExceptPointer = requires {
      typename Iter::difference_type;
      typename Iter::value_type;
      typename Iter::reference;
      typename Iter::iterator_category;
    };

    template <typename Iter>
    concept HasPointer = requires {
      typename Iter::pointer;
    };

    template <typename I>
    concept HasBoth = HasAllExceptPointer<I> && HasPointer<I>;

    template <typename Iter> struct IteratorTraitsImpl {};

    template <typename Iter> struct IteratorTraitsPointer2 { using type = void; };

    template <typename Iter>
    requires requires(Iter& i) {
      i.operator->();
    }
    struct IteratorTraitsPointer2<Iter> {
      using type = decltype(traits::declval<Iter&>().operator->());
    };

    template <typename Iter> struct IteratorTraitsPointer : IteratorTraitsPointer2<Iter> {};

    template <typename Iter>
    requires requires {
      typename Iter::pointer;
    }
    struct IteratorTraitsPointer<Iter> {
      using type = typename Iter::pointer;
    };

    template <typename Iter> struct IteratorTraitsReference { using type = IterReference<Iter>; };

    template <typename Iter>
    requires requires {
      typename Iter::reference;
    }
    struct IteratorTraitsReference<Iter> {
      using type = typename Iter::reference;
    };

    template <typename Iter> struct IteratorTraitsCategory2 { using type = InputTag; };

    template <LegacyForwardIterator Iter> struct IteratorTraitsCategory2<Iter> { using type = ForwardTag; };

    template <LegacyBidirectionalIterator Iter> struct IteratorTraitsCategory2<Iter> { using type = BidirectionTag; };

    template <LegacyRandomAccessIterator Iter> struct IteratorTraitsCategory2<Iter> { using type = RandomTag; };

    template <typename Iter> struct IteratorTraitsCategory : IteratorTraitsCategory2<Iter> {};

    template <typename Iter>
    requires requires {
      typename Iter::iterator_category;
    }
    struct IteratorTraitsCategory<Iter> {
      using type = typename Iter::iterator_category;
    };

    template <HasBoth Iter> struct IteratorTraitsImpl<Iter> {
      using difference_type = typename Iter::difference_type;
      using value_type = typename Iter::value_type;
      using pointer = typename Iter::pointer;
      using reference = typename Iter::reference;
      using iterator_category = typename Iter::iterator_category;
    };

    template <HasAllExceptPointer Iter> struct IteratorTraitsImpl<Iter> {
      using difference_type = typename Iter::difference_type;
      using value_type = typename Iter::value_type;
      using pointer = void;
      using reference = typename Iter::reference;
      using iterator_category = typename Iter::iterator_category;
    };

    template <typename Iter>
    requires(!HasAllExceptPointer<Iter> && LegacyInputIterator<Iter>) struct IteratorTraitsImpl<Iter> {
      using difference_type = typename IncrementableTraits<Iter>::difference_type;
      using value_type = typename IndirectlyReadableTraits<Iter>::value_type;
      using pointer = typename IteratorTraitsPointer<Iter>::type;
      using reference = typename IteratorTraitsReference<Iter>::type;
      using iterator_category = typename IteratorTraitsCategory<Iter>::type;
    };

    template <typename Iter> struct IncrementableDiffType { using type = void; };

    template <typename Iter>
    requires requires {
      typename IncrementableTraits<Iter>::difference_type;
    }
    struct IncrementableDiffType<Iter> {
      using type = typename IncrementableTraits<Iter>::difference_type;
    };

    template <typename Iter>
    requires(!HasAllExceptPointer<
                 Iter> && !LegacyInputIterator<Iter> && LegacyIterator<Iter>) struct IteratorTraitsImpl<Iter> {
      using difference_type = typename IncrementableDiffType<Iter>::type;
      using value_type = void;
      using pointer = void;
      using reference = void;
      using iterator_category = OutputTag;
    };
  } // namespace internal
} // namespace frt