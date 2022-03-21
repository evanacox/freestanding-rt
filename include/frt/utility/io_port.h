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
  namespace internal {
    template <typename T, frt::usize Address>
    class IOPortForwardIterator
        : public frt::IteratorInterface<IOPortForwardIterator<T, Address>, frt::ForwardIteratorTag, T> {
    public:
      IOPortForwardIterator() = default;

      explicit IOPortForwardIterator(bool begin) : begin_{begin} {}

      [[nodiscard]] volatile T& value() const noexcept {
        return *address();
      }

      void next() noexcept {
        begin_ = false;
      }

      [[nodiscard]] bool eq(IOPortForwardIterator other) const noexcept {
        return begin_ && other.begin_;
      }

    private:
      [[nodiscard]] volatile T* address() const noexcept {
        return reinterpret_cast<volatile T*>(Address);
      }

      bool begin_ = false;
    };

    template <typename T, frt::usize Address>
    class IOPortOutputIterator
        : public frt::IteratorInterface<IOPortForwardIterator<T, Address>, frt::OutputIteratorTag, T, volatile T&> {
    public:
      explicit IOPortOutputIterator(bool begin) : begin_{begin} {}

      volatile T& value() const noexcept {
        return *address();
      }

      void next() noexcept {
        begin_ = false;
      }

      bool eq(IOPortOutputIterator other) const noexcept {
        return begin_ && other.begin_;
      }

    private:
      [[nodiscard]] volatile T* address() const noexcept {
        return reinterpret_cast<volatile T*>(Address);
      }

      bool begin_;
    };
  } // namespace internal

  template <TriviallyCopyable T, frt::usize Address> class IOPortRO {
  public:
    using value_type = T;
    using difference_type = frt::isize;
    using pointer = volatile T*;
    using reference = volatile T&;
    using const_pointer = const volatile T*;
    using const_reference = const volatile T&;
    using iterator = internal::IOPortForwardIterator<const T, Address>;
    using const_iterator = iterator;

    [[nodiscard]] value_type read() const noexcept {
      return frt::volatile_read(address());
    }

    [[nodiscard]] operator value_type() const noexcept { // NOLINT(google-explicit-constructor)
      return read();
    }

    [[nodiscard]] const_pointer address() const noexcept {
      return reinterpret_cast<const_pointer>(Address);
    }

    [[nodiscard]] constexpr const_iterator begin() const noexcept {
      return const_iterator{true};
    }

    [[nodiscard]] constexpr const_iterator end() const noexcept {
      return const_iterator{false};
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
    using iterator = internal::IOPortOutputIterator<T, Address>;

    void write(T value) noexcept {
      frt::volatile_write(address(), value);
    }

    IOPortWO& operator=(T value) noexcept {
      write(value);

      return *this;
    }

    [[nodiscard]] pointer address() noexcept {
      return reinterpret_cast<pointer>(Address);
    }

    [[nodiscard]] const_pointer address() const noexcept {
      return reinterpret_cast<const_pointer>(Address);
    }

    [[nodiscard]] constexpr iterator begin() const noexcept {
      return iterator{true};
    }

    [[nodiscard]] constexpr iterator end() const noexcept {
      return iterator{false};
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
    using iterator = internal::IOPortForwardIterator<T, Address>;
    using const_iterator = internal::IOPortForwardIterator<const T, Address>;

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

    [[nodiscard]] pointer address() noexcept {
      return reinterpret_cast<pointer>(Address);
    }

    [[nodiscard]] const_pointer address() const noexcept {
      return reinterpret_cast<const_pointer>(Address);
    }

    [[nodiscard]] constexpr iterator begin() noexcept {
      return iterator{true};
    }

    [[nodiscard]] constexpr iterator end() noexcept {
      return iterator{false};
    }

    [[nodiscard]] constexpr const_iterator begin() const noexcept {
      return const_iterator{true};
    }

    [[nodiscard]] constexpr const_iterator end() const noexcept {
      return const_iterator{false};
    }

    [[nodiscard]] constexpr const_iterator cbegin() const noexcept {
      return const_iterator{true};
    }

    [[nodiscard]] constexpr const_iterator cend() const noexcept {
      return const_iterator{false};
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

    [[nodiscard]] pointer address() noexcept {
      return reinterpret_cast<pointer>(Address);
    }

    [[nodiscard]] const_pointer address() const noexcept {
      return reinterpret_cast<const_pointer>(Address);
    }

    [[nodiscard]] bool operator==(IOPortWOS /*unused*/) const noexcept {
      return true;
    }

  private:
    T value_;
  };

  using t = IteratorTraits<typename IOPortRW<int, 0x800>::iterator>;

  t f() {
    return t{};
  }

} // namespace frt