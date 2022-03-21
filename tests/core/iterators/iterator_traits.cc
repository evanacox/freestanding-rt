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
 * Note: Most of this test is adapted from the libc++ test suite, therefore it is
 * also under the LLVM copyright. See the license header below for details:
 */

//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "frt/core/iterators.h"
#include "test_utilities.h"

template <typename Traits> constexpr bool has_iterator_concept = requires {
  typename Traits::iterator_concept;
};

struct AllMembers {
  struct iterator_category {};
  struct value_type {};
  struct difference_type {};
  struct reference {};
  struct pointer {};
};
using AllMembersTraits = frt::IteratorTraits<AllMembers>;
static_assert(frt::SameAs<AllMembersTraits::iterator_category, AllMembers::iterator_category>);
static_assert(frt::SameAs<AllMembersTraits::value_type, AllMembers::value_type>);
static_assert(frt::SameAs<AllMembersTraits::difference_type, AllMembers::difference_type>);
static_assert(frt::SameAs<AllMembersTraits::reference, AllMembers::reference>);
static_assert(frt::SameAs<AllMembersTraits::pointer, AllMembers::pointer>);
static_assert(!has_iterator_concept<AllMembersTraits>);

struct NoPointerMember {
  struct iterator_category {};
  struct value_type {};
  struct difference_type {};
  struct reference {};
  // ignored, because NoPointerMember is not a LegacyInputIterator:
  value_type* operator->() const;
};
using NoPointerMemberTraits = frt::IteratorTraits<NoPointerMember>;
static_assert(frt::SameAs<NoPointerMemberTraits::iterator_category, NoPointerMember::iterator_category>);
static_assert(frt::SameAs<NoPointerMemberTraits::value_type, NoPointerMember::value_type>);
static_assert(frt::SameAs<NoPointerMemberTraits::difference_type, NoPointerMember::difference_type>);
static_assert(frt::SameAs<NoPointerMemberTraits::reference, NoPointerMember::reference>);
static_assert(frt::SameAs<NoPointerMemberTraits::pointer, void>);
static_assert(!has_iterator_concept<NoPointerMemberTraits>);

struct IterConcept {
  struct iterator_category {};
  struct value_type {};
  struct difference_type {};
  struct reference {};
  struct pointer {};
  // iterator_traits does NOT pass through the iterator_concept of the type itself.
  struct iterator_concept {};
};
using IterConceptTraits = frt::IteratorTraits<IterConcept>;
static_assert(frt::SameAs<IterConceptTraits::iterator_category, IterConcept::iterator_category>);
static_assert(frt::SameAs<IterConceptTraits::value_type, IterConcept::value_type>);
static_assert(frt::SameAs<IterConceptTraits::difference_type, IterConcept::difference_type>);
static_assert(frt::SameAs<IterConceptTraits::reference, IterConcept::reference>);
static_assert(frt::SameAs<IterConceptTraits::pointer, IterConcept::pointer>);
static_assert(!has_iterator_concept<IterConceptTraits>);

struct LegacyInput {
  struct iterator_category {};
  struct value_type {};
  struct reference {
    operator value_type() const;
  };

  friend bool operator==(LegacyInput, LegacyInput);
  reference operator*() const;
  LegacyInput& operator++();
  LegacyInput operator++(int);
};
template <> struct frt::IncrementableTraits<LegacyInput> { using difference_type = short; };
using LegacyInputTraits = frt::IteratorTraits<LegacyInput>;
static_assert(frt::SameAs<LegacyInputTraits::iterator_category, LegacyInput::iterator_category>);
static_assert(frt::SameAs<LegacyInputTraits::value_type, LegacyInput::value_type>);
static_assert(frt::SameAs<LegacyInputTraits::difference_type, short>);
static_assert(frt::SameAs<LegacyInputTraits::reference, LegacyInput::reference>);
static_assert(frt::SameAs<LegacyInputTraits::pointer, void>);
static_assert(!has_iterator_concept<LegacyInputTraits>);

struct LegacyInputNoValueType {
  struct not_value_type {};
  using difference_type = int; // or any signed integral type
  struct reference {
    operator not_value_type&() const;
  };

