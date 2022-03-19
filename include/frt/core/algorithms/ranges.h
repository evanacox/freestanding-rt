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
    constexpr traits::Decay<T> decay_copy(T&& t) noexcept(traits::is_nothrow_convertible<T, traits::Decay<T>>) {
      return frt::forward<T>(t);
    }

    template <typename T>
    concept MaybeBorrowedRange = traits::is_lvalue_reference<T> || enable_borrowed_range<T>;

    template <typename T>
    concept ClassOrEnum = traits::is_class<T> || traits::is_enum<T>;

    template <typename To, typename From> constexpr decltype(auto) forwarding_as_const(From& object) noexcept {
      static_assert(SameAs<To&, From&>);

      if constexpr (traits::is_lvalue_reference<To>) {
        return const_cast<const From&>(object);
      } else {
        return static_cast<const From&&>(object);
      }
    }
  } // namespace internal

  namespace ranges_access_internal {
    template <typename T>
    concept MemberBegin = requires(T& object) {
      { internal::decay_copy(object.begin()) } -> InputOrOutputIterator;
    };

    template <typename T>
    concept ADLBegin = internal::ClassOrEnum<traits::RemoveReference<T>> && requires(T& object) {
      { internal::decay_copy(begin(object)) } -> InputOrOutputIterator;
    };

    struct Begin {
    private:
      template <typename T> static constexpr bool is_noexcept() {
        if constexpr (traits::is_array<traits::RemoveReference<T>>) {
          return true;
        } else if constexpr (MemberBegin<T>) {
          return noexcept(traits::declval<T&>().begin());
        } else {
          return noexcept(begin(traits::declval<T&>()));
        }
      }

    public:
      template <internal::MaybeBorrowedRange T>
      requires traits::is_array<traits::RemoveReference<T>> || MemberBegin<T> || ADLBegin<T>
      constexpr auto operator()(T&& object) const noexcept(is_noexcept<T>()) {
        if constexpr (traits::is_array<traits::RemoveReference<T>>) {
          return object + 0;
        } else if constexpr (MemberBegin<T>) {
          return object.begin();
        } else {
          return begin(object);
        }
      }
    };
  } // namespace ranges_access_internal

  namespace ranges_access_internal {
    template <typename R> using RangeIter = decltype(Begin{}(traits::declval<R&>()));

    template <typename T>
    concept MemberEnd = requires(T& object) {
      { internal::decay_copy(object.end()) } -> SentinelFor<RangeIter<T>>;
    };

    template <typename T>
    concept ADLEnd = internal::ClassOrEnum<traits::RemoveReference<T>> && requires(T& object) {
      { internal::decay_copy(end(object)) } -> SentinelFor<RangeIter<T>>;
    };

    struct End {
    private:
      template <typename T> static constexpr bool is_noexcept() {
        if constexpr (traits::is_array<traits::RemoveReference<T>>) {
          return true;
        } else if constexpr (MemberEnd<T>) {
          return noexcept(traits::declval<T&>().end());
        } else {
          return noexcept(end(traits::declval<T&>()));
        }
      }

    public:
      template <internal::MaybeBorrowedRange T>
      requires traits::is_bounded_array<traits::RemoveReference<T>> || MemberEnd<T> || ADLEnd<T>
      constexpr auto operator()(T&& object) const noexcept(is_noexcept<T>()) {
        if constexpr (traits::is_bounded_array<traits::RemoveReference<T>>) {
          return object + traits::extent<traits::RemoveReference<T>>;
        } else if constexpr (MemberEnd<T>) {
          return object.end();
        } else {
          return end(object);
        }
      }
    };

    struct CBegin {
      template <internal::MaybeBorrowedRange T>
      constexpr auto operator()(T&& object) const
          noexcept(noexcept(Begin{}(internal::forwarding_as_const<T>(object)))) requires(
              requires { Begin{}(internal::forwarding_as_const<T>(object)); }) {
        return Begin{}(internal::forwarding_as_const<T>(object));
      }
    };

    struct CEnd {
      template <internal::MaybeBorrowedRange T>
      constexpr auto operator()(T&& object) const
          noexcept(noexcept(End{}(internal::forwarding_as_const<T>(object)))) requires(
              requires { End{}(internal::forwarding_as_const<T>(object)); }) {
        return End{}(internal::forwarding_as_const<T>(object));
      }
    };

    template <typename T>
    concept Reversable = requires(T& t) {
      { Begin{}(t) } -> BidirectionalIterator;
      { End{}(t) } -> SameAs<decltype(Begin{}(t))>;
    };

    template <typename T>
    concept MemberRBegin = requires(T& object) {
      { internal::decay_copy(object.rbegin()) } -> InputOrOutputIterator;
    };

    template <typename T>
    concept ADLRBegin = internal::ClassOrEnum<traits::RemoveReference<T>> && requires(T& object) {
      { internal::decay_copy(rbegin(object)) } -> InputOrOutputIterator;
    };

    struct RBegin {
    private:
      template <typename T> static constexpr bool is_noexcept() {
        if constexpr (MemberRBegin<T>) {
          return noexcept(traits::declval<T&>().rbegin());
        } else if constexpr (ADLRBegin<T>) {
          return noexcept(rbegin(traits::declval<T&>()));
        } else {
          return noexcept(End{}(traits::declval<T&>()));
        }
      }

    public:
      template <internal::MaybeBorrowedRange T>
      requires Reversable<T> || MemberRBegin<T> || ADLRBegin<T>
      constexpr auto operator()(T&& object) const noexcept(is_noexcept<T>()) {
        if constexpr (MemberBegin<T>) {
          return object.rbegin();
        } else if constexpr (ADLRBegin<T>) {
          return rbegin(object);
        } else {
          return frt::make_reverse_iterator(End{}(object));
        }
      }
    };

    template <typename T>
    concept MemberREnd = requires(T& object) {
      { internal::decay_copy(object.rend()) } -> SentinelFor<decltype(RBegin{}(object))>;
    };

    template <typename T>
    concept ADLREnd = internal::ClassOrEnum<traits::RemoveReference<T>> && requires(T& object) {
      { internal::decay_copy(rend(object)) } -> SentinelFor<decltype(RBegin{}(object))>;
    };

    struct REnd {
    private:
      template <typename T> static constexpr bool is_noexcept() {
        if constexpr (MemberREnd<T>) {
          return noexcept(traits::declval<T&>().rend());
        } else if constexpr (ADLREnd<T>) {
          return noexcept(rend(traits::declval<T&>()));
        } else {
          return noexcept(Begin{}(traits::declval<T&>()));
        }
      }

    public:
      template <internal::MaybeBorrowedRange T>
      requires Reversable<T> || MemberREnd<T> || ADLREnd<T>
      constexpr auto operator()(T&& object) const noexcept(is_noexcept<T>()) {
        if constexpr (MemberREnd<T>) {
          return object.rend();
        } else if constexpr (ADLREnd<T>) {
          return rend(object);
        } else {
          return frt::make_reverse_iterator(Begin{}(object));
        }
      }
    };

    struct CRBegin {
      template <internal::MaybeBorrowedRange T>
      constexpr auto operator()(T&& object) const
          noexcept(noexcept(RBegin{}(internal::forwarding_as_const<T>(object)))) requires(
              requires { RBegin{}(internal::forwarding_as_const<T>(object)); }) {
        return RBegin{}(internal::forwarding_as_const<T>(object));
      }
    };

    struct CREnd {
      template <internal::MaybeBorrowedRange T>
      constexpr auto operator()(T&& object) const
          noexcept(noexcept(REnd{}(internal::forwarding_as_const<T>(object)))) requires(
              requires { REnd{}(internal::forwarding_as_const<T>(object)); }) {
        return REnd{}(internal::forwarding_as_const<T>(object));
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
    template <typename T>
    requires Integral<traits::RemoveCV<T>>
    constexpr auto to_unsigned_like(T x) noexcept {
      return static_cast<traits::MakeUnsigned<traits::RemoveCV<T>>>(x);
    }

    template <typename T>
    requires Integral<traits::RemoveCV<T>>
    constexpr auto to_signed_like(T x) noexcept {
      return static_cast<frt::isize>(x);
    }

    template <typename T>
    concept NotSizeDisabled = !disable_sized_range<traits::RemoveCVRef<T>>;

    template <typename T>
    concept MemberSize = NotSizeDisabled<T> && requires(T & object) {
      { internal::decay_copy(object.size()) } -> Integral;
    };

    template <typename T>
    concept ADLSize = internal::ClassOrEnum<traits::RemoveReference<T>> && NotSizeDisabled<T> && requires(T & object) {
      { internal::decay_copy(size(object)) } -> Integral;
    };

    struct Size {
    private:
      template <typename T> static constexpr bool is_noexcept() {
        if constexpr (MemberSize<T>) {
          return noexcept(traits::declval<T&>().size());
        } else if constexpr (ADLSize<T>) {
          return noexcept(size(traits::declval<T&>()));
        } else {
          return noexcept(End{}(traits::declval<T&>() - Begin{}(traits::declval<T&>())));
        }
      }

    public:
      template <typename T>
      requires traits::is_bounded_array<traits::RemoveReference<T>> || MemberSize<T> || ADLSize<T>
      constexpr auto operator()(T&& object) const noexcept(is_noexcept<T>()) {
        if constexpr (traits::is_bounded_array<traits::RemoveReference<T>>) {
          return traits::extent<traits::RemoveReference<T>>;
        } else if constexpr (MemberSize<T> && NotSizeDisabled<T> && frt::Integral<decltype(object.size())>) {
          return ranges_access_internal::to_unsigned_like(object.size());
        } else if constexpr (ADLSize<T> && NotSizeDisabled<T>) {
          return ranges_access_internal::to_unsigned_like(size(object));
        } else if constexpr (ForwardRange<T> && SizedSentinelFor<RangeSentinel<T>, RangeIterator<T>>) {
          return ranges_access_internal::to_unsigned_like(frt::end(object) - frt::begin(object));
        }
      }
    };

    struct SSize {
      template <typename T>
      requires requires(T&& object) {
        ranges_access_internal::to_signed_like(Size{}(object));
      }
      constexpr auto operator()(T&& object) const noexcept(Size{}(traits::declval<T&&>())) {
        return ranges_access_internal::to_signed_like(Size{}(frt::forward<T>(object)));
      }
    };

    template <typename T>
    concept MemberEmpty = requires(T & object) {
      { internal::decay_copy(object.empty()) } -> internal::BooleanTestable;
    };

    template <typename T>
    concept SizeComparable = requires(T && object) {
      Size{}(object) == 0;
    };

    struct Empty {
    private:
      template <typename T> static constexpr bool is_noexcept() {
        if constexpr (MemberEmpty<T>) {
          return noexcept(traits::declval<T&&>().empty());
        } else if constexpr (requires(T && object) { Size{}(object) == 0; }) {
          return noexcept(Size{}(traits::declval<T&&>()) == 0);
        } else {
          return noexcept(bool(frt::begin(traits::declval<T&&>()) == frt::end(traits::declval<T&&>())));
        }
      }

    public:
      template <typename T>
      requires MemberEmpty<T> || SizeComparable<T> || ForwardRange<T>
      constexpr bool operator()(T&& object) const noexcept(is_noexcept<T>()) {
        if constexpr (MemberEmpty<T>) {
          return bool(object.empty());
        } else if constexpr (SizeComparable<T>) {
          return bool(Size{}(object) == 0);
        } else {
          return bool(frt::begin(object) == frt::end(object));
        }
      }
    };

    template <typename T>
    concept PointerToObject = traits::is_pointer<T> && traits::is_object<traits::RemovePointer<T>>;

    template <typename T>
    concept MemberData = requires(T && object) {
      { internal::decay_copy(object.data()) } -> PointerToObject;
    };

    template <typename T>
    concept ContiguousBegin = frt::ContiguousIterator<RangeIterator<T>>;

    struct Data {
    private:
      template <typename T> static constexpr bool is_noexcept() {
        if constexpr (MemberData<T>) {
          return noexcept(traits::declval<T&&>().data());
        } else {
          return noexcept(frt::to_address(frt::begin(traits::declval<T&&>())));
        }
      }

    public:
      template <typename T>
      requires MemberData<T> || ContiguousBegin<T>
      constexpr bool operator()(T&& object) const noexcept(is_noexcept<T>()) {
        if constexpr (MemberData<T>) {
          return frt::forward<T>(object).data();
        } else {
          return frt::to_address(frt::begin(frt::forward<T>(object)));
        }
      }
    };

    struct CData {
      template <typename T>
      requires requires(T&& object) {
        Data{}(internal::forwarding_as_const<T>(object));
      }
      constexpr auto operator()(T&& object) const
          noexcept(Data{}(internal::forwarding_as_const<T>(traits::declval<T&&>()))) {
        return Data{}(internal::forwarding_as_const<T>(object));
      }
    };
  } // namespace ranges_access_internal

  inline namespace ranges_access {
    inline constexpr ranges_access_internal::Size size = ranges_access_internal::Size{};
    inline constexpr ranges_access_internal::SSize ssize = ranges_access_internal::SSize{};
    inline constexpr ranges_access_internal::Empty empty = ranges_access_internal::Empty{};
    inline constexpr ranges_access_internal::Data data = ranges_access_internal::Data{};
    inline constexpr ranges_access_internal::CData cdata = ranges_access_internal::CData{};
  } // namespace ranges_access

  template <typename T>
  concept SizedRange = frt::Range<T> && requires(T& t) {
    frt::size(t);
  };
} // namespace frt