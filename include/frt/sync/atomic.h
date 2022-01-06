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
#include "../platform.h"
#include "../types/concepts.h"
#include "../types/move.h"

#if defined(FRT_ARCH_X86_64)
#include <immintrin.h>
#endif

namespace frt {
  namespace internal {
    // Hints to the CPU itself that it's inside a spin-loop, and can save power/yield to other
    // hyper-threads accordingly.
    //
    // On unsupported/unrecognized architectures, this is a no-op.
    FRT_ALWAYS_INLINE void spin_hint() noexcept {
#if defined(FRT_ARCH_X86_64)
      _mm_pause(); // `__builtin_ia32_pause` also introduces a memory barrier, this is **only** meant to be a spin hint
#elif defined(FRT_COMPILER_GNULIKE) && defined(FRT_ARCH_ARM)
      volatile asm("yield"); // equivalent to `pause` on x86
#else
      (void)0; // if we don't support the arch, just do a no-op. won't help power efficiency but wont hurt it either
#endif
    }
  } // namespace internal

  /// Magic template used to enforce compile-time memory order constraints
  ///
  /// Any operations dealing in memory orders can use this, and then can `static_assert`
  /// on the ordering given in order
  ///
  /// \tparam Order The order to use
  template <int Order> struct MemoryOrder {
    /* implicit */ consteval operator int() const noexcept { // NOLINT(google-explicit-constructor)
      return Order;
    }

    [[nodiscard]] consteval static int order() noexcept {
      return Order;
    }
  };

  /// Relaxed ordering, only this operation's atomicity is guaranteed
  ///
  /// See https://en.cppreference.com/w/cpp/atomic/memory_order#Relaxed_ordering
  inline constexpr MemoryOrder<__ATOMIC_RELAXED> memory_order_relaxed;

  /// *Acquires* the memory being loaded, no loads/stores in the current
  /// thread can be reordered before this load. Loads/stores in other threads
  /// that have *released* to the same atomic  variable are visible
  ///
  /// See https://en.cppreference.com/w/cpp/atomic/memory_order#Release-Acquire_ordering
  inline constexpr MemoryOrder<__ATOMIC_ACQUIRE> memory_order_acquire;

  /// *Releases* the memory being stored to, no loads/stores in the current
  /// thread can be reordered after this load. Stores in the current thread
  /// are visible to other threads that *acquire* the same atomic variable
  ///
  /// See https://en.cppreference.com/w/cpp/atomic/memory_order#Release-Acquire_ordering
  inline constexpr MemoryOrder<__ATOMIC_RELEASE> memory_order_release;

  /// Both acquires and releases from the same memory location, only used
  /// for the read-write-modify atomic operations
  ///
  /// See https://en.cppreference.com/w/cpp/atomic/memory_order#Release-Acquire_ordering
  inline constexpr MemoryOrder<__ATOMIC_ACQ_REL> memory_order_acq_rel;

  /// All loads perform an *acquire*, all stores perform a *release*,
  /// all read-write-modify operations perform both an *acquire* and a *release*,
  /// and there is a single total order in which all threads observe the modifications
  /// in the same order
  ///
  /// See https://en.cppreference.com/w/cpp/atomic/memory_order#Sequentially-consistent_ordering
  inline constexpr MemoryOrder<__ATOMIC_SEQ_CST> memory_order_seq_cst;

  namespace internal {
    consteval bool valid_load_order(int order) noexcept {
      return (order != frt::memory_order_release) && (order != frt::memory_order_acq_rel);
    }

    consteval bool valid_store_order(int order) noexcept {
      return (order != frt::memory_order_acquire) && (order != frt::memory_order_acq_rel);
    }

    consteval bool valid_cmpxchg_failure_order(int order) noexcept {
      return valid_load_order(order);
    }

    // see https://en.cppreference.com/w/cpp/atomic/atomic/compare_exchange
    template <int Order = frt::memory_order_seq_cst>
    consteval MemoryOrder<Order == MemoryOrder<__ATOMIC_ACQ_REL>::order()   ? MemoryOrder<__ATOMIC_ACQUIRE>::order()
                          : Order == MemoryOrder<__ATOMIC_RELEASE>::order() ? MemoryOrder<__ATOMIC_RELAXED>::order()
                                                                            : Order>
    cmpxchg_failure_order(MemoryOrder<Order> /*unused*/) {
      return {};
    }

    template <typename T, int Order>
    [[nodiscard]] FRT_ALWAYS_INLINE T atomic_load(const T* address,
        MemoryOrder<Order> order = frt::memory_order_seq_cst) noexcept {
      static_assert(valid_load_order(Order), "store ordering must be one of: `relaxed`, `release`, `seq_cst`");

      return __atomic_load_n(address, order);
    }

    template <typename T, int Order>
    FRT_ALWAYS_INLINE void atomic_store(T* address,
        T desired,
        MemoryOrder<Order> order = frt::memory_order_seq_cst) noexcept {
      static_assert(valid_store_order(Order), "store ordering must be one of: `relaxed`, `release`, `seq_cst`");
      static_assert(!traits::is_const<T>, "cannot atomically write to `const` memory");

      __atomic_store_n(address, desired, order);
    }

