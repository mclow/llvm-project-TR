// RUN: %clang_cc1 -std=c++2b -verify -fsyntax-only %s


class A trivially_relocatable {};
class B trivially_relocatable() {}; // expected-error {{expected expression}}
class C trivially_relocatable(true) {};
class D trivially_relocatable true {}; // expected-error {{variable has incomplete type 'class D'}} \
                                       // expected-error {{expected ';' after top level declarator}} \
                                       // expected-note  {{forward declaration of 'D'}}
class E final trivially_relocatable {};
class F final trivially_relocatable(true) {};
class G trivially_relocatable final{};
class H trivially_relocatable(true) final {};
class I trivially_relocatable trivially_relocatable(true) final {}; // expected-error {{class already marked 'trivially_relocatable'}}
class trivially_relocatable trivially_relocatable {};
