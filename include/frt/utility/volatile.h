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

#include "../types/concepts.h"

namespace frt {
  /// Forces the compiler to perform a read to `address`. Internally, casts `address`
  /// into a `const volatile T*` and reads from it.
  ///
  /// \param address The address to read from
  /// \return The value at `*address`
  template <TriviallyCopyable T> [[nodiscard]] T volatile_read(const T* address) noexcept {
    return *static_cast<const volatile T*>(address);
  }

  /// Forces the compiler to perform a read to `address`. Internally, casts `address`
  /// into a `const volatile T*` and reads from it.
  ///
  /// \param address The address to read from
  /// \return The value at `*address`
  template <TriviallyCopyable T> [[nodiscard]] T volatile_read(const volatile T* address) noexcept {
    return *address;
  }

  /// Forces the compiler to perform a write to `address`. Internally, casts `address`
  /// into a `const volatile T*` and writes to it.
  ///
  /// \param address The address to read from
  /// \param value The value to write to `*address`
  template <TriviallyCopyable T> void volatile_write(T* address, T value) noexcept {
    *static_cast<volatile T*>(address) = value;
  }

  /// Forces the compiler to perform a write to `address`. Internally, casts `address`
  /// into a `const volatile T*` and writes to it.
  ///
  /// \param address The address to read from
  /// \param value The value to write to `*address`
  template <TriviallyCopyable T> void volatile_write(volatile T* address, T value) noexcept {
    *address = value;
  }
} // namespace frt