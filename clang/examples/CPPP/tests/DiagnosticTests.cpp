// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "gtest/gtest.h"
#include <cerrno>
#include <cstdint>
#include <initializer_list>
#include <process.h>
#include <string>
#include <vector>

namespace {
std::intptr_t compile(const char *selector,
                      const char *verification = "expected",
                      std::initializer_list<const char *> options = {}) {
  const std::string compiler = CXXX_COMPILER_PATH;
  const std::string quoted_compiler = "\"" + compiler + "\"";
  const std::string quoted_source =
      "\"" + std::string(CXXX_DIAGNOSTIC_SOURCE) + "\"";
  const std::string define = "-D" + std::string(selector);
  const std::string verify = "-verify=" + std::string(verification);
  std::vector<const char *> args{quoted_compiler.c_str(),
                                 "-cc1",
                                 "-x",
                                 "c+++",
                                 "-std=c++20",
                                 "-fsyntax-only",
                                 define.c_str(),
                                 verify.c_str()};
  args.insert(args.end(), options.begin(), options.end());
  args.push_back(quoted_source.c_str());
  args.push_back(nullptr);
  const auto result = _spawnv(_P_WAIT, compiler.c_str(), args.data());
  if (result == -1)
    ADD_FAILURE() << "Could not launch " << compiler << "; errno=" << errno;
  return result;
}

TEST(CompilerDiagnostics, RejectsCapitalizedTypeAlias) {
  EXPECT_EQ(0, compile("CASE_ALIAS"));
}

TEST(CompilerDiagnostics, RejectsIntegerConstruction) {
  EXPECT_EQ(0, compile("CASE_INTEGER_INIT"));
}

TEST(CompilerDiagnostics, RejectsImplicitBoolConversion) {
  EXPECT_EQ(0, compile("CASE_IMPLICIT_BOOL"));
}

TEST(CompilerDiagnostics, RejectsImplicitIntegerConversion) {
  EXPECT_EQ(0, compile("CASE_IMPLICIT_INT"));
}

TEST(CompilerDiagnostics, RejectsTrineArithmetic) {
  EXPECT_EQ(0, compile("CASE_ARITHMETIC"));
}

TEST(CompilerDiagnostics, RejectsTrineLogicalNegation) {
  EXPECT_EQ(0, compile("CASE_LOGICAL"));
}

TEST(CompilerDiagnostics, RejectsIntegerWhenCondition) {
  EXPECT_EQ(0, compile("CASE_WHEN_INT"));
}

TEST(CompilerDiagnostics, RejectsEmptyWhen) {
  EXPECT_EQ(0, compile("CASE_WHEN_EMPTY"));
}

TEST(CompilerDiagnostics, RejectsDuplicateWhenCase) {
  EXPECT_EQ(0, compile("CASE_WHEN_DUPLICATE"));
}

TEST(CompilerDiagnostics, RequiresCommaBetweenWhenCases) {
  EXPECT_EQ(0, compile("CASE_WHEN_SEPARATOR"));
}

TEST(CompilerDiagnostics, RejectsSemicolonCaseSeparator) {
  EXPECT_EQ(0, compile("CASE_WHEN_SEMICOLON"));
}

TEST(CompilerDiagnostics, RequiresBracedWhenBody) {
  EXPECT_EQ(0, compile("CASE_WHEN_UNBRACED"));
}

TEST(CompilerDiagnostics, WhenDoesNotIntroduceABreakTarget) {
  EXPECT_EQ(0, compile("CASE_WHEN_BREAK"));
}

TEST(CompilerDiagnostics, BoolOtherCaseWarnsByDefault) {
  EXPECT_EQ(0, compile("CASE_BOOL_OTHER", "warning"));
}

TEST(CompilerDiagnostics, BoolOtherWarningCanBeDisabled) {
  EXPECT_EQ(0, compile("CASE_BOOL_OTHER", "quiet", {"-Wno-when-bool-other"}));
}

TEST(CompilerDiagnostics, BoolOtherWarningCanBeAnError) {
  EXPECT_EQ(0,
            compile("CASE_BOOL_OTHER", "error", {"-Werror=when-bool-other"}));
}

TEST(CompilerDiagnostics, BoolOtherWarningIsDeferredUntilInstantiation) {
  EXPECT_EQ(0, compile("CASE_TEMPLATE_OTHER"));
}

TEST(CompilerDiagnostics, MissingOtherwiseWarningIsOffByDefault) {
  EXPECT_EQ(0, compile("CASE_MISSING_OTHERWISE", "quiet"));
}

TEST(CompilerDiagnostics, MissingOtherwiseWarningCanBeEnabled) {
  EXPECT_EQ(0, compile("CASE_MISSING_OTHERWISE", "warning",
                       {"-Wtrine-missing-otherwise"}));
}

#define UNARY_DIAGNOSTIC(Name, Selector)                                       \
  TEST(UnaryDiagnostics, Name) { EXPECT_EQ(0, compile(#Selector)); }

UNARY_DIAGNOSTIC(RejectsFalseConstantAssertion, CASE_UNARY_CONSTANT)
UNARY_DIAGNOSTIC(RejectsFalseBijectiveAssertion, CASE_UNARY_BIJECTIVE)
UNARY_DIAGNOSTIC(RejectsFalseSelfInverseAssertion, CASE_UNARY_SELF_INVERSE)
UNARY_DIAGNOSTIC(RejectsFalseIdempotentAssertion, CASE_UNARY_IDEMPOTENT)
UNARY_DIAGNOSTIC(RejectsFalseFixedPointAssertion, CASE_UNARY_FIXED_POINT)
UNARY_DIAGNOSTIC(RejectsFalseNotConstantAssertion, CASE_UNARY_NOT_CONSTANT)
UNARY_DIAGNOSTIC(RejectsFalseNotBijectiveAssertion, CASE_UNARY_NOT_BIJECTIVE)
UNARY_DIAGNOSTIC(RejectsFalseNotSelfInverseAssertion,
                 CASE_UNARY_NOT_SELF_INVERSE)
UNARY_DIAGNOSTIC(RejectsFalseNotIdempotentAssertion, CASE_UNARY_NOT_IDEMPOTENT)
UNARY_DIAGNOSTIC(RejectsFalseNoFixedPointAssertion, CASE_UNARY_NO_FIXED_POINT)
UNARY_DIAGNOSTIC(RejectsConflictingEntry, CASE_UNARY_ENTRY_CONFLICT)
UNARY_DIAGNOSTIC(RejectsUnknownProperty, CASE_UNARY_UNKNOWN_PROPERTY)
UNARY_DIAGNOSTIC(RejectsContradictoryProperties, CASE_UNARY_CONTRADICTION)
UNARY_DIAGNOSTIC(RejectsUnknownFamily, CASE_UNARY_FAMILY)
UNARY_DIAGNOSTIC(RejectsPolicyFromAnotherFamily, CASE_UNARY_POLICY)
UNARY_DIAGNOSTIC(RejectsMissingPolicy, CASE_UNARY_MISSING_POLICY)
UNARY_DIAGNOSTIC(RejectsDuplicateEntry, CASE_UNARY_DUPLICATE_ENTRY)
UNARY_DIAGNOSTIC(RejectsUnknownEntryInput, CASE_UNARY_ENTRY_INPUT)
UNARY_DIAGNOSTIC(RejectsUnknownEntryOutput, CASE_UNARY_ENTRY_OUTPUT)
UNARY_DIAGNOSTIC(RequiresEqualsInEntry, CASE_UNARY_ENTRY_EQUAL)
UNARY_DIAGNOSTIC(RejectsUnknownArity, CASE_UNARY_ARITY)
UNARY_DIAGNOSTIC(ReservesBinaryArity, CASE_UNARY_BINARY)
UNARY_DIAGNOSTIC(RequiresArityColon, CASE_UNARY_COLON)
UNARY_DIAGNOSTIC(RequiresFamilySeparator, CASE_UNARY_SEPARATOR)
UNARY_DIAGNOSTIC(RejectsIntegerOperand, CASE_UNARY_INT)
UNARY_DIAGNOSTIC(RejectsClassConvertibleToBool, CASE_UNARY_CONVERTIBLE_BOOL)
UNARY_DIAGNOSTIC(RejectsClassConvertibleToTrine, CASE_UNARY_CONVERTIBLE_TRINE)
UNARY_DIAGNOSTIC(ChecksInstantiatedOperandType, CASE_UNARY_TEMPLATE_INT)

#undef UNARY_DIAGNOSTIC
} // namespace
