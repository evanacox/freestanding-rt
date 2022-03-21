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

#include "../core/memory.h"
#include "../types/basic.h"

namespace frt {
  /// Safely reads a `uint64_t` from possibly unaligned memory
  ///
  /// \param address The address to read from. Array pointed to must be at least `sizeof(uint16_t)` bytes long`
  /// \return A 16-bit integer
  frt::u16 unaligned_load16(const frt::ubyte* address) {
    frt::u16 x;
    frt::mem_copy(&x, address, sizeof(frt::u16));
    return x;
  }

  /// Safely reads a `uint64_t` from possibly unaligned memory
  ///
  /// \param address The address to read from. Array pointed to must be at least `sizeof(uint32_t)` bytes long`
  /// \return A 64-bit integer
  frt::u32 unaligned_load32(const frt::ubyte* address) {
    frt::u32 x;
    frt::mem_copy(&x, address, sizeof(frt::u32));
    return x;
  }

  /// Safely reads a `uint64_t` from possibly unaligned memory
  ///
  /// \param address The address to read from. Array pointed to must be at least `sizeof(uint64_t)` bytes long`
  /// \return A 64-bit integer
  frt::u64 unaligned_load64(const frt::ubyte* address) {
    frt::u64 x;
    frt::mem_copy(&x, address, sizeof(frt::u64));
    return x;
  }

  /// Safely stores a `uint16_t` to possibly unaligned memory
  ///
  /// \param address The address to store to. Array pointed to must be at least `sizeof(uint16_t)` bytes long`
  /// \param value The value to copy to `address`
  void unaligned_store16(frt::ubyte* address, frt::u16 value) {
    frt::mem_copy(address, &value, sizeof(frt::u16));
  }

  /// Safely stores a `uint16_t` to possibly unaligned memory
  ///
  /// \param address The address to store to. Array pointed to must be at least `sizeof(uint32_t)` bytes long`
  /// \param value The value to copy to `address`
  void unaligned_store32(frt::ubyte* address, frt::u32 value) {
    frt::mem_copy(address, &value, sizeof(frt::u32));
  }

  /// Safely stores a `uint16_t` to possibly unaligned memory
  ///
  /// \param address The address to store to. Array pointed to must be at least `sizeof(uint64_t)` bytes long`
  /// \param value The value to copy to `address`
  void unaligned_store64(frt::ubyte* address, frt::u64 value) {
    frt::mem_copy(address, &value, sizeof(frt::u64));
  }
} // namespace frt