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

#include "../platform/compiler.h"
#include "../platform/macros.h"
#include "./basic.h"

namespace frt::traits {
  namespace internal {
    template <typename T> inline constexpr bool dependent_false = false;

    template <typename T, T Val> inline constexpr bool dependent_false_value = false;

    template <bool> struct Select { // Select between aliases that extract either their first or second parameter
      template <typename T1, typename> using Apply = T1;
    };

    template <> struct Select<false> { template <typename, typename T2> using Apply = T2; };
  } // namespace internal

  template <typename T, T Value> struct IntegralConstant {
    inline static constexpr T value = Value;

    constexpr T operator()() const noexcept {
      return value;
    }

    constexpr operator T() const noexcept { // NOLINT(google-explicit-constructor)
      return value;
    }
  };

  template <bool Value> using BoolConstant = IntegralConstant<bool, Value>;

  using TrueType = BoolConstant<true>;

  using FalseType = BoolConstant<false>;

  template <typename... Ts> inline constexpr bool conjunction = (Ts::value && ...);

  template <typename... Ts> inline constexpr bool disjunction = (Ts::value || ...);

  template <typename T> inline constexpr bool negation = !T::value;

  template <typename T> struct NegationTrait : BoolConstant<negation<T>> {};

  template <typename... Ts> struct DisjunctionTrait : BoolConstant<disjunction<Ts...>> {};

  template <> struct DisjunctionTrait<> : FalseType {};

  template <typename... Ts> struct ConjunctionTrait : BoolConstant<conjunction<Ts...>> {};

  template <typename T, typename U> inline constexpr bool is_same = __is_same(T, U);

  template <typename T, typename U> struct IsSameTrait : BoolConstant<is_same<T, U>> {};

  template <typename T> struct RemoveReferenceTrait { using type = T; };

  template <typename T> struct RemoveReferenceTrait<T&> { using type = T; };

  template <typename T> struct RemoveReferenceTrait<T&&> { using type = T; };

  template <typename T> using RemoveReference = typename RemoveReferenceTrait<T>::type;

  template <typename T> struct RemoveConstTrait { using type = T; };

  template <typename T> struct RemoveConstTrait<const T> { using type = T; };

  template <typename T> using RemoveConst = typename RemoveConstTrait<T>::type;

  template <typename T> struct RemoveVolatileTrait { using type = T; };

  template <typename T> struct RemoveVolatileTrait<volatile T> { using type = T; };

  template <typename T> using RemoveVolatile = typename RemoveVolatileTrait<T>::type;

  template <typename T> using RemoveCV = RemoveConst<RemoveVolatile<T>>;

  template <typename T> struct RemoveCVTrait { using type = RemoveCV<T>; };

  template <typename T> struct IsConstTrait : FalseType {};

  template <typename T> struct IsConstTrait<const T> : TrueType {};

  template <typename T> inline constexpr bool is_const = IsConstTrait<T>::value;

  template <typename T> inline constexpr bool is_lvalue_reference = false;

  template <typename T> inline constexpr bool is_lvalue_reference<T&> = true;

  template <typename T> struct IsLValueReferenceTrait : BoolConstant<is_lvalue_reference<T>> {};

  template <typename T> inline constexpr bool is_rvalue_reference = false;

  template <typename T> inline constexpr bool is_rvalue_reference<T&&> = true;

  template <typename T> struct IsRValueReferenceTrait : BoolConstant<is_rvalue_reference<T>> {};

  template <typename T> inline constexpr bool is_reference = is_lvalue_reference<T> || is_rvalue_reference<T>;

  template <typename T> struct IsReferenceTrait : BoolConstant<is_reference<T>> {};

  template <typename T> inline constexpr bool is_pointer = false;

  template <typename T> inline constexpr bool is_pointer<T*> = true;

  template <typename T> struct IsPointerTrait : BoolConstant<is_pointer<T*>> {};

  template <typename T> inline constexpr bool is_function = !(is_reference<T> || is_const<const T>);

  template <typename T> struct IsFunctionTrait : public BoolConstant<is_function<T>> {};

  template <typename T> inline constexpr bool is_unbounded_array = false;

  // NOLINTNEXTLINE(modernize-avoid-c-arrays)
  template <typename T> inline constexpr bool is_unbounded_array<T[]> = true;

  template <typename T> struct IsUnboundedArrayTrait : public BoolConstant<is_unbounded_array<T>> {};

  template <typename T> inline constexpr bool is_bounded_array = false;

  // NOLINTNEXTLINE(modernize-avoid-c-arrays)
  template <typename T, frt::isize N> inline constexpr bool is_bounded_array<T[N]> = true;

  template <typename T> struct IsBoundedArrayTrait : public BoolConstant<is_bounded_array<T>> {};

  template <typename T> inline constexpr bool is_array = is_unbounded_array<T> || is_bounded_array<T>;

  template <typename T> struct IsArrayTrait : BoolConstant<is_array<T>> {};

  template <typename T> inline constexpr bool is_void = is_same<RemoveCV<T>, void>;

  template <typename T> struct IsVoidTrait : BoolConstant<is_void<T>> {};

  template <typename T> inline constexpr bool is_null_pointer = is_same<T, frt::nullptr_t>;

