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

#ifdef FRT_GENERATE_DEFAULT_STD_COMPARE

namespace std {
  namespace __frt { // NOLINT(readability-identifier-naming)
    using CmpVal = frt::ibyte;

    enum class CmpOrd : __frt::CmpVal { less = -1, equivalent = 0, greater = 1, unordered = 2 };

    struct Unspecified {
      // NOLINTNEXTLINE(google-explicit-constructor)
      constexpr Unspecified(int /*unused*/) noexcept {};
    };
  } // namespace __frt

  class partial_ordering { // NOLINT(readability-identifier-naming)
  public:
    const static std::partial_ordering less;
    const static std::partial_ordering equivalent;
    const static std::partial_ordering greater;
    const static std::partial_ordering unordered;

    // NOLINTNEXTLINE(readability-named-parameter)
    [[nodiscard]] friend constexpr bool operator==(std::partial_ordering val, __frt::Unspecified) noexcept {
      return val == __frt::CmpOrd::equivalent;
    }

    [[nodiscard]] friend constexpr bool operator==(std::partial_ordering, std::partial_ordering) noexcept = default;

    // NOLINTNEXTLINE(readability-named-parameter)
    [[nodiscard]] friend constexpr bool operator<(std::partial_ordering val, __frt::Unspecified) noexcept {
      return val == __frt::CmpOrd::less;
    }

    // NOLINTNEXTLINE(readability-named-parameter)
    [[nodiscard]] friend constexpr bool operator<(__frt::Unspecified, std::partial_ordering val) noexcept {
      return val == __frt::CmpOrd::greater;
    }

    // NOLINTNEXTLINE(readability-named-parameter)
    [[nodiscard]] friend constexpr bool operator<=(std::partial_ordering val, __frt::Unspecified) noexcept {
      return val <= __frt::CmpOrd::equivalent;
    }

    // NOLINTNEXTLINE(readability-named-parameter)
    [[nodiscard]] friend constexpr bool operator<=(__frt::Unspecified, std::partial_ordering val) noexcept {
      return (val & __frt::CmpOrd::greater) == val;
    }

    // NOLINTNEXTLINE(readability-named-parameter)
    [[nodiscard]] friend constexpr bool operator>(std::partial_ordering val, __frt::Unspecified) noexcept {
      return val == __frt::CmpOrd::greater;
    }

    // NOLINTNEXTLINE(readability-named-parameter)
    [[nodiscard]] friend constexpr bool operator>(__frt::Unspecified, std::partial_ordering val) noexcept {
      return val == __frt::CmpOrd::less;
    }

    // NOLINTNEXTLINE(readability-named-parameter)
    [[nodiscard]] friend constexpr bool operator>=(std::partial_ordering val, __frt::Unspecified) noexcept {
      return (val & __frt::CmpOrd::greater) == val;
    }

    // NOLINTNEXTLINE(readability-named-parameter)
    [[nodiscard]] friend constexpr bool operator>=(__frt::Unspecified, std::partial_ordering val) noexcept {
      return val <= __frt::CmpOrd::equivalent;
    }

    [[nodiscard]] friend constexpr std::partial_ordering operator<=>(std::partial_ordering val,
        __frt::Unspecified) noexcept { // NOLINT(readability-named-parameter)
      return val;
    }

    // NOLINTNEXTLINE(readability-named-parameter)
    [[nodiscard]] friend constexpr std::partial_ordering operator<=>(__frt::Unspecified,
        std::partial_ordering val) noexcept {
      // if val == less or greater, return the opposite
      if ((val & __frt::CmpOrd::greater) != 0) {
        return std::partial_ordering(static_cast<__frt::CmpOrd>(-val.value_));
      }

      return val;
    }

  private:
    constexpr explicit partial_ordering(__frt::CmpOrd val) noexcept : value_{static_cast<__frt::CmpVal>(val)} {}

    [[nodiscard]] friend constexpr bool operator==(std::partial_ordering val, __frt::CmpOrd ord) noexcept {
      return val.value_ == static_cast<__frt::CmpVal>(ord);
    }

    [[nodiscard]] friend constexpr bool operator<=(std::partial_ordering val, __frt::CmpOrd ord) noexcept {
      return val.value_ <= static_cast<__frt::CmpVal>(ord);
    }

