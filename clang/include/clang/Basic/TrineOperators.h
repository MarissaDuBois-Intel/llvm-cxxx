//===--- TrineOperators.h - C+++ unary truth tables ---------------*- C++
//-*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_CLANG_BASIC_TRINEOPERATORS_H
#define LLVM_CLANG_BASIC_TRINEOPERATORS_H

#include <array>
#include <cassert>
#include <cstdint>

namespace clang {

// Table indices follow the workbook's T,F,O order, not the storage encoding.
enum class TrineState : uint8_t { True, False, Other };

constexpr uint8_t makeTrineUnaryTable(TrineState T, TrineState F,
                                      TrineState O) {
  return unsigned(T) | (unsigned(F) << 2) | (unsigned(O) << 4);
}

constexpr TrineState trineUnaryOutput(uint8_t Table, TrineState Input) {
  return TrineState((Table >> (2 * unsigned(Input))) & 3);
}

constexpr int trineStateValue(TrineState State) {
  return State == TrineState::True ? 1 : State == TrineState::False ? 0 : -1;
}

inline TrineState trineStateFromValue(int64_t Value) {
  assert((Value == 1 || Value == 0 || Value == -1) && "invalid trine value");
  return Value == 1   ? TrineState::True
         : Value == 0 ? TrineState::False
                      : TrineState::Other;
}

struct TrineUnaryDefinition {
  const char *Family;
  const char *Policy;
  uint8_t Table;
};

#define TRINE_MAP(Family, Policy, T, F, O)                                     \
  {Family, Policy,                                                             \
   makeTrineUnaryTable(TrineState::T, TrineState::F, TrineState::O)}
inline constexpr TrineUnaryDefinition TrineUnaryDefinitions[] = {
    TRINE_MAP("identity", "", True, False, Other),
    TRINE_MAP("constant", "true", True, True, True),
    TRINE_MAP("constant", "false", False, False, False),
    TRINE_MAP("constant", "other", Other, Other, Other),
    TRINE_MAP("swap", "true-false", False, True, Other),
    TRINE_MAP("swap", "true-other", Other, False, True),
    TRINE_MAP("swap", "false-other", True, Other, False),
    TRINE_MAP("cycle", "forward", Other, True, False),
    TRINE_MAP("cycle", "backward", False, Other, True),
    TRINE_MAP("collapse", "true-false", False, False, Other),
    TRINE_MAP("collapse", "true-other", Other, False, Other),
    TRINE_MAP("collapse", "false-true", True, True, Other),
    TRINE_MAP("collapse", "false-other", True, Other, Other),
    TRINE_MAP("collapse", "other-true", True, False, True),
    TRINE_MAP("collapse", "other-false", True, False, False),
    TRINE_MAP("chain", "true-false", True, True, False),
    TRINE_MAP("chain", "true-other", True, Other, True),
    TRINE_MAP("chain", "false-true", False, False, True),
    TRINE_MAP("chain", "false-other", Other, False, False),
    TRINE_MAP("chain", "other-true", Other, True, Other),
    TRINE_MAP("chain", "other-false", False, Other, Other),
    TRINE_MAP("feed", "true-false-true", False, True, True),
    TRINE_MAP("feed", "true-false-false", False, True, False),
    TRINE_MAP("feed", "true-other-true", Other, True, True),
    TRINE_MAP("feed", "true-other-other", Other, Other, True),
    TRINE_MAP("feed", "false-other-false", False, Other, False),
    TRINE_MAP("feed", "false-other-other", Other, Other, False),
};
#undef TRINE_MAP

enum TrineUnaryProperty : uint8_t {
  TUP_Constant = 1,
  TUP_Bijective = 2,
  TUP_SelfInverse = 4,
  TUP_Idempotent = 8,
  TUP_HasFixedPoint = 16
};

inline constexpr std::array<const char *, 5> TrineUnaryPropertyNames = {
    "constant", "bijective", "inverse", "idempotent", "fixed"};
inline constexpr std::array<const char *, 3> TrineStateNames = {"true", "false",
                                                                "other"};

constexpr uint8_t trineUnaryProperties(uint8_t Table) {
  const auto T = trineUnaryOutput(Table, TrineState::True);
  const auto F = trineUnaryOutput(Table, TrineState::False);
  const auto O = trineUnaryOutput(Table, TrineState::Other);
  uint8_t Properties = 0;
  if (T == F && F == O)
    Properties |= TUP_Constant;
  if (T != F && T != O && F != O)
    Properties |= TUP_Bijective;
  bool SelfInverse = true, Idempotent = true, FixedPoint = false;
  for (unsigned I = 0; I != 3; ++I) {
    auto Input = TrineState(I);
    auto Output = trineUnaryOutput(Table, Input);
    SelfInverse &= trineUnaryOutput(Table, Output) == Input;
    Idempotent &= trineUnaryOutput(Table, Output) == Output;
    FixedPoint |= Output == Input;
  }
  if (SelfInverse)
    Properties |= TUP_SelfInverse;
  if (Idempotent)
    Properties |= TUP_Idempotent;
  if (FixedPoint)
    Properties |= TUP_HasFixedPoint;
  return Properties;
}

struct TrineUnaryDescriptor {
  uint8_t Base = 0;
  uint8_t Required = 0;
  uint8_t Forbidden = 0;
  uint8_t Entries = 0;

  constexpr uint8_t getTable() const {
    return TrineUnaryDefinitions[Base].Table;
  }
};

} // namespace clang

#endif
