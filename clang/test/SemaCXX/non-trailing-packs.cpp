// RUN: %clang_cc1 -std=c++2b -fsyntax-only -verify %s

template <typename A, typename B>
constexpr bool is_same = false;

template <typename A>
constexpr bool is_same<A, A> = true;

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

namespace partial_specialization {
    template <typename... T, typename U>
    struct S {            // expected-note {{template is declared here}}
        using foo = U;
    };

    S<>::foo; //expected-error {{too few template arguments for class template 'S'}}
    S<int, int>::foo s = 42;

    static_assert(is_same<S<int>::foo, int>);
    static_assert(is_same<S<int, int>::foo, int>);

    template <typename... T>
    struct S<T..., double> {
        using foo = float;
    };
    static_assert(is_same<S<double, double, double>::foo, float>);
    static_assert(is_same<S<double>::foo, float>);
    static_assert(is_same<S<int, double>::foo, float>);
}