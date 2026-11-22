// -*- mode: c+++ -*-
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "TestSupport.h"
#include <compare>

namespace {
enum class Order { Less, Equal, Greater, Unordered };

constexpr Order mixed_other =
    CXXX_TEST_OTHER_ORDERED ? Order::Less : Order::Unordered;
constexpr Order reversed_other =
    CXXX_TEST_OTHER_ORDERED ? Order::Greater : Order::Unordered;
constexpr Order self_other =
    CXXX_TEST_OTHER_EQUAL ? Order::Equal : Order::Unordered;

constexpr std::partial_ordering category(Order value) {
  switch (value) {
  case Order::Less: return std::partial_ordering::less;
  case Order::Equal: return std::partial_ordering::equivalent;
  case Order::Greater: return std::partial_ordering::greater;
  case Order::Unordered: return std::partial_ordering::unordered;
  }
}

// Each operator/operand pair is a separate Google Test, not a matrix assertion.
#define COMPARISON_TESTS(Name, Left, Right, Expected)                           \
  CXXX_TEST(TrineEqual, Name) {                                                \
    volatile auto lhs = (Left);                                               \
    volatile auto rhs = (Right);                                              \
    CXXX_EXPECT_EQ((Expected) == Order::Equal, lhs == rhs);                     \
  }                                                                          \
  CXXX_TEST(TrineNotEqual, Name) {                                             \
    volatile auto lhs = (Left);                                               \
    volatile auto rhs = (Right);                                              \
    CXXX_EXPECT_EQ((Expected) != Order::Equal, lhs != rhs);                     \
  }                                                                          \
  CXXX_TEST(TrineLess, Name) {                                                 \
    volatile auto lhs = (Left);                                               \
    volatile auto rhs = (Right);                                              \
    CXXX_EXPECT_EQ((Expected) == Order::Less, lhs < rhs);                       \
  }                                                                          \
  CXXX_TEST(TrineLessEqual, Name) {                                            \
    volatile auto lhs = (Left);                                               \
    volatile auto rhs = (Right);                                              \
    CXXX_EXPECT_EQ((Expected) == Order::Less || (Expected) == Order::Equal,      \
                   lhs <= rhs);                                              \
  }                                                                          \
  CXXX_TEST(TrineGreater, Name) {                                              \
    volatile auto lhs = (Left);                                               \
    volatile auto rhs = (Right);                                              \
    CXXX_EXPECT_EQ((Expected) == Order::Greater, lhs > rhs);                    \
  }                                                                          \
  CXXX_TEST(TrineGreaterEqual, Name) {                                         \
    volatile auto lhs = (Left);                                               \
    volatile auto rhs = (Right);                                              \
    CXXX_EXPECT_EQ((Expected) == Order::Greater || (Expected) == Order::Equal,   \
                   lhs >= rhs);                                              \
  }                                                                          \
  CXXX_TEST(TrineSpaceship, Name) {                                            \
    volatile auto lhs = (Left);                                               \
    volatile auto rhs = (Right);                                              \
    CXXX_EXPECT_TRUE((lhs <=> rhs) == category(Expected));                     \
  }

COMPARISON_TESTS(FalseFalse, trine(false), trine(false), Order::Equal)
COMPARISON_TESTS(FalseTrue, trine(false), trine(true), Order::Less)
COMPARISON_TESTS(FalseOther, trine(false), other, reversed_other)
COMPARISON_TESTS(TrueFalse, trine(true), trine(false), Order::Greater)
COMPARISON_TESTS(TrueTrue, trine(true), trine(true), Order::Equal)
COMPARISON_TESTS(TrueOther, trine(true), other, reversed_other)
COMPARISON_TESTS(OtherFalse, other, trine(false), mixed_other)
COMPARISON_TESTS(OtherTrue, other, trine(true), mixed_other)
COMPARISON_TESTS(OtherOther, other, other, self_other)
COMPARISON_TESTS(FalseBoolFalse, trine(false), false, Order::Equal)
COMPARISON_TESTS(TrueBoolTrue, trine(true), true, Order::Equal)
COMPARISON_TESTS(OtherBoolFalse, other, false, mixed_other)
COMPARISON_TESTS(OtherBoolTrue, other, true, mixed_other)
COMPARISON_TESTS(BoolFalseOther, false, other, reversed_other)
COMPARISON_TESTS(BoolTrueOther, true, other, reversed_other)
COMPARISON_TESTS(FalseIntegerZero, trine(false), 0, Order::Equal)
COMPARISON_TESTS(TrueIntegerOne, trine(true), 1, Order::Equal)
COMPARISON_TESTS(TrueIntegerTwo, trine(true), 2, Order::Less)
COMPARISON_TESTS(IntegerTwoTrue, 2, trine(true), Order::Greater)
COMPARISON_TESTS(OtherIntegerMinusOne, other, -1, mixed_other)
COMPARISON_TESTS(IntegerMinusOneOther, -1, other, reversed_other)
COMPARISON_TESTS(OtherMinimumInteger, other, (-__INT_MAX__ - 1), mixed_other)
COMPARISON_TESTS(TrueMaximumUnsigned, trine(true), ~0u, Order::Less)
COMPARISON_TESTS(OtherMaximumUnsigned, other, ~0u, mixed_other)
COMPARISON_TESTS(MaximumUnsignedOther, ~0u, other, reversed_other)

#undef COMPARISON_TESTS

trine counted(int &count) {
  ++count;
  return other;
}

#define EVALUATION_TEST(Name, Operator)                                        \
  CXXX_TEST(TrineEvaluation, Name##_LeftOperand) {                             \
    int left = 0, right = 0;                                                  \
    (void)(counted(left) Operator counted(right));                             \
    CXXX_EXPECT_EQ(1, left);                                                  \
  }                                                                          \
  CXXX_TEST(TrineEvaluation, Name##_RightOperand) {                            \
    int left = 0, right = 0;                                                  \
    (void)(counted(left) Operator counted(right));                             \
    CXXX_EXPECT_EQ(1, right);                                                 \
  }

EVALUATION_TEST(EqualityEvaluatesEachOperandOnce, ==)
EVALUATION_TEST(InequalityEvaluatesEachOperandOnce, !=)
EVALUATION_TEST(LessEvaluatesEachOperandOnce, <)
EVALUATION_TEST(LessEqualEvaluatesEachOperandOnce, <=)
EVALUATION_TEST(GreaterEvaluatesEachOperandOnce, >)
EVALUATION_TEST(GreaterEqualEvaluatesEachOperandOnce, >=)
EVALUATION_TEST(SpaceshipEvaluatesEachOperandOnce, <=>)

#undef EVALUATION_TEST
}
