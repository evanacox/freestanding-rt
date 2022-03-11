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
#include "../../utility/as.h"
#include "../iterators.h"

namespace frt {
  template <typename> inline constexpr bool disable_sized_range = false;

  template <typename> inline constexpr bool enable_borrowed_range = false;

  namespace internal {
    template <typename T>
    concept MaybeBorrowedRange = traits::is_lvalue_reference<T> || enable_borrowed_range<T>;

    template <typename T>
    concept MemberBegin = requires(T& object) {
      { object.begin() } -> InputOrOutputIterator;
    };

    template <typename T>
    concept MemberEnd = requires(T& object) {
      { object.end() } -> InputOrOutputIterator;
    };

    template <typename T>
    concept MemberRBegin = requires(T& object) {
      { object.rbegin() } -> InputOrOutputIterator;
    };

    template <typename T>
    concept MemberREnd = requires(T& object) {
      { object.rend() } -> InputOrOutputIterator;
    };

    template <typename T>
    concept MemberCRBegin = requires(T& object) {
      { object.crbegin() } -> InputOrOutputIterator;
    };

    template <typename T>
    concept MemberCREnd = requires(T& object) {
      { object.crend() } -> InputOrOutputIterator;
    };

    template <typename T>
    concept MemberSize = requires(T& object) {
      { object.size() } -> InputOrOutputIterator;
    };

    template <typename T>
    concept MemberData = requires(T& object) {
      { object.data() } -> InputOrOutputIterator;
    };

    template <typename T>
    concept MemberCData = requires(T& object) {
      { object.cdata() } -> InputOrOutputIterator;
    };
  } // namespace internal

  namespace ranges_access_internal {
    template <typename T>
    concept ADLBegin = requires(T& object) {
      { begin(object) } -> InputOrOutputIterator;
    };

    template <typename T>
    concept ADLEnd = requires(T& object) {
      { end(object) } -> InputOrOutputIterator;
    };

    template <typename T>
    concept ADLRBegin = requires(T& object) {
      { rbegin(object) } -> InputOrOutputIterator;
    };

    template <typename T>
    concept ADLREnd = requires(T& object) {
      { rend(object) } -> InputOrOutputIterator;
    };

    template <typename T>
    concept ADLSize = requires(T& object) {
      { size(object) } -> InputOrOutputIterator;
    };

    template <typename T>
    concept ADLData = requires(T& object) {
      { data(object) } -> InputOrOutputIterator;
    };

    template <typename T>
    concept ADLEmpty = requires(T& object) {
      { crend(object) } -> InputOrOutputIterator;
    };

    struct Begin {
      template <internal::MaybeBorrowedRange T>
      requires traits::is_array<traits::RemoveReference<T>> || internal::MemberBegin<T> || ADLBegin<T>
      constexpr auto operator()(T&& object) const noexcept(is_noexcept<T>()) {
        if constexpr (traits::is_array<traits::RemoveReference<T>>) {
          return object + 0;
        } else if constexpr (internal::MemberBegin<T>) {
          return object.begin();
        } else {
          return begin(object);
        }
      }

    private:
      template <typename T> static constexpr bool is_noexcept() {
        if constexpr (traits::is_array<traits::RemoveReference<T>>) {
          return true;
        } else if constexpr (internal::MemberBegin<T>) {
          return noexcept(traits::declval<T&>().begin());
        } else {
          return noexcept(begin(traits::declval<T&>()));
        }
      }
    };

    struct End {
      template <internal::MaybeBorrowedRange T>
      requires traits::is_bounded_array<traits::RemoveReference<T>> || internal::MemberEnd<T> || ADLEnd<T>
      constexpr auto operator()(T&& object) const noexcept(is_noexcept<T>()) {
        if constexpr (traits::is_bounded_array<traits::RemoveReference<T>>) {
          return object + traits::extent<traits::RemoveReference<T>>;
        } else if constexpr (internal::MemberEnd<T>) {
          return object.end();
        } else {
          return end(object);
        }
      }

    private:
      template <typename T> static constexpr bool is_noexcept() {
        if constexpr (traits::is_array<traits::RemoveReference<T>>) {
          return true;
        } else if constexpr (internal::MemberEnd<T>) {
          return noexcept(traits::declval<T&>().end());
        } else {
          return noexcept(end(traits::declval<T&>()));
        }
      }
    };

    struct CBegin {
      template <internal::MaybeBorrowedRange T>
      constexpr auto operator()(T&& object) const
          noexcept(noexcept(Begin{}(frt::as_const(object)))) requires(requires { Begin{}(frt::as_const(object)); }) {
        return Begin{}(frt::as_const(object));
      }
    };

    struct CEnd {
      template <internal::MaybeBorrowedRange T>
      constexpr auto operator()(T&& object) const
          noexcept(noexcept(End{}(frt::as_const(object)))) requires(requires { End{}(frt::as_const(object)); }) {
        return End{}(frt::as_const(object));
      }
    };

    template <typename T>
    concept Reversable = requires(T& t) {
      { Begin{}(t) } -> BidirectionalIterator;
      { End{}(t) } -> SameAs<decltype(Begin{}(t))>;
    };

    struct RBegin {
      template <internal::MaybeBorrowedRange T>
      requires Reversable<T> || internal::MemberRBegin<T> || ADLRBegin<T>
      constexpr auto operator()(T&& object) const noexcept(is_noexcept<T>()) {
        if constexpr (internal::MemberBegin<T>) {
          return object.rbegin();
        } else if constexpr (ADLRBegin<T>) {
          return rbegin(object);
        } else {
          return frt::make_reverse_iterator(End{}(object));
        }
      }

