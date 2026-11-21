// RUN: %clang_cc1 -x c+++ -triple x86_64-pc-windows-msvc -emit-llvm -o - %s | FileCheck %s
// RUN: %clang_cc1 -x c+++ -triple x86_64-pc-windows-msvc -fno-trine-other-equal -emit-llvm -o - %s | FileCheck %s
// RUN: %clang_cc1 -x c+++ -triple x86_64-pc-windows-msvc -ftrine-other-ordered -emit-llvm -o - %s | FileCheck %s
// RUN: %clang_cc1 -x c+++ -triple x86_64-pc-windows-msvc -fprofile-instrument=clang -fcoverage-mapping -emit-llvm -o %t.ll %s
// RUN: %clang_analyze_cc1 -x c+++ -analyzer-checker=core -verify %s
// expected-no-diagnostics

extern "C" trine read_state();
extern "C" void yes();
extern "C" void no();
extern "C" void unknown();

// CHECK-LABEL: define {{.*}}void @branch(
// CHECK-COUNT-1: call i2 @read_state()
// CHECK: call void @yes()
// CHECK: call void @no()
// CHECK: call void @unknown()
// CHECK: ret void
extern "C" void branch() {
  if (read_state())
    yes();
  else
    no();
  otherwise
    unknown();
}