  friend bool operator==(LegacyInputNoValueType, LegacyInputNoValueType);
  reference operator*() const;
  LegacyInputNoValueType& operator++();
  LegacyInputNoValueType operator++(int);
};
template <> struct frt::IndirectlyReadableTraits<LegacyInputNoValueType> {
  using value_type = LegacyInputNoValueType::not_value_type;
};
using LegacyInputNoValueTypeTraits = frt::IteratorTraits<LegacyInputNoValueType>;
static_assert(frt::SameAs<LegacyInputNoValueTypeTraits::iterator_category, frt::InputIteratorTag>);
static_assert(frt::SameAs<LegacyInputNoValueTypeTraits::value_type, LegacyInputNoValueType::not_value_type>);
static_assert(frt::SameAs<LegacyInputNoValueTypeTraits::difference_type, int>);
static_assert(frt::SameAs<LegacyInputNoValueTypeTraits::reference, LegacyInputNoValueType::reference>);
static_assert(frt::SameAs<LegacyInputNoValueTypeTraits::pointer, void>);
static_assert(!has_iterator_concept<LegacyInputNoValueTypeTraits>);

struct LegacyForward {
  struct not_value_type {};

  friend bool operator==(LegacyForward, LegacyForward);
  const not_value_type& operator*() const;
  LegacyForward& operator++();
  LegacyForward operator++(int);
};
template <> struct frt::IndirectlyReadableTraits<LegacyForward> { using value_type = LegacyForward::not_value_type; };
template <> struct frt::IncrementableTraits<LegacyForward> {
  using difference_type = short; // or any signed integral type
};
using LegacyForwardTraits = frt::IteratorTraits<LegacyForward>;
static_assert(frt::SameAs<LegacyForwardTraits::iterator_category, frt::ForwardIteratorTag>);
static_assert(frt::SameAs<LegacyForwardTraits::value_type, LegacyForward::not_value_type>);
static_assert(frt::SameAs<LegacyForwardTraits::difference_type, short>);
static_assert(frt::SameAs<LegacyForwardTraits::reference, const LegacyForward::not_value_type&>);
static_assert(frt::SameAs<LegacyForwardTraits::pointer, void>);
static_assert(!has_iterator_concept<LegacyForwardTraits>);

struct LegacyBidirectional {
  struct value_type {};

  friend bool operator==(LegacyBidirectional, LegacyBidirectional);
  const value_type& operator*() const;
  LegacyBidirectional& operator++();
  LegacyBidirectional operator++(int);
  LegacyBidirectional& operator--();
  LegacyBidirectional operator--(int);
  friend short operator-(LegacyBidirectional, LegacyBidirectional);
};
using LegacyBidirectionalTraits = frt::IteratorTraits<LegacyBidirectional>;
static_assert(frt::SameAs<LegacyBidirectionalTraits::iterator_category, frt::BidirectionalIteratorTag>);
static_assert(frt::SameAs<LegacyBidirectionalTraits::value_type, LegacyBidirectional::value_type>);
static_assert(frt::SameAs<LegacyBidirectionalTraits::difference_type, short>);
static_assert(frt::SameAs<LegacyBidirectionalTraits::reference, const LegacyBidirectional::value_type&>);
static_assert(frt::SameAs<LegacyBidirectionalTraits::pointer, void>);
static_assert(!has_iterator_concept<LegacyBidirectionalTraits>);

// Almost a random access iterator except it is missing operator-(It, It).
struct MinusNotDeclaredIter {
  struct value_type {};

  friend auto operator<=>(MinusNotDeclaredIter, MinusNotDeclaredIter) = default;
  const value_type& operator*() const;
  const value_type& operator[](long) const;
  MinusNotDeclaredIter& operator++();
  MinusNotDeclaredIter operator++(int);
  MinusNotDeclaredIter& operator--();
  MinusNotDeclaredIter operator--(int);
  MinusNotDeclaredIter& operator+=(long);
  MinusNotDeclaredIter& operator-=(long);

  // Providing difference_type does not fully compensate for missing operator-(It, It).
  friend MinusNotDeclaredIter operator-(MinusNotDeclaredIter, int);
  friend MinusNotDeclaredIter operator+(MinusNotDeclaredIter, int);
  friend MinusNotDeclaredIter operator+(int, MinusNotDeclaredIter);
};
template <> struct frt::IncrementableTraits<MinusNotDeclaredIter> { using difference_type = short; };
using MinusNotDeclaredIterTraits = frt::IteratorTraits<MinusNotDeclaredIter>;
static_assert(frt::SameAs<MinusNotDeclaredIterTraits::iterator_category, frt::BidirectionalIteratorTag>);
static_assert(frt::SameAs<MinusNotDeclaredIterTraits::value_type, MinusNotDeclaredIter::value_type>);
static_assert(frt::SameAs<MinusNotDeclaredIterTraits::difference_type, short>);
static_assert(frt::SameAs<MinusNotDeclaredIterTraits::reference, const MinusNotDeclaredIter::value_type&>);
static_assert(frt::SameAs<MinusNotDeclaredIterTraits::pointer, void>);
static_assert(!has_iterator_concept<MinusNotDeclaredIterTraits>);

