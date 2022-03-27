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

  namespace internal {
    template <typename> inline constexpr bool is_reference_wrapper = false;

    template <typename T> inline constexpr bool is_reference_wrapper<frt::ReferenceWrapper<T>> = true;

    template <class T> struct invoke_impl {
      template <typename F, typename... Args>
      static auto call(F&& f, Args&&... args) noexcept(noexcept(frt::forward<F>(f)(frt::forward<Args>(args)...)))
          -> decltype(frt::forward<F>(f)(frt::forward<Args>(args)...));
    };

    template <typename B, typename MT> struct invoke_impl<MT B::*> {
      template <typename T,
          typename Td = typename traits::Decay<T>,
          typename = typename traits::EnableIf<traits::is_base_of<B, Td>>>
      static auto get(T&& t) noexcept -> T&&;

      template <typename T,
          typename Td = typename traits::Decay<T>,
          typename = typename traits::EnableIf<is_reference_wrapper<Td>>>
      static auto get(T&& t) noexcept -> decltype(t.get());

      template <typename T,
          typename Td = typename traits::Decay<T>,
          typename = typename traits::EnableIf<!traits::is_base_of<B, Td>>,
          typename = typename traits::EnableIf<!is_reference_wrapper<Td>>>
      static auto get(T&& t) noexcept(noexcept(*frt::forward<T>(t))) -> decltype(*frt::forward<T>(t));

      template <typename T,
          typename... Args,
          typename MT1,
          typename = typename traits::EnableIf<traits::is_function<MT1>>>
      static auto call(MT1 B::*pmf, T&& t, Args&&... args) noexcept(
          noexcept((invoke_impl::get(frt::forward<T>(t)).*pmf)(frt::forward<Args>(args)...)))
          -> decltype((invoke_impl::get(frt::forward<T>(t)).*pmf)(frt::forward<Args>(args)...));

      template <typename T>
      static auto call(MT B::*pmd, T&& t) noexcept(noexcept(invoke_impl::get(frt::forward<T>(t)).*pmd))
          -> decltype(invoke_impl::get(frt::forward<T>(t)).*pmd);
    };

    template <typename F, typename... Args, typename Fd = typename traits::Decay<F>>
    auto INVOKE(F&& f, Args&&... args) noexcept(
        noexcept(invoke_impl<Fd>::call(frt::forward<F>(f), frt::forward<Args>(args)...)))
        -> decltype(invoke_impl<Fd>::call(frt::forward<F>(f), frt::forward<Args>(args)...));

    template <typename AlwaysVoid, typename, typename...> struct InvokeResultTraitImpl {};

    template <typename F, typename... Args>
    struct InvokeResultTraitImpl<
        frt::traits::Void<decltype(internal::INVOKE(traits::declval<F>(), traits::declval<Args>()...))>,
        F,
        Args...> {
      using type = decltype(internal::INVOKE(traits::declval<F>(), traits::declval<Args>()...));
    };
  } // namespace internal

  namespace traits {
    template <typename F, typename... Args>
    using InvokeResult = typename frt::internal::InvokeResultTraitImpl<void, F, Args...>::type;

    template <typename F, typename... Args>
    struct InvokeResultTrait : frt::internal::InvokeResultTraitImpl<void, F, Args...> {};
  } // namespace traits

  namespace internal {
    template <typename F, typename... Args>
    concept IsInvocable = requires(F&& f, Args&&... args) {
      internal::INVOKE(frt::forward<F>(f), frt::forward<Args>(args)...);
    };

    template <typename F, typename... Args>
    concept IsNothrowInvocable = requires(F&& f, Args&&... args) {
      { internal::INVOKE(frt::forward<F>(f), frt::forward<Args>(args)...) }
      noexcept;
    };

    template <typename From, typename To>
    concept InvokeConvertible = traits::is_void<To> || traits::is_convertible<From, To>;

    template <typename From, typename To>
    concept InvokeNothrowConvertible = traits::is_void<To> || traits::is_nothrow_convertible<From, To>;

    template <typename R, typename F, typename... Args>
    concept IsInvocableR = IsInvocable<F, Args...> && InvokeConvertible<traits::InvokeResult<F, Args...>, R>;

    template <typename R, typename F, typename... Args>
    concept IsNothrowInvocableR =
        IsNothrowInvocable<F, Args...> && InvokeNothrowConvertible<traits::InvokeResult<F, Args...>, R>;

    template <class C, class Pointed, class T1, class... Args>
    constexpr decltype(auto) invoke_memptr(Pointed C::*f, T1&& t1, Args&&... args) {
      if constexpr (traits::is_function<Pointed>) {
        if constexpr (traits::is_base_of<C, traits::Decay<T1>>) {
          return (frt::forward<T1>(t1).*f)(frt::forward<Args>(args)...);
        } else if constexpr (is_reference_wrapper<traits::Decay<T1>>) {
          return (t1.get().*f)(frt::forward<Args>(args)...);
        } else {
          return ((*frt::forward<T1>(t1)).*f)(frt::forward<Args>(args)...);
        }
      } else {
        static_assert(traits::is_object<Pointed> && sizeof...(args) == 0);
        if constexpr (traits::is_base_of<C, traits::Decay<T1>>) {
          return frt::forward<T1>(t1).*f;
        } else if constexpr (is_reference_wrapper<traits::Decay<T1>>) {
          return t1.get().*f;
        } else {
          return (*frt::forward<T1>(t1)).*f;
        }
      }
    }

  } // namespace internal

  namespace traits {
    template <typename F, typename... Args> inline constexpr bool is_invocable = frt::internal::IsInvocable<F, Args...>;

    template <typename F, typename... Args> struct IsInvocableTrait : BoolConstant<is_invocable<F, Args...>> {};

    template <typename F, typename... Args>
    inline constexpr bool is_nothrow_invocable = frt::internal::IsNothrowInvocable<F, Args...>;

    template <typename F, typename... Args>
    struct IsNothrowInvocableTrait : BoolConstant<is_nothrow_invocable<F, Args...>> {};

    template <typename R, typename F, typename... Args>
    inline constexpr bool is_invocable_r = frt::internal::IsInvocableR<R, F, Args...>;

    template <typename R, typename F, typename... Args>
    struct IsInvocableRTrait : BoolConstant<is_invocable_r<R, F, Args...>> {};

    template <typename R, typename F, typename... Args>
    inline constexpr bool is_nothrow_invocable_r = frt::internal::IsNothrowInvocableR<R, F, Args...>;

    template <typename R, typename F, typename... Args>
    struct IsNothrowInvocableRTrait : BoolConstant<is_nothrow_invocable_r<R, F, Args...>> {};
  } // namespace traits

  template <typename F, typename... Args>
  constexpr traits::InvokeResult<F, Args...> invoke(F&& f, Args&&... args) noexcept(
      traits::is_nothrow_invocable<F, Args...>) requires traits::is_invocable<F, Args...> {
    if constexpr (traits::is_member_pointer<traits::Decay<F>>) {
      return internal::invoke_memptr(f, frt::forward<Args>(args)...);
    } else {
      return frt::forward<F>(f)(frt::forward<Args>(args)...);
    }
  }

  template <typename R, typename F, typename... Args>
  constexpr R invoke_r(F&& f, Args&&... args) noexcept(
      traits::is_nothrow_invocable_r<R, F, Args...>) requires traits::is_invocable_r<R, F, Args...> {
    return frt::invoke(frt::forward<F>(f), frt::forward<Args>(args)...);
  }

  namespace internal {
    template <typename T> void ref_wrapper_fun(T&) noexcept;

    template <typename T> void ref_wrapper_fun(T &&) = delete;
  } // namespace internal

  template <typename T>
  requires traits::is_object<T> || traits::is_function<T>
  class ReferenceWrapper { // NOLINT(fuchsia-trailing-return)
  public:
    template <typename U>
    // NOLINTNEXTLINE(google-explicit-constructor, bugprone-forwarding-reference-overload, fuchsia-trailing-return)
    constexpr ReferenceWrapper(U&& ref) noexcept(traits::is_nothrow_convertible<T, U&&>) requires(
        !traits::is_same<traits::Decay<U>,
            ReferenceWrapper> && requires { internal::ref_wrapper_fun<T>(traits::declval<U>()); })
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

  template <typename T> [[nodiscard]] constexpr ReferenceWrapper<T> ref(ReferenceWrapper<T> ref) noexcept {
    return ref;
  }

  template <typename T> void ref(const T&&) = delete;

  template <typename T> [[nodiscard]] constexpr ReferenceWrapper<const T> cref(const T& ref) noexcept {
    return ReferenceWrapper<const T>(ref);
  }

  template <typename T> [[nodiscard]] constexpr ReferenceWrapper<const T> cref(ReferenceWrapper<T> ref) noexcept {
    return ref;
  }

  template <typename T> void cref(const T&&) = delete;
} // namespace frt