  template <typename T> using TypeIdentity = T;

  template <typename T> struct TypeIdentityTrait { using type = TypeIdentity<T>; };

  template <typename T> T&& declval() noexcept;

  template <typename T> T declval() noexcept requires(is_void<T>);

  namespace internal {
    template <typename T> inline constexpr bool is_member_pointer_helper = false;

    template <typename T, typename U> inline constexpr bool is_member_pointer_helper<T U::*> = true;
  } // namespace internal

  template <typename T> inline constexpr bool is_member_pointer = internal::is_member_pointer_helper<RemoveCV<T>>;

  template <typename T> struct IsMemberPointerTrait : BoolConstant<is_member_pointer<T>> {};

  namespace internal {
    template <typename T> inline constexpr bool is_member_function_pointer_helper = false;

    template <typename T, typename U> inline constexpr bool is_member_function_pointer_helper<T U::*> = is_function<T>;
  } // namespace internal

  template <typename T>
  inline constexpr bool is_member_function_pointer = internal::is_member_function_pointer_helper<RemoveCV<T>>;

  template <typename T> struct IsMemberFunctionPointerTrait : BoolConstant<is_member_function_pointer<T>> {};

  template <typename T>
  inline constexpr bool is_member_object_pointer = is_member_pointer<T> && !is_member_function_pointer<T>;

  template <typename T> struct IsMemberObjectPointer : BoolConstant<is_member_object_pointer<T>> {};

  template <typename T> struct RemoveExtentTrait { using type = T; };

  // NOLINTNEXTLINE(modernize-avoid-c-arrays)
  template <typename T> struct RemoveExtentTrait<T[]> { using type = T; };

  // NOLINTNEXTLINE(modernize-avoid-c-arrays)
  template <typename T, frt::isize N> struct RemoveExtentTrait<T[N]> { using type = T; };

  template <typename T> using RemoveExtent = typename RemoveExtentTrait<T>::type;

  template <typename T> struct RemoveAllExtentsTrait { using type = T; };

  // NOLINTNEXTLINE(modernize-avoid-c-arrays)
  template <typename T> struct RemoveAllExtentsTrait<T[]> { using type = typename RemoveAllExtentsTrait<T>::type; };

  // NOLINTNEXTLINE(modernize-avoid-c-arrays)
  template <typename T, frt::isize N> struct RemoveAllExtentsTrait<T[N]> {
    using type = typename RemoveAllExtentsTrait<T>::type;
  };

  template <typename T> using RemoveAllExtents = typename RemoveAllExtentsTrait<T>::type;

  namespace internal {
    template <typename T, typename... Args>
    concept OneOf = sizeof...(Args) > 0 && (traits::is_same<T, Args> || ...);
  } // namespace internal

  template <typename T>
  inline constexpr bool is_integral = internal::OneOf<RemoveCV<T>,
      bool,
      char,
      unsigned char,
      signed char,
      unsigned short, // NOLINT(google-runtime-int)
      short,          // NOLINT(google-runtime-int)
      unsigned int,
      int,
      unsigned long,      // NOLINT(google-runtime-int)
      long,               // NOLINT(google-runtime-int)
      unsigned long long, // NOLINT(google-runtime-int)
      long long,          // NOLINT(google-runtime-int)
      __int128,
      unsigned __int128,
      char8_t,
      char16_t,
      char32_t,
      wchar_t>;

  template <typename T> struct IsIntegralTrait : BoolConstant<is_integral<T>> {};

  template <typename T>
  inline constexpr bool is_floating_point = internal::OneOf<RemoveCV<T>, float, double, long double>;

  template <typename T> struct IsFloatingPointTrait : BoolConstant<is_floating_point<T>> {};

  template <typename T> inline constexpr bool is_arithmetic = is_integral<T> || is_floating_point<T>;

  template <typename T> struct IsArithmeticTrait : BoolConstant<is_arithmetic<T>> {};

  template <typename T> inline constexpr bool is_fundamental = is_void<T> || is_null_pointer<T> || is_arithmetic<T>;

  template <typename T> struct IsFundamentalTrait : BoolConstant<is_arithmetic<T>> {};

  template <typename T> inline constexpr bool is_enum = __is_enum(T);

  template <typename T> struct IsEnumTrait : BoolConstant<is_enum<T>> {};

  template <typename T> inline constexpr bool is_union = __is_union(T);

  template <typename T> struct IsUnionTrait : BoolConstant<is_union<T>> {};

  template <typename T> inline constexpr bool is_class = __is_class(T);

  template <typename T> struct IsClassTrait : BoolConstant<is_class<T>> {};

  template <typename T>
  inline constexpr bool is_scalar =
      is_arithmetic<T> || is_pointer<T> || is_member_pointer<T> || is_enum<T> || is_null_pointer<T>;

  template <typename T> struct IsScalarTrait : BoolConstant<is_scalar<T>> {};

  template <typename T> inline constexpr bool is_object = !is_function<T> && !is_reference<T> && !is_void<T>;

  template <typename T> struct IsObjectTrait : BoolConstant<is_object<T>> {};

  template <typename T> inline constexpr bool is_compound = !is_arithmetic<T>;

