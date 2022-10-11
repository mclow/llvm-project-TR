// RUN: %clang_cc1 -std=c++2b -verify -fsyntax-only %s
//expected-no-diagnostics

template <typename...>
struct typelist{};

template <typename... T>
requires requires(T...[0]) { {T...[0](0)}; }
struct S : T...[1] {
    [[no_unique_address]] T...[1] base = {};
    using foo = T...[1];
    S()
    : T...[1]()
    {}
    typelist<T...[0]> a;
    const T...[0] f(T...[0] && p) noexcept((T...[0])0) {
        T...[0] (*test)(const volatile T...[0]**);
        thread_local T...[0] d;
        [[maybe_unused]] T...[0] a = p;
        auto ptr = new T...[0](0);
        (*ptr).~T...[0]();
        return T...[0](0);
        typename T...[1]::foo b = 0;
        T...[1]::i = 0;
        return (T...[0])(a);
        new T...[0];
        [[maybe_unused]] auto l = []<T...[0]>(T...[0][1]) -> T...[0]{return{};};
        [[maybe_unused]] auto _ = l.template operator()<T...[0]{}>({0});
    }
    operator T...[0]() const{}
};

struct base {
    using foo = int;
    static inline int i = 42;
};

int main() {
    S<int, base>().f(0);
}