struct WrongSubscriptReturnType {
  struct value_type {};

  friend auto operator<=>(WrongSubscriptReturnType, WrongSubscriptReturnType) = default;

  // The type of it[n] is not convertible to the type of *it; therefore, this is not random-access.
  value_type& operator*() const;
  const value_type& operator[](long) const;
  WrongSubscriptReturnType& operator++();
  WrongSubscriptReturnType operator++(int);
  WrongSubscriptReturnType& operator--();
  WrongSubscriptReturnType operator--(int);
  WrongSubscriptReturnType& operator+=(long);
  WrongSubscriptReturnType& operator-=(long);
  friend short operator-(WrongSubscriptReturnType, WrongSubscriptReturnType);
  friend WrongSubscriptReturnType operator-(WrongSubscriptReturnType, int);
  friend WrongSubscriptReturnType operator+(WrongSubscriptReturnType, int);
  friend WrongSubscriptReturnType operator+(int, WrongSubscriptReturnType);
};
using WrongSubscriptReturnTypeTraits = frt::IteratorTraits<WrongSubscriptReturnType>;
static_assert(frt::SameAs<WrongSubscriptReturnTypeTraits::iterator_category, frt::BidirectionalIteratorTag>);
static_assert(frt::SameAs<WrongSubscriptReturnTypeTraits::value_type, WrongSubscriptReturnType::value_type>);
static_assert(frt::SameAs<WrongSubscriptReturnTypeTraits::difference_type, short>);
static_assert(frt::SameAs<WrongSubscriptReturnTypeTraits::reference, WrongSubscriptReturnType::value_type&>);
static_assert(frt::SameAs<WrongSubscriptReturnTypeTraits::pointer, void>);
static_assert(!has_iterator_concept<WrongSubscriptReturnTypeTraits>);

struct LegacyRandomAccess {
  struct value_type {};

  friend bool operator==(LegacyRandomAccess, LegacyRandomAccess);
  friend bool operator<(LegacyRandomAccess, LegacyRandomAccess);
  friend bool operator<=(LegacyRandomAccess, LegacyRandomAccess);
  friend bool operator>(LegacyRandomAccess, LegacyRandomAccess);
  friend bool operator>=(LegacyRandomAccess, LegacyRandomAccess);
  const value_type& operator*() const;
  const value_type& operator[](long) const;
  LegacyRandomAccess& operator++();
  LegacyRandomAccess operator++(int);
  LegacyRandomAccess& operator--();
  LegacyRandomAccess operator--(int);
  LegacyRandomAccess& operator+=(long);
  LegacyRandomAccess& operator-=(long);
  friend short operator-(LegacyRandomAccess, LegacyRandomAccess);
  friend LegacyRandomAccess operator-(LegacyRandomAccess, int);
  friend LegacyRandomAccess operator+(LegacyRandomAccess, int);
  friend LegacyRandomAccess operator+(int, LegacyRandomAccess);
};
using LegacyRandomAccessTraits = frt::IteratorTraits<LegacyRandomAccess>;
static_assert(frt::SameAs<LegacyRandomAccessTraits::iterator_category, frt::RandomAccessIteratorTag>);
static_assert(frt::SameAs<LegacyRandomAccessTraits::value_type, LegacyRandomAccess::value_type>);
static_assert(frt::SameAs<LegacyRandomAccessTraits::difference_type, short>);
static_assert(frt::SameAs<LegacyRandomAccessTraits::reference, const LegacyRandomAccess::value_type&>);
static_assert(frt::SameAs<LegacyRandomAccessTraits::pointer, void>);
static_assert(!has_iterator_concept<LegacyRandomAccessTraits>);

struct LegacyRandomAccessSpaceship {
  struct not_value_type {};
  struct ReferenceConvertible {
    operator not_value_type&() const;
  };