    template <typename T, int Order>
    FRT_ALWAYS_INLINE T atomic_exchange(T* address,
        T desired,
        MemoryOrder<Order> order = frt::memory_order_seq_cst) noexcept {
      static_assert(!traits::is_const<T>, "cannot atomically write to `const` memory");

      return __atomic_exchange_n(address, desired, order);
    }

    template <typename T, int SuccessOrder, int FailOrder>
    FRT_ALWAYS_INLINE bool atomic_cmpxchg_weak(T* address,
        T& expected,
        T desired,
        MemoryOrder<SuccessOrder> success,
        MemoryOrder<FailOrder> failure) noexcept {
      static_assert(!traits::is_const<T>, "cannot atomically write to `const` memory");
      static_assert(valid_cmpxchg_failure_order(FailOrder),
          "failure order must be one of: `relaxed`, `acquire`, `seq_cst`");

      return __atomic_compare_exchange_n(address, &expected, desired, true, success, failure);
    }

    template <typename T, int SuccessOrder, int FailOrder>
    FRT_ALWAYS_INLINE bool atomic_cmpxchg_strong(T* address,
        T& expected,
        T desired,
        MemoryOrder<SuccessOrder> success,
        MemoryOrder<FailOrder> failure) noexcept {
      static_assert(!traits::is_const<T>, "cannot atomically write to `const` memory");
      static_assert(valid_cmpxchg_failure_order(FailOrder),
          "failure order must be one of: `relaxed`, `acquire`, `seq_cst`");

      return __atomic_compare_exchange_n(address, &expected, desired, false, success, failure);
    }

    template <typename T, int Order>
    FRT_ALWAYS_INLINE T atomic_add_fetch(T* address,
        T n,
        MemoryOrder<Order> order = frt::memory_order_seq_cst) noexcept {
      static_assert(!traits::is_const<T>, "cannot atomically write to `const` memory");

      return __atomic_add_fetch(address, n, order);
    }

    template <typename T, int Order>
    FRT_ALWAYS_INLINE T atomic_sub_fetch(T* address,
        T n,
        MemoryOrder<Order> order = frt::memory_order_seq_cst) noexcept {
      static_assert(!traits::is_const<T>, "cannot atomically write to `const` memory");

      return __atomic_sub_fetch(address, n, order);
    }

    template <typename T, int Order>
    FRT_ALWAYS_INLINE T atomic_and_fetch(T* address,
        T n,
        MemoryOrder<Order> order = frt::memory_order_seq_cst) noexcept {
      static_assert(!traits::is_const<T>, "cannot atomically write to `const` memory");

      return __atomic_and_fetch(address, n, order);
    }

    template <typename T, int Order>
    FRT_ALWAYS_INLINE T atomic_xor_fetch(T* address,
        T n,
        MemoryOrder<Order> order = frt::memory_order_seq_cst) noexcept {
      static_assert(!traits::is_const<T>, "cannot atomically write to `const` memory");

      return __atomic_xor_fetch(address, n, order);
    }

    template <typename T, int Order>
    FRT_ALWAYS_INLINE T atomic_or_fetch(T* address,
        T n,
        MemoryOrder<Order> order = frt::memory_order_seq_cst) noexcept {
      static_assert(!traits::is_const<T>, "cannot atomically write to `const` memory");

      return __atomic_or_fetch(address, n, order);
    }

    template <typename T, int Order>
    FRT_ALWAYS_INLINE T atomic_nand_fetch(T* address,
        T n,
        MemoryOrder<Order> order = frt::memory_order_seq_cst) noexcept {
      static_assert(!traits::is_const<T>, "cannot atomically write to `const` memory");

      return __atomic_nand_fetch(address, n, order);
    }

    template <typename T, int Order>
    FRT_ALWAYS_INLINE T atomic_fetch_add(T* address,
        T n,
        MemoryOrder<Order> order = frt::memory_order_seq_cst) noexcept {
      static_assert(!traits::is_const<T>, "cannot atomically write to `const` memory");

      return __atomic_fetch_add(address, n, order);
    }

    template <typename T, int Order>
    FRT_ALWAYS_INLINE T atomic_fetch_sub(T* address,
        T n,
        MemoryOrder<Order> order = frt::memory_order_seq_cst) noexcept {
      static_assert(!traits::is_const<T>, "cannot atomically write to `const` memory");

      return __atomic_fetch_sub(address, n, order);
    }

    template <typename T, int Order>
    FRT_ALWAYS_INLINE T atomic_fetch_and(T* address,
        T n,
        MemoryOrder<Order> order = frt::memory_order_seq_cst) noexcept {
      static_assert(!traits::is_const<T>, "cannot atomically write to `const` memory");

      return __atomic_fetch_and(address, n, order);
    }

