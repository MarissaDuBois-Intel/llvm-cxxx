// -*- mode: c+++ -*-
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "TestSupport.h"

#define UNARY_INPUT(Name, Family, Policy, StateName, Input, Expected)            \
  CXXX_TEST(TrineUnary, Name##_##StateName) {                                   \
    volatile trine value = Input;                                            \
    CXXX_EXPECT_EQ(Expected,                                                 \
                   static_cast<int>(@<u:;Family;Policy> value));              \
  }                                                                          \
  CXXX_TEST(TrineUnaryConstexpr, Name##_##StateName) {                          \
    constexpr trine result = @<u:;Family;Policy> Input;                        \
    CXXX_EXPECT_EQ(Expected, static_cast<int>(result));                        \
  }
#define UNARY_CASE(Name, Family, Policy, T, F, O)                              \
  UNARY_INPUT(Name, Family, Policy, True, true, T)                             \
  UNARY_INPUT(Name, Family, Policy, False, false, F)                           \
  UNARY_INPUT(Name, Family, Policy, Other, other, O)
#include "UnaryCases.def"
#undef UNARY_CASE
#undef UNARY_INPUT

namespace {
trine counted_state(int &calls) {
  ++calls;
  return other;
}

struct LifetimeProbe {
  int &destroyed;
  ~LifetimeProbe() { ++destroyed; }
};

template <class T> constexpr trine swap_boolean_states(T value) {
  return @<u:bijective,inverse;swap;true-false> value;
}

template <trine Value> struct StateArgument {
  static constexpr trine value = Value;
};

trine throwing_state() { throw 7; }
}

CXXX_TEST(TrineUnaryProperties, Constant) {
  CXXX_EXPECT_EQ(1, static_cast<int>(@<u:constant;constant;true> other));
}

CXXX_TEST(TrineUnaryProperties, Bijective) {
  CXXX_EXPECT_EQ(-1, static_cast<int>(@<u:bijective;swap;true-false> other));
}

CXXX_TEST(TrineUnaryProperties, SelfInverseUsesStateIdentity) {
  CXXX_EXPECT_EQ(-1, static_cast<int>(@<u:inverse;identity> other));
}

CXXX_TEST(TrineUnaryProperties, Idempotent) {
  CXXX_EXPECT_EQ(0, static_cast<int>(@<u:idempotent;collapse;true-false> true));
}

CXXX_TEST(TrineUnaryProperties, HasFixedPoint) {
  CXXX_EXPECT_EQ(1, static_cast<int>(@<u:fixed;chain;other-true> false));
}

CXXX_TEST(TrineUnaryProperties, NotConstant) {
  CXXX_EXPECT_EQ(1, static_cast<int>(@<u:!constant;cycle;forward> false));
}

CXXX_TEST(TrineUnaryProperties, NotBijective) {
  CXXX_EXPECT_EQ(-1, static_cast<int>(@<u:!bijective;collapse;false-other> false));
}

CXXX_TEST(TrineUnaryProperties, NotSelfInverse) {
  CXXX_EXPECT_EQ(0, static_cast<int>(@<u:!inverse;chain;true-false> other));
}

CXXX_TEST(TrineUnaryProperties, NotIdempotent) {
  CXXX_EXPECT_EQ(0, static_cast<int>(@<u:!idempotent;cycle;forward> other));
}

CXXX_TEST(TrineUnaryProperties, NoFixedPoint) {
  CXXX_EXPECT_EQ(1, static_cast<int>(
      @<u:!fixed;feed;true-false-false> false));
}

CXXX_TEST(TrineUnaryProperties, MultipleAssertions) {
  CXXX_EXPECT_EQ(-1, static_cast<int>(
      @<u:bijective,inverse,idempotent,fixed;identity> other));
}

CXXX_TEST(TrineUnaryEntries, AddressesIdentityOtherRow) {
  CXXX_EXPECT_EQ(-1, static_cast<int>(@<u:;identity;;other=other> other));
}

CXXX_TEST(TrineUnaryEntries, PreservesUnaddressedRow) {
  CXXX_EXPECT_EQ(1, static_cast<int>(@<u:;identity;;other=other> true));
}

CXXX_TEST(TrineUnaryEntries, DoesNotModifyOperand) {
  trine value = other;
  (void)(@<u:;collapse;other-false;other=false> value);
  CXXX_EXPECT_EQ(-1, static_cast<int>(value));
}

CXXX_TEST(TrineUnaryEntries, AssertionsDescribeSelectedTable) {
  CXXX_EXPECT_EQ(0, static_cast<int>(
      @<u:!bijective,idempotent;collapse;other-false;other=false> other));
}

CXXX_TEST(TrineUnaryEntries, AddressesAllRows) {
  CXXX_EXPECT_EQ(1, static_cast<int>(
      @<u:bijective,!fixed;cycle;forward;true=other,false=true,other=false>
          false));
}

CXXX_TEST(TrineUnaryEntries, AddressesExplicitPolicy) {
  CXXX_EXPECT_EQ(1, static_cast<int>(@<u:;constant;true;other=true> other));
}

