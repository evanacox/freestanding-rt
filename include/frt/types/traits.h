//======---------------------------------------------------------------======//
//                                                                           //
// Copyright 2021 Evan Cox <evanacox00@gmail.com>. All rights reserved.      //
//                                                                           //
// Use of this source code is governed by a BSD-style license that can be    //
// found in the LICENSE.txt file at the root of this project, or at the      //
// following link: https://opensource.org/licenses/BSD-3-Clause              //
//                                                                           //
//======---------------------------------------------------------------======//

#pragma once

#include "../platform/compiler.h"
#include "./basic.h"
#include <limits.h>

namespace frt::traits {
  namespace internal {
    template <typename T> inline constexpr bool dependent_false = false;

    template <typename T, T Val> inline constexpr bool dependent_false_value = false;
  } // namespace internal

  template <typename T, T Value> struct IntegralConstant { inline static constexpr auto value = Value; };

  template <bool Value> using BoolConstant = IntegralConstant<bool, Value>;

  using TrueType = BoolConstant<true>;

  using FalseType = BoolConstant<false>;

  template <typename T> struct Negation : BoolConstant<!T::value> {};

  template <typename... Ts> struct Disjunction : BoolConstant<(Ts::value || ...)> {};

  template <> struct Disjunction<> : FalseType {};

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

  template <typename T> inline constexpr bool is_const = is_same<T, const T>;

  template <typename T> struct IsConstTrait : BoolConstant<is_const<T>> {};

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

  template <typename T> inline constexpr bool is_function = !is_reference<T> && !is_const<const T>;

  template <typename T> struct IsFunctionTrait : public BoolConstant<is_function<T>> {};

  template <typename T> inline constexpr bool is_unbounded_array = false;

  // NOLINTNEXTLINE(modernize-avoid-c-arrays)
  template <typename T> inline constexpr bool is_unbounded_array<T[]> = true;

  template <typename T> struct IsUnboundedArrayTrait : public BoolConstant<is_unbounded_array<T>> {};

  template <typename T> inline constexpr bool is_bounded_array = false;

  // NOLINTNEXTLINE(modernize-avoid-c-arrays)
  template <typename T, frt::size N> inline constexpr bool is_bounded_array<T[N]> = true;

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
  template <typename T, frt::size N> struct RemoveExtentTrait<T[N]> { using type = T; };

  template <typename T> using RemoveExtent = typename RemoveExtentTrait<T>::type;

  template <typename T> struct RemoveAllExtentsTrait { using type = T; };

  // NOLINTNEXTLINE(modernize-avoid-c-arrays)
  template <typename T> struct RemoveAllExtentsTrait<T[]> { using type = typename RemoveAllExtentsTrait<T>::type; };

