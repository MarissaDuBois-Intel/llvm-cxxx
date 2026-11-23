// RUN: %clang_cc1 -x c++ -std=c++20 -fsyntax-only -verify %s
// RUN: %clang_cc1 -x c+++ -std=c++20 -fsyntax-only -verify %s
// RUN: %clang_cc1 -triple x86_64-apple-darwin -x objective-c++ -std=c++20 -fsyntax-only -verify %s
// expected-no-diagnostics

template <class T> struct Box { using type = T; };
Box<Box<int>> nested;
static_assert((8 >> 2) == 2);
int identity, constant, swap, cycle, collapse, chain, feed;

#ifdef __OBJC__
@interface NSNumber
+ (NSNumber *)numberWithInt:(int)value;
@end
id boxed = @(1);
#endif
