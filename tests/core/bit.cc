//======---------------------------------------------------------------======//
//                                                                           //
// Copyright 2021-2022 Evan Cox <evanacox00@gmail.com>. All rights reserved. //
//                                                                           //
// Use of this source code is governed by a BSD-style license that can be    //
// found in the LICENSE.txt file at the root of this project, or at the      //
// following link: https://opensource.org/licenses/BSD-3-Clause              //
//                                                                           //
//======---------------------------------------------------------------======//

#include "frt/core/bit.h"
#include "frt/collections/array.h"
#include "test_utilities.h"
#include "gtest/gtest.h"
#include <cstdint>
#include <limits>

static_assert(sizeof(std::int64_t) == sizeof(double) && std::numeric_limits<double>::is_iec559);
constexpr std::int64_t iee754_as_int = 4608083138725491507;
constexpr double iee754_as_double = 1.2;

constexpr bool constexpr_bit_cast() {
  return iee754_as_int == frt::bit_cast<std::int64_t>(iee754_as_double);
}

TEST(FrtCoreBit, BitCast) {
  static_assert(constexpr_bit_cast());

  EXPECT_EQ(iee754_as_int, frt::bit_cast<std::int64_t>(iee754_as_double)); // exact bit pattern from IEEE-754
  EXPECT_EQ(5, frt::bit_cast<int>(Wrapper{5}));
  EXPECT_EQ(std::numeric_limits<unsigned>::max(), frt::bit_cast<unsigned>(-1));
}

constexpr bool constexpr_byte_swap() {
  return 2 == frt::byte_swap<int>(frt::byte_swap<int>(2));
}

TEST(FrtCoreBit, ByteSwap) {
  static_assert(constexpr_byte_swap());

  {
    frt::Array<std::uint8_t, 2> start{0x01, 0x02};
    frt::Array<std::uint8_t, 2> end{0x02, 0x01};
    auto as_int = frt::bit_cast<std::uint16_t>(start);
    auto swapped = frt::byte_swap(as_int);

    CHECK_SAME_TYPE(std::uint16_t, swapped);
    EXPECT_EQ(frt::bit_cast<std::uint16_t>(end), swapped);
  }

  {
    frt::Array<std::uint8_t, 4> start{0x01, 0x02, 0x03, 0x04};
    frt::Array<std::uint8_t, 4> end{0x04, 0x03, 0x02, 0x01};
    auto as_int = frt::bit_cast<std::uint32_t>(start);
    auto swapped = frt::byte_swap(as_int);

    CHECK_SAME_TYPE(std::uint32_t, swapped);
    EXPECT_EQ(frt::bit_cast<std::uint32_t>(end), swapped);
  }

  {
    frt::Array<std::uint8_t, 8> start{0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08};
    frt::Array<std::uint8_t, 8> end{0x08, 0x07, 0x06, 0x05, 0x04, 0x03, 0x02, 0x01};
    auto as_int = frt::bit_cast<std::uint64_t>(start);
    auto swapped = frt::byte_swap(as_int);

    CHECK_SAME_TYPE(std::uint64_t, swapped);
    EXPECT_EQ(frt::bit_cast<std::uint64_t>(end), swapped);
  }

  CHECK_SAME_TYPE(long long, frt::byte_swap<long long>(12));
  CHECK_SAME_TYPE(signed char, frt::byte_swap<signed char>(12));
  CHECK_SAME_TYPE(int, frt::byte_swap<int>(12));
  EXPECT_EQ(static_cast<long long>(52830), frt::byte_swap<long long>(frt::byte_swap<long long>(52830)));
}

