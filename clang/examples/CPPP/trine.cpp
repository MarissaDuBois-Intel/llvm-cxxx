// -*- mode: c+++ -*-
#include <compare>
#include <stdio.h>

static_assert(__is_fundamental(trine), "C+++ requires the native trine type");

const char *boolean(bool value) { return value ? "true" : "false"; }

template <class L, class R>
void show_comparison(const char *label, L lhs, R rhs) {
  std::partial_ordering result = lhs <=> rhs;
  const char *ordering = result == 0  ? "equivalent"
                         : result < 0 ? "less"
                         : result > 0 ? "greater"
                                      : "unordered";
  printf("%s: == %s, != %s, < %s, <= %s, > %s, >= %s, <=> %s\n", label,
         boolean(lhs == rhs), boolean(lhs != rhs), boolean(lhs < rhs),
         boolean(lhs <= rhs), boolean(lhs > rhs), boolean(lhs >= rhs),
         ordering);
}

trine checked_state(trine state, int &checks) {
  ++checks;
  return state;
}

bool show_branches() {
  puts("\nThree-way branching:");
  trine states[] = {false, true, other};
  for (trine state : states) {
    printf("state %d: ", static_cast<int>(state));
    if (state) {
      puts("if (true)");
    } else {
      puts("else (false)");
    }
    otherwise { puts("otherwise (other)"); }
  }

  puts("\nElse-if chains (later conditions are skipped after true or other):");
  trine inputs[][3] = {
      {false, true, other}, {false, other, true}, {false, false, false}};
  int expected[] = {2, -1, 0};
  for (int row = 0; row < 3; ++row) {
    int checks = 0, selected = 9;
    if (checked_state(inputs[row][0], checks)) {
      selected = 1;
    } else if (checked_state(inputs[row][1], checks)) {
      selected = 2;
    } else if (checked_state(inputs[row][2], checks)) {
      selected = 3;
    } else {
      selected = 0;
    }
    otherwise { selected = -1; }
    printf("chain %d: branch %d, conditions evaluated %d\n", row + 1, selected,
           checks);
    if (selected != expected[row] || checks != (row == 2 ? 3 : 2))
      return false;
  }

  int actions = 0;
  trine state = other;
  if (state)
    ++actions;
  if (state)
    ++actions;
  else
    ++actions;
  printf("Omitted otherwise: other executed %d bodies\n", actions);

  state = true;
  if (state == true)
    puts("A Boolean comparison needs no otherwise.");
  state = other;
  if (state == other)
    puts("state == other matched (self-equality enabled).");
  else
    puts("state == other did not match (self-equality disabled).");
  return actions == 0;
}

int main() {
  trine value = false;
  printf("false = %d\n", static_cast<int>(value));
  if (value != false || static_cast<int>(value) != 0)
    return 1;

  value = true;
  printf("true = %d\n", static_cast<int>(value));
  if (value != true || static_cast<int>(value) != 1)
    return 1;

  value = other;
  printf("other = %d\n", static_cast<int>(value));
  if (static_cast<int>(value) != -1)
    return 1;

  Trine copy = value;
  if (static_cast<int>(copy) == -1)
    puts("The third state survives assignment and copying.");
  else
    return 1;

  printf("other == other: %s\n", other == other ? "true" : "false");
  printf("other ordering: %s\n", other < false ? "ordered" : "unordered");

  trine f = false, t = true;
  show_comparison("false / true", f, t);
  show_comparison("other / true", other, true);
  show_comparison("other / other", other, other);
  show_comparison("true / integer 1", t, 1);
  show_comparison("true / integer 2", t, 2);
  show_comparison("other / integer -1", other, -1);
  show_comparison("integer -1 / other", -1, other);

  auto comparison = other <=> true;
  bool unordered = comparison != 0 && !(comparison < 0) && !(comparison > 0);
  printf("other <=> true is unordered: %s\n", boolean(unordered));
  printf("Explicit encoding inspection: int(other) == -1 is %s\n",
         boolean(static_cast<int>(other) == -1));

  if (!show_branches())
    return 1;

  puts("Press Enter to exit.");
  char c;
  scanf("%c", &c);

  return 0;
}
