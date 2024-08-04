// RUN: %clang_cc1 -std=c++2c -verify %s

class Trivial {};
struct NonRelocatable {
    ~NonRelocatable();
};
static NonRelocatable NonRelocatable_g;

class A memberwise_trivially_relocatable {};
class B memberwise_trivially_relocatable : Trivial{};
class C memberwise_trivially_relocatable {
    int a;
    void* b;
    int c[3];
    Trivial d[3];
    NonRelocatable& e = NonRelocatable_g;
};
class D memberwise_trivially_relocatable : Trivial {};
class E memberwise_trivially_relocatable : virtual Trivial {};
// expected-error@-1 {{invalid 'trivially_relocatable' specifier on non trivially-relocatable class 'E'}}
// expected-note@-2  {{because it has a virtual base class 'Trivial'}}

class F memberwise_trivially_relocatable : NonRelocatable {};
// expected-error@-1 {{invalid 'trivially_relocatable' specifier on non trivially-relocatable class 'F'}}
// expected-note@-2  {{because it inherits from a non trivially-relocatable class 'NonRelocatable'}}

class I memberwise_trivially_relocatable {
    NonRelocatable a;
    NonRelocatable b[1];
    const NonRelocatable c;
    const NonRelocatable d[1];
};

class J memberwise_trivially_relocatable:  virtual Trivial, NonRelocatable {
// expected-error@-1 {{invalid 'trivially_relocatable' specifier on non trivially-relocatable class 'J'}}
// expected-note@-2 {{because it has a virtual base class 'Trivial'}}
// expected-note@-3 {{because it inherits from a non trivially-relocatable class 'NonRelocatable'}}
    NonRelocatable a;
};

class G memberwise_trivially_relocatable {
    G(G&&);
};

class H memberwise_trivially_relocatable {
    ~H();
};

struct Incomplete; // expected-note {{forward declaration of 'Incomplete'}}
static_assert(__is_cpp_trivially_relocatable(Incomplete));  // expected-error {{incomplete type 'Incomplete' used in type trait expression}}
static_assert(__is_cpp_trivially_relocatable(Trivial));
static_assert(__is_cpp_trivially_relocatable(G));
static_assert(__is_cpp_trivially_relocatable(H));
static_assert(__is_cpp_trivially_relocatable(int));
static_assert(__is_cpp_trivially_relocatable(void*));
static_assert(!__is_cpp_trivially_relocatable(int&));
static_assert(!__is_cpp_trivially_relocatable(Trivial&));
static_assert(__is_cpp_trivially_relocatable(const Trivial));
static_assert(__is_cpp_trivially_relocatable(Trivial[1]));

struct UserDtr {
    ~UserDtr();
};

struct DefaultedDtr {
    ~DefaultedDtr() = default;
};
struct UserMoveWithDefaultCopy {
    UserMoveWithDefaultCopy(UserMoveWithDefaultCopy&&);
    UserMoveWithDefaultCopy(const UserMoveWithDefaultCopy&) = default;
};

struct UserMove{
    UserMove(UserMove&&);
};

struct UserMoveDefault{
    UserMoveDefault(UserMoveDefault&&) = default;
};

struct UserCopy{
    UserCopy(const UserCopy&);
};

struct UserCopyDefault{
    UserCopyDefault(const UserCopyDefault&) = default;
};


struct UserDeletedMove{
    UserDeletedMove(UserDeletedMove&) = delete;
    UserDeletedMove(const UserDeletedMove&) = default;
};

static_assert(!__is_cpp_trivially_relocatable(UserDtr));
static_assert(__is_cpp_trivially_relocatable(DefaultedDtr));
static_assert(!__is_cpp_trivially_relocatable(UserMoveWithDefaultCopy));
static_assert(!__is_cpp_trivially_relocatable(UserMove));
static_assert(!__is_cpp_trivially_relocatable(UserCopy));
static_assert(__is_cpp_trivially_relocatable(UserMoveDefault));
static_assert(__is_cpp_trivially_relocatable(UserCopyDefault));
static_assert(__is_cpp_trivially_relocatable(UserDeletedMove));

template <typename T>
class TestDependentErrors memberwise_trivially_relocatable : T {};
// expected-error@-1 {{invalid 'trivially_relocatable' specifier on non trivially-relocatable class 'TestDependentErrors<NonRelocatable>'}}
// expected-note@-2 {{because it inherits from a non trivially-relocatable class 'NonRelocatable'}}
TestDependentErrors<Trivial> Ok;
TestDependentErrors<NonRelocatable> Err; // expected-note {{in instantiation of template class}}
