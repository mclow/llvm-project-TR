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
   using a = S<Ts...>::...types; // expected-error {{declaration type contains unexpanded parameter pack 'types'}}
   using b = S<Ts...>::...types::a; // expected-error {{declaration type contains unexpanded parameter pack 'types'}}
   using ...c = S<Ts...>::types; // expected-error {{'typename S<Ts...>::types' does not contain an unexpanded pack}}
   using ...d = S<Ts...>::types::a; // expected-error {{'typename S<Ts...>::types::a' does not contain an unexpanded pack}}
   S<Ts...>::...types e; // expected-error {{data member type contains unexpanded parameter pack 'types'}}
   S<Ts...>::...types::a f; // expected-error {{data member type contains unexpanded parameter pack 'types'}}
};

template <typename... Ts>
struct ok {
    using ...a = S<Ts...>::...types;
    using ...b = S<Ts...>::...types::a;
};



namespace DependentNNS {

template <typename...Ts>
struct S {
    using ... A = Ts;
};

template <typename T>
struct Inner {
    using type = T;
};

template <typename...Ts>
struct AAAA {
};

template <typename... T>
void f() {
    AAAA<typename S<T...>::...A::type...> t;
}

int test() {
    f<Inner<int>, Inner<long>, Inner<bool>, Inner<float>>();
}

}