  template <typename T> struct IsCompoundTrait : BoolConstant<is_compound<T>> {};

  namespace internal {
    template <typename T, size_t = sizeof(T)>
    constexpr TrueType is_complete_or_unbounded(TypeIdentityTrait<T> /*unused*/) {
      return {};
    }

    template <typename Identity, typename Nested = typename Identity::type>
    constexpr BoolConstant<is_reference<Nested> || is_function<Nested> || is_void<Nested> || is_unbounded_array<Nested>>
    is_complete_or_unbounded(Identity /*unused*/) {
      return {};
    }

    template <typename T> constexpr auto complete_or_unbounded() noexcept {
      static_assert(is_complete_or_unbounded(TypeIdentityTrait<T>{}),
          "argument must be a complete type, i.e something you can do `sizeof` on");

      return true;
    }
  } // namespace internal

  template <typename T> inline constexpr bool is_trivial = internal::complete_or_unbounded<T>() && __is_trivial(T);

  template <typename T> struct IsTrivialTrait : BoolConstant<is_trivial<T>> {};

  template <typename T>
  inline constexpr bool is_trivially_copyable = internal::complete_or_unbounded<T>() && __is_trivially_copyable(T);

  template <typename T> struct IsTriviallyCopyableTrait : BoolConstant<is_trivially_copyable<T>> {};

  template <typename T>
  inline constexpr bool is_standard_layout = internal::complete_or_unbounded<T>() && __is_standard_layout(T);

  template <typename T> struct IsStandardLayoutTrait : BoolConstant<is_standard_layout<T>> {};

  template <typename T>
  inline constexpr bool has_unique_object_representations = internal::complete_or_unbounded<T>()
                                                            && __has_unique_object_representations(
                                                                RemoveCV<RemoveAllExtents<T>>);

  template <typename T>
  struct HasUniqueObjectRepresentationsTrait : BoolConstant<has_unique_object_representations<T>> {};

  template <typename T> inline constexpr bool is_empty = internal::complete_or_unbounded<T>() && __is_empty(T);

  template <typename T> struct IsEmptyTrait : BoolConstant<is_empty<T>> {};

  template <typename T>
  inline constexpr bool is_polymorphic = internal::complete_or_unbounded<T>() && __is_polymorphic(T);

  template <typename T> struct IsPolymorphicTrait : BoolConstant<is_polymorphic<T>> {};

  template <typename T> inline constexpr bool is_final = internal::complete_or_unbounded<T>() && __is_final(T);

  template <typename T> struct IsFinalTrait : BoolConstant<is_final<T>> {};

  template <typename T> inline constexpr bool is_abstract = internal::complete_or_unbounded<T>() && __is_abstract(T);

  template <typename T> struct IsAbstractTrait : BoolConstant<is_abstract<T>> {};

  template <typename T> inline constexpr bool is_aggregate = internal::complete_or_unbounded<T>() && __is_aggregate(T);

  template <typename T> struct IsAggregateTrait : BoolConstant<is_aggregate<T>> {};

  template <typename T> inline constexpr bool is_signed = is_arithmetic<T>&& T(-1) < T(0);

  template <typename T> struct IsSignedTrait : BoolConstant<is_signed<T>> {};

  template <typename T> inline constexpr bool is_unsigned = is_arithmetic<T> && !is_signed<T>;

  template <typename T> struct IsUnsignedTrait : BoolConstant<is_unsigned<T>> {};

  namespace internal {
    template <typename From, typename To>
    struct IsConvertibleHelper : ConjunctionTrait<traits::IsVoidTrait<From>, traits::IsVoidTrait<To>> {};

    template <class From, class To>
    requires requires {
      static_cast<To (*)()>(nullptr);
      traits::declval<void (&)(To)>()(traits::declval<From>());
    }
    struct IsConvertibleHelper<From, To> : traits::TrueType {};

    template <typename From, typename To>
    struct IsNothrowConvertibleHelper : ConjunctionTrait<traits::IsVoidTrait<From>, traits::IsVoidTrait<To>> {};

    template <class From, class To>
    requires requires {
      static_cast<To (*)()>(nullptr);
      { traits::declval<void (&)(To) noexcept>()(traits::declval<From>()) }
      noexcept;
    }
    struct IsNothrowConvertibleHelper<From, To> : traits::TrueType {};
  } // namespace internal

  template <typename From, typename To>
  inline constexpr bool is_convertible = internal::IsConvertibleHelper<From, To>::value;

  template <typename From, typename To> struct IsConvertibleTrait : BoolConstant<is_convertible<From, To>> {};

  template <typename From, typename To>
  inline constexpr bool is_nothrow_convertible = internal::IsNothrowConvertibleHelper<From, To>::value;

  template <typename From, typename To>
  struct IsNothrowConvertibleTrait : BoolConstant<is_nothrow_convertible<From, To>> {};

  template <typename T> inline constexpr bool is_scoped_enum = is_enum<T> && !is_convertible<T, int>;

  template <typename T> struct IsScopedEnumTrait : BoolConstant<is_scoped_enum<T>> {};

  template <typename T> using AddConst = const T;

  template <typename T> struct AddConstTrait { using type = AddConst<T>; };

  template <typename T> using AddVolatile = volatile T;

