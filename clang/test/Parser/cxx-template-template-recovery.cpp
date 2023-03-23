// RUN: %clang_cc1 -std=c++20 -verify -fsyntax-only %s

namespace a {
  template <typename T>
  concept C1 = true; //expected-note {{here}}

  template <typename T>
  auto V1 = true; //expected-note {{here}}

  namespace b {
    template <typename T>
    concept C2 = true; //expected-note {{here}}

    template <typename T>
    auto V2 = true; //expected-note {{here}}
  }
}

template <typename T>
concept C3 = true; //expected-note {{here}}
template <typename T>
auto V3 = true; //expected-note {{here}}

template <template <typename T> typename C>
constexpr bool test = true;

static_assert(test<a::C1>); //expected-error {{template argument does not refer to a class or alias template, or template template parameter}}
static_assert(test<a::b::C2>); //expected-error {{template argument does not refer to a class or alias template, or template template parameter}}
static_assert(test<C3>); //expected-error {{template argument does not refer to a class or alias template, or template template parameter}}

static_assert(test<a::V1>); //expected-error {{template argument does not refer to a class or alias template, or template template parameter}}
static_assert(test<a::b::V2>); //expected-error {{template argument does not refer to a class or alias template, or template template parameter}}
static_assert(test<V3>); //expected-error {{template argument does not refer to a class or alias template, or template template parameter}}


void f() {
    C3 t1 = 0;  // expected-error{{expected 'auto' or 'decltype(auto)' after concept name}}
    a::C1 t2 = 0; // expected-error{{expected 'auto' or 'decltype(auto)' after concept name}}
    a::b::C2 t3 = 0; // expected-error{{expected 'auto' or 'decltype(auto)' after concept name}}
}
