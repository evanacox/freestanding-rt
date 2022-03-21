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

#include "../core/memory.h"
#include "./forward.h"
#include "./traits.h"

namespace frt {
  template <typename T>
  requires traits::is_object<T> || traits::is_function<T>
  class ReferenceWrapper;

  namespace traits {
    namespace internal {
      template <typename> inline constexpr bool is_reference_wrapper = false;

      template <typename T> inline constexpr bool is_reference_wrapper<frt::ReferenceWrapper<T>> = true;

      template <typename T, typename Type, typename U, typename... Args>
      constexpr decltype(auto) INVOKE(Type T::*f, U&& u, Args&&... args) {
        if constexpr (traits::is_member_function_pointer<decltype(f)>) {
          if constexpr (traits::is_base_of<T, traits::Decay<U>>) {
            return (frt::forward<U>(u).*f)(frt::forward<Args>(args)...);
          } else if constexpr (internal::is_reference_wrapper<traits::Decay<U>>) {
            return (u.get().*f)(frt::forward<Args>(args)...);
          } else {
            return ((*frt::forward<U>(u)).*f)(frt::forward<Args>(args)...);
          }
        } else {
          static_assert(traits::is_member_object_pointer<decltype(f)>,
              "internal::INVOKE: f must be an member object pointer if it's not a member function pointer");
          static_assert(sizeof...(args) == 0, "internal::INVOKE: argument list must be 0 long");

          if constexpr (traits::is_base_of<T, traits::Decay<U>>) {
            return frt::forward<U>(u).*f;
          } else if constexpr (internal::is_reference_wrapper<traits::Decay<U>>) {
            return u.get().*f;
          } else {
            return (*frt::forward<U>(u)).*f;
          }
        }
      }

      // implements INVOKE for normal callable objects
      template <typename F, typename... Args> constexpr decltype(auto) INVOKE(F&& f, Args&&... args) requires requires {
        frt::forward<F>(f)(frt::forward<Args>(args)...);
      }
      { return frt::forward<F>(f)(frt::forward<Args>(args)...); }

      template <typename F, typename... Args>
      concept IsInvocable = requires(F&& f, Args&&... args) {
        internal::INVOKE<F, Args...>(frt::forward<F>(f), frt::forward<Args>(args)...);
      };

      template <typename F, typename... Args>
      concept IsNothrowInvocable = requires(F&& f, Args&&... args) {
        // clang-format off
      { internal::INVOKE<F, Args...>(frt::forward<F>(f), frt::forward<Args>(args)...) } noexcept;
        // clang-format on
      };

      template <typename From, typename To>
      concept InvokeConvertible = traits::is_convertible<From, To>;

      template <typename R, typename F, typename... Args>
      concept IsInvocableR = requires(F&& f, Args&&... args) {
        { internal::INVOKE<F, Args...>(frt::forward<F>(f), frt::forward<Args>(args)...) } -> InvokeConvertible<R>;
      };

      template <typename R, typename F, typename... Args>
      concept IsNothrowInvocableR = requires(F&& f, Args&&... args) {
        // clang-format off
      { internal::INVOKE<F, Args...>(frt::forward<F>(f), frt::forward<Args>(args)...) } noexcept -> InvokeConvertible<R>;
        // clang-format on
      };
    } // namespace internal

    template <typename Void, typename, typename...> struct InvokeResultTrait {};

    template <typename F, typename... Args>
    struct InvokeResultTrait<decltype(internal::INVOKE(traits::declval<F&&>(), traits::declval<Args&&>()...)),
        F,
        Args...> {
      using type = decltype(internal::INVOKE(traits::declval<F&&>(), traits::declval<Args&&>()...));
    };

    template <typename F, typename... Args> using InvokeResult = typename InvokeResultTrait<F, Args...>::type;

    template <typename F, typename... Args> inline constexpr bool is_invocable = internal::IsInvocable<F, Args...>;

