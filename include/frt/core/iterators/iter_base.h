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
#include "../../types/traits.h"
#include "./concepts.h"
#include "./operations.h"
#include "./tags.h"
#include "./traits.h"

namespace frt {
  namespace internal {
    template <typename Derived, typename Type, typename Value, typename Reference, typename Pointer, typename DiffType>
    class IteratorInterfaceBase;

    template <typename Derived, typename Value, typename Reference, typename Pointer, typename DiffType>
    class IteratorInterfaceBase<Derived, internal::OutputTag, Value, Reference, Pointer, DiffType> {
    protected:
      constexpr OutputIteratorReady<Reference, Pointer, DiffType> auto& self() noexcept {
        return *static_cast<Derived*>(this);
      }

      constexpr const OutputIteratorReady<Reference, Pointer, DiffType> auto& self() const noexcept {
        return *static_cast<const Derived*>(this);
      }

      constexpr decltype(auto) value() const noexcept(noexcept(self().value())) {
        return self().value();
      }

      constexpr void next() noexcept(noexcept(self().next())) {
        self().next();
      }
    };

    template <typename Derived, typename Value, typename Reference, typename Pointer, typename DiffType>
    class IteratorInterfaceBase<Derived, internal::InputTag, Value, Reference, Pointer, DiffType> {
    protected:
      constexpr InputIteratorReady<Reference, Pointer, DiffType> auto& self() noexcept {
        return *static_cast<Derived*>(this);
      }

      constexpr const InputIteratorReady<Reference, Pointer, DiffType> auto& self() const noexcept {
        return *static_cast<const Derived*>(this);
      }

      constexpr decltype(auto) value() const noexcept(noexcept(self().value())) {
        return self().value();
      }

      constexpr void next() noexcept(noexcept(self().next())) {
        self().next();
      }
    };

    template <typename Derived, typename Value, typename Reference, typename Pointer, typename DiffType>
    class IteratorInterfaceBase<Derived, internal::ForwardTag, Value, Reference, Pointer, DiffType> {
    protected:
      constexpr ForwardIteratorReady<Reference, Pointer, DiffType> auto& self() noexcept {
        return *static_cast<Derived*>(this);
      }

      constexpr const ForwardIteratorReady<Reference, Pointer, DiffType> auto& self() const noexcept {
        return *static_cast<const Derived*>(this);
      }

      constexpr decltype(auto) value() const noexcept(noexcept(self().value())) {
        return self().value();
      }

      constexpr void next() noexcept(noexcept(self().next())) {
        self().next();
      }
    };

    template <typename Derived, typename Value, typename Reference, typename Pointer, typename DiffType>
    class IteratorInterfaceBase<Derived, internal::BidirectionTag, Value, Reference, Pointer, DiffType> {
    protected:
      constexpr BidirectionalIteratorReady<Reference, Pointer, DiffType> auto& self() noexcept {
        return *static_cast<Derived*>(this);
      }

      constexpr const BidirectionalIteratorReady<Reference, Pointer, DiffType> auto& self() const noexcept {
        return *static_cast<const Derived*>(this);
      }

      constexpr decltype(auto) value() const noexcept(noexcept(self().value())) {
        return self().value();
      }

      constexpr void next() noexcept(noexcept(self().next())) {
        self().next();
      }

      constexpr void prev() noexcept(noexcept(self().prev())) {
        self().prev();
      }
    };

    template <typename Derived, typename Value, typename Reference, typename Pointer, typename DiffType>
    class IteratorInterfaceBase<Derived, internal::RandomTag, Value, Reference, Pointer, DiffType> {
    protected:
      constexpr RandomAccessIteratorReady<Reference, Pointer, DiffType> auto& self() noexcept {
        return *static_cast<Derived*>(this);
      }

      constexpr const RandomAccessIteratorReady<Reference, Pointer, DiffType> auto& self() const noexcept {
        return *static_cast<const Derived*>(this);
      }

      constexpr decltype(auto) value() const noexcept(noexcept(self().value())) {
        return self().value();
      }

      constexpr void move(DiffType n) noexcept(noexcept(self().move(n))) {
        self().move(n);
      }

      constexpr void next() noexcept(noexcept(move(1))) {
        move(1);
      }

      constexpr void prev() noexcept(noexcept(move(-1))) {
        move(-1);
      }

      constexpr DiffType distance_to(const Derived& other) noexcept(noexcept(self().distance_to(other))) {
        return self().distance_to(other);
      }
    };

