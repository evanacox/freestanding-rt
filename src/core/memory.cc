//======---------------------------------------------------------------======//
//                                                                           //
// Copyright 2021-2022 Evan Cox <evanacox00@gmail.com>. All rights reserved. //
//                                                                           //
// Use of this source code is governed by a BSD-style license that can be    //
// found in the LICENSE.txt file at the root of this project, or at the      //
// following link: https://opensource.org/licenses/BSD-3-Clause              //
//                                                                           //
//======---------------------------------------------------------------======//
#include "frt/core/memory.h"
#include "frt/platform/macros.h"

extern "C" FRT_ALWAYS_INLINE void* frt::__frt_mem_copy(void* __restrict to,
    const void* __restrict from,
    frt::usize length) noexcept {
  // TODO: make a better memcpy. __builtin_memcpy_inline? rep movsb?
  auto* dst = static_cast<frt::ubyte* __restrict>(to);
  const auto* src = static_cast<const frt::ubyte* __restrict>(from);

  while (length-- > 0) {
    *dst++ = *src++;
  }

  return to;
}

extern "C" FRT_ALWAYS_INLINE void* frt::__frt_mem_move(void* to, const void* from, frt::usize length) noexcept {
  // TODO: fast mem_move
  auto* dst = static_cast<frt::ubyte*>(to);
  const auto* src = static_cast<const frt::ubyte*>(from);

  if (from < to) {
    // `__restrict` may or may not be satisfied here, so we can't just call `__frt_mem_copy`
    while (length-- > 0) {
      *dst++ = *src++;
    }
  } else {
    dst += length - 1;
    src += length - 1;

    while (length-- > 0) {
      *dst-- = *src--;
    }
  }

  return to;
}

extern "C" FRT_ALWAYS_INLINE void* frt::__frt_mem_set(void* to, int value, frt::usize length) noexcept {
  // TODO: fast mem_set
  auto* dst = static_cast<frt::ubyte*>(to);

  while (length-- > 0) {
    *dst++ = static_cast<frt::ubyte>(value);
  }

  return dst;
}

extern "C" FRT_ALWAYS_INLINE int frt::__frt_mem_compare(const void* lhs, const void* rhs, frt::usize length) noexcept {
  const auto* a = static_cast<const frt::ubyte*>(lhs);
  const auto* b = static_cast<const frt::ubyte*>(rhs);

  while (length-- > 0) {
    if (*a++ != *b++) {
      return (a[-1] < b[-1]) ? -1 : 1;
    }
  }

  return 0;
}

#ifdef FRT_GENERATE_DEFAULT_MEM_INTRINS

// all of these just fall back to the `__frt` functions, but are force-inlined to avoid the call overhead.
// this also makes it equivalent to calling the other one without breaking the expectation that these symbols
// actually exist like would happen with `__attribute__((alias))`

extern "C" void* memcpy(void* __restrict to, const void* __restrict from, frt::usize length) noexcept {
  return frt::__frt_mem_copy(to, from, length);
}

extern "C" void* memmove(void* to, const void* from, frt::usize length) noexcept {
  return frt::__frt_mem_move(to, from, length);
}

extern "C" void* memset(void* to, int value, frt::usize length) noexcept {
  return frt::__frt_mem_set(to, value, length);
}

extern "C" int memcmp(const void* lhs, const void* rhs, frt::usize length) noexcept {
  return frt::__frt_mem_compare(lhs, rhs, length);
}

#endif