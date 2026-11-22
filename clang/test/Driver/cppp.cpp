// RUN: %clang -x c+++ -fsyntax-only %s
// RUN: %clang -x C+++ -fsyntax-only %s
// RUN: %clang -x cppp -fsyntax-only %s
// RUN: %clang -x c-plus-plus-plus -fsyntax-only %s
// RUN: %clang -x cxxx -fsyntax-only %s
// RUN: %clang -x CXXX -fsyntax-only %s
// RUN: %clang -x cppp -nostdinc -fsyntax-only %s
// RUN: %clang -x cppp --target=x86_64-pc-windows-msvc -fsyntax-only %s
// RUN: %clang -x cppp --target=x86_64-unknown-linux-gnu -fsyntax-only %s
// RUN: %clang -x cppp -std=c++23 -DEXPECT_STD=202302L -fsyntax-only %s
// RUN: %clang -x cppp -E %s -o %t.ii
// RUN: %clang -x cppp-cpp-output -fsyntax-only %t.ii
// RUN: %clang -x cppp -save-temps -c %s -o %t.o
// RUN: %clang -x cppp-header -fsyntax-only %s
// RUN: cp %s %t.c
// RUN: %clang -x cppp -fsyntax-only %t.c
// RUN: not %clang -x cppp -std=c17 -fsyntax-only %s 2>&1 | FileCheck %s --check-prefix=BAD-C
// RUN: not %clang -x cppp -std=c++98 -fsyntax-only %s 2>&1 | FileCheck %s --check-prefix=BAD-CXX
// RUN: %clang -x c++ -std=c++17 -fsyntax-only -DORDINARY %s
// RUN: %clang -x c -std=c17 -fsyntax-only -DORDINARY %s
// RUN: %clang -std=c++17 -fsyntax-only -DORDINARY %s
// RUN: %clang -std=c17 -fsyntax-only -DORDINARY %t.c

// BAD-C: error: invalid argument '-std=c17' not allowed with 'C+++'
// BAD-CXX: error: invalid argument '-std=c++98' not allowed with 'C+++'

int Trine;

#ifdef ORDINARY
#ifdef __cxxx__
#error C+++ must be opt-in
#endif
int trine;
int other;
int otherwise;
#else
#ifndef __cxxx__
#error Missing C+++ language macro
#endif
#ifndef EXPECT_STD
#define EXPECT_STD 202700L
#endif
static_assert(__cplusplus == EXPECT_STD, "C+++ must retain C++ standards");
static_assert(static_cast<int>(other) == -1, "");
trine value = true;
#if __cplusplus >= 202400L
template <typename... Ts> using first_type = Ts...[0];
static_assert(__is_same(first_type<int, long>, int));
#endif
#endif