    template <typename Derived, typename Value, typename Reference, typename Pointer, typename DiffType>
    class IteratorInterfaceBase<Derived, internal::ContiguousTag, Value, Reference, Pointer, DiffType> {
    protected:
      static inline constexpr auto has_value = requires(Derived d) {
        { d.value() } -> SameAs<Reference>;
      };

      constexpr ContiguousIteratorReady<Pointer, DiffType> auto& self() noexcept {
        return *static_cast<Derived*>(this);
      }

      constexpr const ContiguousIteratorReady<Pointer, DiffType> auto& self() const noexcept {
        return *static_cast<const Derived*>(this);
      }

      constexpr decltype(auto) value() const noexcept {
        if constexpr (has_value) {
          return self().value();
        } else {
          return *(self().address());
        }
      }

      constexpr decltype(auto) address() const noexcept {
        return self().address();
      }

      constexpr void move(DiffType n) noexcept {
        self().move(n);
      }

      constexpr void next() noexcept {
        move(1);
      }

      constexpr void prev() noexcept {
        move(-1);
      }

      constexpr DiffType distance_to(const Derived& other) noexcept(noexcept(self().distance_to(other))) {
        return self().distance_to(other);
      }
    };

    template <typename Derived, typename Type, typename Value, typename Reference, typename Pointer, typename DiffType>
    class IteratorTypedefs : public IteratorInterfaceBase<Derived, Type, Value, Reference, Pointer, DiffType> {
    public:
      using iterator_category = Type;
      using difference_type = DiffType;
      using value_type = Value;
      using reference = Reference;
      using pointer = Pointer;
    };

    template <typename Derived, typename Type, typename Value, typename Reference, typename Pointer, typename DiffType>
    class InputIteratorBase : public IteratorTypedefs<Derived, Type, Value, Reference, Pointer, DiffType> {
      using Base = IteratorTypedefs<Derived, Type, Value, Reference, Pointer, DiffType>;

    public:
      /// Gets the current value of the iterator
      ///
      /// \return A reference to the current value
      typename Base::reference operator*() const noexcept(noexcept(traits::declval<Derived>().value())) {
        return Base::value();
      }

      /// Moves to the next value
      ///
      /// \return A reference to *this
      Derived& operator++() noexcept(noexcept(traits::declval<Derived>().next())) {
        Base::next();

        return Base::self();
      }

      /// Moves to the next value
      ///
      /// \tparam Hidden Used for Concepts
      template <typename Hidden = Derived>
      constexpr void operator++(int) noexcept(noexcept(traits::declval<Derived>().next())) requires(
          !CopyConstructible<Hidden>) {
        Base::next();
      }

      /// Moves to the next value, and returns the old state
      ///
      /// \tparam Hidden Used for Concepts
      /// \return A copy of the iterator from before ++
      template <typename Hidden = Derived>
      [[nodiscard]] constexpr Derived operator++(int) noexcept(
          noexcept(traits::declval<Derived>().next())
          && traits::is_nothrow_copy_constructible<Derived>) requires CopyConstructible<Hidden> {
        auto copy = Base::self();
        Base::next();
        return copy;
      }

      /// If `Derived` has an `eq` member, uses that. Otherwise uses `cmp` and checks
      /// against `strong_ordering::equal`
      ///
      /// \param other The other to compare against
      /// \return `lhs.eq(rhs)` or `lhs.cmp(rhs) == std::strong_ordering::equal`
      [[nodiscard]] friend constexpr bool operator==(const Derived& lhs, const Derived& rhs) noexcept {
        if constexpr (internal::HasEq<Derived>) {
          return lhs.eq(rhs);
        } else {
          return lhs.cmp(rhs) == std::strong_ordering::equal;
        }
      }

    protected:
      ~InputIteratorBase() noexcept = default;
    };

    template <typename Derived, typename Type, typename Value, typename Reference, typename Pointer, typename DiffType>
    class ForwardIteratorBase : public InputIteratorBase<Derived, Type, Value, Reference, Pointer, DiffType> {};

    template <typename Derived, typename Type, typename Value, typename Reference, typename Pointer, typename DiffType>
    class BidirectionIteratorBase : public ForwardIteratorBase<Derived, Type, Value, Reference, Pointer, DiffType> {
      using Base = ForwardIteratorBase<Derived, Type, Value, Reference, Pointer, DiffType>;

    public:
      /// Moves the iterator backwards one element
      ///
      /// \return A reference to the current iterator
      constexpr Derived& operator--() noexcept(noexcept(traits::declval<Derived>().prev())) {
        Base::prev();

        return Base::self();
      }