  template <typename T> struct AddVolatileTrait { using type = AddVolatile<T>; };

  template <typename T> using AddCV = AddConst<AddVolatile<T>>;

  template <typename T> struct AddCVTrait { using type = AddCV<T>; };

  template <typename T> using AddLValueReference = T&;

  template <typename T> struct AddLValueReferenceTrait { using type = AddLValueReference<T>; };

  template <typename T> using AddRValueReference = T&&;

  template <typename T> struct AddRValueReferenceTrait { using type = AddLValueReference<T>; };

  template <typename T> struct RemovePointerTrait { using type = T; };

  template <typename T> struct RemovePointerTrait<T*> { using type = T; };

  template <typename T> using RemovePointer = typename RemovePointerTrait<T>::type;

  template <typename T> struct AddPointerTrait { using type = T*; };

  template <typename T> struct AddPointerTrait<T&> { using type = T*; };

  template <typename T> using AddPointer = typename AddPointerTrait<T>::type;

  namespace internal {
    template <frt::isize N> struct AssociatedWChar;

    template <> struct AssociatedWChar<8> { using type = frt::i8; };

    template <> struct AssociatedWChar<16> { using type = frt::i16; };

    template <> struct AssociatedWChar<32> { using type = frt::i32; };

    template <> struct AssociatedWChar<64> { using type = frt::i64; };

    using WCharSignedType = typename AssociatedWChar<sizeof(wchar_t) * CHAR_BIT>::type;
  } // namespace internal

  template <typename T> struct MakeSignedTrait {
    static_assert(is_integral<T>);

    using type = T;
  };

  template <> struct MakeSignedTrait<char> { using type = signed char; };

  template <> struct MakeSignedTrait<unsigned char> { using type = signed char; };

  // NOLINTNEXTLINE(google-runtime-int)
  template <> struct MakeSignedTrait<unsigned short> { using type = signed short; };

  template <> struct MakeSignedTrait<unsigned int> { using type = signed int; };

  // NOLINTNEXTLINE(google-runtime-int)
  template <> struct MakeSignedTrait<unsigned long> { using type = signed long; };

  // NOLINTNEXTLINE(google-runtime-int)
  template <> struct MakeSignedTrait<unsigned long long> { using type = signed long long; };

  template <> struct MakeSignedTrait<char8_t> { using type = frt::i8; };

  template <> struct MakeSignedTrait<char16_t> { using type = frt::i16; };

  template <> struct MakeSignedTrait<char32_t> { using type = frt::i32; };

  template <> struct MakeSignedTrait<wchar_t> { using type = internal::WCharSignedType; };

  template <typename T> using MakeSigned = typename MakeSignedTrait<T>::type;

  template <typename T> struct MakeUnsignedTrait { using type = T; };

  template <> struct MakeUnsignedTrait<char> { using type = unsigned char; };

  template <> struct MakeUnsignedTrait<signed char> { using type = unsigned char; };

  // NOLINTNEXTLINE(google-runtime-int)
  template <> struct MakeUnsignedTrait<short> { using type = unsigned short; };

  template <> struct MakeUnsignedTrait<int> { using type = unsigned int; };

  // NOLINTNEXTLINE(google-runtime-int)
  template <> struct MakeUnsignedTrait<long> { using type = unsigned long; };

  // NOLINTNEXTLINE(google-runtime-int)
  template <> struct MakeUnsignedTrait<long long> { using type = unsigned long long; };

  template <> struct MakeUnsignedTrait<char8_t> { using type = frt::i8; };

  template <> struct MakeUnsignedTrait<char16_t> { using type = frt::i16; };

  template <> struct MakeUnsignedTrait<char32_t> { using type = frt::i32; };

  template <> struct MakeUnsignedTrait<wchar_t> {
    using type = typename MakeUnsignedTrait<internal::WCharSignedType>::type;
  };

  template <typename T> using MakeUnsigned = typename MakeUnsignedTrait<T>::type;

  template <typename T, typename... Args>
  inline constexpr bool is_constructible = internal::complete_or_unbounded<T>() && __is_constructible(T, Args...);

  template <typename T, typename... Args> struct IsConstructibleTrait : BoolConstant<is_constructible<T, Args...>> {};

  template <typename T, typename... Args>
  inline constexpr bool is_trivially_constructible = internal::complete_or_unbounded<T>()
                                                     && __is_trivially_constructible(T, Args...);

  template <typename T, typename... Args>
  struct IsTriviallyConstructibleTrait : BoolConstant<is_trivially_constructible<T, Args...>> {};

  template <typename T, typename... Args>
  inline constexpr bool is_nothrow_constructible = internal::complete_or_unbounded<T>()
                                                   && __is_nothrow_constructible(T, Args...);

