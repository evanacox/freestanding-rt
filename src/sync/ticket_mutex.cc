//======---------------------------------------------------------------======//
//                                                                           //
// Copyright 2021-2022 Evan Cox <evanacox00@gmail.com>. All rights reserved. //
//                                                                           //
// Use of this source code is governed by a BSD-style license that can be    //
// found in the LICENSE.txt file at the root of this project, or at the      //
// following link: https://opensource.org/licenses/BSD-3-Clause              //
//                                                                           //
//======---------------------------------------------------------------======//

#include "frt/sync/ticket_mutex.h"
#include "frt/sync/spin_mutex.h"

namespace frt {
  void TicketMutex::lock() noexcept {
    auto ticket = count_.fetch_add(1, frt::memory_order_relaxed);

    while (true) {
      if (current_.load(frt::memory_order_acquire) == ticket) {
        break;
      }

      while (current_.load(frt::memory_order_relaxed) != ticket) {
        frt::internal::spin_hint();
      }
    }
  }

  void TicketMutex::unlock() noexcept {
    // release changes to any threads using this lock
    current_.fetch_add(1, frt::memory_order_release);
  }
} // namespace frt