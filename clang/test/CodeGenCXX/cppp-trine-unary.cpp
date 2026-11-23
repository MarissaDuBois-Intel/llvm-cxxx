// RUN: %clang_cc1 -triple x86_64-unknown-linux-gnu -x c+++ -std=c++20 -emit-llvm -o - %s | FileCheck %s
// RUN: %clang_cc1 -triple x86_64-unknown-linux-gnu -x c+++ -std=c++20 -fno-trine-other-equal -emit-llvm -o - %s | FileCheck %s
// RUN: %clang_cc1 -triple x86_64-unknown-linux-gnu -x c+++ -std=c++20 -ftrine-other-ordered -emit-llvm -o - %s | FileCheck %s

extern "C" trine next();

// CHECK-LABEL: define {{.*}} @dispatch(
// CHECK: call {{.*}} @next()
// CHECK-NOT: call {{.*}} @next()
// CHECK: icmp eq i2 {{.*}}, 1
// CHECK: icmp eq i2 {{.*}}, 0
// CHECK: select i1
// CHECK: select i1
// CHECK: ret i2
extern "C" trine dispatch() {
  return @<u:bijective;cycle;forward> next();
}

// CHECK-LABEL: define {{.*}} @constant(
// CHECK: call {{.*}} @next()
// CHECK-NOT: call {{.*}} @next()
// CHECK: [[CONST_FO:%.*]] = select i1 {{.*}}, i2 -1, i2 -1
// CHECK: [[CONST_RESULT:%.*]] = select i1 {{.*}}, i2 -1, i2 [[CONST_FO]]
// CHECK: ret i2 [[CONST_RESULT]]
extern "C" trine constant() {
  return @<u:constant;constant;other> next();
}

// CHECK-LABEL: define {{.*}} @volatile_read(
// CHECK: load volatile i8
// CHECK: [[VOLATILE_FO:%.*]] = select i1 {{.*}}, i2 1, i2 1
// CHECK: [[VOLATILE_RESULT:%.*]] = select i1 {{.*}}, i2 1, i2 [[VOLATILE_FO]]
// CHECK: ret i2 [[VOLATILE_RESULT]]
extern "C" trine volatile_read(volatile trine &value) {
  return @<u:;constant;true> value;
}

template <trine V> struct State {};
template <trine V> int dependent(State<@<u:;identity> V>) { return 1; }
// CHECK: define {{.*}}__cxxx_trine_unary_
template int dependent<other>(State<other>);
