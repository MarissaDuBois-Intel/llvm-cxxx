// -*- mode: c+++ -*-
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "TestSupport.h"

namespace {
template <class T> constexpr int select_state(T value) {
  when (value) {
    other: { return -1; },
    false: { return 0; },
    true: { return 1; }
  }
}

template <class T> constexpr int select_boolean(T value) {
  when (value) {
    false: { return 0; },
    true: { return 1; }
  }
}

CXXX_TEST(WhenDispatch, SelectsFalse) {
  volatile trine value = false;
  CXXX_EXPECT_EQ(0, select_state(value));
}

CXXX_TEST(WhenDispatch, SelectsTrue) {
  volatile trine value = true;
  CXXX_EXPECT_EQ(1, select_state(value));
}

CXXX_TEST(WhenDispatch, SelectsOtherIndependentlyOfComparisonFlags) {
  volatile trine value = other;
  CXXX_EXPECT_EQ(-1, select_state(value));
}

CXXX_TEST(WhenBoolean, SelectsFalse) {
  volatile bool value = false;
  CXXX_EXPECT_EQ(0, select_boolean(value));
}

CXXX_TEST(WhenBoolean, SelectsTrue) {
  volatile bool value = true;
  CXXX_EXPECT_EQ(1, select_boolean(value));
}

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wwhen-bool-other"

CXXX_TEST(WhenBoolean, FalseDoesNotSelectOther) {
  volatile bool value = false;
  int selected = 0;
  when (value) { other: { selected = 1; } }
  CXXX_EXPECT_EQ(0, selected);
}

CXXX_TEST(WhenBoolean, TrueDoesNotSelectOther) {
  volatile bool value = true;
  int selected = 0;
  when (value) { other: { selected = 1; } }
  CXXX_EXPECT_EQ(0, selected);
}

#pragma clang diagnostic pop

#define ORDER_CASE(Name, First, Second, Third, State, Suffix)                   \
  CXXX_TEST(WhenCaseOrder, Name##_##Suffix) {                                  \
    volatile trine value = State;                                            \
    int selected = 7;                                                        \
    when (value) {                                                           \
      First: { selected = static_cast<int>(First); },                         \
      Second: { selected = static_cast<int>(Second); },                       \
      Third: { selected = static_cast<int>(Third); }                          \
    }                                                                        \
    CXXX_EXPECT_EQ(static_cast<int>(State), selected);                        \
  }

#define ORDER_TESTS(Name, First, Second, Third)                                \
  ORDER_CASE(Name, First, Second, Third, false, False)                         \
  ORDER_CASE(Name, First, Second, Third, true, True)                           \
  ORDER_CASE(Name, First, Second, Third, other, Other)

ORDER_TESTS(TrueFalseOther, true, false, other)
ORDER_TESTS(TrueOtherFalse, true, other, false)
ORDER_TESTS(FalseTrueOther, false, true, other)
ORDER_TESTS(FalseOtherTrue, false, other, true)
ORDER_TESTS(OtherTrueFalse, other, true, false)
ORDER_TESTS(OtherFalseTrue, other, false, true)

#undef ORDER_TESTS
#undef ORDER_CASE

CXXX_TEST(WhenCases, SingleTrueCaseNeedsNoTrailingComma) {
  int selected = 0;
  when (trine(true)) { true: { selected = 1; } }
  CXXX_EXPECT_EQ(1, selected);
}

CXXX_TEST(WhenCases, SingleCaseAllowsTrailingComma) {
  int selected = 0;
  when (other) { other: { selected = 1; }, }
  CXXX_EXPECT_EQ(1, selected);
}

CXXX_TEST(WhenCases, MissingFalseCaseDoesNothing) {
  int selected = 0;
  when (trine(false)) { true: { selected = 1; } }
  CXXX_EXPECT_EQ(0, selected);
}

CXXX_TEST(WhenCases, MissingOtherCaseDoesNothing) {
  int selected = 0;
  when (other) { true: { selected = 1; }, false: { selected = 2; } }
  CXXX_EXPECT_EQ(0, selected);
}

CXXX_TEST(WhenCases, DoesNotFallThrough) {
  int actions = 0;
  when (trine(true)) {
    true: { ++actions; },
    false: { ++actions; },
    other: { ++actions; }
  }
  CXXX_EXPECT_EQ(1, actions);
}

CXXX_TEST(WhenEvaluation, TrineConditionIsEvaluatedOnce) {
  int calls = 0;
  when ((++calls, other)) { other: {} }
  CXXX_EXPECT_EQ(1, calls);
}

CXXX_TEST(WhenEvaluation, BoolConditionIsEvaluatedOnce) {
  int calls = 0;
  when ((++calls, true)) { true: {} }
  CXXX_EXPECT_EQ(1, calls);
}

CXXX_TEST(WhenEvaluation, UnmatchedConditionIsStillEvaluatedOnce) {
  int calls = 0;
  when ((++calls, other)) { true: {} }
  CXXX_EXPECT_EQ(1, calls);
}

struct Temporary {
  int &live;
  explicit Temporary(int &count) : live(count) { ++live; }
  ~Temporary() { --live; }
  trine state() const { return other; }
};

CXXX_TEST(WhenLifetime, ConditionTemporaryIsDestroyedBeforeCaseBody) {
  int live = 0, observed = -1;
  when (Temporary(live).state()) { other: { observed = live; } }
  CXXX_EXPECT_EQ(0, observed);
}

CXXX_TEST(WhenControlFlow, BreakTargetsEnclosingLoop) {
  int iterations = 0;
  for (int i = 0; i < 3; ++i) {
    ++iterations;
    when (other) { other: { break; } }
  }
  CXXX_EXPECT_EQ(1, iterations);
}

CXXX_TEST(WhenControlFlow, ContinueSkipsRemainingLoopBody) {
  int tail_calls = 0;
  for (int i = 0; i < 3; ++i) {
    when (other) { other: { continue; } }
    ++tail_calls;
  }
  CXXX_EXPECT_EQ(0, tail_calls);
}

CXXX_TEST(WhenControlFlow, BreakTargetsEnclosingSwitch) {
  int selected = 0;
  switch (1) {
  case 1:
    when (other) { other: { break; } }
    selected = 1;
  }
  CXXX_EXPECT_EQ(0, selected);
}

CXXX_TEST(WhenConstexpr, TrineOtherWorksInConstantEvaluation) {
  constexpr int selected = select_state(other);
  CXXX_EXPECT_EQ(-1, selected);
}

CXXX_TEST(WhenConstexpr, BoolTrueWorksInConstantEvaluation) {
  constexpr int selected = select_boolean(true);
  CXXX_EXPECT_EQ(1, selected);
}

CXXX_TEST(WhenChaining, ElseWhenRunsAfterFalse) {
  int selected = 0;
  if (false) {}
  else when (other) { other: { selected = 1; } }
  CXXX_EXPECT_EQ(1, selected);
}

CXXX_TEST(WhenChaining, ElseWhenIsNotEvaluatedAfterTrue) {
  int calls = 0;
  if (true) {}
  else when ((++calls, other)) { other: {} }
  CXXX_EXPECT_EQ(0, calls);
}

CXXX_TEST(WhenChaining, ElseWhenIsNotEvaluatedAfterOther) {
  int calls = 0;
  if (other) {}
  else when ((++calls, other)) { other: {} }
  otherwise {}
  CXXX_EXPECT_EQ(0, calls);
}

CXXX_TEST(WhenChaining, OtherwiseWhenHandlesOther) {
  int selected = 0;
  if (other) {}
  otherwise when (other) { other: { selected = 1; } }
  CXXX_EXPECT_EQ(1, selected);
}

CXXX_TEST(WhenChaining, OtherwiseIfStartsAnInnerChain) {
  int selected = 0;
  if (other) {}
  otherwise if (false) { selected = 1; }
  else when (other) { other: { selected = 2; } }
  CXXX_EXPECT_EQ(2, selected);
}

CXXX_TEST(WhenChaining, InnerOtherwiseBelongsToTheInnerIf) {
  int selected = 0;
  if (other) {}
  otherwise if (other) {}
  otherwise when (other) { other: { selected = 1; } }
  CXXX_EXPECT_EQ(1, selected);
}

CXXX_TEST(WhenChaining, MissingCaseDoesNotPropagateToOuterOtherwise) {
  int selected = 0;
  if (false) {}
  else when (other) { true: { selected = 1; } }
  otherwise { selected = 2; }
  CXXX_EXPECT_EQ(0, selected);
}
}