    template <typename T, int Order>
    FRT_ALWAYS_INLINE T atomic_fetch_xor(T* address,
        T n,
        MemoryOrder<Order> order = frt::memory_order_seq_cst) noexcept {
      static_assert(!traits::is_const<T>, "cannot atomically write to `const` memory");

      return __atomic_fetch_xor(address, n, order);
    }

    template <typename T, int Order>
    FRT_ALWAYS_INLINE T atomic_fetch_or(T* address,
        T n,
        MemoryOrder<Order> order = frt::memory_order_seq_cst) noexcept {
      static_assert(!traits::is_const<T>, "cannot atomically write to `const` memory");

      return __atomic_fetch_or(address, n, order);
    }

    template <typename T, int Order>
    FRT_ALWAYS_INLINE T atomic_fetch_nand(T* address,
        T n,
        MemoryOrder<Order> order = frt::memory_order_seq_cst) noexcept {
      static_assert(!traits::is_const<T>, "cannot atomically write to `const` memory");

      return __atomic_fetch_nand(address, n, order);
    }

    template <typename T, int Order>
    FRT_ALWAYS_INLINE void atomic_spin_wait(T* address, T old, MemoryOrder<Order> order) noexcept {
      while (true) {
        if (atomic_load(address, order) != old) {
          return;
        }

        internal::spin_hint();
      }
    }
  } // namespace internal