      /// Moves the iterator backwards one element and returns a copy that has the
      /// pre-decrement value
      ///
      /// \return A copy of the iterator from before `--` was called
      constexpr Derived operator--(int) noexcept(
          noexcept(traits::declval<Derived>().prev()) && traits::is_nothrow_copy_constructible<Derived>) {
        auto copy = Base::self();
        Base::prev();
        return copy;
      }
    };

    template <typename Derived, typename Type, typename Value, typename Reference, typename Pointer, typename DiffType>
    class RandomAccessIteratorBase
        : public BidirectionIteratorBase<Derived, Type, Value, Reference, Pointer, DiffType> {
      using Base = BidirectionIteratorBase<Derived, Type, Value, Reference, Pointer, DiffType>;

    public:
      using OpalRandomAccessIteratorTag = void;

      /// Advances the iterator `n` times
      ///
      /// \param n The number of elements to move forward in the iterator
      /// \return A reference to self
      constexpr Derived& operator+=(DiffType n) noexcept(noexcept(traits::declval<Derived>().move(n))) {
        Base::move(n);

        return Base::self();
      }

      /// Returns a new iterator that's `n` elements farther along in the range
      ///
      /// \param n The number of elements to move forward in the new iterator
      /// \return A new iterator
      [[nodiscard]] constexpr Derived operator+(DiffType n) const
          noexcept(noexcept(traits::declval<Derived>().move(n))) {
        auto copy = Base::self();
        copy.move(n);
        return copy;
      }

      /// Moves the iterator backwards `n` times
      ///
      /// \param n The number of elements to move backward in the iterator
      /// \return A reference to self
      constexpr Derived& operator-=(DiffType n) noexcept(noexcept(traits::declval<Derived>().move(n))) {
        Base::move(-n);

        return Base::self();
      }

      /// Returns a new iterator that's `n` elements farther back than the current value
      ///
      /// \param n The number of elements to move backward in the new iterator
      /// \return A new iterator
      [[nodiscard]] constexpr Derived operator-(DiffType n) const
          noexcept(noexcept(traits::declval<Derived>().move(n))) {
        auto copy = Base::self();
        copy.move(-n);
        return copy;
      }

      /// Gets the distance between `this` and `other`. Effectively `other.distance_to(this)`
      ///
      /// \param other The other iterator to compute distance with. Assumed to be **before** `*this`.
      /// \return A new iterator
      [[nodiscard]] constexpr DiffType operator-(const Derived& other) const
          noexcept(noexcept(other.distance_to(this->self()))) {
        return other.distance_to(Base::self());
      }

      /// Equivalent to `*(*this + n)`. Assumes `*this + n` is valid and dereferenceable
      ///
      /// \param n The number of elements to move forward/backward
      /// \return The result of `*(*this + n)`
      [[nodiscard]] constexpr Reference operator[](DiffType n) const
          noexcept(noexcept(traits::declval<Derived>().move(n)) && noexcept(traits::declval<Derived>().value())
                   && traits::is_nothrow_copy_constructible<Derived>) {
        return *(Base::self() + n);
      }

      /// Compares two iterators with `cmp`
      ///
      /// \param other The other iterator to compare
      /// \return The result of `lhs.cmp(rhs)`.
      [[nodiscard]] friend constexpr std::strong_ordering operator<=>(const Derived& lhs, const Derived& rhs) noexcept(
          noexcept(lhs.cmp(rhs))) {
        return lhs.cmp(rhs);
      }
    };

    template <typename I>
    concept DerivedFromRandomAccessIteratorBase = requires {
      typename I::OpalRandomAccessIteratorTag;
    };

    /// Returns a new iterator that's `n` elements farther along in the range
    ///
    /// \param n The number of elements to move forward in the new iterator
    /// \param iter The iter to move forward `n`
    /// \return A new iterator
    template <typename I>
    [[nodiscard]] constexpr I operator+(IterDifference<I> n, const I& iter) noexcept(noexcept(iter + n)) requires(
        RandomAccessIteratorReady<I, IterReference<I>, typename I::pointer, IterDifference<I>>) {
      return iter + n;
    }

    template <typename Derived, typename Type, typename Value, typename Reference, typename Pointer, typename DiffType>
    class ContiguousIteratorBase : public RandomAccessIteratorBase<Derived, Type, Value, Reference, Pointer, DiffType> {
      using Base = RandomAccessIteratorBase<Derived, Type, Value, Reference, Pointer, DiffType>;

