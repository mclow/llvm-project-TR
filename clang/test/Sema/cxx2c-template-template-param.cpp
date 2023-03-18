// RUN:  %clang_cc1 -std=c++2b -verify %s

namespace Errors {

template <template<typename T> auto>
struct S1;
template <template<auto T> auto>
struct S2;
template <template<typename T> concept>
struct S3;
template <template<auto T> concept>
struct S4;
int a;

template <typename T>
concept C = true; // expected-note 2{{template argument refers to a concept 'C', here}}
template <typename T>
auto Var = 0; // expected-note 2{{template argument refers to a variable template 'Var', here}}

S1<1> t1; // expected-error {{template argument for template template parameter must be a variable template}}
S1<a> t2; // expected-error {{template argument for template template parameter must be a variable template}}
S1<int> t3; // expected-error {{template argument for template template parameter must be a variable template}}
S1<C> t4; // expected-error {{template argument does not refer to a variable template, or template template parameter}}
S2<1> t5; // expected-error {{template argument for template template parameter must be a variable template}}
S2<a> t6; // expected-error {{template argument for template template parameter must be a variable template}}
S2<int> t7; // expected-error {{template argument for template template parameter must be a variable template}}
S2<C> t8; // expected-error {{template argument does not refer to a variable template, or template template parameter}}
S3<1> t9; // expected-error {{template argument for template template parameter must be a concept}}
S3<a> t10; // expected-error {{template argument for template template parameter must be a concept}}
S3<int> t11; // expected-error {{template argument for template template parameter must be a concept}}
S3<Var> t12; // expected-error {{template argument does not refer to a concept, or template template parameter}}
S4<1> t13; // expected-error {{template argument for template template parameter must be a concept}}
S4<a> t14; // expected-error {{template argument for template template parameter must be a concept}}
S4<int> t15; // expected-error {{template argument for template template parameter must be a concept}}
S4<Var> t16; // expected-error {{template argument does not refer to a concept, or template template parameter}}

}

template <template<typename T> auto V> // expected-note {{previous template template parameter is here}}
struct S1 {
    static_assert(V<int> == 42);
    static_assert(V<const int> == 84);
    static_assert(V<double> == 0);
};
template <template<auto T> auto V>  // expected-note {{previous template template parameter is here}}
struct S2 {
    static_assert(V<0> == 1);
    static_assert(V<1> == 0);
};
template <template<typename T> concept C > // expected-note {{previous template template parameter is here}}
struct S3 {
    static_assert(C<int>);
};
template <template<auto> concept C> // expected-note {{previous template template parameter is here}}
struct S4 {
    static_assert(C<0>);
};

template <typename T> // expected-note {{template parameter has a different kind in template argument}}
concept C = true;

template <auto I> // expected-note {{template parameter has a different kind in template argument}}
concept CI = true;

template <typename T> // expected-note {{template parameter has a different kind in template argument}}
constexpr auto Var = 42;
template <typename T>
constexpr auto Var<const T> = 84;
template <>
constexpr auto Var<double> = 0;

template <auto N> // expected-note {{template parameter has a different kind in template argument}}
constexpr auto Var2 = 0;
template <auto N>
requires (N%2 == 0)
constexpr auto Var2<N> = 1;

void test () {
    S1<Var2> sE; // expected-error {{template template argument has different template parameters than its corresponding template template parameter}}
    S2<Var>  sE; // expected-error {{template template argument has different template parameters than its corresponding template template parameter}}
    S1<Var> s1;
    S2<Var2> s2;
    S3<C> s3;
    S4<C> sE; // expected-error {{template template argument has different template parameters than its corresponding template template parameter}}
    S4<CI> s4;
    S3<CI> sE; // expected-error {{template template argument has different template parameters than its corresponding template template parameter}}

}
