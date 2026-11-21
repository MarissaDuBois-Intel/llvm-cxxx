// RUN: %clang_cc1 -x c+++ -triple x86_64-unknown-linux-gnu -emit-llvm -disable-llvm-passes -o - %s | FileCheck %s --check-prefix=EQUAL
// RUN: %clang_cc1 -x c+++ -triple x86_64-unknown-linux-gnu -fno-trine-other-equal -emit-llvm -disable-llvm-passes -o - %s | FileCheck %s --check-prefix=UNEQUAL

// EQUAL-LABEL: define {{.*}}i1 @equal(
// EQUAL: icmp eq i2
// EQUAL: select i1 {{.*}}, i8 0, i8 4
// EQUAL: icmp eq i8 {{.*}}, 0
// EQUAL: ret i1
// UNEQUAL-LABEL: define {{.*}}i1 @equal(
// UNEQUAL: icmp eq i2
// UNEQUAL: select i1 {{.*}}, i8 4, i8
// UNEQUAL: icmp eq i8 {{.*}}, 0
// UNEQUAL: ret i1
extern "C" bool equal(trine lhs, trine rhs) {
  return lhs == rhs;
}

// EQUAL-LABEL: define {{.*}}i1 @unequal(
// EQUAL: icmp eq i2
// EQUAL: icmp eq i8 {{.*}}, 0
// EQUAL: xor i1 {{.*}}, true
// EQUAL: ret i1
// UNEQUAL-LABEL: define {{.*}}i1 @unequal(
// UNEQUAL: icmp eq i2
// UNEQUAL: select i1 {{.*}}, i8 4, i8
// UNEQUAL: icmp eq i8 {{.*}}, 0
// UNEQUAL: xor i1 {{.*}}, true
// UNEQUAL: ret i1
extern "C" bool unequal(trine lhs, trine rhs) {
  return lhs != rhs;
}

// EQUAL-LABEL: define {{.*}}i1 @constant_self(
// EQUAL: ret i1 true
// UNEQUAL-LABEL: define {{.*}}i1 @constant_self(
// UNEQUAL: ret i1 false
extern "C" bool constant_self() {
  return other == other;
}