  template <typename T, typename... Args>
  struct IsNothrowConstructibleTrait : BoolConstant<is_nothrow_constructible<T, Args...>> {};

#define FRT_INTERNAL_IS_X_CONSTRUCTIBLE(name, pascal_name)                                                             \
  template <typename T> inline constexpr bool is_##name##_constructible = is_constructible<T FRT_INTERNAL_X>;          \
                                                                                                                       \
  template <typename T> struct Is##pascal_name##ConstructibleTrait : BoolConstant<is_##name##_constructible<T>> {};    \
                                                                                                                       \
  template <typename T>                                                                                                \
  inline constexpr bool is_trivially_##name##_constructible = is_trivially_constructible<T FRT_INTERNAL_X>;            \
                                                                                                                       \
  template <typename T>                                                                                                \
  struct IsTrivially##pascal_name##ConstructibleTrait : BoolConstant<is_trivially_##name##_constructible<T>> {};       \
                                                                                                                       \
  template <typename T>                                                                                                \
  inline constexpr bool is_nothrow_##name##_constructible = is_nothrow_constructible<T FRT_INTERNAL_X>;                \
                                                                                                                       \
  template <typename T>                                                                                                \
  struct IsNothrow##pascal_name##ConstructibleTrait : BoolConstant<is_nothrow_##name##_constructible<T>> {}

#define FRT_INTERNAL_X
  // is_default_constructible
  // is_trivially_default_constructible
  // is_nothrow_default_constructible
  FRT_INTERNAL_IS_X_CONSTRUCTIBLE(default, Default);
#undef FRT_INTERNAL_X
#define FRT_INTERNAL_X , const T&
  // is_copy_constructible
  // is_trivially_copy_constructible
  // is_nothrow_copy_constructible
  FRT_INTERNAL_IS_X_CONSTRUCTIBLE(copy, Copy);
#undef FRT_INTERNAL_X
#define FRT_INTERNAL_X , T&&
  // is_move_constructible
  // is_trivially_move_constructible
  // is_nothrow_move_constructible
  FRT_INTERNAL_IS_X_CONSTRUCTIBLE(move, Move);
#undef FRT_INTERNAL_X

#undef FRT_INTERNAL_IS_X_CONSTRUCTIBLE

  template <typename T, typename U>
  inline constexpr bool is_assignable = internal::complete_or_unbounded<T>() && __is_assignable(T, U);

  template <typename T, typename U> struct IsAssignableTrait : BoolConstant<is_assignable<T, U>> {};

  template <typename T, typename U>
  inline constexpr bool is_trivially_assignable = internal::complete_or_unbounded<T>()
                                                  && __is_trivially_assignable(T, U);

  template <typename T, typename U> struct IsTriviallyAssignableTrait : BoolConstant<is_trivially_assignable<T, U>> {};

  template <typename T, typename U>
  inline constexpr bool is_nothrow_assignable = internal::complete_or_unbounded<T>() && __is_nothrow_assignable(T, U);

  template <typename T, typename U> struct IsNothrowAssignableTrait : BoolConstant<is_nothrow_assignable<T, U>> {};

#define FRT_INTERNAL_IS_X_ASSIGNABLE(name, pascal_name)                                                                \
  template <typename T> inline constexpr bool is_##name##_assignable = is_assignable<T FRT_INTERNAL_X>;                \
                                                                                                                       \
  template <typename T> struct Is##pascal_name##AssignableTrait : BoolConstant<is_##name##_assignable<T>> {};          \
                                                                                                                       \
  template <typename T>                                                                                                \
  inline constexpr bool is_trivially_##name##_assignable = is_trivially_assignable<T FRT_INTERNAL_X>;                  \
                                                                                                                       \
  template <typename T>                                                                                                \
  struct IsTrivially##pascal_name##AssignableTrait : BoolConstant<is_trivially_##name##_assignable<T>> {};             \
                                                                                                                       \
  template <typename T>                                                                                                \
  inline constexpr bool is_nothrow_##name##_assignable = is_nothrow_assignable<T FRT_INTERNAL_X>;                      \
                                                                                                                       \
  template <typename T>                                                                                                \
  struct IsNothrow##pascal_name##AssignableTrait : BoolConstant<is_nothrow_##name##_assignable<T>> {}

#define FRT_INTERNAL_X , const T&
  // is_copy_assignable
  // is_trivially_copy_assignable
  // is_nothrow_copy_assignable
  FRT_INTERNAL_IS_X_ASSIGNABLE(copy, Copy);
#undef FRT_INTERNAL_X
#define FRT_INTERNAL_X , T&&
  // is_move_assignable
  // is_trivially_move_assignable
  // is_nothrow_move_assignable
  FRT_INTERNAL_IS_X_ASSIGNABLE(move, Move);
#undef FRT_INTERNAL_X

#undef FRT_INTERNAL_IS_X_ASSIGNABLE

  namespace internal {
    template <typename T>
    concept Destructible = requires(T t) {
      t.~T();
    };

    template <typename T>
    concept NothrowDestructible = requires(T t) {
      // clang-format off
      { t.~T() } noexcept;
      // clang-format on
    };

    template <typename T,
        bool = (is_void<T> || is_unbounded_array<T> || is_function<T>),
        bool = (is_reference<T> || is_scalar<T>)>
    struct IsDestructibleSafe;

    template <typename T>
    struct IsDestructibleSafe<T, false, false> : public BoolConstant<Destructible<RemoveAllExtents<T>>> {
      static_assert(internal::is_complete_or_unbounded(TypeIdentityTrait<T>{}));
    };

