// RUN: %clang_cc1 -x c+++ -std=c++20 -fsyntax-only -verify %s
// RUN: %clang_cc1 -x c+++ -std=c++20 -fexperimental-new-constant-interpreter -fsyntax-only -verify %s
// RUN: %clang_cc1 -x c+++ -std=c++20 -fno-trine-other-equal -fsyntax-only -verify %s
// RUN: %clang_cc1 -x c+++ -std=c++20 -fno-trine-other-equal -fexperimental-new-constant-interpreter -fsyntax-only -verify %s
// RUN: %clang_cc1 -x c+++ -std=c++20 -ftrine-other-ordered -fsyntax-only -verify %s
// RUN: %clang_cc1 -x c+++ -std=c++20 -ftrine-other-ordered -fexperimental-new-constant-interpreter -fsyntax-only -verify %s
// RUN: %clang_cc1 -x c+++ -std=c++20 -ast-print %s | FileCheck %s --check-prefix=PRINT
// RUN: %clang_cc1 -x c+++ -std=c++20 -ast-dump %s | FileCheck %s --check-prefix=AST

#define UNARY_CASE(Name, Family, Policy, T, F, O)                               \
  static_assert(static_cast<int>(@<u:;Family;Policy> trine(true)) == T);        \
  static_assert(static_cast<int>(@<u:;Family;Policy> trine(false)) == F);       \
  static_assert(static_cast<int>(@<u:;Family;Policy> other) == O);
#include "../../examples/CPPP/tests/UnaryCases.def"
#undef UNARY_CASE

// PRINT: @<u:!bijective,idempotent;collapse;other-false;other=false>(other)
// AST: TrineUnaryExpr {{.*}} collapse other-false TFO=TFF
constexpr trine addressed =
    @<u:!bijective,idempotent;collapse;other-false;other=false> other;
static_assert(static_cast<int>(addressed) == 0);
static_assert(static_cast<int>(@<u:;identity;;other=other> other) == -1);
static_assert(static_cast<int>(
    @<u:bijective,inverse,idempotent,fixed;identity> other) == -1);
static_assert(static_cast<int>(
    @<u:bijective,!inverse,!idempotent,!fixed;cycle;forward>
        other) == 0);

template <class T> constexpr trine apply(T value) {
  return @<u:bijective;swap;true-false> value;
}
static_assert(static_cast<int>(apply(true)) == 0);
static_assert(static_cast<int>(apply(other)) == -1);

template <trine V> struct State { static constexpr trine value = V; };
template <class T> struct Box {};
using Nested = Box<State<@<u:;identity> other>>;
static_assert(static_cast<int>(State<@<u:;cycle;forward> other>::value) == 0);

constexpr int single_evaluation() {
  int count = 0;
  @<u:;constant;other> (++count, other); // expected-warning {{expression result unused}}
  return count;
}
static_assert(single_evaluation() == 1);
static_assert(__is_same(decltype(@<u:;identity> true), trine));
static_assert(noexcept(@<u:;identity> other));

constexpr int lifetime() {
  int destroyed = 0;
  struct Probe {
    int &count;
    constexpr ~Probe() { ++count; }
  };
  (void)(@<u:;constant;other> ((void)Probe{destroyed}, true));
  return destroyed;
}
static_assert(lifetime() == 1);