  // NOLINTNEXTLINE(modernize-avoid-c-arrays)
  template <typename T, frt::size N> struct RemoveAllExtentsTrait<T[N]> {
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
#if defined(FRT_SUPPORTS_INT128)
      __int128,
      unsigned __int128,
#endif
      char8_t,
      char16_t,
      char32_t,
      wchar_t>;

  template <typename T> struct IsIntegralTrait : BoolConstant<is_integral<T>> {};

  template <typename T>
  inline constexpr bool is_floating_point = internal::OneOf<RemoveCV<T>, float, double, long double, __float128>;

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

  template <typename T> inline constexpr bool is_trivial = __is_trivial(T);

  template <typename T> struct IsTrivialTrait : BoolConstant<is_trivial<T>> {};

  template <typename T> inline constexpr bool is_trivially_copyable = __is_trivially_copyable(T);

  template <typename T> struct IsTriviallyCopyableTrait : BoolConstant<is_trivially_copyable<T>> {};

  template <typename T> inline constexpr bool is_standard_layout = __is_standard_layout(T);

  template <typename T> struct IsStandardLayoutTrait : BoolConstant<is_standard_layout<T>> {};

  template <typename T>
  inline constexpr bool has_unique_object_representations = __has_unique_object_representations(
      RemoveCV<RemoveAllExtents<T>>);

  template <typename T>
  struct HasUniqueObjectRepresentationsTrait : BoolConstant<has_unique_object_representations<T>> {};

  template <typename T> inline constexpr bool is_empty = __is_empty(T);

  template <typename T> struct IsEmptyTrait : BoolConstant<is_empty<T>> {};

  template <typename T> inline constexpr bool is_polymorphic = __is_polymorphic(T);

  template <typename T> struct IsPolymorphicTrait : BoolConstant<is_polymorphic<T>> {};

  template <typename T> inline constexpr bool is_final = __is_final(T);

  template <typename T> struct IsFinalTrait : BoolConstant<is_final<T>> {};

  template <typename T> inline constexpr bool is_abstract = __is_abstract(T);

  template <typename T> struct IsAbstractTrait : BoolConstant<is_abstract<T>> {};

  template <typename T, bool = is_arithmetic<T>> inline constexpr bool is_signed = false;

  template <typename T> inline constexpr bool is_signed<T, true> = T(-1) < T(0);

  template <typename T> struct IsSignedTrait : BoolConstant<is_signed<T>> {};

  template <typename T> inline constexpr bool is_unsigned = is_arithmetic<T> && !is_signed<T>;

  template <typename T> struct IsUnsignedTrait : BoolConstant<is_unsigned<T>> {};

  namespace internal {
    template <typename From, typename To, bool = is_void<From> || is_function<To> || is_array<To>>
    struct IsConvertibleHelper {
      using type = BoolConstant<is_void<To>>;
    };

    template <typename From, typename To> class IsConvertibleHelper<From, To, false> {
      template <typename To1> static void test1(To1) noexcept;

      template <typename From1, typename To1, typename = decltype(test1<To1>(traits::declval<From1>()))>
      static TrueType test2(int);

      template <typename, typename> static FalseType test2(...);

    public:
      using type = decltype(test2<From, To>(0));
    };
  } // namespace internal

  template <typename From, typename To>
  inline constexpr bool is_convertible = internal::IsConvertibleHelper<From, To>::type::value;

  template <typename From, typename To> struct IsConvertibleTrait : BoolConstant<is_convertible<From, To>> {};

  template <typename T> inline constexpr bool is_scoped_enum = is_enum<T> && !is_convertible<T, int>;

  template <typename T> struct IsScopedEnumTrait : BoolConstant<is_scoped_enum<T>> {};

  template <typename T> using AddConst = const T;

  template <typename T> struct AddConstTrait { using type = AddConst<T>; };

  template <typename T> using AddVolatile = volatile T;

  template <typename T> struct AddVolatileTrait { using type = AddVolatile<T>; };

  template <typename T> using AddCV = AddConst<AddVolatile<T>>;

  template <typename T> using AddLValueReference = T&;

  template <typename T> struct AddLValueReferenceTrait { using type = AddLValueReference<T>; };

  template <typename T> using AddRValueReference = T&&;

  template <typename T> struct AddRValueReferenceTrait { using type = AddLValueReference<T>; };

  template <typename T> struct RemovePointerTrait { using type = T; };

  template <typename T> struct RemovePointerTrait<T*> { using type = T; };

  template <typename T> using RemovePointer = typename RemovePointerTrait<T>::type;

  template <typename T> using AddPointer = T*;

  template <typename T> struct AddPointerTrait { using type = AddPointer<T>; };

  namespace internal {
    template <frt::size Size> struct AssociatedWChar;

    template <> struct AssociatedWChar<8> { using type = frt::i8; };

    template <> struct AssociatedWChar<16> { using type = frt::i16; };

    template <> struct AssociatedWChar<32> { using type = frt::i32; };

    template <> struct AssociatedWChar<64> { using type = frt::i64; };
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

  template <> struct MakeSignedTrait<char8_t> { using type = frt::u8; };

  template <> struct MakeSignedTrait<char16_t> { using type = frt::u16; };

  template <> struct MakeSignedTrait<char32_t> { using type = frt::u32; };

  template <> struct MakeSignedTrait<wchar_t> {
    using type = typename MakeSignedTrait<typename internal::AssociatedWChar<sizeof(wchar_t) * CHAR_BIT>::type>::type;
  };

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
    using type = typename MakeUnsignedTrait<typename internal::AssociatedWChar<sizeof(wchar_t) * CHAR_BIT>::type>::type;
  };

  template <typename T> using MakeUnsigned = typename MakeUnsignedTrait<T>::type;

  template <frt::size Len, frt::size Align> struct AlignedStorageTrait {
    // NOLINTNEXTLINE(readability-identifier-naming)
    struct type {
      // NOLINTNEXTLINE(modernize-avoid-c-arrays)
      alignas(Align) frt::byte data[Len];
    };
  };

  template <frt::size Len, frt::size Align> using AlignedStorage = typename AlignedStorageTrait<Len, Align>::type;

  template <typename T> using AlignedStorageFor = typename AlignedStorageTrait<sizeof(T), alignof(T)>::type;

  namespace internal {
    template <frt::size... Values> struct TraitsMax;

    template <frt::size A, frt::size B, frt::size... Values> struct TraitsMax<A, B, Values...> {
      inline static constexpr frt::size value =
          (A > B) ? TraitsMax<A, Values...>::value : TraitsMax<B, Values...>::value;
    };

    template <frt::size A> struct TraitsMax<A> { inline static constexpr frt::size value = A; };

    template <frt::size... Values> inline constexpr frt::size traits_max = TraitsMax<Values...>::value;
  } // namespace internal

  template <frt::size Len, typename... Ts> struct AlignedUnionTrait {
    inline static constexpr frt::size alignment_value = internal::traits_max<sizeof(Ts)...>;

    // NOLINTNEXTLINE(readability-identifier-naming)
    struct type {
      // NOLINTNEXTLINE(modernize-avoid-c-arrays)
      alignas(alignment_value) frt::byte data[internal::traits_max<Len, alignment_value>];
    };
  };

  template <typename... Ts> using AlignedUnion = typename AlignedUnionTrait<0, Ts...>::type;

  template <bool B, typename T, typename F> struct ConditionalTrait { using type = T; };

  template <typename T, typename F> struct ConditionalTrait<false, T, F> { using type = F; };

  template <bool B, typename T, typename F> using Conditional = typename ConditionalTrait<B, T, F>::type;
} // namespace frt::traits