CXXX_TEST(TrineUnarySyntax, OmittedTrailingFields) {
  CXXX_EXPECT_EQ(-1, static_cast<int>(@<u:;identity> other));
}

CXXX_TEST(TrineUnarySyntax, ExplicitEmptyTrailingFields) {
  CXXX_EXPECT_EQ(-1, static_cast<int>(@<u:;identity;;> other));
}

CXXX_TEST(TrineUnarySyntax, BindsBeforeComparison) {
  trine value = false;
  CXXX_EXPECT_TRUE(@<u:;swap;true-false> value == true);
}

CXXX_TEST(TrineUnarySyntax, NestsRightToLeft) {
  CXXX_EXPECT_EQ(1, static_cast<int>(
      @<u:;swap;true-false> @<u:;collapse;other-false> other));
}

CXXX_TEST(TrineUnarySyntax, ParenthesesProtectMacroCommas) {
#define UNARY_ID(Expression) Expression
  trine value = UNARY_ID((@<u:bijective,inverse;identity> other));
#undef UNARY_ID
  CXXX_EXPECT_EQ(-1, static_cast<int>(value));
}

CXXX_TEST(TrineUnaryTypes, BoolOperandProducesTrine) {
  CXXX_EXPECT_TRUE(__is_same(decltype(@<u:;identity> true), trine));
}

CXXX_TEST(TrineUnaryTypes, TrineOperandProducesPrvalue) {
  trine value = other;
  CXXX_EXPECT_TRUE(__is_same(decltype((@<u:;identity> value)), trine));
}

CXXX_TEST(TrineUnaryTypes, BitFieldOperand) {
  struct Flags { trine state : 2; };
  Flags flags{other};
  CXXX_EXPECT_EQ(1, static_cast<int>(@<u:;swap;true-other> flags.state));
}

CXXX_TEST(TrineUnaryTemplates, TrineInstantiation) {
  CXXX_EXPECT_EQ(-1, static_cast<int>(swap_boolean_states(other)));
}

CXXX_TEST(TrineUnaryTemplates, BoolInstantiation) {
  CXXX_EXPECT_EQ(0, static_cast<int>(swap_boolean_states(true)));
}

CXXX_TEST(TrineUnaryTemplates, NonTypeArgument) {
  CXXX_EXPECT_EQ(0, static_cast<int>(
      StateArgument<@<u:;cycle;forward> other>::value));
}

CXXX_TEST(TrineUnaryEvaluation, OperandIsEvaluatedOnce) {
  int calls = 0;
  (void)(@<u:;cycle;forward> counted_state(calls));
  CXXX_EXPECT_EQ(1, calls);
}

CXXX_TEST(TrineUnaryEvaluation, ConstantStillEvaluatesOperand) {
  int calls = 0;
  trine result = @<u:;constant;true> counted_state(calls);
  (void)result;
  CXXX_EXPECT_EQ(1, calls);
}

CXXX_TEST(TrineUnaryEvaluation, DiscardedConstantStillEvaluatesOperand) {
  int calls = 0;
  (void)(@<u:;constant;true> counted_state(calls));
  CXXX_EXPECT_EQ(1, calls);
}

CXXX_TEST(TrineUnaryEvaluation, StandaloneExpressionStatement) {
  int calls = 0;
  @<u:;constant;true> counted_state(calls);
  CXXX_EXPECT_EQ(1, calls);
}

CXXX_TEST(TrineUnaryEvaluation, SizeofDoesNotEvaluateOperand) {
  int calls = 0;
  (void)sizeof(@<u:;constant;true> counted_state(calls));
  CXXX_EXPECT_EQ(0, calls);
}

CXXX_TEST(TrineUnaryLifetime, TemporarySurvivesUntilFullExpressionEnds) {
  int destroyed = 0;
  int observed =
      ((void)(@<u:;constant;true> ((void)LifetimeProbe{destroyed}, other)),
       destroyed);
  CXXX_EXPECT_EQ(0, observed);
}

CXXX_TEST(TrineUnaryLifetime, TemporaryIsDestroyedAfterFullExpression) {
  int destroyed = 0;
  (void)(@<u:;constant;true> ((void)LifetimeProbe{destroyed}, other));
  CXXX_EXPECT_EQ(1, destroyed);
}

CXXX_TEST(TrineUnaryExceptions, PureOperandIsNoexcept) {
  CXXX_EXPECT_TRUE(noexcept(@<u:;identity> other));
}

CXXX_TEST(TrineUnaryExceptions, ThrowingOperandIsNotNoexcept) {
  CXXX_EXPECT_TRUE(!noexcept(@<u:;constant;true> throwing_state()));
}

CXXX_TEST(TrineUnaryExceptions, ConstantPropagatesOperandException) {
  int caught = 0;
  try {
    (void)(@<u:;constant;true> throwing_state());
  } catch (int value) {
    caught = value;
  }
  CXXX_EXPECT_EQ(7, caught);
}
