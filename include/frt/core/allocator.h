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

#include "../types/basic.h"
#include "../types/concepts.h"
#include "../types/traits.h"
#include "./pointers.h"

namespace frt {
  namespace internal {
    template <typename A>
    concept HasValueType = requires {
      typename A::value_type;
    };

    template <typename A, typename = void> struct AllocatorPointer { using type = typename A::value_type*; };

    template <typename A> struct AllocatorPointer<A, traits::Void<typename A::pointer>> {
      using type = typename A::pointer;
    };

    template <HasValueType A> using AllocPtr = typename AllocatorPointer<A>::type;

    template <typename A, typename = void> struct AllocatorConstPointer {
      using T = typename A::value_type;

      using type = typename PointerTraits<AllocPtr<A>>::template rebind<const T>;
    };

    template <typename A> struct AllocatorConstPointer<A, traits::Void<typename A::const_pointer>> {
      using type = typename A::const_pointer;
    };

    template <HasValueType A> using AllocConstPtr = typename AllocatorConstPointer<A>::type;

    template <typename A, typename = void> struct AllocatorVoidPointer {
      using type = typename PointerTraits<AllocPtr<A>>::template rebind<void>;
    };

    template <typename A> struct AllocatorVoidPointer<A, traits::Void<typename A::void_pointer>> {
      using type = typename A::void_pointer;
    };

    template <HasValueType A> using AllocVoidPtr = typename AllocatorVoidPointer<A>::type;

    template <typename A, typename = void> struct AllocatorConstVoidPointer {
      using type = typename PointerTraits<AllocPtr<A>>::template rebind<const void>;
    };

    template <typename A> struct AllocatorConstVoidPointer<A, traits::Void<typename A::const_void_pointer>> {
      using type = typename A::const_pointer;
    };

    template <HasValueType A> using AllocConstVoidPtr = typename AllocatorConstVoidPointer<A>::type;

    template <typename A, typename = void> struct AllocatorDiffType {
      using type = typename PointerTraits<AllocPtr<A>>::difference_type;
    };

    template <typename A> struct AllocatorDiffType<A, traits::Void<typename A::difference_type>> {
      using type = typename A::difference_type;
    };

    template <HasValueType A> using AllocDiff = typename AllocatorDiffType<A>::type;

    template <typename A, typename = void> struct AllocatorSizeType {
      using type = traits::MakeUnsigned<AllocDiff<A>>;
    };

    template <typename A> struct AllocatorSizeType<A, traits::Void<typename A::size_type>> {
      using type = typename A::size_type;
    };

    template <HasValueType A> using AllocSize = typename AllocatorSizeType<A>::type;

    template <typename T> struct FirstTemplateArg;

    template <template <typename...> typename A, typename T, typename... Args> struct FirstTemplateArg<A<T, Args...>> {
      using type = T;
    };

    template <typename U, typename A, typename = void> struct AllocatorRebind;

    template <typename U, typename A>
    struct AllocatorRebind<U, A, traits::Void<typename A::template rebind<U>::other>> {
      using type = typename A::template rebind<U>::other;
    };

    template <typename U, template <typename, typename...> typename A, typename T, typename... Args>
    struct AllocatorRebind<U, A<T, Args...>, void> {
      using type = A<U, Args...>;
    };

    template <HasValueType A, typename U> using AllocRebind = typename AllocatorRebind<U, A>::type;

    template <HasValueType A> using AllocValue = typename A::value_type;

    struct EmptyTestTypeT {};
    struct EmptyTestTypeU {};
    struct EmptyTestTypeC {};

    template <typename A>
    concept AllocatorPointerOps = traits::is_convertible<AllocPtr<A>, AllocConstPtr<A>> && requires(AllocPtr<A> p,
        AllocConstPtr<A> q,
        AllocValue<A>& r,
        AllocVoidPtr<A> w,
        AllocConstVoidPtr<A> x) {
      { *p } -> SameAs<AllocValue<A>&>;
      { *q } -> SameAs<traits::AddConst<AllocValue<A>>&>;
      { static_cast<AllocPtr<A>>(w) } -> SameAs<AllocPtr<A>>;
      { static_cast<AllocConstPtr<A>>(x) } -> SameAs<AllocConstPtr<A>>;
      { PointerTraits<AllocPtr<A>>::pointer_to(r) } -> SameAs<AllocPtr<A>>;
    };

    template <typename A>
    concept AllocatorStorage = requires(A a, AllocPtr<A> p, AllocSize<A> n) {
      { a.allocate(n) } -> SameAs<AllocPtr<A>>;
      // clang-format off
      { a.deallocate(p, n) } noexcept;
      // clang-format on
    };
  } // namespace internal

  /// A concept that models allocators for the FRT library. However, these are not
  /// the same as the `Cpp17Allocator` requirements in the standard!
  ///
  /// There is one key difference: Allocators are **value-based**. This makes it possible
  /// to make this like stack allocators and have the container still be able to be
  /// moved around and whatnot.
  template <typename A>
  concept Allocator = internal::HasValueType<A>                        //
      && internal::AllocatorPointerOps<A>                              //
      && internal::AllocatorStorage<A>                                 //
      && Regular<A>                                                    //
      && SameAs<typename A::value_type, internal::FirstTemplateArg<A>> //
      && requires(A a, A a1, A a2) {
    typename internal::AllocPtr<A>;
    typename internal::AllocConstPtr<A>;
    typename internal::AllocVoidPtr<A>;
    typename internal::AllocConstVoidPtr<A>;
    typename internal::AllocSize<A>;
    typename internal::AllocRebind<A, internal::EmptyTestTypeU>;
  };
} // namespace frt