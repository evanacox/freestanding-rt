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

#include "../platform/macros.h"
#include "../runtime/assert_runtime.h"
#include "../types/basic.h"

extern "C" {
  void* memcpy(void* __restrict to, const void* __restrict from, frt::usize length) noexcept;

  void* memmove(void* to, const void* from, frt::usize length) noexcept;

  void* memset(void* to, int value, frt::usize length) noexcept;

  int memcmp(const void* lhs, const void* rhs, frt::usize length) noexcept;
}

namespace frt {
  extern "C" {
    /// FRT implementation of `memcpy`. It isn't actually spelled `memcpy` unless
    /// certain macros are defined at compile-time which tell FRT to make
    /// this an alias for `memcpy`
    ///
    /// \param to The destination of the copy
    /// \param from The source of the copy
    /// \param length The number of bytes to copy
    /// \return `to`
    void* __frt_mem_copy(void* to, const void* from, frt::usize length) noexcept;

    /// FRT implementation of `memmove`. It isn't actually spelled `memmove` unless
    /// certain macros are defined at compile-time which tell FRT to make
    /// this an alias for `memmove`
    ///
    /// \param to The destination of the move
    /// \param from The source of the move
    /// \param length The number of bytes to copy
    /// \return `to`
    void* __frt_mem_move(void* to, const void* from, frt::usize length) noexcept;

    /// FRT implementation of `memset`. It isn't actually spelled `memset` unless
    /// certain macros are defined at compile-time which tell FRT to make
    /// this an alias for `memset`
    ///
    /// \param to The destination to set to `static_cast<unsigned char>(value)`
    /// \param value The value to set to the range
    /// \param length The number of bytes to set
    /// \return `to`
    void* __frt_mem_set(void* to, int value, frt::usize length) noexcept;

    /// FRT implementation of `memcmp`. It isn't actually spelled `memset` unless
    /// certain macros are defined at compile-time which tell FRT to make
    /// this an alias for `memcmp`
    ///
    /// \param lhs The first set of bytes to compare
    /// \param rhs The second set of bytes to compare
    /// \param length The number of bytes to compare
    /// \return `0` if equal, negative if first different byte is less-than in `lhs`, positive if first different byte
    /// is greater-than in `lhs`
    int __frt_mem_compare(const void* lhs, const void* rhs, frt::usize length) noexcept;
  }

  /// Wrapper function for `memcpy`
  ///
  /// \param to The destination of the copy
  /// \param from The source of the copy
  /// \param length The number of bytes to copy
  /// \return `to`
  FRT_ALWAYS_INLINE void* mem_copy(void* to, const void* from, frt::size length) noexcept {
    // enables better optimization based on the forced inlining & compiler knowledge
    // of what `memcopy` does, may be optimized out with a constant size
    return ::memcpy(to, from, static_cast<frt::usize>(length));
  }

  /// Wrapper function for `memmove`
  ///
  /// \param to The destination of the move
  /// \param from The source of the move
  /// \param length The number of bytes to copy
  /// \return `to`
  FRT_ALWAYS_INLINE void* mem_move(void* to, const void* from, frt::size length) noexcept {
    // enables better optimization based on the forced inlining & compiler knowledge
    // of what `memmove` does, may be optimized out with a constant size
    return ::memmove(to, from, static_cast<frt::usize>(length));
  }

  /// Wrapper function for `memset`
  ///
  /// \param to The destination to set to `static_cast<unsigned char>(value)`
  /// \param value The value to set to the range
  /// \param length The number of bytes to set
  /// \return `to`
  FRT_ALWAYS_INLINE void* mem_set(void* to, frt::byte value, frt::size length) noexcept {
    // enables better optimization based on the forced inlining & compiler knowledge
    // of what `memset` does, may be optimized out with a constant size
    return ::memset(to, static_cast<int>(value), static_cast<frt::usize>(length));
  }

  /// Wrapper function for `memcmp`
  ///
  /// \param lhs The first set of bytes to compare
  /// \param rhs The second set of bytes to compare
  /// \param length The number of bytes to compare
  /// \return `0` if equal, negative if first different byte is less-than in `lhs`, positive if first different byte
  /// is greater-than in `lhs`
  FRT_ALWAYS_INLINE int mem_compare(const void* lhs, const void* rhs, frt::size length) noexcept {
    // enables better optimization based on the forced inlining & compiler knowledge
    // of what `memcmp` does, may be optimized out with a constant size
    return ::memcmp(lhs, rhs, static_cast<frt::usize>(length));
  }

  /// `memcpy` equivalent, except assertions are enabled and it can't be optimized away with `as-if`
  ///
  /// \param to The destination of the copy
  /// \param from The source of the copy
  /// \param length The number of bytes to copy
  /// \return `to`
  FRT_ALWAYS_INLINE void* force_mem_copy(void* __restrict to, const void* __restrict from, frt::size length) noexcept;

  /// `memmove` equivalent, except assertions are enabled and it can't be optimized away with `as-if`
  ///
  /// \param to The destination of the move
  /// \param from The source of the move
  /// \param length The number of bytes to copy
  /// \return `to`
  FRT_ALWAYS_INLINE void* force_mem_move(void* to, const void* from, frt::size length) noexcept;

  /// `memset` equivalent, except assertions are enabled and it can't be optimized away with `as-if`
  ///
  /// \param to The destination to set to `static_cast<unsigned char>(value)`
  /// \param value The value to set to the range
  /// \param length The number of bytes to set
  /// \return `to`
  void* force_mem_set(void* to, frt::byte value, frt::size length) noexcept;

  /// `memcmp` equivalent, except assertions are enabled and it can't be optimized away with `as-if`
  ///
  /// \param lhs The first set of bytes to compare
  /// \param rhs The second set of bytes to compare
  /// \param length The number of bytes to compare
  /// \return `0` if equal, negative if first different byte is less-than in `lhs`, positive if first different byte
  /// is greater-than in `lhs`
  [[nodiscard]] int force_mem_compare(const void* lhs, const void* rhs, frt::size length) noexcept;

  /// Gets the address of an object without ever using `operator&` overloads
  ///
  /// \param object The object to get the address of
  /// \return A pointer to `object`
  template <typename T> [[nodiscard]] FRT_ALWAYS_INLINE constexpr T* address_of(T& object) noexcept {
    // we can either do it in a `constexpr`-compatible way, or we can do it in a standard way
    // since we use builtins everywhere else, I say we do it the `constexpr` way
    return __builtin_addressof(object);
  }

  /// Deleted overload to prevent taking address of temporaries
  template <typename T> constexpr const T* address_of(const T&& object) noexcept = delete;
} // namespace frt