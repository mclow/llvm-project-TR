// RUN: %clang_cc1 -std=c++2c -verify %s
// expected-no-diagnostics

template <class T> concept A = true;
template <class T> concept C = A<T> && true;

template <class T> requires (A<T>)
constexpr int f(T) { return 0; };
template <class... T> requires (C<T> && ...)
constexpr int f(T...) { return 1; };

static_assert(f(0) == 0);

template <class... T> requires (A<T> && ...)
constexpr int g(T...) { return 0; };
template <class... T> requires (C<T> && ...)
constexpr int g(T...) { return 1; };

static_assert(g(0) == 1);

template <class... T> requires (A<T> && ...)
constexpr int h(T...) { return 0; };
template <class... T> requires (C<T> || ...)
constexpr int h(T...) { return 1; };

static_assert(h(0) == 1);

template <class... T> requires (A<T> || ...)
constexpr int i(T...) { return 0; };
template <class... T> requires (C<T> && ...)
constexpr int i(T...) { return 1; };

static_assert(i(0) == 1);


template <class... T> requires (A<T> || ... || true)
constexpr int j(T...) { return 0; };
template <class... T> requires (C<T> && ... && true)
constexpr int j(T...) { return 1; };

static_assert(j(0) == 1);
