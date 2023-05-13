// RUN: %clang_cc1 -fsyntax-only -verify -std=c++2b -Wplaceholder-variable-extension %s

struct S {
int a;
};

void call(int);
void test_param(int _) {}
void test_params(int _, int _) { // expected-warning {{placeholder variables are a Clang extension}} \
                                 // expected-note 4 {{placeholder declared here}}
    call(_); // expected-error {{referring to placeholder '_' is not allowed}}
    {
        int _;
    }
    int _;   // expected-warning {{placeholder variables are a Clang extension}} \
             // expected-note {{placeholder declared here}}
    _,_; // expected-error {{referring to placeholder '_' is not allowed}}
}

void static_fun() {
    static int _; // expected-note {{previous definition is here}} \
                  // expected-note {{candidate}}
    static int _; // expected-error {{redefinition of '_'}}
    int _;        // expected-warning {{placeholder variables are a Clang extension}} \
                  // expected-note {{candidate}}
    _++; // expected-error{{reference to '_' is ambiguous}}
}

void bindings() {
    int arr[4] = {0, 1, 2, 3};
    auto [_, _, _, _] = arr; // expected-warning 3{{placeholder variables are a Clang extension}} \
                             // expected-note 4{{placeholder declared here}}
    _ == 42; // expected-error {{referring to placeholder '_' is not allowed}}
    {
        auto [_, a, b, c] = arr;
        auto [_, _, _, _] = arr; // expected-warning 4{{placeholder variables are a Clang extension}}
    }
}

void lambda() {
    (void)[_ = 0, _ = 1] { // expected-warning {{placeholder variables are a Clang extension}} \
                           // expected-note 4{{placeholder declared here}} \\
                           // expected-warning 2{{placeholder variable has no side effect}}
        (void)_++; // expected-error {{referring to placeholder '_' is not allowed}}
    };
}

namespace global_var {
    int _; // expected-note {{previous definition is here}}
    int _; // expected-error {{redefinition of '_'}}
}

namespace global_fun {
void _();
void _();

void _() {} // expected-note {{previous definition is here}}
void _() {} // expected-error {{redefinition of '_'}}
void _(int){};
}

void extern_test() {
    extern int _;
    extern int _; // expected-note {{candidate}}
    int _; //expected-note {{candidate}}
    _++; // expected-error {{reference to '_' is ambiguous}}
}
