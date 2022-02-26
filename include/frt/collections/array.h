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
#include "../core/limits.h"
#include "../runtime/assert.h"
#include "../runtime/failures.h"
#include "../types/basic.h"
#include "../utility/swap.h"
#include "./internal/member_types.h"

namespace frt {
  /// A freestanding-compatible version of `std::array`.
  ///
  /// \tparam T The type held in the array
  /// \tparam N The length of the array
  template <typename T, frt::usize N, frt::usize Align = alignof(T)>
  struct Array // NOLINT(fuchsia-trailing-return)
      : public internal::ContainerTypes<T> {
    using value_type = typename internal::ContainerTypes<T>::value_type;
    using size_type = typename internal::ContainerTypes<T>::size_type;
    using difference_type = typename internal::ContainerTypes<T>::difference_type;
    using reference = typename internal::ContainerTypes<T>::reference;
    using const_reference = typename internal::ContainerTypes<T>::const_reference;
    using pointer = typename internal::ContainerTypes<T>::pointer;
    using const_pointer = typename internal::ContainerTypes<T>::const_pointer;
    using iterator = pointer;
    using const_iterator = const_pointer;
    using reverse_iterator = ReverseIterator<iterator>;
    using const_reverse_iterator = ReverseIterator<const_iterator>;

    /// Performs a bounds-checked array access. If the index is out of bounds, `frt::bounds_fail`
    /// is called and this function does not return.
    ///
    /// \param index The index to access
    /// \return `array[index]`
    [[nodiscard]] constexpr reference at(size_type index) noexcept {
      if (index >= N) {
        frt::bounds_fail("array index out of bounds");
      }

      return __array[index];
    }

    /// Performs a bounds-checked array access. If the index is out of bounds, `frt::bounds_fail`
    /// is called and this function does not return.
    ///
    /// \param index The index to access
    /// \return `array[index]`
    [[nodiscard]] constexpr const_reference at(size_type index) const noexcept {
      if (index >= N) {
        frt::bounds_fail("array index out of bounds");
      }

      return __array[index];
    }

    /// Performs an unchecked array access. If the index is out of bounds, the behavior
    /// of this function is undefined.
    ///
    /// \param index The index to access
    /// \return `array[index]`
    [[nodiscard]] constexpr reference operator[](size_type index) noexcept {
      FRT_ASSERT(index < N, "index must be in bounds of array");

      return __array[index];
    }

    /// Performs an unchecked array access. If the index is out of bounds, the behavior
    /// of this function is undefined.
    ///
    /// \param index The index to access
    /// \return `array[index]`
    [[nodiscard]] constexpr const_reference operator[](size_type index) const noexcept {
      FRT_ASSERT(index < N, "index must be in bounds of array");

      return __array[index];
    }

    /// Gets the first element of the array. Equivalent to `[0]`.
    ///
    /// \return `array[0]`
    [[nodiscard]] constexpr reference front() noexcept {
      return __array[0];
    }

    /// Gets the first element of the array. Equivalent to `[0]`.
    ///
    /// \return `array[0]`
    [[nodiscard]] constexpr const_reference front() const noexcept {
      return __array[0];
    }

    /// Gets the last element of the array. Equivalent to `[N - 1]`.
    ///
    /// \return `array[N - 1]`
    [[nodiscard]] constexpr reference back() noexcept {
      return __array[N - 1];
    }

    /// Gets the last element of the array. Equivalent to `[N - 1]`.
    ///
    /// \return `array[N - 1]`
    [[nodiscard]] constexpr const_reference back() const noexcept {
      return __array[N - 1];
    }

    /// Gets a pointer to the first element of the array. This array is valid to access
    /// in the range `[ptr, ptr + N)`.
    ///
    /// \return `&array[0]`
    [[nodiscard]] constexpr pointer data() noexcept {
      return __array;
    }

    /// Gets a pointer to the first element of the array. This array is valid to access
    /// in the range `[ptr, ptr + N)`.
    ///
    /// \return `&array[0]`
    [[nodiscard]] constexpr const_pointer data() const noexcept {
      return __array;
    }

    /// Checks if the array has no elements
    ///
    /// \return True if the array has no elements
    [[nodiscard]] constexpr bool empty() const noexcept {
      return N == 0;
    }

    /// Checks if the array has no elements
    ///
    /// \return True if the array has no elements
    [[nodiscard]] constexpr size_type size() const noexcept {
      return N;
    }

    /// Gets the maximum possible size of an array
    ///
    /// \return The largest possible array size
    [[nodiscard]] constexpr size_type max_size() const noexcept {
      return NumericLimits<difference_type>::max;
    }

    [[nodiscard]] constexpr iterator begin() noexcept {
      return data();
    }

    [[nodiscard]] constexpr const_iterator begin() const noexcept {
      return cbegin();
    }

    [[nodiscard]] constexpr const_iterator cbegin() const noexcept {
      return data();
    }

    [[nodiscard]] constexpr iterator end() noexcept {
      return data() + N;
    }

    [[nodiscard]] constexpr const_iterator end() const noexcept {
      return cend();
    }

    [[nodiscard]] constexpr const_iterator cend() const noexcept {
      return data() + N;
    }

    [[nodiscard]] constexpr iterator rbegin() noexcept {
      return frt::make_reverse_iterator(end());
    }

    [[nodiscard]] constexpr const_iterator rbegin() const noexcept {
      return crbegin();
    }

    [[nodiscard]] constexpr const_iterator crbegin() const noexcept {
      return frt::make_reverse_iterator(cend());
    }

    [[nodiscard]] constexpr iterator rend() noexcept {
      return frt::make_reverse_iterator(begin());
    }

    [[nodiscard]] constexpr const_iterator rend() const noexcept {
      return crend();
    }

    [[nodiscard]] constexpr const_iterator crend() const noexcept {
      return frt::make_reverse_iterator(cbegin());
    }

    constexpr void swap(Array& other) noexcept(traits::is_nothrow_swappable<T>) {
      for (auto i = size_type{0}; i < N; ++i) {
        frt::swap(__array[i], other.__array[i]);
      }
    }

    constexpr void fill(const value_type& value) noexcept(traits::is_nothrow_copy_assignable<value_type>) {
      for (auto& element : *this) {
        element = value;
      }
    }

    T __array[N]; // NOLINT(modernize-avoid-c-arrays, misc-non-private-member-variables-in-classes)
  };

  template <typename T, typename... Args>
  Array(T, Args...) -> Array<T, 1 + sizeof...(Args)>; // NOLINT(fuchsia-trailing-return)
} // namespace frt