    template <typename F, typename... Args> struct IsInvocableTrait : BoolConstant<is_invocable<F, Args...>> {};

    template <typename F, typename... Args>
    inline constexpr bool is_nothrow_invocable = internal::IsNothrowInvocable<F, Args...>;

    template <typename F, typename... Args>
    struct IsNothrowInvocableTrait : BoolConstant<is_nothrow_invocable<F, Args...>> {};

    template <typename R, typename F, typename... Args>
    inline constexpr bool is_invocable_r = internal::IsInvocableR<R, F, Args...>;

    template <typename R, typename F, typename... Args>
    struct IsInvocableRTrait : BoolConstant<is_invocable_r<R, F, Args...>> {};

    template <typename R, typename F, typename... Args>
    inline constexpr bool is_nothrow_invocable_r = internal::IsNothrowInvocableR<R, F, Args...>;

    template <typename R, typename F, typename... Args>
    struct IsNothrowInvocableRTrait : BoolConstant<is_nothrow_invocable_r<R, F, Args...>> {};
  } // namespace traits

  template <typename F, typename... Args>
  constexpr traits::InvokeResult<F, Args...> invoke(F&& f, Args&&... args) noexcept(
      traits::is_nothrow_invocable<F, Args...>) requires requires {
    traits::internal::INVOKE(frt::forward<F>(f), frt::forward<Args>(args)...);
  }
  { return traits::internal::INVOKE(frt::forward<F>(f), frt::forward<Args>(args)...); }

  template <typename T>
  requires traits::is_object<T> || traits::is_function<T>
  class ReferenceWrapper { // NOLINT(fuchsia-trailing-return)
  public:
    template <typename U>
    // NOLINTNEXTLINE(google-explicit-constructor, bugprone-forwarding-reference-overload, fuchsia-trailing-return)
    constexpr ReferenceWrapper(U&& ref) noexcept(traits::is_nothrow_convertible<T, U&&>) requires(
        !traits::is_same<traits::Decay<U>, ReferenceWrapper>)
        : ptr_(frt::address_of(static_cast<T&>(frt::forward<U>(ref)))) {}

    // NOLINTNEXTLINE(fuchsia-trailing-return)
    constexpr ReferenceWrapper(const ReferenceWrapper&) noexcept = default;

    ReferenceWrapper& operator=(const ReferenceWrapper&) noexcept = default;

    [[nodiscard]] constexpr T& get() const noexcept {
      return *ptr_;
    }

    [[nodiscard]] constexpr operator T&() const noexcept { // NOLINT(google-explicit-constructor)
      return *ptr_;
    }

    template <typename... Args>
    constexpr traits::InvokeResult<T&, Args...> operator()(Args&&... args) const
        noexcept(traits::is_nothrow_invocable<decltype(get()), Args...>) {
      return frt::invoke(get(), frt::forward<Args>(args)...);
    }

  private:
    T* ptr_;
  };

  // NOLINTNEXTLINE(fuchsia-trailing-return)
  template <typename T> ReferenceWrapper(T&) -> ReferenceWrapper<T>;

  template <typename T> [[nodiscard]] constexpr ReferenceWrapper<T> ref(T& ref) noexcept {
    return ReferenceWrapper<T>(ref);
  }

  template <typename T> [[nodiscard]] constexpr ReferenceWrapper<T> ref(ReferenceWrapper<T>& ref) noexcept {
    return frt::ref(ref.get());
  }

  template <typename T> void ref(const T&&) = delete;

  template <typename T> [[nodiscard]] constexpr ReferenceWrapper<const T> cref(const T& ref) noexcept {
    return ReferenceWrapper<const T>(ref);
  }

  template <typename T> [[nodiscard]] constexpr ReferenceWrapper<const T> cref(ReferenceWrapper<T>& ref) noexcept {
    return frt::cref(ref.get());
  }

  template <typename T> void cref(const T&&) = delete;
} // namespace frt