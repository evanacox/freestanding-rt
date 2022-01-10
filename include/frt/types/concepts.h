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

#include "./forward.h"
#include "./traits.h"

namespace frt {
  /// Checks if two types are the same
  ///
  /// \tparam T The first type to check
  /// \tparam U The second type to check
  template <typename T, typename U>
  concept SameAs = traits::is_same<T, U>;

  /// Checks if type `T` is one of the types given in `Args`
  ///
  /// \tparam T The type to look for
  /// \tparam Args The types to look for `T` in
  template <typename T, typename... Args>
  concept OneOf = traits::internal::OneOf<T, Args...>;

  /// Checks if a type is derived from another
  ///
  /// Semantic requirements: see https://en.cppreference.com/w/cpp/concepts/derived_from
  ///
  /// \tparam Derived The type to check
  /// \tparam Base The type that `Derived` is supposed to be derived from
  template <typename Derived, typename Base>
  concept DerivedFrom =
      traits::is_base_of<Base, Derived> && traits::is_convertible<const volatile Derived*, const volatile Base*>;

  /// Checks if `From` can be implicitly and explicitly converted to `To`
  ///
  /// Semantic requirements: see https://en.cppreference.com/w/cpp/concepts/convertible_to
  ///
  /// \tparam From The type being converted from
  /// \tparam To The type being converted to
  template <typename From, typename To>
  concept ConvertibleTo = traits::is_convertible<From, To> && requires {
    static_cast<To>(traits::declval<From>());
  };

  /// Checks if a type has a common reference type with another type
  ///
  /// Semantic requirements: see https://en.cppreference.com/w/cpp/concepts/common_reference_with
  ///
  /// \tparam T The first type
  /// \tparam U The second type
  template <typename T, typename U>
  concept CommonReferenceWith = SameAs<traits::CommonReference<T, U>, traits::CommonReference<U, T>> && ConvertibleTo<T,
      traits::CommonReference<T, U>> && ConvertibleTo<U, traits::CommonReference<T, U>>;

  /// Checks that two types share a common type that both of them can be converted to
  ///
  /// Semantic requirements: see https://en.cppreference.com/w/cpp/concepts/common_with
  ///
  /// \tparam T The first type
  /// \tparam U The second type
  template <typename T, typename U>
  concept CommonWith = SameAs<traits::CommonType<T, U>, traits::CommonType<U, T>> //
      && CommonReferenceWith<traits::AddLValueReference<const T>,
          traits::AddLValueReference<const U>> //
      && CommonReferenceWith<traits::AddLValueReference<traits::CommonType<T, U>>,
          traits::CommonReference<traits::AddLValueReference<const T>,
              traits::AddLValueReference<const U>>> //
      && requires {
    static_cast<traits::CommonType<T, U>>(traits::declval<T>());
    static_cast<traits::CommonType<T, U>>(traits::declval<U>());
  };

  /// Checks if a given type is one of C++'s "arithmetic" types, i.e
  /// integral or floating-point
  ///
  /// \tparam T The type to check
  template <typename T>
  concept Arithmetic = traits::is_arithmetic<T>;

  /// Checks if a given type is one of C++'s "integral" types, i.e
  /// `bool`, a char type, or one of the integer types
  ///
  /// \tparam T The type to check
  template <typename T>
  concept Integral = traits::is_integral<T>;

  /// Checks if a given type is both one of the integral types and
  /// is one of the *unsigned* integral types
  ///
  /// \tparam T The type to check
  template <typename T>
  concept UnsignedIntegral = Integral<T> && traits::is_unsigned<T>;

  /// Checks if a given type is a signed arithmetic type
  ///
  /// \tparam T The type to check
  template <typename T>
  concept Signed = traits::is_signed<T>;

  /// Checks if a given type is a signed integral type
  ///
  /// \tparam T The type to check
  template <typename T>
  concept SignedIntegral = Integral<T> && traits::is_signed<T>;

  /// Checks if a given type is one of the floating-point types
  ///
  /// \tparam T The type to check
  template <typename T>
  concept FloatingPoint = traits::is_floating_point<T>;

  /// Checks that an object of type `LHS` can be assigned to from `RHS`.
  ///
  /// Semantic requirements: see https://en.cppreference.com/w/cpp/concepts/assignable_from
  ///
  /// \tparam LHS The left hand side of the assignment
  /// \tparam RHS The right hand side of the assignment
  template <typename LHS, typename RHS>
  concept AssignableFrom = traits::is_lvalue_reference<LHS> && CommonReferenceWith<
      traits::AddLValueReference<const traits::RemoveReference<LHS>>,
      traits::AddLValueReference<const traits::RemoveReference<RHS>>> && requires(LHS lhs, RHS&& rhs) {
    { lhs = frt::forward<RHS>(rhs) } -> SameAs<LHS>;
  };

  /// Checks that a type is destructible. Also requires that the destructor is `noexcept`
  ///
  /// \tparam T The type to check
  template <typename T>
  concept Destructible = traits::is_nothrow_destructible<T>;

  /// Checks if a type is constructible and destructible given a set of arguments
  ///
  /// \tparam T The type to check
  /// \tparam Args The arguments for the constructor
  template <typename T, typename... Args>
  concept ConstructibleFrom = Destructible<T> && traits::is_constructible<T, Args...>;

  /// Checks if a type can be value-initialized, direct-list-initialized and default-initialized.
  ///
  /// \tparam T The type to check
  template <typename T>
  concept DefaultInitializable = ConstructibleFrom<T> && requires {
    T{};
    ::new (static_cast<void*>(nullptr)) T; // `T t;` is well-formed
  };