  friend auto operator<=>(LegacyRandomAccessSpaceship, LegacyRandomAccessSpaceship) = default;
  not_value_type& operator*() const;
  ReferenceConvertible operator[](long) const;
  LegacyRandomAccessSpaceship& operator++();
  LegacyRandomAccessSpaceship operator++(int);
  LegacyRandomAccessSpaceship& operator--();
  LegacyRandomAccessSpaceship operator--(int);
  LegacyRandomAccessSpaceship& operator+=(long);
  LegacyRandomAccessSpaceship& operator-=(long);
  friend short operator-(LegacyRandomAccessSpaceship, LegacyRandomAccessSpaceship);
  friend LegacyRandomAccessSpaceship operator-(LegacyRandomAccessSpaceship, int);
  friend LegacyRandomAccessSpaceship operator+(LegacyRandomAccessSpaceship, int);
  friend LegacyRandomAccessSpaceship operator+(int, LegacyRandomAccessSpaceship);
};
template <> struct frt::IndirectlyReadableTraits<LegacyRandomAccessSpaceship> {
  using value_type = LegacyRandomAccessSpaceship::not_value_type;
};
template <> struct frt::IncrementableTraits<LegacyRandomAccessSpaceship> {
  using difference_type = short; // or any signed integral type
};
using LegacyRandomAccessSpaceshipTraits = frt::IteratorTraits<LegacyRandomAccessSpaceship>;
static_assert(frt::SameAs<LegacyRandomAccessSpaceshipTraits::iterator_category, frt::RandomAccessIteratorTag>);
static_assert(frt::SameAs<LegacyRandomAccessSpaceshipTraits::value_type, LegacyRandomAccessSpaceship::not_value_type>);
static_assert(frt::SameAs<LegacyRandomAccessSpaceshipTraits::difference_type, short>);
static_assert(frt::SameAs<LegacyRandomAccessSpaceshipTraits::reference, LegacyRandomAccessSpaceship::not_value_type&>);
static_assert(frt::SameAs<LegacyRandomAccessSpaceshipTraits::pointer, void>);
static_assert(!has_iterator_concept<LegacyRandomAccessSpaceshipTraits>);

// For output iterators, value_type, difference_type, and reference may be void.
struct BareLegacyOutput {
  struct Empty {};
  Empty operator*() const;
  BareLegacyOutput& operator++();
  BareLegacyOutput operator++(int);
};
using BareLegacyOutputTraits = frt::IteratorTraits<BareLegacyOutput>;
static_assert(frt::SameAs<BareLegacyOutputTraits::iterator_category, frt::OutputIteratorTag>);
static_assert(frt::SameAs<BareLegacyOutputTraits::value_type, void>);
static_assert(frt::SameAs<BareLegacyOutputTraits::difference_type, void>);
static_assert(frt::SameAs<BareLegacyOutputTraits::reference, void>);
static_assert(frt::SameAs<BareLegacyOutputTraits::pointer, void>);
static_assert(!has_iterator_concept<BareLegacyOutputTraits>);

// The operator- means we get difference_type.
struct LegacyOutputWithMinus {
  struct Empty {};
  Empty operator*() const;
  LegacyOutputWithMinus& operator++();
  LegacyOutputWithMinus operator++(int);
  friend short operator-(LegacyOutputWithMinus, LegacyOutputWithMinus);
  // Lacking operator==, this is a LegacyIterator but not a LegacyInputIterator.
};
using LegacyOutputWithMinusTraits = frt::IteratorTraits<LegacyOutputWithMinus>;
static_assert(frt::SameAs<LegacyOutputWithMinusTraits::iterator_category, frt::OutputIteratorTag>);
static_assert(frt::SameAs<LegacyOutputWithMinusTraits::value_type, void>);
static_assert(frt::SameAs<LegacyOutputWithMinusTraits::difference_type, short>);
static_assert(frt::SameAs<LegacyOutputWithMinusTraits::reference, void>);
static_assert(frt::SameAs<LegacyOutputWithMinusTraits::pointer, void>);
static_assert(!has_iterator_concept<LegacyOutputWithMinusTraits>);

struct LegacyOutputWithMemberTypes {
  struct value_type {}; // ignored
  struct reference {};  // ignored
  using difference_type = long;

  friend bool operator==(LegacyOutputWithMemberTypes, LegacyOutputWithMemberTypes);
  reference operator*() const;
  LegacyOutputWithMemberTypes& operator++();
  LegacyOutputWithMemberTypes operator++(int);
  friend short operator-(LegacyOutputWithMemberTypes, LegacyOutputWithMemberTypes); // ignored
  // Since (*it) is not convertible to value_type, this is not a LegacyInputIterator.
};
using LegacyOutputWithMemberTypesTraits = frt::IteratorTraits<LegacyOutputWithMemberTypes>;
static_assert(frt::SameAs<LegacyOutputWithMemberTypesTraits::iterator_category, frt::OutputIteratorTag>);
static_assert(frt::SameAs<LegacyOutputWithMemberTypesTraits::value_type, void>);
static_assert(frt::SameAs<LegacyOutputWithMemberTypesTraits::difference_type, long>);
static_assert(frt::SameAs<LegacyOutputWithMemberTypesTraits::reference, void>);
static_assert(frt::SameAs<LegacyOutputWithMemberTypesTraits::pointer, void>);
static_assert(!has_iterator_concept<LegacyOutputWithMemberTypesTraits>);

