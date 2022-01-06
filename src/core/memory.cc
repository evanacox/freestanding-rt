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
#include "frt/core/bit.h"
#include "frt/platform/macros.h"
#include "frt/runtime/assert.h"

namespace {
  template <typename T> bool is_aligned_for(const void* address) noexcept {
    return frt::modulo_pow2(reinterpret_cast<frt::usize>(address), alignof(T));
  }

  void memcpy_internal(void* __restrict to, const void* __restrict from, frt::usize len) noexcept {
#ifdef __clang__
    constexpr frt::usize unrolled_cutoff = 64;

    if (len > unrolled_cutoff) {

      __builtin_memcpy_inline(to, from, 64);
    }
#elif defined(FRT_ARCH_X86_64)

#elif defined(FRT_ARCH_X86_64)

#endif
  }
} // namespace

extern "C" FRT_ALWAYS_INLINE void* frt::__frt_mem_copy(void* __restrict to,
    const void* __restrict from,
    frt::usize length) noexcept {
  return to;
}

extern "C" FRT_ALWAYS_INLINE void* frt::__frt_mem_move(void* to, const void* from, frt::usize length) noexcept {
  return to;
}

extern "C" FRT_ALWAYS_INLINE void* frt::__frt_mem_set(void* to, int value, frt::usize length) noexcept {
  return to;
}

extern "C" FRT_ALWAYS_INLINE int frt::__frt_mem_compare(const void* lhs, const void* rhs, frt::usize length) noexcept {
  return 0;
}

#ifndef FRT_GENERATE_MEM_FUNCTIONS

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

void* frt::force_mem_copy(void* to, const void* from, frt::size length) noexcept {
  FRT_ASSERT(to != nullptr && from != nullptr, "neither `to` nor `from` are allowed to be `nullptr`");

  return frt::__frt_mem_copy(to, from, static_cast<frt::usize>(length));
}

void* frt::force_mem_move(void* to, const void* from, frt::size length) noexcept {
  FRT_ASSERT(to != nullptr && from != nullptr, "neither `to` nor `from` are allowed to be `nullptr`");

  return frt::__frt_mem_move(to, from, static_cast<frt::usize>(length));
}

void* frt::force_mem_set(void* to, frt::byte value, frt::size length) noexcept {
  FRT_ASSERT(to != nullptr, "`to` is not allowed to be `nullptr`");

  return frt::__frt_mem_set(to, static_cast<int>(value), static_cast<frt::usize>(length));
}

int frt::force_mem_compare(const void* lhs, const void* rhs, frt::size length) noexcept {
  FRT_ASSERT(lhs != nullptr && rhs != nullptr, "neither `lhs` nor `rhs` are allowed to be `nullptr`");

  return frt::__frt_mem_compare(lhs, rhs, static_cast<frt::usize>(length));
}