  /// Provides atomic access to a contained value of type `T`
  ///
  /// \tparam T The type that this class provides atomic access to
  template <frt::Integral T> class Atomic {
  public:
    /// Initializes the atomic object with a value of `value`. Note that
    /// this initialization is **not** atomic.
    ///
    /// \param value The value to initialize with
    constexpr explicit Atomic(T value = T{}) noexcept : value_{value} {}

    /// Atomics are not copyable
    constexpr Atomic(const Atomic&) noexcept = delete;

    /// Atomics are not movable
    constexpr Atomic(Atomic&&) noexcept = delete;

    /// Use `.store` instead
    Atomic& operator=(const Atomic&) noexcept = delete;

    /// Use `.store` instead
    Atomic& operator=(Atomic&&) noexcept = delete;

    /// Loads from the stored value using the specified memory ordering
    ///
    /// \param order The order to load the memory with
    /// \return Returns the value of `*this`
    template <int Order = frt::memory_order_seq_cst>
    [[nodiscard]] FRT_ALWAYS_INLINE T load(MemoryOrder<Order> order = frt::memory_order_seq_cst) const noexcept {
      return internal::atomic_load<T>(address(), order);
    }

    /// Stores into `*this` using the specified memory ordering
    ///
    /// \param desired The value to store into `*this`
    /// \param order The order to use with the store
    template <int Order = frt::memory_order_seq_cst>
    FRT_ALWAYS_INLINE void store(T desired, MemoryOrder<Order> order = frt::memory_order_seq_cst) noexcept {
      return internal::atomic_store<T>(address(), desired, order);
    }

    /// Atomically exchanges `*this` with `desired`. Sets `*this` to `desired` and returns the old value
    ///
    /// \param desired The value to store into `*this`
    /// \param order The order to use with the store
    /// \return Returns the old value of `*this`
    template <int Order = frt::memory_order_seq_cst>
    [[nodiscard]] FRT_ALWAYS_INLINE T exchange(T desired,
        MemoryOrder<Order> order = frt::memory_order_seq_cst) noexcept {
      return internal::atomic_exchange<T>(address(), desired, order);
    }

    /// Atomically compares and exchanges `*this` and `expected`. If the two are
    /// bitwise-equal, `*this` is set to `desired`, otherwise `expected` is set to `*this`.
    ///
    /// This may spuriously fail and return `false` even when `*this == expected`.
    /// See https://en.cppreference.com/w/cpp/atomic/atomic/compare_exchange#Notes
    ///
    /// \param desired The value to store into `*this`
    /// \param order The order to use with the store
    /// \return Whether or not `*this` was changed
    template <int SuccessOrder, int FailOrder>
    FRT_ALWAYS_INLINE bool compare_exchange_weak(T& expected,
        T desired,
        MemoryOrder<SuccessOrder> success,
        MemoryOrder<FailOrder> failure) noexcept {
      return internal::atomic_cmpxchg_weak<T>(address(), expected, desired, success, failure);
    }

    /// Atomically compares and exchanges `*this` and `expected`. If the two are
    /// bitwise-equal, `*this` is set to `desired`, otherwise `expected` is set to `*this`.
    ///
    /// This may spuriously fail and return `false` even when `*this == expected`.
    /// See https://en.cppreference.com/w/cpp/atomic/atomic/compare_exchange#Notes
    ///
    /// \param expected The expected value, will be set to the current value if `expected != *this`
    /// \param desired The value to store into the object pointed to by `*this`
    /// \param order The order to use for both the load and the read-write-modify
    /// \return Whether or not the object pointed to by `*this` was changed
    template <int Order = frt::memory_order_seq_cst>
    FRT_ALWAYS_INLINE bool compare_exchange_weak(T& expected,
        T desired,
        MemoryOrder<Order> order = frt::memory_order_seq_cst) const noexcept {
      return internal::atomic_cmpxchg_weak<T>(address(),
          expected,
          desired,
          order,
          internal::cmpxchg_failure_order(order));
    }

    /// Atomically compares and exchanges `*this` and `expected`. If the two are
    /// bitwise-equal, `*this` is set to `desired`, otherwise `expected` is set to `*this`.
    ///
    /// \param desired The value to store into `*this`
    /// \param order The order to use with the store
    /// \return Whether or not `*this` was changed
    template <int SuccessOrder, int FailOrder>
    FRT_ALWAYS_INLINE bool compare_exchange_strong(T& expected,
        T desired,
        MemoryOrder<SuccessOrder> success,
        MemoryOrder<FailOrder> failure) noexcept {
      return internal::atomic_cmpxchg_strong<T>(address(), expected, desired, success, failure);
    }

    /// Atomically compares and exchanges `*this` and `expected`. If the two are
    /// bitwise-equal, `*this` is set to `desired`, otherwise `expected` is set to `*this`.
    ///
    /// \param expected The expected value, will be set to the current value if `expected != *this`
    /// \param desired The value to store into the object pointed to by `*this`
    /// \param order The order to use for both the load and the read-write-modify
    /// \return Whether or not the object pointed to by `*this` was changed
    template <int Order = frt::memory_order_seq_cst>
    FRT_ALWAYS_INLINE bool compare_exchange_strong(T& expected,
        T desired,
        MemoryOrder<Order> order = frt::memory_order_seq_cst) const noexcept {
      return internal::atomic_cmpxchg_strong<T>(address(),
          expected,
          desired,
          order,
          internal::cmpxchg_failure_order(order));
    }

    /// Adds `n` to `*this` and then returns the new value of `*this`
    ///
    /// \param n The number to add
    /// \param order The memory order to use
    /// \return The new value of `*this`
    template <int Order = frt::memory_order_seq_cst>
    FRT_ALWAYS_INLINE T add_fetch(T n, MemoryOrder<Order> order = frt::memory_order_seq_cst) noexcept {
      return internal::atomic_add_fetch<T>(address(), n, order);
    }

    /// Subtracts `n` from `*this` and then returns the new value of `*this`
    ///
    /// \param n The number to subtract
    /// \param order The memory order to use
    /// \return The new value of `*this`
    template <int Order = frt::memory_order_seq_cst>
    FRT_ALWAYS_INLINE T sub_fetch(T n, MemoryOrder<Order> order = frt::memory_order_seq_cst) noexcept {
      return internal::atomic_sub_fetch<T>(address(), n, order);
    }

    /// Bitwise ANDs `n` and `*this` and then returns the new value of `*this`
    ///
    /// \param n The number to AND with
    /// \param order The memory order to use
    /// \return The new value of `*this`
    template <int Order = frt::memory_order_seq_cst>
    FRT_ALWAYS_INLINE T and_fetch(T n, MemoryOrder<Order> order = frt::memory_order_seq_cst) noexcept {
      return internal::atomic_and_fetch<T>(address(), n, order);
    }

    /// Bitwise ORs `n` and `*this` and then returns the new value of `*this`
    ///
    /// \param n The number to OR with
    /// \param order The memory order to use
    /// \return The new value of `*this`
    template <int Order = frt::memory_order_seq_cst>
    FRT_ALWAYS_INLINE T or_fetch(T n, MemoryOrder<Order> order = frt::memory_order_seq_cst) noexcept {
      return internal::atomic_or_fetch<T>(address(), n, order);
    }

    /// Bitwise XORs `n` and `*this` and then returns the new value of `*this`
    ///
    /// \param n The number to XOR with
    /// \param order The memory order to use
    /// \return The new value of `*this`
    template <int Order = frt::memory_order_seq_cst>
    FRT_ALWAYS_INLINE T xor_fetch(T n, MemoryOrder<Order> order = frt::memory_order_seq_cst) noexcept {
      return internal::atomic_xor_fetch<T>(address(), n, order);
    }

    /// Bitwise NANDs `n` and `*this` and then returns the new value of `*this`
    ///
    /// \param n The number to NAND with
    /// \param order The memory order to use
    /// \return The new value of `*this`
    template <int Order = frt::memory_order_seq_cst>
    FRT_ALWAYS_INLINE T nand_fetch(T n, MemoryOrder<Order> order = frt::memory_order_seq_cst) noexcept {
      return internal::atomic_nand_fetch<T>(address(), n, order);
    }

    /// Adds `n` to `*this` and then returns the previous value of `*this`
    ///
    /// \param n The number to add
    /// \param order The memory order to use
    /// \return The previous value of `*this`
    template <int Order = frt::memory_order_seq_cst>
    FRT_ALWAYS_INLINE T fetch_add(T n, MemoryOrder<Order> order = frt::memory_order_seq_cst) noexcept {
      return internal::atomic_fetch_add<T>(address(), n, order);
    }

    /// Subtracts `n` from `*this` and then returns the previous value of `*this`
    ///
    /// \param n The number to subtract
    /// \param order The memory order to use
    /// \return The previous value of `*this`
    template <int Order = frt::memory_order_seq_cst>
    FRT_ALWAYS_INLINE T fetch_sub(T n, MemoryOrder<Order> order = frt::memory_order_seq_cst) noexcept {
      return internal::atomic_fetch_sub<T>(address(), n, order);
    }

    /// Bitwise ANDs `n` and `*this` and then returns the previous value of `*this`
    ///
    /// \param n The number to AND with
    /// \param order The memory order to use
    /// \return The previous value of `*this`
    template <int Order = frt::memory_order_seq_cst>
    FRT_ALWAYS_INLINE T fetch_and(T n, MemoryOrder<Order> order = frt::memory_order_seq_cst) noexcept {
      return internal::atomic_fetch_and<T>(address(), n, order);
    }

    /// Bitwise ORs `n` and `*this` and then returns the previous value of `*this`
    ///
    /// \param n The number to OR with
    /// \param order The memory order to use
    /// \return The previous value of `*this`
    template <int Order = frt::memory_order_seq_cst>
    FRT_ALWAYS_INLINE T fetch_or(T n, MemoryOrder<Order> order = frt::memory_order_seq_cst) noexcept {
      return internal::atomic_fetch_or<T>(address(), n, order);
    }

    /// Bitwise XORs `n` and `*this` and then returns the previous value of `*this`
    ///
    /// \param n The number to XOR with
    /// \param order The memory order to use
    /// \return The previous value of `*this`
    template <int Order = frt::memory_order_seq_cst>
    FRT_ALWAYS_INLINE T fetch_xor(T n, MemoryOrder<Order> order = frt::memory_order_seq_cst) noexcept {
      return internal::atomic_fetch_xor<T>(address(), n, order);
    }

    /// Bitwise NANDs `n` and `*this` and then returns the previous value of `*this`
    ///
    /// \param n The number to NAND with
    /// \param order The memory order to use
    /// \return The previous value of `*this`
    template <int Order = frt::memory_order_seq_cst>
    FRT_ALWAYS_INLINE T fetch_nand(T n, MemoryOrder<Order> order = frt::memory_order_seq_cst) noexcept {
      return internal::atomic_fetch_nand<T>(address(), n, order);
    }

    /// Waits until `load(order) != old`, and then returns.
    ///
    /// \param old The value to check that the atomic no longer contains
    /// \param order The ordering to load with
    template <int Order = frt::memory_order_seq_cst>
    FRT_ALWAYS_INLINE void wait(T old, MemoryOrder<Order> order = frt::memory_order_seq_cst) const noexcept {
      internal::atomic_spin_wait(address(), old, order);
    }

  private:
    [[nodiscard]] T* address() noexcept {
      return frt::address_of(value_);
    }

    [[nodiscard]] const T* address() const noexcept {
      return frt::address_of(value_);
    }

    T value_;
  };