    template <typename T> struct IsDestructibleSafe<T, true, false> : public FalseType {
      static_assert(internal::is_complete_or_unbounded(TypeIdentityTrait<T>{}));
    };

    template <typename T> struct IsDestructibleSafe<T, false, true> : public TrueType {
      static_assert(internal::is_complete_or_unbounded(TypeIdentityTrait<T>{}));
    };

    template <typename T,
        bool = (is_void<T> || is_unbounded_array<T> || is_function<T>),
        bool = (is_reference<T> || is_scalar<T>)>
    struct IsNTDestructibleSafe;

    template <typename T>
    struct IsNTDestructibleSafe<T, false, false> : public BoolConstant<NothrowDestructible<RemoveAllExtents<T>>> {
      static_assert(internal::is_complete_or_unbounded(TypeIdentityTrait<T>{}));
    };

    template <typename T> struct IsNTDestructibleSafe<T, true, false> : public FalseType {
      static_assert(internal::is_complete_or_unbounded(TypeIdentityTrait<T>{}));
    };

    template <typename T> struct IsNTDestructibleSafe<T, false, true> : public TrueType {
      static_assert(internal::is_complete_or_unbounded(TypeIdentityTrait<T>{}));
    };
  } // namespace internal

  template <typename T> inline constexpr bool is_destructible = internal::IsNTDestructibleSafe<T>::value;

  template <typename T> struct IsDestructibleTrait : BoolConstant<is_destructible<T>> {};

  template <typename T>
  inline constexpr bool is_trivially_destructible = internal::IsDestructibleSafe<T>::value //
      && __is_trivially_destructible(T);

  template <typename T> struct IsTriviallyDestructibleTrait : BoolConstant<is_trivially_destructible<T>> {};

  template <typename T> inline constexpr bool is_nothrow_destructible = internal::IsNTDestructibleSafe<T>::value;

  template <typename T> struct IsNothrowDestructibleTrait : BoolConstant<is_nothrow_destructible<T>> {};

  template <typename T>
  inline constexpr bool has_virtual_destructor = internal::complete_or_unbounded<T>() && __has_virtual_destructor(T);

  template <typename T> inline constexpr frt::usize alignment_of = (internal::complete_or_unbounded<T>(), alignof(T));

  template <typename T> struct AlignmentOfTrait : IntegralConstant<frt::usize, alignment_of<T>> {};

  template <typename> struct RankTrait : public IntegralConstant<frt::usize, 0> {};

  template <typename T, frt::usize Size>
  struct RankTrait<T[Size]> : // NOLINT(modernize-avoid-c-arrays)
                              public IntegralConstant<frt::usize, 1 + RankTrait<T>::value> {};

  template <typename T>
  struct RankTrait<T[]> : // NOLINT(modernize-avoid-c-arrays)
                          public IntegralConstant<frt::usize, 1 + RankTrait<T>::value> {};

  template <typename, unsigned> struct ExtentTrait : public IntegralConstant<frt::usize, 0> {};

  template <typename T, unsigned U, frt::usize Size>
  struct ExtentTrait<T[Size], U> : // NOLINT(modernize-avoid-c-arrays)
                                   public IntegralConstant<frt::usize, U == 0 ? Size : ExtentTrait<T, U - 1>::value> {};

  template <typename T, unsigned U>
  struct ExtentTrait<T[], U> : // NOLINT(modernize-avoid-c-arrays)
                               public IntegralConstant<frt::usize, U == 0 ? 0 : ExtentTrait<T, U - 1>::value> {};

  template <typename T, unsigned U = 0>
  inline constexpr frt::usize extent = static_cast<frt::usize>(ExtentTrait<T, U>::value);

  template <typename Base, typename Derived> inline constexpr bool is_base_of = __is_base_of(Base, Derived);

  template <typename Base, typename Derived> struct IsBaseOfTrait : BoolConstant<is_base_of<Base, Derived>> {};

  // as of GCC 11.2, this doesn't exist. It seems to be committed for GCC 12, but that isn't a thing yet
#if __has_builtin(__is_layout_compatible)
  template <typename T, typename U>
  inline constexpr bool is_layout_compatible = internal::complete_or_unbounded<T>()
                                               && internal::complete_or_unbounded<U>() && __is_layout_compatible(T, U);

  template <typename T, typename U> struct IsLayoutCompatible : BoolConstant<is_layout_compatible<T, U>> {};
#endif

  template <frt::isize Len, frt::isize Align> struct AlignedStorageTrait {
    struct type {                          // NOLINT(readability-identifier-naming)
      alignas(Align) frt::ubyte data[Len]; // NOLINT(modernize-avoid-c-arrays)
    };
  };

  template <frt::isize Len, frt::isize Align> using AlignedStorage = typename AlignedStorageTrait<Len, Align>::type;

  template <typename T> using AlignedStorageFor = typename AlignedStorageTrait<sizeof(T), alignof(T)>::type;

  namespace internal {
    template <frt::isize... Values> struct TraitsMax;

    template <frt::isize A, frt::isize B, frt::isize... Values> struct TraitsMax<A, B, Values...> {
      inline static constexpr frt::isize value =
          (A > B) ? TraitsMax<A, Values...>::value : TraitsMax<B, Values...>::value;
    };