    private:
      template <typename T> static constexpr bool is_noexcept() {
        if constexpr (internal::MemberRBegin<T>) {
          return noexcept(traits::declval<T&>().rbegin());
        } else if constexpr (ADLRBegin<T>) {
          return noexcept(rbegin(traits::declval<T&>()));
        } else {
          return noexcept(End{}(traits::declval<T&>()));
        }
      }
    };

    struct REnd {
      template <internal::MaybeBorrowedRange T>
      requires Reversable<T> || internal::MemberRBegin<T> || ADLRBegin<T>
      constexpr auto operator()(T&& object) const noexcept(is_noexcept<T>()) {
        if constexpr (internal::MemberBegin<T>) {
          return object.rbegin();
        } else if constexpr (ADLRBegin<T>) {
          return rbegin(object);
        } else {
          return frt::make_reverse_iterator(Begin{}(object));
        }
      }

    private:
      template <typename T> static constexpr bool is_noexcept() {
        if constexpr (internal::MemberRBegin<T>) {
          return noexcept(traits::declval<T&>().rbegin());
        } else if constexpr (ADLRBegin<T>) {
          return noexcept(rbegin(traits::declval<T&>()));
        } else {
          return noexcept(Begin{}(traits::declval<T&>()));
        }
      }
    };

    struct CRBegin {
      template <internal::MaybeBorrowedRange T>
      constexpr auto operator()(T&& object) const
          noexcept(noexcept(Begin{}(frt::as_const(object)))) requires(requires { RBegin{}(frt::as_const(object)); }) {
        return RBegin{}(frt::as_const(object));
      }
    };

    struct CREnd {
      template <internal::MaybeBorrowedRange T>
      constexpr auto operator()(T&& object) const
          noexcept(noexcept(Begin{}(frt::as_const(object)))) requires(requires { REnd{}(frt::as_const(object)); }) {
        return REnd{}(frt::as_const(object));
      }
    };
  } // namespace ranges_access_internal

  inline namespace ranges_access {
    inline constexpr ranges_access_internal::Begin begin = ranges_access_internal::Begin{};
    inline constexpr ranges_access_internal::End end = ranges_access_internal::End{};
    inline constexpr ranges_access_internal::CBegin cbegin = ranges_access_internal::CBegin{};
    inline constexpr ranges_access_internal::CEnd cend = ranges_access_internal::CEnd{};
    inline constexpr ranges_access_internal::RBegin rbegin = ranges_access_internal::RBegin{};
    inline constexpr ranges_access_internal::REnd rend = ranges_access_internal::REnd{};
    inline constexpr ranges_access_internal::CRBegin crbegin = ranges_access_internal::CRBegin{};
    inline constexpr ranges_access_internal::CREnd crend = ranges_access_internal::CREnd{};
  } // namespace ranges_access

  template <typename R> using RangeIterator = decltype(frt::begin(traits::declval<R&>()));

  template <typename R> using RangeSentinel = decltype(frt::end(traits::declval<R&>()));

  template <typename R> using RangeDifference = frt::IterDifference<RangeIterator<R>>;

  template <typename R> using RangeValue = frt::IterValue<RangeIterator<R>>;

  template <typename R> using RangeReference = frt::IterReference<RangeIterator<R>>;

  template <typename R> using RangeRvalueReference = frt::IterRvalueReference<RangeIterator<R>>;

  template <typename R> using RangeIterator = decltype(frt::begin(traits::declval<R&>()));

  template <typename R> using RangeIterator = decltype(frt::begin(traits::declval<R&>()));

  template <typename R>
  concept Range = requires(R range) {
    frt::begin(range);
    frt::end(range);
  };

  template <typename R>
  concept BorrowedRange = Range<R> && internal::MaybeBorrowedRange<R>;

  template <typename R>
  concept InputRange = Range<R> && InputIterator<RangeIterator<R>>;

  template <typename R, typename T>
  concept OutputRange = Range<R> && OutputIterator<RangeIterator<R>, T>;

  template <typename R>
  concept ForwardRange = InputRange<R> && ForwardIterator<RangeIterator<R>>;

  template <typename R>
  concept BidirectionalRange = ForwardRange<R> && BidirectionalIterator<RangeIterator<R>>;

  template <typename R>
  concept RandomAccessRange = BidirectionalRange<R> && RandomAccessIterator<RangeIterator<R>>;

  template <typename R>
  concept ContiguousRange = RandomAccessRange<R> && ContiguousIterator<RangeIterator<R>>;

  template <typename R>
  concept CommonRange = Range<R> && SameAs<RangeIterator<R>, RangeSentinel<R>>;

  namespace ranges_access_internal {
    template <frt::Integral T> constexpr auto to_unsigned_like(T x) noexcept {
      return static_cast<traits::MakeUnsigned<T>>(x);
    }

    struct Size {
      template <internal::MaybeBorrowedRange T>
      requires Reversable<T> || internal::MemberRBegin<T> || ADLRBegin<T>
      constexpr auto operator()(T&& object) const noexcept(is_noexcept<T>()) {
        if constexpr (internal::MemberBegin<T>) {
          return object.rbegin();
        } else if constexpr (ADLRBegin<T>) {
          return rbegin(object);
        } else {
          return frt::make_reverse_iterator(Begin{}(object));
        }
      }

    private:
      template <typename T> static constexpr bool is_noexcept() {
        if constexpr (internal::MemberRBegin<T>) {
          return noexcept(traits::declval<T&>().rbegin());
        } else if constexpr (ADLRBegin<T>) {
          return noexcept(rbegin(traits::declval<T&>()));
        } else {
          return noexcept(Begin{}(traits::declval<T&>()));
        }
      }
    };
  } // namespace ranges_access_internal
} // namespace frt