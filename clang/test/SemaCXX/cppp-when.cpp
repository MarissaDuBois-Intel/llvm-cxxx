// RUN: %clang_cc1 -x c+++ -std=c++20 -Werror -Wtrine-missing-otherwise -fsyntax-only -verify %s
// RUN: %clang_cc1 -x c+++ -fexperimental-new-constant-interpreter -fsyntax-only -verify %s
// RUN: %clang_cc1 -x c+++ -fno-trine-other-equal -fsyntax-only -verify %s
// RUN: %clang_cc1 -x c+++ -fno-trine-other-equal -fexperimental-new-constant-interpreter -fsyntax-only -verify %s
// RUN: %clang_cc1 -x c+++ -ftrine-other-ordered -fsyntax-only -verify %s
// RUN: %clang_cc1 -x c+++ -ftrine-other-ordered -fexperimental-new-constant-interpreter -fsyntax-only -verify %s
// RUN: %clang_cc1 -x c+++ -ast-print %s -o %t.cpp
// RUN: %clang_cc1 -x c+++ -fsyntax-only %t.cpp
// RUN: %clang_cc1 -x c+++ -ast-dump -ast-dump-filter=dispatch %s | FileCheck %s --check-prefix=AST
// expected-no-diagnostics

template <class T> constexpr int dispatch(T state, int &checks) {
  when ((++checks, state)) {
    other: { int result = -1; return result; },
    false: { int result = 0; return result; },
    true: { int result = 1; return result; }
  }
}
// AST: WhenStmt {{.*}} cases other false true

constexpr int subsets(trine state) {
  int result = 0;
  when (state) { true: { result += 1; } }
  when (state) { false: { result += 2; }, }
  when (state) { other: { result += 4; } }
  when (state) {
    other: { result += 8; },
    true: { result += 16; }
  }
  when (state) {
    false: { result += 32; },
    other: { result += 64; },
  }
  when (state) {
    true: { result += 128; },
    false: { result += 256; }
  }
  return result;
}

struct Guard {
  int &active;
  constexpr Guard(int &active) : active(active) { ++active; }
  constexpr ~Guard() { --active; }
  constexpr trine state() const { return other; }
};

constexpr bool lifetime_and_nesting() {
  int active = 0, result = 0;
  when (Guard(active).state()) {
    other: {
      if (active != 0) return false;
      Guard local(active);
      when (trine(true)) {
        true: { result += active; },
      }
    }
  }
  return active == 0 && result == 1;
}

constexpr bool control_flow() {
  int result = 0;
  for (int i = 0; i < 4; ++i) {
    when (other) {
      other: {
        ++result;
        if (i == 1) break;
        continue;
      }
    }
    return false;
  }
  switch (result) {
  case 2:
    when (other) { other: { break; } }
    return false;
  default: return false;
  }
  return result == 2;
}

constexpr bool check(trine state, int expected) {
  int checks = 0;
  int ignored = 0;
  when ((++ignored, state)) { true: {} }
  return dispatch(state, checks) == expected && checks == 1 && ignored == 1;
}

constexpr bool boolean_cases(bool state) {
  int checks = 0, selected = -1, true_only = 0, false_only = 0;
  when ((++checks, state)) {
    false: { selected = 0; },
    true: { selected = 1; }
  }
  when (state) { true: { ++true_only; } }
  when (state) { false: { ++false_only; } }
  return checks == 1 && selected == static_cast<int>(state) &&
         true_only == static_cast<int>(state) && false_only == !state;
}

static_assert(check(false, 0));
static_assert(check(true, 1));
static_assert(check(other, -1));
static_assert(boolean_cases(false));
static_assert(boolean_cases(true));
static_assert(subsets(false) == 290);
static_assert(subsets(true) == 145);
static_assert(subsets(other) == 76);
static_assert(lifetime_and_nesting());
static_assert(control_flow());

template <class T>
constexpr int chained(T first, T second, trine fallback, trine handler,
                      int (&checks)[4]) {
  if ((++checks[0], first)) { return 10; }
  else if ((++checks[1], second)) { return 20; }
  else when ((++checks[2], fallback)) {
    other: { return 32; },
    false: { return 30; },
    true: { return 31; }
  }
  otherwise when ((++checks[3], handler)) {
    true: { return 41; },
    other: { return 42; },
    false: { return 40; }
  }
}

template <class T>
constexpr int conditional_otherwise(trine first, T handler, trine fallback,
                                    int (&checks)[3]) {
  if ((++checks[0], first)) { return 1; }
  else { return 0; }
  otherwise if ((++checks[1], handler)) { return 2; }
  else when ((++checks[2], fallback)) {
    other: { return 3; },
    true: { return 4; }
  }
  otherwise when ((++checks[2], fallback)) {
    false: { return 5; }
  }
  return 9;
}

template <class T> constexpr bool chains(const T (&states)[3]) {
  for (int a = 0; a < 3; ++a)
    for (int b = 0; b < 3; ++b)
      for (int c = 0; c < 3; ++c) {
        int nested_checks[3] = {};
        int nested_expected = a == 0 ? 0 : a == 1 ? 1 : b == 1 ? 2 :
            b == 0 ? (c == 2 ? 3 : c == 1 ? 4 : 9) : (c == 0 ? 5 : 9);
        if (conditional_otherwise(states[a], states[b], states[c],
                                  nested_checks) != nested_expected ||
            nested_checks[0] != 1 || nested_checks[1] != (a == 2) ||
            nested_checks[2] != (a == 2 && b != 1))
          return false;
        for (int d = 0; d < 3; ++d) {
          int checks[4] = {};
          bool use_handler = a == 2 || (a == 0 && b == 2);
          int expected = a == 1 ? 10 : use_handler ? 40 + d :
                         b == 1 ? 20 : 30 + c;
          if (chained(states[a], states[b], states[c], states[d], checks) !=
                  expected ||
              checks[0] != 1 || checks[1] != (a == 0) ||
              checks[2] != (a == 0 && b == 0) ||
              checks[3] != use_handler)
            return false;
        }
      }
  return true;
}
constexpr trine chain_states[] = {false, true, other};
static_assert(chains(chain_states));
static_assert([] {
  int checks[3] = {};
  return conditional_otherwise(other, true, other, checks) == 2 &&
         conditional_otherwise(other, false, trine(true), checks) == 4 &&
         checks[0] == 2 && checks[1] == 2 && checks[2] == 1;
}());

int all_return(trine state) {
  when (state) {
    true: { return 1; },
    other: { return -1; },
    false: { return 0; }
  }
}

#ifdef CXXX_RUNTIME_TEST
int main() {
  volatile trine states[] = {false, true, other};
  int sums[] = {290, 145, 76};
  for (int i = 0; i < 3; ++i) {
    trine state = states[i];
    int expected = i == 2 ? -1 : i;
    if (!check(state, expected) || all_return(state) != expected ||
        subsets(state) != sums[i])
      return 1;
  }
  volatile bool boolean_states[] = {false, true};
  for (bool state : boolean_states)
    if (!boolean_cases(state))
      return 3;
  return lifetime_and_nesting() && control_flow() && chains(states) ? 0 : 2;
}
#endif