  /// Checks if a type is move-constructible
  ///
  /// Semantic requirements: see https://en.cppreference.com/w/cpp/concepts/move_constructible
  ///
  /// \tparam T The type to check
  template <typename T>
  concept MoveConstructible = ConstructibleFrom<T, T> && ConvertibleTo<T, T>;

  /// Checks that a type is copy-constructible
  ///
  /// Semantic requirements: see https://en.cppreference.com/w/cpp/concepts/copy_constructible
  ///
  /// \tparam T
  template <typename T>
  concept CopyConstructible =
      MoveConstructible<T> && ConstructibleFrom<T, T&> && ConvertibleTo<T&, T> && ConstructibleFrom<T,
          const T&> && ConvertibleTo<const T&, T> && ConstructibleFrom<T, const T> && ConvertibleTo<const T, T>;

  /// Models a type that is able to be converted to `bool` and has "normal" logical operators
  ///
  /// Semantic requirements: see https://en.cppreference.com/w/cpp/concepts/boolean-testable
  ///
  /// \tparam T
  template <typename T>
  concept __BooleanTestable = ConvertibleTo<T, bool> && requires(T&& t) {
    { !frt::forward<T>(t) } -> ConvertibleTo<bool>;
  };

  /// Specifies that comparing type `T` and `U` with equality operators (in either order)
  /// yield meaningful and consistent results
  ///
  /// Semantic requirements: see https://en.cppreference.com/w/cpp/concepts/equality_comparable
  ///
  /// \tparam T The first type to check
  /// \tparam U The second type to check
  template <typename T, typename U>
  concept WeaklyEqualityComparableWith = requires(const traits::RemoveReference<T>& t,
      const traits::RemoveReference<U>& u) {
    { t == u } -> __BooleanTestable;
    { t != u } -> __BooleanTestable;
    { u == t } -> __BooleanTestable;
    { u != t } -> __BooleanTestable;
  };

  /// Specifies that comparing instances of type `T` produce meaningful and consistent results
  ///
  /// Semantic requirements: see https://en.cppreference.com/w/cpp/concepts/equality_comparable
  ///
  /// \tparam T The type to check
  template <typename T>
  concept EqualityComparable = WeaklyEqualityComparableWith<T, T>;

  /// Specifies that comparing type `T` and `U` with equality operators (in either order)
  /// yield meaningful and consistent results as-if they were being compared after being converted to their common type
  ///
  /// Semantic requirements: see https://en.cppreference.com/w/cpp/concepts/equality_comparable
  ///
  /// \tparam T The first type to check
  /// \tparam U The second type to check
  template <typename T, typename U>
  concept EqualityComparableWith = EqualityComparable<T> && EqualityComparable<U> && CommonReferenceWith<
      traits::AddLValueReference<const traits::RemoveReference<T>>,
      traits::AddLValueReference<const traits::RemoveReference<U>>> && EqualityComparable<traits::
          CommonReference<traits::AddLValueReference<const traits::RemoveReference<T>>,
              traits::AddLValueReference<const traits::RemoveReference<U>>>> && WeaklyEqualityComparableWith<T, U>;

  /// Specifies that comparing type `T` and `U` with comparison operators (in either order)
  /// yield meaningful and consistent results
  ///
  /// Semantic requirements: see https://en.cppreference.com/w/cpp/concepts/totally_ordered
  ///
  /// \tparam T The first type to check
  /// \tparam U The second type to check
  template <typename T, typename U>
  concept __PartiallyOrderedWith = requires(traits::AddLValueReference<const traits::RemoveReference<T>> t,
      traits::AddLValueReference<const traits::RemoveReference<U>> u) {
    { t < u } -> __BooleanTestable;
    { t > u } -> __BooleanTestable;
    { t <= u } -> __BooleanTestable;
    { t >= u } -> __BooleanTestable;
    { u < t } -> __BooleanTestable;
    { u > t } -> __BooleanTestable;
    { u <= t } -> __BooleanTestable;
    { u >= t } -> __BooleanTestable;
  };

  /// Specifies that objects of type `T` can be compared using comparison operators, and their results
  /// are meaningful and consistent.
  ///
  /// Semantic requirements: see https://en.cppreference.com/w/cpp/concepts/totally_ordered
  ///
  /// \tparam T The first type
  template <typename T>
  concept TotallyOrdered = EqualityComparable<T> && __PartiallyOrderedWith<T, T>;

  /// Specifies that objects of type `T` and objects of type `U` can be compared using comparison operators, and their
  /// results are meaningful and consistent. If they are compared in mixed-form, the results are equivalent to
  /// converting them into a common type and comparing.
  ///
  /// Semantic requirements: see https://en.cppreference.com/w/cpp/concepts/totally_ordered
  ///
  /// \tparam T The first type
  /// \tparam U The second type
  template <typename T, typename U>
  concept TotallyOrderedWith = TotallyOrdered<T> && TotallyOrdered<U> && EqualityComparableWith<T,
      U> && TotallyOrdered<traits::CommonReference<traits::AddLValueReference<const traits::RemoveReference<T>>,
      traits::AddLValueReference<const traits::RemoveReference<U>>>> && __PartiallyOrderedWith<T, U>;

  /// Checks if a type is trivially copyable
  ///
  /// \tparam T The type to check
  template <typename T>
  concept TriviallyCopyable = traits::is_trivially_copyable<T>;
} // namespace frt