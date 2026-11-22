// RUN: %clang_cc1 -x c+++ -fsyntax-only -verify=expected,bool-warning %s
// RUN: %clang_cc1 -x c+++ -Wno-when-bool-other -fsyntax-only -verify=expected %s
// RUN: %clang_cc1 -x c+++ -Werror=when-bool-other -fsyntax-only -verify=expected,bool-error %s

void syntax(trine state) {
  when (state) {} // expected-error {{'when' requires at least one case}}
  when (state) {
    true: {}, // expected-note {{previous case is here}}
    true: {} // expected-error {{duplicate 'true' case in 'when'}}
  }
  when (state) {
    false: {}, // expected-note {{previous case is here}}
    false: {} // expected-error {{duplicate 'false' case in 'when'}}
  }
  when (state) {
    other: {}, // expected-note {{previous case is here}}
    other: {} // expected-error {{duplicate 'other' case in 'when'}}
  }
  when (state) {
    true: {};
    // expected-error@-1 {{expected ',' between 'when' cases}}
  }
  when (state) {
    true: {}
    false: {} // expected-error {{expected ',' between 'when' cases}}
  }
  when (state) { default: {} } // expected-error {{expected 'true', 'false', or 'other' case in 'when'}}
  when (state) { 1: {} } // expected-error {{expected 'true', 'false', or 'other' case in 'when'}}
  when (state) { true {} } // expected-error {{expected ':'}}
  when (state) { true: return; } // expected-error {{expected '{'}}
  when (state) { true: {},, } // expected-error {{expected 'true', 'false', or 'other' case in 'when'}}
  when (state) { true: { break; } } // expected-error {{'break' statement not in loop or switch statement}}
  when (state) { true: { continue; } } // expected-error {{'continue' statement not in loop statement}}
  when (state) {
    true: { int local = 1; },
    false: { local = 0; } // expected-error {{use of undeclared identifier 'local'}}
  }
}

struct Convertible {
  operator trine() const;
  operator bool() const;
};
void conditions(Convertible value) {
  when (1) { true: {} } // expected-error {{'when' requires a bool or trine expression ('int' invalid)}}
  when (value) { true: {} } // expected-error {{'when' requires a bool or trine expression ('Convertible' invalid)}}
}

template <class T> void dependent(T value) {
  when (value) { // expected-error {{'when' requires a bool or trine expression ('int' invalid)}}
    other: {}
    // bool-warning-warning@-1 {{'other' case is never selected by a 'bool' condition}}
    // bool-error-error@-2 {{'other' case is never selected by a 'bool' condition}}
  }
}
void instantiate() {
  dependent(other);
  dependent(true);
  // bool-warning-note@-1 {{in instantiation of function template specialization 'dependent<bool>' requested here}}
  // bool-error-note@-2 {{in instantiation of function template specialization 'dependent<bool>' requested here}}
  dependent(1); // expected-note {{in instantiation of function template specialization 'dependent<int>' requested here}}
}

constexpr int boolean_other(bool value) {
  int result = 0;
  when (value) {
    other: { result = 1; }
    // bool-warning-warning@-1 {{'other' case is never selected by a 'bool' condition}}
    // bool-error-error@-2 {{'other' case is never selected by a 'bool' condition}}
  }
  return result;
}
static_assert(boolean_other(false) == 0);
static_assert(boolean_other(true) == 0);