    public:
      // see https://gcc.gnu.org/bugzilla/show_bug.cgi?id=96416
      //
      // seems to be a defect in the standard with regards to `std::to_address`
      using element_type = typename Base::value_type;

      /// Gets a pointer to the contiguous storage for `std::to_address`
      ///
      /// \return
      [[nodiscard]] Pointer operator->() const noexcept(noexcept(traits::declval<Derived>().address())) {
        return Base::address();
      }
    };

    template <typename Derived, typename Type, typename Value, typename Reference, typename Pointer, typename DiffType>
    class OutputIteratorBase : public InputIteratorBase<Derived, Type, Value, Reference, Pointer, DiffType> {};

    template <typename Derived, typename Type, typename Value, typename Reference, typename Pointer, typename DiffType>
    class IteratorInterfaceDifferentiator;

    template <typename Derived, typename Value, typename Reference, typename Pointer, typename DiffType>
    class IteratorInterfaceDifferentiator<Derived, internal::InputTag, Value, Reference, Pointer, DiffType>
        : public InputIteratorBase<Derived, internal::InputTag, Value, Reference, Pointer, DiffType> {};

    template <typename Derived, typename Value, typename Reference, typename Pointer, typename DiffType>
    class IteratorInterfaceDifferentiator<Derived, internal::ForwardTag, Value, Reference, Pointer, DiffType>
        : public ForwardIteratorBase<Derived, internal::ForwardTag, Value, Reference, Pointer, DiffType> {};

    template <typename Derived, typename Value, typename Reference, typename Pointer, typename DiffType>
    class IteratorInterfaceDifferentiator<Derived, internal::BidirectionTag, Value, Reference, Pointer, DiffType>
        : public BidirectionIteratorBase<Derived, internal::BidirectionTag, Value, Reference, Pointer, DiffType> {};

    template <typename Derived, typename Value, typename Reference, typename Pointer, typename DiffType>
    class IteratorInterfaceDifferentiator<Derived, internal::RandomTag, Value, Reference, Pointer, DiffType>
        : public RandomAccessIteratorBase<Derived, internal::RandomTag, Value, Reference, Pointer, DiffType> {};

    template <typename Derived, typename Value, typename Reference, typename Pointer, typename DiffType>
    class IteratorInterfaceDifferentiator<Derived, internal::ContiguousTag, Value, Reference, Pointer, DiffType>
        : public ContiguousIteratorBase<Derived, internal::ContiguousTag, Value, Reference, Pointer, DiffType> {};

    template <typename Derived, typename Value, typename Reference, typename Pointer, typename DiffType>
    class IteratorInterfaceDifferentiator<Derived, internal::OutputTag, Value, Reference, Pointer, DiffType>
        : public OutputIteratorBase<Derived, internal::OutputTag, Value, Reference, Pointer, DiffType> {};
  } // namespace internal

  /// Defines a reusable iterator interface that can be used to generate
  /// standards-compliant iterator types
  ///
  /// \tparam Derived The type defining the iterator's core operations
  /// \tparam Type An iterator tag for the type of iterator we want
  /// \tparam Value The value type that the iterator yields
  /// \tparam Reference The reference type for the iterator, usually `Value&`
  /// \tparam Pointer The pointer type for the iterator, usually `Value*`
  /// \tparam DiffType
  template <typename Derived,
      internal::IteratorTag Type,
      typename Value,
      typename Reference = Value&,
      typename Pointer = Value*,
      typename DiffType = frt::isize>
  class IteratorInterface
      : public internal::IteratorInterfaceDifferentiator<Derived, Type, Value, Reference, Pointer, DiffType> {
    using Base = internal::IteratorInterfaceDifferentiator<Derived, Type, Value, Reference, Pointer, DiffType>;

  protected:
    constexpr IteratorInterface() noexcept = default;

    constexpr IteratorInterface(const IteratorInterface&) noexcept = default;

    constexpr IteratorInterface(IteratorInterface&&) noexcept = default;

    constexpr IteratorInterface& operator=(const IteratorInterface&) noexcept = default;

    constexpr IteratorInterface& operator=(IteratorInterface&&) noexcept = default;
  };

  template <typename Derived,
      internal::IteratorTag Type,
      typename Value,
      typename Reference = Value,
      typename DiffType = frt::isize>
  using ProxyIteratorInterface = IteratorInterface<Derived, Type, Value, Reference, Reference*, DiffType>;
} // namespace frt