// RUN: %clang_cc1 -x c+++ -triple x86_64-unknown-linux-gnu -emit-llvm -O0 -o - %s | FileCheck %s
// RUN: %clang_cc1 -x c+++ -triple x86_64-unknown-linux-gnu -fno-trine-other-equal -emit-llvm -O0 -o - %s | FileCheck %s
// RUN: %clang_cc1 -x c+++ -triple x86_64-unknown-linux-gnu -ftrine-other-ordered -emit-llvm -O0 -o - %s | FileCheck %s
// RUN: %clang_cc1 -x c+++ -triple x86_64-unknown-linux-gnu -fprofile-instrument=clang -fcoverage-mapping -dump-coverage-mapping -emit-llvm -o %t.ll %s | FileCheck %s --check-prefix=COVERAGE
// RUN: %clang_cc1 -x c+++ -triple x86_64-unknown-linux-gnu -fprofile-instrument=clang -fcoverage-mapping -mllvm -enable-single-byte-coverage -emit-llvm -o %t.byte.ll %s

trine next();
bool next_bool();
void yes();
void no();
void unknown();

// CHECK-LABEL: define {{.*}} @_Z6choosev
// CHECK: call {{.*}} @_Z4nextv()
// CHECK: icmp eq i2 {{.*}}, 1
// CHECK: call void @_Z3yesv()
// CHECK: icmp eq i2 {{.*}}, 0
// CHECK: call void @_Z2nov()
// CHECK: call void @_Z7unknownv()
// CHECK: ret void
// COVERAGE-LABEL: _Z6choosev:
void choose() {
  when (next()) {
    // COVERAGE: File 0, [[#@LINE+1]]:12 -> [[#@LINE+1]]:26 = #3
    other: { unknown(); },
    // COVERAGE: File 0, [[#@LINE+1]]:12 -> [[#@LINE+1]]:21 = #2
    false: { no(); },
    // COVERAGE: File 0, [[#@LINE+1]]:11 -> [[#@LINE+1]]:21 = #1
    true: { yes(); }
  }
}

// COVERAGE-LABEL: _Z6singlev:
void single() {
  // COVERAGE: File 0, [[#@LINE+1]]:26 -> [[#@LINE+1]]:40 = #3
  when (next()) { other: { unknown(); } }
}

// CHECK-LABEL: define {{.*}} @_Z5chainv
// CHECK-COUNT-6: call {{.*}} @_Z4nextv()
// CHECK: ret void
// COVERAGE-LABEL: _Z5chainv:
void chain() {
  if (next()) { yes(); }
  else if (next()) { yes(); }
  else when (next()) {
    other: { unknown(); },
    false: { no(); }
  }
  // COVERAGE: File 0, [[#@LINE+1]]:17 -> [[#@LINE+1]]:23 = #8
  otherwise if (next()) { yes(); }
  // COVERAGE: File 0, [[#@LINE+1]]:14 -> [[#@LINE+1]]:20 = #10
  else when (next()) { true: { yes(); } }
  // COVERAGE: File 0, [[#@LINE+2]]:19 -> [[#@LINE+2]]:25 = #14
  // COVERAGE: File 0, [[#@LINE+1]]:36 -> [[#@LINE+1]]:50 = #17
  otherwise when (next()) { other: { unknown(); } }
}

// CHECK-LABEL: define {{.*}} @_Z11choose_boolv
// CHECK: call {{.*}} @_Z9next_boolv()
// CHECK: call void @_Z3yesv()
// CHECK: call void @_Z2nov()
// CHECK: ret void
// COVERAGE-LABEL: _Z11choose_boolv:
void choose_bool() {
  when (next_bool()) {
    // COVERAGE: Branch,File 0, [[#@LINE-1]]:9 -> [[#@LINE-1]]:20 = #1, (#0 - #1)
    // COVERAGE: File 0, [[#@LINE+1]]:12 -> [[#@LINE+1]]:21 = (#0 - #1)
    false: { no(); },
    // COVERAGE: File 0, [[#@LINE+1]]:11 -> [[#@LINE+1]]:21 = #1
    true: { yes(); }
  }
}
