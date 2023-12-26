// RUN: %clang -cc1 -fsyntax-only -verify -std=c++2c %s
// expected-no-diagnostics

template <typename T>
struct Wrapper;

template <typename... T>
struct Eat {
    static_assert(__is_same_as(T...[0], Wrapper<Wrapper<int>>));
    static_assert(__is_same_as(T...[1], Wrapper<Wrapper<double>>));
};

template <typename... T>
struct Test {
    using ...a =  Wrapper<T>;
    Eat<Wrapper<a>...> t;
};

Test<int, double> t;
