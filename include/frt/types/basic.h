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

#include <limits.h>
#include <stddef.h>
#include <stdint.h>

namespace frt {
  // traits.h and concepts.h rely on this header, so we can't use the `SameAs` from there
  namespace internal {
    template <typename, typename> inline constexpr bool basic_same_as = false;

    template <typename T> inline constexpr bool basic_same_as<T, T> = true;
  } // namespace internal

  static_assert(
      internal::basic_same_as<size_t, uintptr_t> // shouldn't be a difference between these besides semantic meaning
          && internal::basic_same_as<ptrdiff_t, intptr_t> // same as above
          && sizeof(size_t) == sizeof(ptrdiff_t)          // should be signed/unsigned versions of same type
          && CHAR_BIT == 8,                               // 8-bit bytes are an implicit assumption
      "frt: platform needs to be reasonable");

  using size = intptr_t;

  using usize = uintptr_t;

  using byte = unsigned char; // need to not violate aliasing rules

  using ibyte = signed char;

  using nullptr_t = decltype(nullptr);

  using u8 = byte;

  using u16 = uint16_t;

  using u32 = uint32_t;

  using u64 = uint64_t;

#if defined(FRT_SUPPORTS_INT128)
  using u128 = unsigned __int128;
#endif

  using uleast8 = uint_least8_t;

  using uleast16 = uint_least16_t;

  using uleast32 = uint_least32_t;

  using uleast64 = uint_least64_t;

  using ufast8 = uint_fast8_t;

  using ufast16 = uint_fast16_t;

  using ufast32 = uint_fast32_t;

  using ufast64 = uint_fast64_t;

  using i8 = byte;

  using i16 = int16_t;

  using i32 = int32_t;

  using i64 = int64_t;

#if defined(FRT_SUPPORTS_INT128)
  using i128 = __int128;
#endif

  using ileast8 = int_least8_t;

  using ileast16 = int_least16_t;

  using ileast32 = int_least32_t;

  using ileast64 = int_least64_t;

  using ifast8 = int_fast8_t;

  using ifast16 = int_fast16_t;

  using ifast32 = int_fast32_t;

  using ifast64 = int_fast64_t;
} // namespace frt