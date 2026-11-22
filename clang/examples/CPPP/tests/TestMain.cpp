// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "TestSupport.h"
#include "gtest/gtest.h"

namespace {
class FunctionTest final : public testing::Test {
public:
  explicit FunctionTest(void (*body)()) : Body(body) {}
  void TestBody() override { Body(); }

private:
  void (*Body)();
};
} // namespace

testing::TestInfo *cxxx_test::register_test(const char *suite, const char *name,
                                            const char *file, int line,
                                            void (*body)()) {
  return testing::RegisterTest(
      suite, name, nullptr, nullptr, file, line,
      [body]() -> testing::Test * { return new FunctionTest(body); });
}

void cxxx_test::expect_true(bool value, const char *expression,
                            const char *file, int line) {
  if (!value)
    ADD_FAILURE_AT(file, line) << "Expected true: " << expression;
}

void cxxx_test::expect_equal(long long expected, long long actual,
                             const char *expected_expression,
                             const char *actual_expression, const char *file,
                             int line) {
  if (expected != actual)
    ADD_FAILURE_AT(file, line)
        << "Expected equality of " << expected_expression << " and "
        << actual_expression << "\nExpected: " << expected
        << "\nActual: " << actual;
}

int main(int argc, char **argv) {
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