    template <frt::isize A> struct TraitsMax<A> { inline static constexpr frt::isize value = A; };

    template <frt::isize... Values> inline constexpr frt::isize traits_max = TraitsMax<Values...>::value;
  } // namespace internal

  template <frt::isize Len, typename... Ts> struct AlignedUnionTrait {
    inline static constexpr frt::isize alignment_value = internal::traits_max<sizeof(Ts)...>;

    // NOLINTNEXTLINE(readability-identifier-naming)
    struct type {
      // NOLINTNEXTLINE(modernize-avoid-c-arrays)
      alignas(alignment_value) frt::ubyte data[internal::traits_max<Len, alignment_value>];
    };
  };

  template <typename... Ts> using AlignedUnion = typename AlignedUnionTrait<0, Ts...>::type;

  template <bool B, typename T, typename F> struct ConditionalTrait { using type = T; };

  template <typename T, typename F> struct ConditionalTrait<false, T, F> { using type = F; };

  template <bool B, typename T, typename F> using Conditional = typename ConditionalTrait<B, T, F>::type;

  template <typename...> using Void = void;

  template <typename U, typename T, typename...> struct RebindTrait {
    static_assert(internal::dependent_false<T>, "illegal type for `rebind`");
  };

  template <typename U, template <typename, typename...> typename S, typename T, typename... Args>
  struct RebindTrait<U, S<T, Args...>> {
    using type = S<U, Args...>;
  };

  template <typename U, typename... Args> using Rebind = typename RebindTrait<U, Args...>::type;

  template <typename T> using RemoveCVRef = RemoveCV<RemoveReference<T>>;

  template <typename T> struct RemoveCVRefTrait { using type = RemoveCVRef<T>; };

  namespace internal {
    struct Two {
      char data[2]; // NOLINT(modernize-avoid-c-arrays)
    };

    struct IsReferenceableImpl {
      template <typename T> static T& test(int);

      template <typename T> static Two test(...);
    };

    template <typename T>
    inline constexpr bool is_referenceable = !is_same<decltype(IsReferenceableImpl::test<T>(0)), Two>;

    template <typename U, bool> struct DecayTraitImpl { using type = RemoveCV<U>; };

    template <typename U> struct DecayTraitImpl<U, true> {
    public:
      using type = Conditional<is_array<U>,
          typename RemoveExtentTrait<U>::type*,
          Conditional<is_function<U>, typename AddPointerTrait<U>::type, typename RemoveCVTrait<U>::type>>;
    };
  } // namespace internal

  template <typename T> struct DecayTrait {
  private:
    using U = RemoveReference<T>;

  public:
    using type = typename internal::DecayTraitImpl<U, internal::is_referenceable<U>>::type;
  };

  template <typename T> using Decay = typename DecayTrait<T>::type;

  template <bool B, typename T = void> struct EnableIfTrait {};

  template <typename T> struct EnableIfTrait<true, T> { using type = T; };

  template <bool B, typename T = void> using EnableIf = typename EnableIfTrait<B, T>::type;

  template <typename...> struct CommonTypeTrait {};

  template <typename T> struct CommonTypeTrait<T> : CommonTypeTrait<T, T> {};

  namespace internal {
    template <typename T1, typename T2> using If = decltype(false ? declval<T1>() : declval<T2>());

    template <typename, typename, typename = void> struct DecayConditionalResult {};

    template <typename T1, typename T2>
    struct DecayConditionalResult<T1, T2, Void<If<T1, T2>>> : DecayTrait<If<T1, T2>> {};

    template <typename T1, typename T2, typename = void> struct CommonType2Impl {};

    template <typename T1, typename T2>
    struct CommonType2Impl<T1, T2, Void<If<T1, T2>>> : DecayConditionalResult<T1, T2> {};
  } // namespace internal

  template <typename T1, typename T2>
  struct CommonTypeTrait<T1, T2> : Conditional<is_same<T1, Decay<T1>> && is_same<T2, Decay<T2>>,
                                       internal::CommonType2Impl<T1, T2>,
                                       CommonTypeTrait<Decay<T1>, Decay<T2>>> {};

  namespace internal {
    template <typename AlwaysVoid, typename T1, typename T2, typename... R> struct CommonTypeMultiImpl {};

    template <typename T1, typename T2, typename... R>
    struct CommonTypeMultiImpl<Void<typename CommonTypeTrait<T1, T2>::type>, T1, T2, R...>
        : CommonTypeTrait<typename CommonTypeTrait<T1, T2>::type, R...> {};
  } // namespace internal

  template <typename T1, typename T2, typename... R>
  requires requires {
    typename internal::CommonTypeMultiImpl<void, T1, T2, R...>;
  }
  struct CommonTypeTrait<T1, T2, R...> : internal::CommonTypeMultiImpl<void, T1, T2, R...> {};

  template <typename... Ts> using CommonType = typename CommonTypeTrait<Ts...>::type;

  template <typename, typename, template <typename> typename, template <typename> typename>
  struct BasicCommonReference {};

  namespace internal {
    template <typename From> struct CopyCvImpl { template <typename To> using type = To; };

    template <typename From> struct CopyCvImpl<const From> { template <typename To> using type = const To; };

