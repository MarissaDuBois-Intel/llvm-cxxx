// RUN: %clang_cc1 -x c+++-header -std=c++20 -emit-pch -o %t %s
// RUN: %clang_cc1 -x c+++ -std=c++20 -include-pch %t -fsyntax-only -verify %s
// RUN: %clang_cc1 -x c+++ -std=c++20 -ast-merge %t -DTRINE_UNARY_PCH -fsyntax-only -verify %s
// RUN: %clang_cc1 -x c+++-header -std=c++20 -fexperimental-new-constant-interpreter -emit-pch -o %t.new %s
// RUN: %clang_cc1 -x c+++ -std=c++20 -fexperimental-new-constant-interpreter -include-pch %t.new -fsyntax-only -verify %s
// expected-no-diagnostics

#ifndef TRINE_UNARY_PCH
#define TRINE_UNARY_PCH
template <class T> constexpr trine from_pch(T value) {
  return @<u:!bijective,idempotent;collapse;other-false;other=false> value;
}
constexpr trine stored = @<u:bijective;cycle;forward> true;
#else
static_assert(static_cast<int>(from_pch(other)) == 0);
static_assert(static_cast<int>(from_pch(true)) == 1);
static_assert(static_cast<int>(stored) == -1);
#endif
