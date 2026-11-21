// RUN: %clang_cc1 -x c+++ -std=c++20 -fsyntax-only -verify %s
// RUN: %clang_cc1 -x c+++ -fno-trine-other-equal -DEXPECT_OTHER_EQUAL=0 -fsyntax-only -verify %s
// RUN: %clang_cc1 -x c+++ -ftrine-other-ordered -DEXPECT_ORDERED=1 -fsyntax-only -verify %s
// RUN: %clang_cc1 -x c+++ -fexperimental-new-constant-interpreter -fsyntax-only -verify %s
// RUN: %clang_cc1 -x c+++ -fexperimental-new-constant-interpreter -fno-trine-other-equal -DEXPECT_OTHER_EQUAL=0 -fsyntax-only -verify %s
// RUN: %clang_cc1 -x c+++ -fexperimental-new-constant-interpreter -ftrine-other-ordered -DEXPECT_ORDERED=1 -fsyntax-only -verify %s
// RUN: %clang -x c+++ -fno-trine-other-ordered -ftrine-other-ordered -DEXPECT_ORDERED=1 -fsyntax-only -Xclang -verify %s
// RUN: %clang -x c+++ -ftrine-other-ordered -fno-trine-other-ordered -fno-trine-other-equal -DEXPECT_OTHER_EQUAL=0 -fsyntax-only -Xclang -verify %s
// RUN: not %clang -x c+++ -ftrine-other-ordered -fno-trine-other-equal -fsyntax-only %s 2>&1 | FileCheck %s --check-prefix=CONFLICT
// RUN: not %clang_cc1 -x c+++ -fno-trine-other-equal -ftrine-other-ordered -fsyntax-only %s 2>&1 | FileCheck %s --check-prefix=CONFLICT
// CONFLICT: error: invalid argument '-ftrine-other-ordered' not allowed with '-fno-trine-other-equal'
// expected-no-diagnostics

#include "Inputs/std-compare.h"

#ifndef EXPECT_OTHER_EQUAL
#define EXPECT_OTHER_EQUAL 1
#endif
#ifndef EXPECT_ORDERED
#define EXPECT_ORDERED 0
#endif

static_assert(__is_same(decltype(other <=> other), std::partial_ordering));
static_assert(__is_same(decltype(other <=> true), std::partial_ordering));
static_assert(__is_same(decltype(1u <=> other), std::partial_ordering));
static_assert(__is_same(decltype(other <=> 1), std::partial_ordering));
static_assert(__is_same(decltype(static_cast<int>(other) <=> -1),
                        std::strong_ordering));
static_assert(true == 1 && false < 2 && -1 < true);
static_assert(static_cast<int>(other) == -1);

// Expected: -1 = less, 0 = equivalent, 1 = greater, 2 = unordered.
template <class L, class R>
constexpr bool check(L lhs, R rhs, int expected) {
  auto cmp = lhs <=> rhs;
  auto category = expected == -1 ? std::partial_ordering::less
                  : expected == 0 ? std::partial_ordering::equivalent
                  : expected == 1 ? std::partial_ordering::greater
                                  : std::partial_ordering::unordered;
  return (lhs == rhs) == (expected == 0) &&
         (lhs != rhs) == (expected != 0) &&
         (lhs < rhs) == (expected == -1) &&
         (lhs <= rhs) == (expected == -1 || expected == 0) &&
         (lhs > rhs) == (expected == 1) &&
         (lhs >= rhs) == (expected == 1 || expected == 0) &&
         cmp.test_eq(category);
}

template <class L, class R>
constexpr bool check_pair(L lhs, R rhs, int expected) {
  return check(lhs, rhs, expected) &&
         check(rhs, lhs, expected == 2 ? 2 : -expected);
}

constexpr int expected_state(int i, int j) {
  if (i == 2 && j == 2)
    return EXPECT_OTHER_EQUAL ? 0 : 2;
  if (i == 2 || j == 2)
    return EXPECT_ORDERED ? (i == 2 ? -1 : 1) : 2;
  return i == j ? 0 : i < j ? -1 : 1;
}

constexpr bool check_states() {
  trine values[] = {false, true, other};
  for (int i = 0; i != 3; ++i) {
    for (int j = 0; j != 3; ++j)
      if (!check(values[i], values[j], expected_state(i, j)))
        return false;
    for (int j = 0; j != 2; ++j)
      if (!check_pair(values[i], bool(j), expected_state(i, j)))
        return false;
  }
  return true;
}
static_assert(check_states());

