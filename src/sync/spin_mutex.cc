//======---------------------------------------------------------------======//
//                                                                           //
// Copyright 2021 Evan Cox <evanacox00@gmail.com>. All rights reserved.      //
//                                                                           //
// Use of this source code is governed by a BSD-style license that can be    //
// found in the LICENSE.txt file at the root of this project, or at the      //
// following link: https://opensource.org/licenses/BSD-3-Clause              //
//                                                                           //
//======---------------------------------------------------------------======//
#include <frt/sync/spin_mutex.h>

namespace {
  template <bool SpinHint> void spin_until_locked(frt::AtomicFlag* flag) noexcept {
    while (true) {
      // might as well check first, if the lock isn't taken we get to it with 1 less load
      // and if it isn't, we aren't in any hurry to get into the test loop anyway
      if (!flag->test_and_set(frt::memory_order_acquire)) {
        break;
      }

      // inner loop, reduces number of writes and therefore reduces
      // the need to refresh caches for every CPU 24/7
      while (flag->test(frt::memory_order_relaxed)) {
        if constexpr (SpinHint) {
          frt::internal::spin_hint();
        }
      }
    }
  }
} // namespace

namespace frt {
  void SpinMutex::lock() noexcept {
    spin_until_locked<true>(&locked_);
  }

  void SpinMutex::unlock() noexcept {
    locked_.clear(frt::memory_order_release);
  }

  void RawSpinMutex::lock() noexcept {
    spin_until_locked<false>(&locked_);
  }

  void RawSpinMutex::unlock() noexcept {
    locked_.clear(frt::memory_order_release);
  }
} // namespace frt