    template <typename From> struct CopyCvImpl<volatile From> { template <typename To> using type = volatile To; };

    template <typename From> struct CopyCvImpl<const volatile From> {
      template <typename To> using type = const volatile To;
    };

    template <typename From, typename To> using CopyCv = typename CopyCvImpl<From>::template type<To>;

    template <typename From> struct CopyQualifiersImpl { template <typename To> using type = CopyCv<From, To>; };

    template <typename From> struct CopyQualifiersImpl<From&> {
      template <typename To> using type = traits::AddLValueReference<CopyCv<From, To>>;
    };

    template <typename From> struct CopyQualifiersImpl<From&&> {
      template <typename To> using type = traits::AddRValueReference<CopyCv<From, To>>;
    };

    template <typename From> using CopyQualifiers = typename internal::CopyQualifiersImpl<From>;

    template <typename T1, typename T2, class = void> struct CommonReferenceImplTwoCommon : CommonTypeTrait<T1, T2> {};

    template <typename T1, typename T2> struct CommonReferenceImplTwoCommon<T1, T2, Void<If<T1, T2>>> {
      using type = If<T1, T2>;
    };

    template <typename T1, typename T2>
    using BasicCommonReferenceSpecialization = typename BasicCommonReference<RemoveCVRef<T1>,
        RemoveCVRef<T2>,
        CopyQualifiers<T1>::template type,
        CopyQualifiers<T2>::template type>::type;

    template <typename T1, typename T2, class = void>
    struct CommonReferenceImplTwoBasic : CommonReferenceImplTwoCommon<T1, T2> {};

    template <typename T1, typename T2>
    struct CommonReferenceImplTwoBasic<T1, T2, Void<BasicCommonReferenceSpecialization<T1, T2>>> {
      using type = BasicCommonReferenceSpecialization<T1, T2>;
    };

    template <typename T1,
        typename T2,
        typename Result = If<CopyCv<T1, T2>&, CopyCv<T2, T1>&>,
        EnableIf<is_lvalue_reference<Result>, int> = 0>
    using LValueCommonRef = Result;

    template <typename T1, typename T2> using RValueCommonRef = RemoveReference<LValueCommonRef<T1, T2>>&&;

    template <typename T1, typename T2, class = void>
    struct CommonReferenceImplTwo : CommonReferenceImplTwoBasic<T1, T2> {};

    template <typename T1, typename T2> struct CommonReferenceImplTwo<T1&, T2&, Void<LValueCommonRef<T1, T2>>> {
      using type = LValueCommonRef<T1, T2>;
    };

    template <typename T1, typename T2>
    struct CommonReferenceImplTwo<T1&&, T2&, EnableIf<is_convertible<T1&&, LValueCommonRef<const T1, T2>>>> {
      using type = LValueCommonRef<const T1, T2>;
    };

    template <typename T1, typename T2>
    struct CommonReferenceImplTwo<T1&, T2&&, EnableIf<is_convertible<T2&&, LValueCommonRef<const T2, T1>>>> {
      using type = LValueCommonRef<const T2, T1>;
    };

    template <typename T1, typename T2>
    struct CommonReferenceImplTwo<T1&&,
        T2&&,
        EnableIf<is_convertible<T1&&, RValueCommonRef<T1, T2>> && is_convertible<T1&&, RValueCommonRef<T1, T2>>>> {
      using type = RValueCommonRef<T1, T2>;
    };

    template <typename...> struct CommonReferenceImpl;

    template <> struct CommonReferenceImpl<> {};

    template <typename T1> struct CommonReferenceImpl<T1> : TypeIdentityTrait<T1> {};

    template <typename T1, typename T2> struct CommonReferenceImpl<T1, T2> : CommonReferenceImplTwo<T1, T2> {};

    template <typename AlwaysVoid, typename T1, typename T2, typename... Types> struct FoldCommonReference {};

    template <typename T1, typename T2, typename... Types>
    struct FoldCommonReference<Void<typename CommonReferenceImpl<T1, T2>::type>, T1, T2, Types...>
        : CommonReferenceImpl<typename CommonReferenceImpl<T1, T2>::type, Types...> {};

    template <typename T1, typename T2, typename T3, typename... Types>
    struct CommonReferenceImpl<T1, T2, T3, Types...> : FoldCommonReference<void, T1, T2, T3, Types...> {};
  } // namespace internal

  template <typename... Ts> using CommonReference = typename internal::CommonReferenceImpl<Ts...>::type;

  template <typename... Ts> struct CommonReferenceTrait : internal::CommonReferenceImpl<Ts...> {};

  template <typename T> struct UnderlyingTypeTrait {};

  template <typename T>
  requires(traits::is_enum<T>) struct UnderlyingTypeTrait<T> {
    using type = __underlying_type(T);
  };

  template <typename T> using UnderlyingType = typename UnderlyingTypeTrait<T>::type;

  [[nodiscard]] FRT_ALWAYS_INLINE constexpr bool is_constant_evaluated() noexcept {
#if defined(__cpp_if_consteval) && __cpp_if_consteval >= 202106L
    // clang-format off
    if consteval { return true; } else { return false; }
    // clang-format on
#else
    return __builtin_is_constant_evaluated();
#endif
  }
} // namespace frt::traits