constexpr bool constexpr_pop_count() {
  return frt::popcount<unsigned>(0) == 0 && frt::popcount<unsigned char>(0b1010'1010) == 4;
}

TEST(FrtCoreBit, Popcount) {
  static_assert(constexpr_pop_count());

  EXPECT_EQ(frt::popcount<unsigned char>(0b1001'1100), 4);
  EXPECT_EQ(frt::popcount<unsigned short>(0b0000'0000), 0);
  EXPECT_EQ(frt::popcount<unsigned>(0b1111'1111), 8);
  EXPECT_EQ(frt::popcount<unsigned long>(0b0000'0001), 1);
  EXPECT_EQ(frt::popcount<unsigned long long>(0b1111'1100), 6);
}

constexpr bool constexpr_has_single_bit() {
  return !frt::has_single_bit<unsigned short>(0b1000'1000) && frt::has_single_bit<unsigned long long>(0b1000'0000);
}

TEST(FrtCoreBit, HasSingleBit) {
  static_assert(constexpr_has_single_bit());

  EXPECT_EQ(frt::has_single_bit<unsigned char>(1), true);
  EXPECT_EQ(frt::has_single_bit<unsigned short>(2), true);
  EXPECT_EQ(frt::has_single_bit<unsigned>(64), true);
  EXPECT_EQ(frt::has_single_bit<unsigned long>(256), true);
  EXPECT_EQ(frt::has_single_bit<unsigned long long>(32768), true);
  EXPECT_EQ(frt::has_single_bit<unsigned char>(0), false);
  EXPECT_EQ(frt::has_single_bit<unsigned short>(3), false);
  EXPECT_EQ(frt::has_single_bit<unsigned>(31), false);
  EXPECT_EQ(frt::has_single_bit<unsigned long>(8095), false);
  EXPECT_EQ(frt::has_single_bit<unsigned long long>(42), false);
}

constexpr bool constexpr_bit_ceil() {
  return frt::bit_ceil<unsigned char>(0b1001) == 0b1'0000;
}

TEST(FrtCoreBit, BitCeil) {
  static_assert(constexpr_bit_ceil());

  EXPECT_EQ(1, frt::bit_ceil<unsigned>(0));
  EXPECT_EQ(0b00000001, frt::bit_ceil<unsigned char>(0b00000001));
  EXPECT_EQ(0b00000001, frt::bit_ceil<unsigned short>(0b00000001));
  EXPECT_EQ(0b00000010, frt::bit_ceil<unsigned>(0b00000010));
  EXPECT_EQ(0b00000100, frt::bit_ceil<unsigned long>(0b00000011));
  EXPECT_EQ(0b00000100, frt::bit_ceil<unsigned long long>(0b00000100));
  EXPECT_EQ(0b00001000, frt::bit_ceil<unsigned char>(0b00000101));
  EXPECT_EQ(0b00001000, frt::bit_ceil<unsigned short>(0b00000110));
  EXPECT_EQ(0b00001000, frt::bit_ceil<unsigned>(0b00000111));
  EXPECT_EQ(0b00001000, frt::bit_ceil<unsigned long>(0b00001000));
  EXPECT_EQ(0b00010000, frt::bit_ceil<unsigned long long>(0b00001001));

  CHECK_SAME_TYPE(unsigned char, frt::bit_ceil<unsigned char>(0b10000));
  CHECK_SAME_TYPE(unsigned short, frt::bit_ceil<unsigned short>(0b10000));
  CHECK_SAME_TYPE(unsigned, frt::bit_ceil<unsigned>(0b10000));
  CHECK_SAME_TYPE(unsigned long, frt::bit_ceil<unsigned long>(0b10000));
  CHECK_SAME_TYPE(unsigned long long, frt::bit_ceil<unsigned long long>(0b10000));
}

constexpr bool constexpr_bit_floor() {
  return frt::bit_floor<unsigned char>(0b1001) == 0b1000;
}

TEST(FrtCoreBit, BitFloor) {
  static_assert(constexpr_bit_floor());

  EXPECT_EQ(0, frt::bit_floor<unsigned char>(0));
  EXPECT_EQ(0b00000001, frt::bit_floor<unsigned short>(0b00000001));
  EXPECT_EQ(0b00000010, frt::bit_floor<unsigned>(0b00000010));
  EXPECT_EQ(0b00000010, frt::bit_floor<unsigned long>(0b00000011));
  EXPECT_EQ(0b00000100, frt::bit_floor<unsigned long long>(0b00000100));
  EXPECT_EQ(0b00000100, frt::bit_floor<unsigned char>(0b00000101));
  EXPECT_EQ(0b00000100, frt::bit_floor<unsigned short>(0b00000110));
  EXPECT_EQ(0b00000100, frt::bit_floor<unsigned>(0b00000111));
  EXPECT_EQ(0b00001000, frt::bit_floor<unsigned long>(0b00001000));
  EXPECT_EQ(0b00001000, frt::bit_floor<unsigned long long>(0b00001001));

  CHECK_SAME_TYPE(unsigned char, frt::bit_floor<unsigned char>(0b10000));
  CHECK_SAME_TYPE(unsigned short, frt::bit_floor<unsigned short>(0b10000));
  CHECK_SAME_TYPE(unsigned, frt::bit_floor<unsigned>(0b10000));
  CHECK_SAME_TYPE(unsigned long, frt::bit_floor<unsigned long>(0b10000));
  CHECK_SAME_TYPE(unsigned long long, frt::bit_floor<unsigned long long>(0b10000));
}

constexpr bool constexpr_bit_width() {
  return frt::bit_width<unsigned>(0b001) == 1;
}

TEST(FrtCoreBit, BitWidth) {
  static_assert(constexpr_bit_width());

  EXPECT_EQ(0, frt::bit_width<unsigned char>(0b0000));
  EXPECT_EQ(1, frt::bit_width<unsigned short>(0b0001));
  EXPECT_EQ(2, frt::bit_width<unsigned>(0b0010));
  EXPECT_EQ(2, frt::bit_width<unsigned long>(0b0011));
  EXPECT_EQ(3, frt::bit_width<unsigned long long>(0b0100));
  EXPECT_EQ(3, frt::bit_width<unsigned char>(0b00101));
  EXPECT_EQ(3, frt::bit_width<unsigned short>(0b00110));
  EXPECT_EQ(3, frt::bit_width<unsigned>(0b00111));

  CHECK_SAME_TYPE(unsigned char, frt::bit_width<unsigned char>(0b10000));
  CHECK_SAME_TYPE(unsigned short, frt::bit_width<unsigned short>(0b10000));
  CHECK_SAME_TYPE(unsigned, frt::bit_width<unsigned>(0b10000));
  CHECK_SAME_TYPE(unsigned long, frt::bit_width<unsigned long>(0b10000));
  CHECK_SAME_TYPE(unsigned long long, frt::bit_width<unsigned long long>(0b10000));
}

constexpr bool constexpr_rotl() {
  return frt::rotl<std::uint8_t>(0b1111'0001, 1) == 0b1110'0011;
}

TEST(FrtCoreBit, Rotl) {
  static_assert(constexpr_rotl());

  auto x = static_cast<std::uint8_t>(0b00011101U);

  EXPECT_EQ(frt::rotl(x, 0), 0b00011101);
  EXPECT_EQ(frt::rotl(x, 1), 0b00111010);
  EXPECT_EQ(frt::rotl(x, 4), 0b11010001);
  EXPECT_EQ(frt::rotl(x, 9), 0b00111010);
  EXPECT_EQ(frt::rotl(x, -1), 0b10001110);

  CHECK_SAME_TYPE(unsigned char, frt::rotl<unsigned char>(0b10000, 1));
  CHECK_SAME_TYPE(unsigned short, frt::rotl<unsigned short>(0b10000, 1));
  CHECK_SAME_TYPE(unsigned, frt::rotl<unsigned>(0b10000, 1));
  CHECK_SAME_TYPE(unsigned long, frt::rotl<unsigned long>(0b10000, 1));
  CHECK_SAME_TYPE(unsigned long long, frt::rotl<unsigned long long>(0b10000, 1));
}

constexpr bool constexpr_rotr() {
  return frt::rotr<std::uint8_t>(0b1111'0001, 1) == 0b1111'1000;
}

TEST(FrtCoreBit, Rotr) {
  static_assert(constexpr_rotr());

  auto x = static_cast<std::uint8_t>(0b00011101);

  EXPECT_EQ(frt::rotr(x, 0), 0b00011101);
  EXPECT_EQ(frt::rotr(x, 1), 0b10001110);
  EXPECT_EQ(frt::rotr(x, 4), 0b11010001);
  EXPECT_EQ(frt::rotr(x, 9), 0b10001110);
  EXPECT_EQ(frt::rotr(x, -1), 0b00111010);

  CHECK_SAME_TYPE(unsigned char, frt::rotr<unsigned char>(0b10000, 1));
  CHECK_SAME_TYPE(unsigned short, frt::rotr<unsigned short>(0b10000, 1));
  CHECK_SAME_TYPE(unsigned, frt::rotr<unsigned>(0b10000, 1));
  CHECK_SAME_TYPE(unsigned long, frt::rotr<unsigned long>(0b10000, 1));
  CHECK_SAME_TYPE(unsigned long long, frt::rotr<unsigned long long>(0b10000, 1));
}

constexpr bool constexpr_countl_zero() {
  return frt::countl_zero<std::uint8_t>(0b0111'1111) == 1;
}

TEST(FrtCoreBit, CountlZero) {
  static_assert(constexpr_countl_zero());

  EXPECT_EQ(frt::countl_zero<std::uint8_t>(0b0000'0000), 8);
  EXPECT_EQ(frt::countl_zero<std::uint8_t>(0b1111'1111), 0);
  EXPECT_EQ(frt::countl_zero<std::uint8_t>(0b1111'0000), 0);
  EXPECT_EQ(frt::countl_zero<std::uint8_t>(0b0001'1110), 3);
  EXPECT_EQ(frt::countl_zero<std::uint16_t>(0b0001'1111'1111'1111), 3);
  EXPECT_EQ(frt::countl_zero<std::uint32_t>(0b1111), 28);
  EXPECT_EQ(frt::countl_zero<std::uint64_t>(0b11110), 59);

  CHECK_SAME_TYPE(int, frt::countl_zero<unsigned char>(0b10000));
  CHECK_SAME_TYPE(int, frt::countl_zero<unsigned short>(0b10000));
  CHECK_SAME_TYPE(int, frt::countl_zero<unsigned>(0b10000));
  CHECK_SAME_TYPE(int, frt::countl_zero<unsigned long>(0b10000));
  CHECK_SAME_TYPE(int, frt::countl_zero<unsigned long long>(0b10000));
}

constexpr bool constexpr_countl_one() {
  return frt::countl_one<std::uint8_t>(0b0111'1111) == 0;
}

TEST(FrtCoreBit, CountlOne) {
  static_assert(constexpr_countl_one());

  EXPECT_EQ(frt::countl_one<std::uint8_t>(0b0000'0000), 0);
  EXPECT_EQ(frt::countl_one<std::uint8_t>(0b1111'1111), 8);
  EXPECT_EQ(frt::countl_one<std::uint8_t>(0b1111'0000), 4);
  EXPECT_EQ(frt::countl_one<std::uint8_t>(0b0001'1110), 0);
  EXPECT_EQ(frt::countl_one<std::uint16_t>(0b0001'1111'1111'1111), 0);
  EXPECT_EQ(frt::countl_one<std::uint32_t>(~0b1111), 28);
  EXPECT_EQ(frt::countl_one<std::uint64_t>(~0b11110), 59);

  CHECK_SAME_TYPE(int, frt::countl_one<unsigned char>(0b10000));
  CHECK_SAME_TYPE(int, frt::countl_one<unsigned short>(0b10000));
  CHECK_SAME_TYPE(int, frt::countl_one<unsigned>(0b10000));
  CHECK_SAME_TYPE(int, frt::countl_one<unsigned long>(0b10000));
  CHECK_SAME_TYPE(int, frt::countl_one<unsigned long long>(0b10000));
}

constexpr bool constexpr_countr_zero() {
  return frt::countr_zero<std::uint8_t>(0b1110'0000) == 5;
}

TEST(FrtCoreBit, CountrZero) {
  static_assert(constexpr_countr_zero());

  EXPECT_EQ(frt::countr_zero<std::uint8_t>(0b0000'0000), 8);
  EXPECT_EQ(frt::countr_zero<std::uint8_t>(0b1111'1000), 3);
  EXPECT_EQ(frt::countr_zero<std::uint8_t>(0b1111'0000), 4);
  EXPECT_EQ(frt::countr_zero<std::uint8_t>(0b0001'1110), 1);
  EXPECT_EQ(frt::countr_zero<std::uint16_t>(~0b0001'1111'1111'1111), 13);
  EXPECT_EQ(frt::countr_zero<std::uint32_t>(static_cast<std::uint32_t>(0b1111) << 16), 16);
  EXPECT_EQ(frt::countr_zero<std::uint64_t>(static_cast<std::uint64_t>(0b1111) << 36), 36);

  CHECK_SAME_TYPE(int, frt::countr_zero<unsigned char>(0b10000));
  CHECK_SAME_TYPE(int, frt::countr_zero<unsigned short>(0b10000));
  CHECK_SAME_TYPE(int, frt::countr_zero<unsigned>(0b10000));
  CHECK_SAME_TYPE(int, frt::countr_zero<unsigned long>(0b10000));
  CHECK_SAME_TYPE(int, frt::countr_zero<unsigned long long>(0b10000));
}

constexpr bool constexpr_countr_one() {
  return frt::countr_one<std::uint8_t>(0b1110'0000) == 0;
}

TEST(FrtCoreBit, CountrOne) {
  static_assert(constexpr_countr_one());

  EXPECT_EQ(frt::countr_one<std::uint8_t>(0b0000'0000), 0);
  EXPECT_EQ(frt::countr_one<std::uint8_t>(0b1111'0111), 3);
  EXPECT_EQ(frt::countr_one<std::uint8_t>(0b0000'1111), 4);
  EXPECT_EQ(frt::countr_one<std::uint8_t>(0b0001'1110), 0);
  EXPECT_EQ(frt::countr_one<std::uint16_t>(0b0001'1111'1111'1111), 13);
  EXPECT_EQ(frt::countr_one<std::uint32_t>(~static_cast<std::uint32_t>(0)), 32);
  EXPECT_EQ(frt::countr_one<std::uint64_t>(~static_cast<std::uint64_t>(0)), 64);

  CHECK_SAME_TYPE(int, frt::countr_one<unsigned char>(0b10000));
  CHECK_SAME_TYPE(int, frt::countr_one<unsigned short>(0b10000));
  CHECK_SAME_TYPE(int, frt::countr_one<unsigned>(0b10000));
  CHECK_SAME_TYPE(int, frt::countr_one<unsigned long>(0b10000));
  CHECK_SAME_TYPE(int, frt::countr_one<unsigned long long>(0b10000));
}