    [[nodiscard]] friend constexpr bool operator>=(std::partial_ordering val, __frt::CmpOrd ord) noexcept {
      return val.value_ >= static_cast<__frt::CmpVal>(ord);
    }

    [[nodiscard]] friend constexpr __frt::CmpVal operator&(std::partial_ordering val, __frt::CmpOrd ord) noexcept {
      return static_cast<__frt::CmpVal>(val.value_ & static_cast<__frt::CmpVal>(ord));
    }

    friend class weak_ordering;
    friend class strong_ordering;

    __frt::CmpVal value_;
  };

  inline constexpr std::partial_ordering std::partial_ordering::less{__frt::CmpOrd::less};
  inline constexpr std::partial_ordering std::partial_ordering::equivalent{__frt::CmpOrd::equivalent};
  inline constexpr std::partial_ordering std::partial_ordering::greater{__frt::CmpOrd::greater};
  inline constexpr std::partial_ordering std::partial_ordering::unordered{__frt::CmpOrd::unordered};

  class weak_ordering { // NOLINT(readability-identifier-naming)
  public:
    const static std::weak_ordering less;
    const static std::weak_ordering equivalent;
    const static std::weak_ordering greater;

    // NOLINTNEXTLINE(google-explicit-constructor)
    [[nodiscard]] constexpr operator std::partial_ordering() const noexcept {
      return std::partial_ordering{static_cast<__frt::CmpOrd>(value_)};
    }

    // NOLINTNEXTLINE(readability-named-parameter)
    [[nodiscard]] friend constexpr bool operator==(std::weak_ordering val, __frt::Unspecified) noexcept {
      return val == __frt::CmpOrd::equivalent;
    }

    [[nodiscard]] friend constexpr bool operator==(std::weak_ordering, std::weak_ordering) noexcept = default;

    // NOLINTNEXTLINE(readability-named-parameter)
    [[nodiscard]] friend constexpr bool operator<(std::weak_ordering val, __frt::Unspecified) noexcept {
      return val < __frt::CmpOrd::equivalent;
    }

    // NOLINTNEXTLINE(readability-named-parameter)
    [[nodiscard]] friend constexpr bool operator<(__frt::Unspecified, std::weak_ordering val) noexcept {
      return val > __frt::CmpOrd::equivalent;
    }

    // NOLINTNEXTLINE(readability-named-parameter)
    [[nodiscard]] friend constexpr bool operator<=(std::weak_ordering val, __frt::Unspecified) noexcept {
      return val <= __frt::CmpOrd::equivalent;
    }

    // NOLINTNEXTLINE(readability-named-parameter)
    [[nodiscard]] friend constexpr bool operator<=(__frt::Unspecified, std::weak_ordering val) noexcept {
      return val >= __frt::CmpOrd::equivalent;
    }

    // NOLINTNEXTLINE(readability-named-parameter)
    [[nodiscard]] friend constexpr bool operator>(std::weak_ordering val, __frt::Unspecified) noexcept {
      return val > __frt::CmpOrd::equivalent;
    }

    // NOLINTNEXTLINE(readability-named-parameter)
    [[nodiscard]] friend constexpr bool operator>(__frt::Unspecified, std::weak_ordering val) noexcept {
      return val < __frt::CmpOrd::equivalent;
    }

    // NOLINTNEXTLINE(readability-named-parameter)
    [[nodiscard]] friend constexpr bool operator>=(std::weak_ordering val, __frt::Unspecified) noexcept {
      return val >= __frt::CmpOrd::equivalent;
    }

    // NOLINTNEXTLINE(readability-named-parameter)
    [[nodiscard]] friend constexpr bool operator>=(__frt::Unspecified, std::weak_ordering val) noexcept {
      return val <= __frt::CmpOrd::equivalent;
    }

    // NOLINTNEXTLINE(readability-named-parameter)
    [[nodiscard]] friend constexpr std::weak_ordering operator<=>(std::weak_ordering val, __frt::Unspecified) noexcept {
      return val;
    }

