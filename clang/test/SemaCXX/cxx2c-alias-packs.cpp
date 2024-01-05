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

template <typename... T>
struct S {
    using ...types = T;
};
template <typename... Ts>
struct e1 {
   using a = S<Ts...>::...types;
   using b = S<Ts...>::...types::a;
   using ...c = S<Ts...>::types;
   using ...d = S<Ts...>::types::a;
   S<Ts...>::...types e;
   S<Ts...>::...types::a f;
};

template <typename... Ts>
struct ok {
    using ...a = S<Ts...>::...types;
    using ...b = S<Ts...>::...types::a;
};

