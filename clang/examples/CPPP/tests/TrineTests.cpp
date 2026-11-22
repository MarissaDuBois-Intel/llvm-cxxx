// -*- mode: c+++ -*-
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "TestSupport.h"

namespace {

CXXX_TEST(TrineType, IsFundamental) {
  CXXX_EXPECT_TRUE(__is_fundamental(trine));
}

CXXX_TEST(TrineType, IsIntegral) {
  CXXX_EXPECT_TRUE(__is_integral(trine));
}

CXXX_TEST(TrineType, IsSigned) {
  CXXX_EXPECT_TRUE(__is_signed(trine));
}

CXXX_TEST(TrineType, IsDistinctFromBool) {
  CXXX_EXPECT_TRUE(!__is_same(trine, bool));
}

CXXX_TEST(TrineType, IsDistinctFromBitInt) {
  CXXX_EXPECT_TRUE(!__is_same(trine, signed _BitInt(2)));
}

CXXX_TEST(TrineType, OtherLiteralHasTrineType) {
  CXXX_EXPECT_TRUE(__is_same(decltype(other), trine));
}

CXXX_TEST(TrineType, TrueLiteralStillHasBoolType) {
  CXXX_EXPECT_TRUE(__is_same(decltype(true), bool));
}

CXXX_TEST(TrineType, CapitalizedSpellingIsAnIdentifier) {
  int Trine = 7;
  CXXX_EXPECT_EQ(7, Trine);
}

CXXX_TEST(TrineValues, ValueInitializationProducesFalse) {
  volatile trine value{};
  CXXX_EXPECT_EQ(0, static_cast<int>(value));
}

CXXX_TEST(TrineValues, ConstructsFromFalse) {
  volatile bool input = false;
  trine value = input;
  CXXX_EXPECT_EQ(0, static_cast<int>(value));
}

CXXX_TEST(TrineValues, ConstructsFromTrue) {
  volatile bool input = true;
  trine value = input;
  CXXX_EXPECT_EQ(1, static_cast<int>(value));
}

CXXX_TEST(TrineValues, ConstructsFromOther) {
  volatile trine value = other;
  CXXX_EXPECT_EQ(-1, static_cast<int>(value));
}

CXXX_TEST(TrineValues, AssignsFalse) {
  volatile trine value = other;
  value = false;
  CXXX_EXPECT_EQ(0, static_cast<int>(value));
}

CXXX_TEST(TrineValues, AssignsTrue) {
  volatile trine value = other;
  value = true;
  CXXX_EXPECT_EQ(1, static_cast<int>(value));
}

CXXX_TEST(TrineValues, AssignsOther) {
  volatile trine value = true;
  value = other;
  CXXX_EXPECT_EQ(-1, static_cast<int>(value));
}

CXXX_TEST(TrineValues, CopiesOther) {
  volatile trine original = other;
  trine copy = original;
  CXXX_EXPECT_EQ(-1, static_cast<int>(copy));
}

[[clang::noinline]] trine return_state(trine value) { return value; }

CXXX_TEST(TrineValues, ReturnsFalseAcrossACall) {
  CXXX_EXPECT_EQ(0, static_cast<int>(return_state(false)));
}

CXXX_TEST(TrineValues, ReturnsTrueAcrossACall) {
  CXXX_EXPECT_EQ(1, static_cast<int>(return_state(true)));
}

CXXX_TEST(TrineValues, ReturnsOtherAcrossACall) {
  CXXX_EXPECT_EQ(-1, static_cast<int>(return_state(other)));
}

CXXX_TEST(TrineStorage, FalseUsesAZeroByte) {
  volatile trine value = false;
  CXXX_EXPECT_EQ(0, *reinterpret_cast<volatile unsigned char *>(&value));
}

CXXX_TEST(TrineStorage, TrueUsesAOneByte) {
  volatile trine value = true;
  CXXX_EXPECT_EQ(1, *reinterpret_cast<volatile unsigned char *>(&value));
}

CXXX_TEST(TrineStorage, OtherUsesASignExtendedByte) {
  volatile trine value = other;
  CXXX_EXPECT_EQ(255, *reinterpret_cast<volatile unsigned char *>(&value));
}

struct Bits {
  trine first : 2;
  trine second : 2;
};

CXXX_TEST(TrineStorage, OtherSurvivesABitField) {
  volatile Bits bits{other, false};
  CXXX_EXPECT_EQ(-1, static_cast<int>(bits.first));
}

CXXX_TEST(TrineStorage, UpdatingAdjacentBitFieldPreservesOther) {
  volatile Bits bits{other, false};
  bits.second = true;
  CXXX_EXPECT_EQ(-1, static_cast<int>(bits.first));
}

constexpr trine copy_state(trine value) { return value; }

CXXX_TEST(TrineConstexpr, PreservesOther) {
  constexpr trine value = copy_state(other);
  CXXX_EXPECT_EQ(-1, static_cast<int>(value));
}

template <trine Value> constexpr int encoding() {
  return static_cast<int>(Value);
}

CXXX_TEST(TrineTemplates, AcceptsOtherAsANonTypeArgument) {
  CXXX_EXPECT_EQ(-1, encoding<other>());
}

int overload(bool) { return 1; }
int overload(trine) { return 2; }

CXXX_TEST(TrineOverloads, OtherSelectsTrineOverload) {
  CXXX_EXPECT_EQ(2, overload(other));
}

CXXX_TEST(TrineOverloads, TrueSelectsBoolOverload) {
  CXXX_EXPECT_EQ(1, overload(true));
}

}
