// -*- mode: c+++ -*-
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#if defined(CASE_ALIAS)
Trine value; // expected-error {{unknown type name 'Trine'}}

#elif defined(CASE_INTEGER_INIT)
trine value = 2; // expected-error {{cannot initialize}}

#elif defined(CASE_IMPLICIT_BOOL)
bool value = other; // expected-error {{cannot initialize}}

#elif defined(CASE_IMPLICIT_INT)
int value = other; // expected-error {{cannot initialize}}

#elif defined(CASE_ARITHMETIC)
auto value = other + true; // expected-error {{invalid operands}}

#elif defined(CASE_LOGICAL)
auto value = !other; // expected-error {{invalid argument type 'trine'}}

#elif defined(CASE_WHEN_INT)
void test() {
  when (1) { true: {} } // expected-error {{'when' requires a bool or trine expression}}
}

#elif defined(CASE_WHEN_EMPTY)
void test() {
  when (other) {} // expected-error {{'when' requires at least one case}}
}

#elif defined(CASE_WHEN_DUPLICATE)
void test() {
  when (other) {
    true: {}, // expected-note {{previous case is here}}
    true: {} // expected-error {{duplicate 'true' case}}
  }
}

#elif defined(CASE_WHEN_SEPARATOR)
void test() {
  when (other) {
    true: {}
    false: {} // expected-error {{expected ',' between 'when' cases}}
  }
}

#elif defined(CASE_WHEN_SEMICOLON)
void test() {
  when (other) {
    true: {};
    // expected-error@-1 {{expected ',' between 'when' cases}}
  }
}

#elif defined(CASE_WHEN_UNBRACED)
void test() {
  when (other) { other: return; } // expected-error {{expected '{'}}
}

#elif defined(CASE_WHEN_BREAK)
void test() {
  when (other) {
    other: { break; } // expected-error {{'break' statement not in loop or switch statement}}
  }
}

#elif defined(CASE_BOOL_OTHER)
// quiet-no-diagnostics
void test(bool value) {
  when (value) {
    other: {}
    // warning-warning@-1 {{'other' case is never selected by a 'bool' condition}}
    // error-error@-2 {{'other' case is never selected by a 'bool' condition}}
  }
}

#elif defined(CASE_TEMPLATE_OTHER)
template <class T> void select(T value) {
  when (value) {
    other: {} // expected-warning {{'other' case is never selected by a 'bool' condition}}
  }
}
void test() {
  select(other);
  select(true); // expected-note {{in instantiation of function template specialization 'select<bool>' requested here}}
}

#elif defined(CASE_MISSING_OTHERWISE)
// quiet-no-diagnostics
void test(trine value) {
  if (value) {}
  // warning-warning@-1 {{trine condition has no 'otherwise' handler}}
}

#elif defined(CASE_UNARY_CONSTANT)
auto value = @<u:constant;identity> other;
// expected-error@-1 {{does not satisfy property 'constant'}}

#elif defined(CASE_UNARY_BIJECTIVE)
auto value = @<u:bijective;constant;true> other;
// expected-error@-1 {{does not satisfy property 'bijective'}}

#elif defined(CASE_UNARY_SELF_INVERSE)
auto value = @<u:inverse;cycle;forward> other;
// expected-error@-1 {{does not satisfy property 'inverse'}}

#elif defined(CASE_UNARY_IDEMPOTENT)
auto value = @<u:idempotent;swap;true-false> other;
// expected-error@-1 {{does not satisfy property 'idempotent'}}

#elif defined(CASE_UNARY_FIXED_POINT)
auto value = @<u:fixed;feed;true-false-true> other;
// expected-error@-1 {{does not satisfy property 'fixed'}}

#elif defined(CASE_UNARY_NOT_CONSTANT)
auto value = @<u:!constant;constant;other> other;
// expected-error@-1 {{does not satisfy property '!constant'}}

#elif defined(CASE_UNARY_NOT_BIJECTIVE)
auto value = @<u:!bijective;identity> other;
// expected-error@-1 {{does not satisfy property '!bijective'}}

