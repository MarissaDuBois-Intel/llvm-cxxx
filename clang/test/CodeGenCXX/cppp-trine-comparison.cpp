// RUN: %clang_cc1 -x c+++ -triple x86_64-unknown-linux-gnu -emit-llvm -disable-llvm-passes -o - %s | FileCheck %s
// RUN: %clang_cc1 -x c+++ -triple x86_64-unknown-linux-gnu -ftrine-other-ordered -emit-llvm -disable-llvm-passes -o - %s | FileCheck %s
// RUN: %clang_cc1 -x c+++ -triple x86_64-unknown-linux-gnu -fno-trine-other-equal -emit-llvm -disable-llvm-passes -o - %s | FileCheck %s

#include "../SemaCXX/Inputs/std-compare.h"

// CHECK-LABEL: define {{.*}}i1 @less(
// CHECK: sext i2 {{.*}} to i32
// CHECK: icmp eq i32 {{.*}}, -1
// CHECK: icmp slt i32
// CHECK: icmp eq i8 {{.*}}, 2
// CHECK: ret i1
extern "C" bool less(trine lhs, int rhs) {
  return lhs < rhs;
}

// CHECK-LABEL: define {{.*}}i1 @greater_equal(
// CHECK: icmp ult i32
// CHECK: icmp eq i8 {{.*}}, 3
// CHECK: or i1
// CHECK: ret i1
extern "C" bool greater_equal(unsigned lhs, trine rhs) {
  return lhs >= rhs;
}

// CHECK-LABEL: define {{.*}} @_Z9three_way
// CHECK: icmp eq i32 {{.*}}, -1
// CHECK: select i1 {{.*}}, i8 0, i8 -127
// CHECK: select i1 {{.*}}, i8 1, i8
// CHECK: select i1 {{.*}}, i8 -1, i8
std::partial_ordering three_way(trine lhs, int rhs) {
  return lhs <=> rhs;
}
