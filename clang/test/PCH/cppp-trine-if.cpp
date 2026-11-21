// RUN: %clang_cc1 -x c+++-header -emit-pch -o %t %s
// RUN: %clang_cc1 -x c+++ -include-pch %t -fsyntax-only -verify %s
// RUN: %clang_cc1 -x c+++ -include-pch %t -fexperimental-new-constant-interpreter -fsyntax-only -verify %s
// RUN: %clang_cc1 -x c+++ -include-pch %t -emit-llvm -o %t.ll %s
// RUN: %clang_cc1 -x c+++-header -fno-trine-other-equal -emit-pch -o %t.unequal %s
// RUN: %clang_cc1 -x c+++ -fno-trine-other-equal -include-pch %t.unequal -fsyntax-only -verify %s
// expected-no-diagnostics

#ifndef CXXX_TRINE_IF_PCH
#define CXXX_TRINE_IF_PCH
template <class T> constexpr int select(T first, T second) {
  int result = 9;
  if (first) result = 1;
  else if (second) result = 2;
  else result = 0;
  otherwise result = -1;
  return result;
}
constexpr int header_result = select(other, trine(true));
#else
static_assert(header_result == -1);
static_assert(select(other, trine(true)) == -1);
static_assert(select(trine(false), other) == -1);
static_assert(select(false, true) == 2);
static_assert(select(trine(false), trine(false)) == 0);
int from_header(trine first, trine second) { return select(first, second); }
#endif