    // NOLINTNEXTLINE(readability-named-parameter)
    [[nodiscard]] friend constexpr std::weak_ordering operator<=>(__frt::Unspecified, std::weak_ordering val) noexcept {
      // if val == less or greater, return the opposite
      if ((val & __frt::CmpOrd::greater) != 0) {
        return std::weak_ordering(static_cast<__frt::CmpOrd>(-val.value_));
      }

      return val;
    }

  private:
    constexpr explicit weak_ordering(__frt::CmpOrd val) noexcept : value_{static_cast<__frt::CmpVal>(val)} {}

    [[nodiscard]] friend constexpr bool operator==(std::weak_ordering val, __frt::CmpOrd ord) noexcept {
      return val.value_ == static_cast<__frt::CmpVal>(ord);
    }

    [[nodiscard]] friend constexpr bool operator<=(std::weak_ordering val, __frt::CmpOrd ord) noexcept {
      return val.value_ <= static_cast<__frt::CmpVal>(ord);
    }

    [[nodiscard]] friend constexpr bool operator>=(std::weak_ordering val, __frt::CmpOrd ord) noexcept {
      return val.value_ >= static_cast<__frt::CmpVal>(ord);
    }

    [[nodiscard]] friend constexpr bool operator<(std::weak_ordering val, __frt::CmpOrd ord) noexcept {
      return val.value_ < static_cast<__frt::CmpVal>(ord);
    }

    [[nodiscard]] friend constexpr bool operator>(std::weak_ordering val, __frt::CmpOrd ord) noexcept {
      return val.value_ > static_cast<__frt::CmpVal>(ord);
    }

    [[nodiscard]] friend constexpr __frt::CmpVal operator&(std::weak_ordering val, __frt::CmpOrd ord) noexcept {
      return static_cast<__frt::CmpVal>(val.value_ & static_cast<__frt::CmpVal>(ord));
    }

    friend class strong_ordering;

    __frt::CmpVal value_;
  };

  inline constexpr std::weak_ordering std::weak_ordering::less{__frt::CmpOrd::less};
  inline constexpr std::weak_ordering std::weak_ordering::equivalent{__frt::CmpOrd::equivalent};
  inline constexpr std::weak_ordering std::weak_ordering::greater{__frt::CmpOrd::greater};

  class strong_ordering { // NOLINT(readability-identifier-naming)
  public:
    const static std::strong_ordering less;
    const static std::strong_ordering equal;
    const static std::strong_ordering equivalent;
    const static std::strong_ordering greater;

    // NOLINTNEXTLINE(google-explicit-constructor)
    [[nodiscard]] constexpr operator std::partial_ordering() const noexcept {
      return std::partial_ordering{static_cast<__frt::CmpOrd>(value_)};
    }

    // NOLINTNEXTLINE(google-explicit-constructor)
    [[nodiscard]] constexpr operator std::weak_ordering() const noexcept {
      return std::weak_ordering{static_cast<__frt::CmpOrd>(value_)};
    }

    // NOLINTNEXTLINE(readability-named-parameter)
    [[nodiscard]] friend constexpr bool operator==(std::strong_ordering val, __frt::Unspecified) noexcept {
      return val == __frt::CmpOrd::equivalent;
    }

    [[nodiscard]] friend constexpr bool operator==(std::strong_ordering, std::strong_ordering) noexcept = default;

    // NOLINTNEXTLINE(readability-named-parameter)
    [[nodiscard]] friend constexpr bool operator<(std::strong_ordering val, __frt::Unspecified) noexcept {
      return val < __frt::CmpOrd::equivalent;
    }

    // NOLINTNEXTLINE(readability-named-parameter)
    [[nodiscard]] friend constexpr bool operator<(__frt::Unspecified, std::strong_ordering val) noexcept {
      return val > __frt::CmpOrd::equivalent;
    }

    // NOLINTNEXTLINE(readability-named-parameter)
    [[nodiscard]] friend constexpr bool operator<=(std::strong_ordering val, __frt::Unspecified) noexcept {
      return val <= __frt::CmpOrd::equivalent;
    }

    // NOLINTNEXTLINE(readability-named-parameter)
    [[nodiscard]] friend constexpr bool operator<=(__frt::Unspecified, std::strong_ordering val) noexcept {
      return val >= __frt::CmpOrd::equivalent;
    }

