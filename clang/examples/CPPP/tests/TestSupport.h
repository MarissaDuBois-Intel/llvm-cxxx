// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef CXXX_TEST_SUPPORT_H
#define CXXX_TEST_SUPPORT_H

namespace testing {
class TestInfo;
}

// Keep Google Test's headers (which use reserved C+++ words) in ordinary C++.
namespace cxxx_test {
testing::TestInfo *register_test(const char *suite, const char *name,
                                 const char *file, int line, void (*body)());
void expect_true(bool value, const char *expression, const char *file,
                 int line);
void expect_equal(long long expected, long long actual,
                  const char *expected_expression,
                  const char *actual_expression, const char *file, int line);
} // namespace cxxx_test

// Match Google Test's naming so Visual Studio can locate each body in the PDB.
#define CXXX_TEST(Suite, Name)                                                 \
  class Suite##_##Name##_Test {                                                \
  public:                                                                      \
    static void TestBody();                                                    \
  };                                                                           \
  [[maybe_unused]] static auto *Suite##_##Name##_registration =                \
      cxxx_test::register_test(#Suite, #Name, __FILE__, __LINE__,              \
                               &Suite##_##Name##_Test::TestBody);              \
  void Suite##_##Name##_Test::TestBody()

#define CXXX_EXPECT_TRUE(Expression)                                           \
  cxxx_test::expect_true((Expression), #Expression, __FILE__, __LINE__)

#define CXXX_EXPECT_EQ(Expected, Actual)                                       \
  cxxx_test::expect_equal((Expected), (Actual), #Expected, #Actual, __FILE__,  \
                          __LINE__)

#endif
