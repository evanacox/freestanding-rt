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

#include "../../core/iterators.h"
#include "../../types/traits.h"
#include "../../utility/construct.h"
#include "./member_types.h"

namespace frt {
  namespace internal {
    template <typename T> class VecIterator : public IteratorInterface<VecIterator<T>, frt::ContiguousIteratorTag, T> {
      inline static constexpr bool is_const = traits::is_const<T>;

      using Base = IteratorInterface<VecIterator<T>, frt::ContiguousIteratorTag, T>;

    public:
      FRT_USING_ITERATOR_TYPES(Base);

      VecIterator() noexcept = default;

      explicit VecIterator(internal::IteratorInit /*unused*/, pointer data) noexcept : data_{data} {}

      VecIterator(const VecIterator&) noexcept = default;

      // NOLINTNEXTLINE(google-explicit-constructor)
      VecIterator(const VecIterator<traits::RemoveConst<T>>& other) noexcept requires is_const : data_{other.data_} {}

      VecIterator(VecIterator&&) noexcept = default;

      VecIterator& operator=(const VecIterator&) noexcept = default;

      VecIterator& operator=(VecIterator&&) noexcept = default;

      // return the value that the iterator yields, aka what `operator*` should return
      [[nodiscard]] constexpr pointer address() const noexcept {
        return data_;
      }

      constexpr void move(difference_type n) noexcept {
        data_ += n;
      }

      [[nodiscard]] constexpr difference_type distance_to(const VecIterator& other) const noexcept {
        return other.data_ - data_;
      }

      [[nodiscard]] constexpr frt::StrongOrder cmp(const VecIterator& other) const noexcept {
        return data_ <=> other.data_;
      }

    private:
      T* data_;
    };

    template <typename T> struct StaticVecTraits {
      using Value = T;

      inline static constexpr bool can_resize = false;
      inline static constexpr bool has_inline_storage = false;
    };

    /// A raw vector, used as the underlying implementation for other vec types
    ///
    /// \tparam Traits A traits object that defines how the vector operates
    template <typename Traits, typename T = typename Traits::Value>
    class RawVec : public ContainerTypes<T, VecIterator<T>, VecIterator<const T>> {
      using Base = ContainerTypes<T, VecIterator<T>, VecIterator<const T>>;

    public:
      FRT_USING_CONTAINER_TYPES(Base);

      constexpr reference push_back(const T& value) {
        // NEED to return here, if we don't GCC completely stumbles on this, and ends
        // up shoving a ton of code that screws with the stack into the hot path
        if (FRT_UNLIKELY(len_ >= cap_)) {
          return realloc_insert(value);
        }

        return push_end(value);
      }

      template <typename... Args> constexpr reference emplace_back(Args&&... /*unused*/) {
        //
      }

    protected:
      constexpr explicit RawVec(pointer data, size_type cap) : data_{data}, len_{0}, cap_{cap} {}

      constexpr ~RawVec() = default;

    private:
      template <typename U> FRT_COLD constexpr reference realloc_insert(U&& value) {
        return push_end(frt::forward<U>(value));
      }

      template <typename... Args> FRT_ALWAYS_INLINE constexpr reference push_end(Args&&... args) {
        FRT_ASSERT(len_ < cap_, "cannot append to full vector");

        frt::construct_at(data_ + len_, frt::forward<Args>(args)...);

        return data_[len_++];
      }

      pointer data_;
      size_type len_;
      size_type cap_;
    };
  } // namespace internal

  /// Used to accept an arbitrary vector-like type without any templates. This is effectively a
  /// polymorphic wrapper for "vector-ish" things, and is to vectors as `std::function` is to a callable.
  ///
  /// It's implemented through instantiating templated derived classes and passing those to any code
  /// that expects a `VecRef&` or `const VecRef&`.
  ///
  /// \tparam T The type of object held inside the underlying vector
  template <typename T>
  class VecRef : public internal::ContainerTypes<T, internal::VecIterator<T>, internal::VecIterator<const T>> {
    using Base = internal::ContainerTypes<T, internal::VecIterator<T>, internal::VecIterator<const T>>;

  public:
    FRT_USING_CONTAINER_TYPES(Base);

    [[nodiscard]] constexpr virtual reference push_back(const value_type& value) noexcept = 0;

    [[nodiscard]] constexpr virtual reference push_back(value_type&& value) noexcept = 0;

    virtual ~VecRef() = default;
  };

  namespace internal {
    template <typename Underlying, typename T = typename Underlying::value_type>
    class VecRefImpl final : public VecRef<T> {
      using Base = VecRef<T>;

    public:
      FRT_USING_CONTAINER_TYPES(Base);

      explicit VecRefImpl(Underlying& vec) noexcept : vec_{frt::address_of(vec)} {}

      VecRefImpl(const VecRefImpl&) noexcept = default;

      VecRefImpl(VecRefImpl&&) noexcept = default;

      VecRefImpl& operator=(const VecRefImpl&) = delete;

      VecRefImpl& operator=(VecRefImpl&&) = delete;

      [[nodiscard]] constexpr reference push_back(const value_type& value) noexcept final {
        return vec_->push_back(value);
      }

    private:
      Underlying* vec_;
    };
  } // namespace internal

  template <typename Vec> constexpr internal::VecRefImpl<Vec> vec_ref(Vec& vec) noexcept {
    return internal::VecRefImpl<Vec>(vec);
  }
} // namespace frt