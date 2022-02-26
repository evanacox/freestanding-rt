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

#include "../types/basic.h"
#include "./atomic.h"

namespace frt {
  /// Models a fair mutex using a "ticket" system. See https://en.wikipedia.org/wiki/Ticket_lock
  ///
  /// Both numbers are modeled as atomics, locking simply takes a ticket and waits until the lock
  /// reaches that ticket.
  class TicketMutex {
  public:
    /// Initializes a ticket mutex in the unlocked state
    explicit TicketMutex() noexcept = default;

    /// Takes a ticket, and waits until the lock is ready for that ticket.
    ///
    /// Once the lock is available, it is locked and the function returns.
    void lock() noexcept;

    /// Unlocks the mutex, and increments the ticket number.
    ///
    /// Any writes done while the mutex was locked become visible to other threads
    /// that lock the mutex.
    void unlock() noexcept;

  private:
    Atomic<frt::isize> count_;
    Atomic<frt::isize> current_;
  };
} // namespace frt