    // NOLINTNEXTLINE(readability-named-parameter)
    [[nodiscard]] friend constexpr bool operator>(std::strong_ordering val, __frt::Unspecified) noexcept {
      return val > __frt::CmpOrd::equivalent;
    }

    // NOLINTNEXTLINE(readability-named-parameter)
    [[nodiscard]] friend constexpr bool operator>(__frt::Unspecified, std::strong_ordering val) noexcept {
      return val < __frt::CmpOrd::equivalent;
    }

    // NOLINTNEXTLINE(readability-named-parameter)
    [[nodiscard]] friend constexpr bool operator>=(std::strong_ordering val, __frt::Unspecified) noexcept {
      return val >= __frt::CmpOrd::equivalent;
    }

    // NOLINTNEXTLINE(readability-named-parameter)
    [[nodiscard]] friend constexpr bool operator>=(__frt::Unspecified, std::strong_ordering val) noexcept {
      return val <= __frt::CmpOrd::equivalent;
    }

    [[nodiscard]] friend constexpr std::strong_ordering operator<=>(std::strong_ordering val,
        __frt::Unspecified) noexcept { // NOLINT(readability-named-parameter)
      return val;
    }

    // NOLINTNEXTLINE(readability-named-parameter)
    [[nodiscard]] friend constexpr std::strong_ordering operator<=>(__frt::Unspecified,
        std::strong_ordering val) noexcept {
      // if val == less or greater, return the opposite
      if ((val & __frt::CmpOrd::greater) != 0) {
        return std::strong_ordering(static_cast<__frt::CmpOrd>(-val.value_));
      }

      return val;
    }

  private:
    constexpr explicit strong_ordering(__frt::CmpOrd val) noexcept : value_{static_cast<__frt::CmpVal>(val)} {}

    [[nodiscard]] friend constexpr bool operator==(std::strong_ordering val, __frt::CmpOrd ord) noexcept {
      return val.value_ == static_cast<__frt::CmpVal>(ord);
    }

    [[nodiscard]] friend constexpr bool operator<=(std::strong_ordering val, __frt::CmpOrd ord) noexcept {
      return val.value_ <= static_cast<__frt::CmpVal>(ord);
    }

    [[nodiscard]] friend constexpr bool operator>=(std::strong_ordering val, __frt::CmpOrd ord) noexcept {
      return val.value_ >= static_cast<__frt::CmpVal>(ord);
    }

    [[nodiscard]] friend constexpr bool operator<(std::strong_ordering val, __frt::CmpOrd ord) noexcept {
      return val.value_ < static_cast<__frt::CmpVal>(ord);
    }

    [[nodiscard]] friend constexpr bool operator>(std::strong_ordering val, __frt::CmpOrd ord) noexcept {
      return val.value_ > static_cast<__frt::CmpVal>(ord);
    }

    [[nodiscard]] friend constexpr __frt::CmpVal operator&(std::strong_ordering val, __frt::CmpOrd ord) noexcept {
      return static_cast<__frt::CmpVal>(val.value_ & static_cast<__frt::CmpVal>(ord));
    }

    __frt::CmpVal value_;
  };

  inline constexpr std::strong_ordering std::strong_ordering::less{__frt::CmpOrd::less};
  inline constexpr std::strong_ordering std::strong_ordering::equivalent{__frt::CmpOrd::equivalent};
  inline constexpr std::strong_ordering std::strong_ordering::equal{__frt::CmpOrd::equivalent};
  inline constexpr std::strong_ordering std::strong_ordering::greater{__frt::CmpOrd::greater};
} // namespace std

namespace frt {
  inline constexpr bool generated_compare = true;
}

#else
#include <compare>

namespace frt {
  inline constexpr bool generated_compare = false;
}
#endif

namespace frt {
  /// Strong ordering, corresponds to `std::strong_ordering`
  using StrongOrder = std::strong_ordering;

  /// Weak ordering, corresponds to `std::weak_ordering`
  using WeakOrder = std::weak_ordering;

  /// Partial ordering, corresponds to `std::partial_ordering`
  using PartialOrder = std::partial_ordering;
} // namespace frt