template <class I>
constexpr bool check_integers(I negative, I zero, I one, I large) {
  trine f = false, t = true, o = other;
  int sentinel = EXPECT_ORDERED ? -1 : 2;
  return check_pair(f, negative, 1) && check_pair(t, negative, 1) &&
         check_pair(f, zero, 0) && check_pair(t, one, 0) &&
         check_pair(f, one, -1) && check_pair(t, zero, 1) &&
         check_pair(f, large, -1) && check_pair(t, large, -1) &&
         check_pair(o, negative, sentinel) && check_pair(o, zero, sentinel) &&
         check_pair(o, one, sentinel) && check_pair(o, large, sentinel);
}
static_assert(check_integers(-1, 0, 1, 2));
static_assert(check_integers(-__INT_MAX__ - 1, 0, 1, __INT_MAX__));
static_assert(check_integers(-__LONG_LONG_MAX__ - 1, 0LL, 1LL,
                            __LONG_LONG_MAX__));
using Wide = signed _BitInt(129);
static_assert(check_integers(-(Wide(1) << 100), Wide(0), Wide(1),
                            Wide(1) << 100));

template <class I>
constexpr bool check_unsigned(I maximum) {
  trine f = false, t = true, o = other;
  return check_pair(f, I(0), 0) && check_pair(t, I(1), 0) &&
         check_pair(t, maximum, -1) && check_pair(f, maximum, -1) &&
         check_pair(o, maximum, EXPECT_ORDERED ? -1 : 2) &&
         check_pair(o, I(0), EXPECT_ORDERED ? -1 : 2);
}
static_assert(check_unsigned(~0u));
static_assert(check_unsigned(~0ULL));
static_assert(check_unsigned(~(unsigned _BitInt(129))(0)));
static_assert(check_unsigned((unsigned _BitInt(2))(3)));
static_assert(check_pair(other, static_cast<int>(other),
                         EXPECT_ORDERED ? -1 : 2));

template <class I> constexpr trine counted(I &count) {
  count = count + 1;
  return other;
}

template <class I = int>
constexpr bool check_evaluation_count() {
  I left = 0, right = 0;
  bool equal = counted(left) == counted(right);
  bool unequal = counted(left) != counted(right);
  bool less = counted(left) < counted(right);
  bool le = counted(left) <= counted(right);
  bool greater = counted(left) > counted(right);
  bool ge = counted(left) >= counted(right);
  auto cmp = counted(left) <=> counted(right);
  (void)(counted(left) <=> counted(right));
  (void)(counted(left) <=> static_cast<int>(counted(right)));
  return left == 9 && right == 9 && equal == bool(EXPECT_OTHER_EQUAL) &&
         unequal == !bool(EXPECT_OTHER_EQUAL) && !less && !greater &&
         le == equal && ge == equal && (cmp == 0) == equal;
}
static_assert(check_evaluation_count());

#ifdef CXXX_RUNTIME_TEST
int main() {
  volatile trine values[] = {false, true, other};
  for (int i = 0; i != 3; ++i) {
    for (int j = 0; j != 3; ++j)
      if (!check(values[i], values[j], expected_state(i, j)))
        return 1;
    for (int j = 0; j != 2; ++j) {
      volatile bool b = bool(j);
      if (!check_pair(values[i], b, expected_state(i, j)))
        return 2;
    }
  }
  volatile int negative = -1, zero = 0, one = 1, large = 2;
  volatile long long minimum = -__LONG_LONG_MAX__ - 1;
  volatile unsigned umax = ~0u;
  volatile unsigned long long ullmax = ~0ULL;
  volatile Wide wmin = -(Wide(1) << 100), wmax = Wide(1) << 100;
  volatile unsigned _BitInt(129) uwmax = ~(unsigned _BitInt(129))(0);
  return check_integers(negative, zero, one, large) &&
                 check_integers(minimum, 0LL, 1LL, __LONG_LONG_MAX__) &&
                 check_integers(Wide(wmin), Wide(0), Wide(1), Wide(wmax)) &&
                 check_unsigned(umax) && check_unsigned(ullmax) &&
                 check_unsigned(uwmax) && check_evaluation_count<volatile int>()
             ? 0
             : 3;
}
#endif
