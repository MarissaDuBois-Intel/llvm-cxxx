// RUN: %clang -x c++ -fsyntax-only -Xclang -verify %s
// RUN: %clang -x c -fsyntax-only -Xclang -verify %s
// expected-no-diagnostics

int when(int value) { return value; }
int use_when(void) { return when(1); }
