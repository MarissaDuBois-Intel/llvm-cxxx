// -*- mode: c+++ -*-
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "TestSupport.h"

namespace {
template <class T> int select_branch(T input) {
  volatile T value = input;
  if (value) { return 1; }
  else { return 0; }
  otherwise { return -1; }
}

CXXX_TEST(IfDispatch, TrineTrueSelectsThen) {
  CXXX_EXPECT_EQ(1, select_branch(trine(true)));
}

CXXX_TEST(IfDispatch, TrineFalseSelectsElse) {
  CXXX_EXPECT_EQ(0, select_branch(trine(false)));
}

CXXX_TEST(IfDispatch, TrineOtherSelectsOtherwise) {
  CXXX_EXPECT_EQ(-1, select_branch(other));
}

CXXX_TEST(IfDispatch, BoolTrueSelectsThen) {
  CXXX_EXPECT_EQ(1, select_branch(true));
}

CXXX_TEST(IfDispatch, BoolFalseSelectsElse) {
  CXXX_EXPECT_EQ(0, select_branch(false));
}

CXXX_TEST(IfDispatch, IntegerMinusOneRemainsTrue) {
  CXXX_EXPECT_EQ(1, select_branch(-1));
}

trine counted(trine input, int &calls) {
  ++calls;
  volatile trine value = input;
  return value;
}

int select_chain(trine first, trine second, trine third, int &calls) {
  if (counted(first, calls)) { return 1; }
  else if (counted(second, calls)) { return 2; }
  else if (counted(third, calls)) { return 3; }
  else { return 0; }
  otherwise { return -1; }
}

#define CHAIN_RESULT(Name, First, Second, Third, Expected)                     \
  CXXX_TEST(IfChain, Name) {                                                  \
    int calls = 0;                                                           \
    CXXX_EXPECT_EQ(Expected, select_chain(First, Second, Third, calls));        \
  }

CHAIN_RESULT(FirstTrueSelectsFirstBody, true, false, false, 1)
CHAIN_RESULT(SecondTrueSelectsSecondBody, false, true, false, 2)
CHAIN_RESULT(ThirdTrueSelectsThirdBody, false, false, true, 3)
CHAIN_RESULT(AllFalseSelectsElse, false, false, false, 0)
CHAIN_RESULT(FirstOtherSelectsSharedOtherwise, other, true, true, -1)
CHAIN_RESULT(SecondOtherSelectsSharedOtherwise, false, other, true, -1)
CHAIN_RESULT(ThirdOtherSelectsSharedOtherwise, false, false, other, -1)

#undef CHAIN_RESULT

CXXX_TEST(IfEvaluation, TrueSkipsRemainingConditions) {
  int calls = 0;
  select_chain(true, true, true, calls);
  CXXX_EXPECT_EQ(1, calls);
}

CXXX_TEST(IfEvaluation, OtherSkipsRemainingConditions) {
  int calls = 0;
  select_chain(other, true, true, calls);
  CXXX_EXPECT_EQ(1, calls);
}

CXXX_TEST(IfEvaluation, LaterOtherSkipsFollowingCondition) {
  int calls = 0;
  select_chain(false, other, true, calls);
  CXXX_EXPECT_EQ(2, calls);
}

CXXX_TEST(IfEvaluation, AllFalseEvaluatesEachConditionOnce) {
  int calls = 0;
  select_chain(false, false, false, calls);
  CXXX_EXPECT_EQ(3, calls);
}

CXXX_TEST(IfOmitted, OtherDoesNotExecuteElse) {
  volatile trine value = other;
  int selected = 7;
  if (value) { selected = 1; }
  else { selected = 0; }
  CXXX_EXPECT_EQ(7, selected);
}

CXXX_TEST(IfOmitted, OtherStillStopsAnElseIfChain) {
  volatile trine value = other;
  int calls = 0;
  if (value) {}
  else if (counted(true, calls)) {}
  CXXX_EXPECT_EQ(0, calls);
}

CXXX_TEST(IfNesting, InnerOtherDoesNotPropagateToOuterOtherwise) {
  volatile trine value = other;
  int selected = 7;
  if (true) {
    if (value) { selected = 1; }
  }
  otherwise { selected = -1; }
  CXXX_EXPECT_EQ(7, selected);
}

struct LifetimeProbe {
  int &live;
  explicit LifetimeProbe(int &count) : live(count) { ++live; }
  ~LifetimeProbe() { --live; }
};

CXXX_TEST(IfScope, RootInitializerLivesThroughOtherwise) {
  int live = 0, observed = 0;
  volatile trine value = other;
  if (LifetimeProbe root(live); value) {}
  otherwise { observed = root.live; }
  CXXX_EXPECT_EQ(1, observed);
}

CXXX_TEST(IfScope, LaterInitializerIsDestroyedBeforeOtherwise) {
  int live = 0, observed = 0;
  volatile trine value = other;
  if (LifetimeProbe root(live); false) {}
  else if (LifetimeProbe later(live); value) {}
  otherwise { observed = root.live; }
  CXXX_EXPECT_EQ(1, observed);
}

CXXX_TEST(IfScope, ConditionDeclarationIsVisibleInOtherwise) {
  int selected = 7;
  if (trine value = other) {}
  otherwise { selected = static_cast<int>(value); }
  CXXX_EXPECT_EQ(-1, selected);
}

CXXX_TEST(IfControlFlow, OtherwiseBreakTargetsEnclosingLoop) {
  int iterations = 0;
  for (int i = 0; i < 3; ++i) {
    ++iterations;
    if (other) {}
    otherwise { break; }
  }
  CXXX_EXPECT_EQ(1, iterations);
}

CXXX_TEST(IfControlFlow, OtherwiseContinueSkipsRemainingLoopBody) {
  int tail_calls = 0;
  for (int i = 0; i < 3; ++i) {
    if (other) {}
    otherwise { continue; }
    ++tail_calls;
  }
  CXXX_EXPECT_EQ(0, tail_calls);
}

CXXX_TEST(IfComparison, ExplicitOtherEqualityUsesConfiguredEquality) {
  volatile trine value = other;
  int selected = 0;
  if (value == other) { selected = 1; }
  CXXX_EXPECT_EQ(CXXX_TEST_OTHER_EQUAL, selected);
}
}
