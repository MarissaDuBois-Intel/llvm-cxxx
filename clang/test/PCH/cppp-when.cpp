// RUN: %clang_cc1 -x c+++-header -emit-pch -o %t %s
// RUN: %clang_cc1 -x c+++ -include-pch %t -fsyntax-only -verify %s
// RUN: %clang_cc1 -x c+++ -include-pch %t -fexperimental-new-constant-interpreter -fsyntax-only -verify %s
// RUN: %clang_cc1 -x c+++ -include-pch %t -emit-llvm -o %t.ll %s
// RUN: %clang_cc1 -x c+++-header -fno-trine-other-equal -emit-pch -o %t.unequal %s
// RUN: %clang_cc1 -x c+++ -fno-trine-other-equal -include-pch %t.unequal -fsyntax-only -verify %s
// RUN: echo "" > %t.empty.cpp
// RUN: %clang_cc1 -x c+++ -ast-merge %t -fsyntax-only %t.empty.cpp
// expected-no-diagnostics

#ifndef CXXX_WHEN_PCH
#define CXXX_WHEN_PCH
template <class T> constexpr int select(T value) {
  when (value) {
    false: { return 0; },
    true: { return 1; },
    other: { return -1; },
  }
}
constexpr int omitted(trine value) {
  when (value) { other: { return 7; } }
  return 0;
}
constexpr int header_result = select(other);
template <class T> constexpr int select_bool(T value) {
  when (value) {
    false: { return 0; },
    true: { return 1; }
  }
}
constexpr int header_bool_result = select_bool(true);
template <class T> constexpr int chain(T first, T second, trine fallback) {
  if (first) { return 1; }
  else if (second) { return 2; }
  else when (fallback) {
    other: { return 3; },
    true: { return 4; }
  }
  otherwise if (second) { return 5; }
  else when (fallback) { true: { return 6; } }
  otherwise when (fallback) { other: { return 7; } }
  return 0;
}
#else
static_assert(header_result == -1);
static_assert(header_bool_result == 1);
static_assert(select_bool(false) == 0);
static_assert(select(trine(true)) == 1);
static_assert(select(trine(false)) == 0);
static_assert(omitted(other) == 7);
static_assert(omitted(true) == 0);
static_assert(chain(trine(true), trine(false), other) == 1);
static_assert(chain(trine(false), trine(true), other) == 2);
static_assert(chain(trine(false), trine(false), other) == 3);
static_assert(chain(other, trine(true), other) == 5);
static_assert(chain(other, trine(false), trine(true)) == 6);
static_assert(chain(other, other, other) == 7);
static_assert(chain(trine(false), other, other) == 7);
static_assert(chain(trine(false), trine(false), trine(false)) == 0);
int from_header(trine value) {
  return select(value) + omitted(value) + chain(value, trine(false), other);
}
int bool_from_header(bool value) {
  return select_bool(value);
}
#endif
