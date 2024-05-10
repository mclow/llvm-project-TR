// RUN: %clang_cc1 -std=c++2c -verify %s

namespace Expressions {

class D trivially_relocatable("") {};
// expected-error@-1 {{conversion from 'const char[1]' to 'bool' is not allowed in a converted constant expression}};
bool f(); //expected-note {{declared here}}
class E trivially_relocatable(f()) {}; // expected-error{{trivially_relocatable specifier argument is not a constant expression}}\
                                       // expected-note {{non-constexpr function 'f' cannot be used in a constant expression}};
}

class Trivial {};
struct NonRelocatable {
    ~NonRelocatable();
};
static NonRelocatable NonRelocatable_g;

class A trivially_relocatable {};
class B trivially_relocatable : Trivial{};
class C trivially_relocatable {
    int a;
    void* b;
    int c[3];
    Trivial d[3];
    NonRelocatable& e = NonRelocatable_g;
};
class D trivially_relocatable : Trivial {};
class E trivially_relocatable : virtual Trivial {};
// expected-error@-1 {{invalid 'trivially_relocatable' specifier on non trivially-relocatable class 'E'}}
// expected-note@-2  {{because it has a virtual base class 'Trivial'}}

class F trivially_relocatable : NonRelocatable {};
// expected-error@-1 {{invalid 'trivially_relocatable' specifier on non trivially-relocatable class 'F'}}
// expected-note@-2  {{because it inherits from a non trivially-relocatable class 'NonRelocatable'}}

class I trivially_relocatable { // expected-error{{invalid 'trivially_relocatable' specifier on non trivially-relocatable class 'I'}}
    NonRelocatable a;          // expected-note {{because it has a non trivially-relocatable member 'a'}}
    NonRelocatable b[1];       // expected-note {{because it has a non trivially-relocatable member 'b'}}
    const NonRelocatable c;    // expected-note {{because it has a non trivially-relocatable member 'c'}}
    const NonRelocatable d[1]; // expected-note {{because it has a non trivially-relocatable member 'd'}}
};

class J trivially_relocatable(false) :  virtual Trivial, NonRelocatable {
    NonRelocatable a;
};

class G trivially_relocatable {
    G(G&&);
};

class H trivially_relocatable {
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

template <auto... Args>
class UnexpendedPack trivially_relocatable(Args) // expected-error{{expression contains unexpanded parameter pack 'Args'}}
{};

namespace Tpls {

template <auto... Args>
class UnexpendedPack trivially_relocatable(Args) // expected-error{{expression contains unexpanded parameter pack 'Args'}}
{};

template <auto... Args>
class TestDependentValue trivially_relocatable((Args && ...)) {};

static_assert(__is_cpp_trivially_relocatable(TestDependentValue<>));
static_assert(__is_cpp_trivially_relocatable(TestDependentValue<true>));

template <typename T>
class TestDependent
trivially_relocatable(__is_cpp_trivially_relocatable(T)) : T
{};

TestDependent<Trivial> A;
TestDependent<NonRelocatable> B;
static_assert(__is_cpp_trivially_relocatable(TestDependent<Trivial>));
static_assert(__is_cpp_trivially_relocatable(TestDependent<NonRelocatable>)); // expected-error{{static assertion failed due to requirement '__is_cpp_trivially_relocatable(Tpls::TestDependent<NonRelocatable>)'}}

template <typename T>
class TestDependentErrors trivially_relocatable : T {};
// expected-error@-1 {{invalid 'trivially_relocatable' specifier on non trivially-relocatable class 'TestDependentErrors<NonRelocatable>'}}
// expected-note@-2 {{because it inherits from a non trivially-relocatable class 'NonRelocatable'}}
TestDependentErrors<Trivial> Ok;
TestDependentErrors<NonRelocatable> Err; // expected-note {{in instantiation of template class}}
}


namespace std {

template <class T>
struct swap_uses_value_representation {
    static constexpr bool value = __can_swap_using_value_representation(T);
};


template <class T>
constexpr bool swap_uses_value_representation_v = swap_uses_value_representation<T>::value;

}

class S1 {};
class S2 { int m1; };
class S3 { const int m1;  int m2; }; // expected-warning {{does not declare any constructor to initialize its non-modifiable members}} \
                                     // expected-note {{const member 'm1' will never be initialized}}
class S4 { const int &m1; int m2; }; // expected-warning {{does not declare any constructor to initialize its non-modifiable members}} \
                                     // expected-note {{reference member 'm1' will never be initialized}}
class S5 { int &m1; };               // expected-warning {{does not declare any constructor to initialize its non-modifiable members}} \
                                     // expected-note {{reference member 'm1' will never be initialized}}

class Bad { int m1; };

namespace std {
template <>
struct swap_uses_value_representation<Bad> {
    static constexpr bool value = false;
};

}


using std::swap_uses_value_representation_v;

static_assert(swap_uses_value_representation_v<int>);
static_assert(!swap_uses_value_representation_v<int const>);
static_assert(!swap_uses_value_representation_v<int &>);
static_assert(!swap_uses_value_representation_v<int &&>);
static_assert( swap_uses_value_representation_v<S1>);
static_assert( swap_uses_value_representation_v<S2>);
static_assert(!swap_uses_value_representation_v<S3>);
static_assert(!swap_uses_value_representation_v<S4>);
static_assert(!swap_uses_value_representation_v<S5>);
static_assert(!swap_uses_value_representation_v<Bad>);

struct empty {};
struct Test {
    empty nothing [[no_unique_address]];
    int   data;
};

static_assert( swap_uses_value_representation_v<empty>);
static_assert( swap_uses_value_representation_v<Test>);
static_assert( sizeof(Test) == sizeof(int));