  /// Allows atomic access to an object that isn't owned
  ///
  /// Instead of holding an object like `Atomic<T>` does, it instead
  /// holds the address of an object and performs atomic operations through
  /// that pointer
  template <frt::Integral T> class AtomicRef {
  public:
    /// Creates an `AtomicRef` from a pointer
    ///
    /// \param pointer A pointer to the object. Must not be `nullptr`.
    constexpr explicit AtomicRef(T* pointer) noexcept : object_{pointer} {
      FRT_ASSERT(pointer != nullptr, "cannot have a null `AtomicRef`");
    }

    /// Disallows creating an `AtomicRef` from a null literal
    constexpr explicit AtomicRef(frt::nullptr_t) noexcept = delete;

    /// Copying an `AtomicRef` is equivalent to creating another one
    /// with the same pointer
    constexpr AtomicRef(const AtomicRef&) noexcept = default;

    /// Same as copy constructor, copying an `AtomicRef` is equivalent
    /// to creating another one with the same pointer
    constexpr AtomicRef(AtomicRef&&) noexcept = default;

    /// Use `.store` instead
    constexpr AtomicRef& operator=(const AtomicRef&) = delete;

    /// Use `.store` instead
    constexpr AtomicRef& operator=(AtomicRef&&) = delete;

    /// Loads from the stored value using the specified memory ordering
    ///
    /// \param order The order to load the memory with
    /// \return Returns the value of the object pointed to by `*this`
    template <int Order = frt::memory_order_seq_cst>
    [[nodiscard]] FRT_ALWAYS_INLINE T load(MemoryOrder<Order> order = frt::memory_order_seq_cst) const noexcept {
      return internal::atomic_load<T>(address(), order);
    }

    /// Stores into the object pointed to by `*this` using the specified memory ordering
    ///
    /// \param desired The value to store into the object pointed to by `*this`
    /// \param order The order to use with the store
    template <int Order = frt::memory_order_seq_cst>
    FRT_ALWAYS_INLINE void store(T desired, MemoryOrder<Order> order = frt::memory_order_seq_cst) const noexcept {
      return internal::atomic_store<T>(address(), desired, order);
    }

    /// Atomically exchanges the object pointed to by `*this` with `desired`. Sets the object pointed to by `*this` to
    /// `desired` and returns the old value
    ///
    /// \param desired The value to store into the object pointed to by `*this`
    /// \param order The order to use with the store
    /// \return Returns the old value of the object pointed to by `*this`
    template <int Order = frt::memory_order_seq_cst>
    [[nodiscard]] FRT_ALWAYS_INLINE T exchange(T desired,
        MemoryOrder<Order> order = frt::memory_order_seq_cst) const noexcept {
      return internal::atomic_exchange<T>(address(), desired, order);
    }

    /// Atomically compares and exchanges the object pointed to by `*this` and `expected`. If the two are
    /// bitwise-equal, the object pointed to by `*this` is set to `desired`, otherwise `expected` is set to the object
    /// pointed to by `*this`.
    ///
    /// This may spuriously fail and return `false` even when `*this == expected`.
    /// See https://en.cppreference.com/w/cpp/atomic/atomic/compare_exchange#Notes
    ///
    /// \param expected The expected value, will be set to the current value if `expected != *this`
    /// \param desired The value to store into the object pointed to by `*this`
    /// \param success The order to use with the read-write-modify if `expected == *this`
    /// \param failure The order to use with the load if `expected != *this`
    /// \return Whether or not the object pointed to by `*this` was changed
    template <int SuccessOrder, int FailOrder>
    FRT_ALWAYS_INLINE bool compare_exchange_weak(T& expected,
        T desired,
        MemoryOrder<SuccessOrder> success,
        MemoryOrder<FailOrder> failure) const noexcept {
      return internal::atomic_cmpxchg_weak<T>(address(), expected, desired, success, failure);
    }

    /// Atomically compares and exchanges the object pointed to by `*this` and `expected`. If the two are
    /// bitwise-equal, the object pointed to by `*this` is set to `desired`, otherwise `expected` is set to the object
    /// pointed to by `*this`.
    ///
    /// This may spuriously fail and return `false` even when `*this == expected`.
    /// See https://en.cppreference.com/w/cpp/atomic/atomic/compare_exchange#Notes
    ///
    /// \param expected The expected value, will be set to the current value if `expected != *this`
    /// \param desired The value to store into the object pointed to by `*this`
    /// \param order The order to use for both the load and the read-write-modify
    /// \return Whether or not the object pointed to by `*this` was changed
    template <int Order = frt::memory_order_seq_cst>
    FRT_ALWAYS_INLINE bool compare_exchange_weak(T& expected,
        T desired,
        MemoryOrder<Order> order = frt::memory_order_seq_cst) const noexcept {
      return internal::atomic_cmpxchg_weak<T>(address(),
          expected,
          desired,
          order,
          internal::cmpxchg_failure_order(order));
    }

    /// Atomically compares and exchanges the object pointed to by `*this` and `expected`. If the two are
    /// bitwise-equal, the object pointed to by `*this` is set to `desired`, otherwise `expected` is set to the object
    /// pointed to by `*this`.
    ///
    /// \param expected The expected value, will be set to the current value if `expected != *this`
    /// \param desired The value to store into the object pointed to by `*this`
    /// \param success The order to use with the read-write-modify if `expected == *this`
    /// \param failure The order to use with the load if `expected != *this`
    /// \return Whether or not the object pointed to by `*this` was changed
    template <int SuccessOrder, int FailOrder>
    FRT_ALWAYS_INLINE bool compare_exchange_strong(T& expected,
        T desired,
        MemoryOrder<SuccessOrder> success,
        MemoryOrder<FailOrder> failure) const noexcept {
      return internal::atomic_cmpxchg_strong<T>(address(), expected, desired, success, failure);
    }

    /// Atomically compares and exchanges the object pointed to by `*this` and `expected`. If the two are
    /// bitwise-equal, the object pointed to by `*this` is set to `desired`, otherwise `expected` is set to the object
    /// pointed to by `*this`.
    ///
    /// \param expected The expected value, will be set to the current value if `expected != *this`
    /// \param desired The value to store into the object pointed to by `*this`
    /// \param order The order to use for both the load and the read-write-modify
    /// \return Whether or not the object pointed to by `*this` was changed
    template <int Order = frt::memory_order_seq_cst>
    FRT_ALWAYS_INLINE bool compare_exchange_strong(T& expected,
        T desired,
        MemoryOrder<Order> order = frt::memory_order_seq_cst) const noexcept {
      return internal::atomic_cmpxchg_strong<T>(address(),
          expected,
          desired,
          order,
          internal::cmpxchg_failure_order(order));
    }

    /// Adds `n` to the object pointed to by `*this` and then returns the new value of the object pointed to by `*this`
    ///
    /// \param n The number to add
    /// \param order The memory order to use
    /// \return The new value of the object pointed to by `*this`
    template <int Order = frt::memory_order_seq_cst>
    FRT_ALWAYS_INLINE T add_fetch(T n, MemoryOrder<Order> order = frt::memory_order_seq_cst) const noexcept {
      return internal::atomic_add_fetch<T>(address(), n, order);
    }

    /// Subtracts `n` from the object pointed to by `*this` and then returns the new value of the object pointed to by
    /// `*this`
    ///
    /// \param n The number to subtract
    /// \param order The memory order to use
    /// \return The new value of the object pointed to by `*this`
    template <int Order = frt::memory_order_seq_cst>
    FRT_ALWAYS_INLINE T sub_fetch(T n, MemoryOrder<Order> order = frt::memory_order_seq_cst) const noexcept {
      return internal::atomic_sub_fetch<T>(address(), n, order);
    }

    /// Bitwise ANDs `n` and the object pointed to by `*this` and then returns the new value of the object pointed to by
    /// `*this`
    ///
    /// \param n The number to AND with
    /// \param order The memory order to use
    /// \return The new value of the object pointed to by `*this`
    template <int Order = frt::memory_order_seq_cst>
    FRT_ALWAYS_INLINE T and_fetch(T n, MemoryOrder<Order> order = frt::memory_order_seq_cst) const noexcept {
      return internal::atomic_and_fetch<T>(address(), n, order);
    }

    /// Bitwise ORs `n` and the object pointed to by `*this` and then returns the new value of the object pointed to by
    /// `*this`
    ///
    /// \param n The number to OR with
    /// \param order The memory order to use
    /// \return The new value of the object pointed to by `*this`
    template <int Order = frt::memory_order_seq_cst>
    FRT_ALWAYS_INLINE T or_fetch(T n, MemoryOrder<Order> order = frt::memory_order_seq_cst) const noexcept {
      return internal::atomic_or_fetch<T>(address(), n, order);
    }

    /// Bitwise XORs `n` and the object pointed to by `*this` and then returns the new value of the object pointed to by
    /// `*this`
    ///
    /// \param n The number to XOR with
    /// \param order The memory order to use
    /// \return The new value of the object pointed to by `*this`
    template <int Order = frt::memory_order_seq_cst>
    FRT_ALWAYS_INLINE T xor_fetch(T n, MemoryOrder<Order> order = frt::memory_order_seq_cst) const noexcept {
      return internal::atomic_xor_fetch<T>(address(), n, order);
    }

    /// Bitwise NANDs `n` and the object pointed to by `*this` and then returns the new value of the object pointed to
    /// by `*this`
    ///
    /// \param n The number to NAND with
    /// \param order The memory order to use
    /// \return The new value of the object pointed to by `*this`
    template <int Order = frt::memory_order_seq_cst>
    FRT_ALWAYS_INLINE T nand_fetch(T n, MemoryOrder<Order> order = frt::memory_order_seq_cst) const noexcept {
      return internal::atomic_nand_fetch<T>(address(), n, order);
    }

    /// Adds `n` to the object pointed to by `*this` and then returns the previous value of the object pointed to by
    /// `*this`
    ///
    /// \param n The number to add
    /// \param order The memory order to use
    /// \return The previous value of the object pointed to by `*this`
    template <int Order = frt::memory_order_seq_cst>
    FRT_ALWAYS_INLINE T fetch_add(T n, MemoryOrder<Order> order = frt::memory_order_seq_cst) const noexcept {
      return internal::atomic_fetch_add<T>(address(), n, order);
    }

    /// Subtracts `n` from the object pointed to by `*this` and then returns the previous value of the object pointed to
    /// by `*this`
    ///
    /// \param n The number to subtract
    /// \param order The memory order to use
    /// \return The previous value of the object pointed to by `*this`
    template <int Order = frt::memory_order_seq_cst>
    FRT_ALWAYS_INLINE T fetch_sub(T n, MemoryOrder<Order> order = frt::memory_order_seq_cst) const noexcept {
      return internal::atomic_fetch_sub<T>(address(), n, order);
    }

    /// Bitwise ANDs `n` and the object pointed to by `*this` and then returns the previous value of the object pointed
    /// to by `*this`
    ///
    /// \param n The number to AND with
    /// \param order The memory order to use
    /// \return The previous value of the object pointed to by `*this`
    template <int Order = frt::memory_order_seq_cst>
    FRT_ALWAYS_INLINE T fetch_and(T n, MemoryOrder<Order> order = frt::memory_order_seq_cst) const noexcept {
      return internal::atomic_fetch_and<T>(address(), n, order);
    }

    /// Bitwise ORs `n` and the object pointed to by `*this` and then returns the previous value of the object pointed
    /// to by `*this`
    ///
    /// \param n The number to OR with
    /// \param order The memory order to use
    /// \return The previous value of the object pointed to by `*this`
    template <int Order = frt::memory_order_seq_cst>
    FRT_ALWAYS_INLINE T fetch_or(T n, MemoryOrder<Order> order = frt::memory_order_seq_cst) const noexcept {
      return internal::atomic_fetch_or<T>(address(), n, order);
    }

    /// Bitwise XORs `n` and the object pointed to by `*this` and then returns the previous value of the object pointed
    /// to by `*this`
    ///
    /// \param n The number to XOR with
    /// \param order The memory order to use
    /// \return The previous value of the object pointed to by `*this`
    template <int Order = frt::memory_order_seq_cst>
    FRT_ALWAYS_INLINE T fetch_xor(T n, MemoryOrder<Order> order = frt::memory_order_seq_cst) const noexcept {
      return internal::atomic_fetch_xor<T>(address(), n, order);
    }

    /// Bitwise NANDs `n` and the object pointed to by `*this` and then returns the previous value of the object pointed
    /// to by `*this`
    ///
    /// \param n The number to NAND with
    /// \param order The memory order to use
    /// \return The previous value of the object pointed to by `*this`
    template <int Order = frt::memory_order_seq_cst>
    FRT_ALWAYS_INLINE T fetch_nand(T n, MemoryOrder<Order> order = frt::memory_order_seq_cst) const noexcept {
      return internal::atomic_fetch_nand<T>(address(), n, order);
    }

    /// Waits until `load(order) != old`, and then returns.
    ///
    /// \param old The value to check that the atomic no longer contains
    /// \param order The ordering to load with
    template <int Order = frt::memory_order_seq_cst>
    FRT_ALWAYS_INLINE void wait(T old, MemoryOrder<Order> order = frt::memory_order_seq_cst) const noexcept {
      internal::atomic_spin_wait(address(), old, order);
    }

  private:
    [[nodiscard]] T* address() const noexcept {
      return object_;
    }

    T* object_;
  };

