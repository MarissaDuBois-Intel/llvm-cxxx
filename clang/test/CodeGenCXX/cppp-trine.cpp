// RUN: %clang_cc1 -x c+++ -triple x86_64-unknown-linux-gnu -emit-llvm -O1 -o - %s | FileCheck %s
// RUN: %clang_cc1 -x c+++ -triple x86_64-unknown-linux-gnu -emit-llvm -disable-llvm-passes -o - %s | FileCheck %s --check-prefix=RAW
// RUN: %clang_cc1 -x c+++ -triple x86_64-pc-windows-msvc -emit-llvm -O1 -o - %s | FileCheck %s --check-prefix=MS
// RUN: %clang_cc1 -x c+++ -triple x86_64-unknown-linux-gnu -emit-llvm -debug-info-kind=standalone -o - %s | FileCheck %s --check-prefix=DEBUG

// CHECK: @global_state = {{.*}}global i8 -1
// CHECK: @_ZTIu5trine = {{.*}}@_ZTVN10__cxxabiv123__fundamental_type_infoE
trine global_state = other;

// DEBUG: !DIBasicType(name: "trine", size: 8, encoding: DW_ATE_signed)

// RAW: trunc i8 {{.*}} to i2
// RAW-NEXT: {{.*}}sext i2 {{.*}} to i32
extern "C" int read_trine(const trine &value) {
  return static_cast<int>(value);
}

// CHECK-LABEL: define {{.*}}i32 @read_false(
// CHECK: ret i32 0
extern "C" int read_false() {
  return static_cast<int>(trine(false));
}

// CHECK-LABEL: define {{.*}}i32 @read_true(
// CHECK: ret i32 1
extern "C" int read_true() {
  return static_cast<int>(trine(true));
}

// CHECK-LABEL: define {{.*}}i32 @read_other(
// CHECK: ret i32 -1
extern "C" int read_other() {
  return static_cast<int>(other);
}

// CHECK-LABEL: define {{.*}}void @write_bool(
// CHECK: zext i1 {{.*}} to i8
// CHECK: store i8
extern "C" void write_bool(trine &value, bool b) {
  value = b;
}

// CHECK-LABEL: define {{.*}}void @write_other(
// CHECK: store i8 -1,
extern "C" void write_other(trine &value) {
  value = other;
}

// CHECK-LABEL: define {{.*}}i2 @_Z8identityu5trine(i8
// CHECK: ret i2
// MS-LABEL: define {{.*}}i2 @"?identity@@{{.*}}"(i2
// MS: ret i2
trine identity(trine value) {
  return value;
}

struct Flags {
  trine first : 2;
  trine second : 2;
  trine third : 2;
};

extern "C" int read_bitfield(const Flags &flags) {
  return static_cast<int>(flags.third);
}

extern "C" void write_bitfield(Flags &flags, trine value) {
  flags.third = value;
}

namespace std {
class type_info {};
}

const std::type_info &trine_type_info() {
  return typeid(trine);
}