#elif defined(CASE_UNARY_NOT_SELF_INVERSE)
auto value = @<u:!inverse;identity> other;
// expected-error@-1 {{does not satisfy property '!inverse'}}

#elif defined(CASE_UNARY_NOT_IDEMPOTENT)
auto value = @<u:!idempotent;collapse;other-false> other;
// expected-error@-1 {{does not satisfy property '!idempotent'}}

#elif defined(CASE_UNARY_NO_FIXED_POINT)
auto value = @<u:!fixed;constant;false> other;
// expected-error@-1 {{does not satisfy property '!fixed'}}

#elif defined(CASE_UNARY_ENTRY_CONFLICT)
auto value = @<u:;identity;;other=false> other;
// expected-error@-1 {{entry 'other=false' conflicts with selected truth table (expected 'other=other')}}

#elif defined(CASE_UNARY_UNKNOWN_PROPERTY)
auto value = @<u:commutative;identity> other;
// expected-error@-1 {{unknown property 'commutative'}}

#elif defined(CASE_UNARY_CONTRADICTION)
auto value = @<u:bijective,!bijective;identity> other;
// expected-error@-1 {{duplicate or contradictory property 'bijective'}}

#elif defined(CASE_UNARY_FAMILY)
auto value = @<u:;unknown> other;
// expected-error@-1 {{unknown family 'unknown'}}

#elif defined(CASE_UNARY_POLICY)
auto value = @<u:;swap;forward> other;
// expected-error@-1 {{invalid or missing policy 'forward' for family 'swap'}}

#elif defined(CASE_UNARY_MISSING_POLICY)
auto value = @<u:;constant> other;
// expected-error@-1 {{invalid or missing policy '' for family 'constant'}}

#elif defined(CASE_UNARY_DUPLICATE_ENTRY)
auto value = @<u:;identity;;other=other,other=other> other;
// expected-error@-1 {{duplicate explicit entry for 'other'}}

#elif defined(CASE_UNARY_ENTRY_INPUT)
auto value = @<u:;identity;;unknown=false> other;
// expected-error@-1 {{expected an input state in explicit entry}}

#elif defined(CASE_UNARY_ENTRY_OUTPUT)
auto value = @<u:;identity;;other=unknown> other;
// expected-error@-1 {{expected an output state in explicit entry}}

#elif defined(CASE_UNARY_ENTRY_EQUAL)
auto value = @<u:;identity;;other:false> other;
// expected-error@-1 {{expected '=' in explicit entry}}

#elif defined(CASE_UNARY_ARITY)
auto value = @<x:;identity> other;
// expected-error@-1 {{expected arity 'u'}}

#elif defined(CASE_UNARY_BINARY)
auto value = @<b:;identity> other;
// expected-error@-1 {{binary logical operators are not implemented}}

#elif defined(CASE_UNARY_COLON)
auto value = @<u;identity> other;
// expected-error@-1 {{expected ':' after arity}}

#elif defined(CASE_UNARY_SEPARATOR)
auto value = @<u:bijective identity> other;
// expected-error@-1 {{expected ';' before family}}

#elif defined(CASE_UNARY_INT)
auto value = @<u:;identity> 1;
// expected-error@-1 {{requires a bool or trine operand ('int' invalid)}}

#elif defined(CASE_UNARY_CONVERTIBLE_BOOL)
struct Convertible { operator bool(); };
auto value = @<u:;identity> Convertible{};
// expected-error@-1 {{requires a bool or trine operand ('Convertible' invalid)}}

#elif defined(CASE_UNARY_CONVERTIBLE_TRINE)
struct Convertible { operator trine(); };
auto value = @<u:;identity> Convertible{};
// expected-error@-1 {{requires a bool or trine operand ('Convertible' invalid)}}

#elif defined(CASE_UNARY_TEMPLATE_INT)
template <class T> trine select(T value) {
  return @<u:;identity> value;
  // expected-error@-1 {{requires a bool or trine operand ('int' invalid)}}
}
auto value = select(1);
// expected-note@-1 {{in instantiation of function template specialization 'select<int>' requested here}}

#else
#error Select one diagnostic scenario with CASE_*.
#endif
