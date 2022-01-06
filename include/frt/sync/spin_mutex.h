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

#include "./atomic.h"

namespace frt {
  /// A simple mutex based around a spin-lock and an atomic flag. While it's not particularly
  /// efficient, but it works without any support (or lack thereof) from a runtime/operating system
  ///
  /// This implementation will try to use architecture-specific instructions to improve power efficiency
  /// of cores while they're waiting, using something like the x86 `pause` instruction or the
  /// arm `yield` instruction while inside the wait loop to inform the CPU of what's going on.
  class SpinMutex {
  public:
    /// Initializes the mutex in an unlocked state
    explicit SpinMutex() noexcept = default;

    /// Tries to lock the mutex. If the mutex is locked, spins in a wait loop (with an arch-specific
    /// "I'm in a spin-loop, don't burn too much power" instruction inside the loop).
    ///
    /// Once the lock is available, it is locked and the function returns.
    void lock() noexcept;

    /// Unlocks the mutex by clearing the internal flag.
    ///
    /// Any writes done while the mutex was locked become visible to other threads
    /// that lock the mutex.
    void unlock() noexcept;

  private:
    frt::AtomicFlag locked_;
  };

  /// Effectively the same thing as `SpinMutex`, except it doesn't try
  /// to tell the CPU what's going on for power efficiency's sake. This avoids
  /// potential tiny slowdowns from a `pause` or `yield` instruction,
  /// but also harms power efficiency and hyper-threading.
  ///
  /// Do not use this unless all of the following are true:
  ///
  ///   1. Your lock is highly contended
  ///   2. You cannot reduce contention on the lock
  ///   3. You care about a couple of ns of loss on lock acquisition
  ///   4. You are perfectly fine with a hyper-thread on the same core being starved for load-store unit time
  ///   5. You don't care about the large amount of wasted power as the CPU burns cycles trying to
  ///      predict/prefetch/whatever on a 3 instruction loop
  ///
  class RawSpinMutex {
  public:
    /// Initializes the mutex in an unlocked state
    explicit RawSpinMutex() noexcept = default;

    /// Tries to lock the mutex, spins in an empty loop until
    /// the lock is available.
    ///
    /// Once the lock is available, it is locked and the function returns.
    void lock() noexcept;

    /// Unlocks the mutex by clearing the internal flag
    ///
    /// Any writes done while the mutex was locked become visible to other threads
    /// that lock the mutex
    void unlock() noexcept;

  private:
    frt::AtomicFlag locked_;
  };
} // namespace frt
