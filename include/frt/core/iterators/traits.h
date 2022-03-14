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

    template <typename Iter>
    requires HasBoth<Iter>
    struct IteratorTraitsImpl<Iter> {
      using difference_type = typename Iter::difference_type;
      using value_type = typename Iter::value_type;
      using pointer = typename Iter::pointer;
      using reference = typename Iter::reference;
      using iterator_category = typename Iter::iterator_category;
      using __frt_iterator_traits_primary = void;
    };

    template <typename Iter>
    requires HasAllExceptPointer<Iter>
    struct IteratorTraitsImpl<Iter> {
      using difference_type = typename Iter::difference_type;
      using value_type = typename Iter::value_type;
      using pointer = void;
      using reference = typename Iter::reference;
      using iterator_category = typename Iter::iterator_category;
      using __frt_iterator_traits_primary = void;
    };

    template <typename T>
    requires traits::is_object<T>
    struct IteratorTraitsImpl<T*> {
      using difference_type = frt::isize;
      using value_type = traits::RemoveCV<T>;
      using pointer = T*;
      using reference = T&;
      using iterator_category = RandomTag;
      using iterator_concept = ContiguousTag;
      using __frt_iterator_traits_primary = void;
    };

    template <typename T> struct IncremDiffType { using type = void; };

    template <typename T>
    requires requires {
      typename IncrementableTraits<T>::difference_type;
    }
    struct IncremDiffType<T> {
      using type = typename IncrementableTraits<T>::difference_type;
    };

    template <typename T>
    requires LegacyIterator<T>
    struct IteratorTraitsImpl<T> {
      using difference_type = typename IncremDiffType<T>::type;
      using value_type = void;
      using pointer = void;
      using reference = void;
      using iterator_category = OutputTag;
    };
  } // namespace internal

  template <typename Iter> struct IteratorTraits : internal::IteratorTraitsImpl<Iter> {};

  namespace internal {
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

    template <typename T>
    requires HasNoDiffType<T>
    struct IncrementableTraitsImpl<T> {
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
} // namespace frt