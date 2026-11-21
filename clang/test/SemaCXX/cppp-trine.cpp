// RUN: %clang_cc1 -x c+++ -fsyntax-only -verify %s
// RUN: %clang_cc1 -x c+++ -std=c++11 -fsyntax-only -verify %s
// RUN: %clang_cc1 -x c+++ -fexperimental-new-constant-interpreter -fsyntax-only -verify %s
// RUN: not %clang_cc1 -x cppp -fsyntax-only %s 2>&1 | FileCheck %s --check-prefix=CC1-ALIAS

// CC1-ALIAS: error: invalid value 'cppp' in '-x cppp'

static_assert(__is_same(decltype(true), bool), "");
static_assert(__is_same(decltype(false), bool), "");
static_assert(__is_same(Trine, trine), "");
static_assert(!__is_same(trine, bool), "");
static_assert(!__is_same(trine, signed _BitInt(2)), "");
static_assert(__is_same(decltype(other), trine), "");
static_assert(__is_fundamental(trine), "");
static_assert(__is_scalar(trine), "");
static_assert(__is_integral(trine), "");
static_assert(__is_signed(trine), "");
static_assert(!__is_class(trine), "");
static_assert(__is_trivially_copyable(trine), "");
static_assert(!__is_identifier(trine) && !__is_identifier(Trine) &&
              !__is_identifier(other) && !__is_identifier(otherwise), "");

constexpr trine f = false;
constexpr trine t = true;
constexpr trine o = other;
static_assert(static_cast<int>(f) == 0, "");
static_assert(static_cast<int>(t) == 1, "");
static_assert(static_cast<int>(o) == -1, "");
static_assert(trine{} == false, "");
static_assert(f == false && false == f, "");
static_assert(t == true && true == t, "");
static_assert(o == other && other == o, "");
static_assert(f != t && t != o && o != f, "");
static_assert(f < t && f <= t && t > f && t >= f, "");
static_assert(f == 0 && 1 == t && t < 2 && -1 < f, "");
static_assert(!(o < false) && !(o >= false) && o != -1, "");

int overload(bool);
char overload(trine);
static_assert(sizeof(overload(true)) == sizeof(int), "");
static_assert(sizeof(overload(other)) == sizeof(char), "");

template <typename T> constexpr T copy(T v) { return v; }
static_assert(copy(other) == other, "");
static_assert(__is_same(decltype(true ? other : false), trine), "");
static_assert((false ? other : true) == true, "");

template <trine V> constexpr int signed_value() {
  return static_cast<int>(V);
}
static_assert(signed_value<false>() == 0, "");
static_assert(signed_value<true>() == 1, "");
static_assert(signed_value<other>() == -1, "");

constexpr trine values[] = {false, true, other};
static_assert(values[0] == false && values[1] == true && values[2] == other, "");
constexpr const trine &reference = other;
static_assert(reference == other, "");

struct Flags {
  trine first : 2;
  trine second : 2;
  trine third : 2;
};
constexpr Flags flags = {false, true, other};
static_assert(flags.first == false && flags.second == true &&
              flags.third == other, "");
static_assert(static_cast<int>(flags.third) == -1, "");

void assignments(bool b) {
  trine value = b;
  value = false;
  value = true;
  value = other;
  Trine alias = value;
  value = alias;
}

void invalid() {
  trine integer = 2; // expected-error {{cannot initialize}}
  trine negative = -2; // expected-error {{cannot initialize}}
  trine number = -1; // expected-error {{cannot initialize}}
  trine pointer = nullptr; // expected-error {{cannot initialize}}
  bool b = other; // expected-error {{cannot initialize}}
  while (other) {} // expected-error {{not contextually convertible to 'bool'}}
  int n = other; // expected-error {{cannot initialize}}
  auto sum = other + true; // expected-error {{invalid operands}}
  auto floating = other < 1.0; // expected-error {{invalid operands}}
  auto pointer_comparison = other == nullptr; // expected-error {{invalid operands}}
#if __cplusplus >= 202002L
  auto ordinary = true <=> 1; // expected-error {{invalid operands}}
#endif
  auto negation = !other; // expected-error {{invalid argument type 'trine'}}
  trine value = other;
  ++value; // expected-error {{invalid argument type 'trine'}}
  value += true; // expected-error {{invalid operands}}
  value = 2; // expected-error {{incompatible type}}
  (void)static_cast<trine>(2); // expected-error {{from 'int' to 'trine' is not allowed}}
  (void)static_cast<trine>(-2); // expected-error {{from 'int' to 'trine' is not allowed}}
  (void)static_cast<bool>(other); // expected-error {{from 'trine' to 'bool' is not allowed}}
  (void)static_cast<float>(other); // expected-error {{from 'trine' to 'float' is not allowed}}
  (void)(trine)2; // expected-error {{from 'int' to 'trine' is not allowed}}
}

struct TooNarrow {
  trine state : 1; // expected-error {{a trine bit-field requires at least 2 bits}}
};

unsigned trine invalid_unsigned; // expected-error {{'trine' cannot be signed or unsigned}}
