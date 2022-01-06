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

#include "../types/basic.h"
#include "../types/concepts.h"
#include "../types/traits.h"
#include "./memory.h"

namespace frt {
  namespace internal {
    template <typename P, typename D = void> struct PointerTraitsDiffType { using type = frt::size; };

    template <typename P> struct PointerTraitsDiffType<P, traits::Void<typename P::difference_type>> {
      using type = typename P::difference_type;
    };

    template <typename P, typename = void> struct PointerTraitsElemType {
      static_assert(traits::internal::dependent_false<P>, "illegal type for `frt::PointerTraits`");
    };

    template <typename P> struct PointerTraitsElemType<P, traits::Void<typename P::element_type>> {
      using type = typename P::element_type;
    };

    template <template <typename...> typename P, typename T, typename... Args>
    struct PointerTraitsElemType<P<T, Args...>, void> {
      using type = T;
    };

    template <typename Ptr> struct PointerTraitsToAddress {
      using element_type = typename internal::PointerTraitsElemType<Ptr>::type;
    };

    template <template <typename...> typename Ptr, typename T, typename... Args>
    concept HasToAddress = requires(Ptr<T, Args...> p) {
      { Ptr<T, Args...>::to_address(p) } -> SameAs<typename PointerTraitsElemType<Ptr<T, Args...>>::type*>;
    };

    template <template <typename...> typename Ptr, typename T, typename... Args>
    requires HasToAddress<Ptr, T, Args...>
    struct PointerTraitsToAddress<Ptr<T, Args...>> {
      using element_type = typename internal::PointerTraitsElemType<Ptr<T, Args...>>::type;

      [[nodiscard]] FRT_ALWAYS_INLINE constexpr static element_type* to_address(Ptr<T, Args...> p) noexcept {
        return Ptr<T, Args...>::to_address(p);
      }
    };
  } // namespace internal

  /// Pointer traits specialization for "fancy" pointer types, wraps their operations
  /// and provides them in a consistent fashion
  ///
  /// See https://en.cppreference.com/w/cpp/memory/pointer_traits
  template <typename Ptr> struct PointerTraits : internal::PointerTraitsToAddress<Ptr> {
    /// The type of the pointer itself
    using pointer = Ptr;

    /// The type of the object being pointed to
    using element_type = typename internal::PointerTraitsToAddress<Ptr>::element_type;

    /// The difference type of the pointer
    using difference_type = typename internal::PointerTraitsDiffType<Ptr>::type;

    /// Rebinds the fancy pointer type to type `U`
    template <typename U> using rebind = traits::Rebind<U, Ptr>;

    /// Forwards `r` to `Ptr::pointer_to`.
    ///
    /// \param r The reference to forward
    /// \return `Ptr::pointer_to(r)`
    [[nodiscard]] FRT_ALWAYS_INLINE static constexpr pointer pointer_to(element_type& r) noexcept {
      return pointer::pointer_to(r);
    }
  };

  /// Specialization of `PointerTraits` for raw pointer types, simply wraps pointer types
  /// and provides the correct type aliases for them
  ///
  /// See https://en.cppreference.com/w/cpp/memory/pointer_traits
  template <typename T> struct PointerTraits<T*> {
    /// The type of the pointer itself
    using pointer = T*;

    /// The type of the object being pointed to
    using element_type = T;

    /// The difference type of the pointer
    using difference_type = frt::size;

    /// Rebinds the fancy pointer type to type `U`
    template <typename U> using rebind = U*;

  private:
    struct Hidden {};

    using reference = traits::Conditional<traits::is_void<element_type>, Hidden, element_type>;

  public:
    /// Forwards `r` to `frt::address_of(r)`
    ///
    /// \param r The reference to get a pointer to
    /// \return `frt::address_of(r)`
    [[nodiscard]] FRT_ALWAYS_INLINE static constexpr pointer pointer_to(reference& r) noexcept {
      return frt::address_of(r);
    }
  };

  /// Obtain the address represented by `p`
  ///
  /// \param p The pointer with an address
  /// \return The value of `p`
  template <typename T> [[nodiscard]] FRT_ALWAYS_INLINE constexpr T* to_address(T* p) noexcept {
    return p;
  }

  /// Obtain the address represented by `p`, using its `PointerTraits` specialization
  ///
  /// \param p A "fancy" pointer object
  /// \return The value of `PointerTraits<Ptr>::to_address(p)`
  template <typename Ptr> [[nodiscard]] FRT_ALWAYS_INLINE constexpr auto to_address(const Ptr& p) noexcept {
    return PointerTraits<Ptr>::to_address(p);
  }
} // namespace frt