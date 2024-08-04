// RUN: %clang_cc1 -std=c++2b -verify -fsyntax-only %s


class A memberwise_trivially_relocatable {};
class E final memberwise_trivially_relocatable {};
class G memberwise_trivially_relocatable final{};
class I memberwise_trivially_relocatable memberwise_trivially_relocatable final {}; // expected-error {{class already marked 'memberwise_trivially_relocatable'}}
class memberwise_trivially_relocatable memberwise_trivially_relocatable {};
