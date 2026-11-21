// RUN: %clang_cc1 -x c+++ -Wtrine-missing-otherwise -fsyntax-only -verify %s
// RUN: not %clang -x c+++ -Werror=trine-missing-otherwise -fsyntax-only %s 2>&1 | FileCheck %s
// CHECK: error: trine condition has no 'otherwise' handler

void missing(trine state) {
  if (state) {} // expected-warning {{trine condition has no 'otherwise' handler}}
  if (state) {} else {} // expected-warning {{trine condition has no 'otherwise' handler}}
  if (false) {} else if (state) {} // expected-warning {{trine condition has no 'otherwise' handler}}
  if (state == true) {}
  if (state == other) {}
  if (state) {} otherwise {}
  if (state) {} else if (state) {} otherwise {}
}

void invalid(trine state) {
  bool value = state; // expected-error {{cannot initialize}}
  while (state) {} // expected-error {{not contextually convertible to 'bool'}}
  (void)(state ? 1 : 0); // expected-error {{not contextually convertible to 'bool'}}
  if constexpr (true) {} otherwise {} // expected-error {{'otherwise' is not supported}}
  if consteval {} otherwise {} // expected-error {{'otherwise' is not supported}}
  otherwise {} // expected-error {{expected expression}}
}

void scopes(trine state) {
  if (int root = 1; false) {}
  else if (int later = 2; state) {}
  otherwise {
    (void)root;
    (void)later; // expected-error {{use of undeclared identifier 'later'}}
  }
}

void duplicate() {
  if (true) {} otherwise {} otherwise {} // expected-error {{expected expression}}
}
