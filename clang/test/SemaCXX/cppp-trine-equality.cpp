// RUN: %clang_cc1 -x c+++ -fsyntax-only -verify %s
// RUN: %clang_cc1 -x c+++ -fno-trine-other-equal -DEXPECT_OTHER_EQUAL=0 -fsyntax-only -verify %s
// RUN: %clang_cc1 -x c+++ -fno-trine-other-equal -ftrine-other-equal -fsyntax-only -verify %s
// RUN: %clang_cc1 -x c+++ -fexperimental-new-constant-interpreter -fsyntax-only -verify %s
// RUN: %clang_cc1 -x c+++ -fexperimental-new-constant-interpreter -fno-trine-other-equal -DEXPECT_OTHER_EQUAL=0 -fsyntax-only -verify %s
// RUN: %clang -x cppp -fno-trine-other-equal -DEXPECT_OTHER_EQUAL=0 -fsyntax-only -Xclang -verify %s
// RUN: %clang -x cppp -fno-trine-other-equal -ftrine-other-equal -fsyntax-only -Xclang -verify %s
// RUN: %clang -x cppp -ftrine-other-equal -fno-trine-other-equal -DEXPECT_OTHER_EQUAL=0 -fsyntax-only -Xclang -verify %s
// expected-no-diagnostics

#ifndef EXPECT_OTHER_EQUAL
#define EXPECT_OTHER_EQUAL 1
#endif

static_assert((other == other) == bool(EXPECT_OTHER_EQUAL));
static_assert((other != other) == !bool(EXPECT_OTHER_EQUAL));
static_assert(true == true && false == false && true != false);
static_assert(other != true && true != other);
static_assert(other != false && false != other);

constexpr bool check_matrix() {
  trine values[] = {false, true, other};
  for (int i = 0; i != 3; ++i) {
    for (int j = 0; j != 3; ++j) {
      bool expected = i == j && (i != 2 || EXPECT_OTHER_EQUAL);
      if ((values[i] == values[j]) != expected ||
          (values[i] != values[j]) == expected)
        return false;
    }
  }
  return true;
}
static_assert(check_matrix());

constexpr trine counted_other(int &count) {
  ++count;
  return other;
}

constexpr bool check_evaluation_count() {
  int left = 0, right = 0;
  bool equal = counted_other(left) == counted_other(right);
  bool unequal = counted_other(left) != counted_other(right);
  return left == 2 && right == 2 && equal == bool(EXPECT_OTHER_EQUAL) &&
         unequal == !bool(EXPECT_OTHER_EQUAL);
}
static_assert(check_evaluation_count());

#ifdef CXXX_RUNTIME_TEST
trine runtime_counted_other(volatile int &count) {
  count = count + 1;
  return other;
}

int main() {
  volatile trine values[] = {false, true, other};
  for (int i = 0; i != 3; ++i) {
    for (int j = 0; j != 3; ++j) {
      bool expected = i == j && (i != 2 || EXPECT_OTHER_EQUAL);
      if ((values[i] == values[j]) != expected ||
          (values[i] != values[j]) == expected)
        return 1;
    }
  }
  volatile int left = 0, right = 0;
  bool equal = runtime_counted_other(left) == runtime_counted_other(right);
  bool unequal = runtime_counted_other(left) != runtime_counted_other(right);
  return left == 2 && right == 2 && equal == bool(EXPECT_OTHER_EQUAL) &&
                 unequal == !bool(EXPECT_OTHER_EQUAL)
             ? 0
             : 1;
}
#endif
