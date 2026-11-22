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

#else
#error Select one diagnostic scenario with CASE_*.
#endif
