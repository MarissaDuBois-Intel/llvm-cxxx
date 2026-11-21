// RUN: %clang_cc1 -x c+++-header -emit-pch -o %t %s
// RUN: %clang_cc1 -x c+++ -include-pch %t -fsyntax-only -verify %s
// RUN: not %clang_cc1 -x c++ -std=c++2d -include-pch %t -fsyntax-only %s 2>&1 | FileCheck %s
// RUN: not %clang_cc1 -x c+++ -fno-trine-other-equal -include-pch %t -fsyntax-only %s 2>&1 | FileCheck %s --check-prefix=FLAG
// RUN: %clang_cc1 -x c+++-header -fno-trine-other-equal -emit-pch -o %t.unequal %s
// RUN: %clang_cc1 -x c+++ -fno-trine-other-equal -include-pch %t.unequal -fsyntax-only -verify %s
// RUN: not %clang_cc1 -x c+++ -ftrine-other-ordered -include-pch %t -fsyntax-only %s 2>&1 | FileCheck %s --check-prefix=ORDER
// RUN: %clang_cc1 -x c+++-header -ftrine-other-ordered -emit-pch -o %t.ordered %s
// RUN: %clang_cc1 -x c+++ -ftrine-other-ordered -include-pch %t.ordered -fsyntax-only -verify %s
// RUN: not %clang_cc1 -x c+++ -include-pch %t.ordered -fsyntax-only %s 2>&1 | FileCheck %s --check-prefix=UNORDER

// CHECK: C+++ was enabled in precompiled file {{.*}} but is currently disabled
// FLAG: trine 'other' self-equality was enabled in precompiled file {{.*}} but is currently disabled
// ORDER: trine 'other' ordering was disabled in precompiled file {{.*}} but is currently enabled
// UNORDER: trine 'other' ordering was enabled in precompiled file {{.*}} but is currently disabled
// expected-no-diagnostics

#ifndef CXXX_TRINE_PCH
#define CXXX_TRINE_PCH
constexpr trine from_header = other;
constexpr bool header_other_equal = other == other;
constexpr bool header_other_ordered = other < false;
constexpr bool header_mixed_ordered = other < -1;
#else
static_assert(__is_fundamental(decltype(from_header)));
static_assert((from_header == other) == header_other_equal);
static_assert((from_header < false) == header_other_ordered);
static_assert((from_header < -1) == header_mixed_ordered);
static_assert(static_cast<int>(from_header) == -1);
Trine from_pch = true;
#endif