struct LegacyRandomAccessSpecialized {
  struct not_value_type {};

  friend auto operator<=>(LegacyRandomAccessSpecialized, LegacyRandomAccessSpecialized) = default;
  not_value_type& operator*() const;
  not_value_type& operator[](long) const;
  LegacyRandomAccessSpecialized& operator++();
  LegacyRandomAccessSpecialized operator++(int);
  LegacyRandomAccessSpecialized& operator--();
  LegacyRandomAccessSpecialized operator--(int);
  LegacyRandomAccessSpecialized& operator+=(long);
  LegacyRandomAccessSpecialized& operator-=(long);
  friend long operator-(LegacyRandomAccessSpecialized, LegacyRandomAccessSpecialized);
  friend LegacyRandomAccessSpecialized operator-(LegacyRandomAccessSpecialized, int);
  friend LegacyRandomAccessSpecialized operator+(LegacyRandomAccessSpecialized, int);
  friend LegacyRandomAccessSpecialized operator+(int, LegacyRandomAccessSpecialized);
};
template <typename I>
requires frt::SameAs<I, LegacyRandomAccessSpecialized>
struct frt::IteratorTraits<I> {
  using iterator_category = frt::OutputIteratorTag;
  using value_type = short;
  using difference_type = short;
  using reference = short&;
  using pointer = short*;
};
using LegacyRandomAccessSpecializedTraits = frt::IteratorTraits<LegacyRandomAccessSpecialized>;
static_assert(frt::SameAs<LegacyRandomAccessSpecializedTraits::iterator_category, frt::OutputIteratorTag>);
static_assert(frt::SameAs<LegacyRandomAccessSpecializedTraits::value_type, short>);
static_assert(frt::SameAs<LegacyRandomAccessSpecializedTraits::difference_type, short>);
static_assert(frt::SameAs<LegacyRandomAccessSpecializedTraits::reference, short&>);
static_assert(frt::SameAs<LegacyRandomAccessSpecializedTraits::pointer, short*>);
static_assert(!has_iterator_concept<LegacyRandomAccessSpecializedTraits>);

// Other test iterators.

using InputTestIteratorTraits = frt::IteratorTraits<Cpp17TestInputIterator<int*>>;
static_assert(frt::SameAs<InputTestIteratorTraits::iterator_category, frt::InputIteratorTag>);
static_assert(frt::SameAs<InputTestIteratorTraits::value_type, int>);
static_assert(frt::SameAs<InputTestIteratorTraits::difference_type, std::ptrdiff_t>);
static_assert(frt::SameAs<InputTestIteratorTraits::reference, int&>);
static_assert(frt::SameAs<InputTestIteratorTraits::pointer, int*>);
static_assert(!has_iterator_concept<InputTestIteratorTraits>);

using OutputTestIteratorTraits = frt::IteratorTraits<Cpp17TestOutputIterator<int*>>;
static_assert(frt::SameAs<OutputTestIteratorTraits::iterator_category, frt::OutputIteratorTag>);
static_assert(frt::SameAs<OutputTestIteratorTraits::value_type, void>);
static_assert(frt::SameAs<OutputTestIteratorTraits::difference_type, std::ptrdiff_t>);
static_assert(frt::SameAs<OutputTestIteratorTraits::reference, int&>);
static_assert(frt::SameAs<OutputTestIteratorTraits::pointer, int*>);
static_assert(!has_iterator_concept<OutputTestIteratorTraits>);

using ForwardTestIteratorTraits = frt::IteratorTraits<TestForwardIterator<int*>>;
static_assert(frt::SameAs<ForwardTestIteratorTraits::iterator_category, frt::ForwardIteratorTag>);
static_assert(frt::SameAs<ForwardTestIteratorTraits::value_type, int>);
static_assert(frt::SameAs<ForwardTestIteratorTraits::difference_type, std::ptrdiff_t>);
static_assert(frt::SameAs<ForwardTestIteratorTraits::reference, int&>);
static_assert(frt::SameAs<ForwardTestIteratorTraits::pointer, int*>);
static_assert(!has_iterator_concept<ForwardTestIteratorTraits>);