  namespace internal {
    consteval bool valid_flag_clear_order(int order) noexcept {
      return (order != frt::memory_order_acquire) && (order != frt::memory_order_acq_rel);
    }

    consteval bool valid_flag_test_order(int order) noexcept {
      return (order != frt::memory_order_release) && (order != frt::memory_order_acq_rel);
    }
  } // namespace internal

  /// Effectively an atomic bool with the arithmetic operations removed. This is solely
  /// for use as a flag to signal between threads, and it uses the platform's atomic flag
  /// instructions.
  class AtomicFlag {
  public:
    /// The platform-defined `true` value produced by `__atomic_test_and_set`
    inline static constexpr frt::byte true_value = __GCC_ATOMIC_TEST_AND_SET_TRUEVAL;

    /// Creates an atomic flag with an initial state of `false`
    constexpr explicit AtomicFlag() noexcept = default;

    /// Atomically clears the flag, setting its state to `false`
    ///
    /// \param order The memory order to use
    template <int Order = frt::memory_order_seq_cst>
    FRT_ALWAYS_INLINE void clear(MemoryOrder<Order> order = frt::memory_order_seq_cst) noexcept {
      static_assert(internal::valid_flag_clear_order(Order),
          "clear order must be one of: `relaxed`, `release`, `seq_cst`");

      __atomic_clear(&state_, order);
    }

