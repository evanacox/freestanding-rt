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
#include "../types/exchange.h"

namespace frt {
  /// Type that encapsulates a "deferred" callback.
  ///
  /// The type holds some callable object, and when it gets destroyed,
  /// that callback is invoked. Can not be copied or re-assigned, although
  /// moving is permitted.
  ///
  /// Use in place of `goto exit` or one-off RAII objects for deferred
  /// cleanup code
  template <Invocable Lambda> class DeferredAction {
  public:
    DeferredAction() = delete;

    DeferredAction(const DeferredAction&) = delete;

    DeferredAction(DeferredAction&& other) noexcept
        : data_(frt::move(other.data_)),
          should_invoke_(frt::exchange(other.should_invoke_, false)) {}

    explicit DeferredAction(Lambda&& lambda) : data_(frt::forward<Lambda>(lambda)) {}

    DeferredAction& operator=(const DeferredAction&) = delete;

    DeferredAction& operator=(DeferredAction&&) = delete;

    ~DeferredAction() {
      if (should_invoke_) {
        frt::invoke(data_);
      }
    }

  private:
    Lambda data_;
    bool should_invoke_ = true;
  };

  /// Registers some callback that is guaranteed to happen at the end of the current scope,
  /// with exception safety and the like built-in. Useful for RAII semantics without needing
  /// to make one-off RAII types.
  ///
  /// Returns an object that will call the callback in its destructor, this object can
  /// be stored.
  ///
  /// # Example
  /// ```cpp
  /// {
  ///     auto* foo = some_c_code();
  ///     auto _ = defer([&] { some_c_code_cleanup(foo); })
  ///
  ///     // use `foo`
  /// }
  /// ```
  ///
  /// # Parameters
  /// - `functor`: The callable to invoke whenever the returned object is destructed
  ///
  /// # Returns
  /// Returns an object encapsulating the action, that calls the action in its destructor.
  template <Invocable Fn> DeferredAction<Fn> defer(Fn&& functor) {
    return DeferredAction<Fn>(frt::forward<Fn>(functor));
  }
} // namespace frt