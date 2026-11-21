// RUN: %clang_cc1 -x c+++ -std=c++20 -fsyntax-only -verify %s
// RUN: %clang_cc1 -x c+++ -fexperimental-new-constant-interpreter -fsyntax-only -verify %s
// RUN: %clang_cc1 -x c+++ -fno-trine-other-equal -fsyntax-only -verify %s
// RUN: %clang_cc1 -x c+++ -fno-trine-other-equal -fexperimental-new-constant-interpreter -fsyntax-only -verify %s
// RUN: %clang_cc1 -x c+++ -ftrine-other-ordered -fsyntax-only -verify %s
// RUN: %clang_cc1 -x c+++ -ftrine-other-ordered -fexperimental-new-constant-interpreter -fsyntax-only -verify %s
// RUN: %clang_cc1 -x c+++ -ast-print %s -o %t.cpp
// RUN: %clang_cc1 -x c+++ -fsyntax-only %t.cpp
// expected-no-diagnostics

struct Boolean {
  constexpr explicit operator bool() const { return true; }
};

template <class T> constexpr int choose(T state) {
  int selected = 9;
  if (state)
    selected = 1;
  else
    selected = 0;
  otherwise
    selected = -1;
  return selected;
}
static_assert(choose(trine(false)) == 0);
static_assert(choose(trine(true)) == 1);
static_assert(choose(other) == -1);
static_assert(choose(false) == 0 && choose(true) == 1);
static_assert(choose(-1) == 1 && choose(0) == 0 && choose(2) == 1);

template <class I>
constexpr trine counted(trine value, I &count) {
  count = count + 1;
  return value;
}

template <class I>
constexpr int chain(trine a, trine b, trine c, I &count) {
  int selected = 9;
  if (counted(a, count)) {
    selected = 1;
  } else if (counted(b, count)) {
    selected = 2;
  } else if (counted(c, count)) {
    selected = 3;
  } else {
    selected = 0;
  } otherwise {
    selected = -1;
  }
  return selected;
}

constexpr int expected(int a, int b, int c) {
  if (a != 0) return a == 1 ? 1 : -1;
  if (b != 0) return b == 1 ? 2 : -1;
  if (c != 0) return c == 1 ? 3 : -1;
  return 0;
}

constexpr bool matrix() {
  trine values[] = {false, true, other};
  for (int a = 0; a < 3; ++a)
    for (int b = 0; b < 3; ++b)
      for (int c = 0; c < 3; ++c) {
        int count = 0;
        if (chain(values[a], values[b], values[c], count) != expected(a,b,c) ||
            count != (a != 0 ? 1 : b != 0 ? 2 : 3))
          return false;
      }
  return true;
}
static_assert(matrix());

constexpr bool omitted_and_nested() {
  int value = 0, count = 0;
  if (other) value = 1;
  if (other) value = 2; else value = 3;
  if (other) value = 4; else if (counted(true, count)) value = 5;
  if (value != 0 || count != 0) return false;
  if (other) value = 6; otherwise value = 7;
  if (value != 7) return false;
  if (true) {
    if (other) value = 8;
  } otherwise { value = 9; }
  if (value != 7) return false;
  if (other) {
    value = 10;
  } otherwise {
    if (trine(true)) value = 11; otherwise value = 12;
  }
  if (value != 11) return false;
  if (false) value = 13; else if (other) value = 14; otherwise value = 15;
  return value == 15;
}
static_assert(omitted_and_nested());

struct Guard {
  int &active;
  constexpr Guard(int &active) : active(active) { ++active; }
  constexpr ~Guard() { --active; }
};

constexpr bool lifetimes(trine first, trine second) {
  int active = 0, seen = 0;
  if (Guard root(active); first) {
    seen = active;
  } else if (Guard later(active); second) {
    seen = active;
  } else {
    seen = active;
  } otherwise {
    seen = root.active;
  }
  int want = static_cast<int>(first) != 0 ? 1
             : static_cast<int>(second) == -1 ? 1 : 2;
  return active == 0 && seen == want;
}
static_assert(lifetimes(other, true));
static_assert(lifetimes(false, other));
static_assert(lifetimes(false, true));
static_assert(lifetimes(false, false));

constexpr int declaration() {
  int result = 0;
  if (trine state = other) {
    result = 1;
  } else {
    result = 2;
  } otherwise {
    result = static_cast<int>(state);
  }
  return result;
}
static_assert(declaration() == -1);

constexpr int returns(trine state) {
  if (state) return 1;
  else return 0;
  otherwise return -1;
}
static_assert(returns(other) == -1);

constexpr bool control_flow() {
  int result = 0;
  for (int i = 0; i < 4; ++i) {
    if (other) { return false; }
    else { return false; }
    otherwise {
      ++result;
      if (i == 1) break;
      continue;
    }
    return false;
  }
  return result == 2;
}
static_assert(control_flow());

static_assert(choose(Boolean{}) == 1);
static_assert([] {
  if constexpr (true) {
    if (other) return 1; otherwise return 2;
  }
}() == 2);

#ifdef CXXX_RUNTIME_TEST
int main() {
  volatile trine values[] = {false, true, other};
  for (int a = 0; a < 3; ++a)
    for (int b = 0; b < 3; ++b)
      for (int c = 0; c < 3; ++c) {
        volatile int count = 0;
        if (chain(values[a], values[b], values[c], count) != expected(a,b,c) ||
            count != (a != 0 ? 1 : b != 0 ? 2 : 3) ||
            !lifetimes(values[a], values[b]) ||
            returns(values[a]) != (a == 2 ? -1 : a))
          return 1;
      }
  return omitted_and_nested() && control_flow() ? 0 : 2;
}
#endif