    /// Performs an atomic test-and-set, setting the state to `true`
    /// and returning the state that it had before
    ///
    /// \param order The order to use for operation
    /// \return The old value of the flag
    template <int Order = frt::memory_order_seq_cst>
    FRT_ALWAYS_INLINE bool test_and_set(MemoryOrder<Order> order = frt::memory_order_seq_cst) noexcept {
      return __atomic_test_and_set(&state_, order);
    }

    /// Loads the state from the flag and returns it
    ///
    /// \param order The order to use for the load
    /// \return The state of the flag
    template <int Order = frt::memory_order_seq_cst>
    FRT_ALWAYS_INLINE bool test(MemoryOrder<Order> order = frt::memory_order_seq_cst) noexcept {
      return internal::atomic_load(&state_, order) == true_value;
    }

    /// Waits until `test(order) != old`, and then returns.
    ///
    /// \param old The value to check that the atomic no longer contains
    /// \param order The ordering to load with
    template <int Order = frt::memory_order_seq_cst>
    FRT_ALWAYS_INLINE void wait(bool old, MemoryOrder<Order> order = frt::memory_order_seq_cst) const noexcept {
      internal::atomic_spin_wait(&state_, old, order);
    }

  private:
    using Underlying = traits::Conditional<true_value == 1, bool, frt::byte>;

    // the true value may not actually be `1`, in which case we want to use
    // a byte value instead of a `bool`. `0` is always the false value though
    Underlying state_ = static_cast<Underlying>(0); // NOLINT(modernize-use-bool-literals): type not always a bool
  };

  /// Establishes a synchronization fence between threads based on `order`
  ///
  /// See https://en.cppreference.com/w/cpp/atomic/atomic_thread_fence
  ///
  /// \param order The order to use with the thread fence
  template <int Order = frt::memory_order_seq_cst>
  FRT_ALWAYS_INLINE void atomic_thread_fence(MemoryOrder<Order> order = frt::memory_order_seq_cst) noexcept {
    return __atomic_thread_fence(order);
  }
} // namespace frt