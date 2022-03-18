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

#include "../core/iterators.h"
#include "../types/basic.h"
#include "./volatile.h"
#include <iterator>

namespace frt {
  template <TriviallyCopyable T, frt::usize Address> class IOPortRO {
  public:
    using value_type = T;
    using difference_type = frt::isize;
    using pointer = volatile T*;
    using reference = volatile T&;
    using const_pointer = const volatile T*;
    using const_reference = const volatile T&;

    [[nodiscard]] value_type read() const noexcept {
      return frt::volatile_read(address());
    }

    [[nodiscard]] operator value_type() const noexcept { // NOLINT(google-explicit-constructor)
      return read();
    }

    [[nodiscard]] const_reference operator*() const {
      return *address();
    }

    IOPortRO& operator++() noexcept {
      return *this;
    }

    IOPortRO operator++(int) noexcept {
      return *this;
    }

    [[nodiscard]] const_pointer address() const noexcept {
      return reinterpret_cast<const_pointer>(Address);
    }

    [[nodiscard]] bool operator==(IOPortRO /*unused*/) const noexcept {
      return true;
    }
  };

  template <TriviallyCopyable T, frt::usize Address> class IOPortWO {
  public:
    using value_type = T;
    using difference_type = std::ptrdiff_t;
    using pointer = volatile T*;
    using reference = volatile T&;
    using const_pointer = const volatile T*;
    using const_reference = const volatile T&;

    void write(T value) noexcept {
      frt::volatile_write(address(), value);
    }

    IOPortWO& operator=(T value) noexcept {
      write(value);

      return *this;
    }

    [[nodiscard]] IOPortWO& operator*() noexcept {
      return *this;
    }

    [[nodiscard]] IOPortWO& operator++() noexcept {
      return *this;
    }

    IOPortWO operator++(int) noexcept {
      return *this;
    }

    [[nodiscard]] pointer address() noexcept {
      return reinterpret_cast<pointer>(Address);
    }

    [[nodiscard]] const_pointer address() const noexcept {
      return reinterpret_cast<const_pointer>(Address);
    }

    [[nodiscard]] bool operator==(IOPortWO /*unused*/) const noexcept {
      return true;
    }
  };

  template <TriviallyCopyable T, frt::usize Address> class IOPortRW {
  public:
    using value_type = T;
    using difference_type = std::ptrdiff_t;
    using pointer = volatile T*;
    using reference = volatile T&;
    using const_pointer = const volatile T*;
    using const_reference = const volatile T&;

    IOPortRW& operator=(T value) noexcept {
      write(value);

      return *this;
    }

    [[nodiscard]] value_type read() const noexcept {
      return frt::volatile_read(address());
    }

    [[nodiscard]] operator value_type() const noexcept { // NOLINT(google-explicit-constructor)
      return read();
    }

    void write(T value) noexcept {
      frt::volatile_write(address(), value);
    }

    [[nodiscard]] reference operator*() noexcept {
      return *address();
    }

    [[nodiscard]] const_reference operator*() const noexcept {
      return *address();
    }

    [[nodiscard]] pointer address() noexcept {
      return reinterpret_cast<pointer>(Address);
    }

    [[nodiscard]] const_pointer address() const noexcept {
      return reinterpret_cast<const_pointer>(Address);
    }

    IOPortRW& operator++() noexcept {
      return *this;
    }

    IOPortRW operator++(int) noexcept {
      return *this;
    }

    [[nodiscard]] bool operator==(IOPortRW /*unused*/) const noexcept {
      return true;
    }
  };

  template <TriviallyCopyable T, frt::usize Address> class IOPortWOS {
  public:
    using value_type = T;
    using difference_type = std::ptrdiff_t;
    using pointer = volatile T*;
    using reference = volatile T&;
    using const_pointer = const volatile T*;
    using const_reference = const volatile T&;

    IOPortWOS& operator=(T value) noexcept {
      write(value);

      return *this;
    }

    [[nodiscard]] value_type read() const noexcept {
      return value_;
    }

    [[nodiscard]] operator value_type() const noexcept { // NOLINT(google-explicit-constructor)
      return read();
    }

    void write(T value) noexcept {
      value_ = value;

      frt::volatile_write(address(), value);
    }

    [[nodiscard]] const_reference operator*() const noexcept {
      return value_;
    }

    [[nodiscard]] pointer address() noexcept {
      return reinterpret_cast<pointer>(Address);
    }

    [[nodiscard]] const_pointer address() const noexcept {
      return reinterpret_cast<const_pointer>(Address);
    }

    IOPortWOS& operator++() noexcept {
      return *this;
    }

    IOPortWOS operator++(int) noexcept {
      return *this;
    }

    [[nodiscard]] bool operator==(IOPortWOS /*unused*/) const noexcept {
      return true;
    }

  private:
    T value_;
  };
} // namespace frt