using BidirectionalTestIteratorTraits = frt::IteratorTraits<TestBidirectionalIterator<int*>>;
static_assert(frt::SameAs<BidirectionalTestIteratorTraits::iterator_category, frt::BidirectionalIteratorTag>);
static_assert(frt::SameAs<BidirectionalTestIteratorTraits::value_type, int>);
static_assert(frt::SameAs<BidirectionalTestIteratorTraits::difference_type, std::ptrdiff_t>);
static_assert(frt::SameAs<BidirectionalTestIteratorTraits::reference, int&>);
static_assert(frt::SameAs<BidirectionalTestIteratorTraits::pointer, int*>);
static_assert(!has_iterator_concept<BidirectionalTestIteratorTraits>);

using RandomAccessTestIteratorTraits = frt::IteratorTraits<TestRandomAccessIterator<int*>>;
static_assert(frt::SameAs<RandomAccessTestIteratorTraits::iterator_category, frt::RandomAccessIteratorTag>);
static_assert(frt::SameAs<RandomAccessTestIteratorTraits::value_type, int>);
static_assert(frt::SameAs<RandomAccessTestIteratorTraits::difference_type, std::ptrdiff_t>);
static_assert(frt::SameAs<RandomAccessTestIteratorTraits::reference, int&>);
static_assert(frt::SameAs<RandomAccessTestIteratorTraits::pointer, int*>);
static_assert(!has_iterator_concept<RandomAccessTestIteratorTraits>);

using ContiguousTestIteratorTraits = frt::IteratorTraits<TestContiguousIterator<int*>>;
static_assert(frt::SameAs<ContiguousTestIteratorTraits::iterator_category, frt::ContiguousIteratorTag>);
static_assert(frt::SameAs<ContiguousTestIteratorTraits::value_type, int>);
static_assert(frt::SameAs<ContiguousTestIteratorTraits::difference_type, std::ptrdiff_t>);
static_assert(frt::SameAs<ContiguousTestIteratorTraits::reference, int&>);
static_assert(frt::SameAs<ContiguousTestIteratorTraits::pointer, int*>);
static_assert(!has_iterator_concept<ContiguousTestIteratorTraits>);

using Cpp17BasicIteratorTraits = frt::IteratorTraits<IteratorTraitsCpp17TestIterator>;
static_assert(frt::SameAs<Cpp17BasicIteratorTraits::iterator_category, frt::OutputIteratorTag>);
static_assert(frt::SameAs<Cpp17BasicIteratorTraits::value_type, void>);
static_assert(frt::SameAs<Cpp17BasicIteratorTraits::difference_type, void>);
static_assert(frt::SameAs<Cpp17BasicIteratorTraits::reference, void>);
static_assert(frt::SameAs<Cpp17BasicIteratorTraits::pointer, void>);
static_assert(!has_iterator_concept<Cpp17BasicIteratorTraits>);

using Cpp17InputIteratorTraits = frt::IteratorTraits<IteratorTraitsCpp17TestInputIterator>;
static_assert(frt::SameAs<Cpp17InputIteratorTraits::iterator_category, frt::InputIteratorTag>);
static_assert(frt::SameAs<Cpp17InputIteratorTraits::value_type, long>);
static_assert(frt::SameAs<Cpp17InputIteratorTraits::difference_type, int>);
static_assert(frt::SameAs<Cpp17InputIteratorTraits::reference, int&>);
static_assert(frt::SameAs<Cpp17InputIteratorTraits::pointer, void>);
static_assert(!has_iterator_concept<Cpp17InputIteratorTraits>);

using Cpp17ForwardIteratorTraits = frt::IteratorTraits<IteratorTraitsCpp17TestForwardIterator>;
static_assert(frt::SameAs<Cpp17ForwardIteratorTraits::iterator_category, frt::ForwardIteratorTag>);
static_assert(frt::SameAs<Cpp17ForwardIteratorTraits::value_type, int>);
static_assert(frt::SameAs<Cpp17ForwardIteratorTraits::difference_type, int>);
static_assert(frt::SameAs<Cpp17ForwardIteratorTraits::reference, int&>);
static_assert(frt::SameAs<Cpp17ForwardIteratorTraits::pointer, void>);
static_assert(!has_iterator_concept<Cpp17ForwardIteratorTraits>);