// RUN: %clang_cc1 -std=c++2b -fsyntax-only -verify %s
// expected-no-diagnostics

namespace types {

template <typename...First, typename Last>
struct A;

template <typename First, typename... Middle, typename Last>
struct B;

}


namespace multi_level {

template<typename...T>
struct S {
template<typename...U>
void g(U &&...u, T &&...t) {}
};

void test() {
   S<>().g(0, 1);
   S<int>().g(0, 1);
   S<int, int>().g(0, 1);
}

}


namespace test_explicit{
void f(auto a, auto... x, auto b, auto c) {}
void explicit_test()
{
    f<int>(1, 2, 3);
    f<int>(1, 2, 3, 4);
    f<int, int>(1, 2, 3);
    f<int, int>(1, 2, 3, 4);
    f<int, int>(1, 2, 3, 4, 5);
    f<int, int, int, int>(1, 2, 3, 4);
    f<int, double, double>(1, 2, 3);
    f<int, double, double>(1, 2, 3, 4, 5);
}

}