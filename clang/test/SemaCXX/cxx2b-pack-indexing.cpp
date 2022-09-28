// RUN: %clang_cc1 -std=c++2b -verify %s

void not_pack() {
    int i = 0;
    i...[0]; // expected-error {{i does not refer to the name of a parameter pack}}
}

namespace invalid_indexes {

template <int idx>
int f(auto... p) {
    return p...[idx]; //expected-error 3{{is not a valid index for pack 'p' of size}}

}

void test() {
    f<0>();   // expected-note{{here}}
    f<1>(0);  // expected-note{{here}}
    f<-1>(0); // expected-note{{here}}
}

int non_constant_index(); // expected-note {{declared here}}
void invalid_indexes(auto... p) {
    p...[non_constant_index()]; // expected-error {{array size is not a constant expression}}\
                                // expected-note {{cannot be used in a constant expression}}

    const char* no_index = "";
    p...[no_index]; // expected-error {{value of type 'const char *' is not implicitly convertible to 'unsigned long'}}
}

}

template <typename T, typename U>
constexpr bool is_same = false;

template <typename T>
constexpr bool is_same<T, T> = true;

template <typename T>
constexpr bool f(auto&&... p) {
    return is_same<T, decltype(p...[0])>;
}

void g() {
    int a = 0;
    const int b = 0;
    static_assert(f<int&&>(0));
    static_assert(f<int&>(a));
    static_assert(f<const int&>(b));
}

template <auto... p>
struct check_ice {
    enum e {
        x = p...[0]
    };
};

static_assert(check_ice<42>::x == 42);

struct S{};
template <auto... p>
constexpr auto constant_initializer = p...[0];
constexpr auto InitOk = constant_initializer<S{}>;

consteval int evaluate(auto... p) {
    return p...[0];
}
constexpr int x = evaluate(42, S{});
